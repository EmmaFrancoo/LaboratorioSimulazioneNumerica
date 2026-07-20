/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include "system.h"

using namespace std;

int main (int argc, char *argv[]){

  int nconf = 1; 
  System SYS; 
  SYS.initialize(); 
  SYS.initialize_properties(); 
  SYS.block_reset(0);


  for(int i=0; i < SYS.get_nbl(); i++){ 
    for(int j=0; j < SYS.get_nsteps(); j++){ 
      SYS.step(); 
      SYS.measure(); 
      if(j%10 == 0){
//LASCIA COMMENTATO!!//        SYS.write_XYZ(nconf); //Write actual configuration in XYZ format //Commented to avoid "filesystem full"! 
        nconf++; 
      }
    }
  
    SYS.averages(i+1);
    SYS.block_reset(i+1); //azzero per ripartire con il blocco successivo (_block_av infatti accumula per ogni blocco le medie ad ogni step; la media del blocco viene salvata in _average. Es: Voglio calcolare E_k ed ho 50 step per blocco? In _block_av ci saranno 50 medie dell'E_k.)
  }

  
  SYS.finalize();

  return 0;
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
