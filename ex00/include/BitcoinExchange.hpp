#ifndef BITCOINExchange_HPP
#define BITCOINExchange_HPP

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
	
	void process_file(std::string file);
	void print_all(std::map<std::string, double> &db);
};

#endif