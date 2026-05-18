#!/bin/bash

DATA_FILE="results/omp.csv"
OUTPUT_TEST_FILE="test-files/output_omp_decrypted.txt"
ITERATIONS=3

mkdir -p results
mkdir -p test-files

echo "threads,time,data_size" > $DATA_FILE

make omp

SIZES=("1m" "5m" "20m")

for size_label in "${SIZES[@]}"
do
    DATA_SIZE_LABEL=$(echo "${size_label}B" | tr '[:lower:]' '[:upper:]')
    INPUT_TEST_FILE="test-files/input_${size_label}.txt"

    echo "================================================"
    echo "  PREPARING DATA: $DATA_SIZE_LABEL"
    echo "================================================"

    if [ ! -f "$INPUT_TEST_FILE" ]; then
        echo "Generating $INPUT_TEST_FILE..."
        mkfile $size_label $INPUT_TEST_FILE
    fi

    for t in 1 2 4 8 10
    do
        echo "------------------------------------------------"
        echo "Running with $t threads on $DATA_SIZE_LABEL data..."
        echo "------------------------------------------------"
        
        export OMP_NUM_THREADS=$t

        for i in $(seq 1 $ITERATIONS)
        do
            echo "  Iteration $i/$ITERATIONS..."
            ./bin/blowfish_omp $DATA_FILE $INPUT_TEST_FILE $OUTPUT_TEST_FILE $DATA_SIZE_LABEL
        done
    done

    rm -f $INPUT_TEST_FILE
done

rm -f $OUTPUT_TEST_FILE

echo "================================================"
echo "Done! Full OpenMP multi-size benchmark completed."
echo "All data successfully compiled into $DATA_FILE"
echo "================================================"