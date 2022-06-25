#pragma once
#include "socket_client.h"
#include "cache/cache_mgr.h"

namespace jwx {
	class HttpClient : public SocketClientUser {
		protected:
			std::shared_ptr<cache::CacheMgr> cache;
			std::string server_name;
		public:
			HttpClient(std::shared_ptr<cache::CacheMgr> cache, std::string server_name);

			virtual void OnConnectionStarted(const SocketClient& client);
			virtual void OnDataReceived(const SocketClient& client, const std::vector<uint8_t>& data);
			virtual void OnSocketClosed(const SocketClient& client);
	};
}