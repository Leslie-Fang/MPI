//
// Created by leslie-fang on 2017/10/6.
//
#include "mpi.h"
#include <iostream>

using namespace std;

int main(){
    char greets[100];
    int comm_size;
    int comm_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);//comm_size is the number of process
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);//comm_rank is the process's number
    cout<<"Current comm_rank is: "<<comm_rank<<". out of rank:"<<comm_size<<endl;
//    if(comm_rank)
    MPI_Finalize();
    return 0;
}

