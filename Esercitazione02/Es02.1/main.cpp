#include "funzioni.h"

int main() {

    double pi = M_PI;

    Random rnd;  // Create an object of type Random
    random_generator(rnd, 4);

    int M = 100000;  // Total number of throws
    int N = 100;     // Number of blocks
    int L = M / N;   // Number of throws per block

    Results uniform;
    Results imp_sampling;

    double sum = 0;
    double sumIS = 0;

    // Calculate block averages using uniform sampling and importance sampling
    for (int i = 0; i < N; i++) {

        sum = 0;
        sumIS = 0;

        for (int j = 0; j < L; j++) {

            //Uniform sampling
            double n = rnd.Rannyu();    //Random number between 0,1)
            double g = (pi / 2) * cos(n * pi / 2); //integrand to sample

            // Importance sampling
            double r = rnd.Rannyu();    //Random number between 0,1)
            double n1 = 1 - sqrt(1 - r);    // n1 is distributed according to p obtained by inverse transform sampling

            // Probability density evaluated at n1
            double p = 2 * (1 - n1);
            double g1 = (pi / 2) * cos(n1 * pi / 2);    //Integrand to sample

            sum += g;
            sumIS += g1 / p;
        }

        uniform.media.push_back(sum / L);
        uniform.media2.push_back(pow(uniform.media[i], 2));

        imp_sampling.media.push_back(sumIS / L);
        imp_sampling.media2.push_back(pow(imp_sampling.media[i], 2));
    }

    // Calculate progressive averages and errors
    sommeprog(uniform, N);
    sommeprog(imp_sampling, N);

    // Write results to file
    print("output1.txt", N, uniform);
    print("output2.txt", N, imp_sampling);

    return 0;
}