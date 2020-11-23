#include <iostream>
#include <vector>

int main() {
	std::vector<int> v1 = {1, 42, 56, 34};
	// auto v2 = v1;

	auto it = v1.begin();
	v1.erase(it + 3);
	// v2.erase(v2.end());
	// v1.erase()

	for(auto&& i : v1) {
		std::cout << i << ", ";
	}
	// for(auto&& i : v2) {
	// 	std::cout << i << ", ";
	// }
	std::cout << "\n";

	return 0;
}



// // CPP program to illustrate 
// // working of erase() function 
// #include <iostream> 
// #include <vector> 
// using namespace std; 

// int main() 
// { 
// 	vector<int> myvector{ 1, 2, 3, 4, 5 }; 
// 	auto it = myvector.begin(); 
// 	myvector.erase(it); 

// 	// Printing the Vector 
// 	// for (auto it = myvector.begin(); it != myvector.end(); ++it) 
// 	// 	cout << ' ' << *it; 
// 	// return 0; 
// } 
