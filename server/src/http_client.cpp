#include "http_client.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "http/http_request.h"
#include "http/http_response.h"

namespace jwx {
	void HttpClient::OnConnectionStarted(const SocketClient& client) {

	}

	void HttpClient::OnSocketClosed(const SocketClient& client) {

	}

	void HttpClient::OnDataReceived(const SocketClient& client, const std::vector<uint8_t>& data) {
		http::HTTPRequest req;
		req.Load(data);

		std::cout << "[" << req.Version() << "][" << static_cast<std::string>(req.Method()) << "] " << req.Url().Uri() << std::endl;

		std::string response_content = "Echo!";

		http::HTTPResponse response;
		response.StatusCode(500);
		response.StatusText("Inernal Server Error");
		response.Version(req.Version());
		response.Content(std::vector<uint8_t>(response_content.begin(), response_content.end()));
		response.ContentType("text/plain");

		if (req.GetHeader("Connection") == "keep-alive") {
			response.SetHeader("Connection", "keep-alive");
			response.SetHeader("Keep-Alive", "max=5, timeout=5"); //TODO: Use SOCKETCLIENT_READ_TIMEOUT
		} else if (req.GetHeader("connection") == "keep-alive") {
			response.SetHeader("connection", "keep-alive");
			response.SetHeader("keep-alive", "max=5, timeout=5"); //TODO: Use SOCKETCLIENT_READ_TIMEOUT
		}

		std::cout << "[" << response.Version() << "][" << response.StatusCode() << "] " << response.StatusText() << std::endl;

		client.write(response);
	}
}