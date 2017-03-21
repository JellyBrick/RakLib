#pragma once

#include <memory>

#ifdef __unix__
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#define INVALID_SOCKET -1
	#define SOCKET int
#else
	#include <winsock2.h>
	#include <Ws2tcpip.h>
	#include <windows.h>
	#ifdef _MSC_VER
		#pragma comment(lib, "Ws2_32.lib")
	#endif
#endif

#include "Packet.h"

namespace RakLib {
	class UDPSocket {
	private:
#ifdef WIN32
		WSAData WSAData;
#endif
		SOCKET sock;

	public:
		UDPSocket(const std::string& ip, uint16 port);
		~UDPSocket();

		bool bind(const std::string& ip, uint16 port);

		std::unique_ptr<Packet> receive();
		int send(const Packet& packet);

		void close();
	};
}