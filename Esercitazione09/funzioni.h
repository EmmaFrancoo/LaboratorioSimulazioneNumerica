#include "random.h" // Random number generator used by the genetic algorithm
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>

using namespace std;

// Struct to store index and position (x,y) of a city
struct City {

    int index;
    double x;
    double y;
};

// Struct to represent a path and distance L^(1)
struct Individual {

    vector <City> cities;
    double distance;

    Individual() : distance(0.0) {} // Constructors
    Individual(int N) : cities(N), distance(0.0) {}
};

// Function to generate random city coordinates inside the unit square
void square_positions(Individual& ind, Random& rnd){

    int N = ind.cities.size()-1;
    for(int i=0; i<N; i++){
        ind.cities[i].x = rnd.Rannyu();
        ind.cities[i].y = rnd.Rannyu();  
    }
    // Copy the starting coordinates into the final element so that the path is explicitly closed
    ind.cities.back().x = ind.cities[0].x;
    ind.cities.back().y = ind.cities[0].y;
}

// Function to generate city coordinates on the unit circumference
void circle_positions(Individual& ind, Random& rnd){

    int N = ind.cities.size()-1;
    for(int i=0; i<N; i++){
        double theta = rnd.Rannyu(0,2*M_PI);
        ind.cities[i].x = cos(theta);
        ind.cities[i].y = sin(theta);
    }
    // Copy the starting coordinates into the final element so that the path is explicitly closed
    ind.cities.back().x = ind.cities[0].x;
    ind.cities.back().y = ind.cities[0].y;
}

// Function to compute L^(1)
double distance(Individual ind){
    
    int N = ind.cities.size()-1;
    double L_1 = 0.;
    for(int i=0; i<N; i++)
        L_1 +=sqrt(pow(ind.cities[i].x-ind.cities[i+1].x,2)+pow(ind.cities[i].y-ind.cities[i+1].y,2));
    
    return L_1;
}

// Function to implement pair-permutation
Individual pair_permutation(Individual ind, Random& rnd){

    
    int N = ind.cities.size();

    // Draw indices in [1, N-2], the portion of the path that may be permuted. Positions 0 and N-1 both contain city 1 and must never be selected
    int i = static_cast<int>(rnd.Rannyu(1, N-1));
    int j = static_cast<int>(rnd.Rannyu(1, N-1));

    while(j == i)
        j = static_cast<int>(rnd.Rannyu(1, N-1));   // Indeces must be different

    swap(ind.cities[i], ind.cities[j]);

    return ind;
}

// Function to implement block-shift mutation to each of the two children
void shift(vector <Individual>& children, Random& rnd){

    int N = children[0].cities.size();
    
    // m1 and m2 are the lengths of the blocks that will be shifted. 
    int m1 = static_cast<int> (rnd.Rannyu(1,N-2)); 
    int m2 = static_cast<int> (rnd.Rannyu(1,N-2)); 

    // Choose the block starting index so that all m1 (and m2) cities lie before the fixed final city (which is city in position N-2)
    int start1 = static_cast<int> (rnd.Rannyu(1,N-1-m1));
    int start2 = static_cast<int> (rnd.Rannyu(1,N-1-m2));

    // Maximum allowed shift: remaining mutable positions after the selected block
    int n_max1 = N-1-m1-start1+1;   //+1 because rnd.Rannyu exclude the upper bound
    int n_max2 = N-1-m2-start2+1;

    // n1 and n2 specify how many positions each selected block is moved to the right
    int n1 = static_cast<int> (rnd.Rannyu(1, n_max1)); 
    int n2 = static_cast<int> (rnd.Rannyu(1,n_max2));

    // Perform shift
    rotate(children[0].cities.begin()+start1, children[0].cities.begin()+start1+m1, children[0].cities.begin() + start1 + m1 + n1); 
    rotate(children[1].cities.begin()+start2, children[1].cities.begin()+start2+m2, children[1].cities.begin()+start2 + m2 + n2);

}

// Function to reverse the order of the cities between two randomly selected mutable positions
void inversion(Individual& child, Random& rnd){

    int N = child.cities.size();
    
    int a = static_cast<int> (rnd.Rannyu(1,N-1));   // Select endpoints only in [1, N-2], leaving the two copies of city 1 unchanged
    int b = static_cast<int> (rnd.Rannyu(1,N-1));
    while(b==a){
        b = static_cast<int> (rnd.Rannyu(1,N-1));   // a and b must be different   
    }
    
    int start = a;
    int finish = b;

    
    if(a>b){    // Reorder the two indices so that start is the smaller one and finish the larger one
        finish = a;
        start = b;
    }

    int m = start-finish+1; // Length of the block to reverse

    for(int k = 0; k<(m/2); k++)
        swap(child.cities[start+k], child.cities[finish-k]);

}

