#include "Acknowledge.h"

#include <algorithm>

namespace RakLib {
	Acknowledge::Acknowledge(uint8 packetID, const std::vector<uint32>& sequenceArray) : Packet(2048), pid(packetID), sequenceNumbers(sequenceArray) {}

	Acknowledge::Acknowledge(std::unique_ptr<Packet>&& packet) : Packet(packet->getBuffer(), packet->getLength(), "", 0), pid(0) {
		packet->release();
	}

	void Acknowledge::encode() {
		this->position = 3;
		std::sort(this->sequenceNumbers.begin(), this->sequenceNumbers.end());
		int count = this->sequenceNumbers.size();
		short records = 0;

		if (count > 0) {
			int pointer = 1;
			int start = this->sequenceNumbers[0];
			int last = this->sequenceNumbers[0];
			while (pointer < count) {
				int current = this->sequenceNumbers[pointer++];
				int diff = current - last;
				if (diff == 1) {
					last = current;
				} else if (diff > 1) {
					if (start == last) {
						this->putByte((uint8)0x01);
						this->putLTriad(start);
						start = last = current;
					} else {
						this->putByte((uint8)0x00);
						this->putLTriad(start);
						this->putLTriad(last);
						start = last = current;
					}
					++records;
				}
			}

			if (start == last) {
				this->putByte((uint8)0x01);
				this->putLTriad(start);
			} else {
				this->putByte((uint8)0x00);
				this->putLTriad(start);
				this->putLTriad(last);
			}
			++records;
		}

		uint32 size = this->position;
		this->position = 0;
		this->putByte(this->pid);
		this->putShort(records);
		this->resize(size);
	}

	void Acknowledge::decode() {
		this->pid = this->getByte();
		int16 count = this->getShort();

		for (int16 i = 0; i < count; ++i) {
			if (!this->getBool()) {
				int24 start = this->getLTriad();
				int24 end = this->getLTriad();
				if (end - start > 4096) {
					end = start + 4096;
				}

				for (int24 c = start; c <= end; ++c) {
					this->sequenceNumbers.push_back(c);
				}
			} else {
				this->sequenceNumbers.push_back(this->getLTriad());
			}
		}
	}
}