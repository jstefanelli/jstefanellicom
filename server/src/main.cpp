#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "signal.h"
#include "unistd.h"
#include "fcntl.h"
#include "poll.h"
#include <iostream>
#include <vector>
#include "socket_client.h"
#include "http_client.h"
#include "cache/cache_mgr.h"

#define JWX_RECVSIZE 1024 * 5
#define JWX_KEEPALIVE_SECODNS 5

bool alive = true;
std::vector<std::weak_ptr<jwx::SocketClient>> clients;
std::shared_ptr<jwx::cache::CacheMgr> cache;


int main(int argc, char** argv) {
	auto listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_socket < 0) {
		std::cerr << "[Main] Failed to create socket." << std::endl;
		return 1;
	}

	auto flags = fcntl(listen_socket, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(listen_socket, F_SETFL, flags);

	sockaddr_in bind_address{};
	bind_address.sin_family = AF_INET;
	bind_address.sin_port = htons(4955);
	bind_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_socket, reinterpret_cast<sockaddr*>(&bind_address), sizeof(bind_address)) < 0) {
		std::cerr << "[Main] Failed to bind socket." << std::endl;
		return 1;
	}

	if(listen(listen_socket, 5) < 0) {
		std::cerr << "[Main] Listen failed." << std::endl;
		return 1;
	}

	signal(SIGINT, [](int sig) -> void {
		alive = false;
	});

	cache = std::make_shared<jwx::cache::CacheMgr>("/Users/jps/Source/website2/ui/build/");
	
	while (alive) {
		sockaddr_in client_address{};
		socklen_t address_size = sizeof(client_address);

		auto client_socket = accept(listen_socket, reinterpret_cast<sockaddr*>(&client_address), &address_size);
		if (client_socket < 0) {
			auto err = errno;
			if (err == EWOULDBLOCK) {
				pollfd poll_fd{};
				poll_fd.fd = listen_socket;
				poll_fd.events = POLLIN;

				poll(&poll_fd, 1, -1);
			} else {
				std::cerr << "[Main] Invalid error on accept" << std::endl;
				break;
			}
		} else {
			auto client = std::make_shared<jwx::SocketClient>(client_socket);
			client->appendUser(std::make_shared<jwx::HttpClient>(cache));

			clients.push_back(std::weak_ptr<jwx::SocketClient>(client));

			jwx::SocketClient::StartClient(std::move(client));
		}
	}

	std::cerr << "[Main] Exiting..." << std::endl;

	close(listen_socket);

	for(auto i = clients.begin(); i < clients.end(); i++) {
		auto locked = i->lock();

		if (locked != nullptr) {
			locked->stopThread();
			locked->joinThread();
		}
	}

	return 0;
}