// Functioon to verify that the path starts and ends at city 1 and contains no repeated internal cities
bool check(Individual ind){

    if(ind.cities[0].index != 1 or ind.cities.back().index != 1){
        cerr<<"Non ritorna alla città inziale!"<<endl;
        return false;
    }

    int N = ind.cities.size()-1;
    for(int i=1; i<N; i++){
        for(int j=i+1; j<N; j++){
            if(ind.cities[i].index==ind.cities[j].index){
                cerr<<"Città ripetute!"<<endl;
                return false;
            }
        }
    }

    return true;
}

// Function to sort the population in ascending order of path length L^(1)
vector <Individual> order_operator (vector <Individual>& population){

    vector <Individual> ordered_pop;
    ordered_pop.push_back(population[0]);   // Position 0 stores the current shortest path
    
    for(int i=1; i<population.size(); i++){

        int j=1;   

        if(population[i].distance<ordered_pop[0].distance)
            ordered_pop.insert(ordered_pop.begin(), population[i]);

        else { 
            while(j<ordered_pop.size()){   

                if(population[i].distance<ordered_pop[j].distance){
                    ordered_pop.insert(ordered_pop.begin()+j, population[i]);
                    break;
                }
                else
                j++;  
             }

             if (j == ordered_pop.size()) 
                ordered_pop.push_back(population[i]);
        }     
    }

    return ordered_pop;
}

// Function to select two distinct parents with a probability favoring shorter paths
vector <Individual> select (vector <Individual> population, Random& rnd){

    vector <Individual> parents;

    int M = population.size();
    // Since the population is ordered, r^p with p > 1 favors small indices and therefore fitter individuals
    int p = 2;
    int j = static_cast<int> (M*pow(rnd.Rannyu(),p));
    int k = static_cast<int>(M*pow(rnd.Rannyu(),p));
    parents.push_back(population[j]);

    if(k ==j){  // Continues until the two parents are different
        do{
            k = static_cast<int>(M * pow(rnd.Rannyu(), p));
        } while(k == j);
    }

     parents.push_back(population[k]);
    
    return parents;   
}

// Functiion to perform crossover while keeping city 1 fixed at the beginning and end
vector <Individual> crossover(vector <Individual> parents, Random& rnd){

    int N = parents[0].cities.size(); 
    int x = static_cast<int> (rnd.Rannyu(1,N-1)); // Choose the cut point only among mutable positions, excluding the fixed first and last city
    vector <int> indici;

    vector<City> A = parents[0].cities; // Keep copies of both original parents before overwriting their tails
    vector<City> B = parents[1].cities;

    for(int i=x; i<N-1; i++){ // Clear the tail from x to N-2, while preserving the repeated starting city at N-1
        parents[0].cities[i].index = 0.;
        parents[1].cities[i].index = 0.;
    }

    int k=2; // Variable to check city labels (k goes from 2 to N-1 to exclude city 1 and city 35 which has label 1)
    int count = 0; 

    for(int j=0; j<2; j++){ // Loop on the two parents

        // The new tail starts exactly at the crossover point
        int posizione = x;
        indici.clear();

        do{
            for(int i=1; i<x; i++){ // Search only the prefix inherited unchanged by the current child
                if(k != parents[j].cities[i].index)
                    count++;  
            }

            // count == x-1 means that city k does not appear in positions [1, x-1]
            if(count == x-1){
                // City k is absent from the inherited prefix and must therefore be inserted in the child tail

                if( j == 0){    // Parent 1
                    
                    for(int l=1; l<N-1; l++){
                        if(B[l].index == k){
                            indici.push_back(l);    // Store the position of the missing city in the opposite parent
                            break;  // Break the cycle when the label city is found
                        }    
                    }
                }
                else{   //Parent 2
                    for(int l=1; l<N-1; l++){
                        if(A[l].index == k){    
                            indici.push_back(l);
                            break;
                        }    
                    }  
                }
            }

            k++;
            count = 0;

        }while(k<=N-1); // Continue until every non-fixed city label has been checked

        for(int i=0; i<indici.size(); i++){ // Sort positions so that missing cities retain their relative order in the other parent
            for(int l=i+1; l<indici.size(); l++){
                if(indici[i]>indici[l])
                    swap(indici[i], indici[l]);
            }
        }

        // Fill the cleared tail using the order inherited from the opposite parent 
        for (int i = 0; i < indici.size(); i++) {
            if (j == 0) {
                parents[j].cities[posizione] = B[indici[i]];
            } else {
                parents[j].cities[posizione] = A[indici[i]];
            }

            posizione++;
        }

        k = 2;
    }

    return parents; // The modified parent copies now represent the two offspring (children)
}

// Initialize the random number generator from the Primes and seed.in files
void random_generator (Random& rnd, int n) {
    
    int seed[n]; 
    int p1, p2;

    // Read the two prime numbers required by the random number generator
    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
    }
    Primes.close();

    // Read the four-component seed and initialize the generator
    ifstream input("seed.in");
    string property;
    if (input.is_open()) {
        while (input >> property) {
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);  // Complete the generator initialization
            }
        }
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }
    input.close();
}
