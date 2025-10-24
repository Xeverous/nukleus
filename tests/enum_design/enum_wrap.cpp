#include "enum_common.hpp"

namespace wrap {

// V
enum class permissions : nk_flags
{
	read    = PERMISSION_READ,
	write   = PERMISSION_WRITE,
	execute = PERMISSION_EXECUTE
};

// C
class permissions_class
{
public:
	// to/from enum
	/* implicit */ permissions_class(permissions p)
	: m_flags(static_cast<nk_flags>(p))
	{}

	/* implicit */ operator permissions() const
	{
		return static_cast<permissions>(m_flags);
	}

	// from_nk_enum

	/* implicit */ constexpr permissions_class(nk_permissions underlying)
	: m_flags(static_cast<nk_flags>(underlying))
	{}

	static constexpr permissions_class from_nk_enum(nk_permissions flags)
	{
		return from_nk_flags(static_cast<nk_flags>(flags));
	}

	// to_nk_enum

	constexpr nk_permissions to_nk_enum() const
	{
		return static_cast<nk_permissions>(to_nk_flags());
	}

	/* implicit */ constexpr operator nk_permissions() const
	{
		return static_cast<nk_permissions>(m_flags);
	}

	// from_nk_flags

	static constexpr permissions_class from_int(int flags)
	{
		return permissions_class(static_cast<nk_permissions>(flags));
	}

	static constexpr permissions_class from_nk_flags(nk_flags flags)
	{
		return permissions_class(static_cast<nk_permissions>(flags));
	}

	// to_nk_flags

	constexpr nk_flags to_nk_flags() const
	{
		return m_flags;
	}

	// block implicit convertions to nk_flags
	// (otherwise operator nk_permissions converts to nk_flags)
	/* implicit */ constexpr operator nk_flags() const = delete;

	// bool

	explicit constexpr operator bool() const
	{
		return m_flags != 0;
	}

	// operator

	permissions_class operator~() const
	{
		return permissions_class::from_nk_flags(~m_flags);
	}

	NUKLEUS_CPP14_CONSTEXPR permissions_class& operator|=(permissions rhs)
	{
		m_flags |= static_cast<nk_flags>(rhs);
		return *this;
	}
	NUKLEUS_CPP14_CONSTEXPR permissions_class& operator&=(permissions rhs)
	{
		m_flags &= static_cast<nk_flags>(rhs);
		return *this;
	}
	NUKLEUS_CPP14_CONSTEXPR permissions_class& operator^=(permissions rhs)
	{
		m_flags ^= static_cast<nk_flags>(rhs);
		return *this;
	}

private:
	nk_flags m_flags;
};

constexpr permissions_class operator~(permissions value)
{
	return permissions_class::from_nk_flags(~static_cast<nk_flags>(value));
}

constexpr permissions_class operator|(permissions lhs, permissions rhs)
{
	return permissions_class::from_nk_flags(static_cast<nk_flags>(lhs) | static_cast<nk_flags>(rhs));
}
constexpr permissions_class operator|(permissions_class lhs, permissions rhs)
{
	return permissions_class::from_nk_flags(lhs.to_nk_flags() | static_cast<nk_flags>(rhs));
}
constexpr permissions_class operator|(permissions lhs, permissions_class rhs)
{
	return permissions_class::from_nk_flags(static_cast<nk_flags>(lhs) | rhs.to_nk_flags());
}
constexpr permissions_class operator|(permissions_class lhs, permissions_class rhs)
{
	return permissions_class::from_nk_flags(lhs.to_nk_flags() | rhs.to_nk_flags());
}

constexpr permissions_class operator&(permissions lhs, permissions rhs)
{
	return permissions_class::from_nk_flags(static_cast<nk_flags>(lhs) & static_cast<nk_flags>(rhs));
}
constexpr permissions_class operator&(permissions_class lhs, permissions rhs)
{
	return permissions_class::from_nk_flags(lhs.to_nk_flags() & static_cast<nk_flags>(rhs));
}
constexpr permissions_class operator&(permissions lhs, permissions_class rhs)
{
	return permissions_class::from_nk_flags(static_cast<nk_flags>(lhs) & rhs.to_nk_flags());
}
constexpr permissions_class operator&(permissions_class lhs, permissions_class rhs)
{
	return permissions_class::from_nk_flags(lhs.to_nk_flags() & rhs.to_nk_flags());
}

constexpr permissions_class operator^(permissions lhs, permissions rhs)
{
	return permissions_class::from_nk_flags(static_cast<nk_flags>(lhs) ^ static_cast<nk_flags>(rhs));
}
constexpr permissions_class operator^(permissions_class lhs, permissions rhs)
{
	return permissions_class::from_nk_flags(lhs.to_nk_flags() ^ static_cast<nk_flags>(rhs));
}
constexpr permissions_class operator^(permissions lhs, permissions_class rhs)
{
	return permissions_class::from_nk_flags(static_cast<nk_flags>(lhs) ^ rhs.to_nk_flags());
}
constexpr permissions_class operator^(permissions_class lhs, permissions_class rhs)
{
	return permissions_class::from_nk_flags(lhs.to_nk_flags() ^ rhs.to_nk_flags());
}

inline NUKLEUS_CPP14_CONSTEXPR permissions& operator|=(permissions& lhs, permissions rhs)
{
	return lhs = static_cast<permissions>(static_cast<nk_flags>(lhs) | static_cast<nk_flags>(rhs));
}
inline NUKLEUS_CPP14_CONSTEXPR permissions& operator&=(permissions& lhs, permissions rhs)
{
	return lhs = static_cast<permissions>(static_cast<nk_flags>(lhs) & static_cast<nk_flags>(rhs));
}
inline NUKLEUS_CPP14_CONSTEXPR permissions& operator^=(permissions& lhs, permissions rhs)
{
	return lhs = static_cast<permissions>(static_cast<nk_flags>(lhs) ^ static_cast<nk_flags>(rhs));
}

constexpr nk_flags to_nk_flags(permissions p)
{
	return static_cast<nk_flags>(p);
}

inline void accept_nk_flags(nk_flags) {}
inline void accept_nk_enum(nk_permissions) {}
inline void accept_value(permissions) {}
inline bool accept_class(permissions_class)
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

	// bool
	return p4 & permissions::execute ? true : false;
}

inline void test()
{
	constexpr auto p = permissions::read;

	// V to_nk_enum
	accept_nk_enum(static_cast<nk_permissions>(p)); // explicit
	// C to_nk_enum
	accept_nk_enum(p | permissions::write);         // implicit
	accept_nk_enum((p | permissions::write).to_nk_enum());
	// from_nk_enum
	accept_class(PERMISSION_READ);                  // implicit
	accept_class(permissions_class::from_nk_enum(PERMISSION_READ));

	// V to_nk_flags
	accept_nk_flags(to_nk_flags(permissions::read));
	// accept_nk_flags(p | p); // unwanted
	// C to_nk_flags
	accept_nk_flags((p | permissions::read).to_nk_flags());
	// from_nk_flags
	accept_class(permissions_class::from_nk_flags(PERMISSION_READ | PERMISSION_WRITE));

	// V to C
	accept_class(permissions::read);
	// C to V
	accept_value(p & permissions::read);
}

} // namespace wrap
