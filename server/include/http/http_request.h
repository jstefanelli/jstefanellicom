#pragma once
#include "http_message.h"

namespace jwx::http {
	class URL {
	protected:
		std::string uri;
		std::unordered_map<std::string, std::string> queries;

	public:
		explicit URL(std::string urlString);

		inline URL() : URL("/") {

		}
		operator std::string() const;

		std::string Uri() const;
		const std::unordered_map<std::string, std::string>& Queries() const;
	};

	class HTTPRequest : public HTTPMessage {
	protected:
		HTTPMethod method;
		URL url;
		std::string version;

		virtual void ParseFirstLine(std::string line) override;
	public:
		HTTPRequest() = default;

		HTTPMethod Method() const;
		void Method(HTTPMethod method);

		const URL& Url() const;
		void Url(URL url);

		std::string Version() const;
		void Version(std::string v);
	};
}