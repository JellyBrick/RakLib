#pragma once

#include <vector>

#include "../Network/Packet.h"

namespace RakLib {
	class Acknowledge : public Packet {
	public:
		std::vector<uint32> sequenceNumbers;
		uint8 pid; // ACK || NACK

	public:
		Acknowledge(uint8 pid, std::vector<uint32> sequenzeNum);
		Acknowledge(std::unique_ptr<Packet> pck);

		void decode();
		void encode();

	};
}