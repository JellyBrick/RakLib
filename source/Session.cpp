#include "packets/Acknowledge.h"
#include "Session.h"

namespace RakLib {
	Session::Session(std::string ip, uint16 port, int64 clientID, int16 mtu) : ip(ip), port(port), sequenceNum(0), lastSequenceNum(0), messageIndex(0) {
		this->clientID = clientID;
		this->mtuSize = mtu;

		this->normalQueue = new CustomPacket(nullptr, 0);
		this->updateQueue = new CustomPacket(nullptr, 0);
	}

	//This method should be called often but not too often. 
	void Session::update() {
		if (!this->updateQueue->packets.empty()) {
			this->updateQueue->packetID = 0x80;
			this->updateQueue->sequenceNumber = this->sequenceNum++;
			this->updateQueue->encode();

			this->sendPacket(this->updateQueue);
			this->recoveryQueue[this->updateQueue->sequenceNumber] = this->updateQueue;

			this->updateQueue = new CustomPacket(nullptr, 0);
		}

		if (!this->ACKQueue.empty()) {
			Acknowledge ack(0xC0, this->ACKQueue);
			ack.encode();

			this->sendPacket(&ack);
			this->ACKQueue.clear();
		}

		if (!this->NACKQueue.empty()) {
			Acknowledge nack(0xA0, this->NACKQueue);
			nack.encode();

			this->sendPacket(&nack);
		}
	}

	//This method should only be called from RakLib
	void Session::receivePacket(std::unique_ptr<Packet> packet) {
		uint8 packetID = packet->getBuffer()[0];
		if (packetID == 0xC0) { // ACK
			Acknowledge ack(packet.get());
			ack.decode();

			for (uint32 i : ack.sequenceNumbers) {
				if (this->recoveryQueue[i] != nullptr) {
					delete this->recoveryQueue[i];
				}
			}

		} else if (packetID == 0xA0) { // NACK
			Acknowledge nack(packet.get());
			nack.decode();

			for (uint32 i : nack.sequenceNumbers) {
				if (this->recoveryQueue[i] != nullptr) {
					this->sendPacket(this->recoveryQueue[i]);
				}
			}
		} else if (packetID >= 0x80 && packetID <= 0x8F) {  // Custom Packets Range
			CustomPacket customPacket(packet.get());
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

			for (InternalPacket* internalPacket : customPacket.packets) {
				//TODO: Handle splitted packets
				DataPacket dataPacket(internalPacket);
				this->handleDataPacket(&dataPacket);
			}
		} else {
			//This packet is unknown. You could implement a plugin that handle unknown packet
			DataPacket dataPacket(packet.get());
			this->handleDataPacket(&dataPacket);
		}
	}

	//You need to encode the data packet before using this function.
	void Session::addToQueue(DataPacket* packet, QueuePriority priority) {
		//TODO: Split packet if length > MTU
		InternalPacket* internalPacket = new InternalPacket();
		internalPacket->reliability = 2;
		internalPacket->messageIndex = this->messageIndex++;
		internalPacket->buff = packet->getBuffer();
		internalPacket->length = packet->getLength();

		if (priority == QueuePriority::IMMEDIATE) {
			CustomPacket* customPacket = new CustomPacket(nullptr, 0);
			customPacket->packetID = 0x80;
			customPacket->sequenceNumber = this->sequenceNum++;
			
			//Add the data packet
			customPacket->packets.push_back(internalPacket); 
			customPacket->encode();

			this->sendPacket(customPacket);
			this->recoveryQueue[customPacket->sequenceNumber] = customPacket; // When we receive a notification that this packet have been received, the packet will be destroyed.
		} else if (priority == QueuePriority::UPDATE) {
			this->updateQueue->packets.push_back(internalPacket);
		} else if (priority == QueuePriority::FULLQ) {
			this->normalQueue->packets.push_back(internalPacket);
			if (this->normalQueue->getLength() > this->mtuSize) {
				this->normalQueue->packetID = 0x80;
				this->normalQueue->sequenceNumber = this->sequenceNum++;
				this->normalQueue->encode();

				this->sendPacket(this->normalQueue);
				this->recoveryQueue[this->normalQueue->sequenceNumber] = this->normalQueue; // When we receive a notification that this packet have been received, the packet will be destroyed.

				this->normalQueue = new CustomPacket(nullptr, 0);
			}

		}
	}
}
