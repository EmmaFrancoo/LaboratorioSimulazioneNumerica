#include "funzioni.h"
#include "mpi.h"

using namespace std;

int main(int argc, char* argv[]) {

    // MPI communicator size and rank of the process
    int size, rank;

    MPI_Init(&argc, &argv);  // Initialize the MPI environment
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the total number of MPI processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the current process
    MPI_Status stat;
    MPI_Request request;
    bool migration = false; // Set to true to enable migrations between MPI processes

    Random rnd;  
    // Use the MPI rank to initialize an independent random sequence on each process
    random_generator(rnd, rank);

    int N = 111; // Number of provincial capitals plus one repeated city to close the path
    int N_migr= 20; // Number of generations between two consecutive migrations
    //int N = 35; // Uncomment to test the circumference or square configurations
    int M = 100;
    int generations =1000;
    Individual individual (N); 
    vector <Individual> population(M, Individual(N));
    vector <Individual> new_population;
    vector <Individual> parents(2,Individual(N));
    vector <Individual> children(2, Individual(N));

    for(int i=0; i<N; i++)
        individual.cities[i].index = i+1;
    
    individual.cities[N-1].index = 1;

    // Optional test with cities placed on a circumference
   //circle_positions(individual,rnd);  // Uncomment to use the circumference configuration
    // Optional test with cities placed inside a square
    //square_positions(individual_square,rnd);  // Uncomment to use the square configuration

    // Load the coordinates of the Italian provincial capitals from file
    posizioni_capoluoghi(individual, "cap_prov_ita.dat");

    for(int i=0; i<M; i++){
        int k = 0;
        population[i] = individual;
        do{
            population[i] = pair_permutation(population[i],rnd);
            k++;
        }while(k<N);
        
        //cout<<"Primo check"<<endl;
        check(population[i]);
        population[i].distance = distance(population[i]);
    }


    // Each MPI process writes its evolution to an output file
    ofstream output("outputL1_best_rank" + to_string(rank) + ".txt");
        output << "#     GENERATION:   L1:   BEST HALF <L1>:" << endl;

    int gen = 0;
    vector<int> send_path(N);   // Vector to store city labels of the best path sent to another MPI process
    vector<int> recv_path(N);   // Vector to store city labels of the path received from another MPI process
    vector<int> rank_order(size);   //Vector to store permutation of the ranks 

    for(int j=0; j<generations; j++){
        population = order_operator(population);
        // Perform migrations only when multiple MPI processes are available and migration is enabled
        if(size > 1 and migration == true){
            
            if(gen == N_migr){     // Exchange the best individual after N_migr generations

                for(int i=0; i<N; i++)
                    send_path[i] = population[0].cities[i].index;   // Store the current best path
                
                if(rank == 0){                                     // Rank 0 generates a random permutation of all ranks
                    for(int i=0; i<size; i++)
                        rank_order[i]=i;

                for(int i=0; i<size; i++)
                    permutate(rank_order, rnd);

                // Optional check of the random rank ordering
                /*for(int i=0; i<size; i++)
                    cout<<rank_order[i]<<endl;
                 */
                }
               
                MPI_Bcast(rank_order.data(), size, MPI_INT, 0, MPI_COMM_WORLD); // Broadcast the same rank ordering to every process

                // Find the current rank position inside rank_order-->needed to determine ranks from which to receive and to which to send
                int position = -1;
                for(int i=0; i<size; i++){

                    if(rank_order[i] == rank){
                        position = i; 
                        break;
                    }
                }

                // Determine the ranks from which to receive and to which to send
                int rank_recv = 0;
                int rank_send = 0;

                if(position == 0){
                    rank_recv = rank_order[size-1];
                    rank_send = rank_order[1];
                }

                if(position == size-1){
                    rank_recv = rank_order[position-1];
                    rank_send = rank_order[0];
                }

                if(position!=0 and position!=(size-1)){
                    rank_recv = rank_order[position-1];
                    rank_send = rank_order[position+1];
                }

                // Use the same message tag for the path exchange
                int itag =1;

                // Send the best path and receive 
                MPI_Isend(send_path.data(), N, MPI_INT, rank_send, itag, MPI_COMM_WORLD, &request);
                MPI_Recv(recv_path.data(), N, MPI_INT, rank_recv, itag, MPI_COMM_WORLD, &stat);
                // Wait for the non-blocking send to complete
                MPI_Wait(&request, MPI_STATUS_IGNORE);

                // Reconstruct the received individual by matching each city label to its coordinates
                for(int i=0; i<N; i++){
                    // Replace the worst individual with the migrated path labels
                    population[M-1].cities[i].index = recv_path[i];
                    for(int j=0; j<N; j++){
                        if(population[M-1].cities[i].index == individual.cities[j].index){
                            population[M-1].cities[i].x = individual.cities[j].x;
                            population[M-1].cities[i].y = individual.cities[j].y;
                            break;
                        }
                    }
                }

                // Recompute distance of the population individual just exchanged
                population[M-1].distance = distance(population[M-1]);
                check(population[M-1]);
                
                population = order_operator(population);

                // Restart the migration counter after the exchange
                gen = 0;
            }
        }
        
        double sum = 0.;
        for(int i=0; i<M/2; i++)
            sum+=population[i].distance;
        
        double media = sum/(M/2);

        output << setw(12) << j
             << setw(12) << population[0].distance 
             << setw(12) << media << endl;

        int k=1;
        while(k<=M/2){

            //cout<<"Coppia "<<k<<endl;
            parents = select(population, rnd);

            double r = rnd.Rannyu(0,1);
            if(r<0.6){ 
                children = crossover(parents, rnd);
                for(int i=0; i<2; i++)
                    //cout<<"Check crossover"<<endl;
                    check(children[i]);
            }else{
                children = parents;
            }

            double n = rnd.Rannyu();
            if(n<0.09){ 
                //cout<<"Check permutazione"<<endl;
                children[0] = pair_permutation(children[0], rnd);
                children[1] = pair_permutation(children[1], rnd);
                check(children[0]);
                check(children[1]);
            }

            double s = rnd.Rannyu();
            if(s<0.09){
                //cout<<"Check shift"<<endl;
                shift(children, rnd);
                check(children[0]);
                check(children[1]);
            }

            double a = rnd.Rannyu();
            if(a<0.09){
                //cout<<"Check inversione"<<endl;
                inversion(children[0], rnd);
                inversion(children[1], rnd);
                check(children[0]);
                check(children[1]);
            }

            for(int i=0; i<2; i++){
                children[i].distance = distance(children[i]);     
                new_population.push_back(children[i]);
            }

            k++;
        }

        population = new_population;
        new_population.clear();

        gen++;
    
    }
//--------------
    population = order_operator(population);

    double sum = 0.;

    for(int i=0; i<M/2; i++)
        sum+=population[i].distance;
          
    double media = sum/(M/2);

    output << setw(12) << generations
        << setw(12) << population[0].distance 
        << setw(12) << media << endl;

    output.close();

    // Save the best final path found by each MPI rank
    ofstream output2("BestPath_rank" + to_string(rank) + ".txt");
        output2 << "#     x:   y:   indice:" << endl;

    for(int i=0; i<N; i++){
        output2 << setw(12) << population[0].cities[i].x
            << setw(12) << population[0].cities[i].y
            << setw(12) << population[0].cities[i].index <<endl;
    }

    output2.close();

    // Close the MPI environment 
    MPI_Finalize();

    return 0;
}