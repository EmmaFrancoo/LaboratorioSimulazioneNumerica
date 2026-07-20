#include "random.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Function to print progressive averages and statistical errors to file
void print(string filename, int N, vector<double> B, vector<double> C) {

    ofstream out;
    out.open(filename);

    for (int i = 0; i < N; i++) {
        out << i << " " << B[i] << " " << C[i] << endl;
    }

    out.close();
}

// Function to calculate the statistical error
double error(vector<double> av, vector<double> av2, int n) {
    if (n == 0)
        return 0;
    else
        return sqrt((av2[n] - pow(av[n], 2)) / n);
}

// Function to initialize the random number generator
void random_generator(Random& rnd, int n) {

    int seed[n];
    int p1, p2;

    // Read the two prime numbers used by the generator
    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
    }
    Primes.close();

    // Read the seed from file and initialize the generator
    ifstream input("seed.in");
    string property;

    if (input.is_open()) {
        while (input >> property) {
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }

    input.close();
}