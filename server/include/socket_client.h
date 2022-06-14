#pragma once
#include <vector>
#include <memory>
#include "pthread.h"

#define SOCKETCLIENT_WRITE_TIMEOUT 2000
#define SOCKETCLIENT_READ_TIMEOUT 5000
#define SOCKETCLIENT_READ_BUFFER_SIZE 1024 * 16

namespace jwx {

	class SocketClient;

	class SocketClientUser {
	public:
		virtual void OnConnectionStarted(const SocketClient& client) = 0;
		virtual void OnDataReceived(const SocketClient& client, const std::vector<uint8_t>& data) = 0;
		virtual void OnSocketClosed(const SocketClient& client) = 0;
	};

	class SocketClient {
	protected:
		static void* SocketClientStart(void* data);
		bool alive;
		int socket;
		pthread_t thread;
		std::vector<std::shared_ptr<SocketClientUser>> users;

		void* run();
	public:
		explicit SocketClient(int socket);
		SocketClient(const SocketClient&) = delete;
		SocketClient& operator=(const SocketClient&) = delete;


		static void StartClient(std::shared_ptr<SocketClient> client);
		void appendUser(std::shared_ptr<SocketClientUser> user);
		bool write(const std::vector<uint8_t> &data) const;
		void stopThread();
		void joinThread() const;
	};
}