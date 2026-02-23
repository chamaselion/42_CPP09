#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>

class PmergeMe
{
private:
	PmergeMe();
	PmergeMe(const PmergeMe& other);
	PmergeMe& operator=(const PmergeMe& other);
	~PmergeMe();

	static std::vector<int> parseArgs(int argc, char **argv);
	static void	printSequence(const std::vector<int> &sequence);
	static void	fordJohnsonSort(std::vector<int> &sequence);

public:
	static void	run(int argc, char **argv);
};

#endif