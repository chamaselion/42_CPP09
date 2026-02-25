#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <iostream>
#include <algorithm>
#include <map>
#include <stdexcept>

class ValidationException : public std::runtime_error
{
public:
	ValidationException(const std::string &message);
};

class FileOpenException : public ValidationException
{
public:
	FileOpenException(const std::string &message);
};

class BadInputException : public ValidationException
{
public:
	BadInputException(const std::string &message);
};

class InvalidDateException : public ValidationException
{
public:
	InvalidDateException(const std::string &message);
};

class InvalidValueException : public ValidationException
{
public:
	InvalidValueException(const std::string &message);
};


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