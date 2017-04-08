#include "ByteBuffer.h"

#include <cassert>

#include "Swap.h"

namespace RakLib {
	ByteBuffer::ByteBuffer() : endianness(Endianness::BIG), buffer(nullptr), length(0), position(0) {}

	ByteBuffer::ByteBuffer(uint8* buffer, uint32 size) : endianness(Endianness::BIG), buffer(buffer), length(size), position(0) {
		assert(buffer != nullptr && size != 0);
	}

	void ByteBuffer::putByte(uint8 v) {
		assert(position + sizeof(uint8) <= length);
		buffer[position++] = v;
	}

	void ByteBuffer::putByte(uint8* bytes, uint32 size) {
		assert(position + size <= length);
		
		if (size > 0) {
			memcpy(buffer + position, bytes, size);
			position += size;
		}
	}

	void ByteBuffer::putChar(int8 c) {
		assert(position + sizeof(int8) <= length);
		buffer[position++] = c;
	}

	void ByteBuffer::putBool(bool value) {
		assert(position + sizeof(bool) <= length);
		buffer[position++] = value ? 0x01 : 0x00;
	}

	void ByteBuffer::putShort(int16 v) {
		assert(position + sizeof(int16) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = static_cast<int16>(Common::swap16(static_cast<uint16>(v)));
		}

		memcpy(buffer + position, &v, sizeof(int16));
		position += sizeof(int16);
	}

	void ByteBuffer::putUShort(uint16 v) {
		assert(position + sizeof(uint16) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = Common::swap16(v);
		}

		memcpy(buffer + position, &v, sizeof(uint16));
		position += sizeof(uint16);
	}

	void ByteBuffer::putTriad(int24 v) {
		assert(position + 3 <= length);
		buffer[position++] = static_cast<uint8>(v >> 16 & 0xFF);
		buffer[position++] = static_cast<uint8>(v >> 8 & 0xFF);
		buffer[position++] = static_cast<uint8>(v & 0xFF);
	}

	void ByteBuffer::putLTriad(int24 v) {
		assert(position + 3 <= length);
		buffer[position++] = static_cast<uint8>(v & 0xFF);
		buffer[position++] = static_cast<uint8>(v >> 8 & 0xFF);
		buffer[position++] = static_cast<uint8>(v >> 16 & 0xFF);
	}

	void ByteBuffer::putInt(int32 v) {
		assert(position + sizeof(int32) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = static_cast<int32>(Common::swap32(static_cast<uint32>(v)));
		}

		memcpy(buffer + position, &v, sizeof(int32));
		position += sizeof(int32);
	}

	void ByteBuffer::putUInt(uint32 v) {
		assert(position + sizeof(uint32) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = Common::swap32(v);
		}

		memcpy(buffer + position, &v, sizeof(uint32));
		position += sizeof(uint32);
	}

	void ByteBuffer::putVarUInt(uint32 v) {;
		while (v > 127) {
			buffer[position++] = ((uint8_t)(v & 127)) | 128;
			v >>= 7;
		}

		buffer[position++] = ((uint8_t)v) & 127;
	}

	void ByteBuffer::putLong(int64 v) {
		assert(position + sizeof(int64) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = static_cast<int64>(Common::swap64(static_cast<uint64>(v)));
		}

		memcpy(buffer + position, &v, sizeof(int64));
		position += sizeof(int64);
	}

	void ByteBuffer::putULong(uint64 v) {
		assert(position + sizeof(uint64) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = Common::swap64(v);
		}

		memcpy(buffer + position, &v, sizeof(uint64));
		position += sizeof(uint64);
	}

	void ByteBuffer::putVarULong(uint64 v) {
		while (v > 127) {
			buffer[position++] = ((uint8_t)(v & 127)) | 128;
			v >>= 7;
		}

		buffer[position++] = ((uint8_t)v) & 127;
	}

	void ByteBuffer::putFloat(f32 v) {
		assert(position + sizeof(f32) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = Common::swapf(v);
		}

		memcpy(buffer + position, &v, sizeof(f32));
		position += sizeof(f32);
	}

	void ByteBuffer::putDouble(f64 v) {
		assert(position + sizeof(f64) <= length);

		if (ByteBuffer::getSystemEndianness() != endianness) {
			v = Common::swapd(v);
		}

		memcpy(buffer + position, &v, sizeof(f64));
		position += sizeof(f64);
	}

	void ByteBuffer::putString(const std::string& str) {
		putUShort(static_cast<uint16>(str.length()));

		if (!str.empty()) {
			assert(position + str.length() <= length);
			memcpy(buffer + position, str.data(), str.length());
			position += str.length();
		}
	}

	void ByteBuffer::putVarString(const std::string& str) {
		putVarUInt(static_cast<uint32>(str.length()));

		if (!str.empty()) {
			assert(position + str.length() <= length);
			memcpy(buffer + position, str.data(), str.length());
			position += str.length();
		}
	}

	// Read Methods
	uint8 ByteBuffer::getByte() {
		assert(position + sizeof(uint8) <= length);
		return buffer[position++];
	}

	uint8* ByteBuffer::getByte(uint32 size) {
		assert(position + size <= length);

		if (size == 0) {
			return nullptr;
		}

		uint8* retval = new uint8[size];
		memcpy(retval, buffer + position, size);
		position += size;

		return retval;
	}

	int8 ByteBuffer::getChar() {
		assert(position + sizeof(int8) <= length);
		return static_cast<int8>(buffer[position++] & 0xFF);
	}

