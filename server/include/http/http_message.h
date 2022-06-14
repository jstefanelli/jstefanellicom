#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "../utils.h"

namespace jwx::http {
	class HTTPMethod {
	public:
		enum Value : uint8_t {
			GET,
			POST,
			HEAD,
			OPTIONS,
			PUT,
			PATCH,
			DELETE	
		};

		HTTPMethod() : val(GET) {

		};
		constexpr HTTPMethod(Value v) : val(v) {};
		HTTPMethod(std::string v) {
			Utils::trim(v);
			for(auto&c : v) {
				std::toupper(c);
			}

			val = GET;
			if(v == "GET") {
				val = GET;
			} else if(v == "POST") {
				val = POST;
			} else if (v == "HEAD") {
				val = HEAD;
			} else if (v == "OPTIONS") {
				val = OPTIONS;
			} else if (v == "PUT") {
				val = PUT;
			} else if (v == "PATCH") {
				val = PATCH;
			} else if (v == "DELETE") {
				val = DELETE;
			}
		}

		constexpr operator const char*() const {
			switch(val) {
			default:
			case GET:
				return "GET";
			case POST:
				return "POST";
			case HEAD:
				return "HEAD";
			case OPTIONS:
				return "OPTIONS";
			case PUT:
				return "PUT";
			case PATCH:
				return "PATCH";
			case DELETE:
				return "DELETE";
			}
		}

		constexpr operator Value() const { return val; }
		constexpr bool operator==(const HTTPMethod& o) { return o.val == val; }
		constexpr bool operator!=(const HTTPMethod& o) { return o.val != val; }

	private:
		Value val;
	};

	class HTTPMessage {
	protected:
		HTTPMessage() = default;
		std::unordered_map<std::string, std::string> headers;
		std::vector<uint8_t> content;

		virtual void ParseFirstLine(std::string firstLine) = 0;
		virtual bool ParseHeader(const std::string& line);
	public:
		void Load(std::vector<char> data);

		bool HasHeader(std::string header) const;
		std::optional<std::string> GetHeader(std::string header) const;
		void SetHeader(std::string name, std::string value);

		const std::unordered_map<std::string, std::string>& Headers() const;
		std::unordered_map<std::string, std::string>& Headers();

		std::optional<std::string> ContentType() const;
		void ContentType(std::string type);

		std::optional<uint64_t> ContentLength() const;
		void ContentLength(uint64_t length);

		std::optional<std::string> UserAgent() const;
		void UserAgent(std::string userAgent);

		std::optional<std::string> Server() const;
		void Server(std::string server);

		const std::vector<uint8_t>& Content() const;
		void Content(std::vector<uint8_t> content, bool setContentLength = true);
	};
}