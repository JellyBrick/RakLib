#pragma once

#include <memory>
#include <thread>
#include <string>

#include "Network/UDPSocket.h"
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
		RakLib(SessionManager* sessionManager, const std::string& ip, uint16 port);

		inline const std::string& getIP() const { return this->ip; };
		inline uint16 getPort() const { return this->port; };
		inline bool isRunning() const { return this->running; };

		void sendPacket(std::unique_ptr<Packet> packet);

		void start();
		void run();
		void stop();
	};
}