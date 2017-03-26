#include "Packet.h"

#include <cassert>

/*
 * This class would wrap the byte buffer received from the client and his info (Adress, Port)
 * NOTE: This class would destroy the byte buffer when the class is destroyed
 */

namespace RakLib {
	Packet::Packet() : ByteBuffer(), port(0) {}

	Packet::Packet(uint32 size) : port(0) {
		this->buffer = new uint8[size];
		this->position = 0;
		this->length = size;
	}

	Packet::Packet(uint8* buffer, uint32 size, const std::string& ip, uint16 port) : ByteBuffer(buffer, size), ip(std::move(ip)), port(port) {}

	void Packet::clear() {
		assert(this->buffer != nullptr && this->length > 0);
		memset(this->buffer, 0x00, this->length);
		this->position = 0;
	}

	void Packet::close() {
		if (buffer != nullptr) {
			delete[] this->buffer;
			this->buffer = nullptr;
			this->length = 0;
			this->position = 0;
		}
	}

	void Packet::swap(Packet& other) noexcept {
		std::swap(this->buffer, other.buffer);
		std::swap(this->position, other.position);
		std::swap(this->length, other.length);
	}

	void Packet::resize(uint32 size) {
		uint32 newSize = min(size, length);

		uint8* newBuffer = new uint8[size];
		memcpy(newBuffer, this->buffer, newSize);
		delete[] this->buffer;

		this->buffer = newBuffer;
		this->position = min(this->position, newSize);
		this->length = size;
	}

	void Packet::release() {
		this->buffer = nullptr;
		this->position = 0;
		this->length = 0;
	}

	Packet::~Packet() {
		this->close();
	}
}