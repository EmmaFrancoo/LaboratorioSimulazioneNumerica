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

int main(int argc, char *argv[]) {

  int nconf = 1;  

  System SYS;  

  SYS.initialize();            
  SYS.initialize_properties();  
  SYS.block_reset(0);           

  for (int i = 0; i < SYS.get_nbl(); i++) {  

    for (int j = 0; j < SYS.get_nsteps(); j++) {  

      SYS.step();     
      SYS.measure();  

      if (j % 10 == 0) {

        // LASCIARE COMMENTATO!!
        // SYS.write_XYZ(nconf);  // Write the current configuration in XYZ format
                                 // Commented out to avoid filling the filesystem

        nconf++;
      }
    }

    SYS.averages(i + 1);
    SYS.block_reset(i + 1);
  }

  SYS.finalize();

  return 0;
}
