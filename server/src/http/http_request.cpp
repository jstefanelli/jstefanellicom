#include "http/http_request.h"

namespace jwx::http {
	URL::URL(std::string url) {
		Utils::trim(url);
		auto question_idx = url.find_first_of('?');
		if (question_idx == std::string::npos) {
			uri = url;
			return;
		}

		uri = url.substr(0, question_idx);
		if (question_idx == url.length() - 1) {
			return;
		}
		auto query_string = url.substr(question_idx + 1);
		do {
			auto eq_idx = query_string.find_first_of('=');
			auto and_idx = query_string.find_first_of('&');

			if (eq_idx == std::string::npos || eq_idx == 0) {
				break;
			}

			if (and_idx == eq_idx + 1) {
				continue;
			}

			std::string name = query_string.substr(0, eq_idx);
			std::string val = and_idx == std::string::npos ? query_string.substr(eq_idx + 1) : query_string.substr(eq_idx + 1, and_idx);

			Utils::trim(name);
			Utils::trim(val);

			if(name.length() > 0 && val.length() > 0) {
				queries[name] = val;
			}

			if (and_idx == std::string::npos || and_idx == query_string.length()) {
				break;
			} else {
				query_string = query_string.substr(and_idx);
			}
		} while(query_string.length() > 0);
	}

	std::string URL::Uri() const {
		return uri;
	}

	const std::unordered_map<std::string, std::string>& URL::Queries() const {
		return queries;
	}

	void HTTPRequest::ParseFirstLine(std::string line) {
		auto spc_idx = line.find_first_of(' ');
		if (spc_idx == std::string::npos) {
			method = HTTPMethod::GET;
		}else{
			method = HTTPMethod(line.substr(0, spc_idx));
			line = line.substr(spc_idx);
			Utils::ltrim(line);
		}

		spc_idx = line.find_first_of(' ');
		if (spc_idx == std::string::npos) {
			url = URL("/");
		} else {
			url = URL(line.substr(0, spc_idx));
			line = line.substr(spc_idx);
		}

		version = line;
		Utils::trim(version);
	}

	HTTPMethod HTTPRequest::Method() const {
		return method;
	}

	void HTTPRequest::Method(HTTPMethod m) {
		method = m;
	}

	const URL& HTTPRequest::Url() const {
		return url;
	}

	void HTTPRequest::Url(URL u) {
		url = u;
	}

	std::string HTTPRequest::Version() const {
		return version;
	}

	void HTTPRequest::Version(std::string v) {
		version = v;
	}
}