#include "UDPSocket.h" 

#include <cassert>
#include <iostream>

namespace RakLib {
	UDPSocket::UDPSocket(const std::string& ip, uint16 port) {
#ifdef WIN32 
		assert(WSAStartup(MAKEWORD(2, 2), &this->WSAData) == 0);
#endif 

		this->sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (this->sock == INVALID_SOCKET) {
			std::cout << "Could not create the socket!";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif 
			return;
		}

		this->bind(ip, port);
	}

	bool UDPSocket::bind(const std::string& ip, uint16 port) {
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		if (!ip.empty()) {
			inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		} else {
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
		}

		if (::bind(this->sock, (sockaddr*) &addr, sizeof(sockaddr_in)) == -1) {
			std::cout << "Could not bind the socket!";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif
			return false;
		}
		return true;
	}

	std::unique_ptr<Packet> UDPSocket::receive() {
		sockaddr_in recv;
		uint8* buffer = new uint8[Packet::DEFAULT_BUFFER_SIZE];

		int sie = sizeof(sockaddr_in);
		socklen_t size = recvfrom(this->sock, (char*)buffer, Packet::DEFAULT_BUFFER_SIZE, 0, (sockaddr*)&recv, (socklen_t*)&sie);
		if (size == -1) {
			std::cout << "Could not receive the packet!";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif
			this->close();
			return nullptr;
		}


		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &recv.sin_addr, ip, INET_ADDRSTRLEN);
		return std::make_unique<Packet>(buffer, size, ip, recv.sin_port);
	}

	int UDPSocket::send(Packet* pck) {
		sockaddr_in sendaddr;
		sendaddr.sin_family = AF_INET;
		sendaddr.sin_port = pck->port;
		inet_pton(AF_INET, pck->ip.c_str(), &sendaddr.sin_addr);

		int size = sendto(this->sock, (char*)pck->getBuffer(), pck->getLength(), 0, (sockaddr*)&sendaddr, sizeof(sockaddr_in));
		if (size == -1) {
			std::cout << "Could not send the packet!";
#ifdef WIN32 
			std::cout << " Error Code: " << WSAGetLastError() << std::endl;
			WSACleanup();
#endif 
			this->close();
			return size;
		}

		return size;
	}

	void UDPSocket::close() {
#ifdef _WIN32
		closesocket(this->sock);
		WSACleanup();
#else
		close(this->sock);
#endif
	}

	UDPSocket::~UDPSocket() {
		this->close();
	}
}