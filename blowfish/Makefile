CL = clang
MPICC = mpicc

CMD_DIR = cmd
BIN_DIR = bin
ALG_DIR = algorithm

OMP_FLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp

MPI_BIN = $(BIN_DIR)/blowfish_mpi
OMP_BIN = $(BIN_DIR)/blowfish_omp

$(shell mkdir -p $(BIN_DIR))

.PHONY: all clean mpi omp run_mpi run_omp

all: mpi omp

mpi: $(CMD_DIR)/mpi.c
	$(MPICC) $(CMD_DIR)/mpi.c -o $(MPI_BIN) -lm

omp: $(CMD_DIR)/open_mp.c
	$(CL) $(OMP_FLAGS) $(CMD_DIR)/open_mp.c -o $(OMP_BIN) -lm

run_mpi: mpi
	mpirun -np 4 ./$(MPI_BIN)

run_omp: omp
	export OMP_NUM_THREADS=10 && ./$(OMP_BIN)

clean:
	rm -rf $(BIN_DIR)/*
	
help:
	@echo "Usage:"
	@echo "  make mpi      - Build the MPI version"
	@echo "  make omp      - Build the OpenMP version"
	@echo "  make run_mpi  - Build and run MPI with default settings"
	@echo "  make run_omp  - Build and run OpenMP with default settings"
	@echo "  make clean    - Remove binary files"