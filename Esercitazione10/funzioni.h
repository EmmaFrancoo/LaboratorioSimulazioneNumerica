#include "random.h" // Include the random number generator header file
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>

using namespace std;
// For other explanations see Esercitazione 9
struct City {

    int index;
    double x;
    double y;
};

struct Individual {

    vector <City> cities;
    double distance;

    Individual() : distance(0.0) {}
    Individual(int N) : cities(N), distance(0.0) {}
};

// Function to store coordinates of the capoluoghi di provincia italiani
void posizioni_capoluoghi(Individual& ind, string filename){

    int N = ind.cities.size();

    ifstream file(filename);
    if (file.is_open()){

        for (int i = 0; i < N-1; i++)
            file >> ind.cities[i].x >> ind.cities[i].y;

        ind.cities[N - 1].x = ind.cities[0].x;  // So the path returns to the first city
        ind.cities[N - 1].y = ind.cities[0].y;
    }
    else{
        cerr << "PROBLEM: Unable to open" << filename << endl;
    }

    file.close();

}

void square_positions(Individual& ind, Random& rnd){

    int N = ind.cities.size()-1;
    for(int i=0; i<N; i++){
        ind.cities[i].x = rnd.Rannyu();
        ind.cities[i].y = rnd.Rannyu();  
    }
    ind.cities.back().x = ind.cities[0].x;
    ind.cities.back().y = ind.cities[0].y;
}

void circle_positions(Individual& ind, Random& rnd){

    int N = ind.cities.size()-1;
    for(int i=0; i<N; i++){
        double theta = rnd.Rannyu(0,2*M_PI);
        ind.cities[i].x = cos(theta);
        ind.cities[i].y = sin(theta);
    }
    ind.cities.back().x = ind.cities[0].x;
    ind.cities.back().y = ind.cities[0].y;
}

double distance(Individual ind){
    
    int N = ind.cities.size()-1;
    double L_1 = 0.;
    for(int i=0; i<N; i++)
        L_1 +=sqrt(pow(ind.cities[i].x-ind.cities[i+1].x,2)+pow(ind.cities[i].y-ind.cities[i+1].y,2));
    
    return L_1;
}

void permutate(vector <int>& rank_order,Random& rnd){

    int N =rank_order.size();
    int i = static_cast<int>(rnd.Rannyu(0, N));
    int j = static_cast<int>(rnd.Rannyu(0, N));

    while(j == i)
        j = static_cast<int>(rnd.Rannyu(0, N));

    swap(rank_order[i], rank_order[j]);

}

Individual pair_permutation(Individual ind, Random& rnd){

    int N = ind.cities.size();

    int i = static_cast<int>(rnd.Rannyu(1, N-1));
    int j = static_cast<int>(rnd.Rannyu(1, N-1));

    while(j == i)
        j = static_cast<int>(rnd.Rannyu(1, N-1));

    swap(ind.cities[i], ind.cities[j]);

    return ind;
}

void shift(vector <Individual>& children, Random& rnd){

    int N = children[0].cities.size();
    
    int m1 = static_cast<int> (rnd.Rannyu(1,N-2)); 
    int m2 = static_cast<int> (rnd.Rannyu(1,N-2)); 

    int start1 = static_cast<int> (rnd.Rannyu(1,N-1-m1));
    int start2 = static_cast<int> (rnd.Rannyu(1,N-1-m2));

    int n_max1 = N-1-m1-start1+1;
    int n_max2 = N-1-m2-start2+1;

    int n1 = static_cast<int> (rnd.Rannyu(1, n_max1)); 
    int n2 = static_cast<int> (rnd.Rannyu(1,n_max2));

    rotate(children[0].cities.begin()+start1, children[0].cities.begin()+start1+m1, children[0].cities.begin() + start1 + m1 + n1); 
    rotate(children[1].cities.begin()+start2, children[1].cities.begin()+start2+m2, children[1].cities.begin()+start2 + m2 + n2);

}

