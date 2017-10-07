//
// Created by leslie-fang on 2017/10/7.
//
#include "mpi.h"
#include <iostream>
#include "stdio.h"

using namespace std;

int main(){
    int comm_size;
    int comm_rank;
    MPI_Status state;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);//comm_size is the number of process
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);//comm_rank is the process's number


    MPI_Finalize();

    return 0;
}
