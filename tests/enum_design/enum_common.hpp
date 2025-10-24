#pragma once

using nk_flags = unsigned long;

enum nk_permissions
{
	PERMISSION_READ    = 1 << 0,
	PERMISSION_WRITE   = 1 << 1,
	PERMISSION_EXECUTE = 1 << 2
};

#if __cplusplus >= 201402L
	#define NUKLEUS_CPP14
	#define NUKLEUS_CPP14_CONSTEXPR constexpr
#else
	#define NUKLEUS_CPP14_CONSTEXPR
#endif

#if __cplusplus >= 201703L
	#define NUKLEUS_CPP17
	#define NUKLEUS_CPP17_CONSTEXPR constexpr
#else
	#define NUKLEUS_CPP17_CONSTEXPR
#endif
