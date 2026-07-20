#include "funzioni.h"

int main() {

    Random rnd;  // Create an object of type Random
    random_generator(rnd, 4);

    double S = 0; // Variable to store asset price
    double S_0 = 100; // Initial asset price
    double T = 1; // Delivery time
    double K = 100; // Strike price
    double r = 0.1; // Risk-free interest rate
    double sigma = 0.25; // Volatility
    int M = 100000; // Total number of simulations
    int N = 100; // Number of statistical blocks
    int L = M / N; // Number of simulations per block

    vector<double> mediaC(N, 0); // Vector to store mean call option prices
    vector<double> mediaP(N, 0); // Vector to store mean put option prices
    vector<double> media2C(N, 0); // Vector to store squared mean call option prices
    vector<double> media2P(N, 0); // Vector to store squared mean put option prices
    vector<double> sommaprogC(N, 0); // Vector to store cumulative sums for call option prices
    vector<double> sommaprogP(N, 0); // Vector to store cumulative sums for put option prices
    vector<double> sommaprog2C(N, 0); // Vector to store squared cumulative sums for call option prices
    vector<double> sommaprog2P(N, 0); // Vector to store squared cumulative sums for put option prices
    vector<double> erroreC(N, 0); // Vector to store statistical errors for call option prices
    vector<double> erroreP(N, 0); // Vector to store statistical errors for put option prices

	//Part 1: direct sampling
    for (int i = 0; i < N; i++) {
        double sumC = 0; // Initialize sum for call option prices
        double sumP = 0; // Initialize sum for put option prices

        // Loop over simulations in each block
        for (int j = 0; j < L; j++) {

            // Generate asset price using direct sampling
            S = S_0 * exp((r - pow(sigma, 2) / 2) * T + sigma * sqrt(T) * rnd.Gauss(0, 1));

            // Calculate payoff and add to sum for call and put options
            sumC += exp(-r * T) * max(0., (S - K));
            sumP += exp(-r * T) * max(0., (K - S));
        }
        
        // Update mean and squared mean option prices
        mediaC[i] = sumC / L;
        media2C[i] = pow(mediaC[i], 2);
        mediaP[i] = sumP / L;
        media2P[i] = pow(mediaP[i], 2);
        
    }

    //Calculate progressive mean
    blocking(mediaC, media2C, sommaprogC, sommaprog2C, erroreC, N);
    blocking(mediaP, media2P, sommaprogP, sommaprog2P, erroreP, N);

    // Print data to output files
    print("call.txt", N, L, sommaprogC, erroreC);
    print("put.txt", N, L, sommaprogP, erroreP);

    // Part 2: Discrete sampling 
	
    // Variable to store final asset price
    for (int i = 0; i < N; i++) {
        double sumC = 0; // Initialize sum for call option prices
        double sumP = 0; // Initialize sum for put option prices

        // Loop over simulations within each block
        for (int j = 0; j < L; j++) {

            double Sfin = S_0;
            // Generate asset prices using discretized sampling

            for (int t = 0; t < 100; t++) {
                Sfin *= exp((r - 0.5 * sigma * sigma) * 0.01+ sigma * sqrt(0.01) * rnd.Gauss(0, 1));
            }
            // Calculate payoff and add to sum for call and put options
            sumC += exp(-r * T) * max(0., (Sfin - K));
            sumP += exp(-r * T) * max(0., (K - Sfin));
        }
        
        // Update mean and squared mean option prices
        mediaC[i] = sumC / L;
        media2C[i] = pow(mediaC[i], 2);
        mediaP[i] = sumP / L;
        media2P[i] = pow(mediaP[i], 2);
    }

    // Reset vectors for cumulative sums and statistical errors
    reset_vectors(sommaprogC, sommaprogP, sommaprog2C, sommaprog2P);

    //Calcolo somme progressive metodo a blocchi
    blocking(mediaC, media2C, sommaprogC, sommaprog2C, erroreC, N);
    blocking(mediaP, media2P, sommaprogP, sommaprog2P, erroreP, N);
   
    // Print data to output files
    print("callstep.txt", N, L, sommaprogC, erroreC);
    print("putstep.txt", N, L, sommaprogP, erroreP);

    return 0;
}