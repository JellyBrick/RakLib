#pragma once

#include <string>

#include "Common.h"
#include "ByteBuffer.h"

namespace RakLib {
	class Packet : public ByteBuffer {
	public:
		static const uint32 DEFAULT_BUFFER_SIZE;

	public:
		std::string ip;
		uint16 port;

	public:
		Packet();
		Packet(uint32 size);
		Packet(uint8* buffer, uint32 size, const std::string& ip = "", uint16 port = 0);

		virtual ~Packet();

		/*
		 * Would zero the entire byte buffer
		 */
		void clear();

		/*
		 * Would free byte buffer resource
		 */
		void close();

		void swap(Packet& other) noexcept;

		/*
		 * Would resize byte buffer to desire size
		 * @param size
		 */
		void resize(uint32 size);

		/*
		 * Would release the byte buffer pointer
		 * NOTE: If another object doesn't own the byte buffer pointer this would lead to memory leak
		 */
		void release();

	private:
		// Non-Copyable
		Packet(const Packet&) = delete;
		Packet& operator=(Packet&) = delete;

	};
}
