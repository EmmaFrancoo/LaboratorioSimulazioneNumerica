#include "random.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Function to print the generated values to a file
void print(string filename, int M, vector<double> S, vector<double> SExp, vector<double> SCL) {

    ofstream out(filename);

    for (int i = 0; i < M; i++) {
        out << S[i] << " " << SExp[i] << " " << SCL[i] << endl;
    }

    out.close();
}

// Function to calculate M realizations of S_N for the three distributions
void Sum(int M, int N, vector<double>& S, vector<double>& SCL, vector<double>& SExp, Random& rnd) {

    // Clear vectors before calculating a new value of N
    S.clear();
    SCL.clear();
    SExp.clear();

    for (int i = 0; i < M; i++) {

        double sum_exp = 0;
        double sum_cl = 0;
        double sum_dice = 0;

        // Generate N random values for each distribution
        for (int j = 0; j < N; j++) {
            sum_exp += rnd.Exp(1);       // Exponential distribution with lambda = 1
            sum_cl += rnd.CL(0, 1);      // Cauchy-Lorentz distribution with mean = 0 and gamma = 1
            sum_dice += rnd.Rannyu(1, 6); // Uniform distribution in the range [1,6)
        }

        // Store the mean S_N for each distribution
        SExp.push_back(sum_exp / N);
        SCL.push_back(sum_cl / N);
        S.push_back(sum_dice / N);
    }
}

// Function to initialize the random number generator
void random_generator(Random& rnd, int n) {

    int seed[n];
    int p1, p2;

    // Read two prime numbers from the "Primes" file
    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
    }
    Primes.close();

    // Read the seed from "seed.in" and initialize the generator
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