#pragma once
#include "http_message.h"

namespace jwx::http {
	class HTTPResponse : public HTTPMessage {
	protected:
		uint32_t statusCode;
		std::string statusText;
		std::string version;

		virtual void ParseFirstLine(std::string line) override;
	public:
		HTTPResponse() = default;
		uint32_t StatusCode() const;
		void StatusCode(uint32_t status);

		std::string StatusText() const;
		void StatusText(std::string text);

		std::string Version() const;
		void Version(std::string version);

		operator std::vector<uint8_t>() const;
	};
}