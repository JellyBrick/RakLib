#pragma once

#include <memory>
#include <vector>

#include "../Network/Packet.h"

namespace RakLib {
	class Acknowledge : public Packet {
	public:
		std::vector<uint32> sequenceNumbers;
		uint8 pid; // ACK || NACK

	public:
		Acknowledge(uint8 pid, const std::vector<uint32>& sequenzeNum);
		Acknowledge(std::unique_ptr<Packet>&& packet);

		void decode();
		void encode();

	};
}