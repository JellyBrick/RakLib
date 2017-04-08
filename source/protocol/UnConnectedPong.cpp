#include "RakLib/protocol/UnConnectedPong.h"

#include "RakLib/protocol/Info.h"

namespace RakLib {
	UnConnectedPong::UnConnectedPong(int64 serverID, int64 pingID, const std::string& title) 
		: LoginPacket(35 + title.length()), serverID(serverID), pingID(pingID), str(title) {
	}

	void UnConnectedPong::encode() {
		putByte(Packets::UNCONNECTED_PING);
		putLong(pingID);
		putLong(serverID);
		putByte(getMagic(), RAKNET_MAGIC_LENGTH);
		putString(str); //MCPE;ServerName;MCPEProtocol;MCPEVersion;Players;MaxPlayer
	}
}