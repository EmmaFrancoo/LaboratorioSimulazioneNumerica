#include "random.h"  // Include the random number generator header file
#include <cmath>
#include <fstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

// Struct representing a point in three-dimensional space
struct Point {
    double x;
    double y;
    double z;
};

// Function to calculate the statistical error
double error(vector<double> av, vector<double> av2, int n) {
    if (n == 0)
        return 0;
    else
        return sqrt((av2[n] - pow(av[n], 2)) / n);
}

// Metropolis acceptance probability for the hydrogen 1s orbital
double frazione_100(Point& r_old, Point& r_new) {

    double r_old_dist = sqrt(r_old.x*r_old.x + r_old.y*r_old.y + r_old.z*r_old.z); // Calculating distance from origin with r_old
    double r_new_dist = sqrt(r_new.x*r_new.x + r_new.y*r_new.y + r_new.z*r_new.z); // Calculating distance from origin with r_new

    return min(1.0, exp(2.0 * (r_old_dist - r_new_dist))); //Acceptance probability 
}

// Metropolis acceptance probability for the hydrogen 2p orbital
double frazione_210(Point& r_old, Point& r_new) {

    double r_old_dist = sqrt(r_old.x*r_old.x + r_old.y*r_old.y + r_old.z*r_old.z);
    double r_new_dist = sqrt(r_new.x*r_new.x + r_new.y*r_new.y + r_new.z*r_new.z);

    return min(1.0, (r_new.z*r_new.z / (r_old.z*r_old.z)) * exp(r_old_dist - r_new_dist)); //Acceptance probability 
}

// Perform one Metropolis step
void Metropolis(int x, double delta, Point& x_old, Point& x_new, double& accettazione, double (*funzione)(Point&, Point&), Random& rnd) {

    if (x == 0) {
        x_new.x = rnd.Rannyu(x_old.x - delta/2., x_old.x + delta/2.); //Sampling of the new point with uniform transition probability
        x_new.y = rnd.Rannyu(x_old.y - delta/2., x_old.y + delta/2.);
        x_new.z = rnd.Rannyu(x_old.z - delta/2., x_old.z + delta/2.);
    } else if (x == 1) {
        x_new.x = rnd.Gauss(x_old.x, delta);    //Sampling of the new point with Gaussian transition probability
        x_new.y = rnd.Gauss(x_old.y, delta);
        x_new.z = rnd.Gauss(x_old.z, delta);
    }

    double n = rnd.Rannyu();
    double A = funzione(x_old, x_new); //Variable to store acceptance probability 

    if (n <= A) {   //Metropolis' condition to accept the move or not
        x_old = x_new;
        accettazione++; //Variable to track the acceptance rate
    }
}

// Function to equilibrate the Markov chain 
void equilibration(int x, int steps_equil, double delta, Point& x_old, Point& x_new, Random& rnd, double (*funzione)(Point&, Point&), bool save_equilibration = false, string filename = "filename.txt") {

    ofstream out;

    if (save_equilibration == true)
        out.open(filename);

    double acc_dummy = 0.0; //unused here

    for (int i = 0; i < steps_equil; i++) {

        Metropolis(x, delta, x_old, x_new, acc_dummy, funzione, rnd);

        // Save the distance from the origin every 100 equilibration steps
        if (save_equilibration == true and i % 100 == 0) {

            double r = sqrt(x_old.x*x_old.x + x_old.y*x_old.y + x_old.z*x_old.z);
            out << i << " " << r << endl;
        }
    }

    if (save_equilibration == true) {
        out.close();
    }
}

// Function to calculate progressive averages and statistical errors
void sommeprog(vector<double>& media, vector<double>& media2, vector<double>& sommaprog, vector<double>& sommaprog2, vector<double>& errore) {

    for (int i = 0; i < media.size(); i++) {

        for (int j = 0; j <= i; j++) {
            sommaprog[i] += media[j];
            sommaprog2[i] += media2[j];
        }

        sommaprog[i] = sommaprog[i] / (i + 1);
        sommaprog2[i] = sommaprog2[i] / (i + 1);
        errore[i] = error(sommaprog, sommaprog2, i);
    }
}

// Function to print progressive averages, errors and acceptance rates
void print(string filename, vector<double> R, vector<double> Err, vector<double> A) {

    ofstream out(filename);

    for (int i = 0; i < R.size(); i++) {
        out << i << " " << R[i] << " " << Err[i] << " " << A[i] << "%" << endl;
    }

    out.close();
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
                rnd.SetRandom(seed, p1, p2);  // Initialize the random number generator
            }
        }
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }

    input.close();
}