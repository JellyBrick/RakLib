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
		enum class QueuePriority
		{
			IMMEDIATE, // The packet is going to be sended immediately
			UPDATE, // The packet is going to be sended in the next update
			FULLQ, // The packet will be sended when the queue get full
		};

	protected:
		const std::string ip;
		const uint16 port;

		int64 clientID;

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
		Session(std::string ip, uint16 port, int64 clientID, int16 mtu);

		virtual void update();

		virtual void receivePacket(std::unique_ptr<Packet> packet);

		virtual void handleDataPacket(std::unique_ptr<DataPacket> packet) = 0;

		virtual void addToQueue(std::unique_ptr<DataPacket> packet, QueuePriority priority);
		virtual void sendPacket(std::unique_ptr<Packet> packet) = 0; // TODO: Find an better way to find CustomDataPacket. We are copying the buffer everytime we send an packet because of the ACK

		const std::string& getIP() const { return ip; };
		uint16 getPort() const { return port; };
	};
}