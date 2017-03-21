#include "Acknowledge.h"

#include <algorithm>

namespace RakLib {
	Acknowledge::Acknowledge(uint8 pid, std::vector<uint32> sequenzeNum) : Packet(2048) {
		this->pid = pid;
		this->sequenceNumbers = sequenzeNum;
	}

	Acknowledge::Acknowledge(std::unique_ptr<Packet> pck) : Packet(std::move(pck)), pid(0) {}

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

		uint32 length = this->position;
		this->position = 0;
		this->putByte(this->pid);
		this->putShort(records);
		this->resize(length);
	}

	void Acknowledge::decode() {
		this->pid = this->getByte();
		int count = this->getShort();

		for (int i = 0; i < count; ++i) {
			if (this->getByte() == 0x00) {
				int start = this->getLTriad();
				int end = this->getLTriad();
				if ((end - start) > 4096) {
					end = start + 4096;
				}

				for (int c = start; c <= end; ++c) {
					this->sequenceNumbers.push_back(c);
				}
			} else {
				this->sequenceNumbers.push_back(this->getLTriad());
			}
		}
	}
}