#include "RPN.hpp"

RPNException::RPNException(const std::string &message) : _message(message) {}

const char *RPNException::what() const throw()
{
	return _message.c_str();
}

RPNException::~RPNException() throw() {}

std::string trim(const std::string &value)
{
	std::string::size_type start = value.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	std::string::size_type end = value.find_last_not_of(" \t\r\n");
	return value.substr(start, end - start + 1);
}

bool is_number(const std::string &token)
{
	if (token.empty())
		return false;
	std::string trimmed = trim(token);
	if (trimmed.empty())
		return false;
	std::istringstream iss(trimmed);
	double value;
	char remaining;
	if ((iss >> value) && !(iss >> remaining))
	{
		// if (value < 0 || value >= 10)
		// 	return false;
		return true;
	}
	return false;
}

bool is_operator(const std::string &token)
{
	std::string trimmed = trim(token);
	return trimmed == "+" || trimmed == "-" || trimmed == "*" || trimmed == "/";
}

double apply_operator(double a, double b, const std::string &op)
{
	if (op == "+")
		return a + b;
	else if (op == "-")
		return a - b;
	else if (op == "*")
		return a * b;
	else if (op == "/")
	{
		if (b == 0.0)
			throw RPNException("Error: division by zero");
		return a / b;
	}
	throw RPNException("Error: unknown operator");
}

RPN::RPN()
{
}

RPN::RPN(RPN const &other)
{
	*this = other;
}

RPN::~RPN()
{
}

RPN &RPN::operator=(RPN const &other)
{
	if (this != &other)
	{
		_stack = other._stack;
	}
	return *this;
}

void RPN::evaluate(const std::string &expression)
{
	std::istringstream iss(expression);
	std::string token;

	while (iss >> token)
	{
		token = trim(token);
		if (token.empty())
			continue;

		if (is_number(token))
		{
			std::istringstream num_stream(token);
			double value;
			if (!(num_stream >> value))
				throw RPNException("Error: invalid number");
			if (value < 0 || value >= 10)
				throw RPNException("Error: number must be between 0 and 9");
			_stack.push(value);
		}
		else if (is_operator(token))
		{
			if (_stack.size() < 2)
				throw RPNException("Error: insufficient operands");
			
			double b = _stack.top(); _stack.pop();
			double a = _stack.top(); _stack.pop();
			double result = apply_operator(a, b, token);
			_stack.push(result);
		}
		else
		{
			throw RPNException("Error: invalid token");
		}
	}

	if (_stack.size() != 1)
		throw RPNException("Error: invalid expression");
}

double RPN::getResult()
{
	if (_stack.empty())
		throw RPNException("Error: no result");
	return _stack.top();
}
