#include "http/http_message.h"
#include <sstream>
#include "utils.h"

namespace jwx::http {
	void HTTPMessage::Load(const std::vector<char> data) {
		std::string currentLine;
		auto it = data.begin();

		while(it != data.end() && *it != '\n') {
			currentLine += *it;
			it++;
		}
		it++;

		ParseFirstLine(currentLine);

		std::stringstream lines;
		std::string lastLine;

		do {
			while(it != data.end() && *it != '\n') {
				lines.put(*it);
				it++;
			}
			it++;

			lastLine = lines.str();
			lines.str("");
			Utils::trim(lastLine);
			if (lastLine.length() > 0) {
				ParseHeader(lastLine);
			}
		} while(lastLine.length() > 0);

		while(it != data.end()) {
			content.push_back(*it);
			it++;
		}
	}

	bool HTTPMessage::ParseHeader(const std::string& line) {
		auto idx = line.find_first_of(':');
		if (idx == std::string::npos) {
			return false;
		}

		std::string name = line.substr(0, idx);
		std::string value = line.substr(idx);

		Utils::trim(name);
		Utils::trim(value);

		headers[name] = value;
		return true;
	}

	bool HTTPMessage::HasHeader(std::string header) const {
		Utils::trim(header);
		return headers.count(header) > 0;
	}

	std::optional<std::string> HTTPMessage::GetHeader(std::string header) const {
		Utils::trim(header);
		if (!HasHeader(header)) {
			return std::nullopt;
		}

		return headers.at(header);
	}

	void HTTPMessage::SetHeader(std::string name, std::string value) {
		Utils::trim(name);
		Utils::trim(value);

		if (name.length() == 0 || value.length() == 0) {
			return;
		}

		headers[name] = value;
	}

	const std::unordered_map<std::string, std::string>& HTTPMessage::Headers() const {
		return headers;
	}

	std::unordered_map<std::string, std::string>& HTTPMessage::Headers() {
		return headers;
	}

	std::optional<std::string> HTTPMessage::ContentType() const {
		std::string header_name = "Content-Type";
		if (!HasHeader(header_name)) {
			header_name = "content-type";
		}

		return GetHeader(header_name);
	}

	void HTTPMessage::ContentType(std::string type) {
		std::string header_name = "Content-Type";
		if (HasHeader("content-type")) {
			header_name = "content-type";
		}

		SetHeader(header_name, type);
	}

	std::optional<uint64_t> HTTPMessage::ContentLength() const {
		std::string header_name = "Content-Length";
		if (!HasHeader(header_name)) {
			header_name = "content-length";
		}

		auto size = GetHeader(header_name);
		if (size == std::nullopt || size.value().length() == 0) {
			return std::nullopt;
		}

		return std::stoull(size.value());
	}

	void HTTPMessage::ContentLength(uint64_t length) {
		std::string header_name = "Content-Length";
		if (HasHeader("content-length")) {
			header_name = "content-length";
		}

		SetHeader(header_name, std::to_string(length));
	}

	std::optional<std::string> HTTPMessage::UserAgent() const {
		std::string header_name = "User-Agent";
		if (!HasHeader(header_name)) {
			header_name = "user-agent";
		}

		return GetHeader(header_name);
	}

	void HTTPMessage::UserAgent(std::string userAgent) {
		std::string header_name = "User-Agent";
		if (HasHeader("user-agent")) {
			header_name = "user-agent";
		}

		SetHeader(header_name, userAgent);
	}

	std::optional<std::string> HTTPMessage::Server() const {
		std::string header_name = "Server";
		if (!HasHeader(header_name)) {
			header_name = "server";
		}

		return GetHeader(header_name);
	}

	void HTTPMessage::Server(std::string server) {
		std::string header_name = "Server";
		if (HasHeader("server")) {
			header_name = "server";
		}

		SetHeader(header_name, server);
	}

	const std::vector<uint8_t>& HTTPMessage::Content() const {
		return content;
	}

	void HTTPMessage::Content(std::vector<uint8_t> data, bool setContentLength) {
		content = std::move(data);

		if (setContentLength) {
			ContentLength(data.size());
		}
	} 
}