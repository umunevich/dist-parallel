#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

void slowFunction();

int main(int argc, char *argv[]) {
	int NumIntervals = 0;
	double IntervalWidth = 0.0;
	double IntervalLength = 0.0;
	double IntrvlMidPoint = 0.0;
	int Interval = 0;
	double MyPI = 0.0;
	double PI = 0.0;

	double ReferencePI = 3.141592653589793238462643;

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	char (*all_proc_names)[MPI_MAX_PROCESSOR_NAME];

	int numprocs;
	int MyID;
	int namelen;
	int proc = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &MyID);
	MPI_Get_processor_name(processor_name, &namelen);

	all_proc_names = malloc(numprocs * sizeof(*all_proc_names));

	MPI_Gather(processor_name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
	           all_proc_names, MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
	           0, MPI_COMM_WORLD);

	if (MyID == 0) {
		for (proc = 0; proc < numprocs; ++proc) {
			printf("Process %d on %s\n", proc, all_proc_names[proc]);
		}
	}

	if (MyID == 0) {
		if (argc > 1) {
			NumIntervals = atoi(argv[1]);
		} else {
			NumIntervals = 100000;
		}
		printf("NumIntervals = %i\n", NumIntervals);
	}

	// Broadcast number of intervals
	MPI_Bcast(&NumIntervals, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (NumIntervals != 0) {
		// Compute PI
		IntervalWidth = 1.0 / (double)NumIntervals;

		for (Interval = MyID + 1; Interval <= NumIntervals; Interval += numprocs) {
			IntrvlMidPoint = IntervalWidth * ((double)Interval - 0.5);
			IntervalLength += (4.0 / (1.0 + IntrvlMidPoint * IntrvlMidPoint));

            //slowFunction();
		}

		MyPI = IntervalWidth * IntervalLength;

		// Reduce all partial results
		MPI_Reduce(&MyPI, &PI, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

		if (MyID == 0) {
			printf("PI is approximately %.16f, Error is %.16f\n",
			       PI, fabs(PI - ReferencePI));
		}
	}

	free(all_proc_names);

	return MPI_Finalize();
}

void slowFunction() {
    int b = 2;
    for (int i = 0; i < 100000000; i++) {
        b += 13;
    }
}