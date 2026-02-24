#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <iostream>
#include <algorithm>
#include <map>


class BitcoinExchange
{
private:
	std::map<std::string, double> _db;
	BitcoinExchange();
	void validate(std::string file);
public:
	
	BitcoinExchange(std::string file);
	BitcoinExchange(const BitcoinExchange& other);
	BitcoinExchange& operator=(const BitcoinExchange& other);
	~BitcoinExchange();
	
	void fill_db();
	void print_all(std::string file);
};

#endif