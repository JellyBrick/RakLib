// Copyright (c) 2012- PPSSPP Project / Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

#if defined(_MSC_VER)
#include <cstdlib>
#elif defined(__linux__)
#include <byteswap.h>
#elif defined(__Bitrig__) || defined(__DragonFly__) || defined(__FreeBSD__) ||                     \
    defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/endian.h>
#endif

#include <cstring>

#include "Common.h"

// GCC 4.6+
#if __GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)

#if __BYTE_ORDER__ && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && !defined(COMMON_LITTLE_ENDIAN)
#define COMMON_LITTLE_ENDIAN 1
#elif __BYTE_ORDER__ && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) && !defined(COMMON_BIG_ENDIAN)
#define COMMON_BIG_ENDIAN 1
#endif

// LLVM/clang
#elif __clang__

#if __LITTLE_ENDIAN__ && !defined(COMMON_LITTLE_ENDIAN)
#define COMMON_LITTLE_ENDIAN 1
#elif __BIG_ENDIAN__ && !defined(COMMON_BIG_ENDIAN)
#define COMMON_BIG_ENDIAN 1
#endif

// MSVC
#elif defined(_MSC_VER) && !defined(COMMON_BIG_ENDIAN) && !defined(COMMON_LITTLE_ENDIAN)

#define COMMON_LITTLE_ENDIAN 1
#endif

// Worst case, default to little endian.
#if !COMMON_BIG_ENDIAN && !COMMON_LITTLE_ENDIAN
#define COMMON_LITTLE_ENDIAN 1
#endif

namespace Common {

#ifdef _MSC_VER
	inline uint16 swap16(uint16 _data) {
		return _byteswap_ushort(_data);
	}
	inline uint32 swap32(uint32 _data) {
		return _byteswap_ulong(_data);
	}
	inline uint64 swap64(uint64 _data) {
		return _byteswap_uint64(_data);
	}
#elif _M_ARM
	inline uint16 swap16(uint16 _data) {
		uint32 data = _data;
		__asm__("rev16 %0, %1\n" : "=l"(data) : "l"(data));
		return (uint16)data;
	}
	inline uint32 swap32(uint32 _data) {
		__asm__("rev %0, %1\n" : "=l"(_data) : "l"(_data));
		return _data;
	}
	inline uint64 swap64(uint64 _data) {
		return ((uint64)swap32(_data) << 32) | swap32(_data >> 32);
	}
#elif __linux__
	inline uint16 swap16(uint16 _data) {
		return bswap_16(_data);
	}
	inline uint32 swap32(uint32 _data) {
		return bswap_32(_data);
	}
	inline uint64 swap64(uint64 _data) {
		return bswap_64(_data);
	}
#elif __APPLE__
	inline __attribute__((always_inline)) uint16 swap16(uint16 _data) {
		return (_data >> 8) | (_data << 8);
	}
	inline __attribute__((always_inline)) uint32 swap32(uint32 _data) {
		return __builtin_bswap32(_data);
	}
	inline __attribute__((always_inline)) uint64 swap64(uint64 _data) {
		return __builtin_bswap64(_data);
	}
#elif defined(__Bitrig__) || defined(__OpenBSD__)
	// swap16, swap32, swap64 are left as is
#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__)
	inline uint16 swap16(uint16 _data) {
		return bswap16(_data);
	}
	inline uint32 swap32(uint32 _data) {
		return bswap32(_data);
	}
	inline uint64 swap64(uint64 _data) {
		return bswap64(_data);
	}
#else
	// Slow generic implementation.
	inline uint16 swap16(uint16 data) {
		return (data >> 8) | (data << 8);
	}
	inline uint32 swap32(uint32 data) {
		return (swap16(data) << 16) | swap16(data >> 16);
	}
	inline uint64 swap64(uint64 data) {
		return ((uint64)swap32(data) << 32) | swap32(data >> 32);
	}
#endif

	inline f32 swapf(f32 f) {
		static_assert(sizeof(uint32) == sizeof(f32), "f32 must be the same size as uint32_t.");

		uint32 value;
		std::memcpy(&value, &f, sizeof(uint32));

		value = swap32(value);
		std::memcpy(&f, &value, sizeof(uint32));

		return f;
	}

	inline f64 swapd(f64 f) {
		static_assert(sizeof(uint64) == sizeof(f64), "f64 must be the same size as uint64_t.");

		uint64 value;
		std::memcpy(&value, &f, sizeof(uint64));

		value = swap64(value);
		std::memcpy(&f, &value, sizeof(uint64));

		return f;
	}

} // Namespace Common