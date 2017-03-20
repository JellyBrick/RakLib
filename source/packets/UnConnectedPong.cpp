#include "UnConnectedPong.h"

namespace RakLib {
	UnConnectedPong::UnConnectedPong(int64 serverID, int64 pingID, const std::string& title) : LoginPacket(35 + title.length()) {
		this->serverID = serverID;
		this->pingID = pingID;
		this->str = title; //MCPE;ServerName;MCPEProtocol;MCPEVersion;Players;MaxPlayer
	}

	void UnConnectedPong::encode() {
		this->putByte(Packets::UNCONNECTED_PING);
		this->putLong(this->pingID);
		this->putLong(this->serverID);
		this->putByte(this->getMagic(), 16);
		this->putString(this->str);
	}
}