# Blowfish Parallel Implementation (MPI vs OpenMP)

This project provides parallel implementations of the Blowfish encryption/decryption algorithm using two distinct methodologies: **MPI** (Message Passing Interface) for distributed memory and **OpenMP** for shared memory multiprocessors.

The application dynamically reads, pads, parallelizes, and decrypts real binary or text files.

## Requirements

- **Compiler**: `clang` (with OpenMP support)
- **MPI Environment**: Open MPI (`mpicc`, `mpirun`)
- **Libraries**: `libomp` (installed via Homebrew on macOS)

## Directory Structure

- `cmd/` — Source files for the executable benchmarks (`mpi.c`, `open_mp.c`).
- `algorithm/` — Core implementation of the Blowfish algorithm (`BLOWFISH.C`).
- `test-files/` — Directory containing input test assets and decrypted outputs.
- `bin/` — Target directory for compiled binaries (created automatically).

## CLI Arguments Structure

Both binaries accept identical positioning arguments when executed manually:

```bash
./binary_name [stats_csv_path | none] [input_file_path] [output_file_path]
```

1. `stats_csv_path` — Path to append execution time metrics (use `none` to skip writing).
2. `input_file_path` — Path to the file you want to encrypt.
3. `output_file_path` — Path where the final decrypted file will be saved.

## Compilation and Execution via Makefile

### 1. Build All Versions

To compile both the MPI and OpenMP implementations:

```bash
make all
```

### 2. MPI Implementation

To compile the MPI version:

```bash
make mpi
```

To run the default test suite (4 processes, processes `test-files/input.txt`):

```bash
make run_mpi
```

### 3. OpenMP Implementation

To compile the OpenMP version:

```bash
make omp
```

To run the default test suite (10 threads, processes `test-files/input.txt`):

```bash
make run_omp
```

### 4. Cleaning Up

To remove all generated binaries and clear test decryption results:

```bash
make clean
```

## Manual Execution Examples (Custom Files)

If you want to test your own custom files instead of the default `Makefile` shortcuts, use the following commands after compilation:

**Using MPI:**

```bash
mpirun -np 4 ./bin/blowfish_mpi none test-files/my_document.txt test-files/my_document_decrypted.txt
```

**Using OpenMP:**

```bash
export OMP_NUM_THREADS=8
./bin/blowfish_omp none test-files/my_document.txt test-files/my_document_decrypted.txt
```

## Verification

To verify that the parallel encryption and decryption routines worked seamlessly without corrupting any bytes, run a differential check:

```bash
diff test-files/input.txt test-files/output_mpi_decrypted.txt
```

If the command yields no output, the file was processed with 100% byte integrity.
