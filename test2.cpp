//
// Created by leslie-fang on 2017/10/6.
//
#include "mpi.h"
#include <iostream>
#include "stdio.h"

using namespace std;

int main(){
    char greets[100];
    char msg[128], rev[128];
    int comm_size;
    int comm_rank;
    int i;
    MPI_Status state;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);//comm_size is the number of process
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);//comm_rank is the process's number
//    cout<<"Current comm_rank is: "<<comm_rank<<". out of rank:"<<comm_size<<endl;
    int count[comm_size];
    if (comm_rank == 0){
        for(i = 1; i < comm_size; i++){
            sprintf(msg,"hello, %d, this is zero, I'am your master", i);
            printf("This Rank:%d send message to rank:%d\n", comm_rank, i);
            MPI_Send(msg, 128, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
//        for(i = 1; i < comm_size; i++){
//            MPI_Recv(rev, 128, MPI_CHAR, i, 0, MPI_COMM_WORLD, &state);
//            printf("P%d got: %s\n", comm_rank, rev);
//        }
    }else{
        MPI_Recv(rev, 128, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &state);
        printf("Rank:%d got: %s\n", comm_rank, rev);
        MPI_Get_count(&state,MPI_CHAR,&count[comm_rank]);
        cout<<"Rank: "<<comm_rank<<". The count is: "<<count[comm_rank]<<endl;
//        sprintf(msg, "hello, zero, this is %d, I'am your slave", comm_rank);
//        MPI_Send(msg, 128, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

//    if(comm_rank)
    MPI_Finalize();
    return 0;
}

