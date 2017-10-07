//
// Created by leslie-fang on 2017/10/7.
//
#include "mpi.h"
#include <iostream>
#include "stdio.h"

using namespace std;

double f(int x){
    double y;
    return y=(x^2)/2.0;
}

int main(){
    int a=0;
    int b=10;
    double div=1024;
    double area=0;
    int comm_size;
    int comm_rank;
    MPI_Status state;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);//comm_size is the number of process
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);//comm_rank is the process's number

    double step = (b-a)/div;
    double local_div = div/comm_size;
    double local_area;
    double local_start = a+comm_rank*local_div*step;
    for(int i =0;i<local_div;i++){
        local_area +=(f(local_start+i*step)+f(local_start+(i+1)*step))*step/2;
    }
    cout<<"I am rank:"<<comm_rank<<". My local area is:"<<local_area<<endl;

    if(comm_rank != 0){
        MPI_Send(&local_area, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }else{
        area+=local_area;
        double temp_area;
        for(int j=1;j<comm_size;j++){
            MPI_Recv(&temp_area, 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD, &state);
            area+=temp_area;
        }
        cout<<"I am rank:"<<comm_rank<<". The area is:"<<area<<endl;
    }
    MPI_Finalize();
    return 0;
}
