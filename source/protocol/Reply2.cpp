#include "RakLib/protocol/Reply2.h"

#include "RakLib/protocol/Info.h"

namespace RakLib {
	Reply2::Reply2(int64 serverID, uint16 port, int16 mtuSize, bool security) 
		: LoginPacket(30), serverID(serverID), rport(port), mtuSize(mtuSize), security(security) {
	}

	void Reply2::encode() {
		putByte(Packets::CONNECTION_REPLY_2);
		putByte(getMagic(), RAKNET_MAGIC_LENGTH);
		putLong(serverID);
		putUShort(rport);
		putShort(mtuSize);
		putBool(security);
	}
}