	bool ByteBuffer::getBool() {
		assert(position + sizeof(bool) <= length);
		return buffer[position++] == 0x01 ? true : false;
	}

	int16 ByteBuffer::getShort() {
		assert(position + sizeof(int16) <= length);
		int16 value = 0;
		memcpy(&value, buffer + position, sizeof(int16));
		position += sizeof(int16);
		return ByteBuffer::getSystemEndianness() != endianness ? static_cast<int16>(Common::swap16(static_cast<uint16>(value))) : value;
	}

	uint16 ByteBuffer::getUShort() {
		assert(position + sizeof(uint16) <= length);
		uint16 value = 0;
		memcpy(&value, buffer + position, sizeof(uint16));
		position += sizeof(uint16);
		return ByteBuffer::getSystemEndianness() != endianness ? Common::swap16(value) : value;
	}

	int24 ByteBuffer::getTriad() {
		assert(position + 3 <= length);
		return buffer[position++] << 16 & 0xFF | buffer[position++] << 8 & 0xFF | buffer[position++] & 0xFF;
	}

	int24 ByteBuffer::getLTriad() {
		assert(position + 3 <= length);
		return buffer[position++] & 0xFF | buffer[position++] << 8 & 0xFF | buffer[position++] << 16 & 0xFF;
	}

	int32 ByteBuffer::getInt() {
		assert(position + sizeof(int32) <= length);
		int32 value = 0;
		memcpy(&value, buffer + position, sizeof(int32));
		position += sizeof(int32);
		return ByteBuffer::getSystemEndianness() != endianness ? static_cast<int32>(Common::swap32(static_cast<uint32>(value))) : value;
	}

	uint32 ByteBuffer::getUInt() {
		assert(position + sizeof(uint32) <= length);
		uint32 value = 0;
		memcpy(&value, buffer + position, sizeof(uint32));
		position += sizeof(uint32);
		return ByteBuffer::getSystemEndianness() != endianness ? Common::swap32(value) : value;
	}

	uint32 ByteBuffer::getVarUInt() {
		uint32 ret = 0;
		for (size_t i = 0; i < 5; ++i) {
			ret |= (buffer[position] & 127) << (7 * i);
			if (!(buffer[position++] & 128)) {
				break;
			}
		}
		return ret;
	}

	int64 ByteBuffer::getLong() {
		assert(position + sizeof(int64) <= length);
		int64 value = 0;
		memcpy(&value, buffer + position, sizeof(int64));
		position += sizeof(int64);
		return ByteBuffer::getSystemEndianness() != endianness ? static_cast<int64>(Common::swap64(static_cast<uint64>(value))) : value;
	}

	uint64 ByteBuffer::getULong() {
		assert(position + sizeof(uint64) <= length);
		uint64 value = 0;
		memcpy(&value, buffer + position, sizeof(uint64));
		position += sizeof(uint64);
		return ByteBuffer::getSystemEndianness() != endianness ? Common::swap64(value) : value;
	}

	uint64 ByteBuffer::getVarULong() {
		uint32 ret = 0;
		for (uint32 i = 0; i < 10; ++i) {
			ret |= (buffer[position] & 127) << (7 * i);
			if (!(buffer[position++] & 128)) {
				break;
			}
		}
		return ret;
	}

	f32 ByteBuffer::getFloat() {
		assert(position + sizeof(f32) <= length);
		f32 value = 0;
		memcpy(&value, buffer + position, sizeof(f32));
		position += sizeof(f32);
		return ByteBuffer::getSystemEndianness() != endianness ? Common::swapf(value) : value;
	}

	f64 ByteBuffer::getDouble() {
		f64 value = 0;
		memcpy(&value, buffer + position, sizeof(f64));
		position += sizeof(f64);
		return ByteBuffer::getSystemEndianness() != endianness ? Common::swapd(value) : value;
	}

	std::string ByteBuffer::getString() {
		uint16 size = getUShort();

		std::string retval = "";
		if (size > 0) {
			assert(position + size <= length);
			for (uint16 i = 0; i < size; ++i) {
				retval += (char)buffer[position++] & 0xFF;
			}

			return retval;
		}
		return retval;
	}

	std::string ByteBuffer::getVarString() {
		uint32 size = getVarUInt();

		std::string retval = "";
		if (size > 0) {
			assert(position + size <= length);
			for (uint32 i = 0; i < size; ++i) {
				retval += (char)buffer[position++] & 0xFF;
			}

			return retval;
		}

		return retval;
	}

	uint8& ByteBuffer::operator[] (uint32 index) const {
		assert(index < length);
		return buffer[index];
	}

	uint32 ByteBuffer::getLength() const {
		return length;
	}

	uint8* ByteBuffer::getBuffer() const {
		return buffer;
	}

	void ByteBuffer::setPosition(uint32 newPosition) {
		position = newPosition;
	}

	uint32 ByteBuffer::getPosition() const {
		return position;
	}

	void ByteBuffer::print() const {
		for (uint32 i = 0; i < length; ++i) {
			printf("%02X ", buffer[i]);
			if ((i + 1) % 8 == 0 || i == length - 1) {
				printf("\n");
			}
		}
	}

	void ByteBuffer::setEndianness(enum Endianness newEndianness) {
		endianness = newEndianness;
	}

	enum ByteBuffer::Endianness ByteBuffer::getEndianness() const {
		return endianness;
	}

	ByteBuffer::Endianness ByteBuffer::getSystemEndianness() {
#if COMMON_LITTLE_ENDIAN
		return Endianness::LITTLE;
#else
		return Endianness::BIG;
#endif
	}
}