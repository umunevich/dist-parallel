#include <stdio.h>
#include <stdlib.h>
// #include <conio.h>
#include <time.h>

// Function for simple setting the matrix and vector elements
void DummyDataInitialization(double* pMatrix, double* pVector, int Size) {
    int i, j;

    for (i = 0; i < Size; i++) {
        pVector[i] = 1;
        for (j = 0; j < Size; j++) {
            pMatrix[i * Size + j] = i;
        }
    }
}

// Function for random setting the matrix and vector elements
void RandomDataInitialization(double* pMatrix, double* pVector, int Size) {
    int i, j;

    srand((unsigned)clock());

    for (i = 0; i < Size; i++) {
        pVector[i] = rand() / double(1000);
        for (j = 0; j < Size; j++) {
            pMatrix[i * Size + j] = rand() / double(1000);
        }
    }
}

// Function for memory allocation and data initialization
void ProcessInitialization(double*& pMatrix, double*& pVector,
                           double*& pResult, int& Size) {

    do {
        printf("\nEnter the size of the initial objects: ");
        scanf("%d", &Size);

        printf("\nChosen objects size = %d\n", Size);

        if (Size <= 0) {
            printf("\nSize of objects must be greater than 0!\n");
        }

    } while (Size <= 0);

    // Memory allocation
    pMatrix = new double[Size * Size];
    pVector = new double[Size];
    pResult = new double[Size];

    // Initialization
    DummyDataInitialization(pMatrix, pVector, Size);
}

// Function for formatted matrix output
void PrintMatrix(double* pMatrix, int RowCount, int ColCount) {
    int i, j;

    for (i = 0; i < RowCount; i++) {
        for (j = 0; j < ColCount; j++) {
            printf("%7.4f ", pMatrix[i * RowCount + j]);
        }
        printf("\n");
    }
}

// Function for formatted vector output
void PrintVector(double* pVector, int Size) {
    int i;

    for (i = 0; i < Size; i++) {
        printf("%7.4f ", pVector[i]);
    }
}

// Function for matrix-vector multiplication
void ResultCalculation(double* pMatrix, double* pVector,
                       double* pResult, int Size) {
    int i, j;

    for (i = 0; i < Size; i++) {
        pResult[i] = 0;

        for (j = 0; j < Size; j++) {
            pResult[i] += pMatrix[i * Size + j] * pVector[j];
        }
    }
}

// Function for computational process termination
void ProcessTermination(double* pMatrix, double* pVector, double* pResult) {
    delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
}

int main() {
    double* pMatrix;
    double* pVector;
    double* pResult;
    int Size;

    time_t start, finish;
    double duration;

    printf("Serial matrix-vector multiplication program\n");

    // Initialization
    ProcessInitialization(pMatrix, pVector, pResult, Size);

    // Output
    printf("Initial Matrix \n");
    PrintMatrix(pMatrix, Size, Size);

    printf("Initial Vector \n");
    PrintVector(pVector, Size);

    // Calculation
    start = clock();
    ResultCalculation(pMatrix, pVector, pResult, Size);
    finish = clock();

    duration = (finish - start) / double(CLOCKS_PER_SEC);

    // Result output
    printf("\nResult Vector:\n");
    PrintVector(pResult, Size);

    printf("\nTime of execution: %f\n", duration);

    // Cleanup
    ProcessTermination(pMatrix, pVector, pResult);

    getchar();
    return 0;
}