#include <iostream>
#include <cassert>

#include <noname_tools\tools>

using namespace noname;

int main(int argc, char* argv[])
{
	std::cout << "noname_tools test program\n";

	#ifdef NDEBUG
	std::cout << "WARNING: Not built in debug mode! Asserts won't be triggered!\n";
	#endif

	// TODO: Implement proper tests
	const auto testers = tools::sorted_vector<std::string>({ "ccc","bbb","aaa" });
	assert(testers.size() == 3);
	assert(testers.at(0) == "aaa");

	std::cout << "\nDone.\n";
	std::cout << std::endl;
	return 0;
}