#ifndef RPN_HPP
#define RPN_HPP

#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <exception>

class RPNException : public std::exception
{
private:
	std::string _message;
public:
	RPNException(const std::string &message);
	virtual const char *what() const throw();
	virtual ~RPNException() throw();
};

class RPN
{
private:
	std::stack<double> _stack;
	
public:
	RPN();
	RPN(RPN const &other);
	RPN &operator=(RPN const &other);
	~RPN();
	
	void evaluate(const std::string &expression);
	double getResult();
};

#endif
