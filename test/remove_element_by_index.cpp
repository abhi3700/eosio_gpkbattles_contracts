#include <iostream>
#include <vector>
#include <algorithm>


int main() {
	std::vector<int> v {432, 4543, 767, 234, 7676, 878};

	int rand_index = 2;

	v.erase(std::find(v.begin(), v.end(), v[rand_index]));

	for(auto&& i : v) {
		std::cout << i << "\n";
	}
	return 0;
}