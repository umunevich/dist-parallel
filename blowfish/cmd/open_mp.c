#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include "../algorithm/BLOWFISH.C"

#define TOTAL_BLOCKS 1000000

int main(int argc, char** argv) {
    char* output_filename = (argc > 1) ? argv[1] : NULL;

    uint32_t *data_l = malloc(TOTAL_BLOCKS * sizeof(uint32_t));
    uint32_t *data_r = malloc(TOTAL_BLOCKS * sizeof(uint32_t));

    char key[] = "my_secret_key";
    InitializeBlowfish(key, 13);

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        data_l[i] = i;
        data_r[i] = i * 2;
    }

    int num_threads = omp_get_max_threads();
    printf("Starting encryption with %d threads...\n", num_threads);
    double start_time = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        // Кожен потік бере свій елемент масиву
        Blowfish_encipher(&data_l[i], &data_r[i]);
    }

    double end_time = omp_get_wtime();
    double total_time = end_time - start_time;
    
    printf("Encryption finished in %f seconds.\n", end_time - start_time);

    if (output_filename) {
        FILE *f = fopen(output_filename, "a");
        if (f) {
            fprintf(f, "%d,%f\n", num_threads, total_time);
            fclose(f);
        }
    }

    printf("First block result: L=%u, R=%u\n", data_l[0], data_r[0]);

    #pragma omp parallel for
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        Blowfish_decipher(&data_l[i], &data_r[i]);
    }

    printf("Decryption check (first block): L=%u, R=%u (Expected: 0, 0)\n", data_l[0], data_r[0]);

    free(data_l);
    free(data_r);
    return 0;
}