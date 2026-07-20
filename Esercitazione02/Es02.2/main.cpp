#include "funzioni.h"

int main() {

    //for "Walker" definition see "funzioni.h"

    Random rnd;  // Create an object of type Random
    random_generator(rnd, 4);

    int M = 10000;  // Total number of random walk simulations
    int N = 100;    // Number of statistical blocks
    int L = M / N;  // Number of random walks in each block

    int a = 1;      // Lattice constant
    int step = 0;   // Single step variable
    int steptot = 100;  // Total number of steps for each random walk

    // Vectors to store results
    vector<vector<double>> media(N, vector<double>(steptot));  // Average distances per block
    vector<double> sommaprog(steptot, 0.);  // Progressive sum of averages
    vector<double> sommaprog2(steptot, 0.);  // Progressive sum of squared averages
    vector<double> errore(steptot, 0.);  // Error at each step
    vector<Walker> R(L, Walker(steptot));  // Vector of Walker objects

    // DISCRETE CASE
    for (int i = 0; i < N; i++) {  // Loop over blocks

        for (int j = 0; j < L; j++) {  // Loop over random walks in a block
            Walker walker(steptot);

            for (int k = 0; k < steptot; k++) {  // Loop over steps of the current random walk

                // Generate a step for the discrete random walk
                int direction = rnd.Rannyu(0, 3);  // Choose direction (0, 1, 2 for x, y, z)
                double n = rnd.Rannyu();  // Generate random number for step direction

                if (n < 0.5) step = -1;
                else step = +1;

                // Update walker's position based on chosen direction and step
                walker.updatePosition_discrete(direction, a, step);
                walker.r[k] = walker.dist2();  // Store squared distance from the origin
            }

            R[j] = walker;  // Store walker for this random walk
        }

        vector<double> sum(steptot, 0.);

        // Sum the distances for each step
        for (int k = 0; k < steptot; k++) {
            for (int j = 0; j < L; j++) {
                sum[k] += R[j].r[k];  // Accumulate squared distances for each step
            }
            media[i][k] = sqrt(sum[k] /L);  // Compute the root mean squared distance
        }
    }

    // Calculate progressive averages and errors
    for (int k = 0; k < steptot; k++) {
        for (int i = 0; i < N; i++) {
            sommaprog[k] += media[i][k];
            sommaprog2[k] += pow(media[i][k], 2);
        }
        sommaprog[k] /= N;
        sommaprog2[k] /= N;
        errore[k] = error(sommaprog, sommaprog2, k);
    }

    // Print results for the discrete case
    print("output_discrete.txt", sommaprog, errore);

    // CONTINUOUS CASE

    // Reset vectors
    sommaprog.assign(steptot, 0.);
    sommaprog2.assign(steptot, 0.);
    errore.assign(steptot, 0.);

    double pi = M_PI;  // Pi constant

    // Loop over blocks
    for (int i = 0; i < N; i++) {

        for (int j = 0; j < L; j++) {  // Loop over random walks in a block
            Walker walker(steptot);

            for (int k = 0; k < steptot; k++) {  // Loop over steps of the current random walk
                // Generate random angles for the continuous random walk
                double phi = rnd.Rannyu(0, 2 * pi);  // Azimuthal angle
                double u = rnd.Rannyu();
                double theta = acos(1 - 2*u);    // Polar angle

                // Update walker's position based on theta and phi
                walker.updatePosition_continue(theta, phi, a);
                walker.r[k] = walker.dist2();  // Store squared distance from the origin
            }

            R[j] = walker;
        }

        vector<double> sum(steptot, 0.);

        // Sum the distances for each step
        for (int k = 0; k < steptot; k++) {
            for (int j = 0; j < L; j++) {
                sum[k] += R[j].r[k];  // Accumulate squared distances for each step
            }
            media[i][k] = sqrt(sum[k] / steptot);  // Compute the root mean squared distance
        }
    }

    // Calculate progressive averages and errors
    for (int k = 0; k < steptot; k++) {
        for (int i = 0; i < N; i++) {
            sommaprog[k] += media[i][k];
            sommaprog2[k] += pow(media[i][k], 2);
        }
        sommaprog[k] /= N;
        sommaprog2[k] /= N;
        errore[k] = error(sommaprog, sommaprog2, k);
    }

    // Print results for the continuous case
    print("output_continuous.txt", sommaprog, errore);

    return 0;
}
