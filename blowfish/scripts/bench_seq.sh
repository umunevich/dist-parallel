#!/bin/bash

DATA_FILE="results/seq.csv"
OUTPUT_TEST_FILE="test-files/output_seq_decrypted.txt"
ITERATIONS=3

mkdir -p results
mkdir -p test-files

echo "threads,time,data_size" > $DATA_FILE

clang cmd/sequential.c -o bin/blowfish_seq -lm

SIZES=("1m" "5m" "20m")

for size_label in "${SIZES[@]}"
do
    DATA_SIZE_LABEL=$(echo "${size_label}B" | tr '[:lower:]' '[:upper:]')
    INPUT_TEST_FILE="test-files/input_${size_label}.txt"

    if [ ! -f "$INPUT_TEST_FILE" ]; then
        mkfile $size_label $INPUT_TEST_FILE
    fi

    echo "Running Sequential for $DATA_SIZE_LABEL..."
    for i in $(seq 1 $ITERATIONS)
    do
        ./bin/blowfish_seq $DATA_FILE $INPUT_TEST_FILE $OUTPUT_TEST_FILE $DATA_SIZE_LABEL
    done
    
    rm -f $INPUT_TEST_FILE
done

rm -f $OUTPUT_TEST_FILE
echo "Sequential benchmark completed!"