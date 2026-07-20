#include "random.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Function to calculate statistical error
double error(const vector<double>& av, const vector<double>& av2, int n) {
    if (n == 0)
        return 0;
    else
        return sqrt((av2[n] - pow(av[n], 2)) / n);  // Standard deviation
}

// Function to reset vectors
void reset (vector <double>& media, vector <double>& media2, vector <double>& sommaprog, vector <double>& sommaprog2, vector <double>& errore){
    fill(media.begin(), media.end(), 0);
    fill(media2.begin(), media2.end(), 0);
    fill(sommaprog.begin(), sommaprog.end(), 0);
    fill(sommaprog2.begin(), sommaprog2.end(), 0);
    fill(errore.begin(), errore.end(), 0);
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

//Function to print data on file
void print (string filename , vector <double> sommaprog, float x, vector <double> errore) {

    ofstream out(filename);
   
    for (int i = 0; i < sommaprog.size(); i++) {
        out << i << " " << sommaprog[i] - x << " " << errore[i] << endl;
    }
    out.close();
}

//Function to print data on file
void printChi2 (string filename, vector <double> Chi2, double E) {

        ofstream out(filename);
   
    for (int i = 0; i < Chi2.size(); i++) { 
        out << i<< " "<< Chi2[i] / E << endl;
    }
    out.close();


}