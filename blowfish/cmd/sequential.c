#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "../algorithm/BLOWFISH.C"
#include "../utils/file_utils.c"

int main(int argc, char** argv) {
    long actual_file_size = 0;
    clock_t start_time, end_time;

    char* stats_filename = (argc > 1 && strcmp(argv[1], "none") != 0) ? argv[1] : NULL;
    const char* input_file = (argc > 2) ? argv[2] : "test-files/input.txt";
    const char* decrypted_file = (argc > 3) ? argv[3] : "test-files/output_decrypted.txt";

    uint32_t *data_l = NULL;
    uint32_t *data_r = NULL;

    char key[] = "my_secret_key";
    InitializeBlowfish(key, 13);

    int total_elements = read_and_pack_file(input_file, 1, &data_l, &data_r, &actual_file_size);
    if (total_elements < 0) {
        fprintf(stderr, "Error: Failed to open input file '%s'!\n", input_file);
        return 1;
    }
    printf("Sequential: Processed '%s'. Total blocks: %d\n", input_file, total_elements);

    start_time = clock();

    for (int i = 0; i < total_elements; i++) {
        Blowfish_encipher(&data_l[i], &data_r[i]);
    }

    for (int i = 0; i < total_elements; i++) {
        Blowfish_decipher(&data_l[i], &data_r[i]);
    }

    end_time = clock();
    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Sequential finished in %f seconds.\n", total_time);

    if (unpack_and_write_file(decrypted_file, total_elements, data_l, data_r, actual_file_size) == 0) {
        printf("Sequential: Decrypted data saved to '%s'\n", decrypted_file);
    }

    if (stats_filename) {
        FILE *f = fopen(stats_filename, "a");
        if (f) {
            char* data_size_label = (argc > 4) ? argv[4] : "unknown";
            fprintf(f, "1,%f,%s\n", total_time, data_size_label);
            fclose(f);
        }
    }

    free(data_l);
    free(data_r);
    return 0;
}