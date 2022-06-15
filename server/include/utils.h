#pragma once
#include <string>
#include <algorithm>
#include <locale>
 
namespace Utils {
	std::string& ltrim(std::string &s);
	std::string& rtrim(std::string &s);
	std::string& trim(std::string &s);

	std::string& tolower(std::string &s);
	std::string& touppoer(std::string &s);
}