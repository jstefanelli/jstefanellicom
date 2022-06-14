#include "http_client.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "http/http_request.h"

namespace jwx {
	void HttpClient::OnConnectionStarted(const SocketClient& client) {

	}

	void HttpClient::OnSocketClosed(const SocketClient& client) {

	}

	void HttpClient::OnDataReceived(const SocketClient& client, const std::vector<char>& data) {
		http::HTTPRequest req;
		req.Load(data);

		std::cout << "[" << req.Version() << "][" << static_cast<std::string>(req.Method()) << "] " << req.Url().Uri() << std::endl;

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