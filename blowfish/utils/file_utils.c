#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int read_and_pack_file(const char* filename, int mpi_size, uint32_t** out_l, uint32_t** out_r, long* actual_file_size) {
    FILE *f_in = fopen(filename, "rb");
    if (!f_in) {
        return -1;
    }

    fseek(f_in, 0, SEEK_END);
    long file_size = ftell(f_in);
    fseek(f_in, 0, SEEK_SET);

    *actual_file_size = file_size;

    int num_blocks = (file_size + 7) / 8;
    while (num_blocks % mpi_size != 0) {
        num_blocks++; 
    }

    *out_l = (uint32_t*)calloc(num_blocks, sizeof(uint32_t));
    *out_r = (uint32_t*)calloc(num_blocks, sizeof(uint32_t));

    unsigned char *file_buffer = (unsigned char*)calloc(num_blocks * 8, 1);
    fread(file_buffer, 1, file_size, f_in);
    fclose(f_in);

    for (int i = 0; i < num_blocks; i++) {
        memcpy(&(*out_l)[i], &file_buffer[i * 8], 4);
        memcpy(&(*out_r)[i], &file_buffer[i * 8 + 4], 4);
    }
    
    free(file_buffer);
    return num_blocks; 
}

int unpack_and_write_file(const char* filename, int total_elements, uint32_t* data_l, uint32_t* data_r, long actual_file_size) {
    FILE *f_out = fopen(filename, "wb");
    if (!f_out) {
        return -1;
    }

    unsigned char *file_buffer = (unsigned char*)malloc(total_elements * 8);
    
    for (int i = 0; i < total_elements; i++) {
        memcpy(&file_buffer[i * 8], &data_l[i], 4);
        memcpy(&file_buffer[i * 8 + 4], &data_r[i], 4);
    }

    fwrite(file_buffer, 1, actual_file_size, f_out);

    fclose(f_out);
    free(file_buffer);
    return 0;
}