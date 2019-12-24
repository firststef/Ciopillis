#pragma once
#include <exception>
#include <string>

struct MissingDependencyException : public std::exception {//TODO: de scos exceptiile
	std::string msg;
	
	MissingDependencyException(std::string target)
		: msg("Missing dependency: " + target)
	{
	}
	
	const char * what() const override
	{
		return msg.c_str();
	}
};
