#!/bin/bash

DATA_FILE="results/omp.csv"
echo "threads,time" > $DATA_FILE

make omp

echo "Starting OpenMP benchmarks..."

for t in 1 2 4 8 10
do
    echo "Running with $t threads..."
    export OMP_NUM_THREADS=$t
    ./bin/blowfish_omp $DATA_FILE
done

echo "Done! Data saved to $DATA_FILE"