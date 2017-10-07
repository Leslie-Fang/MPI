/**
 * Name:
 * Student id:
 * ITSC email:
 */
/*
 * This is a mpi version of bellman_ford algorithm
 * Compile: mpic++ -std=c++11 -o mpi_bellman_ford mpi_bellman_ford.cpp
 * Run: mpiexec -n <number of processes> ./mpi_bellman_ford <input file>, you will find the output file 'output.txt'
 * */

#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cstring>

#include "mpi.h"

using std::string;
using std::cout;
using std::endl;

#define INF 1000000

MPI_Status state;
/**
 * utils is a namespace for utility functions
 * including I/O (read input file and print results) and matrix dimension convert(2D->1D) function
 */
namespace utils {
    int N; //number of vertices
    int *mat; // the adjacency matrix

    void abort_with_error_message(string msg) {
        std::cerr << msg << endl;
        abort();
    }

    //translate 2-dimension coordinate to 1-dimension
    int convert_dimension_2D_1D(int x, int y, int n) {
        return x * n + y;
    }

    int read_file(string filename) {
        std::ifstream inputf(filename, std::ifstream::in);
        if (!inputf.good()) {
            abort_with_error_message("ERROR OCCURRED WHILE READING INPUT FILE");
        }
        inputf >> N;
        //input matrix should be smaller than 20MB * 20MB (400MB, we don't have too much memory for multi-processors)
        assert(N < (1024 * 1024 * 20));
        mat = (int *) malloc(N * N * sizeof(int));
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                inputf >> mat[convert_dimension_2D_1D(i, j, N)];
            }
        return 0;
    }

    int print_result(bool has_negative_cycle, int *dist) {
        std::ofstream outputf("output.txt", std::ofstream::out);
        if (!has_negative_cycle) {
            for (int i = 0; i < N; i++) {
                if (dist[i] > INF)
                    dist[i] = INF;
                outputf << dist[i] << '\n';
            }
            outputf.flush();
        } else {
            outputf << "FOUND NEGATIVE CYCLE!" << endl;
        }
        outputf.close();
        return 0;
    }
}//namespace utils

