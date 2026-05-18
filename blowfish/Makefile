CL = clang
MPICC = mpicc

CMD_DIR = cmd
BIN_DIR = bin
ALG_DIR = algorithm
TEST_DIR = test-files

DEFAULT_INPUT = $(TEST_DIR)/input.txt
DEFAULT_OUTPUT_MPI = $(TEST_DIR)/output_mpi_decrypted.txt
DEFAULT_OUTPUT_OMP = $(TEST_DIR)/output_omp_decrypted.txt
DEFAULT_OUTPUT_SEQ = $(TEST_DIR)/output_seq_decrypted.txt

OMP_FLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp

MPI_BIN = $(BIN_DIR)/blowfish_mpi
OMP_BIN = $(BIN_DIR)/blowfish_omp
SEQ_BIN = $(BIN_DIR)/blowfish_seq

$(shell mkdir -p $(BIN_DIR))

.PHONY: all clean mpi omp seq run_mpi run_omp run_seq help

all: mpi omp seq

mpi: $(CMD_DIR)/mpi.c
	$(MPICC) $(CMD_DIR)/mpi.c -o $(MPI_BIN) -lm

omp: $(CMD_DIR)/open_mp.c
	$(CL) $(OMP_FLAGS) $(CMD_DIR)/open_mp.c -o $(OMP_BIN) -lm

seq: $(CMD_DIR)/sequential.c
	$(CL) $(CMD_DIR)/sequential.c -o $(SEQ_BIN) -lm

run_mpi: mpi
	mpirun -np 4 ./$(MPI_BIN) none $(DEFAULT_INPUT) $(DEFAULT_OUTPUT_MPI)

run_omp: omp
	export OMP_NUM_THREADS=10 && ./$(OMP_BIN) none $(DEFAULT_INPUT) $(DEFAULT_OUTPUT_OMP)

run_seq: seq
	./$(SEQ_BIN) none $(DEFAULT_INPUT) $(DEFAULT_OUTPUT_SEQ)

clean:
	rm -rf $(BIN_DIR)/*
	rm -f $(TEST_DIR)/output_*_decrypted.txt
    
help:
	@echo "Usage:"
	@echo "  make mpi      - Build the MPI version"
	@echo "  make omp      - Build the OpenMP version"
	@echo "  make run_mpi  - Run MPI with 4 processes using $(DEFAULT_INPUT)"
	@echo "  make run_omp  - Run OpenMP with 10 threads using $(DEFAULT_INPUT)"
	@echo "  make clean    - Remove binary files and decrypted outputs"