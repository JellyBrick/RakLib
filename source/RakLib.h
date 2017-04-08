#pragma once

#include <memory>
#include <thread>
#include <string>

#include "network/UDPSocket.h"
#include "SessionManager.h"

namespace RakLib {
	class RakLib {
	private:
		std::unique_ptr<UDPSocket> socket;
		SessionManager* sessionManager;
		std::thread mainThread;

		std::string ip;
		uint16 port;

		bool running;

	public:
		RakLib(SessionManager* server, const std::string& serverIP, uint16 serverPort);

		const std::string& getIP() const { return ip; };
		uint16 getPort() const { return port; };
		bool isRunning() const { return running; };

		void sendPacket(const Packet& packet) const;

		void start();
		void run() const;
		void stop();
	};
}