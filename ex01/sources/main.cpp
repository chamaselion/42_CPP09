#include "RPN.hpp"
#include <iostream>

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: invalid number of arguments" << std::endl;
		return 1;
	}

	try
	{
		RPN rpn;
		rpn.evaluate(av[1]);
		double result = rpn.getResult();
		std::cout << result << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
