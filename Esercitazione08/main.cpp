#include "funzioni.h"

int main() {

    Random rnd;  // Random number generator 
    random_generator(rnd, 6);

    int M = 1000000;     // Total number of Monte Carlo samples used in the initial VMC estimate 
    int N = 500;    // Number of blocks used for the statistical analysis
    int L = M/N;    // Number of "throws" in each block

    vector<double> media(N, 0);      
    vector<double> media2(N, 0);      
    vector<double> sommaprog(N, 0);   
    vector<double> sommaprog2(N, 0);  
    vector<double> errore(N, 0);      

    // Initial position and parameters of the trial wave function
    double x_old = 0.5;
    double x_new = 0.;
    double sigma = 0.5;
    double mu = 1.;

    // Acceptance rate of the position-space Metropolis algorithm for each block
    vector <double> accettazione_x (N,0);
    double temp_accettazione_x = 0.;
    // Width of the uniform proposal distribution for x
    double delta = 2.5;
    // Number of equilibration steps discarded before measurements
    int steps_equil = 20000;

    // Equilibrate the Markov chain before collecting measurements

    for (int i = 0; i < steps_equil; i++) 
        Metropolis(delta, x_old, x_new, temp_accettazione_x, mu, sigma, rnd );

    // 1) VMC simulation with the fixed parameters
    for (int i = 0; i < N; i++) {  // Loop over the blocks

        double sum = 0.;
        temp_accettazione_x = 0.;
   
        for (int j = 0; j < L; j++) {  //Loop over "throws" in each block
            
            // Generate a new position distributed according to |Psi_T(x)|^2
            Metropolis(delta, x_old, x_new, temp_accettazione_x, mu, sigma, rnd );
            // Accumulate the local energy evaluated at the sampled position
            sum+= local_energy(x_old, mu, sigma); 

        }
        media[i] = sum / L;            // Energy estimate of the i-th block
        media2[i] = pow(media[i], 2);  // Squared block estimate
        accettazione_x[i] = (temp_accettazione_x/double (L)) * 100;
    }

    // Compute the final progressive energy estimate and its uncertainty and print on file
    sommeprog(media, media2, sommaprog, sommaprog2, errore);
    print("output8.1.txt", sommaprog, errore, accettazione_x);

    // 2) Use the first VMC estimate as the initial energy for Simulated Annealing
    double E_old = sommaprog[N-1];
    double err = errore[N-1];

    double E_new = 0.;
    double mu_new = 0.;
    double sigma_new = 0.;
    double err_new = 0.;

    // Maximum variation used to propose a new value of mu
    double delta_mu = 0.3;
    // Maximum variation used to propose a new value of sigma
    double delta_sigma = 0.1;

    // Initial inverse temperature of the Simulated Annealing procedure
    double beta = 1.;

    M = 10000; //Reduced for simulated annealing 
    N= 100; //Reduced for simulated annealing 
    L = M / N;
    delta = 3.0;     // Width of the uniform proposal distribution for x in position-space Metropolis 

    // Variables used to stop the annealing when energy and parameters become stable

    int k = 0;
    double E_prec = E_old;
    double sigma_prec = sigma;
    double mu_prec = mu;
    int cicli_stabili = 0;

    ofstream out_SA("risultati_SA.txt");
        out_SA << "#     STEP_SA:   ENERGY:    ERROR:      MU:      SIGMA:      BETA:      ACC_SA:      ACC_X:" << endl;

    // Increase beta progressively so that energetically unfavorable moves become less likely
    do{
        // Acceptance rate of the Metropolis algorithm in parameter space
        double accettazione_SA = 0.;
        // Average acceptance rate of the position-space Metropolis sampling
        double accettazione_x = 0.;

        for (int l=0; l<10; l++){ // Perform 10 parameter proposals at each inverse temperature

            update_parameters(mu, sigma, mu_new, sigma_new, delta_mu, delta_sigma, rnd); // Propose new variational parameters (mu, sigma)
            // Reset the progressive accumulators before evaluating the new parameters
            fill(sommaprog.begin(), sommaprog.end(), 0.0);
            fill(sommaprog2.begin(), sommaprog2.end(), 0.0);
            temp_accettazione_x = 0.; 

            // Re-equilibrate the spatial Markov chain with the proposed parameters for 1000 steps
            for(int i=0; i<1000; i++)
               Metropolis(delta, x_old, x_new, temp_accettazione_x, mu_new, sigma_new, rnd ); // Metropolis' step in position-space

            temp_accettazione_x = 0.;

                for (int i = 0; i < N; i++) {  

                    double sum = 0.;
            
                    for (int j = 0; j < L; j++) {  
                        
                        Metropolis(delta, x_old, x_new, temp_accettazione_x, mu_new, sigma_new, rnd);   // Metropolis' step in position-space   
                        sum+= local_energy(x_old, mu_new, sigma_new); // Accumulate local energy

                    }
                    media[i] = sum / L;            // Energy estimate of the i-th block
                    media2[i] = pow(media[i], 2);  // Squared block estimate
                }

            // Build the progressive estimate of the energy for the proposed parameters
            sommeprog(media, media2, sommaprog, sommaprog2, errore);
            // Use the final progressive value as the new value of energy 
            E_new = sommaprog[N-1];
            err_new = errore[N-1];
            temp_accettazione_x/=M;
            accettazione_x += temp_accettazione_x;
            // Accept or reject the proposed parameters according to the annealing probability
            Metropolis_SA(E_old, E_new, err, err_new, sigma, sigma_new, mu, mu_new, beta, accettazione_SA, rnd);
        }
        accettazione_SA /= 10.0;
        accettazione_x/=10.0;

        // Save the current annealing state and both acceptance rates
        out_SA << setw(12) << k
             << setw(12) << E_old
             << setw(12) << err
             << setw(12) << mu
             << setw(12) << sigma
             << setw(12) << beta
             << setw(12) << accettazione_SA
             << setw(12) <<accettazione_x << endl;

        // Count consecutive annealing steps for which energy and parameters remain stable
        if (k >= 30 && fabs(E_old - E_prec) < 0.01 and fabs(mu - mu_prec) < 0.01 and fabs(sigma - sigma_prec) < 0.01) {
            cicli_stabili++;}
        else {
            cicli_stabili = 0;
        }

        E_prec = E_old;
        mu_prec = mu;
        sigma_prec = sigma;   

        // Cool the fictitious system by increasing beta after each annealing step
        beta *= 1.05;
        k ++;

    } while (cicli_stabili < 5 and k < 150); // Ends with 5 stable iterations, 150 is a safety measure to not produce an infinite do while cycle

    out_SA.close();

    // At this point, mu and sigma contain the optimized variational parameters

    // 3) Repeat point 1 with optimized parameters 
    M = 1000000; 
    N = 500;
    L = M/N;

    fill(sommaprog.begin(), sommaprog.end(), 0.0);
    fill(sommaprog2.begin(), sommaprog2.end(), 0.0);

    for (int i = 0; i < steps_equil; i++) 
        Metropolis(delta, x_old, x_new, temp_accettazione_x, mu, sigma, rnd );


    // Save sampled positions for the reconstruction of the probability density
    ofstream out_x("x_sampled.txt");

    // VMC simulation with the optimized parameters
    for (int i = 0; i < N; i++) {  // Loop over the blocks

        double sum = 0.;
        temp_accettazione_x = 0.;
   
        for (int j = 0; j < L; j++) {  // Loop over throws in a block
            
            Metropolis(delta, x_old, x_new, temp_accettazione_x, mu, sigma, rnd );
            sum+= local_energy(x_old, mu, sigma); 
            out_x << x_old << endl;  // Store sampled positions to reconstruct |Psi_T(x)|^2

        }
        media[i] = sum / L;            // Energy estimate of the i-th block
        media2[i] = pow(media[i], 2);  // Squared block estimate
        accettazione_x[i] = (temp_accettazione_x/double (L)) * 100;
    }

    out_x.close();

    // Compute the final progressive energy estimate and its uncertainty
    sommeprog(media, media2, sommaprog, sommaprog2, errore);
    print("E_after_SA.txt", sommaprog, errore, accettazione_x);

    rnd.SaveSeed();

    return 0;
}