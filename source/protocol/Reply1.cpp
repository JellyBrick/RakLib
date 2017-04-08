#include "RakLib/protocol/Reply1.h"

#include "RakLib/protocol/Info.h"

namespace RakLib {
	Reply1::Reply1(bool security, int64 serverID, uint16 mtuSize) : LoginPacket(28), 
		security(security), serverID(serverID), mtuSize(mtuSize) {}

	void Reply1::encode() {
		putByte(Packets::CONNECTION_REPLY_1);
		putByte(getMagic(), RAKNET_MAGIC_LENGTH);
		putLong(serverID);
		putBool(security);
		putUShort(mtuSize);
	}
}