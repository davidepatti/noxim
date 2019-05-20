

#include <stdio.h>
#include <cmath>
#include <iostream> 

using namespace std;
 


int toggleKthBit(int n, int k) 
{ 
    return (n ^ (1 << (k-1))); 
} 


int main(int argc, char* argv[]) {

//------Routing algorithm (Router2Router) -------

//****inputs*****

int n; 			// n: number of tiles in butterfly 
cout << "give tile number n = "; cin >> n;

int stg = log2(n); 	// stg: stage number
int sw = n/2; 		// sw: number of switche in each stage
int d = 1; 		//starting dir is changed at first iteration

//****outputs*****

	    /* --- EXPLE ---
	    // sw(1,0) connected to dir 0 of sw(0,0) -> dir 3
	    // - the wire signal is the same
	    t[1][0]->flit_rx[3](flit[0][0].north);
	    t[0][0]->flit_tx[0](flit[0][0].north);

	    // sw(1,0) connected to dir 0 of sw(0,2) -> dir 2
	    t[1][0]->flit_rx[2](flit[0][2].north);
	    t[0][2]->flit_tx[0](flit[0][2].north);
	    */

for (int i = 1; i < stg ; i++) //stg
{
	for (int j = 0; j < sw ; j++) //sw 
	{
	int m = toggleKthBit(j, stg-i); // m: var to flipping bit

	int r = sw/(pow(2,i)); // change every r
    	
	int x = j%r;

	if (x==0) d = 1-d;
	
	if (d==0)
	{ if (i%2==0) //stage od
	 {
	 	cout << "t[" << i << "][" << j << "] -> flit_rx [3](flit[" << i << "][" << j << "].west)"<< "\n";
	 	cout << "t[" << i << "][" << j << "] -> req_rx [3](flit[" << i << "][" << j << "].west)"<< "\n"; 
	 	cout << "t[" << i << "][" << j << "] -> ack_rx [3](flit[" << i << "][" << j << "].west)"<< "\n \n \n";

	 	cout << "t[" << i-1 << "][" << j << "] -> flit_tx ["<< d <<"](flit[" << i << "][" << j << "].west)" << "\n";
	 	cout << "t[" << i-1 << "][" << j << "] -> req_tx ["<< d <<"](flit[" << i << "][" << j << "].west)" << "\n";
	 	cout << "t[" << i-1 << "][" << j << "] -> ack_tx ["<< d <<"](flit[" << i << "][" << j << "].west)"<< "\n \n \n";

	 	cout << "t[" << i << "][" << j << "] -> flit_rx [2](flit[" << i << "][" << j << "].south)" << "\n";
	 	cout << "t[" << i << "][" << j << "] -> req_rx [2](flit[" << i << "][" << j << "].south)"<< "\n";
	 	cout << "t[" << i << "][" << j << "] -> ack_rx [2](flit[" << i << "][" << j << "].south)"<< "\n \n \n";

	 	cout << "t[" << i-1 << "][" << m << "] -> flit_tx ["<< d <<"](flit[" << i << "][" << j << "].south)"<< "\n";
	 	cout << "t[" << i-1 << "][" << m << "] -> req_tx ["<< d <<"](flit[" << i << "][" << j << "].south)"<< "\n";
	 	cout << "t[" << i-1 << "][" << m << "] -> ack_tx ["<< d <<"](flit[" << i << "][" << j << "].south)"<< "\n \n \n";
	}
	 /*else 
	 {

	 	cout << "t[" << i << "][" << j << "] -> flit_rx [3](flit[" << i-1 << "][" << j << "].north)"<< "\n";
	 	cout << "t[" << i << "][" << j << "] -> req_rx [3](flit[" << i-1 << "][" << j << "].north)"<< "\n"; 
	 	cout << "t[" << i << "][" << j << "] -> ack_rx [3](flit[" << i-1 << "][" << j << "].west)"<< "\n \n \n";

	 	cout << "t[" << i-1 << "][" << j << "] -> flit_tx ["<< d <<"](flit[" << i-1 << "][" << j << "].north)" << "\n";
	 	cout << "t[" << i-1 << "][" << j << "] -> req_tx ["<< d <<"](flit[" << i-1 << "][" << j << "].north)" << "\n";
	 	cout << "t[" << i-1 << "][" << j << "] -> ack_tx ["<< d <<"](flit[" << i-1 << "][" << j << "].west)"<< "\n \n \n";

	 	cout << "t[" << i << "][" << j << "] -> flit_rx [2](flit[" << i-1 << "][" << m << "].north)" << "\n";
	 	cout << "t[" << i << "][" << j << "] -> req_rx [2](flit[" << i-1 << "][" << m << "].north)"<< "\n";
	 	cout << "t[" << i << "][" << j << "] -> ack_rx [2](flit[" << i-1 << "][" << m << "].south)"<< "\n \n \n";

	 	cout << "t[" << i-1 << "][" << m << "] -> flit_tx ["<< d <<"](flit[" << i-1 << "][" << m << "].north)"<< "\n";
	 	cout << "t[" << i-1 << "][" << m << "] -> req_tx ["<< d <<"](flit[" << i-1 << "][" << m << "].north)"<< "\n";
	 	cout << "t[" << i-1 << "][" << m << "] -> ack_tx ["<< d <<"](flit[" << i-1 << "][" << m << "].south)"<< "\n \n \n";
	 }
	}
	else
	{
	cout << "t[" << i << "][" << j << "] -> flit_rx [2](flit[" << i-1 << "][" << j << "].east)"<< "\n"; 
	cout << "t[" << i << "][" << j << "] -> req_rx [2](flit[" << i-1 << "][" << j << "].east)"<< "\n";
	cout << "t[" << i << "][" << j << "] -> ack_rx [2](flit[" << i-1 << "][" << j << "].south)"<< "\n \n \n";
	

	
	cout << "t[" << i-1 << "][" << j << "] -> flit_tx ["<< d <<"](flit[" << i-1 << "][" << j << "].east)" << "\n";
	cout << "t[" << i-1 << "][" << j << "] -> req_tx ["<< d <<"](flit[" << i-1 << "][" << j << "].east)" << "\n";
	cout << "t[" << i-1 << "][" << j << "] -> ack_tx ["<< d <<"](flit[" << i-1 << "][" << j << "].south)"<< "\n \n \n";


	cout << "t[" << i << "][" << j << "] -> flit_rx [3](flit[" << i-1 << "][" << m << "].east)" << "\n";
	cout << "t[" << i << "][" << j << "] -> req_rx [3](flit[" << i-1 << "][" << m << "].east)" << "\n";
	cout << "t[" << i << "][" << j << "] -> req_rx [3](flit[" << i-1 << "][" << m << "].west)" << "\n \n \n";


	cout << "t[" << i-1 << "][" << m << "] -> flit_tx ["<< d <<"](flit[" << i-1 << "][" << m << "].east)"<< "\n";
	cout << "t[" << i-1 << "][" << m << "] -> req_tx ["<< d <<"](flit[" << i-1 << "][" << m << "].east)"<< "\n";
	cout << "t[" << i-1 << "][" << m << "] -> req_tx ["<< d <<"](flit[" << i-1 << "][" << m << "].west)"<< "\n \n \n";
	}*/

	}
}
}
return 0; 

}
