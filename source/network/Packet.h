#pragma once

#include <string>

#include "../Common.h"
#include <memory>

namespace RakLib {
	class Packet {
	public:
		std::string ip;
		uint16 port;

	protected:
		uint32 length;
		uint32 position;
		uint8* buffer;

	public:
		static const uint32 DEFAULT_BUFFER_SIZE = 1024 * 5;

	public:
		Packet();
		Packet(uint32 size);
		Packet(uint8* buff, uint32 size);
		Packet(uint8* buff, uint32 size, const std::string& ip, uint16 port);
		Packet(std::unique_ptr<Packet> other);
		virtual ~Packet();

		//Write Methods
		void putByte(uint8 v);
		void putByte(uint8* v, uint32 size);

		void putChar(int8 c);

		void putBool(bool value);

		void putShort(int16 v);
		void putUShort(uint16 v);

		void putTriad(int24 v);
		void putLTriad(int24 v);

		void putInt(int32 v);
		void putUInt(uint32 v);

		void putLong(int64 v);
		void putULong(uint64 v);

		void putFloat(f32 v);
		void putDouble(f64 v);

		void putString(const std::string& str);

		// Read Methods
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

		int64 getLong();
		uint64 getULong();

		f32 getFloat();
		f64 getDouble();

		std::string getString();

		uint8& operator[] (uint32 index) const;

		//Properties
		uint32 getLength() const;
		uint8* getBuffer() const;

		void setPosition(uint32);
		uint32 getPosition() const;

		void resize(size_t);
		void print();
		void clear();
		void close();
	};
}