void inversion(Individual& child, Random& rnd){

    int N = child.cities.size();
    int a = static_cast<int> (rnd.Rannyu(1,N-1)); 
    int b = static_cast<int> (rnd.Rannyu(1,N-1));
    while(b==a){
        b = static_cast<int> (rnd.Rannyu(1,N-1));
    }
    
    int start = a;
    int finish = b;

    if(a>b){
        finish = a;
        start = b;
    }

    int m = start-finish+1;

    for(int k = 0; k<(m/2); k++)
        swap(child.cities[start+k], child.cities[finish-k]);

}

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

vector <Individual> order_operator (vector <Individual>& population){

    vector <Individual> ordered_pop;
    ordered_pop.push_back(population[0]);
    
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

vector <Individual> select (vector <Individual> population, Random& rnd){

    vector <Individual> parents;

    int M = population.size();
    int p = 2;
    int j = static_cast<int> (M*pow(rnd.Rannyu(),p));
    int k = static_cast<int>(M*pow(rnd.Rannyu(),p));
    parents.push_back(population[j]);

    if(k ==j){
        do{
            k = static_cast<int>(M * pow(rnd.Rannyu(), p));
        } while(k == j);
    }

     parents.push_back(population[k]);
    
    return parents;   
}

vector <Individual> crossover(vector <Individual> parents, Random& rnd){

    int N = parents[0].cities.size(); //lunghezza di un vector City -dentro Individual-, elemento di parents (vector di 2 Individual)
    int x = static_cast<int> (rnd.Rannyu(1,N-1)); //scelgo punto dove tagliare i due genitori, escludo la posizione 0 e 34 dove c'è 1 fissato
    vector <int> indici;

    vector<City> A = parents[0].cities; //copie dei genitori
    vector<City> B = parents[1].cities;

    for(int i=x; i<N-1; i++){ //cancello contenuto da x compreso in poi (lasciando l'1)
        parents[0].cities[i].index = 0.;
        parents[1].cities[i].index = 0.;
    }

    int k=2; //variabile per controllare tutti gli indici di città da 2 a 34 (1 iniziale e finale sono fissati!!)
    int count = 0; 

    for(int j=0; j<2; j++){ //ciclo for per considerare entrambi i genitori 

        int posizione = x;
        indici.clear();

        do{
            for(int i=1; i<x; i++){ //ciclo per scorrere su tutte le città di ogni genitore
                if(k != parents[j].cities[i].index)
                    count++;  
            }

            if(count == x-1){
                //parents[j].cities[posizione] = k;

                if( j == 0){
                    for(int l=1; l<N-1; l++){
                        if(B[l].index == k){
                            indici.push_back(l);
                            break;
                        }    
                    }
                }
                else{
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

        }while(k<=N-1); //il ciclo while continua fin quando non ho controllato tutte le città

        for(int i=0; i<indici.size(); i++){ //ho ordinato il vettore di indici secondo l'altro genitore
            for(int l=i+1; l<indici.size(); l++){
                if(indici[i]>indici[l])
                    swap(indici[i], indici[l]);
            }
        }

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

    return parents; //ora che sono figli
}

//Function to set random number generator 
void random_generator (Random& rnd, int rank) {
    
    int seed[4]; //il file seed.in ha 4 elementi
    int p1, p2;

    // Open the "Primes" file and read two integers into p1 and p2
    ifstream Primes("Primes");
    if (Primes.is_open()){
        // Ogni rank prende una coppia diversa dal file Primes
        for (int i = 0; i <= rank; i++)
            Primes >> p1 >> p2;
    }
    else{
        cerr << "PROBLEM: Unable to open Primes" << endl;
    }

    Primes.close();

    // Open the "seed.in" file to initialize the random number generator
    ifstream input("seed.in");
    string property;
    if (input.is_open()) {
        while (input >> property) {
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);  // Initialize the random number generator
            }
        }
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }
    input.close();
}

