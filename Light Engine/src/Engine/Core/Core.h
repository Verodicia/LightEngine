#pragma once

#include "Debug/Logger.h"

#ifdef _WIN32
	#define LIGHT_PLATFORM_WINDOWS
	#ifndef _WIN64
		#error x86 Architecture is not supported
	#endif
#elif defined(__LINUX__)
	#define LIGHT_PLATFORM_LINUX
	#error Unsupported platform: Linux
#elif defind (__APPLE__) || defined(__MACH__)
	#define LIGHT_PLATFORM_MAC
	#error Unsupported platform: Mac
#else
	#error Unsupported platform: Unknown
#endif


#ifdef LIGHT_PLATFORM_WINDOWS
	#define LT_DX(x) x
#else
	#define LT_DX(x)
#endif


#define BIT(x) 1 << x

#define LT_EVENT_FN(fn)        std::bind(&##fn, this, std::placeholders::_1)
#define LT_EVENT_FN_STATIC(fn) std::bind(&##fn,       std::placeholders::_1)

#ifndef LIGHT_DIST
	#define LT_ASSERT(x, ...)      if(!(x)) { LT_FATAL(__VA_ARGS__)     ; __debugbreak(); throw Light::TerminationReq(); }
	#define LT_CORE_ASSERT(x, ...) if(!(x)) { LT_CORE_FATAL(__VA_ARGS__); __debugbreak(); throw Light::TerminationReq(); }
#else
	#define LT_ASSERT(x, code, ...)      if(!(x)) { LT_FATAL(__VA_ARGS__)     ; LT_TERMINATE(code); }
	#define LT_CORE_ASSERT(x, code, ...) if(!(x)) { LT_CORE_FATAL(__VA_ARGS__); LT_TERMINATE(code); }
#endif


namespace Light {

	struct TerminationReq : public std::exception 
	{
		TerminationReq() = default; 
	};

}