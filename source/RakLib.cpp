#include "RakLib.h"

#include <cassert>
#include <sstream>

#include "packets/Info.h"
#include "packets/Reply1.h"
#include "packets/Reply2.h"
#include "packets/Request1.h"
#include "packets/Request2.h"
#include "packets/UnConnectedPing.h"
#include "packets/UnConnectedPong.h"
#include "Session.h"

namespace RakLib {
	RakLib::RakLib(SessionManager* server, const std::string& serverIP, uint16 serverPort) {
		this->sessionManager = server;
		this->ip = serverIP;
		this->port = serverPort;
		this->running = false;
	}

	void RakLib::start() {
		assert(!this->running);

		this->running = true;
		this->socket = std::make_unique<UDPSocket>(this->ip, this->port);
		this->mainThread = std::thread(&RakLib::run, this);
	}

	void RakLib::run() const {
		while (this->running) {
			std::unique_ptr<Packet> packet = this->socket->receive();
			uint8 pid = packet->getBuffer()[0];

			switch (pid) {
			case Packets::UNCONNECTED_PONG:
			case Packets::UNCONNECTED_PONG_2:
			{
				UnConnectedPing ping(std::move(packet));
				ping.decode();

				std::stringstream identifierStream; // "MCPE;Minecraft Server!;34;0.12.2;2;20"
				identifierStream << sessionManager->getType() << ";";
				identifierStream << sessionManager->getName() << ";";
				identifierStream << sessionManager->getProtocol() << ";";
				identifierStream << sessionManager->getGameVersion() << ";";
				identifierStream << sessionManager->getActivePlayers() << ";";
				identifierStream << sessionManager->getMaxPlayer();

				UnConnectedPong pong(this->sessionManager->getIdentifier(), ping.pingID, identifierStream.str());
				pong.encode();

				pong.ip = ping.ip;
				pong.port = ping.port;

				this->socket->send(pong);
			}
			break;

			case Packets::CONNECTION_REQUEST_1:
			{
				Request1 request(std::move(packet));
				request.decode();

				Reply1 reply(this->sessionManager->useSecurity(), this->sessionManager->getIdentifier(), request.mtuSize);
				reply.encode();

				reply.ip = request.ip;
				reply.port = request.port;

				this->socket->send(reply);
			}
			break;

			case Packets::CONNECTION_REQUEST_2:
			{
				Request2 request(std::move(packet));
				request.decode();

				Reply2 reply(this->sessionManager->getIdentifier(), request.port, request.mtuSize, request.security);
				reply.encode();

				reply.ip = request.ip;
				reply.port = request.port;

				this->socket->send(reply);
				this->sessionManager->addSession(request.ip, request.port, request.clientID, reply.mtuSize);
			}
			break;

			default:
			{
				Session* session = this->sessionManager->getSession(packet->ip, packet->port);
				if (session == nullptr) {
					break;
				}
				
				session->receivePacket(std::move(packet));
			}
			break;

			}
		}
	}

	void RakLib::sendPacket(const Packet& packet) {
		this->socket->send(packet);
	}

	void RakLib::stop() {
		assert(!this->running);

		this->running = false;
		this->mainThread.join();
	}
}