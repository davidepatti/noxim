 #include <stdio.h>
#include <cmath>
#include <iostream> 

using namespace std;

int main(int argc, char* argv[]) {

	int n; // n: number of tiles in butterfly 
	cout << "give tile number n = "; cin >> n;

	int stg = log2(n); // stg: stage number
	int sw = n/2; // sw: number of switche in each stage


	/* EXPLE:
	  // --First Stage--
	    // sw(0,0) connected to core(0) -> dir 3 (NB.the wire signal is the same)
	    t[0][0]->flit_rx[3](flit[0][3].west);
	    core[0]->flit_tx[0](flit[0][3].west);

	    // sw(0,0) connected to core(1) -> dir 2
	    t[0][0]->flit_rx[2](flit[0][2].south);
	    core[1]->flit_tx[0](flit[0][2].south);
	    */
	    /* 
          // --Last Stage--
	    // sw(2,0) connected to core(0) -> dir 0 (NB.the wire signal is the same)
	    t[2][0]->flit_rx[0](flit[2][0].north);
	    core[0]->flit_tx[1](flit[2][0].north);

	    // sw(2,0) connected to core(1) -> dir 1
	    t[2][0]->flit_rx[1](flit[2][0].east);
	    core[1]->flit_tx[1](flit[2][0].east);
	    */


	  for (int i = 0; i < sw ; i++) 		
   	   {    // t : means switch	
	   
		cout << "t[" << 0 << "][" << i << "] -> flit_rx[3](flit[" << 0<< "][" << i << "].west)"<< "\n";
		//t[0][i]->flit_rx[3](flit[0][3].west);

		cout << "core[" << i*2 << "]" << " -> flit_tx[0](flit[" << 0<< "][" << i << "].west)"<< "\n";
		//core[sw*2]->flit_tx[0](flit[0][3].west);

		cout << "t[" << 0 << "][" << i << "] -> flit_rx[2](flit[" << 0<< "][" << i << "].south)"<< "\n";
		//t[0][i]->flit_rx[2](flit[0][2].south);

		cout << "core[" << (i*2)+1 << "]" << " -> flit_tx[0](flit[" << 0<< "][" << i << "].south)"<< "\n\n\n";
		//core[(sw*2)+1]->flit_tx[0](flit[0][2].south);
	    }

	  for (int i = 0; i < sw ; i++) 		
   	   {
		cout << "t[" << stg-1 << "][" << i << "] -> flit_tx[0](flit[" << stg-1<< "][" << i << "].north)"<< "\n";
		//t[stg-1][i]->flit_tx[0](flit[stg-1][i].north);

		cout << "core[" << i*2 << "]-> flit_rx[1](flit[" << stg-1<< "][" << i << "].north)"<< "\n";
		//core[sw*2]->flit_rx[1](flit[stg-1][i].north);

		
		cout << "t[" << stg-1 << "][" << i << "] -> flit_tx[1](flit[" << stg-1<< "][" << i << "].east)"<< "\n";
		//t[stg-1][i]->flit_tx[1](flit[stg-1][i].east);

		cout << "core[" << (i*2)+1 << "]-> flit_rx[1](flit[" << stg-1<< "][" << i << "].east)"<< "\n\n\n";
		//core[(sw*2)+1]->flit_rx[1](flit[stg-1][i].east);    
	    }
return 0; 

}
