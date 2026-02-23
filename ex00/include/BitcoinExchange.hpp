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
	void get_datab();
	void get_result(std::map<std::string, double>::iterator &sourceval);
public:
	
	BitcoinExchange(std::string file);
	BitcoinExchange(const BitcoinExchange& other);
	BitcoinExchange& operator=(const BitcoinExchange& other);
	~BitcoinExchange();
	
	void fill_db();
	double look_up_stuff(std::string stuff);
	void process_file(std::string file);
	void print_all(std::string file);
};

#endif