#pragma once

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define NOMINMAX

#pragma warning( disable : 4996 ) // Prefer enum class to enum

#define BUILD_DEBUG   0
#define BUILD_RELEASE 1
#define BUILD_FINAL   2

#if defined _DEBUG
#define BUILD BUILD_DEBUG
#elif defined _RELEASE
#define BUILD BUILD_RELEASE
#elif defined _FINAL
#define BUILD BUILD_FINAL
#else
static_assert(true, "Unrecognized configuration");
#endif

#include <windows.h>
#include <cassert>
#include <stdint.h>
#include <type_traits>
#include <stdio.h>

#define ENGINE_API __stdcall
#define DISABLE_OPTS __pragma( optimize( "", off ) )

#define OFFSET_OF( struct_name, member ) &((struct_name*)0)->member

#define UNREFERED_PARAMETER( param ) (void)(param)

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#if ( BUILD < BUILD_FINAL )
  #define ASSERT_MSG( exp, message ) if ( ( exp ) == 0 ) __debugbreak();
#else
  #define ASSERT_MSG( exp, message ) UNREFERENCED_PARAMETER( exp );
#endif

#include "Origami/Util/Memory.h"
#include "Origami/Util/Hash.h"



// reference additional headers your program requires here
