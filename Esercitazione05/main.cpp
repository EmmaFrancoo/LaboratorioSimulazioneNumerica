#include "funzioni.h"

int main() {

    Random rnd;  // Create an object of type Random
    random_generator(rnd, 4);

    int M = 1000000;  // Total number of Metropolis steps
    int N = 100;      // Number of statistical blocks
    int L = M/N;      // Number of steps in each block

    // Uniform transition probability

    vector<double> media(N, 0);       // Mean value of the radius for each block
    vector<double> media2(N, 0);      // Squared mean value for each block
    vector<double> sommaprog(N, 0);   // Progressive average
    vector<double> sommaprog2(N, 0);  // Progressive average of squared means
    vector<double> errore(N, 0);      // Statistical error
    double sum = 0;

    Point r_old {1.5, 0., 1000.};  // Initial position of the Markov chain (very far from origin on purpose! - to show the usefulness of equilibration )
    Point r_new {0., 0., 0.};      

    vector<double> accettazione(N, 0);  // Acceptance rate for each block
    double temp_accettazione = 0.;      // Number of accepted moves in the current block
    double a = 2.4;                     // Width of the uniform transition probability

    // Gaussian transition probability

    vector<double> media_gauss(N, 0);       
    vector<double> media2_gauss(N, 0);      
    vector<double> sommaprog_gauss(N, 0);   
    vector<double> sommaprog2_gauss(N, 0); 
    vector<double> errore_gauss(N, 0);     
    double sum_gauss = 0;

    Point r_old_gauss {1.5, 0., 0.};  // Initial position of the Gaussian Markov chain
    Point r_new_gauss {0., 0., 0.};  

    vector<double> accettazione_gauss(N, 0);  // Acceptance rate for each block
    double temp_accettazione_gauss = 0.;      // Number of accepted moves in the current block
    double sigma = 0.75;                      // Standard deviation of the Gaussian transition probability

    int steps_equil = 200000;  // Number of equilibration steps

    // Use x = 0 for a uniform transition and x = 1 for a Gaussian transition
    equilibration(0, steps_equil, a, r_old, r_new, rnd, frazione_100, true, "equilibration100_far.txt");
    equilibration(1, steps_equil, sigma, r_old_gauss, r_new_gauss, rnd, frazione_100);

    ofstream pos100("positions100.txt");
    ofstream pos100_gauss("positions100_gauss.txt");

    // Sample the probability density of the hydrogen 1s orbital
    for (int i = 0; i < N; i++) {  // Loop over blocks

        sum = 0.;
        sum_gauss = 0.;
        temp_accettazione = 0.;
        temp_accettazione_gauss = 0.;

        for (int j = 0; j < L; j++) {  // Loop over Metropolis steps

            Metropolis(0, a, r_old, r_new, temp_accettazione, frazione_100, rnd); //Perform one Metropolis step with uniform transition prob.
            sum += sqrt(r_old.x*r_old.x + r_old.y*r_old.y + r_old.z*r_old.z);//Accumulate distance from oorigin 

            // Save one sampled position every 100 steps
            if (j % 100 == 0)
                pos100 << r_old.x << " " << r_old.y << " " << r_old.z << endl;

            Metropolis(1, sigma, r_old_gauss, r_new_gauss, temp_accettazione_gauss, frazione_100, rnd); //Perform one Metropolis step with gaussian transition prob.
            sum_gauss += sqrt(r_old_gauss.x*r_old_gauss.x + r_old_gauss.y*r_old_gauss.y + r_old_gauss.z*r_old_gauss.z);

            // Save one sampled position every 100 steps
            if (j % 100 == 0)
                pos100_gauss << r_old_gauss.x << " " << r_old_gauss.y << " " << r_old_gauss.z << endl;
        }

        media[i] = sum / L;            // Mean radius of the i-th block
        media2[i] = pow(media[i], 2);  // Squared mean radius of the i-th block
        accettazione[i] = (temp_accettazione/double(L)) * 100;

        media_gauss[i] = sum_gauss / L;            // Mean radius of the i-th block
        media2_gauss[i] = pow(media_gauss[i], 2);  // Squared mean radius of the i-th block
        accettazione_gauss[i] = (temp_accettazione_gauss/double(L)) * 100;
    }

    pos100.close();
    pos100_gauss.close();

    // Calculate progressive averages and statistical errors
    sommeprog(media, media2, sommaprog, sommaprog2, errore);
    sommeprog(media_gauss, media2_gauss, sommaprog_gauss, sommaprog2_gauss, errore_gauss);

    // Print the results for the hydrogen 1s orbital
    print("output100.txt", sommaprog, errore, accettazione);
    print("output100_gauss.txt", sommaprog_gauss, errore_gauss, accettazione_gauss);

    // Set the initial positions for sampling the hydrogen 2p orbital
    r_old = {0., 0., 5.};
    r_old_gauss = {0., 0., 5.};

    // Reset the vectors used for the progressive averages and acceptance rates
    for (int i = 0; i < N; i++) {
        accettazione[i] = 0.;
        sommaprog[i] = 0.;
        sommaprog2[i] = 0.;

        accettazione_gauss[i] = 0.;
        sommaprog_gauss[i] = 0.;
        sommaprog2_gauss[i] = 0.;
    }

    a = 5.9;      // Width of the uniform transition probability
    sigma = 1.9;  // Standard deviation of the Gaussian transition probability

    //Equilibration
    equilibration(0, steps_equil, a, r_old, r_new, rnd, frazione_210);
    equilibration(1, steps_equil, sigma, r_old_gauss, r_new_gauss, rnd, frazione_210);

    ofstream pos210("positions210.txt");
    ofstream pos210_gauss("positions210_gauss.txt");

    // Sample the probability density of the hydrogen 2p orbital
    for (int i = 0; i < N; i++) {  // Loop over blocks

        sum = 0;
        temp_accettazione = 0.;

        sum_gauss = 0;
        temp_accettazione_gauss = 0.;

        for (int j = 0; j < L; j++) {  // Loop over Metropolis steps

            Metropolis(0, a, r_old, r_new, temp_accettazione, frazione_210, rnd);
            sum += sqrt(r_old.x*r_old.x + r_old.y*r_old.y + r_old.z*r_old.z);

            // Save one sampled position every 100 steps
            if (j % 100 == 0)
                pos210 << r_old.x << " " << r_old.y << " " << r_old.z << endl;

            Metropolis(1, sigma, r_old_gauss, r_new_gauss, temp_accettazione_gauss, frazione_210, rnd);
            sum_gauss += sqrt(r_old_gauss.x*r_old_gauss.x + r_old_gauss.y*r_old_gauss.y + r_old_gauss.z*r_old_gauss.z);

            // Save one sampled position every 100 steps
            if (j % 100 == 0)
                pos210_gauss << r_old_gauss.x << " " << r_old_gauss.y << " " << r_old_gauss.z << endl;
        }

        media[i] = sum / L;            // Mean radius of the i-th block
        media2[i] = pow(media[i], 2);  // Squared mean radius of the i-th block
        accettazione[i] = (temp_accettazione/double(L)) * 100;

        media_gauss[i] = sum_gauss / L;            // Mean radius of the i-th block
        media2_gauss[i] = pow(media_gauss[i], 2);  // Squared mean radius of the i-th block
        accettazione_gauss[i] = (temp_accettazione_gauss/double(L)) * 100;
    }

    pos210.close();
    pos210_gauss.close();

    // Calculate progressive averages and statistical errors
    sommeprog(media, media2, sommaprog, sommaprog2, errore);
    sommeprog(media_gauss, media2_gauss, sommaprog_gauss, sommaprog2_gauss, errore_gauss);

    // Print the results for the hydrogen 2p orbital
    print("output210.txt", sommaprog, errore, accettazione);
    print("output210_gauss.txt", sommaprog_gauss, errore_gauss, accettazione_gauss);

    return 0;
}