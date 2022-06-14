#pragma once
#include "socket_client.h"

namespace jwx {
	class HttpClient : public SocketClientUser {
		public:
			virtual void OnConnectionStarted(const SocketClient& client);
			virtual void OnDataReceived(const SocketClient& client, const std::vector<uint8_t>& data);
			virtual void OnSocketClosed(const SocketClient& client);
	};
}