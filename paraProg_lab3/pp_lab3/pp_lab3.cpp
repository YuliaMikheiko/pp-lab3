﻿
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "mpi.h" 
#define TAG_N 1
#define TAG_A 2
#define TAG_S 3



using namespace std;

int calculation(int* a, int N, int rank, int size) {
    int nt, beg, h, end, i;
    int sum = 0;
    nt = rank;
    h = N / size;
    beg = h * nt;
    if (nt == size - 1)
    {
        end = N - 1;
    }
    else
    {
        end = beg + h;
    }

    cout << "Process, begin, end of array " << nt << " " << beg << " " << end << "\n";
    for (i = beg; i <= end; i++)
    {
        if (a[i] % 2 != 0)
        {
            sum += a[i];
        }
    }
    return sum;
}

int main(int argc, char** argv) {

    srand(time(0));
    int rank, size;
    int* a;
    int N;
    bool fail = false;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    cout << "The number of processes: " << size << " my number is " << rank << "\n";


    MPI_Barrier(MPI_COMM_WORLD);
    double time = MPI_Wtime();
    if (rank == 0) {

        do {
            fail = false;
            cout << "Enter size array = ";
            cin >> N;
            if (cin.fail() || (N <= 0)) {
                cout << "You entered a non number or a number < 0. Enter again!" << endl;
                fail = true;
            }
            cin.clear();
            cin.ignore();
        } while (fail);



        a = new int[N];
        for (int i = 0; i < N; i++)
        {
            a[i] = 100 + rand() % 10000000;
        }



        for (int i = 1; i < size; i++)
        {
            int val = N % size;
            MPI_Send(&val, 1, MPI_INT, i, TAG_N, MPI_COMM_WORLD);
        }

        for (int i = 1; i < size; i++)
        {
            MPI_Send(a, N % size, MPI_INT, i, TAG_A, MPI_COMM_WORLD);
        }


        int sum = calculation(a, N, rank, size);
        for (int i = 1; i < size; i++) {
            int s = 0;
            MPI_Recv(&s, 1, MPI_INT, i, TAG_S, MPI_COMM_WORLD, &status);
            cout << "status.MPI_SOURCE " << status.MPI_SOURCE << "\n";
            cout << "status.MPI_TAG " << status.MPI_TAG << "\n";
            cout << "status.MPI_ERROR " << status.MPI_ERROR << "\n";
            sum += s;
        }
        cout << "Znachenie virajenia: " << sum << "\n";

    }
    else {
        int N1 = 0;
        MPI_Recv(&N1, 1, MPI_INT, 0, TAG_N, MPI_COMM_WORLD, &status);
        int* a1 = new int[N1];
        MPI_Recv(a1, N1, MPI_INT, 0, TAG_A, MPI_COMM_WORLD, &status);
        int s = calculation(a1, N1, rank, size);
        cout << "Rank: " << rank << " rezult_rank = " << s << "\n";
        MPI_Send(&s, 1, MPI_INT, 0, TAG_S, MPI_COMM_WORLD);

    }
    time = MPI_Wtime() - time;
    cout << "Computation time = " << time << "\n";

    MPI_Finalize();
    return 0;
}


