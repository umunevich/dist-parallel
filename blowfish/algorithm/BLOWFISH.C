#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <ctype.h>

#include "BLOWFISH.H"

#define N               16
#define noErr            0
#define DATAERROR         -1
#define KEYBYTES         8
#define subkeyfilename   "algorithm/BLOWFISH.DAT"

uint32_t P[N + 2];
uint32_t S[4][256];
FILE*         SubkeyFile;

short opensubkeyfile(void)
{
   if((SubkeyFile = fopen(subkeyfilename,"rb")) == NULL) {
      return DATAERROR;
   }
   return noErr;
}

uint32_t F(uint32_t x)
{
   uint32_t a, b, c, d, y;

   d = (uint32_t)(x & 0x00FF);
   x >>= 8;
   c = (uint32_t)(x & 0x00FF);
   x >>= 8;
   b = (uint32_t)(x & 0x00FF);
   x >>= 8;
   a = (uint32_t)(x & 0x00FF);

   y = ((S[0][a] + S[1][b]) ^ S[2][c]) + S[3][d];

   return y;
}

void Blowfish_encipher(uint32_t *xl, uint32_t *xr)
{
   uint32_t  Xl = *xl;
   uint32_t  Xr = *xr;
   uint32_t  temp;
   short     i;

   for (i = 0; i < N; ++i) {
      Xl = Xl ^ P[i];
      Xr = F(Xl) ^ Xr;

      temp = Xl;
      Xl = Xr;
      Xr = temp;
   }

   temp = Xl;
   Xl = Xr;
   Xr = temp;

   Xr = Xr ^ P[N];
   Xl = Xl ^ P[N + 1];
  
   *xl = Xl;
   *xr = Xr;
}

void Blowfish_decipher(uint32_t *xl, uint32_t *xr)
{
   uint32_t  Xl = *xl;
   uint32_t  Xr = *xr;
   uint32_t  temp;
   short     i;

   for (i = N + 1; i > 1; --i) {
      Xl = Xl ^ P[i];
      Xr = F(Xl) ^ Xr;

      temp = Xl;
      Xl = Xr;
      Xr = temp;
   }

   temp = Xl;
   Xl = Xr;
   Xr = temp;

   Xr = Xr ^ P[1];
   Xl = Xl ^ P[0];

   *xl = Xl;
   *xr = Xr;
}

short InitializeBlowfish(char key[], short keybytes)
{
   short          i, j, k, error;
   uint32_t       data, datal, datar;
   size_t         numread;

   error = opensubkeyfile();
   if (error != noErr) {
      printf("Unable to open subkey initialization file\n");
      return error;
   }

   for (i = 0; i < N + 2; ++i) {
      numread = fread(&data, 4, 1, SubkeyFile);
      #ifdef little_endian
         data = ((data & 0xFF000000) >> 24) | ((data & 0x00FF0000) >> 8) |
                ((data & 0x0000FF00) << 8) | ((data & 0x000000FF) << 24);
      #endif
      if (numread != 1) return DATAERROR;
      P[i] = data;
   }

   for (i = 0; i < 4; ++i) {
      for (j = 0; j < 256; ++j) {
         numread = fread(&data, 4, 1, SubkeyFile);
         #ifdef little_endian
            data = ((data & 0xFF000000) >> 24) | ((data & 0x00FF0000) >> 8) |
                   ((data & 0x0000FF00) << 8) | ((data & 0x000000FF) << 24);
         #endif
         if (numread != 1) return DATAERROR;
         S[i][j] = data;
      }
   }
   fclose(SubkeyFile);

   j = 0;
   for (i = 0; i < N + 2; ++i) {
      data = 0x00000000;
      for (k = 0; k < 4; ++k) {
         data = (data << 8) | (uint8_t)key[j];
         j = (j + 1) % keybytes;
      }
      P[i] = P[i] ^ data;
   }

   datal = 0x00000000;
   datar = 0x00000000;

   for (i = 0; i < N + 2; i += 2) {
      Blowfish_encipher(&datal, &datar);
      P[i] = datal;
      P[i + 1] = datar;
   }

   for (i = 0; i < 4; ++i) {
      for (j = 0; j < 256; j += 2) {
         Blowfish_encipher(&datal, &datar);
         S[i][j] = datal;
         S[i][j + 1] = datar;
      }
   }

   return noErr;
}