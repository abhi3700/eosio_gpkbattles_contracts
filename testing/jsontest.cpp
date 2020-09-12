/*

		Compile
			- $ sudo g++ -std=c++17 jsontest.cpp && ./a.out
			
		3.141
		"Niels"
*/

#include "../nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

int main() {
	json j2 = {
	  {"pi", 3.141},
	  {"happy", true},
	  {"name", "Niels"},
	  {"nothing", nullptr},
	  {"answer", {
	    {"everything", 42}
	  }},
	  {"list", {1, 0, 2}},
	  {"object", {
	    {"currency", "USD"},
	    {"value", 42.99}
	  }}
	};

	std::cout << j2["pi"] << "\n";
	std::cout << j2["name"] << "\n";

	return 0;
}