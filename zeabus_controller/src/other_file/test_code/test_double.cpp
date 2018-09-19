#include	<iostream>
#include	<string>
#include	<sstream>

int main(){
	double problem;
	std::ostringstream ss;
	std::cin >> problem;
	ss << problem;
	std::cout << problem << "\n";
	std::cout << " test is " << ss.str() << "\n";
}
