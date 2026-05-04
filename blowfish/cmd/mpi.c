#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../algorithm/BLOWFISH.C"

int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char* output_filename = (argc > 1) ? argv[1] : NULL;

    const int TOTAL_ELEMENTS = 1000000; 
    uint32_t *data_l = NULL;
    uint32_t *data_r = NULL;

    if (rank == 0) {
        char key[] = "my_secret_key";
        InitializeBlowfish(key, 13);

        data_l = (uint32_t*)malloc(TOTAL_ELEMENTS * sizeof(uint32_t));
        data_r = (uint32_t*)malloc(TOTAL_ELEMENTS * sizeof(uint32_t));
        
        for(int i = 0; i < TOTAL_ELEMENTS; i++) {
            data_l[i] = (uint32_t)i; 
            data_r[i] = (uint32_t)(i * 2);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    MPI_Bcast(P, N + 2, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(S, 4 * 256, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    int chunk_size = TOTAL_ELEMENTS / size;
    uint32_t *local_l = (uint32_t*)malloc(chunk_size * sizeof(uint32_t));
    uint32_t *local_r = (uint32_t*)malloc(chunk_size * sizeof(uint32_t));

    MPI_Scatter(data_l, chunk_size, MPI_UINT32_T, local_l, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatter(data_r, chunk_size, MPI_UINT32_T, local_r, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk_size; i++) {
        Blowfish_encipher(&local_l[i], &local_r[i]);
    }

    MPI_Gather(local_l, chunk_size, MPI_UINT32_T, data_l, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Gather(local_r, chunk_size, MPI_UINT32_T, data_r, chunk_size, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    if (rank == 0) {
        double total_time = end_time - start_time;
        printf("Processes: %d, Time: %f s\n", size, total_time);
        printf("First block: L=%u, R=%u\n", data_l[0], data_r[0]);
        
        if (output_filename) {
            FILE *f = fopen(output_filename, "a");
            if (f) {
                fprintf(f, "%d,%f\n", size, total_time);
                fclose(f);
            }
        }

        free(data_l);
        free(data_r);
    }

    free(local_l);
    free(local_r);
    MPI_Finalize();
    return 0;
}