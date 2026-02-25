#include "PmergeMe.hpp"

#include <algorithm>
#include <cstddef>
#include <climits>
#include <ctime>
#include <cstdlib>
#include <deque>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace
{
	void buildJacobsthalInsertionOrder(std::size_t pairCount, std::vector<std::size_t> &order)
	{
		// [JACOBSTHAL SECTION 0] Initialization and trivial-case guard
		order.clear();
		if (pairCount <= 1)
			return;

		std::size_t processed = 1;
		std::size_t jacobPrev = 1;
		std::size_t jacobCurr = 3;

		// [JACOBSTHAL SECTION 1] Build reverse blocks delimited by Jacobsthal values
		while (processed < pairCount)
		{
			std::size_t blockEnd = jacobCurr;
			if (blockEnd > pairCount)
				blockEnd = pairCount;

			// [JACOBSTHAL SECTION 2] Emit current block in reverse index order
			for (std::size_t i = blockEnd; i > processed; --i)
				order.push_back(i - 1);

			// [JACOBSTHAL SECTION 3] Advance Jacobsthal window
			processed = jacobCurr;
			std::size_t jacobNext = jacobCurr + 2 * jacobPrev;
			jacobPrev = jacobCurr;
			jacobCurr = jacobNext;
		}
	}

	void buildJacobsthalInsertionOrderDeque(std::size_t pairCount, std::deque<std::size_t> &order)
	{
		// [JACOBSTHAL SECTION 0] Initialization and trivial-case guard
		order.clear();
		if (pairCount <= 1)
			return;

		std::size_t processed = 1;
		std::size_t jacobPrev = 1;
		std::size_t jacobCurr = 3;

		// [JACOBSTHAL SECTION 1] Build reverse blocks delimited by Jacobsthal values
		while (processed < pairCount)
		{
			std::size_t blockEnd = jacobCurr;
			if (blockEnd > pairCount)
				blockEnd = pairCount;

			// [JACOBSTHAL SECTION 2] Emit current block in reverse index order
			for (std::size_t i = blockEnd; i > processed; --i)
				order.push_back(i - 1);

			// [JACOBSTHAL SECTION 3] Advance Jacobsthal window
			processed = jacobCurr;
			std::size_t jacobNext = jacobCurr + 2 * jacobPrev;
			jacobPrev = jacobCurr;
			jacobCurr = jacobNext;
		}
	}

	void fordJohnsonRecurse(std::vector<int> &seq,
							std::size_t elemCount,
							std::size_t groupSize)
	{
		// [SECTION 0] Base case and level metadata
		std::size_t groupCount = elemCount / groupSize;
		if (groupCount < 2)
			return;

		std::size_t pairCount = groupCount / 2;
		bool hasStraggler = (groupCount % 2 != 0);

		// [SECTION 1] Pair normalization (ensure b_i <= a_i by representative)
		for (std::size_t i = 0; i < pairCount; ++i)
		{
			std::size_t left  = 2 * i * groupSize;
			std::size_t right = (2 * i + 1) * groupSize;
			if (seq[left + groupSize - 1] > seq[right + groupSize - 1])
			{
				for (std::size_t j = 0; j < groupSize; ++j)
					std::swap(seq[left + j], seq[right + j]);
			}
		}

		// [SECTION 2] Recurse on winners at doubled group size
		fordJohnsonRecurse(seq, pairCount * 2 * groupSize, groupSize * 2);

		// [SECTION 3] Build main chain and pending losers
		std::vector<std::size_t> chain;
		chain.reserve(groupCount);
		chain.push_back(0);
		for (std::size_t i = 0; i < pairCount; ++i)
			chain.push_back((2 * i + 1) * groupSize);

		std::vector<std::size_t> pend;
		pend.reserve(pairCount + (hasStraggler ? 1 : 0));
		for (std::size_t i = 1; i < pairCount; ++i)
			pend.push_back(2 * i * groupSize);
		if (hasStraggler)
			pend.push_back(pairCount * 2 * groupSize);

		std::vector<std::size_t> insertionOrder;
		buildJacobsthalInsertionOrder(pairCount + (hasStraggler ? 1 : 0), insertionOrder);

		// [SECTION 4] Insert pending losers with bounded binary search
		for (std::size_t j = 0; j < insertionOrder.size(); ++j)
		{
			std::size_t pairIdx   = insertionOrder[j];
			std::size_t pendStart = pend[pairIdx - 1];
			int value = seq[pendStart + groupSize - 1];

			bool isStragglerPair = hasStraggler && pairIdx == pairCount;
			std::size_t bound = chain.size();
			if (!isStragglerPair)
			{
				std::size_t winnerStart = (2 * pairIdx + 1) * groupSize;
				bound = 0;
				for (std::size_t k = 0; k < chain.size(); ++k)
				{
					if (chain[k] == winnerStart)
					{
						bound = k;
						break;
					}
				}
			}

			std::size_t lo = 0, hi = bound;
			while (lo < hi)
			{
				std::size_t mid = lo + (hi - lo) / 2;
				if (seq[chain[mid] + groupSize - 1] < value)
					lo = mid + 1;
				else
					hi = mid;
			}
			chain.insert(chain.begin() + static_cast<long>(lo), pendStart);
		}

		// [SECTION 5] Materialize chain order into current segment
		std::vector<int> tmp(seq.begin(),
							 seq.begin() + static_cast<long>(elemCount));
		for (std::size_t i = 0; i < chain.size(); ++i)
		{
			std::size_t dst = i * groupSize;
			std::size_t src = chain[i];
			for (std::size_t j = 0; j < groupSize; ++j)
				seq[dst + j] = tmp[src + j];
		}
	}

	void fordJohnsonRecurseDeque(std::deque<int> &seq,
								 std::size_t elemCount,
								 std::size_t groupSize)
	{
		// [SECTION 0] Base case and level metadata
		std::size_t groupCount = elemCount / groupSize;
		if (groupCount < 2)
			return;

		std::size_t pairCount = groupCount / 2;
		bool hasStraggler = (groupCount % 2 != 0);

		// [SECTION 1] Pair normalization (ensure b_i <= a_i by representative)
		for (std::size_t i = 0; i < pairCount; ++i)
		{
			std::size_t left  = 2 * i * groupSize;
			std::size_t right = (2 * i + 1) * groupSize;
			if (seq[left + groupSize - 1] > seq[right + groupSize - 1])
			{
				for (std::size_t j = 0; j < groupSize; ++j)
					std::swap(seq[left + j], seq[right + j]);
			}
		}

		// [SECTION 2] Recurse on winners at doubled group size
		fordJohnsonRecurseDeque(seq, pairCount * 2 * groupSize, groupSize * 2);

		// [SECTION 3] Build main chain and pending losers
		std::deque<std::size_t> chain;
		chain.push_back(0);
		for (std::size_t i = 0; i < pairCount; ++i)
			chain.push_back((2 * i + 1) * groupSize);

		std::deque<std::size_t> pend;
		for (std::size_t i = 1; i < pairCount; ++i)
			pend.push_back(2 * i * groupSize);
		if (hasStraggler)
			pend.push_back(pairCount * 2 * groupSize);

		std::deque<std::size_t> insertionOrder;
		buildJacobsthalInsertionOrderDeque(pairCount + (hasStraggler ? 1 : 0), insertionOrder);

		// [SECTION 4] Insert pending losers with bounded binary search
		for (std::size_t j = 0; j < insertionOrder.size(); ++j)
		{
			std::size_t pairIdx   = insertionOrder[j];
			std::size_t pendStart = pend[pairIdx - 1];
			int value = seq[pendStart + groupSize - 1];

			bool isStragglerPair = hasStraggler && pairIdx == pairCount;
			std::size_t bound = chain.size();
			if (!isStragglerPair)
			{
				std::size_t winnerStart = (2 * pairIdx + 1) * groupSize;
				bound = 0;
				for (std::size_t k = 0; k < chain.size(); ++k)
				{
					if (chain[k] == winnerStart)
					{
						bound = k;
						break;
					}
				}
			}

			std::size_t lo = 0, hi = bound;
			while (lo < hi)
			{
				std::size_t mid = lo + (hi - lo) / 2;
				if (seq[chain[mid] + groupSize - 1] < value)
					lo = mid + 1;
				else
					hi = mid;
			}
			chain.insert(chain.begin() + static_cast<long>(lo), pendStart);
		}

		// [SECTION 5] Materialize chain order into current segment
		std::deque<int> tmp(seq.begin(),
						   seq.begin() + static_cast<long>(elemCount));
		for (std::size_t i = 0; i < chain.size(); ++i)
		{
			std::size_t dst = i * groupSize;
			std::size_t src = chain[i];
			for (std::size_t j = 0; j < groupSize; ++j)
				seq[dst + j] = tmp[src + j];
		}
	}
}

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
	for (std::vector<int>::const_iterator it = sequence.begin(); it != sequence.end(); ++it)
	{
		if (it != sequence.begin())
			std::cout << ' ';
		std::cout << *it;
	}
	std::cout << std::endl;
}

