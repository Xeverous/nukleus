#include "enum_common.hpp"

namespace crtp {

// V
template <typename Derived, typename Underlying>
class enum_flags_base
{
public:
	// from_nk_enum

	/* implicit */ constexpr enum_flags_base(Underlying underlying)
	: m_flags(static_cast<nk_flags>(underlying))
	{}

	static constexpr Derived from_nk_enum(Underlying flags)
	{
		return from_nk_flags(static_cast<nk_flags>(flags));
	}

	// to_nk_enum

	constexpr Underlying to_nk_enum() const
	{
		return static_cast<Underlying>(to_nk_flags());
	}

	/* implicit */ constexpr operator Underlying() const
	{
		return static_cast<Underlying>(m_flags);
	}

	// from_nk_flags

	static constexpr Derived from_int(int flags)
	{
		return from_nk_flags(static_cast<nk_flags>(flags));
	}

	static constexpr Derived from_nk_flags(nk_flags flags)
	{
		return enum_flags_base(flags);
	}

	// to_nk_flags

	constexpr nk_flags to_nk_flags() const
	{
		return m_flags;
	}

	// block implicit convertions to nk_flags
	// (otherwise operator Underlying converts to nk_flags)
	/* implicit */ constexpr operator nk_flags() const = delete;

	// bool

	explicit constexpr operator bool() const
	{
		return m_flags != 0;
	}

	// operator

	inline NUKLEUS_CPP14_CONSTEXPR Derived& operator|=(enum_flags_base rhs)
	{
		m_flags |= rhs.to_nk_flags();
		return as_derived();
	}
	inline NUKLEUS_CPP14_CONSTEXPR Derived& operator&=(enum_flags_base rhs)
	{
		m_flags &= rhs.to_nk_flags();
		return as_derived();
	}
	inline NUKLEUS_CPP14_CONSTEXPR Derived& operator^=(enum_flags_base rhs)
	{
		m_flags ^= rhs.to_nk_flags();
		return as_derived();
	}

	friend constexpr Derived operator|(enum_flags_base lhs, enum_flags_base rhs)
	{
		return Derived::from_nk_flags(lhs.to_nk_flags() | rhs.to_nk_flags());
	}
	friend constexpr Derived operator&(enum_flags_base lhs, enum_flags_base rhs)
	{
		return Derived::from_nk_flags(lhs.to_nk_flags() & rhs.to_nk_flags());
	}
	friend constexpr Derived operator^(enum_flags_base lhs, enum_flags_base rhs)
	{
		return Derived::from_nk_flags(lhs.to_nk_flags() ^ rhs.to_nk_flags());
	}
	friend constexpr Derived operator~(enum_flags_base value)
	{
		return Derived::from_nk_flags(~value.to_nk_flags());
	}

private:
	NUKLEUS_CPP14_CONSTEXPR Derived& as_derived()
	{
		return static_cast<Derived&>(*this);
	}

	NUKLEUS_CPP14_CONSTEXPR const Derived& as_derived() const
	{
		return static_cast<const Derived&>(*this);
	}

	constexpr enum_flags_base(nk_flags flags)
	: m_flags(flags)
	{}

	nk_flags m_flags;
};

// C
class permissions : public enum_flags_base<permissions, nk_permissions>
{
public:
	using base_type = enum_flags_base<permissions, nk_permissions>;

	using base_type::base_type;

	/* implicit */ constexpr permissions(base_type value)
	: base_type(value)
	{}

	static constexpr base_type read    = base_type(PERMISSION_READ);
	static constexpr base_type write   = base_type(PERMISSION_WRITE);
	static constexpr base_type execute = base_type(PERMISSION_EXECUTE);
};

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

	// bool
	return p4 & permissions::execute ? true : false;
}

inline void test()
{
	constexpr auto p = permissions::read;

	// V to_nk_enum
	accept_nk_enum(p);                      // implicit
	accept_nk_enum(permissions::read.to_nk_enum());
	// C to_nk_enum
	accept_nk_enum(p | permissions::write); // implicit
	accept_nk_enum((p | permissions::write).to_nk_enum());
	// from_nk_enum
	accept_class(PERMISSION_READ);           // implicit
	accept_class(permissions::from_nk_enum(PERMISSION_READ));

	// V to_nk_flags
	accept_nk_flags(permissions::read.to_nk_flags());
	// accept_nk_flags(permissions::read); // unwanted
	// C to_nk_flags
	accept_nk_flags((p | permissions::read).to_nk_flags());
	// from_nk_flags
	accept_class(permissions::from_nk_flags(PERMISSION_READ | PERMISSION_WRITE));

	// V to C
	accept_class(permissions::read);
	// C to V
	// (not needed)
}

} // namespace crtp
