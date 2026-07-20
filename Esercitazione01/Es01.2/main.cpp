#include "funzioni.h"

int main() {

    Random rnd;  // Create an object of type Random
    
    random_generator(rnd, 4);

    int M = 10000; // Number of samples used to fill each histogram
    vector <double> SExp, SCL, S; //Vectors to store sums S_N

    //Sums with N=1
    Sum(M,1,S,SCL,SExp,rnd);
    print("outputS1.txt",M,S,SExp,SCL);

    //Sums with N=2
    Sum(M,2,S,SCL,SExp,rnd);
    print("outputS2.txt",M,S,SExp,SCL);

    //Sums with N=10
    Sum(M,10,S,SCL,SExp,rnd);
    print("outputS3.txt",M,S,SExp,SCL);

    //Sums with N=100
    Sum(M,100,S,SCL,SExp,rnd);
    print("outputS4.txt",M,S,SExp,SCL);

    return 0;
}





