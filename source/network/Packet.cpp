#include "Packet.h"

#include <cassert>

namespace RakLib {
	Packet::Packet() : port(0) {
		this->buffer = new uint8[DEFAULT_BUFFER_SIZE];
		this->position = 0;
		this->length = DEFAULT_BUFFER_SIZE;
	}


	Packet::Packet(uint32 size) : port(0) {
		this->buffer = new uint8[size];
		this->position = 0;
		this->length = size;
	}

	// This would not take ownership over the buffer
	Packet::Packet(uint8* buffer, uint32 size) :  port(0) {
		assert(buffer != nullptr);

		this->buffer = new uint8[size];
		memcpy(this->buffer, buffer, size);

		this->position = 0;
		this->length = size;
	}

	// This would take ownership of the buffer
	Packet::Packet(uint8* buffer, uint32 size, const std::string& ip, uint16 port) : ip(std::move(ip)), port(port) {
		assert(buffer != nullptr);

		this->buffer = buffer;
		this->position = 0;
		this->length = size;
	}

	// Should I take ownership?
	Packet::Packet(std::unique_ptr<Packet> other) {
		this->buffer = new uint8[other->getLength()];
		memcpy(this->buffer, other->getBuffer(), other->getLength());

		this->position = 0;
		this->length = other->getLength();

		this->ip = other->ip;
		this->port = other->port;
	}

	// Write Methods
	void Packet::putByte(uint8 v) {
		assert(this->position + sizeof(uint8) <= this->length);
		this->buffer[this->position++] = v;
	}

	void Packet::putByte(uint8* bytes, uint32 size) {
		assert(this->position + size <= this->length);
		memcpy(this->buffer + this->position, bytes, size);
		this->position += size;
	}

	void Packet::putChar(int8 c) {
		assert(this->position + sizeof(int8) <= this->length);
		this->buffer[this->position++] = c;
	}

	void Packet::putBool(bool value) {
		assert(this->position + sizeof(bool) <= this->length);
		this->buffer[this->position++] = value ? 0x01 : 0x00;
	}


	void Packet::putShort(int16 v) {
		assert(this->position + sizeof(int16) <= this->length);
		this->buffer[this->position++] = (uint8)(v >> 8 & 0xFF);
		this->buffer[this->position++] = (uint8)(v      & 0xFF);
	}

	void Packet::putInt(int32 v) {
		assert(this->position + sizeof(int32) <= this->length);
		this->buffer[this->position++] = (uint8)(v >> 24 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 16 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >>  8 & 0xFF);
		this->buffer[this->position++] = (uint8)(v       & 0xFF);
	}

	void Packet::putTriad(int24 v) {
		assert(this->position + 3 <= this->length);
		this->buffer[this->position++] = (uint8)(v >> 16 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >>  8 & 0xFF);
		this->buffer[this->position++] = (uint8)(v       & 0xFF);
	}

	void Packet::putLTriad(int24 v) {
		assert(this->position + 3 <= this->length);
		this->buffer[this->position++] = (uint8)(v       & 0xFF);
		this->buffer[this->position++] = (uint8)(v >>  8 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 16 & 0xFF);
	}


	void Packet::putFloat(f32 v) {
		this->putInt((int32)v);
	}

	void Packet::putLong(int64 v) {
		assert(this->position + sizeof(int64) <= this->length);
		this->buffer[this->position++] = (uint8)(v >> 56 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 48 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 40 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 32 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 24 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 16 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >>  8 & 0xFF);
		this->buffer[this->position++] = (uint8)(v       & 0xFF);
	}

	void Packet::putDouble(f64 v) {
		this->putLong((int64)v);
	}

	void Packet::putString(const std::string& str) {
		assert(this->position + str.length() + sizeof(int16) <= this->length);

		size_t strSize = str.length();

		this->buffer[this->position++] = (uint8)(strSize >> 8 & 0xFF);
		this->buffer[this->position++] = (uint8)(strSize      & 0xFF);

		for (size_t i = 0; i < strSize; ++i) {
			this->buffer[this->position++] = str[i];
		}
	}

	// Read Methods
	uint8 Packet::getByte() {
		assert(this->position + sizeof(uint8) <= this->length);
		return this->buffer[this->position++];
	}

	uint8* Packet::getByte(uint32 size) {
		assert(size != 0 && this->position + size <= this->length);

		uint8* retval = new uint8[size];
		memcpy(retval, this->buffer + this->position, size);
		this->position += size;
		
		return retval;
	}

	int8 Packet::getChar() {
		assert(this->position + sizeof(int8) <= this->length);
		return (int8)(this->buffer[this->position++] & 0xFF);
	}

