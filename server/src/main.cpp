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
#include <semaphore>
#include <thread>

#define JWX_RECVSIZE 1024 * 5
#define JWX_KEEPALIVE_SECODNS 5

const std::string target_content_path_arg("--content-path");
const std::string help_arg("--help");
const std::string port_arg("--port");
bool alive = true;
std::vector<std::weak_ptr<jwx::SocketClient>> clients;
std::shared_ptr<jwx::cache::CacheMgr> cache;

std::binary_semaphore cleanup_semaphore(0);

int main(int argc, char** argv) {
	std::string target_content_path = "./content";
	int target_port = 4955;

	bool found_target_arg = false;
	bool applied_target_arg = false;

	bool found_port_arg = false;
	bool applied_port_arg = false;
	for(int i = 0; i < argc; i++) {
		if (help_arg == argv[i]) {
			std::cout << "jwx (John's Webserver eXperiment) " << JWX_VERSION << std::endl;
			std::cout << "Usage: " << argv[0] << " [" << target_content_path_arg << " ./content] [" << port_arg << " 4955]" << std::endl;
			return 0;
		}
		
		if (target_content_path_arg == argv[i]) {
			if (applied_target_arg || found_target_arg) {
				std::cerr << "Error: multiple '" << target_content_path_arg << "' instruction detected." << std::endl;
				return 1;
			}

			if (found_port_arg) {
				std::cerr << "Error: no value provided for argument '" << port_arg << "'" << std::endl;
				return 1;
			}

			found_target_arg = true;
			continue;
		}

		if (found_target_arg) {
			target_content_path = argv[i];
			found_target_arg = false;
			applied_target_arg = true;
			continue;
		}

		if (port_arg == argv[i]) {
			if (applied_port_arg || found_port_arg) {
				std::cerr << "Error: multiple '" << port_arg << "' instruction detected." << std::endl;
				return 1;
			}

			if (found_target_arg) {
				std::cerr << "Error: no value provided for argument '" << target_content_path_arg << "'" << std::endl;
				return 1;
			}

			found_port_arg = true;
			continue;
		}

		if (found_port_arg) {
			int port = std::atoi(argv[i]);
			if (port == 0) {
				std::cerr << "Error: port '" << argv[i] << "' is an invalid value." << std::endl;
				return 1;
			}
			target_port = port;
			applied_port_arg = true;
			found_port_arg = false;
			continue;
		}
	}

	if (found_target_arg && !applied_target_arg) {
		std::cerr << "Error: no path provided for argument '" << target_content_path_arg << "'" << std::endl;
		return 1;
	}

	if (!std::filesystem::exists(target_content_path)) {
		std::cerr << "Error: content path '" << target_content_path << "' does not exist." << std::endl;
		return 1;
	}

	auto content_path = std::filesystem::canonical(target_content_path);

	if (!std::filesystem::is_directory(content_path)) {
		std::cerr << "Error: content path '" << target_content_path << "' is not a directory." << std::endl;
		return 1;
	}

	auto port = htons(target_port);

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
	bind_address.sin_port = port;
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

	cache = std::make_shared<jwx::cache::CacheMgr>(content_path);

	std::thread cleanup_thread([]() -> void {
		while(alive) {
			cache->Cleanup();

			cleanup_semaphore.try_acquire_for(std::chrono::seconds(60));
		}
	});

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


	cleanup_semaphore.release();
	cleanup_thread.join();

	return 0;
}