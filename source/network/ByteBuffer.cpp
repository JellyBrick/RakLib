#include "ByteBuffer.h"

#include <cassert>

#include "Swap.h"

namespace RakLib {
	ByteBuffer::ByteBuffer() : endianness(BIG_ENDIAN), buffer(nullptr), length(0), position(0) {}

	ByteBuffer::ByteBuffer(uint8* buffer, uint32 size) : endianness(BIG_ENDIAN), buffer(buffer), length(size), position(0) {
		assert(buffer != nullptr && size != 0);
	}

	void ByteBuffer::putByte(uint8 v) {
		assert(this->position + sizeof(uint8) <= this->length);
		this->buffer[this->position++] = v;
	}

	void ByteBuffer::putByte(uint8* bytes, uint32 size) {
		assert(this->position + size <= this->length);
		
		if (size > 0) {
			memcpy(this->buffer + this->position, bytes, size);
			this->position += size;
		}
	}

	void ByteBuffer::putChar(int8 c) {
		assert(this->position + sizeof(int8) <= this->length);
		this->buffer[this->position++] = c;
	}

	void ByteBuffer::putBool(bool value) {
		assert(this->position + sizeof(bool) <= this->length);
		this->buffer[this->position++] = value ? 0x01 : 0x00;
	}

	void ByteBuffer::putShort(int16 v) {
		assert(this->position + sizeof(int16) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = static_cast<int16>(Common::swap16(static_cast<uint16>(v)));
		}

		memcpy(this->buffer + this->position, &v, sizeof(int16));
		this->position += sizeof(int16);
	}

	void ByteBuffer::putUShort(uint16 v) {
		assert(this->position + sizeof(uint16) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = Common::swap16(v);
		}

		memcpy(this->buffer + this->position, &v, sizeof(uint16));
		this->position += sizeof(uint16);
	}

	void ByteBuffer::putTriad(int24 v) {
		assert(this->position + 3 <= this->length);
		this->buffer[this->position++] = (uint8)(v >> 16 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 8 & 0xFF);
		this->buffer[this->position++] = (uint8)(v & 0xFF);
	}

	void ByteBuffer::putLTriad(int24 v) {
		assert(this->position + 3 <= this->length);
		this->buffer[this->position++] = (uint8)(v & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 8 & 0xFF);
		this->buffer[this->position++] = (uint8)(v >> 16 & 0xFF);
	}

	void ByteBuffer::putInt(int32 v) {
		assert(this->position + sizeof(int32) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = static_cast<int32>(Common::swap32(static_cast<uint32>(v)));
		}

		memcpy(this->buffer + this->position, &v, sizeof(int32));
		this->position += sizeof(int32);
	}

	void ByteBuffer::putUInt(uint32 v) {
		assert(this->position + sizeof(uint32) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = Common::swap32(v);
		}

		memcpy(this->buffer + this->position, &v, sizeof(uint32));
		this->position += sizeof(uint32);
	}

	void ByteBuffer::putVarUInt(uint32 v) {;
		while (v > 127) {
			this->buffer[this->position++] = ((uint8_t)(v & 127)) | 128;
			v >>= 7;
		}

		this->buffer[this->position++] = ((uint8_t)v) & 127;
	}

	void ByteBuffer::putLong(int64 v) {
		assert(this->position + sizeof(int64) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = static_cast<int64>(Common::swap64(static_cast<uint64>(v)));
		}

		memcpy(this->buffer + this->position, &v, sizeof(int64));
		this->position += sizeof(int64);
	}

	void ByteBuffer::putULong(uint64 v) {
		assert(this->position + sizeof(uint64) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = Common::swap64(v);
		}

		memcpy(this->buffer + this->position, &v, sizeof(uint64));
		this->position += sizeof(uint64);
	}

	void ByteBuffer::putVarULong(uint64 v) {
		while (v > 127) {
			this->buffer[this->position++] = ((uint8_t)(v & 127)) | 128;
			v >>= 7;
		}

		this->buffer[this->position++] = ((uint8_t)v) & 127;
	}

	void ByteBuffer::putFloat(f32 v) {
		assert(this->position + sizeof(f32) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = Common::swapf(v);
		}

		memcpy(this->buffer + this->position, &v, sizeof(f32));
		this->position += sizeof(f32);
	}

	void ByteBuffer::putDouble(f64 v) {
		assert(this->position + sizeof(f64) <= this->length);

		if (ByteBuffer::getSystemEndianness() != this->endianness) {
			v = Common::swapd(v);
		}

		memcpy(this->buffer + this->position, &v, sizeof(f64));
		this->position += sizeof(f64);
	}

	void ByteBuffer::putString(const std::string& str) {
		this->putUShort((uint16)str.length());

		if (!str.empty()) {
			assert(this->position + str.length() <= this->length);
			memcpy(this->buffer + this->position, str.data(), str.length());
			this->position += str.length();
		}
	}

	void ByteBuffer::putVarString(const std::string& str) {
		this->putVarUInt((uint32)str.length());

		if (!str.empty()) {
			assert(this->position + str.length() <= this->length);
			memcpy(this->buffer + this->position, str.data(), str.length());
			this->position += str.length();
		}
	}

	// Read Methods
	uint8 ByteBuffer::getByte() {
		assert(this->position + sizeof(uint8) <= this->length);
		return this->buffer[this->position++];
	}

	uint8* ByteBuffer::getByte(uint32 size) {
		assert(this->position + size <= this->length);

		if (size == 0) {
			return nullptr;
		}

		uint8* retval = new uint8[size];
		memcpy(retval, this->buffer + this->position, size);
		this->position += size;

		return retval;
	}

