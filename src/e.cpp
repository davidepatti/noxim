#include <cstdio>
#include <iostream>
#include <cmath>

using namespace std;
int main()
{
    int sw;
    int stage;

    cout << "how many sw ? ";
    cin >> sw;

    cout << "which stage  ? ";
    cin >> stage;

    int r = sw/(pow(2,stage)); // change every r
    int d = 1; //starting dir is changed at first iteration

    for (int i = 0; i<sw; i++)
    {
	int x = i%r;

	if (x==0) d = 1-d;
	
	cout << "sw n." << i << " is connected in dir " << d << endl;
    }

    return 0;
}

