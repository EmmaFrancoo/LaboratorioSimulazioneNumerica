#include "random.h" // Include the random number generator header file
#include <cmath>
#include <fstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

// Compute the statistical uncertainty of a progressive block average
double error(vector<double> av, vector<double> av2, int n) {
    if (n == 0)
        return 0;
    else
        return sqrt((av2[n] - pow(av[n], 2)) / n);
}

// Sample |Psi_T(x)|^2 with a Metropolis move in position space
void Metropolis (double delta, double& x_old, double& x_new, double& accettazione, double mu, double sigma, Random& rnd ){

        x_new = rnd.Rannyu(x_old - delta/2., x_old + delta/2.);
        // Evaluate the trial probability density at the proposed position
        double p_new = pow(exp(-(x_new-mu)*(x_new-mu)/(2*sigma*sigma))+exp(-(x_new+mu)*(x_new+mu)/(2*sigma*sigma)),2);
        double p_old = pow(exp(-(x_old-mu)*(x_old-mu)/(2*sigma*sigma))+exp(-(x_old+mu)*(x_old+mu)/(2*sigma*sigma)),2);

        double n = rnd.Rannyu();
        double A = min(1.0, p_new/p_old); // Metropolis' acceptance probability

        if (n <= A){
            x_old = x_new;
            accettazione++; // Keeps tracks of how many times the move is accepted 
        }
}

//Metropolis for Simulated Annealing
void Metropolis_SA (double&E_old, double&E_new, double& err, double& err_new, double& sigma, double& sigma_new, double& mu, double& mu_new, double beta, double& accettazione, Random& rnd){

    double n = rnd.Rannyu();
    double A = min(1.0, exp(-beta*(E_new-E_old)));  // Metropolis' acceptance probability

    if(n<=A){   // If accepted update the parameters 
        E_old = E_new;
        mu = mu_new;
        sigma = sigma_new;
        err = err_new;
        accettazione++;
    }
}

// Propose new values of mu and sigma 
void update_parameters (double&mu, double&sigma, double& mu_new, double& sigma_new, double a, double b, Random& rnd){

    mu_new = rnd.Rannyu(mu - a, mu + a);
    do {
        sigma_new = rnd.Rannyu(sigma - b, sigma + b);
    } while (sigma_new <= 0.); // Repeat the extraction until sigma is positive
    
}

// Compute the local energy H Psi_T(x) / Psi_T(x)
double local_energy(double x_old, double mu, double sigma){

    double psi = exp(-(x_old-mu)*(x_old-mu)/(2*sigma*sigma))+exp(-(x_old+mu)*(x_old+mu)/(2*sigma*sigma));
    double V = pow(x_old,4)-5./2.*x_old*x_old;
    // Analytical second derivative of the trial wave function for the kinetic term
    double deriv_sec = (exp(-pow(x_old-mu,2)/(2*sigma*sigma)))/(sigma*sigma)*((pow(x_old-mu,2)/(sigma*sigma))-1)+(exp(-pow(x_old+mu,2)/(2*sigma*sigma)))/(sigma*sigma)*((pow(x_old+mu,2)/(sigma*sigma))-1);

    return (V*psi-0.5*deriv_sec)/psi;
}

// Compute progressive averages and statistical uncertainties from block estimates
void sommeprog (vector <double>& media, vector <double>& media2, vector <double>& sommaprog, vector <double>& sommaprog2, vector <double>& errore){
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


// Save progressive energy estimates, uncertainties and acceptance rates
void print (string filename, vector <double> E, vector <double> Err, vector <double> A ){

    ofstream out(filename);
   
    for (int i = 0; i < E.size(); i++) {
        out << i+1 << " " << E[i] << " " << Err[i] << " "<<A[i]<< "%" << endl;
    }
    out.close();
}


// Initialize the random number generator from the Primes and seed.in files
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