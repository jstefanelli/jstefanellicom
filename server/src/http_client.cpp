#include "http_client.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace jwx {
	void HttpClient::OnConnectionStarted(const SocketClient& client) {

	}

	void HttpClient::OnSocketClosed(const SocketClient& client) {

	}

	void HttpClient::OnDataReceived(const SocketClient& client, const std::vector<char>& data) {
		std::string request_method;
		std::string request_url;
		std::string request_version;
		std::unordered_map<std::string, std::string> request_headers;
		std::vector<char> request_content;

		size_t i = 0;

		while(i < data.size() && data[i] == ' ') {
			i++;
		}

		while(i < data.size() && data[i] != ' ' && data[i] != '\n') {
			request_method += data[i];
			i++;
		}

		std::cout << "[HttpClient] Request method: " << request_method << std::endl;

		if (i >= data.size() || data[i] != ' ') {
			std::cout << "[HttpClient] Incomplete request(1)." << std::endl;
			return;
		}
		
		while(i < data.size() && data[i] == ' ') {
			i++;
		}

		while(i < data.size() && data[i] != ' ' && data[i] != '\n') {
			request_url += data[i];
			i++;
		}

		std::cout << "[HttpClient] Request URL: " << request_url << std::endl;

		if (i >= data.size() || data[i] != ' ') {
			std::cout << "[HttpClient] Incomplete request(2)." << std::endl;
			i++;
		}
		
		while(i < data.size() && data[i] == ' ') {
			i++;
		}

		while(i < data.size() && data[i] != ' ' && data[i] != '\n') {
			request_version += data[i];
			i++;
		}

		std::cout << "[HttpClient] Request version: " << request_version << std::endl;

		while(i < data.size() && data[i] == ' ') {
			i++;
		}

		if (data[i] != '\n') {
			std::cerr << "[HttpClient] No new line after HTTP version" << std::endl;
			return;
		}
		i++;

		std::string last_header_name = "";
		do {
			last_header_name = "";
			std::string name = "";
			std::string value = "";
			bool name_done = false;
			while(i < data.size() && data[i] != '\n') {
				if (!name_done && data[i] == ':') {
					name_done = true;
					i++;
					continue;
				}

				if (!name_done) {
					name += std::tolower(data[i]);
				} else {
					value += data[i];
				}
				i++;
			}
			if (name.length() > 0) {
				request_headers[name] = value;
				std::cout << "[HttpClient] Found header " << name << std::endl;
				last_header_name = name;
			}
			i++;
		}while(last_header_name != "");

		if (i < data.size() && data[i] == '\n') {
			i++;
		}

		if (i < data.size()) {
			request_content.insert(request_content.end(), data.begin() + i, data.end());
		}
		std::cout << "[HttpClient] Request effective content length: " << request_content.size() << std::endl;

		std::string response_content = "Echo!";

		std::stringstream response_stream;
		response_stream << "HTTP/1.1 500 Internal Server Error\n";
		response_stream << "Connection: keep-alive\n";
		response_stream << "Keep-Alive: max=5, timeout=" << SOCKETCLIENT_READ_TIMEOUT / 1000 << "\n";
		response_stream << "Content-Type: text/plain\n";
		response_stream << "Content-Length: " << response_content.size() << "\n";
		response_stream << "\n";
		response_stream << response_content << "\n";

		std::string response_str = response_stream.str();

		std::vector<char> response(response_str.begin(), response_str.end());
		client.write(response);
	}
}