#include "packets/Acknowledge.h"
#include "Session.h"

namespace RakLib {
	Session::Session(const std::string& ip, uint16 port, int64 clientID, int16 mtu) : ip(std::move(ip)), port(port), clientID(clientID), lastSequenceNum(0), sequenceNum(0), messageIndex(0), mtuSize(mtu) {
		this->updateQueue = std::make_unique<CustomPacket>(nullptr, 0);
		this->normalQueue = std::make_unique<CustomPacket>(nullptr, 0);
	}

	//This method should be called often but not too often. 
	void Session::update() {
		if (!this->updateQueue->packets.empty()) {
			this->updateQueue->packetID = 0x80;
			this->updateQueue->sequenceNumber = this->sequenceNum++;
			this->updateQueue->encode();

			this->sendPacket(*this->updateQueue);
			this->recoveryQueue[this->updateQueue->sequenceNumber] = std::move(this->updateQueue);

			this->updateQueue = std::make_unique<CustomPacket>(nullptr, 0);
		}

		if (!this->ACKQueue.empty()) {
			Acknowledge ack(0xC0, this->ACKQueue);
			ack.encode();

			this->sendPacket(ack);
			this->ACKQueue.clear();
		}

		if (!this->NACKQueue.empty()) {
			Acknowledge nack(0xA0, this->NACKQueue);
			nack.encode();

			this->sendPacket(nack);
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
				if (this->recoveryQueue[sequenceNumber]) {
					this->sendPacket(*this->recoveryQueue[sequenceNumber]);
				}
			}
		} else if (packetID >= 0x80 && packetID <= 0x8F) {  // Custom Packets Range
			CustomPacket customPacket(std::move(packet));
			customPacket.decode();

			this->ACKQueue.push_back(customPacket.sequenceNumber);

			//If false then the custom packet is a lost packet
			if (this->sequenceNum < customPacket.sequenceNumber) {
				this->lastSequenceNum = this->sequenceNum;
				this->sequenceNum = customPacket.sequenceNumber;

				if (customPacket.sequenceNumber - this->lastSequenceNum > 1) {
					for (uint32 i = this->lastSequenceNum + 1; i < this->sequenceNum; i++) {
						this->NACKQueue.push_back(i);
					}
				}
			} else {
				for (uint32 i = 0; i < this->NACKQueue.size(); i++) {
					if (this->NACKQueue[i] == customPacket.sequenceNumber) {
						this->NACKQueue.erase(this->NACKQueue.begin() + i);
					}
				}
			}

			//TODO: Handle splitted packets
			for (const auto& internalPacket : customPacket.packets) {
				this->handleDataPacket(std::make_unique<DataPacket>(std::move(internalPacket)));
			}
		} else {
			this->handleDataPacket(std::make_unique<DataPacket>(std::move(packet)));
		}
	}

	//You need to encode the data packet before using this function.
	void Session::addToQueue(std::unique_ptr<DataPacket> packet, QueuePriority priority) {
		//TODO: Split packet if length > MTU
		InternalPacket* internalPacket = new InternalPacket();
		internalPacket->reliability = 0x02;
		internalPacket->messageIndex = this->messageIndex++;
		internalPacket->length = (uint16)packet->getLength();
		internalPacket->buff = new uint8[internalPacket->length];
		memcpy(internalPacket->buff, packet->getBuffer(), internalPacket->length);

		if (priority == QueuePriority::IMMEDIATE) {
			auto customPacket = std::make_unique<CustomPacket>(nullptr, 0);
			customPacket->packetID = 0x80;
			customPacket->sequenceNumber = this->sequenceNum++;
			customPacket->packets.push_back(internalPacket); 
			customPacket->encode();

			this->sendPacket(*customPacket);
			this->recoveryQueue[customPacket->sequenceNumber] = std::move(customPacket);
		} else if (priority == QueuePriority::UPDATE) {
			this->updateQueue->packets.push_back(internalPacket);
		} else if (priority == QueuePriority::FULLQ) {
			this->normalQueue->packets.push_back(internalPacket);
			if (this->normalQueue->getLength() > this->mtuSize) {
				this->normalQueue->packetID = 0x80;
				this->normalQueue->sequenceNumber = this->sequenceNum++;
				this->normalQueue->encode();

				this->sendPacket(*normalQueue);
				this->recoveryQueue[this->normalQueue->sequenceNumber] = std::move(this->normalQueue);

				this->normalQueue = std::make_unique<CustomPacket>(nullptr, 0);
			}
		}
	}
}
