#include "enum_common.hpp"

namespace simple {

#define DEFINE_ENUM_CLASS_FUNCTIONS_ENUM(NkEnumType, EnumType) \
	constexpr NkEnumType to_nk_enum(EnumType value) \
	{ \
		return static_cast<NkEnumType>(value); \
	} \
	constexpr EnumType from_nk_enum(NkEnumType value) \
	{ \
		return static_cast<EnumType>(value); \
	}

#define DEFINE_ENUM_CLASS_FUNCTIONS_BITWISE(EnumType) \
	constexpr EnumType operator|(EnumType lhs, EnumType rhs) \
	{ \
		return static_cast<EnumType>(static_cast<nk_flags>(lhs) | static_cast<nk_flags>(rhs)); \
	} \
	constexpr EnumType operator&(EnumType lhs, EnumType rhs) \
	{ \
		return static_cast<EnumType>(static_cast<nk_flags>(lhs) & static_cast<nk_flags>(rhs)); \
	} \
	constexpr EnumType operator^(EnumType lhs, EnumType rhs) \
	{ \
		return static_cast<EnumType>(static_cast<nk_flags>(lhs) ^ static_cast<nk_flags>(rhs)); \
	} \
	constexpr EnumType operator~(EnumType value) \
	{ \
		return static_cast<EnumType>(~static_cast<nk_flags>(value)); \
	} \
	inline NUKLEUS_CPP14_CONSTEXPR EnumType& operator|=(EnumType& lhs, EnumType rhs) \
	{ \
		return lhs = static_cast<EnumType>(static_cast<nk_flags>(lhs) | static_cast<nk_flags>(rhs));  \
	} \
	inline NUKLEUS_CPP14_CONSTEXPR EnumType& operator&=(EnumType& lhs, EnumType rhs) \
	{ \
		return lhs = static_cast<EnumType>(static_cast<nk_flags>(lhs) & static_cast<nk_flags>(rhs)); \
	} \
	inline NUKLEUS_CPP14_CONSTEXPR EnumType& operator^=(EnumType& lhs, EnumType rhs) \
	{ \
		return lhs = static_cast<EnumType>(static_cast<nk_flags>(lhs) ^ static_cast<nk_flags>(rhs)); \
	} \
	constexpr bool operator%(EnumType lhs, EnumType rhs) \
	{ \
		return (static_cast<nk_flags>(lhs) & static_cast<nk_flags>(rhs)) != 0u; \
	}

#define DEFINE_ENUM_CLASS_FUNCTIONS_FLAGS(NkEnumType, EnumType) \
	DEFINE_ENUM_CLASS_FUNCTIONS_ENUM(NkEnumType, EnumType) \
	DEFINE_ENUM_CLASS_FUNCTIONS_BITWISE(EnumType) \
	constexpr nk_flags to_nk_flags(EnumType value) \
	{ \
		return static_cast<nk_flags>(value); \
	}

template <typename Enum>
constexpr Enum from_nk_flags(nk_flags value)
{
	return static_cast<Enum>(value);
}

enum class permissions : nk_flags
{
        none = 0,
        read = PERMISSION_READ,
        write = PERMISSION_WRITE,
        execute = PERMISSION_EXECUTE
};
DEFINE_ENUM_CLASS_FUNCTIONS_FLAGS(nk_permissions, permissions)

inline void accept_nk_flags(nk_flags) {}
inline void accept_nk_enum(nk_permissions) {}
inline bool accept_class(permissions)
{
	// operators: V + V
	auto p2 = permissions::read | permissions::write;
	// operators: C + V
	p2 = p2 | permissions::read;
	// operators: V + C
	auto p3 = permissions::write | p2;
	// operators: C + C
	auto p4 = p2 ^ p3;
	// operators: neg V, neg C
	p4 = ~permissions::execute ^ ~p3;

	// assignment operators: V + V
	auto v = permissions::read;
	v &= permissions::read;
	// assignment operators: V + C
	v &= p3;
	// assignment operators: C + V
	p3 &= v;
	// assignment operators: C + C
	p4 ^= p3;

	// bool - requires special operator
	return (p4 % permissions::execute) ? true : false;
}

inline void test()
{
	constexpr auto p = permissions::read;

	// V to_nk_enum
	accept_nk_enum(to_nk_enum(p));               // explicit
	// C to_nk_enum
        // (not present)
	// from_nk_enum
	accept_class(from_nk_enum(PERMISSION_READ)); // explicit

	// V to_nk_flags
	accept_nk_flags(to_nk_flags(permissions::read)); // free: worse than member
	// accept_nk_flags(permissions::read); // unwanted
	// C to_nk_flags
	// (not present)
	// from_nk_flags
	accept_class(from_nk_flags<permissions>(PERMISSION_READ | PERMISSION_WRITE));

	// V to C / C to V
	// (not present)
}

} // namespace simple
