#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange() {std::cout << "you shall not be using this\n"}

BitcoinExchange::BitcoinExchange(std::string file)
{ }

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
	std::cout << "BitcoinExchange copy constructor called\n";
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
	if (this != &other)
	{
	}
	std::cout << "BitcoinExchange assignment operator called\n";
	return *this;
}

BitcoinExchange::~BitcoinExchange()
{
	std::cout << "BitcoinExchange destructor called\n";
}

void BitcoinExchange::validate(std::string file)
{
	std::string file_2 = "data.csv";

}