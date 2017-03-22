#include "Packet.h"

#include <cassert>

#include "Swap.h"

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
#if COMMON_LITTLE_ENDIAN
		uint16 swapped = Common::swap16((uint16)v);
		memcpy(this->buffer + this-> position, &swapped, sizeof(uint16));
#else
		memcpy(this->buffer + this->position, &v, sizeof(int16));
#endif
		this->position += sizeof(int16);
	}

	void Packet::putUShort(uint16 v) {
		assert(this->position + sizeof(uint16) <= this->length);
#if COMMON_LITTLE_ENDIAN
		uint16 swapped = Common::swap16(v);
		memcpy(this->buffer + this->position, &swapped, sizeof(uint16));
#else
		memcpy(this->buffer + this->position, &v, sizeof(uint16));
#endif
		this->position += sizeof(uint16);
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

	void Packet::putInt(int32 v) {
		assert(this->position + sizeof(int32) <= this->length);
#if COMMON_LITTLE_ENDIAN
		uint32 swapped = Common::swap32((uint32)v);
		memcpy(this->buffer + this->position, &swapped, sizeof(uint32));
#else
		memcpy(this->buffer + this->position, &v, sizeof(int32));
#endif
		this->position += sizeof(int32);
	}

	void Packet::putUInt(uint32 v) {
		assert(this->position + sizeof(uint32) <= this->length);
#if COMMON_LITTLE_ENDIAN
		uint32 swapped = Common::swap32(v);
		memcpy(this->buffer + this->position, &swapped, sizeof(uint32));
#else
		memcpy(this->buffer + this->position, &v, sizeof(uint32));
#endif
		this->position += sizeof(uint32);
	}

	void Packet::putLong(int64 v) {
		assert(this->position + sizeof(int64) <= this->length);
#if COMMON_LITTLE_ENDIAN
		uint64 swapped = Common::swap64((uint64)v);
		memcpy(this->buffer + this->position, &swapped, sizeof(uint64));
#else
		memcpy(this->buffer + this->position, &v, sizeof(int64));
#endif
		this->position += sizeof(int64);
	}

	void Packet::putULong(uint64 v) {
		assert(this->position + sizeof(uint64) <= this->length);
#if COMMON_LITTLE_ENDIAN
		uint64 swapped = Common::swap64(v);
		memcpy(this->buffer + this->position, &swapped, sizeof(uint64));
#else
		memcpy(this->buffer + this->position, &v, sizeof(uint64));
#endif
		this->position += sizeof(uint64);
	}

	void Packet::putFloat(f32 v) {
		assert(this->position + sizeof(f32) <= this->length);
#if COMMON_LITTLE_ENDIAN
		f32 swapped = Common::swapf(v);
		memcpy(this->buffer + this->position, &swapped, sizeof(f32));
#else
		memcpy(this->buffer + this->position, &v, sizeof(f32));
#endif
		this->position += sizeof(f32);
	}

	void Packet::putDouble(f64 v) {
		assert(this->position + sizeof(f64) <= this->length);
#if COMMON_LITTLE_ENDIAN
		f64 swapped = Common::swapd(v);
		memcpy(this->buffer + this->position, &swapped, sizeof(f64));
#else
		memcpy(this->buffer + this->position, &v, sizeof(f64));
#endif
		this->position += sizeof(f64);
	}

	void Packet::putString(const std::string& str) {
		this->putUShort((uint16)str.length());
		
		assert(this->position + str.length() <= this->length);
		memcpy(this->buffer + this->position, str.data(), str.length());
		this->position += str.length();
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

	int16 Packet::getShort() {
		assert(this->position + sizeof(int16) <= this->length);
		int16 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(int16));
		this->position += sizeof(int16);
#if COMMON_LITTLE_ENDIAN
		return Common::swap16((uint16)value);
#else
		return value;
#endif
	}

	uint16 Packet::getUShort() {
		assert(this->position + sizeof(uint16) <= this->length);
		uint16 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(uint16));
		this->position += sizeof(uint16);
#if COMMON_LITTLE_ENDIAN
		return Common::swap16(value);
#else
		return value;
#endif
	}

	int24 Packet::getTriad() {
		assert(this->position + 3 <= this->length);
		return this->buffer[this->position++] << 16 & 0xFF | this->buffer[this->position++] << 8 & 0xFF | this->buffer[this->position++] & 0xFF;
	}

	int24 Packet::getLTriad() {
		assert(this->position + 3 <= this->length);
		return this->buffer[this->position++] & 0xFF | this->buffer[this->position++] << 8 & 0xFF | this->buffer[this->position++] << 16 & 0xFF;
	}

	int32 Packet::getInt() {
		assert(this->position + sizeof(int32) <= this->length);
		int32 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(int32));
		this->position += sizeof(int32);
#if COMMON_LITTLE_ENDIAN
		return Common::swap32((uint32)value);
#else
		return value;
#endif
	}

	uint32 Packet::getUInt() {
		assert(this->position + sizeof(uint32) <= this->length);
		uint32 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(uint32));
		this->position += sizeof(uint32);
#if COMMON_LITTLE_ENDIAN
		return Common::swap32(value);
#else
		return value;
#endif
	}

	int64 Packet::getLong() {
		assert(this->position + sizeof(int64) <= this->length);
		int64 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(int64));
		this->position += sizeof(int64);
#if COMMON_LITTLE_ENDIAN
		return Common::swap64((uint64)value);
#else
		return value;
#endif
	}

	uint64 Packet::getULong() {
		assert(this->position + sizeof(uint64) <= this->length);
		uint64 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(uint64));
		this->position += sizeof(uint64);
#if COMMON_LITTLE_ENDIAN
		return Common::swap64(value);
#else
		return valuel
#endif;
	}

	f32 Packet::getFloat() {
		assert(this->position + sizeof(f32) <= this->length);
		f32 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(f32));
		this->position += sizeof(f32);
#if COMMON_LITTLE_ENDIAN
		return Common::swapf(value);
#else
		return value;
#endif
	}

	f64 Packet::getDouble() {
		f64 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(f64));
		this->position += sizeof(f64);
#if COMMON_LITTLE_ENDIAN
		return Common::swapd(value);
#else
		return value;
#endif
	}

	std::string Packet::getString() {
		uint16 size = getUShort();

		assert(this->position + size <= this->length);
		std::string retval;
		for (uint16 i = 0; i < size; ++i) {
			retval += (char)this->buffer[this->position++] & 0xFF;
		}

		return retval;
	}

	uint8& Packet::operator[] (uint32 index) const {
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
			if ((i + 1) % 8 == 0 || i == this->length - 1) {
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