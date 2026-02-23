#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <cctype>

namespace
{
class ValidationException : public std::runtime_error
{
public:
	ValidationException(const std::string &message) : std::runtime_error(message) {}
};

class FileOpenException : public ValidationException
{
public:
	FileOpenException(const std::string &message) : ValidationException(message) {}
};

class BadInputException : public ValidationException
{
public:
	BadInputException(const std::string &message) : ValidationException(message) {}
};

class InvalidDateException : public ValidationException
{
public:
	InvalidDateException(const std::string &message) : ValidationException(message) {}
};

class InvalidValueException : public ValidationException
{
public:
	InvalidValueException(const std::string &message) : ValidationException(message) {}
};

std::string trim(const std::string &value)
{
	std::string::size_type start = value.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	std::string::size_type end = value.find_last_not_of(" \t\r\n");
	return value.substr(start, end - start + 1);
}

bool is_leap_year(int year)
{
	return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

std::string to_string_int(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

void validate_date(const std::string &date, int line_number)
{
	if (date.size() != 10 || date[4] != '-' || date[7] != '-')
		throw InvalidDateException("Error: invalid date format at line " +
			to_string_int(line_number) +
			": " + date);

	for (int i = 0; i < 10; ++i)
	{
		if (i == 4 || i == 7)
			continue;
		if (!std::isdigit(date[i]))
			throw InvalidDateException("Error: invalid date format at line " +
				to_string_int(line_number) +
				": " + date);
	}

	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	if (year < 1 || month < 1 || month > 12)
		throw InvalidDateException("Error: invalid date at line " +
			to_string_int(line_number) +
			": " + date);

	int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (is_leap_year(year))
		month_days[1] = 29;

	if (day < 1 || day > month_days[month - 1])
		throw InvalidDateException("Error: invalid date at line " +
			to_string_int(line_number) +
			": " + date);
}

double parse_amount(const std::string &raw_value, int line_number)
{
	std::string value = trim(raw_value);
	if (value.empty())
		throw InvalidValueException("Error: empty amount at line " +
			to_string_int(line_number));

	char *end_ptr = NULL;
	errno = 0;
	double amount = std::strtod(value.c_str(), &end_ptr);

	if (value.c_str() == end_ptr || *end_ptr != '\0' || errno == ERANGE || !std::isfinite(amount))
		throw InvalidValueException("Error: invalid amount at line " +
			to_string_int(line_number) +
			": " + value);

	if (amount < 0.0)
		throw InvalidValueException("Error: amount must be non-negative at line " +
			to_string_int(line_number) +
			": " + value);

	if (amount > 1000.0)
		throw InvalidValueException("Error: amount exceeds max (1000) at line " +
			to_string_int(line_number) +
			": " + value);

	return amount;
}

double parse_double_value(const std::string &raw_value, int line_number)
{
	std::string value = trim(raw_value);
	if (value.empty())
		throw InvalidValueException("Error: empty database value at line " +
			to_string_int(line_number));

	char *end_ptr = NULL;
	errno = 0;
	double parsed = std::strtod(value.c_str(), &end_ptr);

	if (value.c_str() == end_ptr || *end_ptr != '\0' || errno == ERANGE || !std::isfinite(parsed))
		throw InvalidValueException("Error: invalid database value at line " +
			to_string_int(line_number) +
			": " + value);

	return parsed;
}

void validate_input_line(const std::string &line, int line_number)
{
	std::string::size_type pipe_pos = line.find('|');
	if (pipe_pos == std::string::npos || line.find('|', pipe_pos + 1) != std::string::npos)
		throw BadInputException("Error: invalid line format at line " +
			to_string_int(line_number) +
			": " + line);

	std::string date = trim(line.substr(0, pipe_pos));
	std::string amount = trim(line.substr(pipe_pos + 1));

	validate_date(date, line_number);
	parse_amount(amount, line_number);
}
}

BitcoinExchange::BitcoinExchange() {}

void BitcoinExchange::fill_db()
{
	const std::string db_file = "data.csv";
	std::ifstream input(db_file.c_str());
	if (!input.is_open())
		throw FileOpenException("Error: could not open database file: " + db_file);

	_db.clear();

	std::string line;
	int line_number = 0;
	if (!std::getline(input, line))
		throw BadInputException("Error: database file is empty");
	++line_number;

	if (trim(line) != "date,exchange_rate")
		throw BadInputException("Error: invalid database header");

	while (std::getline(input, line))
	{
		++line_number;
		if (trim(line).empty())
			throw BadInputException("Error: empty database line at line " + to_string_int(line_number));

		std::string::size_type comma_pos = line.find(',');
		if (comma_pos == std::string::npos || line.find(',', comma_pos + 1) != std::string::npos)
			throw BadInputException("Error: invalid database line format at line " +
				to_string_int(line_number) +
				": " + line);

		std::string date = trim(line.substr(0, comma_pos));
		std::string value = trim(line.substr(comma_pos + 1));

		validate_date(date, line_number);
		_db[date] = parse_double_value(value, line_number);
	}

}

void BitcoinExchange::print_all(std::string file)
{
	std::ifstream input(file.c_str());
	if (!input.is_open())
		throw FileOpenException("Error: could not open input file: " + file);

	std::string line;
	if (!std::getline(input, line))
		throw BadInputException("Error: input file is empty");

	while (std::getline(input, line))
	{
		if (trim(line).empty())
			continue;

		std::string::size_type pipe_pos = line.find('|');
		std::string date = trim(line.substr(0, pipe_pos));
		std::string amount_token = trim(line.substr(pipe_pos + 1));
		double amount = parse_amount(amount_token, 0);

		std::map<std::string, double>::iterator it = _db.lower_bound(date);
		if (it == _db.end() || it->first != date)
		{
			if (it == _db.begin())
				throw BadInputException("Error: no exchange rate available for date " + date);
			--it;
		}

		double result = amount * it->second;
		std::cout << date << " => " << amount_token << " = " << result << std::endl;
	}
}

BitcoinExchange::BitcoinExchange(std::string file)
{
	validate(file);
	fill_db();
	print_all(file);
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
	*this = other;
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
	if (this != &other)
	{
		_db = other._db;
	}
	return *this;
}

BitcoinExchange::~BitcoinExchange()
{
}

void validate_db(std::string db_file)
{
	if (db_file.size() < 4 || db_file.substr(db_file.size() - 4) != ".csv")
		throw BadInputException("Error: database file must be a .csv file: " + db_file);

	std::ifstream input(db_file.c_str());
	if (!input.is_open())
		throw FileOpenException("Error: could not open database file: " + db_file);

	std::string line;
	int line_number = 0;
	if (!std::getline(input, line))
		throw BadInputException("Error: database file is empty");
	++line_number;

	if (trim(line) != "date,exchange_rate")
		throw BadInputException("Error: invalid database header");

	while (std::getline(input, line))
	{
		++line_number;
		if (trim(line).empty())
			throw BadInputException("Error: empty database line at line " + to_string_int(line_number));

		std::string::size_type comma_pos = line.find(',');
		if (comma_pos == std::string::npos || line.find(',', comma_pos + 1) != std::string::npos)
			throw BadInputException("Error: invalid database line format at line " +
				to_string_int(line_number) +
				": " + line);

		std::string date = trim(line.substr(0, comma_pos));
		std::string value = trim(line.substr(comma_pos + 1));

		validate_date(date, line_number);
		parse_double_value(value, line_number);
	}

}
void validate_inp(std::string inp_file)
{
	std::ifstream input(inp_file.c_str());
	if (!input.is_open())
		throw FileOpenException("Error: could not open input file: " + inp_file);

	std::string line;
	int line_number = 0;
	if (!std::getline(input, line))
		throw BadInputException("Error: input file is empty");
	++line_number;

	std::string first = trim(line);
	if (first != "date | value" && first != "date | amount_of_btc")
		validate_input_line(line, line_number);

	while (std::getline(input, line))
	{
		++line_number;
		if (trim(line).empty())
			throw BadInputException("Error: empty line at line " + to_string_int(line_number));
		validate_input_line(line, line_number);
	}

}

void BitcoinExchange::validate(std::string inp_file)
{
	std::string db_file = "data.csv";
	validate_db(db_file);
	validate_inp(inp_file);

}