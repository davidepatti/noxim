
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

for (int i = 1; i < stg ; i++) //stg
{
	for (int j = 0; j < sw ; j++) //sw 
	{
	int m = toggleKthBit(j, stg-i); // m: var to flipping bit

	int r = sw/(pow(2,i)); // change every r
    	
	int x = j%r;

	if (x==0) d = 1-d;
	cout << "The switch sw(" << i << "," << j << ") get connected to the direction " << d << d << " of Node: sw(" << i-1 << "," << j << 		")and sw(" << i-1 << "," << m << ")" << "\n";
		
	}
}

return 0; 

}
