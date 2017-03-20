#pragma once

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;

typedef float f32;
typedef double f64;

typedef int32 int24;

#define min(min, max) (min > max ? max : min)
#define max(min, max) (min > max ? min : max)
