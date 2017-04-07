#pragma once

#include <string>

#include "Common.h"

/*
 * This class would iterate over an byte buffer.
 * Note: Would not free mentioned buffer when this class get destroyed. Owner of this buffer should take care of that
 */
namespace RakLib {
	class ByteBuffer {
	public:
		enum Endianness{
			LITTLE_ENDIAN,
			BIG_ENDIAN
		};

	protected:
		Endianness endianness;
		uint8* buffer;
		uint32 length;
		uint32 position;

	public:
		ByteBuffer();
		ByteBuffer(uint8* buffer, uint32 length);

		void putByte(uint8 v);
		void putByte(uint8* v, uint32 length);

		void putChar(int8 c);

		void putBool(bool value);

		void putShort(int16 v);
		void putUShort(uint16 v);

		void putTriad(int24 v);
		void putLTriad(int24 v);

		void putInt(int32 v);
		void putUInt(uint32 v);

		void putVarUInt(uint32 v);

		void putLong(int64 v);
		void putULong(uint64 v);

		void putVarULong(uint64 v);

		void putFloat(f32 v);
		void putDouble(f64 v);

		void putString(const std::string& str);

		void putVarString(const std::string& str);

		uint8 getByte();
		uint8* getByte(uint32 size);

		int8 getChar();

		bool getBool();

		int16 getShort();
		uint16 getUShort();

		int24 getTriad();
		int24 getLTriad();

		int32 getInt();
		uint32 getUInt();

		uint32 getVarUInt();

		int64 getLong();
		uint64 getULong();

		uint64 getVarULong();

		f32 getFloat();
		f64 getDouble();

		std::string getString();

		std::string getVarString();

		uint8& operator[] (uint32 index) const;

		uint32 getLength() const;
		uint8* getBuffer() const;

		void setPosition(uint32 newPosition);
		uint32 getPosition() const;

		void setEndianness(Endianness newEndianness);
		Endianness getEndianness() const;

		void print() const;

		static Endianness getSystemEndianness();
	};
}