void PmergeMe::printSequence(const std::deque<int> &sequence)
{
	for (std::deque<int>::const_iterator it = sequence.begin(); it != sequence.end(); ++it)
	{
		if (it != sequence.begin())
			std::cout << ' ';
		std::cout << *it;
	}
	std::cout << std::endl;
}

void PmergeMe::fordJohnsonSort(std::vector<int> &sequence)
{
	if (sequence.size() <= 1)
		return;
	fordJohnsonRecurse(sequence, sequence.size(), 1);
}

void PmergeMe::fordJohnsonSort(std::deque<int> &sequence)
{
	if (sequence.size() <= 1)
		return;
	fordJohnsonRecurseDeque(sequence, sequence.size(), 1);
}

void PmergeMe::run(int argc, char **argv)
{
	std::vector<int> sequence = parseArgs(argc, argv);
	std::deque<int> sequenceDeque(sequence.begin(), sequence.end());
	std::vector<int> original(sequence);

	std::cout << "Before: ";
	printSequence(original);

	std::clock_t startTime = std::clock();
	fordJohnsonSort(sequence);
	std::clock_t endTime = std::clock();

	double elapsedMicroseconds = 0.0;
	if (endTime >= startTime)
		elapsedMicroseconds =
			(static_cast<double>(endTime - startTime) * 1000000.0) /
			static_cast<double>(CLOCKS_PER_SEC);

	std::clock_t startTimeDeque = std::clock();
	fordJohnsonSort(sequenceDeque);
	std::clock_t endTimeDeque = std::clock();

	double elapsedMicrosecondsDeque = 0.0;
	if (endTimeDeque >= startTimeDeque)
		elapsedMicrosecondsDeque =
			(static_cast<double>(endTimeDeque - startTimeDeque) * 1000000.0) /
			static_cast<double>(CLOCKS_PER_SEC);

	std::cout << "After:  ";
	printSequence(sequence);
	std::cout << "Time to process a range of " << sequence.size()
			  << " elements with std::vector : "
			  << elapsedMicroseconds << " us" << std::endl;
	std::cout << "Time to process a range of " << sequenceDeque.size()
			  << " elements with std::deque  : "
			  << elapsedMicrosecondsDeque << " us" << std::endl;
}