	bool Packet::getBool() {
		assert(this->position + sizeof(bool) <= this->length);
		return this->buffer[this->position++] == 0x01 ? true : false;
	}


	std::string Packet::getString() {
		assert(this->position + sizeof(int16) <= this->length);
		int16 size = getShort();

		assert(this->position + size <= this->length);
		std::string retval;
		for (int16 i = 0; i < size; ++i) {
			retval += (char)this->buffer[this->position++];
		}

		return retval;
	}

	short Packet::getShort() {
		assert(this->position + sizeof(int16) <= this->length);

		//WHY I CAN'T JUST DO: return (short) (((this->buffer[this->position++] << 8) & 0xFF) | (this->buffer[this->position++] & 0xFF)); 
		//WTF??
		this->position += 2;
		return (short) (this->buffer[this->position - 2] << 8 & 0xFF | this->buffer[this->position - 1] & 0xFF);
	}

	int24 Packet::getTriad() {
		assert(this->position + 3 <= this->length);
		return (
			this->buffer[this->position++] << 16 & 0xFF |
			this->buffer[this->position++] <<  8 & 0xFF |
			this->buffer[this->position++]       & 0xFF
			);
	}

	int Packet::getLTriad() {
		assert(this->position + 3 <= this->length);
		return (
			this->buffer[this->position++]       & 0xFF |
			this->buffer[this->position++] <<  8 & 0xFF |
			this->buffer[this->position++] << 16 & 0xFF
			);
	}

	int32 Packet::getInt() {
		assert(this->position + sizeof(int32) <= this->length);
		return (
			this->buffer[this->position++] << 24 & 0xFF |
			this->buffer[this->position++] << 16 & 0xFF |
			this->buffer[this->position++] <<  8 & 0xFF |
			this->buffer[this->position++]       & 0xFF
		);
	}

	f32 Packet::getFloat() {
		return (f32)this->getInt();
	}

	int64 Packet::getLong() {
		assert(this->position + sizeof(int64) <= this->length);
		return (int64)
			(
			(int64)this->buffer[this->position++] << 56 & 0xFF |
			(int64)this->buffer[this->position++] << 48 & 0xFF |
			(int64)this->buffer[this->position++] << 40 & 0xFF |
			(int64)this->buffer[this->position++] << 32 & 0xFF |
			(int64)this->buffer[this->position++] << 24 & 0xFF |
			(int64)this->buffer[this->position++] << 16 & 0xFF |
			(int64)this->buffer[this->position++] <<  8 & 0xFF |
			(int64)this->buffer[this->position++]       & 0xFF
			);
	}

	uint64 Packet::getULong() {
		assert(this->position + sizeof(uint64) <= this->length);
		return (uint64)
			(
			(uint64)this->buffer[this->position++] << 56 |
			(uint64)this->buffer[this->position++] << 48 |
			(uint64)this->buffer[this->position++] << 40 |
			(uint64)this->buffer[this->position++] << 32 |
			(uint64)this->buffer[this->position++] << 24 |
			(uint64)this->buffer[this->position++] << 16 |
			(uint64)this->buffer[this->position++] <<  8 |
			(uint64)this->buffer[this->position++]      
			);
	}

	f64 Packet::getDouble() {
		return (f64)this->getLong();
	}


	uint8& Packet::operator[] (uint32 index) {
		assert(index < this->length);
		return this->buffer[index];
	}

	uint32 Packet::getLength() const {
		return this->length;
	}

	uint32 Packet::getPosition() const {
		return this->position;
	}

	uint8* Packet::getBuffer() const {
		return this->buffer;
	}

	void Packet::setPosition(uint32 position) {
		this->position = position;
	}

	void Packet::resize(size_t size) {
		size_t newSize = min(size, length);

		uint8* newBuffer = new uint8[size];
		memcpy(newBuffer, this->buffer, newSize);
		delete[] this->buffer;

		this->buffer = newBuffer;
		this->position = min(this->position, newSize);
		this->length = size;
	}

	void Packet::print() {
		for (uint32 i = 0; i < this->length; ++i) {
			printf("%02X ", this->buffer[i]);
			if (i != 0 && i % 8 == 0 || i == this->length - 1) {
				printf("\n");
			}
		}
	}

	void Packet::clear() {
		memset(this->buffer, 0x00, this->length);
		this->position = 0;
	}

	void Packet::close() {
		delete[] this->buffer;
		this->length = 0;
		this->position = 0;
	}

	Packet::~Packet() {
		this->close();
	}
}