// you may add some helper functions here.
void mybroad(int my_rank, int p, void* data, void* revdata,int count, MPI_Datatype datatype, MPI_Comm comm){

}
/**
 * Bellman-Ford algorithm. Find the shortest path from vertex 0 to other vertices.
 * @param my_rank the rank of current process
 * @param p number of processes
 * @param comm the MPI communicator
 * @param n input size
 * @param *mat input adjacency matrix
 * @param *dist distance array
 * @param *has_negative_cycle a bool variable to recode if there are negative cycles
*/
void bellman_ford(int my_rank, int p, MPI_Comm comm, int n, int *mat, int *dist, bool *has_negative_cycle) {
    //------your code starts from here------
    //step 1: broadcast N
    *has_negative_cycle = false;

    //init the dist in rank0
    if(my_rank == 0){
        for (int i = 0; i < n; i++) {
            dist[i] = INF;
        }
        dist[0] = 0;
    }
    MPI_Barrier(comm);

    //broadcast the n: number of verticals
    if(my_rank == 0){
        for(int j=1;j<p;j++){
            MPI_Send(&n, 1, MPI_INT, j, 0, comm);
        }
    }else{
        MPI_Recv(&n, 1, MPI_INT, 0, 0, comm, &state);
    }
    MPI_Barrier(comm);

    //broadcast the dist
    int *localdist;
    localdist = (int *) malloc(sizeof(int) * n);
    if(my_rank == 0){
        for(int j=1;j<p;j++){
            MPI_Send(dist, n, MPI_INT, j, 0, comm);
        }
        //In the rank 0, just copy it
        localdist = dist;
    }else{
        MPI_Recv(localdist, n, MPI_INT, 0, 0, comm, &state);
    }
    MPI_Barrier(comm);

    //a flag to record if there is any distance change in this iteration
    bool has_change = false;

    //step 2: find local task range
    int localEdges = n*n/p;//localEdges is the number of edges would be calculated in each process

    //step 3: allocate local memory
    int *localmat;
    localmat = (int *) malloc(n * n * sizeof(int));

    //step 4: broadcast matrix mat
    if(my_rank == 0){
        for(int j=1;j<p;j++){
            MPI_Send(mat, n * n, MPI_INT, j, 0, comm);
        }
        //In the rank 0, just copy it
        localmat = mat;
    }else{
        MPI_Recv(localmat, n*n, MPI_INT, 0, 0, comm, &state);
    }
    MPI_Barrier(comm);

    //step 5: bellman-ford algorithm
    //bellman-ford edge relaxation
    int localStartEdge = localEdges*my_rank;
    for (int i = 0; i < n - 1; i++){// n - 1 iteration
        has_change = false;
        for(int m=localStartEdge;m<localStartEdge+localEdges;m++) {
            int weight = localmat[m];
            if (weight < INF) {//test if u--v has an edge
                int u = m / n;
                int v = m % n;
                if (localdist[u] + weight < localdist[v]) {
                    has_change = true;
                    localdist[v] = localdist[u] + weight;
                }
            }
        }
        MPI_Barrier(comm);
//      broadcast has_change to the root
        if(my_rank != 0){
            MPI_Send(&has_change, 1, MPI_BYTE, 0, 0, comm);
        }else{
            bool localhas_change = false;
            for(int j=1;j<p;j++){
                MPI_Recv(&localhas_change, 1, MPI_BYTE, j, 0, comm, &state);
                has_change = localhas_change | has_change;
            }
        }
        MPI_Barrier(comm);
        if(my_rank == 0){
            for(int k = 1; k < p; k++){
                MPI_Send(&has_change, 1, MPI_BYTE, k, 0, comm);
            }
        }else{
            MPI_Recv(&has_change, 1, MPI_BYTE, 0, 0, comm, &state);
        }
        MPI_Barrier(comm);
        //if there is no change in this iteration, then we have finished
        if(has_change) {
            if(my_rank != 0){
                MPI_Send(localdist, n, MPI_INT, 0, 0, comm);
            }else{
                int *tmpdist;
                tmpdist = (int *) malloc(sizeof(int) * n);
                for(int j=1;j<p;j++){
                    MPI_Recv(tmpdist, n, MPI_INT, j, 0, comm, &state);
                    for(int k=0;k<n;k++){
                        if(tmpdist[k] < localdist[k]){
                            localdist[k] = tmpdist[k];
                        }
                    }
                }
            }
            MPI_Barrier(comm);
            if(my_rank == 0){
                for(int j=1;j<p;j++){
                    MPI_Send(localdist, n, MPI_INT, j, 0, comm);
                }
            }else{
                MPI_Recv(localdist, n, MPI_INT, 0, 0, comm, &state);
            }
            MPI_Barrier(comm);
        }else{
            break;
        }
    }

    //last run to judge
    for(int m=localStartEdge;m<localStartEdge+localEdges;m++) {
        int weight = localmat[m];
        if (weight < INF) {//test if u--v has an edge
            int u = m / n;
            int v = m % n;
            if (localdist[u] + weight < localdist[v]) {
                *has_negative_cycle = true;
            }
        }
    }
    if(my_rank != 0){
        MPI_Send(has_negative_cycle, 1, MPI_BYTE, 0, 0, comm);
    }else{
        bool localhas_negative_cycle = false;
        for(int j=1;j<p;j++){
            MPI_Recv(&localhas_negative_cycle, 1, MPI_BYTE, j, 0, comm, &state);
            *has_negative_cycle = localhas_negative_cycle | *has_negative_cycle;
        }
    }

    //step 6: retrieve results back
//    if(my_rank == 0){
//        for(int k=0;k<n;k++){
//            dist[k] = localdist[k];
//        }
//    }
    //step 7: remember to free memory
   if(my_rank != 0){
       free(localdist);
       free(localmat);
   }
    return;
    //------end of your code------
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        utils::abort_with_error_message("INPUT FILE WAS NOT FOUND!");
    }
    string filename = argv[1];

    int *dist;
    bool has_negative_cycle = false;
    //MPI initialization
    MPI_Init(&argc, &argv);
    MPI_Comm comm;

    int p;//number of processors
    int my_rank;//my global rank
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &my_rank);

    //only rank 0 process do the I/O
    if (my_rank == 0) {
        assert(utils::read_file(filename) == 0);
        dist = (int *) malloc(sizeof(int) * utils::N);
    }

    MPI_Barrier(comm);//waiting until all the processes implemented to this statement
    //time counter
    double t1, t2;
    t1 = MPI_Wtime();

//    bellman-ford algorithm
    bellman_ford(my_rank, p, comm, utils::N, utils::mat, dist, &has_negative_cycle);
    MPI_Barrier(comm);
//    //end timer
    t2 = MPI_Wtime();
    if (my_rank == 0) {
        std::cerr.setf(std::ios::fixed);
        std::cerr << std::setprecision(6) << "Time(s): " << (t2 - t1) << endl;
        utils::print_result(has_negative_cycle, dist);
        free(dist);
        free(utils::mat);
    }
    MPI_Finalize();
    return 0;
}
