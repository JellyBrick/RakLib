#include "RakLib/protocol/Acknowledge.h"

#include <algorithm>

namespace RakLib {
	Acknowledge::Acknowledge(uint8 packetID, const std::vector<uint32>& sequenceArray) : Packet(2048), pid(packetID), sequenceNumbers(sequenceArray) {}

	Acknowledge::Acknowledge(std::unique_ptr<Packet>&& packet) : Packet(packet->getBuffer(), packet->getLength(), "", 0), pid(0) {
		packet->release();
	}

	void Acknowledge::encode() {
		position = 3;
		std::sort(sequenceNumbers.begin(), sequenceNumbers.end());
		int count = sequenceNumbers.size();
		short records = 0;

		if (count > 0) {
			int pointer = 1;
			int start = sequenceNumbers[0];
			int last = sequenceNumbers[0];
			while (pointer < count) {
				int current = sequenceNumbers[pointer++];
				int diff = current - last;
				if (diff == 1) {
					last = current;
				} else if (diff > 1) {
					if (start == last) {
						putBool(true);
						putLTriad(start);
						start = last = current;
					} else {
						putBool(false);
						putLTriad(start);
						putLTriad(last);
						start = last = current;
					}
					++records;
				}
			}

			if (start == last) {
				putBool(true);
				putLTriad(start);
			} else {
				putBool(false);
				putLTriad(start);
				putLTriad(last);
			}
			++records;
		}

		uint32 size = position;
		position = 0;
		putByte(pid);
		putShort(records);
		resize(size);
	}

	void Acknowledge::decode() {
		pid = getByte();
		int16 count = getShort();

		for (int16 i = 0; i < count; ++i) {
			if (!getBool()) {
				int24 start = getLTriad();
				int24 end = getLTriad();
				if (end - start > 4096) {
					end = start + 4096;
				}

				for (int24 c = start; c <= end; ++c) {
					sequenceNumbers.push_back(c);
				}
			} else {
				sequenceNumbers.push_back(getLTriad());
			}
		}
	}
}