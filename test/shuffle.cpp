#include <iostream>
#include <algorithm>
#include <random>
#include <vector>

// using namespace std;
using std::default_random_engine;

int main()
{
	std::vector<int> A = { 1, 2, 3, 4, 5, 6 };

	shuffle(A.begin(), A.end(), default_random_engine(time(NULL)));
	for (auto &&a : A)
	std::cout << a << " " <<std::endl;
	return 0;
}
