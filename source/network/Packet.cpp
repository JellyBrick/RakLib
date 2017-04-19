#include "RakLib/network/Packet.h"

#include <algorithm>
#include <cassert>
#include <cstring>

/*
 * This class would wrap the byte buffer received from the client and his info (Adress, Port)
 * NOTE: This class would destroy the byte buffer when the class is destroyed
 */

namespace RakLib {

	const uint32 Packet::DEFAULT_BUFFER_SIZE = 1024 * 1024 * 2;
	
	Packet::Packet() : ByteBuffer(), port(0) {}

	Packet::Packet(uint32 size) : port(0) {
		buffer = new uint8[size];
		position = 0;
		length = size;
	}

	Packet::Packet(uint8* buffer, uint32 size, const std::string& ip, uint16 port) : ByteBuffer(buffer, size), ip(std::move(ip)), port(port) {}

	void Packet::clear() {
		assert(buffer != nullptr && length > 0);
		memset(buffer, 0x00, length);
		position = 0;
	}

	void Packet::close() {
		if (buffer != nullptr) {
			delete[] buffer;
			buffer = nullptr;
			length = 0;
			position = 0;
		}
	}

	void Packet::swap(Packet& other) noexcept {
		std::swap(buffer, other.buffer);
		std::swap(position, other.position);
		std::swap(length, other.length);
	}

	void Packet::resize(uint32 size) {
		uint32 newSize = std::min(size, length);

		uint8* newBuffer = new uint8[size];
		memcpy(newBuffer, buffer, newSize);
		delete[] buffer;

		buffer = newBuffer;
		position = std::min(position, newSize);
		length = size;
	}

	void Packet::release() {
		buffer = nullptr;
		position = 0;
		length = 0;
	}

	Packet::~Packet() {
		close();
	}
}
