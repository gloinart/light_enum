#pragma once
/*

LightEnum

Usage:
	#include "light_enum.hpp" where you access enums, or in your precomiled headers
	#include "light_enum_register.hpp" ONLY in .cpp files where you register enums for usage



Configuration macros:


LIGHT_ENUM_USE_BOOST_TYPEINDEX
Use boost::type_index instead of std::type_index.
In difference to std::type_index, boost::type_index does not need RTTI to be enabled.


LIGHT_ENUM_IGNORE_SMALL_RANGE
By default, light enum reguses to compile if an registered enum seems to be larger than 


LIGHT_ENUM_DEBUG
Define in order to enable internal debug checks


LIGHT_ENUM_ASSERT
Define in order to use another assertion macro if LIGHT_ENUM_DEBUG is enabled


LIGHT_ENUM_ALIAS_MAP_BASE
Define in order to change the underlying std::unordered_map to a map of your choice
Example: 
#define LIGHT_ENUM_ALIAS_MAP_BASE boost::unordered_map


LIGHT_ENUM_IGNORE_DUPLICATE
By default, light_enum throws an exception if the same enum is registered twice.
Define this to ignore this exception and register the enum again.

*/




#ifdef LIGHT_ENUM_DEBUG
	#define LIGHT_ENUM_INTERNAL_DEBUG_MODE 1
	#ifdef LIGHT_ENUM_ASSERT
		#define LIGHT_ENUM_INTERNAL_ASSERT LIGHT_ENUM_ASSERT
	#else
		#include <cassert>
		#define LIGHT_ENUM_INTERNAL_ASSERT(x) assert(x)
	#endif
#else
	#define LIGHT_ENUM_INTERNAL_DEBUG_MODE 0
	#define LIGHT_ENUM_INTERNAL_ASSERT(x)
#endif



#ifdef LIGHT_ENUM_USE_BOOST_TYPEINDEX
	#define LIGHT_ENUM_INTERNAL_USE_BOOST_TYPEINDEX 1
#else
	#define LIGHT_ENUM_INTERNAL_USE_BOOST_TYPEINDEX 0
#endif



#ifdef LIGHT_ENUM_IGNORE_SMALL_RANGE
	#define LIGHT_ENUM_INTERNAL_ABORT_ON_SMALL_RANGE 0
#else
	#define LIGHT_ENUM_INTERNAL_ABORT_ON_SMALL_RANGE 1
#endif



#ifdef LIGHT_ENUM_ALIAS_MAP_BASE
	#define LIGHT_ENUM_INTERNAL_ALIAS_MAP_BASE LIGHT_ENUM_ALIAS_MAP_BASE
#endif



#ifdef LIGHT_ENUM_IGNORE_DUPLICATE
	#define LIGHT_ENUM_INTERNAL_ABORT_ON_DUPLICATE 0
#else
	#define LIGHT_ENUM_INTERNAL_ABORT_ON_DUPLICATE 1
#endif
