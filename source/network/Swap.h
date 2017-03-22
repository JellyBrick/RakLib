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

	inline float swapf(float f) {
		static_assert(sizeof(uint32) == sizeof(float), "float must be the same size as uint32_t.");

		uint32 value;
		std::memcpy(&value, &f, sizeof(uint32));

		value = swap32(value);
		std::memcpy(&f, &value, sizeof(uint32));

		return f;
	}

	inline double swapd(double f) {
		static_assert(sizeof(uint64) == sizeof(double), "double must be the same size as uint64_t.");

		uint64 value;
		std::memcpy(&value, &f, sizeof(uint64));

		value = swap64(value);
		std::memcpy(&f, &value, sizeof(uint64));

		return f;
	}

} // Namespace Common

template <typename T, typename F>
struct swap_struct_t {
	typedef swap_struct_t<T, F> swapped_t;

protected:
	T value = T();

	static T swap(T v) {
		return F::swap(v);
	}

public:
	T const swap() const {
		return swap(value);
	}
	swap_struct_t() = default;
	swap_struct_t(const T& v) : value(swap(v)) {}

	template <typename S>
	swapped_t& operator=(const S& source) {
		value = swap((T)source);
		return *this;
	}

	operator int8() const {
		return (int8)swap();
	}
	operator uint8() const {
		return (uint8)swap();
	}
	operator int16() const {
		return (int16)swap();
	}
	operator uint16() const {
		return (uint16)swap();
	}
	operator int32() const {
		return (int32)swap();
	}
	operator uint32() const {
		return (uint32)swap();
	}
	operator int64() const {
		return (int64)swap();
	}
	operator uint64() const {
		return (uint64)swap();
	}
	operator float() const {
		return (float)swap();
	}
	operator double() const {
		return (double)swap();
	}

	// +v
	swapped_t operator+() const {
		return +swap();
	}
	// -v
	swapped_t operator-() const {
		return -swap();
	}

	// v / 5
	swapped_t operator/(const swapped_t& i) const {
		return swap() / i.swap();
	}
	template <typename S>
	swapped_t operator/(const S& i) const {
		return swap() / i;
	}

	// v * 5
	swapped_t operator*(const swapped_t& i) const {
		return swap() * i.swap();
	}
	template <typename S>
	swapped_t operator*(const S& i) const {
		return swap() * i;
	}

	// v + 5
	swapped_t operator+(const swapped_t& i) const {
		return swap() + i.swap();
	}
	template <typename S>
	swapped_t operator+(const S& i) const {
		return swap() + (T)i;
	}
	// v - 5
	swapped_t operator-(const swapped_t& i) const {
		return swap() - i.swap();
	}
	template <typename S>
	swapped_t operator-(const S& i) const {
		return swap() - (T)i;
	}

	// v += 5
	swapped_t& operator+=(const swapped_t& i) {
		value = swap(swap() + i.swap());
		return *this;
	}
	template <typename S>
	swapped_t& operator+=(const S& i) {
		value = swap(swap() + (T)i);
		return *this;
	}
	// v -= 5
	swapped_t& operator-=(const swapped_t& i) {
		value = swap(swap() - i.swap());
		return *this;
	}
	template <typename S>
	swapped_t& operator-=(const S& i) {
		value = swap(swap() - (T)i);
		return *this;
	}

	// ++v
	swapped_t& operator++() {
		value = swap(swap() + 1);
		return *this;
	}
	// --v
	swapped_t& operator--() {
		value = swap(swap() - 1);
		return *this;
	}

	// v++
	swapped_t operator++(int) {
		swapped_t old = *this;
		value = swap(swap() + 1);
		return old;
	}
	// v--
	swapped_t operator--(int) {
		swapped_t old = *this;
		value = swap(swap() - 1);
		return old;
	}
	// Comparaison
	// v == i
	bool operator==(const swapped_t& i) const {
		return swap() == i.swap();
	}
	template <typename S>
	bool operator==(const S& i) const {
		return swap() == i;
	}

	// v != i
	bool operator!=(const swapped_t& i) const {
		return swap() != i.swap();
	}
	template <typename S>
	bool operator!=(const S& i) const {
		return swap() != i;
	}

