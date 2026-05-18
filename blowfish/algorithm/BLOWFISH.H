#include <stdint.h>

#define MAXKEYBYTES 56

#define little_endian 1         

short opensubkeyfile(void);
uint32_t F(uint32_t x);
void Blowfish_encipher(uint32_t *xl, uint32_t *xr);
void Blowfish_decipher(uint32_t *xl, uint32_t *xr);
short InitializeBlowfish(char key[], short keybytes);