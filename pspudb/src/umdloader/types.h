// Copyright (c) 2013- PSPUDB Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/georgemoralis/pspudb and http://www.pspudb.com/

#ifndef TYPES_H_
#define TYPES_H_

typedef long long s64;
typedef int s32;
typedef short s16;
typedef char s8;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef float f32;

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))

#ifdef __GNUC__
#define __forceinline inline __attribute__((always_inline))
#endif

typedef u64 insn_count_t;

#if defined(WIN32) || defined(__WINS__) || defined(__MINGW32__) || defined(_MSC_VER)
#define inline __inline
#define snprintf _snprintf
#endif


#endif /* TYPES_H_ */
