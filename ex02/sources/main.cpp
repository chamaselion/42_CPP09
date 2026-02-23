#include "PmergeMe.hpp"
#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
	try
	{
		PmergeMe::run(argc, argv);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "Error: unknown exception" << std::endl;
		return 1;
	}
	return 0;
}