	// v > i
	bool operator>(const swapped_t& i) const {
		return swap() > i.swap();
	}
	template <typename S>
	bool operator>(const S& i) const {
		return swap() > i;
	}

	// v < i
	bool operator<(const swapped_t& i) const {
		return swap() < i.swap();
	}
	template <typename S>
	bool operator<(const S& i) const {
		return swap() < i;
	}

	// v >= i
	bool operator>=(const swapped_t& i) const {
		return swap() >= i.swap();
	}
	template <typename S>
	bool operator>=(const S& i) const {
		return swap() >= i;
	}

	// v <= i
	bool operator<=(const swapped_t& i) const {
		return swap() <= i.swap();
	}
	template <typename S>
	bool operator<=(const S& i) const {
		return swap() <= i;
	}

	// logical
	swapped_t operator!() const {
		return !swap();
	}

	// bitmath
	swapped_t operator~() const {
		return ~swap();
	}

	swapped_t operator&(const swapped_t& b) const {
		return swap() & b.swap();
	}
	template <typename S>
	swapped_t operator&(const S& b) const {
		return swap() & b;
	}
	swapped_t& operator&=(const swapped_t& b) {
		value = swap(swap() & b.swap());
		return *this;
	}
	template <typename S>
	swapped_t& operator&=(const S b) {
		value = swap(swap() & b);
		return *this;
	}

	swapped_t operator|(const swapped_t& b) const {
		return swap() | b.swap();
	}
	template <typename S>
	swapped_t operator|(const S& b) const {
		return swap() | b;
	}
	swapped_t& operator|=(const swapped_t& b) {
		value = swap(swap() | b.swap());
		return *this;
	}
	template <typename S>
	swapped_t& operator|=(const S& b) {
		value = swap(swap() | b);
		return *this;
	}

	swapped_t operator^(const swapped_t& b) const {
		return swap() ^ b.swap();
	}
	template <typename S>
	swapped_t operator^(const S& b) const {
		return swap() ^ b;
	}
	swapped_t& operator^=(const swapped_t& b) {
		value = swap(swap() ^ b.swap());
		return *this;
	}
	template <typename S>
	swapped_t& operator^=(const S& b) {
		value = swap(swap() ^ b);
		return *this;
	}

	template <typename S>
	swapped_t operator<<(const S& b) const {
		return swap() << b;
	}
	template <typename S>
	swapped_t& operator<<=(const S& b) const {
		value = swap(swap() << b);
		return *this;
	}

	template <typename S>
	swapped_t operator >> (const S& b) const {
		return swap() >> b;
	}
	template <typename S>
	swapped_t& operator>>=(const S& b) const {
		value = swap(swap() >> b);
		return *this;
	}

	// Member
	/** todo **/

