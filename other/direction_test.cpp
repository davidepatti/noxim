//--------------------------- Fliiping bit---------------------------------
//long unsigned int  j ^= 1UL << log2(8)-i; bitset& flip (size_t 3-i);
//cout << "The Node N(" << i << "," << j << ") get connected to the 2 Nodes: N1(" << i-1 << "," << j << ") and N2(" << i-1 << "," 			<< m << ")" << "\n";
	
	//|| i/2 == 0 && j < sw/4)

//*************************************************************************
//------------ First Solution: 1 stage & last satge------------------------
// *** The first stage *** (i == 1 && j < sw/2 )
//if (i != stg-1 && j < sw/2*i )
/*else if (i != stg-1 && j >= sw/2*i  )
	cout << "The Node N(" << i << "," << j << ") get connected to the direction 1 1 of Node: N1(" << i-1 << "," << j << ")  N2(" << i-1 		<< "," << m << ")" << "\n";*/

	// *** stage in the midle ???!!!! ***
	
	//if (i/2 != 0 && j < sw/8) **** i-2 <= j && j <= i-1 && j >= 2*i && j <= 2*i+1
	/*if (i == 2 &&  j < sw/4 )
	cout << "The Node N(" << i << "," << j << ") get connected to the direction 0 0 of Node: N1(" << i-1 << "," << j << ") and N2(" << i-1 		<< "," << m << ")" << "\n";*/
	/*else 
	cout << "The Node N(" << i << "," << j << ") get connected to the direction 1 1 of Node: N1(" << i-1 << "," << j << ") N2(" << i-1 << 		"," << m << ")" << "\n";*/
	
// *** The last stage ***

	/*if (i == stg-1 && j%2 == 0   )
	cout << "The Node N(" << i << "," << j << ") get connected to the direction 0 0 of Node: N1(" << i-1 << "," << j << ") and N2(" << i-1 		<< "," << m << ")" << "\n";

	else if (i == stg-1 && j%2 != 0)
	cout << "The Node N(" << i << "," << j << ") get connected to the direction 1 1 of Node: N1(" << i-1 << "," << j << ") and N2(" << i-1 		<< "," << m << ")" << "\n";*/


//********************************************************************************************
/*string get_direction(int sw, int stg, int n_sw)
{
// n_sw: where am I ? : define line/switch number in stage -> j
int r= r = sw/(pow(2,stage));// r: number of print to change direction 00 -> 11 or 11 -> 00

if (n_sw / r == 0 ) 
return  "direction 0 0";
else  
return  "direction 1 1";

}
*/
//***************************************************************************************************************

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
int n; // n: number of tiles in butterfly 
cout << "give tile number n = "; cin >> n;

int stg = log2(n); // stg: stage number
int sw = n/2; // sw: number of switche in each stage
int d = 1; //starting dir is changed at first iteration
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
	
  	 
	//cout << "sw n." << i << " is connected in dir " << d << endl;
	

	
	}
}

//------Routing algorithm (Router2Tile) -------

/*int id;
cout << "give Router id = "; cin >> id;
int digit = id % 10;
  
//cout << "number " << digit << "\n";
cout << "The Router " << id << " get connected to the 2 Tiles: T" << digit *2 << " and T" << digit *2+1 << "\n";
*/

return 0; 

}
