#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../algorithm/BLOWFISH.C"
#include "../utils/file_utils.c"

int main(int argc, char** argv) {
    long actual_file_size = 0;
    int rank, size;
    double start_time, end_time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char* stats_filename = (argc > 1 && strcmp(argv[1], "none") != 0) ? argv[1] : NULL;
    const char* input_file = (argc > 2) ? argv[2] : "test-files/input.txt";
    const char* decrypted_file = (argc > 3) ? argv[3] : "test-files/output_decrypted.txt";

    int total_elements = 0;
    uint32_t *data_l = NULL;
    uint32_t *data_r = NULL;

    char key[] = "my_secret_key";
    InitializeBlowfish(key, 13);

    if (rank == 0) {
        total_elements = read_and_pack_file(input_file, size, &data_l, &data_r, &actual_file_size);
        if (total_elements < 0) {
            fprintf(stderr, "Error: Failed to open input file '%s'!\n", input_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        printf("Master: Successfully processed '%s'. Total blocks: %d\n", input_file, total_elements);
    }

    MPI_Bcast(&total_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&actual_file_size, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    int chunk_size = total_elements / size;
    uint32_t *local_l = (uint32_t*)malloc(chunk_size * sizeof(uint32_t));
    uint32_t *local_r = (uint32_t*)malloc(chunk_size * sizeof(uint32_t));

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    MPI_Scatter(data_l, chunk_size, MPI_UINT32_T, local_l, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatter(data_r, chunk_size, MPI_UINT32_T, local_r, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk_size; i++) {
        Blowfish_encipher(&local_l[i], &local_r[i]);
    }

    MPI_Gather(local_l, chunk_size, MPI_UINT32_T, data_l, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Gather(local_r, chunk_size, MPI_UINT32_T, data_r, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    MPI_Scatter(data_l, chunk_size, MPI_UINT32_T, local_l, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatter(data_r, chunk_size, MPI_UINT32_T, local_r, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk_size; i++) {
        Blowfish_decipher(&local_l[i], &local_r[i]);
    }

    uint32_t *decrypted_l = NULL;
    uint32_t *decrypted_r = NULL;
    if (rank == 0) {
        decrypted_l = (uint32_t*)malloc(total_elements * sizeof(uint32_t));
        decrypted_r = (uint32_t*)malloc(total_elements * sizeof(uint32_t));
    }

    MPI_Gather(local_l, chunk_size, MPI_UINT32_T, decrypted_l, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Gather(local_r, chunk_size, MPI_UINT32_T, decrypted_r, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    if (rank == 0) {
        double total_time = end_time - start_time;
        printf("Processes: %d, Time: %f s\n", size, total_time);
        
        if (unpack_and_write_file(decrypted_file, total_elements, decrypted_l, decrypted_r, actual_file_size) == 0) {
            printf("Master: Decrypted data successfully saved to '%s'\n", decrypted_file);
        }

        if (stats_filename) {
            FILE *f_stat = fopen(stats_filename, "a");
            if (f_stat) {
                char* data_size_label = (argc > 4) ? argv[4] : "unknown";
  
                fprintf(f_stat, "%d,%f,%s\n", size, total_time, data_size_label);
                fclose(f_stat);
            }
        }

        free(data_l); free(data_r);
        free(decrypted_l); free(decrypted_r);
    }

    free(local_l); free(local_r);
    MPI_Finalize();
    return 0;
}