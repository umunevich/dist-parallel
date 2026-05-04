#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstring>

// Гіпотетична структура/клас Blowfish
// В реальному проєкті використовуйте перевірену бібліотеку, наприклад OpenSSL або mbedTLS
void blowfish_encrypt(unsigned char* data, int length, const char* key) {
    // Спрощена ілюстрація: XOR з ключем (замініть на реальний Blowfish)
    for (int i = 0; i < length; ++i) {
        data[i] ^= key[i % strlen(key)];
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const char* key = "secret_key_123";
    const int total_data_size = 1024; // Має бути кратним 8 байтам для Blowfish
    int chunk_size = total_data_size / world_size;

    std::vector<unsigned char> full_data;
    std::vector<unsigned char> local_data(chunk_size);

    if (world_rank == 0) {
        // Ініціалізація даних (Master)
        full_data.resize(total_data_size);
        for (int i = 0; i < total_data_size; ++i) full_data[i] = (unsigned char)(i % 256);
        std::cout << "Master: Data initialized. Starting encryption on " << world_size << " nodes.\n";
    }

    // Розсилка частин даних всім процесам
    MPI_Scatter(full_data.data(), chunk_size, MPI_UNSIGNED_CHAR, 
                local_data.data(), chunk_size, MPI_UNSIGNED_CHAR, 
                0, MPI_COMM_WORLD);

    // Локальне шифрування на кожному вузлі
    blowfish_encrypt(local_data.data(), chunk_size, key);

    // Збір зашифрованих частин
    if (world_rank == 0) full_data.assign(total_data_size, 0);

    MPI_Gather(local_data.data(), chunk_size, MPI_UNSIGNED_CHAR, 
               full_data.data(), chunk_size, MPI_UNSIGNED_CHAR, 
               0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        std::cout << "Master: Encryption finished. Collected all chunks.\n";
    }

    MPI_Finalize();
    return 0;
}