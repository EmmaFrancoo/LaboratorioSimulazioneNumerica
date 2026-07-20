#include "funzioni.h"

int main() {

    Random rnd;  // Create an object of type Random
    random_generator(rnd, 4);


    // Part 1: Calculate the integral from 0 to 1 of r

    int M = 100000;  // Total number of throws
    int N = 100;     // Number of blocks
    int L = M / N;   // Number of throws per block
    vector<double> media(N, 0);       // Mean of each block
    vector<double> media2(N, 0);      // Squared mean of each block
    vector<double> sommaprog(N, 0);   // Cumulative average for each block
    vector<double> sommaprog2(N, 0);  // Cumulative average of squared means for each block
    vector<double> errore(N, 0);      // Error for each block
    double sum = 0;

    // Calculate mean and mean squared for each block
    for (int i = 0; i < N; i++) {  // i is the block index
        sum = 0;
        for (int j = 0; j < L; j++) {  // j is the throw index inside the block
            double n = rnd.Rannyu();  // Generate a random number between 0 and 1
            sum += n;
        }
        media[i] = sum / L;            // Mean of the i-th block
        media2[i] = pow(media[i], 2);  // Square of the mean of the i-th block
    }

    // Calculate cumulative average and error for each block
    for (int i = 0; i < N; i++) {
        for (int k = 0; k <= i; k++) { 
            sommaprog[i] += media[k];
            sommaprog2[i] += media2[k];
        }
        sommaprog[i] = sommaprog[i] / (i + 1);  // Average of means up to block i
        sommaprog2[i] = sommaprog2[i] / (i + 1);  // Average of squared means up to block i
        errore[i] = error(sommaprog, sommaprog2, i);  // Calculate error
    }

    // Write results to "output.txt"
    print("output.txt", sommaprog, 0.5, errore);

    // Part 2: Calculate the integral from 0 to 1 of (r-1/2)^2

    // Reset vectors
    reset(media, media2, sommaprog, sommaprog2, errore);

    // Calculate mean and mean squared for each block
    for (int i = 0; i < N; i++) {
        sum = 0;
        for (int j = 0; j < L; j++) {
            double n = rnd.Rannyu() - 0.5;  // Generate a random number between -0.5 and 0.5
            sum += pow(n, 2);  
        }
        media[i] = sum / L;            // Mean of the i-th block
        media2[i] = pow(media[i], 2);  // Square of the mean of the i-th block
    }

    // Calculate cumulative average and error for each block
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            sommaprog[i] += media[j];
            sommaprog2[i] += media2[j];
        }
        sommaprog[i] = sommaprog[i] / (i + 1);  // Average of means up to block i
        sommaprog2[i] = sommaprog2[i] / (i + 1);  // Average of squared means up to block i
        errore[i] = error(sommaprog, sommaprog2, i);  // Calculate error
    }

    // Write results to "output2.txt"
    print("output2.txt", sommaprog, 1.0 / 12, errore);

    // Part 3: Chi-square test
    
    double delta = 1.0 / (double)(N);  // Size of each interval
    int n = 10000;  // Number of random numbers for the chi-square test
    double E = n / (double)(N);  // Expected value for each interval
    vector<double> Chi2(N, 0);  // Vector to store chi-square results for each block

    // Perform chi-square test for 100 iterations
    for (int k = 0; k < N; k++) {

        vector<double> O(N, 0);  // Observed counts in each interval

        // Divide [0,1] into N intervals
        for (int i = 0; i < n; i++) {
            double x = rnd.Rannyu();
            for (int j = 0; j < N; j++) {
                if (x >= j * delta and x < (j + 1) * delta) 
                    O[j]+=1;
                
            }
        }

        // Sum chi-square values for the current iteration
        for (int i = 0; i < N; i++) 
            Chi2[k] += pow((O[i] - E), 2);
        
    }

    // Write chi-square results to "outputCHI2.txt"
    printChi2("outputCHI2.txt", Chi2, E);

    return 0;
}

