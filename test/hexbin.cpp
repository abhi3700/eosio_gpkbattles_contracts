#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::cout;
using std::endl;
using std::vector;

string hextobin(const string &s){
    string out;
    for(auto i: s){
        uint8_t n;
        if(i <= '9' and i >= '0')
            n = i - '0';
        else
            n = 10 + i - 'a';
        for(int8_t j = 3; j >= 0; --j)
            out.push_back((n & (1<<j))? '1':'0');
    }

    return out;
}
  
int main() 
{ 
  
	vector<string> v = {
		"9c2aebe105c54236a2683a02df52304bcfe5d21b448937aa964d51bcc9fb3a2a",
		"0259f2ded20fa7a8d5ccaffc30c3b5c39201efa290a33d7b17ea599bcba2544a",
		"833e49aae1d307f1a914d901110e1cc1629727b1a21025f2068a740cec7272a2",
		"e0854563a9344196a19c87753f0fb05222f1b40cd0dbf564653499eac9d61e30",
		"193ac0a408fd9798869db1d520f35ebb95624f3d05c99ecfa97a61b1bba472c4",
		"133c7a6f2da7bab285a1a1cba1ef03638f1299c1895ad7a297f524054a9fa4a5",
		"f672b6e2297f12b9acccc355b5053b9ce6aab3ab9220007ad1afa2a6910124d7",
		"14be3bd145a0cfda6a2cb2fc5dc12bf2322e792b95fd808b25eac8c3fe1a09a3",
		"467fcef0df64d81876f7692c0140049522d70c6442afd60059e3a3c9c03b3e02",
		"d691e6532619055bd5488cbab0f978d5d9cb7677b9127d764a63ff49eff641ac",
		"913856f7aa906ff43e73a8f5d4c54beb67841ddbc59c3e492d3e1ddfcceec600"

	};
	for(auto&& i : v) {
		string bin_str = hextobin(i);
		string res = "";
			if(bin_str[200] == '0')
				res = "a";
			else if(bin_str[200] == '1')
				res = "b";
		cout << res << ", "; // prints 1111111100010001  
	}
	std::cout << "\n";

    return 0; 
} 