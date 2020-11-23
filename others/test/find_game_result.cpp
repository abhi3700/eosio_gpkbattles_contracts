#include <iostream>
#include <string>

using std::string;

// Function that return count of the given  
// character in the string 
int count(string s, char c) 
{ 
    // Count variable 
    int res = 0; 
  
    for (int i=0;i<s.length();i++) 
  
        // checking character in string 
        if (s[i] == c) 
            res++; 
  
    return res; 
}

int main() {
	string s = "998ee6a862294ef13b606e37b57e0a384bf177c633f0939054ff69d16958d740";
	// logic-2

	size_t found_a = s.find("a");
	size_t found_b = s.find("b");

	if(found_a < found_b) {
		std::cout << "A wins" << "\n";
		std::cout << found_a << "\n";
		std::cout << found_b << "\n";
	}
	else if (found_a > found_b) {
		std::cout << "B wins" << "\n";
		std::cout << found_a << "\n";
		std::cout << found_b << "\n";
	} else {
		std::cout << "None wins" << "\n";
		std::cout << found_a << "\n";
		std::cout << found_b << "\n";
	}

	// logic-2

	// size_t count_a = count(s, 'a');
	// size_t count_b = count(s, 'b');

	// if(count_a > count_b) {
	// 	std::cout << "A wins" << "\n";
	// 	std::cout << count_a << "\n";
	// 	std::cout << count_b << "\n";
	// }
	// else if (count_a < count_b) {
	// 	std::cout << "B wins" << "\n";
	// 	std::cout << count_a << "\n";
	// 	std::cout << count_b << "\n";
	// } else {
	// 	std::cout << "None wins" << "\n";
	// 	std::cout << count_a << "\n";
	// 	std::cout << count_b << "\n";
	// }

	return 0;
}