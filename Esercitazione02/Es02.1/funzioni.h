#ifndef FUNZIONI_H
#define FUNZIONI_H

#include "random.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Struct used to store block averages, progressive averages and errors
struct Results {
    vector<double> media;
    vector<double> media2;
    vector<double> sommaprog;
    vector<double> sommaprog2;
    vector<double> errore;
};

// Function to calculate the statistical error
double error(const Results& res, int n) {
    if (n == 0)
        return 0;
    else
        return sqrt((res.sommaprog2[n] - pow(res.sommaprog[n], 2)) / n);
}

// Function to write progressive averages and errors to file
void print(string filename, int N, const Results& res) {

    ofstream out;
    out.open(filename);

    for (int i = 0; i < N; i++) {
        out << i << " " << res.sommaprog[i] - 1 << " " << res.errore[i] << endl;
    }

    out.close();
}

// Function to calculate progressive averages and statistical errors
void sommeprog(Results& res, int N) {

    for (int i = 0; i < N; i++) {

        res.sommaprog.push_back(0.0);
        res.sommaprog2.push_back(0.0);

        for (int j = 0; j <= i; j++) {
            res.sommaprog[i] += res.media[j];
            res.sommaprog2[i] += res.media2[j];
        }

        res.sommaprog[i] = res.sommaprog[i] / (i + 1);
        res.sommaprog2[i] = res.sommaprog2[i] / (i + 1);
        res.errore.push_back(error(res, i));
    }
}

// Function to set the random number generator
void random_generator(Random& rnd, int n) {

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
                rnd.SetRandom(seed, p1, p2);
            }
        }
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }

    input.close();
}

#endif