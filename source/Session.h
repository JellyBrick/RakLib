#pragma once

#include <map>
#include <memory>
#include <vector>

#include "packets/CustomPacket.h"
#include "packets/DataPacket.h"
#include "network/Packet.h"

namespace RakLib {
	class Session {
	public:
		enum class QueuePriority {
			IMMEDIATE, // The packet is going to be sended immediately
			UPDATE, // The packet is going to be sended in the next update
		};

	protected:
		const std::string ip;
		const uint16 port;

		uint64 clientID;

	private:
		uint32 lastSequenceNum;
		uint32 sequenceNum;
		uint32 messageIndex;

	protected:
		uint16 mtuSize;

	private:
		std::unique_ptr<CustomPacket> normalQueue;
		std::unique_ptr<CustomPacket> updateQueue;
		std::vector<uint32> ACKQueue; // Received Packet Queue
		std::vector<uint32> NACKQueue; // Not received packet queue
		std::map<uint32, std::unique_ptr<CustomPacket>> recoveryQueue; // Packet sent queue to be used if not received

	public:
		Session(const std::string& ip, uint16 port, uint64 clientID, uint16 mtu);

		virtual void update();

		virtual void receivePacket(std::unique_ptr<Packet> packet);

		virtual void handleDataPacket(std::unique_ptr<Packet> packet) = 0;

		virtual void addToQueue(std::unique_ptr<DataPacket> packet, QueuePriority priority);

		virtual void sendPacket(Packet& packet) = 0;

		const std::string& getIP() const { return ip; };

		uint16 getPort() const { return port; };
	};
}