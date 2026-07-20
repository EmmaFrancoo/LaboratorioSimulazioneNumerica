#include "funzioni.h"

int main() {

    Random rnd;  // Create an object of type Random
    
    random_generator(rnd, 4);

    double l = 1;     // needle's length
    int d = 2;        // distance between lines
    int n = 1000;     // total number of lines
    int t_length = d*n; // table's length 

    int M = 100000;   // total number of throws
    int N = 100;      // statistical blocks
    int L = M / N;    // throws in each block

    // vectors to store means, squared means, progressive values and errors
    vector<double> media(N, 0);
    vector<double> media2(N, 0);
    vector<double> sommaprog(N, 0);
    vector<double> sommaprog2(N, 0);
    vector<double> errore(N, 0);
    double sum = 0;

    for (int i = 0; i < N; i++) { // loop on blocks
        sum = 0; // variable to store intersections per block
        for (int j = 0; j < L; j++) { // loop over throws in each block

            // each end of the needle has (x, y), but imagining an infinite plane
            // (divided into parallel lines along the y-axis) the y coordinates are irrelevant -->
            // x2 marks the x of the upper end and an x1 marks the x of the lower end
            double x1 = rnd.Rannyu(0, t_length); // simulates the x coordinate where the lower end of the needle falls, y1 is set to zero

            // if I keep (x1, y1) fixed and rotate the needle, I trace out a circle
            // centered at (x1, y1=0) with radius l=1 --> unit circle
            bool nel_cerchio = false;
            double x, y;

            do {
                x = rnd.Rannyu(0, 1); // I focus on the line at the right of the needle
                y = rnd.Rannyu(-1, 1);

                if (x * x + y * y <= 1)
                    nel_cerchio = true;

            } while (!nel_cerchio);

            double theta = atan2(y, x); // The function returns the angle in radians between the x-axis
                                        // and the segment that connects the origin to the point of intersection
            double x2 = x1 + l * cos(theta); // calculate the x coordinate of the upper end of the needle (x2) (y2 is irrelevant)

            for (int k = 0; k < n; k++) {

                // Check if the needle crosses any of the lines
                if (x1 < (k * d) && x2 > (k * d))
                    sum++;

                else if (x1 > (k * d) && x2 < (k * d))
                    sum++;
            }
        }

        // Average for each block
        media[i] = (2 * l * L) / (double)(sum * d); // Estimate for pi using Buffon's formula
        media2[i] = pow(media[i], 2);
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            sommaprog[i] += media[j];
            sommaprog2[i] += media2[j];
        }
        sommaprog[i] = sommaprog[i] / (i + 1);
        sommaprog2[i] = sommaprog2[i] / (i + 1);
        errore[i] = error(sommaprog, sommaprog2, i);
    }

    print("output.txt", N, sommaprog, errore); // printing data on file

    return 0;
}

