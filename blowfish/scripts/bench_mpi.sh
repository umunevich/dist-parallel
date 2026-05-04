#!/bin/bash

DATA_FILE="results/mpi.csv"

echo "processes,time" > $DATA_FILE

make mpi

echo "Starting benchmarks..."

for p in 1 2 4 8 10
do
    echo "Running with $p processes..."
    mpirun -np $p ./bin/blowfish_mpi $DATA_FILE
done

echo "Done! Data saved to $DATA_FILE"