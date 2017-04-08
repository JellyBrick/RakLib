#include "RakLib/protocol/Acknowledge.h"

#include "RakLib/Session.h"

namespace RakLib {
	Session::Session(const std::string& ip, uint16 port, uint64 clientID, uint16 mtu) : ip(std::move(ip)), port(port), clientID(clientID), lastSequenceNum(0), sequenceNum(0), messageIndex(0), mtuSize(mtu) {
		updateQueue = std::make_unique<CustomPacket>();
		normalQueue = std::make_unique<CustomPacket>();
	}

	//This method should be called often but not too often. 
	void Session::update() {
		if (!updateQueue->packets.empty()) {
			updateQueue->packetID = 0x80;
			updateQueue->sequenceNumber = sequenceNum++;
			updateQueue->encode();

			sendPacket(*updateQueue);
			recoveryQueue[updateQueue->sequenceNumber] = std::move(updateQueue);

			updateQueue = std::make_unique<CustomPacket>();
		}

		if (!ACKQueue.empty()) {
			Acknowledge ack(0xC0, ACKQueue);
			ack.encode();

			sendPacket(ack);
			ACKQueue.clear();
		}

		if (!NACKQueue.empty()) {
			Acknowledge nack(0xA0, NACKQueue);
			nack.encode();

			sendPacket(nack);
		}
	}

	//This method should only be called from RakLib
	void Session::receivePacket(std::unique_ptr<Packet> packet) {
		uint8 packetID = packet->getBuffer()[0];
		if (packetID == 0xC0) { // ACK
			Acknowledge ack(std::move(packet));
			ack.decode();

			for (const auto& sequenceNumber : ack.sequenceNumbers) {
				recoveryQueue.erase(sequenceNumber);
			}

		} else if (packetID == 0xA0) { // NACK
			Acknowledge nack(std::move(packet));
			nack.decode();

			for (const auto& sequenceNumber : nack.sequenceNumbers) {
				if (recoveryQueue[sequenceNumber]) {
					sendPacket(*recoveryQueue[sequenceNumber]);
				}
			}
		} else if (packetID >= 0x80 && packetID <= 0x8F) {  // Custom Packets Range
			CustomPacket customPacket(std::move(packet));
			customPacket.decode();

			ACKQueue.push_back(customPacket.sequenceNumber);

			//If false then the custom packet is a lost packet
			if (sequenceNum < customPacket.sequenceNumber) {
				lastSequenceNum = sequenceNum;
				sequenceNum = customPacket.sequenceNumber;

				if (customPacket.sequenceNumber - lastSequenceNum > 1) {
					for (uint32 i = lastSequenceNum + 1; i < sequenceNum; i++) {
						NACKQueue.push_back(i);
					}
				}
			} else {
				for (uint32 i = 0; i < NACKQueue.size(); i++) {
					if (NACKQueue[i] == customPacket.sequenceNumber) {
						NACKQueue.erase(NACKQueue.begin() + i);
					}
				}
			}

			//TODO: Handle splitted packets
			for (const auto& internalPacket : customPacket.packets) {
				handleDataPacket(std::make_unique<Packet>(internalPacket->buff, static_cast<uint32>(internalPacket->length)));
				internalPacket->buff = nullptr;
			}
		} else {
			handleDataPacket(std::move(packet));
		}
	}

	//You need to encode the data packet before using this function.
	void Session::addToQueue(std::unique_ptr<DataPacket> packet, QueuePriority priority) {
		//TODO: Split packet if length > MTU
		InternalPacket* internalPacket = new InternalPacket();
		internalPacket->reliability = 0x02;
		internalPacket->messageIndex = messageIndex++;
		internalPacket->length = static_cast<uint16>(packet->getLength());
		internalPacket->buff = packet->getBuffer();
		packet->release();

		if (priority == QueuePriority::IMMEDIATE) {
			auto customPacket = std::make_unique<CustomPacket>();
			customPacket->packetID = 0x80;
			customPacket->sequenceNumber = sequenceNum++;
			customPacket->packets.push_back(internalPacket); 
			customPacket->encode();

			sendPacket(*customPacket);
			recoveryQueue[customPacket->sequenceNumber] = std::move(customPacket);
		} else if (priority == QueuePriority::UPDATE) {
			updateQueue->packets.push_back(internalPacket);
		}
	}
}
