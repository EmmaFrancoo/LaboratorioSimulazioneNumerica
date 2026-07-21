#include "funzioni.h"

using namespace std;

int main() {

    Random rnd;  
    random_generator(rnd, 4);

    int N = 35;     // Number of cities: 34 distinct cities plus the repeated starting city
    int M = 100;    // Number of individuals in each population
    int generations =1000;      // Number of generations
    
    // Elements used for the circumference configuration
    Individual individual (N); 
    vector <Individual> population(M, Individual(N));
    vector <Individual> new_population;
    vector <Individual> parents(2,Individual(N));
    vector <Individual> children(2, Individual(N));

    // Elements used for the square configuration
    Individual individual_square (N); 
    vector <Individual> population_square(M, Individual(N));
    vector <Individual> new_population_square;
    vector <Individual> parents_square(2,Individual(N));
    vector <Individual> children_square(2, Individual(N));

    // Assign city labels in order; the final label is overwritten with 1 to close the path
    for(int i=0; i<N; i++){
        individual.cities[i].index = i+1;
        individual_square.cities[i].index = i+1;
    }
    individual.cities[N-1].index = 1;
    individual_square.cities[N-1].index = 1;

    // Generate the city coordinates on the unit circumference
    circle_positions(individual,rnd);  
    // Generate the city coordinates inside the unit square
    square_positions(individual_square,rnd);  

    // Build the initial populations by repeatedly permuting the same reference path
    for(int i=0; i<M; i++){
        int k = 0;
        population[i] = individual;
        population_square[i] = individual_square;
        // Perform several random pair permutations
        do{
            population[i] = pair_permutation(population[i],rnd);
            population_square[i] = pair_permutation(population_square[i],rnd);
            k++;
        }while(k<N);
        
        // Verify that each generated path starts and ends at city 1 and contains no repetitions
        check(population[i]);
        check(population_square[i]);
        population[i].distance = distance(population[i]);
        population_square[i].distance = distance(population_square[i]);
    }


    // Files to save the best path length and the mean over the best half at each generation
    ofstream output("outputL1_best.txt");
        output << "#     GENERATION:   L1:   BEST HALF <L1>:" << endl;

    ofstream output_s("outputL1_best_square.txt");
        output_s << "#     GENERATION:   L1:   BEST HALF <L1>:" << endl;

// --------------------------- Genetic evolution loop both for circumference and square case

    for(int j=0; j<generations; j++){
        
        // Oreder populations by increasing distance
        population = order_operator(population);
        population_square = order_operator(population_square);

        // Files to store the worst path of the initial sorted population for later comparison
        if(j==0){
            ofstream out ("WorstPath.txt");
            out << "#     x:   y:" << endl;

            ofstream out_s ("WorstPath_square.txt");
            out_s << "#     x:   y:" << endl;

            for(int i=0; i<N; i++){
                out << setw(12) << population[N-1].cities[i].x
                << setw(12) << population[N-1].cities[i].y
                << setw(12) << population[N-1].cities[i].index <<endl;

                out_s << setw(12) << population_square[N-1].cities[i].x
                << setw(12) << population_square[N-1].cities[i].y
                << setw(12) << population_square[N-1].cities[i].index <<endl;
            }
            out.close();
            out_s.close();
        }

        // Compute the average path length over the best half of each population
        double sum = 0.;
        double sum_s = 0.;
        for(int i=0; i<M/2; i++){
            sum+=population[i].distance;
            sum_s+=population_square[i].distance;
        }

        double media = sum/(M/2);
        double media_s = sum_s/(M/2);

        output << setw(12) << j
             << setw(12) << population[0].distance 
             << setw(12) << media << endl;

        output_s << setw(12) << j
             << setw(12) << population_square[0].distance 
             << setw(12) << media_s << endl;

        // Each iteration creates two children; M/2 iterations therefore rebuild a population of size M
        int k=1;
        while(k<=M/2){

            // Select two parents 
            parents = select(population, rnd);
            parents_square = select(population_square, rnd);

            // Crossover with >50% probability
            double r = rnd.Rannyu(0,1);
            if(r<0.6){ 
                children = crossover(parents, rnd);
                children_square = crossover(parents_square, rnd);
                
            for(int i=0; i<2; i++){
                    // Check if children satisfy the constraints
                    check(children[i]);
                    check(children_square[i]);
                }
            }else{
                children = parents; // If crossover doesn't happen the parents are preserved
                children_square = parents_square;
            }

            // Pair-permutation mutation with <10% probability
            double n = rnd.Rannyu();
            if(n<0.09){ 
                // Apply pair-permutation mutation to both children and check them
                children[0] = pair_permutation(children[0], rnd);
                children[1] = pair_permutation(children[1], rnd);
                check(children[0]);
                check(children[1]);

                children_square[0] = pair_permutation(children_square[0], rnd);
                children_square[1] = pair_permutation(children_square[1], rnd);
                check(children_square[0]);
                check(children_square[1]);
            }

            // Shift mutation with <10% probability 
            double s = rnd.Rannyu();
            if(s<0.09){
                // Apply block-shift mutation to both children and check them
                shift(children, rnd);
                check(children[0]);
                check(children[1]);

                shift(children_square, rnd);
                check(children_square[0]);
                check(children_square[1]);
            }

            // Inversion mutation with <10% probability 
            double a = rnd.Rannyu();
            if(a<0.09){
                // Apply inversion mutation to both children  and check them
                inversion(children[0], rnd);
                inversion(children[1], rnd);
                check(children[0]);
                check(children[1]);

                inversion(children_square[0], rnd);
                inversion(children_square[1], rnd);
                check(children_square[0]);
                check(children_square[1]);
            }

            // Compute distance of the new children and add them to the new population
            for(int i=0; i<2; i++){
                children[i].distance = distance(children[i]);     
                children_square[i].distance = distance(children_square[i]);
                new_population.push_back(children[i]);
                new_population_square.push_back(children_square[i]);
            }
            
            k++;
        }

        // Replace the old populations with the newly generated offspring
        population = new_population;
        new_population.clear();

        population_square = new_population_square;
        new_population_square.clear();
    
    }
// --------------------------- End of the genetic evolution loop

    population = order_operator(population);
    population_square = order_operator(population_square);

    double sum = 0.;
    double sum_s = 0.;
    for(int i=0; i<M/2; i++){
        sum+=population[i].distance;
        sum_s+=population_square[i].distance;
    }   
    double media = sum/(M/2);
    double media_s = sum_s/(M/2);

    output << setw(12) << generations
        << setw(12) << population[0].distance 
        << setw(12) << media << endl;

    output_s << setw(12) << generations
        << setw(12) << population_square[0].distance 
        << setw(12) << media_s << endl;

    output.close();
    output_s.close();

    // Save the coordinates and labels of the best final paths
    ofstream output2("BestPath.txt");
        output2 << "#     x:   y:   indice:" << endl;

    ofstream output2_s("BestPath_square.txt");
        output2_s << "#     x:   y:   indice:" << endl;

    for(int i=0; i<N; i++){
        output2 << setw(12) << population[0].cities[i].x
            << setw(12) << population[0].cities[i].y
            << setw(12) << population[0].cities[i].index <<endl;

            output2_s << setw(12) << population_square[0].cities[i].x
            << setw(12) << population_square[0].cities[i].y
            << setw(12) << population_square[0].cities[i].index <<endl;
    }

    output2.close();
    output2_s.close();

    return 0;
}