#include <iostream>
#include <math.h>
#include <chrono>

constexpr double PI = 3.14159;

double function(double x){
    return 1.0+sin(x);
}

double trapezoidal_rule(double la, double lb, double h, double ln){
    double total{(function(la)+function(lb))*0.5};
    for(int i=1; i<ln; i++)total+=function(la+i*h);
    return total*h;
}

int main(int argc, char* argv[]){

    int n{10240};
    double a{0}, b{PI};
    double h{(b-a)/n};

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    double sum = trapezoidal_rule(a,b,h,n);

    std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();

    std::cout<<"Numerical integration using trapezoidal rule : "<<sum<<", Analytical result :"<<(b-cos(b))-(a-cos(a))<<"\n";

    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);

    std::cout<<"Time taken for numerical integration : "<<duration.count()<<" seconds\n";

    return 0;
}