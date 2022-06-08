#include "socket_client.h"
#include "poll.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "unistd.h"
#include "signal.h"
#include <iostream>
#include "fcntl.h"

namespace jwx {
	void* SocketClient::SocketClientStart(void* data) {
		if (data == nullptr)
			return nullptr;

		std::shared_ptr<SocketClient>* client_ptr = reinterpret_cast<std::shared_ptr<SocketClient>*>(data);
		std::shared_ptr<SocketClient> client(*client_ptr);
		delete client_ptr;
		return client->run();
	}

	void SocketClient::StartClient(std::shared_ptr<SocketClient> client) {
		std::shared_ptr<SocketClient>* client_ptr = new std::shared_ptr<SocketClient>(client);

		pthread_create(&client->thread, NULL, &SocketClient::SocketClientStart, reinterpret_cast<void*>(client_ptr));
	}

	SocketClient::SocketClient(int socket) {
		this->socket = socket;
	}

	void* SocketClient::run() {
		auto flags = fcntl(socket, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(socket, F_SETFL, flags);

		alive = true;
		for(auto i = users.begin(); i != users.end(); i++) {
			(*i)->OnConnectionStarted(*this);
		}

		char* buffer = new char[SOCKETCLIENT_READ_BUFFER_SIZE];
		std::vector<char> currentMessage;
		while(alive) {
			std::cout << "[SocketClient] Receiving.." << std::endl;
			ssize_t read = recv(socket, buffer, SOCKETCLIENT_READ_BUFFER_SIZE, 0);
			if (read > 0) {
				std::cout << "[SocketClient] Received " << read << " bytes" << std::endl;
				currentMessage.insert(currentMessage.end(), buffer, buffer + read);
			} else if (read == 0) {
				//Connection closed
				std::cout << "[SocketClient] Connection closed. " << std::endl;
				alive = false;
				break;
			} else if (read < 0) {
				auto err = errno;
				if (err == EWOULDBLOCK) {
					if (currentMessage.size() > 0) {
						std::cout << "[SocketClient] Received " << currentMessage.size() << " bytes" << std::endl;
						//Read operation should be over, send the message to users
						for(auto i = users.begin(); i != users.end(); i++) {
							(*i)->OnDataReceived(*this, currentMessage);
						}
						currentMessage.clear();
					}

					std::cout << "[SocketClient] Waiting.." << std::endl;
					pollfd poll_fd{};
					poll_fd.fd = socket;
					poll_fd.events = POLLIN;
					int poll_result = poll(&poll_fd, 1, SOCKETCLIENT_READ_TIMEOUT);
					if (poll_result > 0) {
						continue;
					} else if (poll_result == 0) {
						alive = false;
						break;
					} else if (poll_result < 0) {
						alive = false;
						std::cerr << "[SocketClient] Read poll error" << std::endl;
						break;
					}
				} else {
					alive = false;
					std::cerr << "[SocketClient] Read error" << std::endl;
					break;
				}
			}
		}
		delete[] buffer;

		std::cout << "[SocketClient] Socket closing." << std::endl;
		close(socket);

		for(auto i = users.begin(); i != users.end(); i++) {
			(*i)->OnSocketClosed(*this);
		}

		return nullptr;
	}

	void SocketClient::appendUser(std::shared_ptr<SocketClientUser> user) {
		if (user == nullptr)
			return;

		this->users.push_back(std::move(user));
	}

	bool SocketClient::write(const std::vector<char>& data) const {
		pthread_t current_thread = pthread_self();
		if (!pthread_equal(current_thread, thread)) {
			std::cerr << "[SocketClient] Called write() from invalid thread" << std::endl;
			return false;
		}

		size_t total_written = 0;

		do {
			int written = send(socket, &data[total_written], data.size() - total_written, 0);
			if (written < 0) {
				auto err = errno;
				if (err == EWOULDBLOCK) {
					pollfd poll_fd {};
					poll_fd.fd = socket;
					poll_fd.events = POLLOUT;
					auto pollRes = poll(&poll_fd, 1, SOCKETCLIENT_WRITE_TIMEOUT);
					if (pollRes == 0) {
						return false; //Timeout
					} else if (pollRes < 0) {
						//Error during poll(), usually SIGINT from main thread
						std::cerr << "[SocketClient] Write error during poll()" << std::endl;
						return false;
					} else {
						continue;
					}
				} else {
					std::cerr << "[SocketClient] Generic write error" << std::endl;
					return false;
				}
			} else if (written > 0) {
				total_written += written;

				if (total_written >= data.size()) {
					break;
				}
			}
		} while(true);

		std::cout << "[SocketClient] Written " << total_written << " bytes" << std::endl;
		return true;
	}

	void SocketClient::stopThread() {
		alive = false;
		pthread_kill(thread, SIGINT);
	}

	void SocketClient::joinThread() const {
		if (alive) {
			pthread_join(thread, nullptr);
		}
	}
}