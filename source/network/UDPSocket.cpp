#include "RakLib/network/UDPSocket.h" 

#include <cassert>
#include <iostream>

namespace RakLib {
	UDPSocket::UDPSocket(const std::string& ip, uint16 port) {
#ifdef WIN32 
		assert(WSAStartup(MAKEWORD(2, 2), &WSAData) == 0);
#endif 

		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET) {
			std::cout << "Could not create the socket!";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif 
			return;
		}

		setOptions();
		bind(ip, port);
	}

	bool UDPSocket::bind(const std::string& ip, uint16 port) const {
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		if (!ip.empty()) {
			inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		} else {
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
		}

		if (::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) == -1) {
			std::cout << "Could not bind the socket. Maybe another program is using it\n";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif
			return false;
		}
		return true;
	}

	std::unique_ptr<Packet> UDPSocket::receive() const {
		sockaddr_in recv;
		uint8* buffer = new uint8[Packet::DEFAULT_BUFFER_SIZE];

		int sie = sizeof(sockaddr_in);
		socklen_t size = recvfrom(sock, reinterpret_cast<char*>(buffer), Packet::DEFAULT_BUFFER_SIZE, 0, reinterpret_cast<sockaddr*>(&recv), reinterpret_cast<socklen_t*>(&sie));
		if (size == -1) {
			std::cout << "Could not receive the packet.";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif
			return nullptr;
		}


		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &recv.sin_addr, ip, INET_ADDRSTRLEN);
		return std::make_unique<Packet>(buffer, size, ip, recv.sin_port);
	}

	int UDPSocket::send(const Packet& packet) const {
		sockaddr_in sendaddr;
		sendaddr.sin_family = AF_INET;
		sendaddr.sin_port = packet.port;
		inet_pton(AF_INET, packet.ip.c_str(), &sendaddr.sin_addr);

		int size = sendto(sock, reinterpret_cast<char*>(packet.getBuffer()), packet.getLength(), 0, reinterpret_cast<const sockaddr*>(&sendaddr), sizeof(sockaddr_in));
		if (size == -1) {
			std::cout << "Could not send the packet!";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif 
			return size;
		}

		return size;
	}

	void UDPSocket::close() const {
#ifdef _WIN32
		closesocket(sock);
		WSACleanup();
#else
		::close(sock);
#endif
	}

	void UDPSocket::setOptions() const {
		// Set maximun Receive Buffer Size
		setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&Packet::DEFAULT_BUFFER_SIZE), sizeof(Packet::DEFAULT_BUFFER_SIZE));
		// Set socket bound to this proccess
		bool exclusiveUse = true;
#ifdef _WIN32
			setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<const char*>(&exclusiveUse), sizeof(exclusiveUse));
#endif
		// Set socket to not reuse adress if already in use
		bool reuseAddress = false;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(reuseAddress), sizeof(reuseAddress));
	}

	UDPSocket::~UDPSocket() {
		close();
	}
}