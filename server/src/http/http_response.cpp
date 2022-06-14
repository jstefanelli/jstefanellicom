#include "http/http_response.h"
#include <iostream>

namespace jwx::http {
	void HTTPResponse::ParseFirstLine(std::string line) {
		Utils::trim(line);
		auto spc_idx = line.find_first_of(' ');
		if (spc_idx == std::string::npos) {
			return;
		}

		version = line.substr(0, spc_idx);
		line = line.substr(spc_idx);
		Utils::ltrim(line);

		spc_idx = line.find_first_of(' ');
		if (spc_idx == std::string::npos) {
			return;
		}

		statusCode = std::stoul(line.substr(0, spc_idx));
		line = line.substr(spc_idx);
		Utils::ltrim(line);

		statusText = line;
	}

	uint32_t HTTPResponse::StatusCode() const {
		return statusCode;
	}

	void HTTPResponse::StatusCode(uint32_t code) {
		statusCode = code;
	}

	std::string HTTPResponse::StatusText() const {
		return statusText;
	}

	void HTTPResponse::StatusText(std::string text) {
		statusText = text;
	}

	std::string HTTPResponse::Version() const {
		return version;
	}

	void HTTPResponse::Version(std::string v) {
		version = v;
	}

	HTTPResponse::operator std::vector<uint8_t>() const {
		std::vector<uint8_t> data;
		data.insert(data.end(), version.begin(), version.end());
		data.push_back(' ');
		auto code_str = std::to_string(statusCode);
		data.insert(data.end(), code_str.begin(), code_str.end());
		data.push_back(' ');
		data.insert(data.end(), statusText.begin(), statusText.end());
		data.push_back('\n');

		for(auto it = headers.begin(); it != headers.end(); it++) {
			data.insert(data.end(), it->first.begin(), it->first.end());
			data.push_back(':');
			data.push_back(' ');
			data.insert(data.end(), it->second.begin(), it->second.end());
			data.push_back('\n');
		}
		data.push_back('\n');
		data.insert(data.end(), content.begin(), content.end());

		return data;
	}
}