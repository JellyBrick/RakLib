#include "Reply2.h"

#include "Info.h"

namespace RakLib {
	Reply2::Reply2(int64 serverID, uint16 port, int16 mtuSize, bool security) : LoginPacket(30) {
		this->serverID = serverID;
		this->rport = port;
		this->mtuSize = mtuSize;
		this->security = security;
	}

	void Reply2::encode() {
		this->putByte(Packets::CONNECTION_REPLY_2);
		this->putByte(this->getMagic(), RAKNET_MAGIC_LENGTH);
		this->putLong(this->serverID);
		this->putUShort(this->rport);
		this->putShort(this->mtuSize);
		this->putBool(this->security);
	}
}