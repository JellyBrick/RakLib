#pragma once

#include <string>

#include "Common.h"

namespace RakLib {
	class Session;
	class SessionManager {
	public:
		/*
		 * TODO: Description
		 */
		virtual void addSession(const std::string& ip, uint16 port, int64 clientID, int16 mtu) = 0;

		/*
		 * TODO: Description
		 */
		virtual void removeSession(const std::string& ip, unsigned short port) = 0;

		/*
		 * TODO: Description
		 */
		virtual Session* getSession(const std::string& ip, unsigned short port) = 0;

		/*
		 *  Server Identifier
		 */
		virtual int64 getIdentifier() = 0;

		/*
		 * Server use security
		 */
		virtual bool useSecurity() = 0;

		/*
		 *	Server type
		 *	Expected: MCPE
		 */
		virtual std::string getType() = 0;

		/* 
		 * Server Name
		 */
		virtual std::string getName() = 0;

		/*
		 * Server Protocol
		 */
		virtual int32 getProtocol() = 0;

		/*
		 * Server Game Version Supported
		 */
		virtual std::string getGameVersion() = 0;

		/*
		 * Server Current number of player
		 */
		virtual uint32 getActivePlayers() = 0;

		/*
		 * Server Player's Capacity
		 */
		virtual uint32 getMaxPlayer() = 0;
	};
}