	int8 ByteBuffer::getChar() {
		assert(this->position + sizeof(int8) <= this->length);
		return (int8)(this->buffer[this->position++] & 0xFF);
	}

	bool ByteBuffer::getBool() {
		assert(this->position + sizeof(bool) <= this->length);
		return this->buffer[this->position++] == 0x01 ? true : false;
	}

	int16 ByteBuffer::getShort() {
		assert(this->position + sizeof(int16) <= this->length);
		int16 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(int16));
		this->position += sizeof(int16);
		return ByteBuffer::getSystemEndianness() != this->endianness ? static_cast<int16>(Common::swap16(static_cast<uint16>(value))) : value;
	}

	uint16 ByteBuffer::getUShort() {
		assert(this->position + sizeof(uint16) <= this->length);
		uint16 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(uint16));
		this->position += sizeof(uint16);
		return ByteBuffer::getSystemEndianness() != this->endianness ? Common::swap16(value) : value;
	}

	int24 ByteBuffer::getTriad() {
		assert(this->position + 3 <= this->length);
		return this->buffer[this->position++] << 16 & 0xFF | this->buffer[this->position++] << 8 & 0xFF | this->buffer[this->position++] & 0xFF;
	}

	int24 ByteBuffer::getLTriad() {
		assert(this->position + 3 <= this->length);
		return this->buffer[this->position++] & 0xFF | this->buffer[this->position++] << 8 & 0xFF | this->buffer[this->position++] << 16 & 0xFF;
	}

	int32 ByteBuffer::getInt() {
		assert(this->position + sizeof(int32) <= this->length);
		int32 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(int32));
		this->position += sizeof(int32);
		return ByteBuffer::getSystemEndianness() != this->endianness ? static_cast<int32>(Common::swap32(static_cast<uint32>(value))) : value;
	}

	uint32 ByteBuffer::getUInt() {
		assert(this->position + sizeof(uint32) <= this->length);
		uint32 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(uint32));
		this->position += sizeof(uint32);
		return ByteBuffer::getSystemEndianness() != this->endianness ? Common::swap32(value) : value;
	}

	uint32 ByteBuffer::getVarUInt() {
		uint32 ret = 0;
		for (size_t i = 0; i < 5; ++i) {
			ret |= (this->buffer[this->position] & 127) << (7 * i);
			if (!(this->buffer[this->position++] & 128)) {
				break;
			}
		}
		return ret;
	}

	int64 ByteBuffer::getLong() {
		assert(this->position + sizeof(int64) <= this->length);
		int64 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(int64));
		this->position += sizeof(int64);
		return ByteBuffer::getSystemEndianness() != this->endianness ? static_cast<int64>(Common::swap64(static_cast<uint64>(value))) : value;
	}

	uint64 ByteBuffer::getULong() {
		assert(this->position + sizeof(uint64) <= this->length);
		uint64 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(uint64));
		this->position += sizeof(uint64);
		return ByteBuffer::getSystemEndianness() != this->endianness ? Common::swap64(value) : value;
	}

	uint64 ByteBuffer::getVarULong() {
		uint32 ret = 0;
		for (uint32 i = 0; i < 10; ++i) {
			ret |= (this->buffer[this->position] & 127) << (7 * i);
			if (!(this->buffer[this->position++] & 128)) {
				break;
			}
		}
		return ret;
	}

	f32 ByteBuffer::getFloat() {
		assert(this->position + sizeof(f32) <= this->length);
		f32 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(f32));
		this->position += sizeof(f32);
		return ByteBuffer::getSystemEndianness() != this->endianness ? Common::swapf(value) : value;
	}

	f64 ByteBuffer::getDouble() {
		f64 value = 0;
		memcpy(&value, this->buffer + this->position, sizeof(f64));
		this->position += sizeof(f64);
		return ByteBuffer::getSystemEndianness() != this->endianness ? Common::swapd(value) : value;
	}

	std::string ByteBuffer::getString() {
		uint16 size = getUShort();

		std::string retval = "";
		if (size > 0) {
			assert(this->position + size <= this->length);
			for (uint16 i = 0; i < size; ++i) {
				retval += (char)this->buffer[this->position++] & 0xFF;
			}

			return retval;
		}
		return retval;
	}

	std::string ByteBuffer::getVarString() {
		uint32 size = getVarUInt();

		std::string retval = "";
		if (size > 0) {
			assert(this->position + size <= this->length);
			for (uint32 i = 0; i < size; ++i) {
				retval += (char)this->buffer[this->position++] & 0xFF;
			}

			return retval;
		}

		return retval;
	}

	uint8& ByteBuffer::operator[] (uint32 index) const {
		assert(index < this->length);
		return this->buffer[index];
	}

	uint32 ByteBuffer::getLength() const {
		return this->length;
	}

	uint8* ByteBuffer::getBuffer() const {
		return this->buffer;
	}

	void ByteBuffer::setPosition(uint32 newPosition) {
		this->position = newPosition;
	}

	uint32 ByteBuffer::getPosition() const {
		return this->position;
	}

	void ByteBuffer::print() const {
		for (uint32 i = 0; i < this->length; ++i) {
			printf("%02X ", this->buffer[i]);
			if ((i + 1) % 8 == 0 || i == this->length - 1) {
				printf("\n");
			}
		}
	}

	void ByteBuffer::setEndianness(enum Endianness newEndianness) {
		this->endianness = newEndianness;
	}

	enum ByteBuffer::Endianness ByteBuffer::getEndianness() const {
		return this->endianness;
	}

	ByteBuffer::Endianness ByteBuffer::getSystemEndianness() {
#if COMMON_LITTLE_ENDIAN
		return Endianness::LITTLE_ENDIAN;
#else
		return Endianness::BIG_ENDIAN;
#endif
	}

}