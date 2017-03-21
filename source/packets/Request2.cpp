#include "Request2.h"

namespace RakLib {
	Request2::Request2(std::unique_ptr<Packet> packet) : LoginPacket(std::move(packet)), security(false), cookie(0), port(0), mtuSize(0), clientID(0){}

	void Request2::decode() {
		this->position += 17;
		this->security = this->getBool();
		this->cookie = this->getInt();
		this->port = (uint16)this->getShort();
		this->mtuSize = (uint16)this->getShort();
		this->clientID = this->getLong();
	}
}