#include "http_client.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include "http/http_request.h"
#include "http/http_response.h"

namespace jwx {
	HttpClient::HttpClient(std::shared_ptr<cache::CacheMgr> cache, std::string server_name) : cache(cache), server_name(std::move(server_name)) {

	}

	void HttpClient::OnConnectionStarted(const SocketClient& client) {

	}

	void HttpClient::OnSocketClosed(const SocketClient& client) {

	}

	void HttpClient::OnDataReceived(const SocketClient& client, const std::vector<uint8_t>& data) {
		http::HTTPRequest req;
		req.Load(data);

		std::cout << "[" << req.Version() << "][" << static_cast<std::string>(req.Method()) << "] " << req.Url().Uri() << std::endl;

		http::HTTPResponse response;

		response.Version(req.Version());

		if (req.GetHeader("Connection") == "keep-alive") {
			response.SetHeader("Connection", "keep-alive");
			response.SetHeader("Keep-Alive", "max=5, timeout=5"); //TODO: Use SOCKETCLIENT_READ_TIMEOUT
		} else if (req.GetHeader("connection") == "keep-alive") {
			response.SetHeader("connection", "keep-alive");
			response.SetHeader("keep-alive", "max=5, timeout=5"); //TODO: Use SOCKETCLIENT_READ_TIMEOUT
		}
		response.Server(server_name);

		bool requestOk = false;
		bool streamResponse = false;
		std::shared_ptr<cache::Cache> file;
		if ((req.Method() == http::HTTPMethod::GET) || req.Method() == http::HTTPMethod::HEAD) {
			std::filesystem::path path(req.Url().Uri());

			if (!path.has_filename()) {
				path.concat("index.html");
			}

			file = cache->Request(path);

			if (file) {
				requestOk = true;
				response.StatusCode(200);
				response.StatusText("Ok");
				if (file->Data() != nullptr) {
					if (req.Method() == http::HTTPMethod::GET) {
						response.Content(*(file->Data()));
					} else if (req.Method() == http::HTTPMethod::HEAD) {
						response.ContentLength(file->Size());
					}
				} else {
					streamResponse = true;
				}

				if (path.has_extension()) {
					std::string ext = path.extension();
					Utils::tolower(ext);

					if (ext == ".html") {
						response.ContentType("text/html");
					} else if (ext == ".js") {
						response.ContentType("text/javascript");
					} else if (ext == ".css") {
						response.ContentType("text/css");
					} else if (ext == ".txt") {
						response.ContentType("text/plain");
					} else if (ext == ".mp4") {
						response.ContentType("video/mp4");
					} else if (ext == ".mp3") {
						response.ContentType("audio/mp3");
					} else if (ext == ".png") {
						response.ContentType("image/png");
					} else if (ext == ".jpg" || ext == ".jpeg") {
						response.ContentType("image/jpeg");
					} else {
						response.ContentType("application/octet-stream");
					}
				} else {
					response.ContentType("application/octet-stream");
				}
			}
		}

		if (!requestOk) {
			std::string response_content("JWX: File not found");
			response.StatusCode(404);
			response.StatusText("Not Found");
			response.Content(std::vector<uint8_t>(response_content.begin(), response_content.end()));
			response.ContentType("text/plain");
		}

		if (streamResponse && requestOk) {
			std::ifstream stream(file->Path());

			stream.seekg(0, std::ios::end);
			size_t file_size = stream.tellg();
			stream.seekg(0, std::ios::beg);

			response.ContentLength(file_size);


			std::cout << "[" << response.Version() << "][" << response.StatusCode() << "] " << response.StatusText() << " " << file_size << " (streaming...)"  << std::endl;
			client.write(response);
			if (req.Method() == http::HTTPMethod::GET) {
				client.writeStream(stream);
			}
		} else {
			auto length = response.ContentLength();
			std::cout << "[" << response.Version() << "][" << response.StatusCode() << "] " << response.StatusText() << " " << (length.has_value() ? length.value() : 0)  << std::endl;

			client.write(response);
		}
	}
}