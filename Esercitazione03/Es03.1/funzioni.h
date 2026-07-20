#include "random.h" // Include the random number generator header file
#include <cmath>
#include <fstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

// Function for computing statistical error
double error(vector<double> av, vector<double> av2, int n) {
    if (n == 0)
        return 0;
    else
        return sqrt((av2[n] - pow(av[n], 2)) / n);
}

// Function for printing data to files
void print(string filename, int N, int L, vector<double> somme, vector<double> errore) {
    ofstream out;
    out.open(filename); 
    for (int i = 0; i < N; i++) {
        out << i * L << " " << somme[i] << " " << errore[i] << endl;
    }
    out.close(); 
}

//Function to set random number generator 
void random_generator (Random& rnd, int n) {
    
    int seed[n]; 
    int p1, p2;

    // Open the "Primes" file and read two integers into p1 and p2
    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
    }
    Primes.close();

    // Open the "seed.in" file to initialize the random number generator
    ifstream input("seed.in");
    string property;
    if (input.is_open()) {
        while (input >> property) {
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);  // Initialize the random number generator
            }
        }
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }
    input.close();
}

// Function to reset vectors
void reset_vectors (vector <double>& A, vector <double>& B, vector <double>& C, vector <double>& D){

    for (int i = 0; i < A.size(); i++) {
        A[i] = 0;
        B[i] = 0;
        C[i] = 0;
        D[i] = 0;
    }
}

//Function to calculate progressive averages 
void blocking (const vector<double>& media, const vector<double>& media2, vector<double>& sommaprog, vector<double>& sommaprog2,vector<double>& errore, int N){
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            sommaprog[i] += media[j];
            sommaprog2[i] += media2[j];
        }

        sommaprog[i] = sommaprog[i] / (i + 1);
        sommaprog2[i] = sommaprog2[i] / (i + 1);
        errore[i] = error(sommaprog, sommaprog2, i);
    }
}

