#include "PmergeMe.hpp"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe &other)
{
	(void)other;
}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	(void)other;
	return *this;
}

PmergeMe::~PmergeMe() {}

std::vector<int> PmergeMe::parseArgs(int argc, char **argv)
{
	if (argc < 2)
		throw std::runtime_error("no input sequence provided");

	std::vector<int> sequence;
	sequence.reserve(static_cast<std::size_t>(argc - 1));

	for (int i = 1; i < argc; ++i)
	{
		const char *arg = argv[i];
		if (!arg || *arg == '\0')
			throw std::runtime_error("empty argument");

		for (const char *p = arg; *p; ++p)
		{
			if (*p < '0' || *p > '9')
				throw std::runtime_error("invalid character in input");
		}

		char *end = 0;
		long value = std::strtol(arg, &end, 10);
		if (*end != '\0')
			throw std::runtime_error("invalid integer value");
		if (value <= 0)
			throw std::runtime_error("only positive integers are allowed");
		if (value > INT_MAX)
			throw std::runtime_error("integer value out of range");

		sequence.push_back(static_cast<int>(value));
		if (sequence.size() > 10000u)
			throw std::runtime_error("too many input values (max 10000)");
	}

	return sequence;
}

void PmergeMe::printSequence(const std::vector<int> &sequence)
{
	for (std::size_t i = 0; i < sequence.size(); ++i)
	{
		std::cout << sequence[i];
		if (i + 1 < sequence.size())
			std::cout << ' ';
	}
	std::cout << std::endl;
}

void PmergeMe::fordJohnsonSort(std::vector<int> &sequence)
{
	if (sequence.size() <= 1)
		return;
	if (sequence.size() == 2)
	{
		if (sequence[0] > sequence[1])
			std::swap(sequence[0], sequence[1]);
		return;
	}

	std::vector<int> larger;
	std::vector<int> smaller;
	larger.reserve((sequence.size() + 1) / 2);
	smaller.reserve(sequence.size() / 2);

	std::size_t i = 0;
	for (; i + 1 < sequence.size(); i += 2)
	{
		int a = sequence[i];
		int b = sequence[i + 1];
		if (a > b)
			std::swap(a, b);
		smaller.push_back(a);
		larger.push_back(b);
	}

	bool hasLeftover = (i < sequence.size());
	int leftover = 0;
	if (hasLeftover)
		leftover = sequence[i];

	fordJohnsonSort(larger);

	std::vector<int> mainChain;
	mainChain.reserve(sequence.size());
	mainChain.insert(mainChain.end(), larger.begin(), larger.end());

	for (std::size_t j = 0; j < smaller.size(); ++j)
	{
		std::vector<int>::iterator pos = std::lower_bound(mainChain.begin(), mainChain.end(), smaller[j]);
		mainChain.insert(pos, smaller[j]);
	}

	if (hasLeftover)
	{
		std::vector<int>::iterator pos = std::lower_bound(mainChain.begin(), mainChain.end(), leftover);
		mainChain.insert(pos, leftover);
	}

	sequence.swap(mainChain);
}

void PmergeMe::run(int argc, char **argv)
{
	std::vector<int> sequence = parseArgs(argc, argv);
	std::vector<int> original(sequence);

	std::cout << "Before: ";
	printSequence(original);

	fordJohnsonSort(sequence);

	std::cout << "After:  ";
	printSequence(sequence);
}