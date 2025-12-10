// miscellaneous collection of useful headers (and macros) combined into a single include
// (extraneous includes will be optimized out)
#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "hash.h"
#include "util.h"
#include <xorstr.h>

#define X(x) xorstr_(x)

#define INTERNAL_CONCATENATE(LEFT, RIGHT) LEFT##RIGHT
#define CONCATENATE(LEFT, RIGHT) INTERNAL_CONCATENATE(LEFT, RIGHT)

#define MEM_PAD(SIZE)										\
private:												\
	char CONCATENATE(pad_0, __COUNTER__)[SIZE]; \
public:

#define MEM_PAD_VTABLE (COUNT)\
MEM_PAD(COUNT * sizeof(void*))