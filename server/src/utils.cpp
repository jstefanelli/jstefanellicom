#include "utils.h"

namespace Utils {
	std::string& ltrim(std::string &s)
	{
		auto it = std::find_if(s.begin(), s.end(),
						[](char c) {
							return !std::isspace<char>(c, std::locale::classic());
						});
		s.erase(s.begin(), it);
		return s;
	}
	
	std::string& rtrim(std::string &s)
	{
		auto it = std::find_if(s.rbegin(), s.rend(),
						[](char c) {
							return !std::isspace<char>(c, std::locale::classic());
						});
		s.erase(it.base(), s.end());
		return s;
	}
	
	std::string& trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	std::string& tolower(std::string &s) {
		std::transform(s.begin(), s.end(), s.begin(), [](auto c) {
			return std::tolower(c);
		});
		return s;
	}

	std::string& toupper(std::string &s) {
		std::transform(s.begin(), s.end(), s.begin(), [](auto c) {
			return std::toupper(c);
		});
		return s;
	}
}