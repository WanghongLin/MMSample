
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

static std::string countAndSay(int n)
{
	std::stringstream out;

	if (n <= 0) {
		out << 0;
	} else if (n == 1) {
		out << 1;
	} else {
		std::string previous = countAndSay(n-1);
		std::cout << previous << std::endl;
		int count = 0;
		for (std::string::const_iterator it = previous.begin(); it < previous.end(); ++it) {
			if (it == previous.begin()) {
				count++;
			} else if (*it != *(it-1)) {
				out << count << *(it-1);
				count = 1;
			} else {
				count++;
			}

			if (it == (previous.end()-1)) {
				out << count << *(previous.end()-1);
			}
		}
	}

	return out.str();
}

int main(int argc, const char** argv)
{
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " number" << std::endl;
		std::exit(0);
	}
	std::cout << countAndSay(std::atoi(argv[1])) << std::endl;
	return 0;
}
