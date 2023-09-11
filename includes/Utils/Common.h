#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#define GLM_FORCE_INTRINSICS
#include <glm.hpp>
#include <limits>
#include <string>
#include <uuid.h>

#define assertm(exp, msg) assert(((void)msg, exp))

typedef int8_t   int8;
typedef uint8_t  uint8;
typedef int16_t  int16;
typedef uint16_t uint16;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef int64_t  int64;
typedef uint64_t uint64;

typedef float  float32;
typedef double float64;

typedef std::uintptr_t uintptr;