	// Arithmetics
	template <typename S, typename T2, typename F2>
	friend S operator+(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend S operator-(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend S operator/(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend S operator*(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend S operator%(const S& p, const swapped_t v);

	// Arithmetics + assignements
	template <typename S, typename T2, typename F2>
	friend S operator+=(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend S operator-=(const S& p, const swapped_t v);

	// Bitmath
	template <typename S, typename T2, typename F2>
	friend S operator&(const S& p, const swapped_t v);

	// Comparison
	template <typename S, typename T2, typename F2>
	friend bool operator<(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend bool operator>(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend bool operator<=(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend bool operator>=(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend bool operator!=(const S& p, const swapped_t v);

	template <typename S, typename T2, typename F2>
	friend bool operator==(const S& p, const swapped_t v);
};

// Arithmetics
template <typename S, typename T, typename F>
S operator+(const S& i, const swap_struct_t<T, F> v) {
	return i + v.swap();
}

template <typename S, typename T, typename F>
S operator-(const S& i, const swap_struct_t<T, F> v) {
	return i - v.swap();
}

template <typename S, typename T, typename F>
S operator/(const S& i, const swap_struct_t<T, F> v) {
	return i / v.swap();
}

template <typename S, typename T, typename F>
S operator*(const S& i, const swap_struct_t<T, F> v) {
	return i * v.swap();
}

template <typename S, typename T, typename F>
S operator%(const S& i, const swap_struct_t<T, F> v) {
	return i % v.swap();
}

// Arithmetics + assignements
template <typename S, typename T, typename F>
S& operator+=(S& i, const swap_struct_t<T, F> v) {
	i += v.swap();
	return i;
}

template <typename S, typename T, typename F>
S& operator-=(S& i, const swap_struct_t<T, F> v) {
	i -= v.swap();
	return i;
}

// Logical
template <typename S, typename T, typename F>
S operator&(const S& i, const swap_struct_t<T, F> v) {
	return i & v.swap();
}

template <typename S, typename T, typename F>
S operator&(const swap_struct_t<T, F> v, const S& i) {
	return (S)(v.swap() & i);
}

// Comparaison
template <typename S, typename T, typename F>
bool operator<(const S& p, const swap_struct_t<T, F> v) {
	return p < v.swap();
}
template <typename S, typename T, typename F>
bool operator>(const S& p, const swap_struct_t<T, F> v) {
	return p > v.swap();
}
template <typename S, typename T, typename F>
bool operator<=(const S& p, const swap_struct_t<T, F> v) {
	return p <= v.swap();
}
template <typename S, typename T, typename F>
bool operator>=(const S& p, const swap_struct_t<T, F> v) {
	return p >= v.swap();
}
template <typename S, typename T, typename F>
bool operator!=(const S& p, const swap_struct_t<T, F> v) {
	return p != v.swap();
}
template <typename S, typename T, typename F>
bool operator==(const S& p, const swap_struct_t<T, F> v) {
	return p == v.swap();
}

template <typename T>
struct swap_64_t {
	static T swap(T x) {
		return static_cast<T>(Common::swap64(x));
	}
};

template <typename T>
struct swap_32_t {
	static T swap(T x) {
		return static_cast<T>(Common::swap32(x));
	}
};

template <typename T>
struct swap_16_t {
	static T swap(T x) {
		return static_cast<T>(Common::swap16(x));
	}
};

template <typename T>
struct swap_float_t {
	static T swap(T x) {
		return static_cast<T>(Common::swapf(x));
	}
};

template <typename T>
struct swap_double_t {
	static T swap(T x) {
		return static_cast<T>(Common::swapd(x));
	}
};

#if COMMON_LITTLE_ENDIAN
typedef uint16 uint16_le;
typedef uint32 uint32_le;
typedef uint64 uint64_le;

typedef int16 int16_le;
typedef int32 int32_le;
typedef int64 int64_le;

typedef f32 f32_le;
typedef f64 f64_le;

typedef swap_struct_t<uint64, swap_64_t<uint64>> uint64_be;
typedef swap_struct_t<int64, swap_64_t<int64>> int64_be;

typedef swap_struct_t<uint32, swap_32_t<uint32>> uint32_be;
typedef swap_struct_t<int32, swap_32_t<int32>> int32_be;

typedef swap_struct_t<uint16, swap_16_t<uint16>> uint16_be;
typedef swap_struct_t<int16, swap_16_t<int16>> int16_be;

typedef swap_struct_t<f32, swap_float_t<f32>> f32_be;
typedef swap_struct_t<f64, swap_double_t<f64>> f64_be;
#else

typedef uint32 uint32_be;
typedef uint16 uint16_be;
typedef uint64 uint64_be;

typedef int32 int32_be;
typedef int16 int16_be;
typedef int64 int64_be;

typedef f32 float_be;
typedef f64 double_be;

typedef swap_struct_t<uint64, swap_64_t<uint64>> uint64_le;
typedef swap_struct_t<int64, swap_64_t<int64>> int64_le;

typedef swap_struct_t<uint32, swap_32_t<uint32>> uint32_le;
typedef swap_struct_t<int32, swap_32_t<int32>> int32_le;

typedef swap_struct_t<uint16, swap_16_t<uint16>> uint16_le;
typedef swap_struct_t<int16, swap_16_t<int16>> int16_le;

typedef swap_struct_t<f32, swap_float_t<f32>> f32_le;
typedef swap_struct_t<f64, swap_double_t<f64>> f64_le;

#endif