#pragma once

// std includes
#include <string>


class ErrorHandler {
public:
	ErrorHandler(std::string msg);

	std::string what() const;

private:
	std::string message;
};
