#include "random.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Structure representing a Random Walker
struct Walker {
    vector<double> position;  // Stores the current position of the walker in 3D space
    vector<double> r;         // Vector to store the squared distance from the origin at each step

    // Constructor that initializes the walker's position at the origin and r with zero
    Walker(int steptot) : position(3, 0.), r(steptot, 0.) {}

    // Function to calculate the squared distance from the origin
    double dist2() const {
        return pow(position[0], 2) + pow(position[1], 2) + pow(position[2], 2);
    }

    // Function to update position for discrete random walks
    void updatePosition_discrete(int direction, int a, int step) {
        position[direction] += a * step;
    }

    // Function to update position for continuous random walks
    void updatePosition_continue(double theta, double phi, int a) {
        position[0] += a * sin(theta) * cos(phi);
        position[1] += a * sin(theta) * sin(phi);
        position[2] += a * cos(theta);
    }
};


// Function to print the results to a file
void print(string filename, vector<double> A, vector<double> B) {
    ofstream out;
    out.open(filename);
    for (int i = 0; i < A.size(); i++) {
        out << i << " " << A[i] << " " << B[i] << endl;
    }
    out.close();
}

// Function to calculate the error (standard deviation) for block averages
double error(const vector<double>& av, const vector<double>& av2, int n) {
    if (n == 0)
        return 0;
    else
        return sqrt((av2[n] - pow(av[n], 2)) / n);  // Standard deviation
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


