//
// Created by leslie-fang on 2017/10/6.
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
    double step=(b-a)/div;
    double area=0;
    for(int i=0;i<div;i++){
        area += (f(a+i*step)+f(a+(i+1)*step))*step/2;
    }
    cout<<"The area is: "<<area<<endl;
    return 0;
}

