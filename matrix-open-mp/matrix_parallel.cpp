#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int ProcNum = 0;   // Number of processes
int ProcRank = 0;  // Rank of current process

// Dummy initialization
void DummyDataInitialization(double* pMatrix, double* pVector, int Size) {
    for (int i = 0; i < Size; i++) {
        pVector[i] = 1;
        for (int j = 0; j < Size; j++) {
            pMatrix[i * Size + j] = i;
        }
    }
}

// Random initialization
void RandomDataInitialization(double* pMatrix, double* pVector, int Size) {
    srand((unsigned)clock());

    for (int i = 0; i < Size; i++) {
        pVector[i] = rand() / (double)1000;
        for (int j = 0; j < Size; j++) {
            pMatrix[i * Size + j] = rand() / (double)1000;
        }
    }
}

// Initialization
void ProcessInitialization(double*& pMatrix, double*& pVector,
                           double*& pResult, double*& pProcRows,
                           double*& pProcResult,
                           int& Size, int& RowNum) {

    int RestRows;
    setvbuf(stdout, 0, _IONBF, 0);

    if (ProcRank == 0) {
        do {
            printf("\nEnter the size of the matrix and vector: ");
            scanf("%d", &Size);

            if (Size < ProcNum) {
                printf("Size must be >= number of processes!\n");
            }
        } while (Size < ProcNum);
    }

    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    RestRows = Size;
    for (int i = 0; i < ProcRank; i++) {
        RestRows -= RestRows / (ProcNum - i);
    }

    RowNum = RestRows / (ProcNum - ProcRank);

    pVector = new double[Size];
    pResult = new double[Size];
    pProcRows = new double[RowNum * Size];
    pProcResult = new double[RowNum];

    if (ProcRank == 0) {
        pMatrix = new double[Size * Size];
        RandomDataInitialization(pMatrix, pVector, Size);
    }
}

// Data distribution
void DataDistribution(double* pMatrix, double* pProcRows,
                      double* pVector, int Size, int RowNum) {

    int* pSendNum = new int[ProcNum];
    int* pSendInd = new int[ProcNum];

    MPI_Bcast(pVector, Size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int RestRows = Size;
    int rows = Size / ProcNum;

    pSendNum[0] = rows * Size;
    pSendInd[0] = 0;

    for (int i = 1; i < ProcNum; i++) {
        RestRows -= rows;
        rows = RestRows / (ProcNum - i);
        pSendNum[i] = rows * Size;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
    }

    MPI_Scatterv(pMatrix, pSendNum, pSendInd, MPI_DOUBLE,
                 pProcRows, pSendNum[ProcRank],
                 MPI_DOUBLE, 0, MPI_COMM_WORLD);

    delete[] pSendNum;
    delete[] pSendInd;
}

// Result gathering
void ResultReplication(double* pProcResult, double* pResult,
                       int Size, int RowNum) {

    int* pReceiveNum = new int[ProcNum];
    int* pReceiveInd = new int[ProcNum];

    int RestRows = Size;

    pReceiveInd[0] = 0;
    pReceiveNum[0] = Size / ProcNum;

    for (int i = 1; i < ProcNum; i++) {
        RestRows -= pReceiveNum[i - 1];
        pReceiveNum[i] = RestRows / (ProcNum - i);
        pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1];
    }

    MPI_Allgatherv(pProcResult, pReceiveNum[ProcRank], MPI_DOUBLE,
                   pResult, pReceiveNum, pReceiveInd,
                   MPI_DOUBLE, MPI_COMM_WORLD);

    delete[] pReceiveNum;
    delete[] pReceiveInd;
}

// Parallel calculation
void ParallelResultCalculation(double* pProcRows, double* pVector,
                               double* pProcResult,
                               int Size, int RowNum) {

    for (int i = 0; i < RowNum; i++) {
        pProcResult[i] = 0;
        for (int j = 0; j < Size; j++) {
            pProcResult[i] += pProcRows[i * Size + j] * pVector[j];
        }
    }
}

// Serial version (for checking)
void SerialResultCalculation(double* pMatrix, double* pVector,
                             double* pResult, int Size) {

    for (int i = 0; i < Size; i++) {
        pResult[i] = 0;
        for (int j = 0; j < Size; j++) {
            pResult[i] += pMatrix[i * Size + j] * pVector[j];
        }
    }
}

// Check correctness
void TestResult(double* pMatrix, double* pVector,
                double* pResult, int Size) {

    if (ProcRank == 0) {
        double* pSerial = new double[Size];

        SerialResultCalculation(pMatrix, pVector, pSerial, Size);

        int equal = 1;
        for (int i = 0; i < Size; i++) {
            if (pResult[i] != pSerial[i]) {
                equal = 0;
                break;
            }
        }

        if (equal)
            printf("Results are identical\n");
        else
            printf("Results are NOT identical\n");

        delete[] pSerial;
    }
}

// Cleanup
void ProcessTermination(double* pMatrix, double* pVector,
                        double* pResult, double* pProcRows,
                        double* pProcResult) {

    if (ProcRank == 0)
        delete[] pMatrix;

    delete[] pVector;
    delete[] pResult;
    delete[] pProcRows;
    delete[] pProcResult;
}

// MAIN
int main(int argc, char* argv[]) {

    double* pMatrix;
    double* pVector;
    double* pResult;
    double* pProcRows;
    double* pProcResult;

    int Size;
    int RowNum;

    double Start, Finish, Duration;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0)
        printf("Parallel matrix-vector multiplication\n");

    ProcessInitialization(pMatrix, pVector, pResult,
                          pProcRows, pProcResult,
                          Size, RowNum);

    Start = MPI_Wtime();

    DataDistribution(pMatrix, pProcRows, pVector, Size, RowNum);
    ParallelResultCalculation(pProcRows, pVector, pProcResult, Size, RowNum);
    ResultReplication(pProcResult, pResult, Size, RowNum);

    Finish = MPI_Wtime();
    Duration = Finish - Start;

    TestResult(pMatrix, pVector, pResult, Size);

    if (ProcRank == 0)
        printf("Time: %f\n", Duration);

    ProcessTermination(pMatrix, pVector, pResult,
                       pProcRows, pProcResult);

    MPI_Finalize();
    return 0;
}