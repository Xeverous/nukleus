/**
 * @file nukleus.hpp
 * @brief Nukleus only file.
 */
#pragma once

// -------- compiler configuration --------

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

#if __cplusplus >= 202002L
	#define NUKLEUS_CPP20
	#define NUKLEUS_CPP20_CONSTEXPR constexpr
#else
	#define NUKLEUS_CPP20_CONSTEXPR
#endif

#if __cplusplus >= 202302L
	#define NUKLEUS_CPP23
	#define NUKLEUS_CPP23_CONSTEXPR constexpr
#else
	#define NUKLEUS_CPP23_CONSTEXPR
#endif

// Unlike GCC and Clang:
// - MSVC requires annotations on both declaration and definition of a function.
// - The annotation needs to be at the start of the declaration/definition (GCC and Clang allows either).
// - The annotation requires /analyze compiler switch to work.
//
// Since Nukleus is header-only and uses inline functions only:
// - There is no declaration/definition problem.
// - The macro should be used at the very start of a statement that declares/defines a function.
#ifdef NUKLEUS_CPP17
	#define NUKLEUS_NODISCARD [[nodiscard]]
#else
	#if defined(__GNUC__) && (__GNUC__ >= 4) // clang also defines these
		#define NUKLEUS_NODISCARD __attribute__((warn_unused_result))
	#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
		#define NUKLEUS_NODISCARD _Check_return_
	#else
		#define NUKLEUS_NODISCARD
	#endif
#endif

// -------- build configuration --------

// ---- implementation dependencies ----

// If you don't define this, nuklear will use assert().
// #define NK_ASSERT
// Nuklear exposes its assertion macro only under NK_IMPLEMENTATION.
// Because Nukleus needs assertions in its header, it has to have its own assertion macro.
// You are free to supply your own implementation though, for both libraries.
#ifndef NUKLEUS_ASSERT
	#ifdef NUKLEUS_AVOID_STDLIB
		#error "If you want to avoid standard library headers, you need to define your own NUKLEUS_ASSERT"
	#endif

	#include <assert.h>
	#define NUKLEUS_ASSERT(expr) assert(expr)
	#define NUKLEUS_ASSERT_MSG(expr, msg) NUKLEUS_ASSERT((expr) && (msg))
#endif

#ifndef NK_MAX_NUMBER_BUFFER
	#define NK_MAX_NUMBER_BUFFER 16 // should be more than enough
#endif

#ifndef NUKLEUS_AVOID_STDLIB
	#include <initializer_list>
#endif

// All of the following options (if defined) need to be defined for the implementation mode.
#ifdef NK_IMPLEMENTATION

// You can define this to 'memset' or your own memset implementation replacement.
// If not nuklear will use its own version.
#ifndef NK_MEMSET
	#ifndef NUKLEUS_AVOID_STDLIB
		#include <string.h>
		#define NK_MEMSET memset
	#endif
#endif

// You can define this to 'memcpy' or your own memcpy implementation replacement.
// If not nuklear will use its own version.
#ifndef NK_MEMCPY
	#ifndef NUKLEUS_AVOID_STDLIB
		#include <string.h>
		#define NK_MEMCPY memcpy
	#endif
#endif

// You can define this to your own inverse sqrt implementation replacement.
// If not nuklear will use its own slow and not highly accurate version.
#ifndef NK_INV_SQRT
	#ifndef NUKLEUS_AVOID_STDLIB
		#include <math.h>
		inline float nk_inv_sqrt_stdlib(float x)
		{
			return 1.0f / sqrtf(x);
		}
		#define NK_INV_SQRT nk_inv_sqrt_stdlib
	#endif
#endif

// You can define this to 'sinf' or your own sine implementation replacement.
// If not nuklear will use its own approximation implementation.
#ifndef NK_SIN
	#ifndef NUKLEUS_AVOID_STDLIB
		#include <math.h>
		#define NK_SIN sinf
	#endif
#endif

// You can define this to 'cosf' or your own cosine implementation replacement.
// If not nuklear will use its own approximation implementation.
#ifndef NK_COS
	#ifndef NUKLEUS_AVOID_STDLIB
		#include <math.h>
		#define NK_COS cosf
	#endif
#endif

// You can define this to strtod or your own string to double conversion implementation replacement.
// If not defined nuklear will use its own imprecise and possibly unsafe version (does not handle nan or infinity!).
#ifndef NK_STRTOD
	#ifndef NUKLEUS_AVOID_STDLIB
		#include <stdlib.h>
		#define NK_STRTOD strtod
	#endif
#endif

// You can define this to dtoa or your own double to string conversion implementation replacement.
// If not defined nuklear will use its own imprecise and possibly unsafe version (does not handle nan or infinity!).
#ifndef NK_DTOA
	#ifndef NUKLEUS_AVOID_STDLIB
		#define NK_DTOA nk_dtoa_stdlib

		// Note: there is <charconv> in C++17 but some compilers do not implement it even at C++17.
		// GCC 12.3.0 has it, can compile it but always returns error on floating-point overload.
		// Thus, an additional flag is required to enable this implementation.
		#if __cplusplus >= 201703L && defined(NUKLEUS_USE_CHARCONV)
			#include <charconv>
			inline char* nk_dtoa_stdlib(char* s, double n)
			{
				NUKLEUS_ASSERT(s != nullptr);
				if (s == nullptr)
					return nullptr;

				// -1 because to_chars does not add null terminator
				const std::to_chars_result result =
					std::to_chars(s, s + NK_MAX_NUMBER_BUFFER - 1, n, std::chars_format::fixed);

				if (static_cast<int>(result.ec) == 0) {
					*result.ptr = '\0';
				}
				else {
					s[0] = '?';
					s[1] = '\0';
				}

				return s;
			}
		#else
			#include <stdio.h>
			inline char* nk_dtoa_stdlib(char* s, double n)
			{
				NUKLEUS_ASSERT(s != nullptr);
				if (s == nullptr)
					return nullptr;

				if (snprintf(s, NK_MAX_NUMBER_BUFFER, "%f", n) < 0) {
					s[0] = '?';
					s[1] = '\0';
				}

				return s;
			}
		#endif
	#endif
#endif

// If you define NK_INCLUDE_STANDARD_VARARGS as well as NK_INCLUDE_STANDARD_IO and want to be safe define this to
// vsnprintf on compilers supporting later versions of C or C++. By default nuklear will check for your stdlib version
// in C as well as compiler version in C++. if vsnprintf is available it will define it to vsnprintf directly.
// If not defined and if you have older versions of C or C++ it will be defined to vsprintf which is unsafe.
#ifndef NK_VSNPRINTF
	#ifndef NUKLEUS_AVOID_STDLIB
		#include <stdio.h>
		#define NK_VSNPRINTF vsnprintf
	#endif
#endif

#endif // NK_IMPLEMENTATION

// Silence some warnings from Nuklear.
// The library implementation seems to not care about few minor ones.
#if defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wfloat-equal"
	#pragma clang diagnostic ignored "-Wsign-conversion"
	#pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUG__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wfloat-equal"
	#pragma GCC diagnostic ignored "-Wsign-conversion"
	#pragma GCC diagnostic ignored "-Wint-in-bool-context"
	#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <nuklear.h>

#if defined(__clang__)
	#pragma clang diagnostic pop
#elif defined(__GNUG__)
	#pragma GCC diagnostic pop
#endif

/*
TODO possible improvements:
- nk_flags - an alias for uint, not type safe
- UTF-8 - undocumented
*/

namespace nk {

/**
 * @defgroup stdlib Standard Library elements
 * @{
 */

/**
 * @defgroup type_traits Type Traits
 * @brief Implementation of some traits to avoid including \<type_traits\>.
 * @{
 */

/**
 * @defgroup detection_traits Detection Traits
 * @{
 */

template <typename T, T Value>
struct integral_constant
{
	static constexpr T value = Value;
	using value_type = T;
	using type = integral_constant;
	constexpr operator value_type() const noexcept { return value; }
	constexpr value_type operator()() const noexcept { return value; }
};

using true_type  = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <typename T, typename U> struct is_same       : false_type {};
template <typename T>             struct is_same<T, T> : true_type {};

template <typename T>            struct is_void : false_type {};
template <> struct is_void<void>                : true_type {};
template <> struct is_void<const void>          : true_type {};
template <> struct is_void<volatile void>       : true_type {};
template <> struct is_void<const volatile void> : true_type {};

template <typename T>                  struct is_array       : false_type {};
template <typename T>                  struct is_array<T[]>  : true_type {};
template <typename T, unsigned long N> struct is_array<T[N]> : true_type {};

template <typename T> struct is_pointer      : false_type {};
template <typename T> struct is_pointer<T*>  : true_type {};

template <typename>   struct is_lvalue_reference     : false_type {};
template <typename T> struct is_lvalue_reference<T&> : true_type {};

template <typename>   struct is_rvalue_reference      : false_type {};
template <typename T> struct is_rvalue_reference<T&&> : true_type {};

template <typename T> struct is_reference      : false_type {};
template <typename T> struct is_reference<T&>  : true_type {};
template <typename T> struct is_reference<T&&> : true_type {};

template <typename>   struct is_const          : false_type {};
template <typename T> struct is_const<const T> : true_type {};

template <typename>   struct is_volatile             : false_type {};
template <typename T> struct is_volatile<volatile T> : true_type {};

template <typename T>
struct is_function :
	integral_constant<
		bool,
		!is_const<const T>::value && !is_reference<T>::value
	> {};

namespace detail
{
	// Note: a positive case should do this:
	// integral_constant<bool, !is_union<T>::value> test(int T::*);
	// but is_union is not implementable without compiler support.

	template <typename T> true_type class_test(int T::*);
	template <typename>  false_type class_test(...);
}

/**
 * @brief simplified `std::is_class` trait, does not handle union types
 * @tparam T type to check, should not be a union type
 */
template <typename T>
struct is_class : decltype(detail::class_test<T>(nullptr)) {};

namespace detail
{
	template <typename Base> true_type test_ptr_conv(const volatile Base*);
	template <typename>     false_type test_ptr_conv(const volatile void*);

	template <typename Base, typename Derived>
	auto test_is_base_of(int) -> decltype(test_ptr_conv<Base>(static_cast<Derived*>(nullptr)));
	template <typename, typename>
	auto test_is_base_of(...) -> true_type; // private or ambiguous base
}

/**
 * @brief simplified `std::is_base_of` trait, does not handle union types
 * @tparam Base base type
 * @tparam Derived derived type
 */
template <typename Base, typename Derived>
struct is_base_of :
	integral_constant<
		bool,
		is_class<Base>::value &&
		is_class<Derived>::value &&
		decltype(detail::test_is_base_of<Base, Derived>(0))::value
	> {};

#ifdef NUKLEUS_CPP14 // C++14 variable templates
template <typename T, typename U> constexpr bool is_same_v = is_same<T, U>::value;
template <typename T> constexpr bool is_void_v = is_void<T>::value;
template <typename T> constexpr bool is_array_v = is_array<T>::value;
template <typename T> constexpr bool is_pointer_v = is_pointer<T>::value;
template <typename T> constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
template <typename T> constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
template <typename T> constexpr bool is_reference_v = is_reference<T>::value;
template <typename T> constexpr bool is_const_v = is_const<T>::value;
template <typename T> constexpr bool is_volatile_v = is_volatile<T>::value;
template <typename T> constexpr bool is_function_v = is_function<T>::value;
template <typename T> constexpr bool is_class_v = is_class<T>::value;
template <typename Base, typename Derived> constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
#endif

/// @} // detection_traits

/**
 * @defgroup transformation_traits Transformation Traits
 * @details The implementation skips some corner cases:
 * - The `add_*_reference` traits do not support abominable types (https://wg21.link/P0172r0#2.1)
 *   which means you will get compiler errors when using such types with these traits (instead of same type).
 *   This also affects other things which depent on these, including @ref declval
 * - @ref invoke_result does not handle `std::reference_wrapper` (would need to add an include)
 * @{
 */

template <typename T> struct add_lvalue_reference            { using type = T&; };
template <> struct add_lvalue_reference<void>                { using type = void; };
template <> struct add_lvalue_reference<const void>          { using type = const void; };
template <> struct add_lvalue_reference<volatile void>       { using type = volatile void; };
template <> struct add_lvalue_reference<const volatile void> { using type = const volatile void; };
template <typename T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T> struct add_rvalue_reference            { using type = T&&; };
template <> struct add_rvalue_reference<void>                { using type = void; };
template <> struct add_rvalue_reference<const void>          { using type = const void; };
template <> struct add_rvalue_reference<volatile void>       { using type = volatile void; };
template <> struct add_rvalue_reference<const volatile void> { using type = const volatile void; };
template <typename T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

template <typename T>                  struct remove_extent       { using type = T; };
template <typename T>                  struct remove_extent<T[]>  { using type = T; };
template <typename T, unsigned long N> struct remove_extent<T[N]> { using type = T; };
template <typename T> using remove_extent_t = typename remove_extent<T>::type;

template <typename T> struct remove_reference      { using type = T; };
template <typename T> struct remove_reference<T&>  { using type = T; };
template <typename T> struct remove_reference<T&&> { using type = T; };
template <typename T> using remove_reference_t = typename remove_reference<T>::type;

template <typename T> struct remove_cv                   { using type = T; };
template <typename T> struct remove_cv<const T>          { using type = T; };
template <typename T> struct remove_cv<volatile T>       { using type = T; };
template <typename T> struct remove_cv<const volatile T> { using type = T; };
template <typename T> using remove_cv_t = typename remove_cv<T>::type;

template <typename T> struct remove_cvref { using type = remove_cv_t<remove_reference_t<T>>; };
template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T> struct type_identity { using type = T; };
template <typename T> using type_identity_t = typename type_identity<T>::type;

namespace detail
{
	template <typename T>
	auto try_add_pointer(int) -> type_identity<remove_reference_t<T>*>; // usual case

	template <typename T>
	auto try_add_pointer(...) -> type_identity<T>; // unusual case (cannot form a pointer)
} // namespace detail

template <typename T>
struct add_pointer : decltype(detail::try_add_pointer<T>(0)) {};
template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

/// @} // transformation_traits

/**
 * @defgroup meta_traits Meta Traits
 * @{
 */

template <typename...>
struct param_pack {};

template <bool B, typename T, typename F>
struct conditional { using type = T; };
template <typename T, typename F>
struct conditional<false, T, F> { using type = F; };
template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

template <bool B, typename T = void> struct enable_if {};
template <typename T>                struct enable_if<true, T> { using type = T; };
template <bool B, typename T = void> using enable_if_t = typename enable_if<B, T>::type;

template <typename... Ts> struct void_t_impl { using type = void; };
template <typename... Ts> using void_t = typename void_t_impl<Ts...>::type;

/// @} // meta_traits

// (depends on conditional)
/**
 * @addtogroup transformation_traits
 * @{
 */
template <typename T>
struct decay
{
private:
	using U = remove_reference_t<T>;
public:
	using type = conditional_t<
		is_array<U>::value,
		add_pointer_t<remove_extent_t<U>>,
		conditional_t<
			is_function<U>::value,
			add_pointer_t<U>,
			remove_cv_t<U>
		>
	>;
};

template <typename T>
using decay_t = typename decay<T>::type;
/// @}

/// @} // type_traits

/**
 * @defgroup utility Utility functions
 * @brief Implementation of some functions from \<utility\>.
 * @{
 */

template <typename T>
add_rvalue_reference_t<T> declval() noexcept
{
	static_assert(sizeof(T) == 0u, "this function should only be used in unevaluated contexts");
}

inline bool is_aligned(const void* ptr, nk_size alignment) noexcept
{
	// Note: the standard does not guuarantee that a conversion from
	// pointer to integer preserves the numeric representation of the address.
	// I don't know of any platform/toolchain where this doesn't work though.
	return !(reinterpret_cast<nk_size>(ptr) % alignment);
}

template <typename T>
bool is_aligned(const void* ptr) noexcept
{
	return is_aligned(ptr, alignof(T));
}

template <typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept
{
	return static_cast<remove_reference_t<T>&&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>& t) noexcept
{
	return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>&& t) noexcept
{
	static_assert(!is_lvalue_reference<T>::value, "forward must not be used to convert an rvalue to an lvalue");
	return static_cast<T&&>(t);
}

// skipped noexcept implementation (requires more traits)
// skipped SFINAE (requires more traits)
template <typename T>
NUKLEUS_CPP14_CONSTEXPR void swap(T& a, T& b)
{
	T temp = move(a);
	a = move(b);
	b = move(temp);
}

// skipped noexcept implementation (requires more traits)
template <typename T, typename U = T>
NUKLEUS_CPP14_CONSTEXPR T exchange(T& obj, U&& new_value)
{
	T old_value = move(obj);
	obj = forward<U>(new_value);
	return old_value;
}

template <typename T, unsigned N>
constexpr unsigned size(const T (&/*array*/)[N]) noexcept
{
	return N;
}

template <typename T, unsigned N>
constexpr signed ssize(const T (&/*array*/)[N]) noexcept
{
	return static_cast<signed>(N);
}

namespace detail {

/**
 * @brief A struct to abstract output parameters of type `nk_bool`. Intended to be used as parameter replacement.
 * @details if `NK_INCLUDE_STANDARD_BOOL`, then `nk_bool` is `bool` and a pointer can be given directly.
 * But if not `NK_INCLUDE_STANDARD_BOOL`, then `nk_bool` is `int` and an additional temporary variable
 * is needed. The struct passes a pointer to this temporary and updates the bool in the destructor.
 */
struct output_bool
{
#ifdef NK_INCLUDE_STANDARD_BOOL // nk_bool is bool
	/* implicit */ output_bool(bool& b)
	: m_b(b)
	{}

	/* implicit */ operator nk_bool*()
	{
		return &m_b;
	}

	bool& m_b;
#else // nk_bool is int
	/* implicit */ output_bool(bool& b)
	: m_b(b)
	, m_nk_b(b)
	{}

	~output_bool()
	{
		m_b = m_nk_b == nk_true;
	}

	/* implicit */ operator nk_bool*()
	{
		return &m_nk_b;
	}

	bool& m_b;
	nk_bool m_nk_b;
#endif
};

}

/// @} // utility

// (depends on declval)
/**
 * @addtogroup meta_traits
 * @{
 */
namespace detail
{
	template <typename T>
	struct invoke_impl
	{
		template <typename F, typename... Args>
		static auto call(F&& f, Args&&... args)
			-> decltype(forward<F>(f)(forward<Args>(args)...));
	};

	template <typename B, typename MT>
	struct invoke_impl<MT B::*>
	{
		template <typename T, typename Td = decay_t<T>,
			typename = enable_if_t<is_base_of<B, Td>::value>>
		static auto get(T&& t) -> T&&;

		template <typename T, typename Td = decay_t<T>,
			typename = enable_if_t<!is_base_of<B, Td>::value>>
		static auto get(T&& t) -> decltype(*forward<T>(t));

		template <typename T, typename... Args, typename MT1,
			typename = enable_if_t<is_function<MT1>::value>>
		static auto call(MT1 B::*pmf, T&& t, Args&&... args)
			-> decltype((invoke_impl::get(forward<T>(t)).*pmf)(forward<Args>(args)...));

		template <typename T>
		static auto call(MT B::*pmd, T&& t)
			-> decltype(invoke_impl::get(forward<T>(t)).*pmd);
	};

	template <typename F, typename... Args, typename Fd = decay_t<F>>
	auto invoke(F&& f, Args&&... args)
		-> decltype(invoke_impl<Fd>::call(forward<F>(f), forward<Args>(args)...));

	template <typename AlwaysVoid, typename, typename...> struct invoke_result {};

	template <typename F, typename... Args> struct invoke_result<
		decltype(void(detail::invoke(declval<F>(), declval<Args>()...))),
		F,
		Args...>
	{
		using type = decltype(detail::invoke(declval<F>(), declval<Args>()...));
	};
} // namespace detail

/**
 * @brief simplified implementation of C++17 `std::invoke_result`
 * @details does not handle:
 * - `std::reference_wrapper`
 * - abominable function types (see transformation traits description)
 * - union types
 */
template <typename F, typename... Args>
struct invoke_result : detail::invoke_result<void, F, Args...> {};
template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

/// @} // meta_traits

// (depends on invoke_result and declval)
/**
 * @addtogroup detection_traits
 * @{
 */

namespace detail
{
	template <typename T>
	auto test_returnable(int) -> decltype(
		void(static_cast<T(*)()>(nullptr)), true_type{}
	);
	template <typename>
	auto test_returnable(...) -> false_type;

	template <typename From, typename To>
	auto test_implicitly_convertible(int) -> decltype(
		void(declval<void(&)(To)>()(declval<From>())), true_type{}
	);
	template <typename, typename>
	auto test_implicitly_convertible(...) -> false_type;
} // namespace detail

template <typename From, typename To>
struct is_convertible : integral_constant<bool,
	(decltype(detail::test_returnable<To>(0))::value &&
	 decltype(detail::test_implicitly_convertible<From, To>(0))::value) ||
	(is_void<From>::value && is_void<To>::value)
> {};

// https://stackoverflow.com/a/78566465/4818802
namespace detail {
	template <typename, typename, typename = void>
	struct is_invocable : false_type {};

	template <typename F, typename... Args>
	struct is_invocable<
		F,
		param_pack<Args...>,
		void_t<invoke_result_t<F, Args...>>>
	: true_type {};
}

template <typename F, typename... Args>
struct is_invocable : detail::is_invocable<F, param_pack<Args...>> {};

namespace detail
{
	template <typename, typename, typename, typename = void>
	struct is_invocable_r : false_type {};

	template <typename R, typename F, typename... Args>
	struct is_invocable_r<
		R,
		F,
		param_pack<Args...>,
		void_t<is_convertible<invoke_result<F, Args...>, R>>>
	: true_type {};
}

template <typename R, typename F, typename... Args>
struct is_invocable_r : detail::is_invocable_r<R, F, param_pack<Args...>> {};

#ifdef NUKLEUS_CPP14
template <typename From, typename To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;

template <typename Fn, typename... Args>
constexpr bool is_invocable_v = is_invocable<Fn, Args...>::value;

template <typename R, typename Fn, typename... Args>
constexpr bool is_invocable_r_v = is_invocable_r<R, Fn, Args...>::value;
#endif

/// @}

/// @} // stdlib

/**
 * @defgroup basic Basic
 * @{
 */

/**
 * @defgroup types Types and Aliases
 * @{
 */

// Need to use struct/enum to disambiguate from functions with the same name
using byte = nk_byte;
using hash = nk_hash;
using rune = nk_rune; ///< Unicode UTF-32 codepoint
using uint = nk_uint;
using ushort = nk_ushort;
using handle = nk_handle;

using color_format = nk_color_format;
using image = struct nk_image;
using nine_slice = nk_nine_slice;
using symbol_type = nk_symbol_type;
using heading = nk_heading;

using widget_layout_states = nk_widget_layout_states;
using buttons = nk_buttons;
using text_align = nk_text_align;
using style_button = nk_style_button;
using button_behavior = enum nk_button_behavior;

/// @} // types

/**
 * @defgroup math Math
 * @details Nukleus math types are implicitly convertible to/from Nuklear types if possible.
 * @{
 */

template <typename T>
constexpr T max_unsigned_value()
{
	// use guuaranteed overflow on unsigned types
	static_assert(static_cast<T>(0) < static_cast<T>(-1), "type should be unsigned");
	return static_cast<T>(-1);
}

template <typename T>
constexpr T max_signed_value()
{
	static_assert(static_cast<T>(0) > static_cast<T>(-1), "type should be signed");
	// shift twice with -1 in between to avoid overflow
	return
		(static_cast<T>(1) << (sizeof(T) * 8 - 2)) - static_cast<T>(1) +
		(static_cast<T>(1) << (sizeof(T) * 8 - 2));
}

/**
 * @brief 2D vector type mirroring `nk_vec2(i)`, implicitly convertible to/from Nuklear's types
 * @tparam T storage type
 */
template <typename T>
struct vec2
{
	vec2() = default;

	vec2(T x, T y)
	: x(x), y(y)
	{}

	template <enable_if<is_same<T, float>::value>* = nullptr>
	vec2(struct nk_vec2 v)
	: x(v.x), y(v.y)
	{}

	template <enable_if<is_same<T, short>::value>* = nullptr>
	vec2(struct nk_vec2i v)
	: x(v.x), y(v.y)
	{}

	template <enable_if<is_same<T, float>::value>* = nullptr>
	operator struct nk_vec2() const { return {x, y}; }

	template <enable_if<is_same<T, short>::value>* = nullptr>
	operator struct nk_vec2i() const { return {x, y}; }

	T x{};
	T y{};
};

/**
 * @brief Rectangle type mirroring `nk_rect(i)`, implicitly convertible to/from Nuklear's types
 * @tparam T storage type
 */
template <typename T>
struct rect
{
	rect() = default;

	rect(T x, T y, T w, T h)
	: x(x), y(y), w(w), h(h)
	{}

	rect(vec2<T> pos, vec2<T> size)
	: x(pos.x), y(pos.y), w(size.x), h(size.y)
	{}

	template <enable_if<is_same<T, float>::value>* = nullptr>
	rect(struct nk_rect r)
	: x(r.x), y(r.y), w(r.w), h(r.h)
	{}

	template <enable_if<is_same<T, short>::value>* = nullptr>
	rect(struct nk_recti r)
	: x(r.x), y(r.y), w(r.w), h(r.h)
	{}

	vec2<T> pos() const { return {x, y}; }
	vec2<T> size() const { return {w, h}; }

	template <enable_if<is_same<T, float>::value>* = nullptr>
	operator struct nk_rect() const { return {x, y, w, h}; }

	template <enable_if<is_same<T, short>::value>* = nullptr>
	operator struct nk_recti() const { return {x, y, w, h}; }

	T x{};
	T y{};
	T w{};
	T h{};
};

template <typename T>
struct triangle
{
	vec2<T> a;
	vec2<T> b;
	vec2<T> c;
};

inline triangle<float> triangle_from_direction(rect<float> r, float pad_x, float pad_y, heading direction)
{
	struct nk_vec2 result[3];
	nk_triangle_from_direction(result, r, pad_x, pad_y, direction);
	return {result[0], result[1], result[2]};
}

inline hash murmur_hash(const void* data, int len, hash seed)
{
	return nk_murmur_hash(data, len, seed);
}

/// @} // math

/**
 * @defgroup color Color and Styling
 * @details Nukleus color types are implicitly convertible to/from Nuklear types.
 * @{
 */

/**
 * @brief RGBA color type with byte storage: [0, 255]
 * @details mirrors nk_color
 */
struct color
{
	color() = default;

	color(nk_byte r, nk_byte g, nk_byte b, nk_byte a = 255)
	: r(r), g(g), b(b), a(a)
	{}

	/* implicit */ color(nk_color col)
	: color(col.r, col.g, col.b, col.a)
	{}

	color(nk_color col, nk_byte a)
	: color(col.r, col.g, col.b, a)
	{}

	/* implicit */ color(nk_colorf col)
	: color(nk_rgba_cf(col))
	{}

	color(nk_colorf col, float a)
	: color(nk_rgba_f(col.r, col.g, col.b, a))
	{}

	color(nk_colorf col, nk_byte a)
	: color(nk_rgb_f(col.r, col.g, col.b), a)
	{}

	/* implicit */ operator nk_color() const { return {r, g, b, a}; }

	/**
	 * @name from RGB (int)
	 * @{
	 */

	/// @brief clamps values in range [0, 255]
	NUKLEUS_NODISCARD static color from_rgb(int r, int g, int b) { return nk_rgb(r, g, b); }
	NUKLEUS_NODISCARD static color from_rgb(nk_byte r, nk_byte g, nk_byte b) { return nk_color{r, g, b, static_cast<nk_byte>(255)}; }
	/// @brief clamps values in range [0, 255]; [rgb, rgb+3) must be a valid range
	NUKLEUS_NODISCARD static color from_rgb(const int* rgb) { return nk_rgb_iv(rgb); }
	/// @brief [rgb, rgb+3) must be a valid range
	NUKLEUS_NODISCARD static color from_rgb(const nk_byte* rgb) { return nk_rgb_bv(rgb); }

	/// @}

	/**
	 * @name from RGB (float)
	 * @{
	 */

	/// @brief clamps values in range [0, 1.0]
	NUKLEUS_NODISCARD static color from_rgb(float r, float g, float b) { return nk_rgb_f(r, g, b); }
	/// @brief clamps values in range [0, 1.0]; [rgb, rgb+3) must be a valid range
	NUKLEUS_NODISCARD static color from_rgb(const float* rgb) { return nk_rgb_fv(rgb); }

	/// @}

	/**
	 * @name from RGBA (int)
	 * @{
	 */

	/// @brief clamps values in range [0, 255]
	NUKLEUS_NODISCARD static color from_rgba(int r, int g, int b, int a) { return nk_rgba(r, g, b, a); }
	NUKLEUS_NODISCARD static color from_rgba(nk_byte r, nk_byte g, nk_byte b, nk_byte a) { return nk_color{r, g, b, a}; }
	/// @brief clamps values in range [0, 255]; [rgba, rgba+4) must be a valid range
	NUKLEUS_NODISCARD static color from_rgba(const int* rgba) { return nk_rgba_iv(rgba); }
	/// @brief [rgba, rgba+4) must be a valid range
	NUKLEUS_NODISCARD static color from_rgba(const nk_byte* rgba) { return nk_rgba_bv(rgba); }

	NUKLEUS_NODISCARD static color from_rgba(nk_uint rgba) { return nk_rgba_u32(rgba); }
	/// @brief multiply other color by a factor, alpha unchanged
	NUKLEUS_NODISCARD static color from_factor(nk_color col, float factor) { return nk_rgb_factor(col, factor); }

	/// @}

	/**
	 * @name from RGBA (float)
	 * @{
	 */

	/// @brief clamps values in valid range [0, 1.0]
	NUKLEUS_NODISCARD static color from_rgba(float r, float g, float b, float a) { return nk_rgba_f(r, g, b, a); }
	/// @brief clamps values in range [0, 1.0]; [rgba, rgba+4) must be a valid range
	NUKLEUS_NODISCARD static color from_rgba(const float* rgba) { return nk_rgba_fv(rgba); }

	/// @}

	/**
	 * @name from HSV (int)
	 * @{
	 */

	/// @brief clamps values in range [0, 255]
	NUKLEUS_NODISCARD static color from_hsv(int h, int s, int v) { return nk_hsv(h, s, v); }
	NUKLEUS_NODISCARD static color from_hsv(nk_byte h, nk_byte s, nk_byte v) { return nk_hsva_f(h / 255.0f, s / 255.0f, v / 255.0f, 1.0f); }
	/// @brief clamps values in range [0, 255]; [hsv, hsv+3) must be a valid range
	NUKLEUS_NODISCARD static color from_hsv(const int* hsv) { return nk_hsv_iv(hsv); }
	/// @brief [hsv, hsv+3) must be a valid range
	NUKLEUS_NODISCARD static color from_hsv(const nk_byte* hsv) { return nk_hsv_bv(hsv); }

	/// @}

	/**
	 * @name from HSV (float)
	 * @{
	 */

	NUKLEUS_NODISCARD static color from_hsv(float h, float s, float v) { return nk_hsv_f(h, s, v); }
	/// @brief [hsv, hsv+3) must be a valid range
	NUKLEUS_NODISCARD static color from_hsv(const float* hsv) { return nk_hsv_fv(hsv); }

	/// @}

	/**
	 * @name from HSVA (int)
	 * @{
	 */

	/// @brief clamps values in range [0, 255]
	NUKLEUS_NODISCARD static color from_hsva(int h, int s, int v, int a) { return nk_hsva(h, s, v, a); }
	NUKLEUS_NODISCARD static color from_hsva(nk_byte h, nk_byte s, nk_byte v, nk_byte a) { return nk_hsva_f(h / 255.0f, s / 255.0f, v / 255.0f, a / 255.0f); }
	/// @brief clamps values in range [0, 255]; [hsv, hsv+4) must be a valid range
	NUKLEUS_NODISCARD static color from_hsva(const int* hsva) { return nk_hsva_iv(hsva); }
	/// @brief [hsva, hsva+4) must be a valid range
	NUKLEUS_NODISCARD static color from_hsva(const nk_byte* hsva) { return nk_hsva_bv(hsva); }

	/// @}

	/**
	 * @name from HSVA (float)
	 * @{
	 */

	NUKLEUS_NODISCARD static color from_hsva(float h, float s, float v, float a) { return nk_hsva_f(h, s, v, a); }
	/// @brief [hsva, hsva+4) must be a valid range
	NUKLEUS_NODISCARD static color from_hsva(const float* hsva) { return nk_hsva_fv(hsva); }

	/// @}

	/**
	 * @name from HEX
	 * @{
	 */

	/// @brief string should have the form "#RRGGBB" or "RRGGBB"
	NUKLEUS_NODISCARD static color from_rgb_hex(const char* rgb) { return nk_rgb_hex(rgb); }
	/// @brief string should have the form "#RRGGBBAA" or "RRGGBBAA"
	NUKLEUS_NODISCARD static color from_rgba_hex(const char* rgba) { return nk_rgba_hex(rgba); }

	/// @}

	/**
	 * @name to HEX
	 * @{
	 */

	/// @brief [output, output+7) must be a valid range
	void to_rgb_hex(char* output) const { nk_color_hex_rgb(output, *this); }
	/// @brief [output, output+9) must be a valid range
	void to_rgba_hex(char* output) const { nk_color_hex_rgba(output, *this); }

	/// @}

	nk_uint to_u32() const { return nk_color_u32(*this); }

	nk_byte r = 0;
	nk_byte g = 0;
	nk_byte b = 0;
	nk_byte a = 255;
};

/**
 * @brief RGBA color type with float storage: [0, 1.0f]
 * @details mirrors nk_colorf
 */
struct colorf
{
	colorf() = default;

	colorf(float r, float g, float b, float a = 1.0f)
	: r(r), g(g), b(b), a(a)
	{}

	/* implicit */ colorf(nk_colorf col)
	: colorf(col.r, col.g, col.b, col.a)
	{}

	colorf(nk_colorf col, float a)
	: colorf(col.r, col.g, col.b, a)
	{}

	/* implicit */ colorf(nk_color col)
	: colorf(nk_color_cf(col))
	{}

	colorf(nk_color col, nk_byte a)
	: colorf(nk_color_cf({col.r, col.g, col.b, a}))
	{}

	colorf(nk_color col, float a)
	: colorf(nk_color_cf(col), a)
	{}

	/* implicit */ operator nk_colorf() const { return {r, g, b, a}; }

	/**
	 * @name from HSVA (float)
	 * @{
	 */

	NUKLEUS_NODISCARD static colorf from_hsva(float h, float s, float v, float a) { return nk_hsva_colorf(h, s, v, a); }
	NUKLEUS_NODISCARD static colorf from_hsva(const float* hsva) { return nk_hsva_colorfv(hsva); }

	/// @}

	float r = 0;
	float g = 0;
	float b = 0;
	float a = 1.0f;
};

/**
 * @brief HSVA color type with byte storage: [0, 255]
 */
struct color_hsva
{
	color_hsva() = default;

	/* implicit */ color_hsva(nk_color col)
	{
		nk_color_hsva_b(&h, &s, &v, &a, col);
	}

	color_hsva(nk_color col, nk_byte a)
	{
		nk_color_hsv_b(&h, &s, &v, col);
		this->a = a;
	}

	nk_byte h = 0;
	nk_byte s = 0;
	nk_byte v = 0;
	nk_byte a = 255;
};

/**
 * @brief HSVA color type with float storage: [0, 1.0f]
 */
struct colorf_hsva
{
	colorf_hsva() = default;

	/* implicit */ colorf_hsva(nk_colorf col)
	{
		nk_colorf_hsva_f(&h, &s, &v, &a, col);
	}

	colorf_hsva(nk_colorf col, float a)
	: colorf_hsva(nk_colorf{col.r, col.g, col.b, a})
	{}

	/* implicit */ colorf_hsva(nk_color col)
	{
		nk_color_hsva_f(&h, &s, &v, &a, col);
	}

	float h = 0;
	float s = 0;
	float v = 0;
	float a = 1.0f;
};

// RGBA (int)  => RGBA (float)
// RGBA (float) => RGBA (int)
inline colorf rgba_to_rgbaf(color rgba) { return static_cast<nk_color>(rgba); }
inline color rgbaf_to_rgba(colorf rgbaf) { return static_cast<nk_colorf>(rgbaf); }

// HSVA (int)  => HSVA (float)
// HSVA (float) => HSVA (int)
// inline colorf_hsva hsva_to_hsvaf(color_hsva hsva); // TODO
// inline color_hsva hsvaf_to_hsva(colorf_hsva hsvaf); // TODO

// RGBA (int) => HSVA (int)
// HSVA (int) => RGBA (int)
inline color_hsva rgba_to_hsva(color rgba) { return static_cast<nk_color>(rgba); }
inline color hsva_to_rgba(color_hsva hsva) { return nk_hsva(hsva.h, hsva.s, hsva.v, hsva.a); }

// RGBA (float) => HSVA (float)
// HSVA (float) => RGBA (float)
inline colorf_hsva rgbaf_to_hsvaf(colorf rgbaf) { return static_cast<nk_colorf>(rgbaf); }
inline colorf hsvaf_to_rgbaf(colorf_hsva hsvaf) { return nk_hsva_colorf(hsvaf.h, hsvaf.s, hsvaf.v, hsvaf.a); }

// RGBA (int)  => HSVA (float)
// HSVA (float) => RGBA (int)
inline colorf_hsva rgba_to_hsvaf(color rgba) { return static_cast<nk_color>(rgba); }
inline color hsvaf_to_rgba(colorf_hsva hsvaf) { return nk_hsva_f(hsvaf.h, hsvaf.s, hsvaf.v, hsvaf.a); }

// RGBA (float) => HSVA (int)
// HSVA (int)  => RGBA (float)
inline color_hsva rgbaf_to_hsva(colorf rgbaf) { return rgba_to_hsva(rgbaf_to_rgba(rgbaf)); }
inline colorf hsva_to_rgbaf(color_hsva hsva) { return rgba_to_rgbaf(hsva_to_rgba(hsva)); }

/*
not used:

// RGBA (float) => HSVA (float) // there is already colorf_hsva::colorf_hsva(nk_colorf)
void nk_colorf_hsva_fv(float *hsva, struct nk_colorf in);

// RGBA (int) => RGBA (float) // there is already colorf::colorf(nk_color)
NK_API void nk_color_f(float *r, float *g, float *b, float *a, struct nk_color);
NK_API void nk_color_fv(float *rgba_out, struct nk_color);

// RGBA (int) => RGBA (double) // no need for doubles
NK_API void nk_color_d(double *r, double *g, double *b, double *a, struct nk_color);
NK_API void nk_color_dv(double *rgba_out, struct nk_color);

// RGB (int) => HSV (int) // no alpha
NK_API void nk_color_hsv_i(int *out_h, int *out_s, int *out_v, struct nk_color);
NK_API void nk_color_hsv_iv(int *hsv_out, struct nk_color);
NK_API void nk_color_hsv_bv(nk_byte *hsv_out, struct nk_color);

// RGB (int) => HSV (float) // no alpha
NK_API void nk_color_hsv_f(float *out_h, float *out_s, float *out_v, struct nk_color);
NK_API void nk_color_hsv_fv(float *hsv_out, struct nk_color);

// RGBA (int) => HSVA (int) // already used overload with separate nk_byte
NK_API void nk_color_hsva_i(int *h, int *s, int *v, int *a, struct nk_color);
NK_API void nk_color_hsva_iv(int *hsva_out, struct nk_color);
NK_API void nk_color_hsva_bv(nk_byte *hsva_out, struct nk_color);

// RGBA (int) => HSVA (float) // already used overload with separate float
NK_API void nk_color_hsva_fv(float *hsva_out, struct nk_color);
*/

class color_table
{
public:
	color_table() = default;
	color_table(const nk_color (&table)[NK_COLOR_COUNT])
	{
		for (int i = 0; i < NK_COLOR_COUNT; ++i)
			m_table[i] = table[i];
	}

	nk_color& operator[](nk_style_colors color_index)
	{
		NUKLEUS_ASSERT(color_index < NK_COLOR_COUNT);
		NUKLEUS_ASSERT(color_index >= 0);
		return m_table[color_index];
	}

	nk_color operator[](nk_style_colors color_index) const
	{
		NUKLEUS_ASSERT(color_index < NK_COLOR_COUNT);
		NUKLEUS_ASSERT(color_index >= 0);
		return m_table[color_index];
	}

	      nk_color* get()       { return m_table; }
	const nk_color* get() const { return m_table; }

private:
	nk_color m_table[NK_COLOR_COUNT] = {};
};

inline nk_style_item style_item_color(color c)
{
	return nk_style_item_color(c);
}

inline nk_style_item style_item_image(image img)
{
	return nk_style_item_image(img);
}

inline nk_style_item style_item_nine_slice(nk_nine_slice slice)
{
	return nk_style_item_nine_slice(slice);
}

inline nk_style_item style_item_hide()
{
	return nk_style_item_hide();
}

/// @} // color

/**
 * @defgroup image Image and Handle
 * @{
 */

inline handle make_handle(void* ptr)
{
	return nk_handle_ptr(ptr);
}

inline handle make_handle(int id)
{
	return nk_handle_id(id);
}

inline image make_image(handle h)
{
	return nk_image_handle(h);
}

inline image make_image(void* ptr)
{
	return nk_image_ptr(ptr);
}

inline image make_image(int id)
{
	return nk_image_id(id);
}

NUKLEUS_NODISCARD inline bool is_subimage(const image& img)
{
	return nk_image_is_subimage(&img) == nk_true;
}

inline image make_subimage(void* ptr, ushort width, ushort height, rect<float> sub_region)
{
	return nk_subimage_ptr(ptr, width, height, sub_region);
}

inline image make_subimage(int id, ushort width, ushort height, rect<float> sub_region)
{
	return nk_subimage_id(id, width, height, sub_region);
}

inline image make_subimage(handle h, ushort width, ushort height, rect<float> sub_region)
{
	return nk_subimage_handle(h, width, height, sub_region);
}

/// @} // image

/**
 * @defgroup nine 9-Slice
 * UNDOCUMENTED
 * @{
 */

inline nine_slice make_9slice(handle h, ushort left, ushort top, ushort right, ushort bottom)
{
	return nk_nine_slice_handle(h, left, top, right, bottom);
}

inline nine_slice make_9slice(void* ptr, ushort left, ushort top, ushort right, ushort bottom)
{
	return nk_nine_slice_ptr(ptr, left, top, right, bottom);
}

inline nine_slice make_9slice(int id, ushort left, ushort top, ushort right, ushort bottom)
{
	return nk_nine_slice_id(id, left, top, right, bottom);
}

inline bool is_sub9slice(const nine_slice& img)
{
	return nk_nine_slice_is_sub9slice(&img) != 0;
}

inline nine_slice make_sub9slice(void* ptr, ushort width, ushort height, rect<float> sub_region, ushort left, ushort top, ushort right, ushort bottom)
{
	return nk_sub9slice_ptr(ptr, width, height, sub_region, left, top, right, bottom);
}

inline nine_slice make_sub9slice(int id, ushort width, ushort height, rect<float> sub_region, ushort left, ushort top, ushort right, ushort bottom)
{
	return nk_sub9slice_id(id, width, height, sub_region, left, top, right, bottom);
}

inline nine_slice make_sub9slice(handle h, ushort width, ushort height, rect<float> sub_region, ushort left, ushort top, ushort right, ushort bottom)
{
	return nk_sub9slice_handle(h, width, height, sub_region, left, top, right, bottom);
}

/// @} // nine

/**
 * @defgroup string String utility functions
 * @{
 */

/**
 * @brief Return string length in bytes
 * @param str string to check
 * @return length in bytes (excluding '\0')
 */
inline int strlen(const char* str) { return nk_strlen(str); }

/**
 * @brief String compare, case insensitive
 * @param s1 first string
 * @param s2 second string
 * @return
 * - <0 if s1 < s2
 * - 0 if s1 == s2
 * - >0 if s1 > s2
 */
inline int stricmp(const char* s1, const char* s2) { return nk_stricmp(s1, s2); }

/**
 * @copydoc stricmp
 * @param n max number of characters to compare
 */
inline int stricmpn(const char* s1, const char* s2, int n) { return nk_stricmpn(s1, s2, n); }

/**
 * @brief string to int
 * @param str string to convert
 * @param endptr position after last character interpreted (with const casted away, ugly like strtol)
 * @return value or 0 if convertion failed
 */
inline int strtoi(const char* str, char** endptr = nullptr) { return nk_strtoi(str, endptr); }

/**
 * @brief string to float
 * @param str string to convert
 * @param endptr position after last character interpreted (with const casted away, ugly like strtol)
 * @return value or 0 if convertion failed
 */
inline float strtof(const char* str, char** endptr = nullptr) { return nk_strtof(str, endptr); }

/**
 * @brief A simple string matching with support for few special characters
 * - `C` matches any literal character C
 * - `.` matches any single character
 * - `^` matches the beginning of the input string
 * - `$` matches the end of the input string
 * - `*` matches zero or more occurrences of the previous character
 * @param text text to filter
 * @param regexp filter expression (not actual regex)
 * @return non-zero value on match
 */
inline int strfilter(const char* text, const char* regexp) { return nk_strfilter(text, regexp); }

/**
 * @brief perform fuzzy string search
 * @param str string to search within
 * @param pattern pattern to find
 * @param out_score set if pattern is found, has no intrinsic meaning, comparison only makes sense with same search pattern
 * @return true if each character in pattern is found sequentially within @p str
 */
inline bool strmatch_fuzzy_string(char const* str, char const* pattern, int* out_score = nullptr)
{
	return nk_strmatch_fuzzy_string(str, pattern, out_score) == nk_true;
}

/**
 * @copydoc strmatch_fuzzy_string
 * @param str_len length of @p str
 */
inline bool strmatch_fuzzy_text(const char* str, int str_len, const char* pattern, int* out_score = nullptr)
{
	return nk_strmatch_fuzzy_text(str, str_len, pattern, out_score) == nk_true;
}

/// @} // string

/**
 * @defgroup iterators_ranges Iterators and Ranges
 * @{
 */

/**
 * @brief Class for conveniently iterating over the list of commands
 * @sa context::commands
 */
class command_iterator
{
public:
	command_iterator(nk_context& ctx, const nk_command* cmd)
	: m_ctx(&ctx)
	, m_cmd(cmd)
	{}

	const nk_command& operator*() const noexcept
	{
		return *m_cmd;
	}

	const nk_command* operator->() const noexcept
	{
		return m_cmd;
	}

	command_iterator& operator++()
	{
		m_cmd = nk__next(m_ctx, m_cmd);
		return *this;
	}

	command_iterator operator++(int)
	{
		command_iterator old = *this;
		operator++();
		return old;
	}

	friend bool operator==(command_iterator lhs, command_iterator rhs) noexcept;

private:
	nk_context* m_ctx;
	const nk_command* m_cmd;
};

inline bool operator==(command_iterator lhs, command_iterator rhs) noexcept
{
	return lhs.m_cmd == rhs.m_cmd && lhs.m_ctx == rhs.m_ctx;
}

inline bool operator!=(command_iterator lhs, command_iterator rhs) noexcept
{
	return !(lhs == rhs);
}

/**
 * @brief Class for conveniently iterating over the list of vertex commands
 * @sa context::draw_commands
 */
class draw_command_iterator
{
public:
	draw_command_iterator(const nk_context& ctx, const nk_buffer& buf, const nk_draw_command* cmd)
	: m_ctx(&ctx)
	, m_buf(&buf)
	, m_cmd(cmd)
	{}

	const nk_draw_command& operator*() const noexcept
	{
		return *m_cmd;
	}

	const nk_draw_command* operator->() const noexcept
	{
		return m_cmd;
	}

	draw_command_iterator& operator++()
	{
		m_cmd = nk__draw_next(m_cmd, m_buf, m_ctx);
		return *this;
	}

	draw_command_iterator operator++(int)
	{
		draw_command_iterator old = *this;
		operator++();
		return old;
	}

	friend bool operator==(draw_command_iterator lhs, draw_command_iterator rhs) noexcept;

private:
	const nk_context* m_ctx;
	const nk_buffer* m_buf;
	const nk_draw_command* m_cmd;
};

inline bool operator==(draw_command_iterator lhs, draw_command_iterator rhs) noexcept
{
	return lhs.m_cmd == rhs.m_cmd && lhs.m_buf == rhs.m_buf && lhs.m_ctx == rhs.m_ctx;
}

inline bool operator!=(draw_command_iterator lhs, draw_command_iterator rhs) noexcept
{
	return !(lhs == rhs);
}

#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
/**
 * @brief Class for conveniently iterating through draw list commands
 * @details requires `NK_INCLUDE_VERTEX_BUFFER_OUTPUT`
 * @sa draw_list
 */
class draw_list_iterator
{
public:
	draw_list_iterator(const nk_draw_list& list, const nk_buffer& buf, const nk_draw_command* cmd)
	: m_draw_list(&list)
	, m_buf(&buf)
	, m_cmd(cmd)
	{}

	const nk_draw_command& operator*() const noexcept
	{
		return *m_cmd;
	}

	const nk_draw_command* operator->() const noexcept
	{
		return m_cmd;
	}

	draw_list_iterator& operator++()
	{
		m_cmd = nk__draw_list_next(m_cmd, m_buf, m_draw_list);
		return *this;
	}

	draw_list_iterator operator++(int)
	{
		draw_list_iterator old = *this;
		operator++();
		return old;
	}

	friend bool operator==(draw_list_iterator lhs, draw_list_iterator rhs) noexcept;

private:
	const nk_draw_list* m_draw_list;
	const nk_buffer* m_buf;
	const nk_draw_command* m_cmd;
};

inline bool operator==(draw_list_iterator lhs, draw_list_iterator rhs) noexcept
{
	return lhs.m_draw_list == rhs.m_draw_list && lhs.m_buf == rhs.m_buf && lhs.m_cmd == rhs.m_cmd;
}

inline bool operator!=(draw_list_iterator lhs, draw_list_iterator rhs) noexcept
{
	return !(lhs == rhs);
}
#endif // NK_INCLUDE_VERTEX_BUFFER_OUTPUT

template <typename Iterator>
class range
{
public:
	constexpr range(Iterator b, Iterator e)
	: m_begin(b)
	, m_end(e)
	{}

	constexpr Iterator begin() const
	{
		return m_begin;
	}

	constexpr Iterator end() const
	{
		return m_end;
	}

private:
	Iterator m_begin;
	Iterator m_end;
};

template <typename Iterator>
constexpr bool operator==(range<Iterator> lhs, range<Iterator> rhs)
{
	return lhs.begin() == rhs.begin() && lhs.end() == rhs.end();
}

template <typename Iterator>
constexpr bool operator!=(range<Iterator> lhs, range<Iterator> rhs)
{
	return !(lhs == rhs);
}

template <typename T>
class span
{
public:
	template <unsigned N>
	/* implicit */ constexpr span(T (&array)[N])
	: m_begin(array)
	, m_size(N)
	{}

	constexpr span(T* ptr, int length)
	: m_begin(ptr)
	, m_size(length)
	{}

	constexpr T* data() const
	{
		return m_begin;
	}

	constexpr T* begin() const
	{
		return m_begin;
	}

	constexpr T* end() const
	{
		return m_begin + m_size;
	}

	constexpr int size() const
	{
		return m_size;
	}

	constexpr bool empty() const
	{
		return size() == 0;
	}

	constexpr T& operator[](int index) const
	{
		return m_begin[index];
	}

private:
	T* m_begin;
	int m_size;
};

template <typename T>
constexpr bool operator==(span<T> lhs, span<T> rhs)
{
	return lhs.begin() == rhs.begin() && lhs.end() == rhs.end();
}

template <typename T>
constexpr bool operator!=(span<T> lhs, span<T> rhs)
{
	return !(lhs == rhs);
}

template <typename T, unsigned N>
constexpr span<T> make_span(T (&array)[N])
{
	return span<T>(array);
}

template <typename T, unsigned N>
constexpr span<T> make_span(T* ptr, int length)
{
	return span<T>(ptr, length);
}

/// @} // iterators_ranges

/**
 * @defgroup lifetime RAII support
 * @brief Types that implement support for scoped calls.
 * @details @ref final_action implements the @ref finally keyword from other languages and
 * should be used to defer an action to the end of scope: `auto _ = nk::finally([](){...});`.
 *
 * If you need full lifetime management consider using `std::unique_ptr` with custom deleter.
 *
 * Scope guard classes are used to implement Nuklear's cleanup requirements.
 * These do not rely on final action because:
 * - the implementation needs to store `nk_context` and a function pointer in an accessible way
 * - the implementation can optimize out `bool` member by setting function pointer to null
 *
 * There are multiple such classes because Nuklear's scope management can differ in each case:
 * - Does the `begin` call return `void` (A - always succeeds) or `bool` (B - check required)?
 * - If the `begin` call returns `false`, should the `end` call be made? (B1 - YES, B2 - NO)
 * - Can the `end` call fail? (B2+ - requires additional assertion) (B1+ does not exist)
 *
 * List of Nuklear's cases:
 * - A: `begin` returns `void`, always call `end`, no check to perform
 *   (`event_input`, most `layout`, `widget_disable`, `menubar`)
 * - B1: `begin` returns `bool`, always call `end`, check on `bool`
 *   (`window` - has special `operator bool` in order to support blank window)
 * - B2: `begin` returns `bool`, call `end` if `bool`, check on `bool`
 *   (`group`, `tree`, `chart`, `popup`, `combobox`, `contextual`, `menu`, `tooltip`)
 * - B2+: `begin` returns `bool`, call `end` if `bool`, check on `bool`, `end` can fail (extra assert)
 *   (`button_behavior`, any `style_*`)
 *
 * @ref scope_guard_base implements common code for other scope guard classes.
 * It implements configurable `end` call (pass function pointer or null pointer to constructor).
 * Particular implementations of `operator bool` are expected to be in derived types.
 * Since class is not fully operational by itself, its destructor is protected.
 * - @ref simple_scope_guard implements A.
 * - @ref scope_guard implements B1 and B2.
 * - @ref scoped_override_guard implements B2+
 *
 * Implementation notes:
 * - A: inherit from @ref simple_scope_guard, always pass valid function pointer to ctor
 * - B1: inherit from @ref scope_guard, always pass valid function pointer to ctor
 * - B2: like B1 but pass nullptr to ctor on begin failure
 * - B2+: inherit from @ref scoped_override_guard, pass nullptr to ctor on begin failure
 * - If there are no additional calls to be made available,
 *   the class can be used directly instead of inheriting from it
 *   (some types inherit scope guards to offer widget-scope-limited functionality)
 * - @ref window is B1 and should inherit @ref scope_guard but it inherits @ref simple_scope_guard
 *   instead to implement its own `operator bool` because it also supports blank window instance
 *
 * @{
 */

/**
 * @brief Based on Core Guidelines `gsl::final_action`.
 * @tparam F function object type
 */
template <typename F>
class final_action
{
public:
	static_assert(
		!is_reference<F>::value && !is_const<F>::value && !is_volatile<F>::value,
		"final_action should store its callable by value");

	explicit final_action(F f, bool invoke = true) noexcept
	: m_f(move(f))
	, m_invoke(invoke)
	{}

	final_action(final_action&& other) noexcept
	: m_f(move(other.m_f))
	, m_invoke(exchange(other.m_invoke, false))
	{}

	// For some reason, gsl::final_action deletes move assignment.
	// Perhaps it should never be reset and always perform its action at the end of scope.
	final_action& operator=(final_action&&) noexcept = delete;

	final_action(const final_action&) = delete;
	final_action& operator=(const final_action&) = delete;

	~final_action()
	{
		if (m_invoke)
			m_f();
	}

private:
	F m_f;
	bool m_invoke = true;
};

template <typename F>
NUKLEUS_NODISCARD final_action<remove_cvref_t<F>>
finally(F&& f, bool invoke = true) noexcept
{
	return final_action<remove_cvref_t<F>>(forward<F>(f), invoke);
}

/**
 * @brief Base class for implementing scope guards.
 * @details The destructor is protected because this class does not implement cleanup.
 * Derived types are expected to perform it in their own way.
 */
template <typename F>
class scope_guard_base
{
public:
	using func_type = F;

	explicit scope_guard_base(nk_context& ctx, func_type* func) noexcept
	: m_ctx(&ctx)
	, m_func(func)
	{}

	scope_guard_base(scope_guard_base&& other) noexcept
	: m_ctx(other.m_ctx)
	, m_func(exchange(other.m_func, nullptr))
	{}

	scope_guard_base(const scope_guard_base&) = delete;
	scope_guard_base& operator=(const scope_guard_base&) = delete;
	scope_guard_base& operator=(scope_guard_base&&) noexcept = delete;

	/**
	 * @brief Get the active state of this guard.
	 * @return `true` if an action has begun (but not ended), `false` otherwise.
	 */
	bool is_scope_active() const noexcept
	{
		return m_func != nullptr;
	}

	/**
	 * @brief Get a reference to the context.
	 * @return The context.
	 * @details This function is intended to allow to use Nuklear directly, if such need appears.
	 * Watch out when modifying begin/end state as all objects of this type use the RAII idiom.
	 * @sa release.
	 */
	nk_context& get_context()
	{
		return *m_ctx;
	}

	/**
	 * @copydoc get_context()
	 */
	const nk_context& get_context() const
	{
		return *m_ctx;
	}

	/**
	 * @brief Release the state for the caller's management, like `unique_ptr::release`
	 * @return Function that must be called to free resources/reset state. May be null.
	 */
	NUKLEUS_NODISCARD func_type* release()
	{
		return exchange(m_func, nullptr);
	}

protected:
	// Protected because this class should not be used directly.
	// It does not implement cleanup in the destructor. Derived classes do
	// different cleanups and this class is used as a base to deduplicate code.
	~scope_guard_base() = default;

	func_type* get_func() const
	{
		return m_func;
	}

	void set_func(func_type* func)
	{
		m_func = func;
	}

private:
	nk_context* m_ctx;
	func_type* m_func = nullptr;
};

/**
 * @brief simplest scope guard (A), just calls the function in dtor, has no validity check
 */
class simple_scope_guard : public scope_guard_base<void (nk_context*)>
{
public:
	// bring 2-argument ctor
	using scope_guard_base::scope_guard_base;

	// required because explicit destructor disables move (rule of 5)
	simple_scope_guard(const simple_scope_guard&) = delete;
	simple_scope_guard(simple_scope_guard&&) noexcept = default;
	simple_scope_guard& operator=(const simple_scope_guard&) = delete;
	simple_scope_guard& operator=(simple_scope_guard&&) noexcept = delete;

	/**
	 * @brief Reset state of this guard. Will call end function if active.
	 */
	void reset()
	{
		func_type* func = get_func();
		if (func)
		{
			(*func)(&get_context());
			set_func(nullptr);
		}
	}

	~simple_scope_guard()
	{
		reset();
	}
};

/**
 * @brief typical scope guard (B1, B2), calls the function in dtor, requires validity check
 */
class scope_guard : public simple_scope_guard
{
public:
	using simple_scope_guard::simple_scope_guard;

	explicit operator bool() && noexcept = delete;
	/**
	 * @brief Return whether this scope-widget should be processed.
	 * @return `true` if visible and fillable with widgets, otherwise `false`.
	 */
	explicit operator bool() const & noexcept
	{
		return is_scope_active();
	}
};

/**
 * @brief scope guard for style overrides (B2+), has additional assertion for end call
 */
class scoped_override_guard : public scope_guard_base<nk_bool (nk_context*)>
{
public:
	// bring 2-argument ctor
	using scope_guard_base::scope_guard_base;

	// required because explicit destructor disables move (rule of 5)
	scoped_override_guard(const scoped_override_guard&) = delete;
	scoped_override_guard(scoped_override_guard&&) noexcept = default;
	scoped_override_guard& operator=(const scoped_override_guard&) = delete;
	scoped_override_guard& operator=(scoped_override_guard&&) noexcept = delete;

	~scoped_override_guard()
	{
		reset();
	}

	explicit operator bool() && noexcept = delete;
	explicit operator bool() const & noexcept
	{
		return is_scope_active();
	}

	void reset()
	{
		func_type* func = get_func();
		if (func)
		{
			const nk_bool result = (*func)(&get_context());
			set_func(nullptr);
			NUKLEUS_ASSERT(result == nk_true); // if pop fails, something must gone really wrong
		}
	}
};

/// @} // lifetime

/// @} // basic

/**
 * @defgroup main Main Library
 * @{
 */

/**
 * @defgroup buffers Buffer types
 * @{
 */

/**
 * @brief wraps nk_buffer for convenience
 * @details A basic (double)-buffer with linear allocation and resetting as only
 * freeing policy. The buffer's main purpose is to control all memory management
 * inside the GUI toolkit and still leave memory control as much as possible in
 * the hand of the user while also making sure the library is easy to use if
 * not as much control is needed.
 * In general all memory inside this library can be provided from the user in
 * three different ways.
 *
 * The first way and the one providing most control is by just passing a fixed
 * size memory block. In this case all control lies in the hand of the user
 * since he can exactly control where the memory comes from and how much memory
 * the library should consume. Of course using the fixed size API removes the
 * ability to automatically resize a buffer if not enough memory is provided so
 * you have to take over the resizing. While being a fixed sized buffer sounds
 * quite limiting, it is very effective in this library since the actual memory
 * consumption is quite stable and has a fixed upper bound for a lot of cases.
 *
 * If you don't want to think about how much memory the library should allocate
 * at all time or have a very dynamic UI with unpredictable memory consumption
 * habits but still want control over memory allocation you can use the dynamic
 * allocator based API. The allocator consists of two callbacks for allocating
 * and freeing memory and optional userdata so you can plugin your own allocator.
 *
 * The final and easiest way can be used by defining
 * `NK_INCLUDE_DEFAULT_ALLOCATOR` which uses the standard library memory
 * allocation functions malloc and free and takes over complete control over
 * memory in this library.
 */
class buffer
{
public:
	/**
	 * @name Construction
	 * @{
	 */

	/* implicit */ buffer(nk_buffer buf, bool initialized = true)
	: m_buffer(buf)
	, m_initialized(initialized)
	{}

	buffer(const buffer& other) = delete;
	buffer(buffer&& other) noexcept
	{
		swap(*this, other);
	}

	buffer& operator=(const buffer& other) = delete;
	buffer& operator=(buffer&& other) noexcept
	{
		swap(*this, other);
		return *this;
	}

	~buffer()
	{
		free();
	}

	void free()
	{
		if (!m_initialized)
			return;

		nk_buffer_free(&m_buffer);
		m_initialized = false;
	}

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
	/**
	 * @brief Requires `NK_INCLUDE_DEFAULT_ALLOCATOR`.
	 * @return buffer instance.
	 */
	NUKLEUS_NODISCARD static buffer init_default()
	{
		buffer buf;
		nk_buffer_init_default(&buf.m_buffer);
		buf.m_initialized = true;
		return buf;
	}
#endif

	NUKLEUS_NODISCARD static buffer init(const nk_allocator& alloc, nk_size size)
	{
		buffer buf;
		nk_buffer_init(&buf.m_buffer, &alloc, size);
		buf.m_initialized = true;
		return buf;
	}

	NUKLEUS_NODISCARD static buffer init_fixed(void* memory, nk_size size)
	{
		buffer buf;
		nk_buffer_init_fixed(&buf.m_buffer, memory, size);
		buf.m_initialized = true;
		return buf;
	}

	/// @}

	/**
	 * @name Buffer Management
	 * @{
	 */

	nk_memory_status info() const
	{
		nk_memory_status status;
		nk_buffer_info(&status, &m_buffer);
		return status;
	}

	nk_size total() const
	{
		return nk_buffer_total(&m_buffer);
	}

	void push(nk_buffer_allocation_type type, const void* memory, nk_size size, nk_size align)
	{
		nk_buffer_push(&m_buffer, type, memory, size, align);
	}

	void mark(nk_buffer_allocation_type type)
	{
		nk_buffer_mark(&m_buffer, type);
	}

	void reset(nk_buffer_allocation_type type)
	{
		nk_buffer_reset(&m_buffer, type);
	}

	void clear()
	{
		nk_buffer_clear(&m_buffer);
	}

	/// @}

	/**
	 * @name Access
	 * @{
	 */

	      void* memory()       { return nk_buffer_memory(&m_buffer); }
	const void* memory() const { return nk_buffer_memory_const(&m_buffer); }

	      nk_buffer& get()       { return m_buffer; }
	const nk_buffer& get() const { return m_buffer; }

	/// @}

private:
	buffer() = default;

	nk_buffer m_buffer = {};
	bool m_initialized = false;
};

/**
 * @brief wraps `nk_str` for convenience
 * @details Basic string buffer which is only used in context with the text editor
 * to manage and manipulate dynamic or fixed size string content. This is NOT
 * the default string handling method. The only instance you should have any contact
 * with this API is if you interact with an @ref text_edit object inside one of the
 * copy and paste functions and even there only for more advanced cases.
 */
class string_buffer
{
public:
	/**
	 * @name Construction
	 * @{
	 */

	/* implicit */ string_buffer(nk_str str, bool initialized = true)
	: m_str(str)
	, m_initialized(initialized)
	{}

	string_buffer(const string_buffer& other) = delete;
	string_buffer(string_buffer&& other) noexcept
	{
		swap(*this, other);
	}

	string_buffer& operator=(const string_buffer& other) = delete;
	string_buffer& operator=(string_buffer&& other) noexcept
	{
		swap(*this, other);
		return *this;
	}

	~string_buffer()
	{
		free();
	}

	void free()
	{
		if (!m_initialized)
			return;

		nk_str_free(&m_str);
		m_initialized = false;
	}

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
	/**
	 * @brief Requires `NK_INCLUDE_DEFAULT_ALLOCATOR`.
	 * @return string_buffer instance.
	 */
	NUKLEUS_NODISCARD static string_buffer init_default()
	{
		string_buffer str;
		nk_str_init_default(&str.m_str);
		str.m_initialized = true;
		return str;
	}
#endif

	NUKLEUS_NODISCARD static string_buffer init(const nk_allocator& alloc, nk_size size)
	{
		string_buffer str;
		nk_str_init(&str.m_str, &alloc, size);
		str.m_initialized = true;
		return str;
	}

	NUKLEUS_NODISCARD static string_buffer init_fixed(void* memory, nk_size size)
	{
		string_buffer str;
		nk_str_init_fixed(&str.m_str, memory, size);
		str.m_initialized = true;
		return str;
	}

	/// @}

	/**
	 * @name Management
	 * @{
	 */

	void clear()
	{
		nk_str_clear(&m_str);
	}

	int append_text_char(const char* text, int len)
	{
		return nk_str_append_text_char(&m_str, text, len);
	}

	int append_str_char(const char* str)
	{
		return nk_str_append_str_char(&m_str, str);
	}

	int append_text_utf8(const char* text, int len)
	{
		return nk_str_append_text_utf8(&m_str, text, len);
	}

	int append_str_utf8(const char* str)
	{
		return nk_str_append_str_utf8(&m_str, str);
	}

	int append_text_runes(const nk_rune* text, int len)
	{
		return nk_str_append_text_runes(&m_str, text, len);
	}

	int append_str_runes(const nk_rune* str)
	{
		return nk_str_append_str_runes(&m_str, str);
	}

	int insert_at_char(int pos, const char* text, int len)
	{
		return nk_str_insert_at_char(&m_str, pos, text, len);
	}

	int insert_at_rune(int pos, const char* text, int len)
	{
		return nk_str_insert_at_rune(&m_str, pos, text, len);
	}

	int insert_text_char(int pos, const char* text, int len)
	{
		return nk_str_insert_text_char(&m_str, pos, text, len);
	}

	int insert_str_char(int pos, const char* str)
	{
		return nk_str_insert_str_char(&m_str, pos, str);
	}

	int insert_text_utf8(int pos, const char* text, int len)
	{
		return nk_str_insert_text_utf8(&m_str, pos, text, len);
	}

	int insert_str_utf8(int pos, const char* str)
	{
		return nk_str_insert_str_utf8(&m_str, pos, str);
	}

	int insert_text_runes(int pos, const nk_rune* text, int len)
	{
		return nk_str_insert_text_runes(&m_str, pos, text, len);
	}

	int insert_str_runes(int pos, const nk_rune* str)
	{
		return nk_str_insert_str_runes(&m_str, pos, str);
	}

	void remove_chars(int len)
	{
		nk_str_remove_chars(&m_str, len);
	}

	void remove_runes(int len)
	{
		nk_str_remove_runes(&m_str, len);
	}

	void delete_chars(int pos, int len)
	{
		nk_str_delete_chars(&m_str, pos, len);
	}

	void delete_runes(int pos, int len)
	{
		nk_str_delete_runes(&m_str, pos, len);
	}

	char* at_char(int pos)
	{
		return nk_str_at_char(&m_str, pos);
	}

	const char* at_char(int pos) const
	{
		return nk_str_at_char_const(&m_str, pos);
	}

	char* at_rune(int pos, nk_rune& unicode, int& len)
	{
		return nk_str_at_rune(&m_str, pos, &unicode, &len);
	}

	const char* at_rune(int pos, nk_rune& unicode, int& len) const
	{
		// nuklear's name is inconsistent here, should be nk_str_at_rune_const
		return nk_str_at_const(&m_str, pos, &unicode, &len);
	}

	nk_rune rune_at(int pos) const
	{
		return nk_str_rune_at(&m_str, pos);
	}

	char* get()
	{
		return nk_str_get(&m_str);
	}

	const char* get() const
	{
		return nk_str_get_const(&m_str);
	}

	int len() const
	{
		return nk_str_len(&m_str);
	}

	int len_char() const
	{
		return nk_str_len_char(&m_str);
	}

	      nk_str& get_str()       { return m_str; }
	const nk_str& get_str() const { return m_str; }

	/// @}

private:
	string_buffer() = default;

	nk_str m_str = {};
	bool m_initialized = false;
};

/**
 * @brief Text Editor, wraps `nk_text_edit` for convenience
 * @details Editing text in this library is handled by either @ref window::edit_string or
 * @ref window::edit_buffer. But like almost everything in this library there are multiple
 * ways of doing it and a balance between control and ease of use with memory
 * as well as functionality controlled by flags.
 *
 * This library generally allows three different levels of memory control:
 * First of is the most basic way of just providing a simple char array with
 * string length. This method is probably the easiest way of handling simple
 * user text input. Main upside is complete control over memory while the biggest
 * downside in comparison with the other two approaches is missing undo/redo.
 *
 * For UIs that require undo/redo the second way was created. It is based on
 * a fixed size `nk_text_edit` struct, which has an internal undo/redo stack.
 * This is mainly useful if you want something more like a text editor but don't want
 * to have a dynamically growing buffer.
 *
 * The final way is using a dynamically growing struct, which
 * has both a default version if you don't care where memory comes from and an
 * allocator version if you do. While the text editor is quite powerful for its
 * complexity I would not recommend editing gigabytes of data with it.
 * It is rather designed for uses cases which make sense for a GUI library not for
 * a full blown text editor.
 *
 * @note You may want to define `NK_TEXTEDIT_UNDOSTATECOUNT` and
 * `NK_TEXTEDIT_UNDOCHARCOUNT` if default limits do not meet your needs.
 */
class text_edit
{
public:
	/**
	 * @name Construction
	 * @{
	 */

	/* implicit */ text_edit(nk_text_edit state, bool initialized = true)
	: m_state(state)
	, m_initialized(initialized)
	{}

	text_edit(const text_edit& other) = delete;
	text_edit(text_edit&& other) noexcept
	{
		swap(*this, other);
	}

	text_edit& operator=(const text_edit& other) = delete;
	text_edit& operator=(text_edit&& other) noexcept
	{
		swap(*this, other);
		return *this;
	}

	~text_edit()
	{
		free();
	}

	void free()
	{
		if (!m_initialized)
			return;

		nk_textedit_free(&m_state);
		m_initialized = false;
	}

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
	/**
	 * @brief Requires `NK_INCLUDE_DEFAULT_ALLOCATOR`.
	 * @return text_edit instance.
	 */
	NUKLEUS_NODISCARD static text_edit init_default()
	{
		text_edit state;
		nk_textedit_init_default(&state.m_state);
		state.m_initialized = true;
		return state;
	}
#endif

	NUKLEUS_NODISCARD static text_edit init(const nk_allocator& alloc, nk_size size)
	{
		text_edit state;
		nk_textedit_init(&state.m_state, &alloc, size);
		state.m_initialized = true;
		return state;
	}

	NUKLEUS_NODISCARD static text_edit init_fixed(void* memory, nk_size size)
	{
		text_edit state;
		nk_textedit_init_fixed(&state.m_state, memory, size);
		state.m_initialized = true;
		return state;
	}

	/// @}

	/**
	 * @name Filtering
	 * @{
	 */

	NUKLEUS_NODISCARD bool filter_default(nk_rune unicode) const
	{
		return nk_filter_default(&m_state, unicode) == nk_true;
	}

	NUKLEUS_NODISCARD bool filter_ascii(nk_rune unicode) const
	{
		return nk_filter_ascii(&m_state, unicode) == nk_true;
	}

	NUKLEUS_NODISCARD bool filter_float(nk_rune unicode) const
	{
		return nk_filter_float(&m_state, unicode) == nk_true;
	}

	NUKLEUS_NODISCARD bool filter_decimal(nk_rune unicode) const
	{
		return nk_filter_decimal(&m_state, unicode) == nk_true;
	}

	NUKLEUS_NODISCARD bool filter_hex(nk_rune unicode) const
	{
		return nk_filter_hex(&m_state, unicode) == nk_true;
	}

	NUKLEUS_NODISCARD bool filter_oct(nk_rune unicode) const
	{
		return nk_filter_oct(&m_state, unicode) == nk_true;
	}

	NUKLEUS_NODISCARD bool filter_binary(nk_rune unicode) const
	{
		return nk_filter_binary(&m_state, unicode) == nk_true;
	}

	/// @}

	/**
	 * @name Editing
	 * @{
	 */

	void text(const char* text, int total_len)
	{
		nk_textedit_text(&m_state, text, total_len);
	}

	void delete_at(int where, int len) // can not be named "delete" - clash with keyword
	{
		nk_textedit_delete(&m_state, where, len);
	}

	void delete_selection()
	{
		nk_textedit_delete_selection(&m_state);
	}

	void select_all()
	{
		nk_textedit_select_all(&m_state);
	}

	NUKLEUS_NODISCARD bool cut()
	{
		return nk_textedit_cut(&m_state) == nk_true;
	}

	NUKLEUS_NODISCARD bool paste(const char* text, int len)
	{
		return nk_textedit_paste(&m_state, text, len) == nk_true;
	}

	void undo()
	{
		nk_textedit_undo(&m_state);
	}

	void redo()
	{
		nk_textedit_redo(&m_state);
	}

	      nk_text_edit& get_state()       { return m_state; }
	const nk_text_edit& get_state() const { return m_state; }

	/// @}

private:
	text_edit() = default;

	nk_text_edit m_state = {};
	bool m_initialized = false;
};

/// @} // buffers

/**
 * @defgroup font_handling Font Handling
 * @details Font handling in this library was designed to be quite customizable and let
 * you decide what you want to use and what you want to provide. There are three
 * different ways to use the font atlas:
 * 1. (simplest) your font handling scheme. Requires only essential data.
 * 2. Vertex buffer output. Requires only essential data + `NK_INCLUDE_VERTEX_BUFFER_OUTPUT`.
 * 3. (most complex) Nuklear's font baking API. Requires `NK_INCLUDE_FONT_BAKING`.
 *
 * Additional details: https://github.com/Immediate-Mode-UI/Nuklear/wiki/Complete-font-guide
 *
 * ### (1) Using your own implementation without vertex buffer output
 *
 * The easiest way to do font handling is by just providing a
 * `nk_user_font` struct which only requires the height in pixel of the used
 * font and a callback to calculate the width of a string. This way of handling
 * fonts is best fitted for using the normal draw shape command API where you
 * do all the text drawing yourself and the library does not require any kind
 * of deeper knowledge about which font handling mechanism you use.
 *
 * **Important:** the `nk_user_font` object provided to nuklear has to persist
 * over the complete life time! I know this sucks but it is currently the only
 * way to switch between fonts.
 *
 * ```cpp
 * float your_text_width_calculation(nk_handle handle, float height, const char* text, int len)
 * {
 *     your_font_type* type = handle.ptr;
 *     float text_width = ...;
 *     return text_width;
 * }
 *
 * nk_user_font font;
 * font.userdata.ptr = &your_font_class_or_struct;
 * font.height = your_font_height;
 * font.width = &your_text_width_calculation;
 *
 * auto ctx = context::init_default(font);
 * ```
 *
 * ### (2) Using your own implementation with vertex buffer output
 *
 * While the first approach works fine if you don't want to use the optional
 * vertex buffer output it is not enough if you do. To get font handling working
 * for these cases you have to provide two additional parameters inside `nk_user_font`:
 * - a texture atlas used to draw text as subimages of a bigger font atlas texture
 * - a callback to query a character's glyph information (offset, size, ...)
 *
 * So it is still possible to provide your own font and use the vertex buffer output.
 *
 * ```cpp
 * float your_text_width_calculation(nk_handle handle, float height, const char* text, int len)
 * {
 *     your_font_type* type = handle.ptr;
 *     float text_width = ...;
 *     return text_width;
 * }
 *
 * void query_your_font_glyph(nk_handle handle, float font_height, nk_user_font_glyph* glyph, nk_rune codepoint, nk_rune next_codepoint)
 * {
 *     your_font_type* type = handle.ptr;
 *     glyph.width = ...;
 *     glyph.height = ...;
 *     glyph.xadvance = ...;
 *     glyph.uv[0].x = ...;
 *     glyph.uv[0].y = ...;
 *     glyph.uv[1].x = ...;
 *     glyph.uv[1].y = ...;
 *     glyph.offset.x = ...;
 *     glyph.offset.y = ...;
 * }
 *
 * nk_user_font font;
 * font.userdata.ptr = &your_font_class_or_struct;
 * font.height = your_font_height;
 * font.width = &your_text_width_calculation;
 * font.query = &query_your_font_glyph;
 * font.texture.id = your_font_texture;
 *
 * auto ctx = nk::context::init_default(font);
 * ```
 *
 * ### (3) Nuklear font baker
 *
 * The final approach if you do not have a font handling functionality or don't
 * want to use it in this library is by using the optional font baker.
 * The font baker APIs can be used to create a font plus font atlas texture
 * and can be used with or without the vertex buffer output.
 *
 * It still uses the `nk_user_font` struct and the two different approaches
 * previously stated still work. The font baker is not located inside
 * `nk_context` like all other systems since it can be understood as more of
 * an extension to nuklear and does not really depend on any context state.
 *
 * Font baker needs to be initialized first. If you don't care about memory just call
 * @ref font_atlas::init_default which will allocate all memory from the standard library
 * (requires `NK_INCLUDE_DEFAULT_ALLOCATOR`). If you want to control memory allocation
 * but you don't care if the allocated memory is temporary (which can be freed directly
 * after the baking process is over) or permanent you can call @ref font_atlas::init.
 * If you do care, use @ref font_atlas::init_custom.
 *
 * After successfully initializing the font baker you can add TrueType fonts (`.ttf`)
 * from different sources like memory or files by calling one of the atlas functions.
 * Adding a font will permanently store it with font config and the TTF memory block
 * inside the atlas for later reuse. If you don't want to reuse the font baker
 * (by for example adding additional fonts) you can call @ref font_atlas::cleanup
 * after the baking process is over (after end of @ref font_atlas::end).
 *
 * As soon as you added all fonts you wanted you can now start the baking process
 * for every selected glyph to image by calling @ref font_atlas::bake.
 * The baking process returns image memory, width and height which can be used to
 * either create your own image object or upload it to any graphics library.
 * No matter which case, you finally have to call @ref font_atlas::end which
 * will free all temporary memory including the font atlas image so make sure
 * you created the texture beforehand. @ref font_atlas::end requires a handle
 * to your font texture or object and optionally fills a `struct nk_draw_null_texture`
 * which can be used for the optional vertex output. If you don't want it just
 * set the argument to `nullptr`.
 *
 * At this point you are done and if you don't want to reuse the font atlas you
 * can call @ref font_atlas::cleanup to free all TrueType blobs and configuration
 * memory. Finally if you don't use the font atlas and any of its fonts anymore
 * you need to call @ref font_atlas::clear to free all memory still being used.
 * This function is also automatically called in the atlas destructor.
 *
 * ```cpp
 * auto atlas = nk::font_atlas::init_default();
 *
 * atlas.begin();
 * nk_font* font1 = atlas.add_from_file("Path/To/Your/TTF_Font1.ttf", 13);
 * nk_font* font2 = atlas.add_from_file("Path/To/Your/TTF_Font2.ttf", 16);
 * nk::vec2<int> dimentions{};
 * const void* img = atlas.bake(dimentions, NK_FONT_ATLAS_RGBA32);
 * auto font_tex = upload_texture_to_gpu(image, dimentions);
 * nk_draw_null_texture tex_null = atlas.end(nk_handle_id(font_tex));
 *
 * auto ctx = nk::context::init_default(font1->handle);
 * ```
 *
 * The font baker API is probably the most complex API inside this library and
 * Nuklear suggests reading examples in the `example/` directory to get a grip on how
 * to use the font atlas. There is a number of details which are left out. For example:
 * how to merge fonts, configure a font with `nk_font_config` to use other languages,
 * use another texture coordinate format and a lot more:
 *
 * ```cpp
 * nk::font_config cfg(font_pixel_height);
 * cfg.get().merge_mode = nk_true;
 * cfg.get().range = nk_font_korean_glyph_ranges();
 * cfg.get().coord_type = NK_COORD_PIXEL;
 * nk_font* font = atlas.add_from_file("Path/To/Your/TTF_Font.ttf", 13, cfg);
 * ```
 *
 * @{
 */

#ifdef NK_INCLUDE_FONT_BAKING

inline const nk_font_glyph* font_find_glyph(struct nk_font& font, nk_rune unicode)
{
	return nk_font_find_glyph(&font, unicode);
}

/**
 * @brief Font configuration storage. Requires `NK_INCLUDE_FONT_BAKING`.
 */
class font_config
{
public:
	font_config(float pixel_height)
	: m_config(nk_font_config(pixel_height))
	{}

	void reset(float pixel_height)
	{
		m_config = nk_font_config(pixel_height);
	}

	      struct nk_font_config& get()       { return m_config; }
	const struct nk_font_config& get() const { return m_config; }

private:
	struct nk_font_config m_config = {};
};

/**
 * @brief Font Atlas class for most complex font handling variant. Requires `NK_INCLUDE_FONT_BAKING`.
 */
class font_atlas
{
public:
	/**
	 * @name Construction
	 * @{
	 */
	~font_atlas()
	{
		clear();
	}

	/* implicit */ font_atlas(nk_font_atlas atlas, bool initialized = true)
	: m_atlas(atlas)
	, m_initialized(initialized)
	{}

	font_atlas(const font_atlas& other) = delete;
	font_atlas(font_atlas&& other) noexcept
	{
		swap(*this, other);
	}

	font_atlas& operator=(const font_atlas& other) = delete;
	font_atlas& operator=(font_atlas&& other) noexcept
	{
		swap(*this, other);
		return *this;
	}

	/**
	 * @brief Free all atlas resources. Called in the destructor.
	 */
	void clear()
	{
		if (!m_initialized)
			return;

		nk_font_atlas_clear(&m_atlas);
		m_initialized = false;
	}

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
	/**
	 * @brief Create font atlas with standard library memory allocation. Requires `NK_INCLUDE_DEFAULT_ALLOCATOR`.
	 * @return Font atlas object.
	 */
	NUKLEUS_NODISCARD static font_atlas init_default()
	{
		font_atlas atlas;
		nk_font_atlas_init_default(&atlas.m_atlas);
		atlas.m_initialized = true;
		return atlas;
	}
#endif

	/**
	 * @brief Create font atlas with custom allocator.
	 * @param allocator The allocator to use.
	 * @return Font atlas object.
	 */
	NUKLEUS_NODISCARD static font_atlas init(const nk_allocator& allocator)
	{
		font_atlas atlas;
		nk_font_atlas_init(&atlas.m_atlas, &allocator);
		atlas.m_initialized = true;
		return atlas;
	}

	/**
	 * @brief Create font atlas with custom allocators.
	 * @param persistent The allocator to use for persistent storage.
	 * @param transient The allocator to use for temporary storage (see @ref cleanup).
	 * @return Font atlas object.
	 */
	NUKLEUS_NODISCARD static font_atlas init_custom(const nk_allocator& persistent, const nk_allocator& transient)
	{
		font_atlas atlas;
		nk_font_atlas_init_custom(&atlas.m_atlas, &persistent, &transient);
		atlas.m_initialized = true;
		return atlas;
	}

	/// @}

	/**
	 * @name Baking
	 * @{
	 */

	/**
	 * @brief Begin the font baking. See @ref bake.
	 */
	void begin()
	{
		nk_font_atlas_begin(&m_atlas);
	}

	NUKLEUS_NODISCARD nk_font* add(const struct nk_font_config& config)
	{
		return nk_font_atlas_add(&m_atlas, &config); // (config can not be null here)
	}

	NUKLEUS_NODISCARD nk_font* add(const font_config& config)
	{
		return add(config.get());
	}

#ifdef NK_INCLUDE_DEFAULT_FONT
	/**
	 * @brief Add Nuklear's built-in ProggyClean font. Requires `NK_INCLUDE_DEFAULT_FONT`.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	NUKLEUS_NODISCARD nk_font* add_default(float height, const struct nk_font_config* config = nullptr)
	{
		return nk_font_atlas_add_default(&m_atlas, height, config);
	}

	/**
	 * @copydoc add_default(float, const struct nk_font_config*)
	 */
	NUKLEUS_NODISCARD nk_font* add_default(float height, const font_config& config)
	{
		return add_default(height, &config.get());
	}
#endif

	/**
	 * @brief Add a font from memory.
	 * @param memory Address of the memory block.
	 * @param size length of the memory block.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	NUKLEUS_NODISCARD nk_font* add_from_memory(const void* memory, nk_size size, float height, const struct nk_font_config* config = nullptr)
	{
		// const_cast is fine: the library never modifies the data (actually copies it) - API const issue?
		return nk_font_atlas_add_from_memory(&m_atlas, const_cast<void*>(memory), size, height, config);
	}

	/**
	 * @copydoc add_from_memory(const void*, nk_size, float, const struct nk_font_config*)
	 */
	NUKLEUS_NODISCARD nk_font* add_from_memory(const void* memory, nk_size size, float height, const font_config& config)
	{
		return add_from_memory(memory, size, height, &config.get());
	}

	/**
	 * @brief Add a compressed font from memory.
	 * @param memory Address of the memory block.
	 * @param size length of the memory block.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	NUKLEUS_NODISCARD nk_font* add_compressed(const void* memory, nk_size size, float height, const struct nk_font_config* config = nullptr)
	{
		// const_cast is fine: the library never modifies the data (actually copies it) - API const issue?
		return nk_font_atlas_add_compressed(&m_atlas, const_cast<void*>(memory), size, height, config);
	}

	/**
	 * @copydoc add_compressed(const void*, nk_size, float, const struct nk_font_config*)
	 */
	NUKLEUS_NODISCARD nk_font* add_compressed(const void* memory, nk_size size, float height, const font_config& config)
	{
		return add_compressed(memory, size, height, &config.get());
	}

	/**
	 * @brief Add a compressed font from memory which is base-85 encoded on top.
	 * @param data Null-terminated base-85 string.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	NUKLEUS_NODISCARD nk_font* add_compressed_base85(const char* data, float height, const struct nk_font_config* config = nullptr)
	{
		return nk_font_atlas_add_compressed_base85(&m_atlas, data, height, config);
	}

	/**
	 * @copydoc add_compressed_base85(const char*, float, const struct nk_font_config*)
	 */
	NUKLEUS_NODISCARD nk_font* add_compressed_base85(const char* data, float height, const font_config& config)
	{
		return add_compressed_base85(data, height, &config.get());
	}

#ifdef NK_INCLUDE_STANDARD_IO
	/**
	 * @brief Add a font from file. Requires `NK_INCLUDE_STANDARD_IO`.
	 * @param file_path Path to the file with font data.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	NUKLEUS_NODISCARD nk_font* add_from_file(const char* file_path, float height, const struct nk_font_config* config = nullptr)
	{
		return nk_font_atlas_add_from_file(&m_atlas, file_path, height, config);
	}

	/**
	 * @copydoc add_from_file(const char*, float, const struct nk_font_config*)
	 */
	NUKLEUS_NODISCARD nk_font* add_from_file(const char* file_path, float height, const font_config& config)
	{
		return add_from_file(file_path, height, &config.get());
	}
#endif

	/**
	 * @brief Perform the baking process.
	 * @param dimentions Resulting image dimentions.
	 * @param fmt One of available atlas formats.
	 * @return Pointer to resulting image.
	 * @attention This function must be called between @ref begin and @ref end.
	 */
	NUKLEUS_NODISCARD const void* bake(vec2<int>& dimentions, nk_font_atlas_format fmt)
	{
		return nk_font_atlas_bake(&m_atlas, &dimentions.x, &dimentions.y, fmt);
	}

	NUKLEUS_NODISCARD nk_draw_null_texture end(handle texture)
	{
		nk_draw_null_texture tex_null{};
		nk_font_atlas_end(&m_atlas, texture, &tex_null);
		return tex_null;
	}

	/**
	 * @brief Free any resources that were allocated for the baking process. Can be called after @ref end.
	 */
	void cleanup()
	{
		nk_font_atlas_cleanup(&m_atlas);
	}

	/// @}

	/**
	 * @name Access
	 * @{
	 */

	      nk_font_atlas& get()       { return m_atlas; }
	const nk_font_atlas& get() const { return m_atlas; }

	nk_user_font* get_default_font()
	{
		if (!m_initialized)
			return nullptr;

		if (m_atlas.default_font == nullptr)
			return nullptr;

		return &m_atlas.default_font->handle;
	}

	const nk_user_font* get_default_font() const
	{
		if (!m_initialized)
			return nullptr;

		if (m_atlas.default_font == nullptr)
			return nullptr;

		return &m_atlas.default_font->handle;
	}

	/// @}

private:
	font_atlas() = default;

	nk_font_atlas m_atlas = {};
	bool m_initialized = false;
};

#endif // NK_INCLUDE_FONT_BAKING

/// @} // font_handling

/**
 * @defgroup core Core
 * @brief Context, Windows and Widgets.
 * @details The library's core are simple classes that wrap underlying begin/end calls.
 * In Nuklear, pretty much every function uses the `nk_context` struct.
 * In Nukleus, it's @ref context and @ref window classes.
 * Their functions have been grouped by usage patterns and lifetime requirements.
 * For example, you can not call a widget or layout function without first starting a window.
 * For this reason, Nukleus does not bundle all functions in the context class.
 * Instead, each class (including context) returns scope guard objects that give access to specific functions.
 * Each scope object manages the lifetime of a specific entity (context, window, layout, etc.).
 * First create a context object, then a window object from it and then you can create
 * individual widgets and layout from the window object.
 *
 * This design:
 * - Nicely groups related functions together.
 * - Prevents calling specific functions when they should not be called.
 * - Automatically manages begin/end calls through RAII.
 *
 * Object creation tree:
 * - @ref context
 *   - @ref event_input
 *   - @ref window
 *     - @ref tree
 *     - @ref layout, @ref layout_row, @ref layout_row_template, @ref layout_space
 *       - @ref group
 *     - @ref menu
 *     - @ref combobox
 *     - @ref contextual
 *     - @ref popup
 *     - @ref chart
 *     - @ref canvas
 *     - individual widgets in a single function like buttons, labels etc.
 *
 * Each class also gives an access to the underlying context object if you ever have a need to use Nuklear directly.
 *
 * @attention Normally you should not need to call constructors of any of these types.
 * The constructors assume `nk_xxx_begin` was already called.
 * Constructors are public for implementation reasons; they are not intended for public use.
 * Use context's static functions (named constructor idiom) to create it and then
 * use context's public functions to create all descendant widget/layout etc. objects.
 *
 * @{
 */

/**
 * @brief Input class, offering access to provide events to Nuklear.
 * @details The input API is responsible for holding the current input state
 * composed of mouse, key and text input states.
 * It is worth noting that no direct OS or window handling is done in nuklear.
 * Instead all input state has to be provided by platform specific code. This on one hand
 * expects more work from the user and complicates usage but on the other hand
 * provides simple abstraction over a big number of platforms, libraries and other
 * already provided functionality.
 * Input state needs to be provided to nuklear by first calling @ref context::input_scoped
 * which resets internal state like delta mouse position and button transitions.
 * After it, all current input state needs to be provided. This includes
 * mouse motion, button and key pressed and released, text input and scrolling.
 * Both event- or state-based input handling are supported by this API
 * and should work without problems. Finally after all input state has been provided
 * @ref event_input destructor calls internally `nk_input_end` to finish the input process.
 *
 * This class is named `event_input` to avoid confusing it with hypothetical `nk_input` struct wrapper.
 */
class event_input : public simple_scope_guard
{
public:
	using simple_scope_guard::simple_scope_guard;

	/**
	 * @name Setters
	 * @{
	 */

	/**
	 * @brief Mirrors current mouse position to nuklear.
	 * @param x current mouse cursor x-position
	 * @param y current mouse cursor y-position
	 */
	void motion(int x, int y)
	{
		nk_input_motion(&get_context(), x, y);
	}

	/**
	 * @brief Mirrors the state of a specific key to nuklear.
	 * @param key any value specified in enum nk_keys that needs to be mirrored
	 * @param down false for key being up and true for key being down
	 */
	void key(nk_keys key, bool down)
	{
		nk_input_key(&get_context(), key, down);
	}

	/**
	 * @brief Mirrors the state of a specific mouse button to nuklear.
	 * @param button any value specified in enum nk_buttons that needs to be mirrored
	 * @param x mouse cursor x-position on click up/down
	 * @param y mouse cursor y-position on click up/down
	 * @param down false for key being up and true for key being down
	 */
	void button(nk_buttons button, int x, int y, bool down)
	{
		nk_input_button(&get_context(), button, x, y, down);
	}

	/**
	 * @brief Copies the last mouse scroll value to nuklear.
	 * @param val vector with both X- as well as Y-scroll value
	 */
	void scroll(vec2<float> val)
	{
		nk_input_scroll(&get_context(), val);
	}

	/**
	 * @brief Copies a single ASCII character into an internal text buffer.
	 * This is basically a helper function to quickly push ASCII characters into nuklear.
	 * @param c an ASCII character, preferably one that can be printed
	 * @attention Stores up to `NK_INPUT_MAX` bytes.
	 */
	void char_(char c)
	{
		nk_input_char(&get_context(), c);
	}

	/**
	 * @brief Converts an encoded unicode rune into UTF-8 and copies the result into an
	 * internal text buffer.
	 * @param g UTF-32 unicode codepoint
	 * @attention Stores up to `NK_INPUT_MAX` bytes.
	 */
	void glyph(nk_glyph g)
	{
		nk_input_glyph(&get_context(), g);
	}

	/**
	 * @brief Converts a unicode rune into UTF-8 and copies the result
	 * into an internal text buffer.
	 * @param rune UTF-32 unicode codepoint
	 * @attention Stores up to `NK_INPUT_MAX` bytes.
	 */
	void unicode(nk_rune rune)
	{
		nk_input_unicode(&get_context(), rune);
	}

	/// @}

	/**
	 * @name Getters
	 * @{
	 */

	bool has_mouse_click(nk_buttons id) const
	{
		return nk_input_has_mouse_click(&get(), id) == nk_true;
	}

	bool has_mouse_click_in_rect(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_has_mouse_click_in_rect(&get(), id, bounds) == nk_true;
	}

	bool has_mouse_click_in_button_rect(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_has_mouse_click_in_button_rect(&get(), id, bounds) == nk_true;
	}

	bool has_mouse_click_down_in_rect(nk_buttons id, rect<float> bounds, bool down) const
	{
		return nk_input_has_mouse_click_down_in_rect(&get(), id, bounds, down) == nk_true;
	}

	bool is_mouse_click_in_rect(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_is_mouse_click_in_rect(&get(), id, bounds) == nk_true;
	}

	bool is_mouse_click_down_in_rect(nk_buttons id, rect<float> bounds, bool down) const
	{
		return nk_input_is_mouse_click_down_in_rect(&get(), id, bounds, down) == nk_true;
	}

	bool any_mouse_click_in_rect(rect<float> bounds) const
	{
		return nk_input_any_mouse_click_in_rect(&get(), bounds) == nk_true;
	}

	bool is_mouse_prev_hovering_rect(rect<float> bounds) const
	{
		return nk_input_is_mouse_prev_hovering_rect(&get(), bounds) == nk_true;
	}

	bool is_mouse_hovering_rect(rect<float> bounds) const
	{
		return nk_input_is_mouse_hovering_rect(&get(), bounds) == nk_true;
	}

	bool mouse_clicked(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_mouse_clicked(&get(), id, bounds) == nk_true;
	}

	bool is_mouse_down(nk_buttons id) const
	{
		return nk_input_is_mouse_down(&get(), id) == nk_true;
	}

	bool is_mouse_pressed(nk_buttons id) const
	{
		return nk_input_is_mouse_pressed(&get(), id) == nk_true;
	}

	bool is_mouse_released(nk_buttons id) const
	{
		return nk_input_is_mouse_released(&get(), id) == nk_true;
	}

	bool is_key_pressed(nk_keys key) const
	{
		return nk_input_is_key_pressed(&get(), key) == nk_true;
	}

	bool is_key_released(nk_keys key) const
	{
		return nk_input_is_key_released(&get(), key) == nk_true;
	}

	bool is_key_down(nk_keys key) const
	{
		return nk_input_is_key_down(&get(), key) == nk_true;
	}

	      nk_input& get()       { return get_context().input; }
	const nk_input& get() const { return get_context().input; }

	/// @}
};

/**
 * @brief Grouping API
 * @details Groups are basically windows inside windows. They allow to subdivide
 * space in a window to layout widgets as a group. Almost all more complex widget
 * layouting requirements can be solved using groups and basic layouting
 * functionality. Groups just like windows are identified by a unique name and
 * internally keep track of scrollbar offsets by default. However additional
 * versions are provided to directly manage the scrollbar.
 *
 * Nesting groups is possible and even encouraged since many layouting schemes
 * can only be achieved by nesting.
 */
class group : public scope_guard
{
public:
	using scope_guard::scope_guard;

	/**
	 * @name Group creation
	 * @{
	 */

	/**
	 * @copydoc layout::group_blank
	 */
	NUKLEUS_NODISCARD group subgroup_blank()
	{
		return group(get_context(), nullptr);
	}

	/**
	 * @copydoc layout::group_scoped
	 */
	NUKLEUS_NODISCARD group subgroup_scoped(const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin(&get_context(), title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @copydoc layout::group_titled_scoped
	 */
	NUKLEUS_NODISCARD group subgroup_titled_scoped(const char* name, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin_titled(&get_context(), name, title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @copydoc layout::group_scrolled_offset_scoped
	 */
	NUKLEUS_NODISCARD group subgroup_scrolled_offset_scoped(uint& x_offset, uint& y_offset, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_scrolled_offset_begin(
				&get_context(), &x_offset, &y_offset, title, flags
			) == nk_true ? &nk_group_scrolled_end : nullptr);
	}

	/**
	 * @copydoc layout::group_scrolled_scoped
	 */
	NUKLEUS_NODISCARD group subgroup_scrolled_scoped(nk_scroll& off, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_scrolled_begin(
				&get_context(), &off, title, flags
			) == nk_true ? &nk_group_scrolled_end : nullptr);
	}

	/// @}
};

/**
 * @brief Base class for layouts. Specific layouts (derived types) offer additional functions.
 * @details See @ref window layout documentation for description and examples
 */
class layout : public simple_scope_guard
{
public:
	using simple_scope_guard::simple_scope_guard;

	/**
	 * @name Group API
	 * Make a group. All groups requires a previous layouting function to specify a pos/size.
	 * @{
	 */

	/**
	 * @brief Create a group scope guard to gain access to group's API.
	 * @return A group guard that does not manage a group.
	 * @details Normally you should not need to use this function.
	 */
	group group_blank()
	{
		return group(get_context(), nullptr);
	}

	/**
	 * @brief Start a new widget group. Requires a previous layouting function to specify a pos/size.
	 * @param title Unique group title used to both identify and display in the group header.
	 * @param flags Window flags defined in the nk_panel_flags section with a number of different group behaviors.
	 * @return scope guard that should be immediately checked
	 */
	group group_scoped(const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin(&get_context(), title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @copybrief layout::group_scoped
	 * @param name Unique identifier for this group.
	 * @param title Group header title.
	 * @param flags Window flags defined in the nk_panel_flags section with a number of different group behaviors.
	 * @return scope guard that should be immediately checked
	 */
	group group_titled_scoped(const char* name, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin_titled(&get_context(), name, title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @brief Start a new widget group. Requires a previous layouting function to specify
	 * a size. Does not keep track of scrollbar.
	 * @param x_offset Scrollbar x-offset to offset all widgets inside the group horizontally.
	 * @param y_offset Scrollbar y-offset to offset all widgets inside the group vertically.
	 * @param title Unique group title used to both identify and display in the group header.
	 * @param flags Window flags from the nk_panel_flags section
	 * @return scope guard that should be immediately checked
	 */
	group group_scrolled_offset_scoped(uint& x_offset, uint& y_offset, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_scrolled_offset_begin(
				&get_context(), &x_offset, &y_offset, title, flags
			) == nk_true ? &nk_group_scrolled_end : nullptr);
	}

	/**
	 * @brief Start a new widget group. Requires a previous
	 * layouting function to specify a size. Does not keep track of scrollbar.
	 * @param off Both x- and y- scroll offset. Allows for manual scrollbar control.
	 * @param title Unique group title used to both identify and display in the group header.
	 * @param flags Window flags from the nk_panel_flags section
	 * @return scope guard that should be immediately checked
	 */
	group group_scrolled_scoped(nk_scroll& off, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_scrolled_begin(
				&get_context(), &off, title, flags
			) == nk_true ? &nk_group_scrolled_end : nullptr);
	}

	/// @}
};

class layout_row : public layout
{
public:
	explicit layout_row(nk_context& ctx)
	: layout(ctx, &nk_layout_row_end)
	{}

	/**
	 * @brief Specifies either window ratio or width of a single column.
	 * @param value Either a window ratio or fixed width depending on `fmt` in previous row_scoped call.
	 */
	void push(float value) &
	{
		nk_layout_row_push(&get_context(), value);
	}
};

class layout_row_template : public layout
{
public:
	explicit layout_row_template(nk_context& ctx)
	: layout(ctx, &nk_layout_row_template_end)
	{}

	/**
	 * @brief Add a dynamic column that dynamically grows and can go to zero if not enough space.
	 */
	void push_dynamic() &
	{
		nk_layout_row_template_push_dynamic(&get_context());
	}

	/**
	 * @brief Add a variable column that dynamically grows but does not shrink below specified pixel width.
	 * @param min_width Minimum pixel width the next column must always be.
	 */
	void push_variable(float min_width) &
	{
		nk_layout_row_template_push_variable(&get_context(), min_width);
	}

	/**
	 * @brief Add a static column that does not grow and will always have the same size.
	 * @param width Absolute pixel width value the next column must be.
	 */
	void push_static(float width) &
	{
		nk_layout_row_template_push_static(&get_context(), width);
	}
};

class layout_space : public layout
{
public:
	explicit layout_space(nk_context& ctx)
	: layout(ctx, &nk_layout_space_end)
	{}

	/**
	 * @brief Push position and size of the next widget in own coordinate space either as pixel or ratio.
	 * @param bounds Position and size in laoyut space local coordinates.
	 */
	void push(rect<float> bounds) &
	{
		nk_layout_space_push(&get_context(), bounds);
	}

	/**
	 * @brief Utility function to calculate total space allocated for `nk_layout_space`.
	 * @return Total space allocated.
	 */
	NUKLEUS_NODISCARD rect<float> bounds() const
	{
		return nk_layout_space_bounds(&get_context());
	}

	/**
	 * @brief Convert vector from nk_layout_space coordinate space into screen space.
	 * @param vec Position to convert from layout space into screen coordinate space.
	 * @return Screen space coordinates.
	 */
	NUKLEUS_NODISCARD vec2<float> to_screen(vec2<float> vec) const
	{
		return nk_layout_space_to_screen(&get_context(), vec);
	}

	/**
	 * @brief Convert vector from screen space into layout space.
	 * @param vec Position to convert from screen space into layout coordinate space.
	 * @return Layout space coordinates.
	 */
	NUKLEUS_NODISCARD vec2<float> to_local(vec2<float> vec) const
	{
		return nk_layout_space_to_local(&get_context(), vec);
	}

	/**
	 * @brief Convert rectangle from layout space into screen space.
	 * @param bounds Rectangle to convert from layout space into screen space.
	 * @return Screen space coordinates.
	 */
	NUKLEUS_NODISCARD rect<float> rect_to_screen(rect<float> bounds) const
	{
		return nk_layout_space_rect_to_screen(&get_context(), bounds);
	}

	/**
	 * @brief Convert rectangle from screen space into layout space.
	 * @param bounds Rectangle to convert from screen space into layout space.
	 * @return Layout space coordinates.
	 */
	NUKLEUS_NODISCARD rect<float> rect_to_local(rect<float> bounds) const
	{
		return nk_layout_space_rect_to_local(&get_context(), bounds);
	}

	/**
	 * @brief Spacer is a dummy widget that consumes space as usual but doesn't draw anything.
	 */
	void spacer()
	{
		nk_spacer(&get_context());
	}
};

// An alias in case tree gets some functions in the future
using tree = scope_guard;

/**
 * @brief List View scope guard (quite special)
 * @details This guard is quite special because the list view has rather unique API:
 * - `nk_list_view_begin` has an output `nk_list_view` struct parameter
 * - `nk_list_view_end` requires this struct, not context
 */
class list_view
{
public:
	list_view() = default;

	/* implicit */ list_view(nk_list_view lview, bool valid = false)
	: m_list_view(lview)
	, m_valid(valid)
	{}

	list_view(list_view&& other) noexcept
	: m_list_view(exchange(other.m_list_view, {}))
	, m_valid(exchange(other.m_valid, false))
	{}

	list_view(const list_view&) = delete;
	list_view& operator=(const list_view&) = delete;
	list_view& operator=(list_view&&) noexcept = delete;

	~list_view()
	{
		reset();
	}

	void reset()
	{
		if (m_valid)
		{
			nk_list_view_end(&get());
			m_valid = false;
		}
	}

	/**
	 * @brief Get the active state of this guard.
	 * @return `true` if an action has begun (but not ended), `false` otherwise.
	 */
	bool is_scope_active() const noexcept
	{
		return m_valid;
	}

	explicit operator bool() && noexcept = delete;
	/**
	 * @brief Return whether this scope-widget should be processed.
	 * @return `true` if visible and fillable with widgets, otherwise `false`.
	 */
	explicit operator bool() const & noexcept
	{
		return is_scope_active();
	}

	int begin() const { return m_list_view.begin; }
	int end()   const { return m_list_view.end; }
	int count() const { return m_list_view.count; }

	void begin(int value) { m_list_view.begin = value; }
	void end  (int value) { m_list_view.end = value; }
	void count(int value) { m_list_view.count = value; }

	const nk_list_view& get() const { return m_list_view; }
	      nk_list_view& get()       { return m_list_view; }

private:
	nk_list_view m_list_view = {};
	bool m_valid = false;
};

/**
 * @brief UNDOCUMENTED
 */
class chart : public scope_guard
{
public:
	using scope_guard::scope_guard;

	void add_slot(nk_chart_type type, int count, float min_value, float max_value)
	{
		nk_chart_add_slot(&get_context(), type, count, min_value, max_value);
	}

	void add_slot_colored(nk_chart_type type, color col, color highlight, int count, float min_value, float max_value)
	{
		nk_chart_add_slot_colored(&get_context(), type, col, highlight, count, min_value, max_value);
	}

	NUKLEUS_NODISCARD nk_flags push(float value)
	{
		return nk_chart_push(&get_context(), value);
	}

	NUKLEUS_NODISCARD nk_flags push_slot(float value, int slot)
	{
		return nk_chart_push_slot(&get_context(), value, slot);
	}
};

class popup : public scope_guard
{
public:
	using scope_guard::scope_guard;

	void close()
	{
		nk_popup_close(&get_context());
	}

	/**
	 * @brief get scroll offsets
	 * @param offset_x (optional) variable to save X offset to
	 * @param offset_y (optional) variable to save Y offset to
	 */
	void get_scroll(nk_uint* offset_x, nk_uint* offset_y) const
	{
		nk_popup_get_scroll(&get_context(), offset_x, offset_y);
	}

	NUKLEUS_NODISCARD vec2<nk_uint> get_scroll() const
	{
		vec2<nk_uint> result;
		nk_popup_get_scroll(&get_context(), &result.x, &result.y);
		return result;
	}

	void set_scroll(nk_uint offset_x, nk_uint offset_y)
	{
		nk_popup_set_scroll(&get_context(), offset_x, offset_y);
	}

	void set_scroll(vec2<nk_uint> offsets)
	{
		set_scroll(offsets.x, offsets.y);
	}
};

class combobox : public scope_guard
{
public:
	using scope_guard::scope_guard;

	NUKLEUS_NODISCARD bool item_label(const char* label, nk_flags alignment)
	{
		return nk_combo_item_label(&get_context(), label, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_text(const char* text, int len, nk_flags alignment)
	{
		return nk_combo_item_text(&get_context(), text, len, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_image_label(image img, const char* text, nk_flags alignment)
	{
		return nk_combo_item_image_label(&get_context(), img, text, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_image_text(image img, const char* text, int len, nk_flags alignment)
	{
		return nk_combo_item_image_text(&get_context(), img, text, len, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_symbol_label(nk_symbol_type symbol, const char* label, nk_flags alignment)
	{
		return nk_combo_item_symbol_label(&get_context(), symbol, label, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_symbol_text(nk_symbol_type symbol, const char* text, int len, nk_flags alignment)
	{
		return nk_combo_item_symbol_text(&get_context(), symbol, text, len, alignment) == nk_true;
	}

	void close()
	{
		nk_combo_close(&get_context());
	}
};

/**
 * @brief contextual widget scope guard
 */
class contextual : public scope_guard
{
public:
	using scope_guard::scope_guard;

	NUKLEUS_NODISCARD bool item_text(const char* text, int len, nk_flags align)
	{
		return nk_contextual_item_text(&get_context(), text, len, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_label(const char* label, nk_flags align)
	{
		return nk_contextual_item_label(&get_context(), label, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_image_label(image img, const char* label, nk_flags alignment)
	{
		return nk_contextual_item_image_label(&get_context(), img, label, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_image_text(image img, const char* text, int len, nk_flags alignment)
	{
		return nk_contextual_item_image_text(&get_context(), img, text, len, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_symbol_label(nk_symbol_type symbol, const char* label, nk_flags alignment)
	{
		return nk_contextual_item_symbol_label(&get_context(), symbol, label, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_symbol_text(nk_symbol_type symbol, const char* text, int len, nk_flags alignment)
	{
		return nk_contextual_item_symbol_text(&get_context(), symbol, text, len, alignment) == nk_true;
	}

	void close()
	{
		nk_contextual_close(&get_context());
	}
};

/**
 * @brief menu widget scope guard
 */
class menu : public scope_guard
{
public:
	using scope_guard::scope_guard;

	NUKLEUS_NODISCARD bool item_text(const char* text, int len, nk_flags alignment)
	{
		return nk_menu_item_text(&get_context(), text, len, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_label(const char* label, nk_flags alignment)
	{
		return nk_menu_item_label(&get_context(), label, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_image_label(image img, const char* label, nk_flags alignment)
	{
		return nk_menu_item_image_label(&get_context(), img, label, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_image_text(image img, const char* text, int len, nk_flags alignment)
	{
		return nk_menu_item_image_text(&get_context(), img, text, len, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_symbol_text(nk_symbol_type symbol, const char* text, int len, nk_flags alignment)
	{
		return nk_menu_item_symbol_text(&get_context(), symbol, text, len, alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool item_symbol_label(nk_symbol_type symbol, const char* label, nk_flags alignment)
	{
		return nk_menu_item_symbol_label(&get_context(), symbol, label, alignment) == nk_true;
	}

	void close()
	{
		nk_menu_close(&get_context());
	}
};

/**
 * @brief Drawing Canvas (non-owning wrapper of `nk_command_buffer`)
 * @details This library was designed to be render backend agnostic so it does
 * not draw anything to screen. Instead all drawn shapes, widgets
 * are made of, are buffered into memory and make up a command queue.
 * Each frame therefore fills the command buffer with draw commands
 * that then need to be executed by the user and his own render backend.
 * After that the command buffer needs to be cleared and a new frame can be
 * started. It is probably important to note that the command buffer is the main
 * drawing API and the optional vertex buffer API only takes this format and
 * converts it into a hardware accessible format.
 *
 * To use the command queue to draw your own widgets you can access the
 * command buffer of each window by calling @ref window::get_canvas.
 *
 * ```cpp
 * void draw_red_rectangle_widget(nk::window& window)
 * {
 *     nk::rect<float> space;
 *     const nk::widget_layout_states state = window.widget(space);
 *
 *     if (state == NK_WIDGET_INVALID)
 *         return;
 *
 *     if (state != NK_WIDGET_ROM)
 *         update_your_widget_by_user_input(...);
 *
 *     auto canvas = window.get_canvas();
 *     canvas.fill_rect(space, 0, nk::color(255, 0, 0));
 * }
 *
 * if (auto window = ctx.xxx_scoped(...); window) {
 *     window.row_dynamic(25, 1);
 *     draw_red_rectangle_widget(window);
 * }
 * ```
 * Important to know if you want to create your own widgets is the @ref window::widget call.
 * It allocates space on the panel reserved for the widget to be used.
 * See function documentation for more details.
 */
class canvas
{
public:
	/* implicit */ canvas(nk_command_buffer& cmd_buf)
	: m_cmd_buf(&cmd_buf)
	{}

	      nk_command_buffer& get()       { return *m_cmd_buf; }
	const nk_command_buffer& get() const { return *m_cmd_buf; }

	/**
	 * @name Shape Outlines
	 * @{
	 */

	void stroke_line(float x0, float y0, float x1, float y1, float line_thickness, color c)
	{
		nk_stroke_line(m_cmd_buf, x0, y0, x1, y1, line_thickness, c);
	}

	void stroke_curve(float ax, float ay, float ctrl0x, float ctrl0y, float ctrl1x, float ctrl1y, float bx, float by, float line_thickness, color c)
	{
		nk_stroke_curve(m_cmd_buf, ax, ay, ctrl0x, ctrl0y, ctrl1x, ctrl1y, bx, by, line_thickness, c);
	}

	void stroke_rect(rect<float> r, float rounding, float line_thickness, color c)
	{
		nk_stroke_rect(m_cmd_buf, r, rounding, line_thickness, c);
	}

	void stroke_circle(rect<float> r, float line_thickness, color c)
	{
		nk_stroke_circle(m_cmd_buf, r, line_thickness, c);
	}

	void stroke_arc(float cx, float cy, float radius, float a_min, float a_max, float line_thickness, color c)
	{
		nk_stroke_arc(m_cmd_buf, cx, cy, radius, a_min, a_max, line_thickness, c);
	}

	void stroke_triangle(float x0, float y0, float x1, float y1, float x2, float y2, float line_thickness, color c)
	{
		nk_stroke_triangle(m_cmd_buf, x0, y0, x1, y1, x2, y2, line_thickness, c);
	}

	void stroke_polyline(const float* points, int point_count, float line_thickness, color c)
	{
		nk_stroke_polyline(m_cmd_buf, points, point_count, line_thickness, c);
	}

	void stroke_polygon(const float* points, int point_count, float line_thickness, color c)
	{
		nk_stroke_polygon(m_cmd_buf, points, point_count, line_thickness, c);
	}

	/// @}

	/**
	 * @name Filled Shapes
	 * @{
	 */

	void fill_rect(rect<float> r, float rounding, color c)
	{
		nk_fill_rect(m_cmd_buf, r, rounding, c);
	}

	void fill_rect_multi_color(rect<float> r, color left, color top, color right, color bottom)
	{
		nk_fill_rect_multi_color(m_cmd_buf, r, left, top, right, bottom);
	}

	void fill_circle(rect<float> r, color c)
	{
		nk_fill_circle(m_cmd_buf, r, c);
	}

	void fill_arc(float cx, float cy, float radius, float a_min, float a_max, color c)
	{
		nk_fill_arc(m_cmd_buf, cx, cy, radius, a_min, a_max, c);
	}

	void fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, color c)
	{
		nk_fill_triangle(m_cmd_buf, x0, y0, x1, y1, x2, y2, c);
	}

	void fill_polygon(const float* points, int point_count, color c)
	{
		nk_fill_polygon(m_cmd_buf, points, point_count, c);
	}

	/// @}

	/**
	 * @name Other
	 * @{
	 */

	void draw_image(rect<float> r, const image& img, color c)
	{
		nk_draw_image(m_cmd_buf, r, &img, c);
	}

	void draw_nine_slice(rect<float> r, const nk_nine_slice& slc, color c)
	{
		nk_draw_nine_slice(m_cmd_buf, r, &slc, c);
	}

	void draw_text(rect<float> r, const char* text, int len, const nk_user_font& font, color bg, color fg)
	{
		nk_draw_text(m_cmd_buf, r, text, len, &font, bg, fg);
	}

	void push_scissor(rect<float> r)
	{
		nk_push_scissor(m_cmd_buf, r);
	}

	// Can not provide better abstraction here - the callback is not used immediately
	void push_custom(rect<float> r, nk_command_custom_callback cb, handle usr)
	{
		nk_push_custom(m_cmd_buf, r, cb, usr);
	}

	/// @}

private:
	nk_command_buffer* m_cmd_buf;
};

/**
 * @brief Window
 * Windows are the main persistent state used inside nuklear and are life time
 * controlled by simply "retouching" (i.e. calling) each window each frame.
 * All widgets inside nuklear can only be added inside the function pair `nk_begin_xxx`
 * and `nk_end`. Calling any widgets outside these two functions will result in an
 * assert in debug or no state change in release mode.
 *
 * Each window holds frame persistent state like position, size, flags, state tables,
 * and some garbage collected internal persistent widget state. Each window
 * is linked into a window stack list which determines the drawing and overlapping
 * order. The topmost window thereby is the currently active window.
 *
 * To change window position inside the stack occurs either automatically by user input
 * by being clicked on or programmatically by calling @ref window::window_set_focus.
 * Windows by default are visible unless explicitly being defined with flag
 * `NK_WINDOW_HIDDEN`, the user clicked the close button on windows with flag
 * `NK_WINDOW_CLOSABLE` or if a window was explicitly hidden by calling
 * `nk_window_show`. To explicitly close and destroy a window call `nk_window_close`.
 *
 * **Usage:**
 *
 * To create and keep a window you have to call one of the two `nk_begin_xxx`
 * functions to start window declarations and `nk_end` at the end. Furthermore it
 * is recommended to check the return value of `nk_begin_xxx` and only process
 * widgets inside the window if the value is not 0. Either way you have to call
 * `nk_end` at the end of window declarations. Furthermore, do not attempt to
 * nest `nk_begin_xxx` calls which will hopefully result in an assert or if not
 * in a segmentation fault.
 */
class window : public simple_scope_guard
{
private:
	window(nk_context& ctx, func_type* func, bool valid)
	: simple_scope_guard(ctx, func)
	, m_valid(valid)
	{}

public:
	/**
	 * @brief create a window object
	 * @param ctx `nk_context` reference
	 * @param valid whether the instance manages a window
	 * @return window object
	 * @details Normally you should not need to use this function. See @ref context::window_scoped.
	 */
	NUKLEUS_NODISCARD static window create(nk_context& ctx, bool valid)
	{
		return window(ctx, &nk_end, valid);
	}

	/**
	 * @brief create a window object that does not manage any window
	 * @param ctx `nk_context` reference
	 * @return empty window object
	 * @details Normally you should not need to use this function. @ref context::window_blank.
	 */
	NUKLEUS_NODISCARD static window create_blank(nk_context& ctx)
	{
		return window(ctx, nullptr, false);
	}

	window(window&& other) noexcept
	: simple_scope_guard(move(other))
	, m_valid(exchange(other.m_valid, false))
	{}

	window(const window&) = delete;
	window& operator=(const window&) = delete;
	window& operator=(window&&) noexcept = delete;

	explicit operator bool() && noexcept = delete;
	/**
	 * @brief Return whether this window should be processed.
	 * @return `true` if `nk_begin_xxx` returned `true` and this object manages window lifetime.
	 * @details Do not confuse with is_scope_active which checks only for lifetime.
	 * A window always needs an end call (`nk_end`) but if `begin` returned `false`
	 * then the window should not be processed.
	 */
	explicit operator bool() const & noexcept
	{
		return m_valid && is_scope_active();
	}

	/**
	 * @name Global window functions
	 * These functions apply to the window of specified name.
	 * They are also available in the context class.
	 * @{
	 */

	/**
	 * @brief Find and return a window from passed name.
	 * @param name Window identifier.
	 * @return Window pointer or null pointer.
	 */
	NUKLEUS_NODISCARD nk_window* window_find(const char* name) const
	{
		return nk_window_find(&get_context(), name);
	}

	/**
	 * @brief Return whether the window with given name is currently minimized/collapsed.
	 * @param name Window identifier.
	 * @return `true` if the window is minimized and `false` if not minimized or not found.
	 */
	NUKLEUS_NODISCARD bool window_is_collapsed(const char* name) const
	{
		return nk_window_is_collapsed(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Return whether the window with given name was closed by calling `nk_close`.
	 * @param name Window identifier.
	 * @return `true` if the window was closed and `false` if not closed or not found.
	 */
	NUKLEUS_NODISCARD bool window_is_closed(const char* name) const
	{
		return nk_window_is_closed(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Return whether the window with given name is hidden.
	 * @param name Window identifier.
	 * @return `true` if the window is hidden and `false` if not hidden or not found.
	 */
	NUKLEUS_NODISCARD bool window_is_hidden(const char* name) const
	{
		return nk_window_is_hidden(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Same as `window_has_focus` for some reason.
	 * @param name Window identifier.
	 * @return `true` if the window is active and `false` if not active or not found.
	 */
	NUKLEUS_NODISCARD bool window_is_active(const char* name) const
	{
		return nk_window_is_active(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Return whether any window is being hovered.
	 * @return `true` if any window is hovered or `false` otherwise.
	 */
	NUKLEUS_NODISCARD bool window_is_any_hovered() const
	{
		return nk_window_is_any_hovered(&get_context()) == nk_true;
	}

	/**
	 * @brief Returns if the any window is being hovered or any widget is currently active.
	 * Can be used to decide if input should be processed by UI or your specific input handling.
	 * Example could be UI and 3D camera to move inside a 3D space.
	 * @return `true` if any window is active or `false` otherwise.
	 */
	NUKLEUS_NODISCARD bool item_is_any_active() const
	{
		return nk_item_is_any_active(&get_context()) == nk_true;
	}

	/**
	 * @brief Update position and size of window with passed in name.
	 * @param name Window identifier.
	 * @param bounds New position and size.
	 */
	void window_set_bounds(const char* name, rect<float> bounds)
	{
		nk_window_set_bounds(&get_context(), name, bounds);
	}

	/**
	 * @brief Update position of window with passed in name.
	 * @param name Window identifier.
	 * @param pos New position.
	 */
	void window_set_position(const char* name, vec2<float> pos)
	{
		nk_window_set_position(&get_context(), name, pos);
	}

	/**
	 * @brief Update size of window with passed in name.
	 * @param name Window identifier.
	 * @param size New size.
	 */
	void window_set_size(const char* name, vec2<float> size)
	{
		nk_window_set_size(&get_context(), name, size);
	}

	/**
	 * @brief Set the window with given name as active.
	 * @param name Window identifier.
	 */
	void window_set_focus(const char* name)
	{
		nk_window_set_focus(&get_context(), name);
	}

	/**
	 * @brief Close a window and mark it for being freed at the end of the frame.
	 * @param name Window identifier.
	 */
	void window_close(const char* name)
	{
		nk_window_close(&get_context(), name);
	}

	/**
	 * @brief Update collapse state of a window with given name.
	 * @param name Window identifier.
	 * @param state One of collapse state values.
	 */
	void window_collapse(const char* name, nk_collapse_states state)
	{
		nk_window_collapse(&get_context(), name, state);
	}

	/**
	 * @brief Update collapse state of a window with given name if given condition is met.
	 * @param name Window identifier.
	 * @param state One of collapse state values.
	 * @param cond If 0, function has no effect.
	 */
	void window_collapse_if(const char* name, nk_collapse_states state, int cond)
	{
		nk_window_collapse_if(&get_context(), name, state, cond);
	}

	/**
	 * @brief Update visibility state of a window with given name.
	 * @param name Window identifier.
	 * @param state New state to apply.
	 */
	void window_show(const char* name, nk_show_states state)
	{
		nk_window_show(&get_context(), name, state);
	}

	/**
	 * @brief Update visibility state of a window with given name if given condition is met.
	 * @param name Window identifier.
	 * @param state New state to apply.
	 * @param cond If 0, function has no effect.
	 */
	void window_show_if(const char* name, nk_show_states state, int cond)
	{
		nk_window_show_if(&get_context(), name, state, cond);
	}

	/// @}

	/**
	 * @name Local window functions
	 * These functions apply to the currently processed window.
	 * The window must be alive (`if (window)`) in order to call these functions.
	 * @{
	 */

	/**
	 * @brief Return a rectangle with screen position and size of the currently processed window.
	 * @return Window upper left window position and size.
	 */
	NUKLEUS_NODISCARD rect<float> get_bounds() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_bounds(&get_context());
	}

	/**
	 * @brief Return the position of the currently processed window.
	 * @return Window upper left position.
	 */
	NUKLEUS_NODISCARD vec2<float> get_position() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_position(&get_context());
	}

	/**
	 * @brief Return the size with width and height of the currently processed window.
	 * @return Window width and height.
	 */
	NUKLEUS_NODISCARD vec2<float> get_size() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_size(&get_context());
	}

	/**
	 * @brief Return the width of the currently processed window.
	 * @return Current window width.
	 */
	NUKLEUS_NODISCARD float get_width() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_width(&get_context());
	}

	/**
	 * @brief Return the height of the currently processed window.
	 * @return Current window height.
	 */
	NUKLEUS_NODISCARD float get_height() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_height(&get_context());
	}

	/**
	 * @brief Return the underlying panel which contains all processing state of the current window.
	 * @return Pointer to window internal `nk_panel` state.
	 * @attention Do not keep the returned object around, it is only valid until end of window's scope.
	 */
	NUKLEUS_NODISCARD nk_panel& get_panel() &
	{
		NUKLEUS_ASSERT(m_valid);
		// takes const nk_context* but returns non-const nk_panel*
		// This looks like an attempt to supply both const and non-const "overloads"
		return *nk_window_get_panel(&get_context());
	}

	/**
	 * @copydoc get_panel() &
	 */
	NUKLEUS_NODISCARD const nk_panel& get_panel() const &
	{
		NUKLEUS_ASSERT(m_valid);
		return *nk_window_get_panel(&get_context());
	}

	/**
	 * @brief Return the position and size of the currently visible and non-clipped space inside the currently processed window.
	 * @return Screen position and size (no scrollbar offset) of the visible space inside the current window.
	 */
	NUKLEUS_NODISCARD rect<float> get_content_region() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_content_region(&get_context());
	}

	/**
	 * @brief Return the upper left position of the currently visible and non-clipped space inside the currently processed window.
	 * @return Upper left screen position (no scrollbar offset) of the visible space inside the current window.
	 */
	NUKLEUS_NODISCARD vec2<float> get_content_region_min() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_content_region_min(&get_context());
	}

	/**
	 * @brief Returns the lower right screen position of the currently visible and non-clipped space inside the currently processed window.
	 * @return Lower right screen position (no scrollbar offset) of the visible space inside the current window
	 */
	NUKLEUS_NODISCARD vec2<float> get_content_region_max() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_content_region_max(&get_context());
	}

	/**
	 * @brief Return the size of the currently visible and non-clipped space inside the currently processed window.
	 * @return Size of the visible space inside the current window.
	 */
	NUKLEUS_NODISCARD vec2<float> get_content_region_size() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_get_content_region_size(&get_context());
	}

	/**
	 * @brief Return window's canvas, which can be used to draw custom widgets.
	 * @return Canvas object made from pointer to window's internal `nk_command_buffer`.
	 * @attention Do not keep the returned object between frames, it is only valid until end of window's scope.
	 */
	NUKLEUS_NODISCARD canvas get_canvas() &
	{
		NUKLEUS_ASSERT(m_valid);
		return *nk_window_get_canvas(&get_context());
	}

	/**
	 * @brief Get the scroll offset for the current window.
	 * @param offset_x x offset output (use nullptr to ignore)
	 * @param offset_y y offset output (use nullptr to ignore)
	 */
	void get_scroll(nk_uint* offset_x, nk_uint* offset_y) const
	{
		NUKLEUS_ASSERT(m_valid);
		nk_window_get_scroll(&get_context(), offset_x, offset_y);
	}

	/**
	 * @brief Get the scroll offset for the current window.
	 * @return Scroll offset for the current window.
	 */
	NUKLEUS_NODISCARD vec2<nk_uint> get_scroll() const
	{
		NUKLEUS_ASSERT(m_valid);
		nk_uint x = 0;
		nk_uint y = 0;
		nk_window_get_scroll(&get_context(), &x, &y);
		return {x, y};
	}

	/**
	 * @brief Sets the scroll offset for the current window.
	 * @param offset_x The x offset to scroll to.
	 * @param offset_y The y offset to scroll to.
	 */
	void set_scroll(nk_uint offset_x, nk_uint offset_y)
	{
		nk_window_set_scroll(&get_context(), offset_x, offset_y);
	}

	/**
	 * @brief Sets the scroll offset for the current window.
	 * @param offsets The offsets to scroll to.
	 */
	void set_scroll(vec2<nk_uint> offsets)
	{
		nk_window_set_scroll(&get_context(), offsets.x, offsets.y);
	}

	/**
	 * @brief Return whether the currently processed window is currently active.
	 * @return `true` if active.
	 */
	NUKLEUS_NODISCARD bool has_focus() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_has_focus(&get_context()) == nk_true;
	}

	/**
	 * @brief Return whether the currently processed window is being hovered.
	 * @return `true` if hovered.
	 */
	NUKLEUS_NODISCARD bool is_hovered() const
	{
		NUKLEUS_ASSERT(m_valid);
		return nk_window_is_hovered(&get_context()) == nk_true;
	}

	/**
	 * @brief Line for visual separation. Draws a line with thickness determined by the current row height.
	 * @param color Color of the horizontal line.
	 * @param rounding Whether or not to make the line round.
	 */
	void rule_horizontal(nk_color color, bool rounding)
	{
		nk_rule_horizontal(&get_context(), color, rounding);
	}

	/// @}

	/**
	 * @name Layout
	 * Layouting in general describes placing widget inside a window with position and size.
	 * There are many different APIs for layouting and
	 * each has a different trade offs between control and ease of use.
	 *
	 * All layouting methods in this library are based around the concept of a row.
	 * A row has a height the window content grows by and a number of columns and each
	 * layouting method specifies how each widget is placed inside the row.
	 * After a row has been allocated by calling a layouting functions and then
	 * filled with widgets will advance an internal pointer over the allocated row.
	 *
	 * To actually define a layout you just call the appropriate layouting function
	 * and each subsequent widget call will place the widget as specified. Important here
	 * is that if you define more widgets than columns defined inside the layout functions
	 * it will allocate the next row without you having to make another layouting call.
	 * This behavior (automatic repeat) is supported by most layout functions.
	 *
	 * Biggest limitation with using all these APIs outside the `layout_space_xxx` API
	 * is that you have to define the row height for each. However the row height
	 * often depends on the height of the font.
	 *
	 * To fix that internally nuklear uses a minimum row height that is set to the
	 * height plus padding of currently active font and overwrites the row height
	 * value if zero.
	 *
	 * If you manually want to change the minimum row height then
	 * use @ref layout_set_min_row_height, and use @ref layout_reset_min_row_height to
	 * reset it back to be derived from font height.
	 *
	 * Also if you change the font in nuklear it will automatically change the minimum
	 * row height for you and. This means if you change the font but still want
	 * a minimum row height smaller than the font you have to repush your value.
	 *
	 * For actually more advanced UI I would even recommend using the `layout_space_xxx`
	 * layouting method in combination with a cassowary constraint solver (there are
	 * some versions on github with permissive license model) to take over all control over widget
	 * layouting yourself. However for quick and dirty layouting using all the other layouting
	 * functions should be fine.
	 *
	 * @parblock
	 * @note Some layouts are automatically repeating. This means that after
	 * a row got the specified amount of widgets (columns), the layout will
	 * automatically create a new row with identical parameters. For non-repeating
	 * layout functions you need to call them again before each new row.
	 * @endparblock
	 *
	 * @parblock
	 * @note Capturing layout object created by non-scoped layouts is not required.
	 * These functions have no `[[nodiscard]]` and holding their returned object
	 * is only useful if you want to call layout-specific functions (e.g. groups).
	 * @endparblock
	 *
	 * @parblock
	 * @note If you want to put multiple widgets into the same column use grouping
	 * API to bundle these widgets into one widget (the group). Groups also allow
	 * you to nest different layouts inside of them.
	 * @endparblock
	 *
	 * Usage:
	 *
	 * 1. **Uniform static/dynamic**: each column with identical configuration.
	 *    Either static (fixed pixel width) or dynamic (space equally distributed).
	 *
	 *    This layout is automatically repeating.
	 *    ```cpp
	 *    // first row with height 30, composed of two widgets
	 *    win.layout_row_dynamic(30, 2);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    // second row with same parameters as defined above
	 *    win.widget(...);
	 *    win.widget(...);
	 *    // third row uses 0 for height which will use auto layouting
	 *    win.layout_row_dynamic(0, 2);
	 *    win.widget(...);
	 *    win.widget(...);
	 *
	 *    // first row with height 30, composed of two widgets with width 80
	 *    win.layout_row_static(30, 80, 2);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    // second row with same parameters as defined above
	 *    win.widget(...);
	 *    win.widget(...);
	 *    // third row uses 0 for height which will use auto layouting
	 *    win.layout_row_static(0, 80, 2);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    ```
	 * 2. **Array static/dynamic**: like previous, but provide widths explicitly.
	 *    This allows to set different fixed width (for static) or
	 *    space fraction (for dynamic) for each column.
	 *
	 *    This layout is automatically repeating.
	 *    ```cpp
	 *    // two rows with height 30, composed of two widgets with widths 60 and 40
	 *    // NOTE: the {}-overload requires std::initializer_list
	 *    // it is not available when `NUKLEUS_AVOID_STDLIB` is defined
	 *    win.layout_row_static(30, 2, {60, 40});
	 *    win.widget(...);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    // two rows with height 30, composed of two widgets with window ratios 0.25 and 0.75
	 *    win.layout_row_dynamic(30, 2, {0.25, 0.75});
	 *    win.widget(...);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    // two rows with auto generated height composed of two widgets with window ratios 0.25 and 0.75
	 *    win.layout_row_dynamic(0, 2, {0.25, 0.75});
	 *    win.widget(...);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    win.widget(...);
	 *    ```
	 * 3. **Scoped static/dynamic**: like previous, but push width before each widget.
	 *
	 *    This layout is not automatically repeating.
	 *    ```cpp
	 *    // first row with height 25 composed of two widgets with widths 60 and 40
	 *    {
	 *        auto layout_row = win.layout_row_static_scoped(25, 2);
	 *        layout_row.push(60);
	 *        win.widget(...);
	 *        layout_row.push(40);
	 *        win.widget(...);
	 *    }
	 *    // second row with height 25 composed of two widgets with window ratios 0.25 and 0.75
	 *    {
	 *        auto layout_row = win.layout_row_dynamic_scoped(25, 2);
	 *        layout_row.push(0.25f);
	 *        win.widget(...);
	 *        layout_row.push(0.75f);
	 *        win.widget(...);
	 *    }
	 *    // third row with auto generated height, composed of two widgets with window ratios 0.25 and 0.75
	 *    {
	 *        auto layout_row = win.layout_row_dynamic_scoped(0, 2);
	 *        layout_row.push(0.25f);
	 *        win.widget(...);
	 *        layout_row.push(0.75f);
	 *        win.widget(...);
	 *    }
	 *    ```
	 * 4. **Template**: the most complex and second most flexible API is a simplified flexbox version without line
	 *    wrapping and weights for dynamic widgets. It is an immediate mode API but needs to be called before
	 *    creating the widgets. The row template layout has three different per-widget size specifiers:
	 *    - @ref layout_row_template::push_static defines fixed widget pixel width.
	 *    - @ref layout_row_template::push_variable defines a minimum widget size
	 *      (can grow if more space is available and not taken by other widgets).
	 *    - @ref layout_row_template::push_dynamic widgets are completely flexible and unlike variable widgets can
	 *      even shrink to zero if not enough space is provided.
	 *
	 *    This layout is automatically repeating (but note it has to be finished before creating widgets).
	 *    ```cpp
	 *    // two rows with height 30, composed of three widgets
	 *    {
	 *        auto layout_row_template = win.layout_row_template_scoped(30);
	 *        layout_row_template.push_dynamic();
	 *        layout_row_template.push_variable(80);
	 *        layout_row_template.push_static(80);
	 *    }
	 *    // first row
	 *    nk_widget(...); // can go to zero if there is not enough space
	 *    nk_widget(...); // min 80 pixels but can grow bigger if there is enough space
	 *    nk_widget(...); // static widget with fixed 80 pixel width
	 *    // second row same layout
	 *    nk_widget(...);
	 *    nk_widget(...);
	 *    nk_widget(...);
	 *    ```
	 * 5. **Space**: the most flexible API directly allows you to place widgets inside the window. The space layout
	 *    API is an immediate mode API which directly sets position and size of a widget. Position and size hereby
	 *    can be either specified as ratio of allocated space or allocated space local position and pixel size.
	 *    Since this API is quite powerful there are a number of utility functions to get the available space and
	 *    convert between local allocated space and screen space.
	 *
	 *    This layout is not automatically repeating.
	 *    ```cpp
	 *    // static row with height 500
	 *    // (you can omit columns parameter if you don't want to be bothered)
	 *    {
	 *        auto layout_space = win.layout_space_static_scoped(500);
	 *        layout_space.push({0, 0, 150, 200});
	 *        win.widget(...);
	 *        layout_space.push({200, 200, 100, 200});
	 *        win.widget(...);
	 *    }
	 *    // dynamic row with height 500
	 *    {
	 *        auto layout_space = win.layout_space_dynamic_scoped(500);
	 *        layout_space.push({0.5, 0.5, 0.1, 0.1});
	 *        win.widget(...);
	 *        layout_space.push({0.7, 0.6, 0.1, 0.1});
	 *        win.widget(...);
	 *    }
	 *    ```
	 *
	 * @{
	 */

	/**
	 * @brief Create a layout scope guard that does not manage any layout.
	 * @return Blank layout object.
	 * @note Use this function when you want to gain access to layout functions (e.g. groups)
	 * but do not want to change or create a new layout.
	 */
	NUKLEUS_NODISCARD layout layout_blank()
	{
		return layout(get_context(), nullptr);
	}

	/**
	 * @brief Dynamic row - repeating layout of same-sized growing columns.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param cols Number of widgets inside row.
	 * @return Blank layout guard object
	 * @note This layout is autorepeating and does not need cleanup. Therefore you don't need to keep the
	 * result of this function. You may still want to save it to gain access to layout functions (e.g. groups).
	 * @details Sets current row layout to share horizontal space between @p cols number of widgets evenly.
	 * Once called all subsequent widget calls greater than @p cols will allocate a new row with same layout.
	 */
	layout layout_row_dynamic(float height, int cols) &
	{
		nk_layout_row_dynamic(&get_context(), height, cols);
		return layout_blank();
	}

	/**
	 * @brief Static row - repeating layout of fixed-size columns.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param width Pixel width of each widget in the row.
	 * @param cols Number of widgets inside row.
	 * @return Blank layout guard object
	 * @note This layout is autorepeating and does not need cleanup. Therefore you don't need to keep the
	 * result of this function. You may still want to save it to gain access to layout functions (e.g. groups).
	 * @details Sets current row layout to fill @p cols number of widgets
	 * in row with same @p width horizontal size. Once called all subsequent
	 * widget calls greater than @p cols will allocate a new row with same layout.
	 */
	layout layout_row_static(float height, int width, int cols) &
	{
		nk_layout_row_static(&get_context(), height, width, cols);
		return layout_blank();
	}

	/**
	 * @brief Array layout (dynamic) where each widget has its unique size specified in advance.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param ratios array of widget widths, ratio to available space
	 * @return Blank layout guard object
	 * @note This layout is autorepeating and does not need cleanup. Therefore you don't need to keep the
	 * result of this function. You may still want to save it to gain access to layout functions (e.g. groups).
	 */
	layout layout_row_dynamic(float height, span<const float> ratios) &
	{
		nk_layout_row(&get_context(), NK_DYNAMIC, height, ratios.size(), ratios.begin());
		return layout_blank();
	}

	/**
	 * @brief Array layout (static) where each widget has its unique size specified in advance.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param ratios array of widget widths, in pixels (fixed size)
	 * @return Blank layout guard object
	 * @note This layout is autorepeating and does not need cleanup. Therefore you don't need to keep the
	 * result of this function. You may still want to save it to gain access to layout functions (e.g. groups).
	 */
	layout layout_row_static(float height, span<const float> ratios) &
	{
		nk_layout_row(&get_context(), NK_STATIC, height, ratios.size(), ratios.begin());
		return layout_blank();
	}

#ifndef NUKLEUS_AVOID_STDLIB
	/**
	 * @copydoc layout_row_dynamic(float height, span<const float> ratios)&
	 * @note this overload is not available when `NUKLEUS_AVOID_STDLIB` is defined
	 */
	layout layout_row_dynamic(float height, std::initializer_list<float> ratios) &
	{
		// There is no .data() but .begin() is guuaranteed to return const T*
		return layout_row_dynamic(height, {ratios.begin(), static_cast<int>(ratios.size())});
	}

	/**
	 * @copydoc layout_row_static(float height, span<const float> ratios)&
	 * @note this overload is not available when `NUKLEUS_AVOID_STDLIB` is defined
	 */
	layout layout_row_static(float height, std::initializer_list<float> ratios) &
	{
		return layout_row_static(height, {ratios.begin(), static_cast<int>(ratios.size())});
	}
#endif

	/**
	 * @brief Scoped row (dynamic) - scoped layout where each widget can have a unique size.
	 * @param height Height of each widget in row (fraction of available space) or zero for auto layouting.
	 * @param cols Number of widgets inside row.
	 * @return scope guard object for this layout.
	 * @note This layout is not autorepeating.
	 */
	NUKLEUS_NODISCARD layout_row layout_row_dynamic_scoped(float height, int cols) &
	{
		nk_layout_row_begin(&get_context(), NK_DYNAMIC, height, cols);
		return layout_row(get_context());
	}

	/**
	 * @brief Scoped row (static) - scoped layout where each widget can have a unique size.
	 * @param height Height of each widget in row (in pixels) or zero for auto layouting.
	 * @param cols Number of widgets inside row.
	 * @return scope guard object for this layout.
	 * @note This layout is not autorepeating.
	 */
	NUKLEUS_NODISCARD layout_row layout_row_static_scoped(float height, int cols) &
	{
		nk_layout_row_begin(&get_context(), NK_STATIC, height, cols);
		return layout_row(get_context());
	}

	/**
	 * @brief Template row - layout where a set of layout behaviors is specified in advance.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @return scope guard object for this layout.
	 * @details Begins the row template scope.
	 * Each widget can be separately statically or dynamically sized.
	 * This layout must be completed before widgets are added.
	 * Either destroy the layout guard object or call reset before adding widgets.
	 */
	NUKLEUS_NODISCARD layout_row_template layout_row_template_scoped(float height) &
	{
		nk_layout_row_template_begin(&get_context(), height);
		return layout_row_template(get_context());
	}

	/**
	 * @brief Space layout (dynamic) - unconstrained, individual widget placement.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param widget_count Number of widgets inside row.
	 * @return scope guard object for this layout.
	 * @details Begin a new layouting space that allows to specify each widget's position and size.
	 * Allows to directly place widgets inside the window. The space layout API is an immediate mode API which
	 * does not support row auto repeat and directly sets position and size of a widget. Position and size
	 * hereby are specified as a ratio of allocated space.
	 * Since this API is quite powerful there are a number of utility functions to get the available space and
	 * convert between local allocated space and screen space.
	 */
	NUKLEUS_NODISCARD layout_space layout_space_dynamic_scoped(float height, int widget_count = max_signed_value<int>()) &
	{
		nk_layout_space_begin(&get_context(), NK_DYNAMIC, height, widget_count);
		return layout_space(get_context());
	}

	/**
	 * @brief Space layout (static) - unconstrained, individual widget placement.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param widget_count Number of widgets inside row.
	 * @return scope guard object for this layout.
	 * @details Begin a new layouting space that allows to specify each widget's position and size.
	 * Allows to directly place widgets inside the window. The space layout API is an immediate mode API which
	 * does not support row auto repeat and directly sets position and size of a widget. Position and size
	 * hereby are specified as allocated space local position and pixel size.
	 * Since this API is quite powerful there are a number of utility functions to get the available space and
	 * convert between local allocated space and screen space.
	 */
	NUKLEUS_NODISCARD layout_space layout_space_static_scoped(float height, int widget_count = max_signed_value<int>()) &
	{
		nk_layout_space_begin(&get_context(), NK_STATIC, height, widget_count);
		return layout_space(get_context());
	}

	/// @}

	/**
	 * @name Layout - global settings and utility
	 * Calling these functions does not require specific layout state.
	 * @{
	 */

	/**
	 * @brief Set the currently used minimum row height.
	 * @param height New minimum row height to be used for auto generating the row height.
	 * @attention The passed height needs to include both your preferred row height as well as padding. No internal padding is added.
	 */
	void layout_set_min_row_height(float height)
	{
		nk_layout_set_min_row_height(&get_context(), height);
	}

	/**
	 * @brief Reset the currently used minimum row height back to `font_height + text_padding + padding`.
	 */
	void layout_reset_min_row_height()
	{
		nk_layout_reset_min_row_height(&get_context());
	}

	/**
	 * @brief Return the width of the next row allocated by one of the layouting functions.
	 */
	NUKLEUS_NODISCARD rect<float> layout_widget_bounds() const
	{
		return nk_layout_widget_bounds(&get_context());
	}

	/**
	 * @brief Utility function to calculate window ratio from pixel size.
	 * @param pixel_width Pixel width to convert to window ratio.
	 * @return Window ratio.
	 */
	NUKLEUS_NODISCARD float layout_ratio_from_pixel(float pixel_width) const
	{
		return nk_layout_ratio_from_pixel(&get_context(), pixel_width);
	}

	/// @}

	/**
	 * @name Group management
	 * For creating groups, see layout class.
	 * @{
	 */

	/**
	 * @brief Get the scroll position of the given group.
	 * @param id The id of the group to get the scroll position of
	 * @param x_offset x offset output, use null to ignore
	 * @param y_offset y offset output, use null to ignore
	 */
	void group_get_scroll(const char* id, nk_uint* x_offset, nk_uint* y_offset) // MISSING CONST
	{
		nk_group_get_scroll(&get_context(), id, x_offset, y_offset);
	}

	/**
	 * @brief Get the scroll position of the given group.
	 * @param id The id of the group to get the scroll position of
	 * @return x and y offsets.
	 */
	vec2<nk_uint> group_get_scroll(const char* id) // MISSING CONST
	{
		vec2<nk_uint> offset{};
		nk_group_get_scroll(&get_context(), id, &offset.x, &offset.y);
		return offset;
	}

	/**
	 * @brief Set the scroll position of the given group.
	 * @param id The id of the group to scroll
	 * @param x_offset The x offset to scroll to
	 * @param y_offset The y offset to scroll to
	 */
	void group_set_scroll(const char* id, nk_uint x_offset, nk_uint y_offset)
	{
		nk_group_set_scroll(&get_context(), id, x_offset, y_offset);
	}

	void group_set_scroll(const char* id, vec2<nk_uint> offsets)
	{
		nk_group_set_scroll(&get_context(), id, offsets.x, offsets.y);
	}

	/// @}

	/**
	 * @name Internal functions
	 * These functions are public for implementation reasons.
	 * They are used by `NUKLEUS_TREE_*` macros. Do not use these directly.
	 * @{
	 */
	tree _tree_scoped_internal(nk_bool result)
	{
		return tree(get_context(), result == nk_true ? &nk_tree_pop : nullptr);
	}

	tree _tree_state_scoped_internal(nk_bool result)
	{
		return tree(get_context(), result == nk_true ? &nk_tree_state_pop : nullptr);
	}

	tree _tree_element_scoped_internal(nk_bool result)
	{
		return tree(get_context(), result == nk_true ? &nk_tree_element_pop : nullptr);
	}

	/// @}

	/**
	 * @name Trees
	 * Trees represent two different concepts. First the concept of a collapsible
	 * UI section that can be either in a hidden or visible state. They allow the UI
	 * user to selectively minimize the current set of visible UI to comprehend.
	 * The second concept are tree widgets for visual UI representation of trees.
	 *
	 * Trees thereby can be nested for tree representations and multiple nested
	 * collapsible UI sections. Each starting functions takes a title label
	 * and optionally an image to be displayed and the initial collapse state from
	 * the `nk_collapse_states` section.
	 *
	 * The runtime state of the tree is either stored outside the library by the caller
	 * or inside which requires a unique ID. The unique ID can either be generated automatically
	 * from `__FILE__` and `__LINE__` with macro `nk_tree_push` (@ref NUKLEUS_TREE_SCOPED), by
	 * `__FILE__` and a user provided ID generated for example by loop index with macro
	 * `nk_tree_push_id` (@ref NUKLEUS_TREE_ID_SCOPED) or completely provided from outside by
	 * user with function `nk_tree_push_hashed` (@ref nk::window::tree_hashed_scoped).
	 *
	 * **Usage:**
	 *
	 * Each tree starting function will return a scope_guard object which must be checked.
	 * If the check fails, the tree is collapsed or hidden and therefore does not need
	 * to be filled with content.
	 *
	 * The tree header does not require and layouting function and instead
	 * calculates an auto height based on the currently used font size.
	 * @{
	 */

	/**
	 * @brief Start a collapsible UI section with internal state management with full
	 * control over internal unique ID used to store state.
	 * @param type Visually mark a tree node header as either a collapsible UI section or tree node.
	 * @param title Label printed in the tree header.
	 * @param initial_state Initial tree state value.
	 * @param hash Memory block or string to generate the ID from.
	 * @param len Size of passed memory block or string in @p hash.
	 * @param seed Seeding value if this function is called in a loop or default to `0`.
	 * @return Tree scope guard object that should be immediately checked.
	 */
	NUKLEUS_NODISCARD tree tree_hashed_scoped(
		nk_tree_type type, const char* title, nk_collapse_states initial_state, const char* hash, int len, int seed = 0)
	{
		return _tree_scoped_internal(nk_tree_push_hashed(&get_context(), type, title, initial_state, hash, len, seed));
	}

	/**
	 * @copydoc tree_hashed_scoped
	 * @param img Image to display inside the header on the left of the label.
	 */
	NUKLEUS_NODISCARD tree tree_image_hashed_scoped(
		nk_tree_type type, image img, const char* title, nk_collapse_states initial_state, const char* hash, int len, int seed = 0)
	{
		return _tree_scoped_internal(nk_tree_image_push_hashed(&get_context(), type, img, title, initial_state, hash, len, seed));
	}

	/**
	 * @brief Start a collapsible UI section with external state management.
	 * @param type Visually mark a tree node header as either a collapsible UI section or tree node.
	 * @param title Label printed in the tree header.
	 * @param state Persistent state to update.
	 * @return Tree scope guard object that should be immediately checked.
	 */
	NUKLEUS_NODISCARD tree tree_state_scoped(nk_tree_type type, const char* title, nk_collapse_states& state)
	{
		return _tree_state_scoped_internal(nk_tree_state_push(&get_context(), type, title, &state));
	}

	/**
	 * @copydoc tree_state_scoped(nk_tree_type, const char*, nk_collapse_states&)
	 */
	NUKLEUS_NODISCARD tree tree_state_scoped(nk_tree_type type, const char* title, bool& state)
	{
		nk_collapse_states collapse_state = state ? NK_MAXIMIZED : NK_MINIMIZED;
		tree result = tree_state_scoped(type, title, collapse_state);
		state = collapse_state == NK_MAXIMIZED;
		return result;
	}

	/**
	 * @copydoc tree_state_scoped(nk_tree_type, const char*, nk_collapse_states&)
	 * @param img Image to display inside the header on the left of the label.
	 */
	NUKLEUS_NODISCARD tree tree_state_image_scoped(
		nk_tree_type type, image img, const char* title, nk_collapse_states& state)
	{
		return _tree_state_scoped_internal(nk_tree_state_image_push(&get_context(), type, img, title, &state));
	}

	/**
	 * @copydoc tree_state_image_scoped(nk_tree_type, image, const char*, nk_collapse_states&)
	 */
	NUKLEUS_NODISCARD tree tree_state_image_scoped(
		nk_tree_type type, image img, const char* title, bool& state)
	{
		nk_collapse_states collapse_state = state ? NK_MAXIMIZED : NK_MINIMIZED;
		tree result = tree_state_image_scoped(type, img, title, collapse_state);
		state = collapse_state == NK_MAXIMIZED;
		return result;
	}

	/**
	 * @brief Start a collapsible UI section with external state management.
	 * @param type Visually mark a tree node header as either a collapsible UI section or tree node.
	 * @param title Label printed in the tree header.
	 * @param initial_state Initial tree state value.
	 * @param selected State to update
	 * @param hash Memory block or string to generate the ID from.
	 * @param len Size of passed memory block or string in @p hash.
	 * @param seed Seeding value if this function is called in a loop or default to `0`.
	 * @return Tree scope guard object that should be immediately checked.
	 */
	NUKLEUS_NODISCARD tree tree_element_hashed_scoped(
		nk_tree_type type, const char* title, nk_collapse_states initial_state, bool& selected, const char* hash, int len, int seed = 0)
	{
		return _tree_element_scoped_internal(nk_tree_element_push_hashed(
			&get_context(), type, title, initial_state, detail::output_bool(selected), hash, len, seed));
	}

	/**
	 * @copydoc tree_element_hashed_scoped
	 * @param img Image to display inside the header on the left of the label.
	 */
	NUKLEUS_NODISCARD tree tree_element_image_hashed_scoped(
		nk_tree_type type, image img, const char* title, nk_collapse_states initial_state, bool& selected, const char* hash, int len, int seed = 0)
	{
		return _tree_element_scoped_internal(nk_tree_element_image_push_hashed(
			&get_context(), type, img, title, initial_state, detail::output_bool(selected), hash, len, seed));
	}

	/**
	 * @brief Start a collapsible UI section with internal state management.
	 * @param win nk::window&
	 * @param type nk_tree_type.
	 * @param title const char*.
	 * @param state Initial tree state of type nk_collapse_states.
	 * @return Tree scope guard object that should be immediately checked.
	 *
	 * @details This macro automates @ref nk::window::tree_hashed_scoped.
	 */
#define NUKLEUS_TREE_SCOPED(win, type, title, state) \
	win._tree_scoped_internal(nk_tree_push   (&win.get_context(), type, title, state))
	/**
	 * @copydoc NUKLEUS_TREE_SCOPED
	 * @param id Loop counter index if this function is called in a loop.
	 */
#define NUKLEUS_TREE_ID_SCOPED(win, type, title, state, id) \
	win._tree_scoped_internal(nk_tree_push_id(&win.get_context(), type, title, state, id))

	/**
	 * @brief Start a collapsible UI section with internal state management.
	 * @param win nk::window&.
	 * @param type nk_tree_type.
	 * @param img nk_image.
	 * @param title const char*.
	 * @param state Initial tree state of type nk_collapse_states.
	 * @return Tree scope guard object that should be immediately checked.
	 *
	 * @details This macro automates @ref nk::window::tree_image_hashed_scoped.
	 */
#define NUKLEUS_TREE_IMAGE_SCOPED(win, type, img, title, state) \
	win._tree_scoped_internal(nk_tree_image_push   (&win.get_context(), type, img, title, state))
	/**
	 * @copydoc NUKLEUS_TREE_IMAGE_SCOPED
	 * @param id Loop counter index if this function is called in a loop.
	 */
#define NUKLEUS_TREE_IMAGE_ID_SCOPED(win, type, img, title, state, id) \
	win._tree_scoped_internal(nk_tree_image_push_id(&win.get_context(), type, img, title, state, id))

	/**
	 * @brief Start a collapsible UI section with external state management.
	 * @param win nk::window&.
	 * @param type nk_tree_type.
	 * @param title const char*.
	 * @param state Initial tree state of type nk_collapse_states.
	 * @param sel bool& selected state, will be updated
	 * @return Tree scope guard object that should be immediately checked.
	 *
	 * @details This macro automates @ref nk::window::tree_element_hashed_scoped.
	 */
#define NUKLEUS_TREE_ELEMENT_SCOPED(win, type, title, state, sel) \
	win._tree_element_scoped_internal(nk_tree_element_push   (&win.get_context(), type, title, state, ::nk::detail::output_bool(sel)))
	/**
	 * @copydoc NUKLEUS_TREE_ELEMENT_SCOPED
	 * @param id Loop counter index if this function is called in a loop.
	 */
#define NUKLEUS_TREE_ELEMENT_ID_SCOPED(win, type, title, state, sel, id) \
	win._tree_element_scoped_internal(nk_tree_element_push_id(&win.get_context(), type, title, state, ::nk::detail::output_bool(sel), id))

	/// @}

	/**
	 * @name List View
	 * UNDOCUMENTED and has NO EXAMPLES
	 * @{
	 */

	list_view list_view_scoped(const char* id, nk_flags flags, int row_height, int row_count)
	{
		nk_list_view lview;
		// Do not merge the lines below. list_view(lview, nk_list_view_begin(..., &lview, ...)) would cause
		// it to rely on function argument order of evaluation which is unspecified in C++.
		// This means it could copy the nk_list_view struct before nk_list_view_begin function call.
		bool valid = nk_list_view_begin(&get_context(), &lview, id, flags, row_height, row_count) == nk_true;
		return list_view(lview, valid);
	}

	/// @}

	/**
	 * @name Widget
	 * UNDOCUMENTED
	 * @{
	 */

	/**
	 * @brief Allocates space on the panel reserved for this widget to be used.
	 * @param bounds widget bounds
	 * @return the state of the widget space
	 * @details Return value should dictate further action:
	 * - `NK_WIDGET_INVALID`: widget is not seen and does not have to be updated or drawn
	 * - `NK_WIDGET_ROM`: widget is only (partially) visible - draw but don't update
	 * - other: do drawing and update
	 *
	 * The reason for it is to only draw and update what is
	 * actually necessary which is crucial for performance.
	 */
	widget_layout_states widget(rect<float>& bounds) const
	{
		struct nk_rect r{};
		auto result = nk_widget(&r, &get_context());
		bounds = r;
		return result;
	}

	widget_layout_states widget_fitting(rect<float>& bounds, vec2<float> item_padding) const
	{
		struct nk_rect r{};
		auto result = nk_widget_fitting(&r, &get_context(), item_padding);
		bounds = r;
		return result;
	}

	rect<float> widget_bounds() const
	{
		return nk_widget_bounds(&get_context());
	}

	vec2<float> widget_position() const
	{
		return nk_widget_position(&get_context());
	}

	vec2<float> widget_size() const
	{
		return nk_widget_size(&get_context());
	}

	float widget_width() const
	{
		return nk_widget_width(&get_context());
	}

	float widget_height() const
	{
		return nk_widget_height(&get_context());
	}

	bool widget_is_hovered() const
	{
		return nk_widget_is_hovered(&get_context()) == nk_true;
	}

	bool widget_is_mouse_clicked(buttons btn) const
	{
		return nk_widget_is_mouse_clicked(&get_context(), btn) == nk_true;
	}

	bool widget_has_mouse_click_down(buttons btn, bool down) const
	{
		return nk_widget_has_mouse_click_down(&get_context(), btn, down) == nk_true;
	}

	void spacing(int cols)
	{
		nk_spacing(&get_context(), cols);
	}

	/**
	 * @brief enable/disable widgets in scope.
	 * Disable widgets are greyed out and do not react to any input events.
	 * @param enable if true - no change, if false - disable widgets
	 * @return scope guard to hold, no checking required (guard is empty on `false`)
	 *
	 * @details It was a hard decision how to name this function:
	 * - https://english.stackexchange.com/questions/31878/noun-for-enable-enability-enabliness
	 * - https://english.stackexchange.com/questions/229276/is-there-a-better-word-for-enabledness
	 * - https://english.stackexchange.com/questions/92781/what-term-describes-the-state-of-being-either-enabled-or-disabled
	 */
	NUKLEUS_NODISCARD simple_scope_guard widget_enabledness_scoped(bool enable)
	{
		if (!enable)
			nk_widget_disable_begin(&get_context());

		return simple_scope_guard(get_context(), enable ? nullptr : nk_widget_disable_end);
	}

	/**
	 * @brief equivalent to `widget_enabledness_scoped(false)`
	 * @return scope guard to hold, no checking required
	 */
	NUKLEUS_NODISCARD simple_scope_guard widget_disable_scoped()
	{
		return widget_enabledness_scoped(false);
	}

	/// @}

	/**
	 * @name Text (unformatted)
	 * UNDOCUMENTED
	 * @{
	 */

	void text(const char* str, int len, nk_flags alignment)
	{
		nk_text(&get_context(), str, len, alignment);
	}

	void text_colored(const char* str, int len, nk_flags alignment, color col)
	{
		nk_text_colored(&get_context(), str, len, alignment, col);
	}

	void text_wrap(const char* str, int len)
	{
		nk_text_wrap(&get_context(), str, len);
	}

	void text_wrap_colored(const char* str, int len, color col)
	{
		nk_text_wrap_colored(&get_context(), str, len, col);
	}

	void label(const char* str, nk_flags alignment)
	{
		nk_label(&get_context(), str, alignment);
	}

	void label_colored(const char* str, nk_flags align, color col)
	{
		nk_label_colored(&get_context(), str, align, col);
	}

	void label_wrap(const char* str)
	{
		nk_label_wrap(&get_context(), str);
	}

	void label_colored_wrap(const char* str, color col)
	{
		nk_label_colored_wrap(&get_context(), str, col);
	}

	void image(nk::image img)
	{
		nk_image(&get_context(), img);
	}

	void image_color(nk::image img, color col)
	{
		nk_image_color(&get_context(), img, col);
	}

	/// @}

#ifdef NK_INCLUDE_STANDARD_VARARGS
	/**
	 * @name Text (formatted)
	 * Requires `NK_INCLUDE_STANDARD_VARARGS`.
	 * UNDOCUMENTED
	 * @{
	 */

	void labelf(nk_flags flags, NK_PRINTF_FORMAT_STRING const char* fmt, ...) NK_PRINTF_VARARG_FUNC(3)
	{
		va_list args;
		va_start(args, fmt);
		nk_labelfv(&get_context(), flags, fmt, args);
		va_end(args);
	}

	void labelf_colored(nk_flags flags, color col, NK_PRINTF_FORMAT_STRING const char* fmt, ...) NK_PRINTF_VARARG_FUNC(4)
	{
		va_list args;
		va_start(args, fmt);
		nk_labelfv_colored(&get_context(), flags, col, fmt, args);
		va_end(args);
	}

	void labelf_wrap(NK_PRINTF_FORMAT_STRING const char* fmt, ...) NK_PRINTF_VARARG_FUNC(2)
	{
		va_list args;
		va_start(args, fmt);
		nk_labelfv_wrap(&get_context(), fmt, args);
		va_end(args);
	}

	void labelf_colored_wrap(color col, NK_PRINTF_FORMAT_STRING const char* fmt, ...) NK_PRINTF_VARARG_FUNC(3)
	{
		va_list args;
		va_start(args, fmt);
		nk_labelfv_colored_wrap(&get_context(), col, fmt, args);
		va_end(args);
	}

	void labelfv(nk_flags flags, NK_PRINTF_FORMAT_STRING const char* fmt, va_list args) NK_PRINTF_VALIST_FUNC(3)
	{
		nk_labelfv(&get_context(), flags, fmt, args);
	}

	void labelfv_colored(nk_flags flags, color col, NK_PRINTF_FORMAT_STRING const char* fmt, va_list args) NK_PRINTF_VALIST_FUNC(4)
	{
		nk_labelfv_colored(&get_context(), flags, col, fmt, args);
	}

	void labelfv_wrap(NK_PRINTF_FORMAT_STRING const char* fmt, va_list args) NK_PRINTF_VALIST_FUNC(2)
	{
		nk_labelfv_wrap(&get_context(), fmt, args);
	}

	void labelfv_colored_wrap(color col, NK_PRINTF_FORMAT_STRING const char* fmt, va_list args) NK_PRINTF_VALIST_FUNC(3)
	{
		nk_labelfv_colored_wrap(&get_context(), col, fmt, args);
	}

	void value(const char* prefix, bool value)
	{
		nk_value_bool(&get_context(), prefix, value);
	}

	void value(const char* prefix, int value)
	{
		nk_value_int(&get_context(), prefix, value);
	}

	void value(const char* prefix, unsigned value)
	{
		nk_value_uint(&get_context(), prefix, value);
	}

	void value(const char* prefix, float value)
	{
		nk_value_float(&get_context(), prefix, value);
	}

	void value(const char* prefix, double value)
	{
		// There is no nk_value_double but the implementation of nk_value_float casts to double internally.
		// The line below is identical to nk_value_float except it actually uses double parameter.
		nk_labelf(&get_context(), NK_TEXT_LEFT, "%s: %.3f", prefix, value);
	}

	void value_color_byte(const char* prefix, color col)
	{
		nk_value_color_byte(&get_context(), prefix, col);
	}

	void value_color_float(const char* prefix, color col)
	{
		nk_value_color_float(&get_context(), prefix, col);
	}

	void value_color_hex(const char* prefix, color col)
	{
		nk_value_color_hex(&get_context(), prefix, col);
	}

	/// @}
#endif // NK_INCLUDE_STANDARD_VARARGS

	/**
	 * @name Button
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD bool button_text(const char* title, int len)
	{
		return nk_button_text(&get_context(), title, len) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_label(const char* title)
	{
		return nk_button_label(&get_context(), title) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_color(color col)
	{
		return nk_button_color(&get_context(), col) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_symbol(symbol_type symbol)
	{
		return nk_button_symbol(&get_context(), symbol) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_image(nk::image img)
	{
		return nk_button_image(&get_context(), img) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_symbol_label(symbol_type symbol, const char* label, nk_flags align)
	{
		return nk_button_symbol_label(&get_context(), symbol, label, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_symbol_text(symbol_type symbol, const char* text, int len, nk_flags align)
	{
		return nk_button_symbol_text(&get_context(), symbol, text, len, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_image_label(nk::image img, const char* label, nk_flags align)
	{
		return nk_button_image_label(&get_context(), img, label, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_image_text(nk::image img, const char* text, int len, nk_flags align)
	{
		return nk_button_image_text(&get_context(), img, text, len, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_text_styled(const style_button& style, const char* title, int len)
	{
		return nk_button_text_styled(&get_context(), &style, title, len) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_label_styled(const style_button& style, const char* title)
	{
		return nk_button_label_styled(&get_context(), &style, title) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_symbol_styled(const style_button& style, symbol_type symbol)
	{
		return nk_button_symbol_styled(&get_context(), &style, symbol) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_image_styled(const style_button& style, nk::image img)
	{
		return nk_button_image_styled(&get_context(), &style, img) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_symbol_text_styled(const style_button& style, symbol_type symbol, const char* text, int len, nk_flags align)
	{
		return nk_button_symbol_text_styled(&get_context(), &style, symbol, text, len, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_symbol_label_styled(const style_button& style, symbol_type symbol, const char* title, nk_flags align)
	{
		return nk_button_symbol_label_styled(&get_context(), &style, symbol, title, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_image_label_styled(const style_button& style, nk::image img, const char* label, nk_flags align)
	{
		return nk_button_image_label_styled(&get_context(), &style, img, label, align) == nk_true;
	}

	NUKLEUS_NODISCARD bool button_image_text_styled(const style_button& style, nk::image img, const char* text, int len, nk_flags align)
	{
		return nk_button_image_text_styled(&get_context(), &style, img, text, len, align) == nk_true;
	}

	/**
	 * @brief set button behavior (persistent, untill changed again)
	 * @param behavior behavior to set
	 * @details use `NK_BUTTON_DEFAULT` or @ref button_reset_behavior to reset
	 */
	void button_set_behavior(button_behavior behavior)
	{
		nk_button_set_behavior(&get_context(), behavior);
	}

	void button_reset_behavior()
	{
		nk_button_set_behavior(&get_context(), NK_BUTTON_DEFAULT);
	}

	NUKLEUS_NODISCARD scoped_override_guard button_behavior_scoped(button_behavior behavior)
	{
		return scoped_override_guard(
			get_context(),
			nk_button_push_behavior(&get_context(), behavior) == nk_true ? &nk_button_pop_behavior : nullptr);
	}

	/// @}

	/**
	 * @name Checkbox
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD bool checkbox_label(const char* label, bool active)
	{
		return nk_check_label(&get_context(), label, active) == nk_true;
	}

	NUKLEUS_NODISCARD bool checkbox_text(const char* text, int len, bool active)
	{
		return nk_check_text(&get_context(), text, len, active) == nk_true;
	}

	NUKLEUS_NODISCARD bool checkbox_text_align(const char* text, int len, bool active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_check_text_align(&get_context(), text, len, active, widget_alignment, text_alignment) == nk_true;
	}

	NUKLEUS_NODISCARD unsigned checkbox_flags_label(const char* label, unsigned flags, unsigned value)
	{
		return nk_check_flags_label(&get_context(), label, flags, value);
	}

	NUKLEUS_NODISCARD unsigned checkbox_flags_text(const char* text, int len, unsigned flags, unsigned value)
	{
		return nk_check_flags_text(&get_context(), text, len, flags, value);
	}

	NUKLEUS_NODISCARD bool checkbox_label_in_place(const char* label, bool& active)
	{
		return nk_checkbox_label(&get_context(), label, detail::output_bool(active)) == nk_true;
	}

	NUKLEUS_NODISCARD bool checkbox_label_align_in_place(const char* label, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_checkbox_label_align(&get_context(), label, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool checkbox_text_in_place(const char* text, int len, bool& active)
	{
		return nk_checkbox_text(&get_context(), text, len, detail::output_bool(active)) == nk_true;
	}

	NUKLEUS_NODISCARD bool checkbox_text_align_in_place(const char* text, int len, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_checkbox_text_align(&get_context(), text, len, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool checkbox_flags_label_in_place(const char* label, unsigned& flags, unsigned value)
	{
		return nk_checkbox_flags_label(&get_context(), label, &flags, value) == nk_true;
	}

	NUKLEUS_NODISCARD bool checkbox_flags_text_in_place(const char* text, int len, unsigned& flags, unsigned value)
	{
		return nk_checkbox_flags_text(&get_context(), text, len, &flags, value) == nk_true;
	}

	/// @}

	/**
	 * @name Radio Button
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD bool radio_label_in_place(const char* label, bool& active)
	{
		return nk_radio_label(&get_context(), label, detail::output_bool(active)) == nk_true;
	}
	NUKLEUS_NODISCARD bool radio_label_align_in_place(const char* label, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_radio_label_align(&get_context(), label, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool radio_text_in_place(const char* text, int len, bool& active)
	{
		return nk_radio_text(&get_context(), text, len, detail::output_bool(active)) == nk_true;
	}

	NUKLEUS_NODISCARD bool radio_text_align_in_place(const char* text, int len, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_radio_text_align(&get_context(), text, len, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool option_label(const char* label, bool active)
	{
		return nk_option_label(&get_context(), label, active) == nk_true;
	}

	NUKLEUS_NODISCARD bool option_label_align(const char* label, bool active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_option_label_align(&get_context(), label, active, widget_alignment, text_alignment) == nk_true;
	}

	NUKLEUS_NODISCARD bool option_text(const char* text, int len, bool active)
	{
		return nk_option_text(&get_context(), text, len, active) == nk_true;
	}

	NUKLEUS_NODISCARD bool option_text_align(const char* text, int len, bool active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_option_text_align(&get_context(), text,  len, active, widget_alignment, text_alignment) == nk_true;
	}

	/// @}

	/**
	 * @name Selectable
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD bool selectable_label_in_place(const char* str, nk_flags align, bool& value)
	{
		return nk_selectable_label(&get_context(), str, align, detail::output_bool(value)) == nk_true;
	}

	NUKLEUS_NODISCARD bool selectable_text_in_place(const char* str, int len, nk_flags align, bool& value)
	{
		return nk_selectable_text(&get_context(), str, len, align, detail::output_bool(value));
	}

	NUKLEUS_NODISCARD bool selectable_image_label_in_place(nk::image img, const char* str, nk_flags align, bool& value)
	{
		return nk_selectable_image_label(&get_context(), img, str, align, detail::output_bool(value));
	}

	NUKLEUS_NODISCARD bool selectable_image_text_in_place(nk::image img, const char* str, int len, nk_flags align, bool& value)
	{
		return nk_selectable_image_text(&get_context(), img, str, len, align, detail::output_bool(value));
	}

	NUKLEUS_NODISCARD bool selectable_symbol_label_in_place(symbol_type symbol, const char* str, nk_flags align, bool& value)
	{
		return nk_selectable_symbol_label(&get_context(), symbol, str, align, detail::output_bool(value));
	}

	NUKLEUS_NODISCARD bool selectable_symbol_text_in_place(symbol_type symbol, const char* str, int len, nk_flags align, bool& value)
	{
		return nk_selectable_symbol_text(&get_context(), symbol, str, len, align, detail::output_bool(value));
	}

	NUKLEUS_NODISCARD bool selectable_label(const char* str, nk_flags align, bool value)
	{
		return nk_select_label(&get_context(), str, align, value) == nk_true;
	}

	NUKLEUS_NODISCARD bool selectable_text(const char* str, int len, nk_flags align, bool value)
	{
		return nk_select_text(&get_context(), str, len, align, value) == nk_true;
	}

	NUKLEUS_NODISCARD bool selectable_image_label(nk::image img, const char* str, nk_flags align, bool value)
	{
		return nk_select_image_label(&get_context(), img, str, align, value) == nk_true;
	}

	NUKLEUS_NODISCARD bool selectable_image_text(nk::image img, const char* str, int len, nk_flags align, bool value)
	{
		return nk_select_image_text(&get_context(), img, str, len, align, value) == nk_true;
	}

	NUKLEUS_NODISCARD bool selectable_symbol_label(symbol_type symbol, const char* str, nk_flags align, bool value)
	{
		return nk_select_symbol_label(&get_context(), symbol, str, align, value) == nk_true;
	}

	NUKLEUS_NODISCARD bool selectable_symbol_text(symbol_type symbol, const char* str, int len, nk_flags align, bool value)
	{
		return nk_select_symbol_text(&get_context(), symbol, str, len, align, value) == nk_true;
	}

	/// @}

	/**
	 * @name Slider
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD float slider(float min, float value, float max, float step)
	{
		return nk_slide_float(&get_context(), min, value, max, step);
	}

	NUKLEUS_NODISCARD int slider(int min, int value, int max, int step)
	{
		return nk_slide_int(&get_context(), min, value, max, step);
	}

	NUKLEUS_NODISCARD bool slider_in_place(float min, float& value, float max, float step)
	{
		return nk_slider_float(&get_context(), min, &value, max, step) == nk_true;
	}

	NUKLEUS_NODISCARD bool slider_in_place(int min, int& value, int max, int step)
	{
		return nk_slider_int(&get_context(), min, &value, max, step) == nk_true;
	}

	/// @}

	/**
	 * @name Knob
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD bool knob_in_place(float min, float& value, float max, float step, heading zero_direction, float dead_zone_degrees)
	{
		return nk_knob_float(&get_context(), min, &value, max, step, zero_direction, dead_zone_degrees) == nk_true;
	}

	NUKLEUS_NODISCARD bool knob_in_place(int min, int& value, int max, int step, heading zero_direction, float dead_zone_degrees)
	{
		return nk_knob_int(&get_context(), min, &value, max, step, zero_direction, dead_zone_degrees) == nk_true;
	}

	/// @}

	/**
	 * @name Progress Bar
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD bool progress_bar_in_place(nk_size& cur, nk_size max, bool modifyable)
	{
		return nk_progress(&get_context(), &cur, max, modifyable) == nk_true;
	}

	NUKLEUS_NODISCARD nk_size progress_bar(nk_size cur, nk_size max, bool modifyable)
	{
		return nk_prog(&get_context(), cur, max, modifyable);
	}

	/// @}

	/**
	 * @name Color Picker
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD colorf color_picker(colorf col, color_format fmt)
	{
		return nk_color_picker(&get_context(), col, fmt);
	}

	NUKLEUS_NODISCARD bool color_picker_in_place(nk_colorf& col, color_format fmt)
	{
		return nk_color_pick(&get_context(), &col, fmt) == nk_true;
	}

	NUKLEUS_NODISCARD bool color_picker_in_place(colorf& col, color_format fmt)
	{
		auto c = static_cast<nk_colorf>(col);
		const bool result = color_picker_in_place(c, fmt);
		col = c;
		return result;
	}

	/// @}

	/**
	 * @name Properties
	 * Properties are the main value modification widgets in Nuklear. Changing a value
	 * can be achieved by dragging, adding/removing incremental steps on button click
	 * or by directly typing a number.
	 *
	 * Each property requires a unique name for identification that is also used for
	 * displaying a label. If you want to use the same name multiple times make sure
	 * to add a '#' before your name. The '#' will not be shown but will generate a
	 * unique ID. Each property also takes in a minimum and maximum value.
	 * In additional each property takes in
	 * a increment value that will be added or subtracted if either the increment
	 * decrement button is clicked. Finally there is a value for increment per pixel
	 * dragged that is added or subtracted from the value.
	 *
	 * @{
	 */

	/**
	 * @brief Property directly modifying a passed in value.
	 * @param name String used both as a label as well as a unique identifier.
	 * @param min Minimum allowed value.
	 * @param val Value to be modified.
	 * @param max Maximum allowed value.
	 * @param step Value change on increment and decrement button.
	 * @param inc_per_pixel Value change per pixel on dragging.
	 */
	void property_in_place(const char* name, int min, int& val, int max, int step = 1, float inc_per_pixel = 1.0f)
	{
		nk_property_int(&get_context(), name, min, &val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc property_in_place(const char*, int, int&, int, int, float)
	 */
	void property_in_place(const char* name, float min, float& val, float max, float step = 1.0f, float inc_per_pixel = 1.0f)
	{
		nk_property_float(&get_context(), name, min, &val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc property_in_place(const char*, int, int&, int, int, float)
	 */
	void property_in_place(const char* name, double min, double& val, double max, double step = 1.0, float inc_per_pixel = 1.0f)
	{
		nk_property_double(&get_context(), name, min, &val, max, step, inc_per_pixel);
	}

	/**
	 * @brief Property modifying a passed in value and returning the new value.
	 * @param name String used both as a label as well as a unique identifier.
	 * @param min Minimum allowed value.
	 * @param val Value to be modified.
	 * @param max Maximum allowed value.
	 * @param step Value change on increment and decrement button.
	 * @param inc_per_pixel Value change per pixel on dragging.
	 * @return New value, after modification.
	 */
	NUKLEUS_NODISCARD int property(const char* name, int min, int val, int max, int step = 1, float inc_per_pixel = 1.0f)
	{
		return nk_propertyi(&get_context(), name, min, val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc property(const char* name, int min, int val, int max, int step = 1, float inc_per_pixel)
	 */
	NUKLEUS_NODISCARD float property(const char* name, float min, float val, float max, float step = 1.0f, float inc_per_pixel = 1.0f)
	{
		return nk_propertyf(&get_context(), name, min, val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc property(const char* name, int min, int val, int max, int step = 1, float inc_per_pixel)
	 */
	NUKLEUS_NODISCARD double property(const char* name, double min, double val, double max, double step = 1.0, float inc_per_pixel = 1.0f)
	{
		return nk_propertyd(&get_context(), name, min, val, max, step, inc_per_pixel);
	}

	/// @}

	/**
	 * @name Text Edit
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD nk_flags edit_string(nk_flags flags, char* buffer, int& len, int max, nk_plugin_filter filter)
	{
		return nk_edit_string(&get_context(), flags, buffer, &len, max, filter);
	}

	NUKLEUS_NODISCARD nk_flags edit_string_zero_terminated(nk_flags flags, char* buffer, int max, nk_plugin_filter filter)
	{
		return nk_edit_string_zero_terminated(&get_context(), flags, buffer, max, filter);
	}

	NUKLEUS_NODISCARD nk_flags edit_buffer(nk_flags flags, nk_text_edit& edit, nk_plugin_filter filter)
	{
		return nk_edit_buffer(&get_context(), flags, &edit, filter);
	}

	void edit_focus(nk_flags flags)
	{
		nk_edit_focus(&get_context(), flags);
	}

	void edit_unfocus()
	{
		nk_edit_unfocus(&get_context());
	}

	/// @}

	/**
	 * @name Chart
	 * UNDOCUMENTED (text from demo)
	 * This library has two different rather simple charts. The line and the
	 * column chart. Both provide a simple way of visualizing values and
	 * have a retained mode and immediate mode API version.
	 *
	 * For the retained
	 * mode version `plot` and `plot_function` you either provide
	 * an array or a callback to call to handle drawing the graph.
	 *
	 * For the immediate mode version you start by calling `chart_scoped`
	 * and need to provide min and max values for scaling on the Y-axis.
	 * and then call `push` on it to push values into the chart.
	 * @{
	 */

	NUKLEUS_NODISCARD chart chart_scoped(nk_chart_type type, int count, float min, float max)
	{
		return chart(
			get_context(),
			nk_chart_begin(&get_context(), type, count, min, max) == nk_true ? &nk_chart_end : nullptr);
	}

	NUKLEUS_NODISCARD chart chart_colored_scoped(nk_chart_type type, color col, color highlight, int count, float min, float max)
	{
		return chart(
			get_context(),
			nk_chart_begin_colored(&get_context(), type, col, highlight, count, min, max) == nk_true ? &nk_chart_end : nullptr);
	}

	void plot(nk_chart_type type, const float* values, int count, int offset)
	{
		nk_plot(&get_context(), type, values, count, offset);
	}

	void plot_function(nk_chart_type type, void* userdata, float(*value_getter)(void* userdata, int index), int count, int offset)
	{
		nk_plot_function(&get_context(), type, userdata, value_getter, count, offset);
	}

	/**
	 * @brief make a plot with the help of a function object
	 * @tparam F function usable as `float (int index)`
	 * @param type chart type
	 * @param f function object returning values under specific indexes
	 * @param count number of values
	 * @param offset value that will be added to any index
	 */
	template <typename F>
	void plot_function(nk_chart_type type, F&& f, int count, int offset)
	{
		static_assert(!is_pointer<remove_reference_t<F>>::value, "pass a function object/reference, not a function pointer");
		static_assert(is_invocable_r<float, F, int>::value, "function must satisfy float(int)");

		auto value_getter = [](void* userdata, int index) -> float {
			return (*static_cast<remove_reference_t<F>*>(userdata))(index);
		};
		plot_function(type, static_cast<void*>(&f), value_getter, count, offset);
	}

	/// @}

	/**
	 * @name Popup
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD popup popup_scoped(nk_popup_type type, const char* title, nk_flags flags, rect<float> bounds)
	{
		return popup(
			get_context(),
			nk_popup_begin(&get_context(), type, title, flags, bounds) == nk_true ? &nk_popup_end : nullptr);
	}

	/// @}

	/**
	 * @name Combobox (whole widget)
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD int combobox(const char* const* items, int count, int selected, int item_height, vec2<float> size)
	{
		return nk_combo(&get_context(), items, count, selected, item_height, size);
	}

	NUKLEUS_NODISCARD int combobox_string(const char* items_separated_by_zeros, int selected, int count, int item_height, vec2<float> size)
	{
		return nk_combo_string(&get_context(), items_separated_by_zeros, selected, count, item_height, size);
	}

	NUKLEUS_NODISCARD int combobox_separator(const char* items_separated_by_separator, int separator, int selected, int count, int item_height, vec2<float> size)
	{
		return nk_combo_separator(&get_context(), items_separated_by_separator, separator, selected, count, item_height, size);
	}

	NUKLEUS_NODISCARD int combobox_callback(void (*item_getter)(void*, int, const char**), void* userdata, int selected, int count, int item_height, vec2<float> size)
	{
		return nk_combo_callback(&get_context(), item_getter, userdata, selected, count, item_height, size);
	}

	template <typename F>
	NUKLEUS_NODISCARD int combobox_callback(F&& item_getter, int selected, int count, int item_height, vec2<float> size)
	{
		static_assert(!is_pointer<remove_reference_t<F>>::value, "pass a function object/reference, not a function pointer");
		static_assert(is_invocable_r<const char*, F, int>::value, "function must satisfy const char*(int)");

		auto func = [](void* data, int index, const char** result)
		{
			*result = (*static_cast<remove_reference_t<F>*>(data))(index);
		};
		return combobox_callback(func, &item_getter, selected, count, item_height, size);
	}

	void combobox_in_place(const char* const* items, int count, int& selected, int item_height, vec2<float> size)
	{
		nk_combobox(&get_context(), items, count, &selected, item_height, size);
	}

	void combobox_string_in_place(const char* items_separated_by_zeros, int& selected, int count, int item_height, vec2<float> size)
	{
		nk_combobox_string(&get_context(), items_separated_by_zeros, &selected, count, item_height, size);
	}

	void combobox_separator_in_place(const char* items_separated_by_separator, int separator, int& selected, int count, int item_height, vec2<float> size)
	{
		nk_combobox_separator(&get_context(), items_separated_by_separator, separator, &selected, count, item_height, size);
	}

	void combobox_callback_in_place(void (*item_getter)(void*, int, const char**), void* userdata, int& selected, int count, int item_height, vec2<float> size)
	{
		nk_combobox_callback(&get_context(), item_getter, userdata, &selected, count, item_height, size);
	}

	template <typename F>
	void combobox_callback_in_place(F&& item_getter, int& selected, int count, int item_height, vec2<float> size)
	{
		static_assert(!is_pointer<remove_reference_t<F>>::value, "pass a function object/reference, not a function pointer");
		static_assert(is_invocable_r<const char*, F, int>::value, "function must satisfy const char*(int)");

		auto func = [](void* data, int index, const char** result)
		{
			*result = (*static_cast<remove_reference_t<F>*>(data))(index);
		};
		combobox_callback_in_place(func, &item_getter, selected, count, item_height, size);
	}

	/// @}

private:
	// need to use class to disambiguate between a function and a type
	NUKLEUS_NODISCARD class combobox combo_internal_scoped(nk_bool result)
	{
		using return_type = class combobox;
		return return_type(get_context(), result == nk_true ? &nk_combo_end : nullptr);
	}

public:
	/**
	 * @name Abstract Combobox
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD class combobox combo_text_scoped(const char* selected, int len, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_text(&get_context(), selected, len, size));
	}

	NUKLEUS_NODISCARD class combobox combo_label_scoped(const char* selected, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_label(&get_context(), selected, size));
	}

	NUKLEUS_NODISCARD class combobox combo_color_scoped(color col, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_color(&get_context(), col, size));
	}

	NUKLEUS_NODISCARD class combobox combo_symbol_scoped(nk_symbol_type symbol, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_symbol(&get_context(), symbol, size));
	}

	NUKLEUS_NODISCARD class combobox combo_symbol_label_scoped(const char* selected, nk_symbol_type symbol, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_symbol_label(&get_context(), selected, symbol, size));
	}

	NUKLEUS_NODISCARD class combobox combo_symbol_text_scoped(const char* selected, int len, nk_symbol_type symbol, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_symbol_text(&get_context(), selected, len, symbol, size));
	}

	NUKLEUS_NODISCARD class combobox combo_image_scoped(nk::image img, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_image(&get_context(), img, size));
	}

	NUKLEUS_NODISCARD class combobox combo_image_label_scoped(const char* selected, nk::image img, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_image_label(&get_context(), selected, img, size));
	}

	NUKLEUS_NODISCARD class combobox combo_image_text_scoped(const char* selected, int len, nk::image img, vec2<float> size)
	{
		return combo_internal_scoped(nk_combo_begin_image_text(&get_context(), selected, len, img, size));
	}

	/// @}

	/**
	 * @name Contextual
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD contextual contextual_scoped(nk_flags flags, vec2<float> size, rect<float> trigger_bounds)
	{
		return contextual(
			get_context(),
			nk_contextual_begin(&get_context(), flags, size, trigger_bounds) == nk_true ? &nk_contextual_end : nullptr);
	}

	/// @}

	/**
	 * @name Tooltip
	 * UNDOCUMENTED
	 * @{
	 */

	void tooltip(const char* label)
	{
		nk_tooltip(&get_context(), label);
	}

#ifdef NK_INCLUDE_STANDARD_VARARGS
	/**
	 * @brief Requires `NK_INCLUDE_STANDARD_VARARGS`.
	 * @param fmt format string.
	 */
	void tooltipf(NK_PRINTF_FORMAT_STRING const char* fmt, ...) NK_PRINTF_VARARG_FUNC(2)
	{
		va_list args;
		va_start(args, fmt);
		nk_tooltipfv(&get_context(), fmt, args);
		va_end(args);
	}

	/**
	 * @brief Requires `NK_INCLUDE_STANDARD_VARARGS`.
	 * @param fmt format string.
	 * @param args variadic arguments list
	 */
	void tooltipfv(NK_PRINTF_FORMAT_STRING const char* fmt, va_list args) NK_PRINTF_VALIST_FUNC(2)
	{
		nk_tooltipfv(&get_context(), fmt, args);
	}
#endif

	scope_guard tooltip_scoped(float width)
	{
		return scope_guard(get_context(), nk_tooltip_begin(&get_context(), width) == nk_true ? &nk_tooltip_end : nullptr);
	}

	/// @}

private:
	NUKLEUS_NODISCARD menu menu_internal_scoped(nk_bool result)
	{
		return menu(get_context(), result == nk_true ? &nk_menu_end : nullptr);
	}

public:
	/**
	 * @name Menu
	 * UNDOCUMENTED
	 * @{
	 */

	NUKLEUS_NODISCARD scope_guard menubar_scoped()
	{
		nk_menubar_begin(&get_context()); // this one returns void - always succeeds
		return scope_guard(get_context(), &nk_menubar_end);
	}

	NUKLEUS_NODISCARD menu menu_text_scoped(const char* text, int len, nk_flags alignment, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_text(&get_context(), text, len, alignment, size));
	}

	NUKLEUS_NODISCARD menu menu_label_scoped(const char* label, nk_flags alignment, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_label(&get_context(), label, alignment, size));
	}

	NUKLEUS_NODISCARD menu menu_image_scoped(const char* id, nk::image img, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_image(&get_context(), id, img, size));
	}

	NUKLEUS_NODISCARD menu menu_image_text_scoped(const char* text, int len, nk_flags alignment, nk::image img, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_image_text(&get_context(), text, len, alignment, img, size));
	}

	NUKLEUS_NODISCARD menu menu_image_label_scoped(const char* label, nk_flags alignment, nk::image img, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_image_label(&get_context(), label, alignment, img, size));
	}

	NUKLEUS_NODISCARD menu menu_symbol_scoped(const char* id, nk_symbol_type symbol, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_symbol(&get_context(), id, symbol, size));
	}

	NUKLEUS_NODISCARD menu menu_symbol_text_scoped(const char* text, int len, nk_flags alignment, nk_symbol_type symbol, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_symbol_text(&get_context(), text, len, alignment, symbol, size));
	}

	NUKLEUS_NODISCARD menu menu_symbol_label_scoped(const char* label, nk_flags alignment, nk_symbol_type symbol, vec2<float> size)
	{
		return menu_internal_scoped(nk_menu_begin_symbol_label(&get_context(), label, alignment, symbol, size));
	}

	/// @}

private:
	bool m_valid;
};

/**
 * @brief Contexts are the main entry point and contain all required state. They are used for window, memory, input,
 * style, stack, commands and time management and need to be passed into all nuklear GUI specific functions.
 */
class context
{
public:
	/**
	 * @name Context lifetime
	 * Functions related to context lifetime, initialization and management.
	 * @{
	 */

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
	/**
	 * @brief Initialize context with default standard library allocator. Requires `NK_INCLUDE_DEFAULT_ALLOCATOR`.
	 * @details Should be used if you don't want to be bothered with memory management in nuklear.
	 * @param user_font Previously initialized font handle.
	 * @return Context object - always check @ref is_valid after the call.
	 */
	NUKLEUS_NODISCARD static context init_default(const nk_user_font& user_font)
	{
		context ctx;
		ctx.m_valid = nk_init_default(&ctx.m_ctx, &user_font) == nk_true;
		return ctx;
	}

	/**
	 * @brief Initialize context with default standard library allocator. Requires `NK_INCLUDE_DEFAULT_ALLOCATOR`.
	 * @details Should be used if you don't want to be bothered with memory management in nuklear.
	 * @return Context object - always check @ref is_valid after the call.
	 * @attention This overload initializes context without any font. You must call @ref style_set_font afterward.
	 */
	NUKLEUS_NODISCARD static context init_default()
	{
		context ctx;
		ctx.m_valid = nk_init_default(&ctx.m_ctx, nullptr) == nk_true;
		return ctx;
	}
#endif

	/**
	 * @brief Initializes context from single fixed size memory block
	 * @details Should be used if you want complete control over nuklear's memory management.
	 * Especially recommended for system with little memory or systems with virtual memory.
	 * For the later case you can just allocate for example 16MB of virtual memory
	 * and only the required amount of memory will actually be committed.
	 * @param memory Previously allocated memory block.
	 * @param size Size of memory.
	 * @param user_font Previously initialized font handle.
	 * @return Context object - always check @ref is_valid after the call.
	 *
	 * @attention Make sure the passed memory block is aligned correctly for `nk_draw_command`s.
	 */
	NUKLEUS_NODISCARD static context init_fixed(void* memory, nk_size size, const nk_user_font& user_font)
	{
		NUKLEUS_ASSERT_MSG(is_aligned<nk_draw_command>(memory), "Memory pointer must be aligned");
		context ctx;
		ctx.m_valid = nk_init_fixed(&ctx.m_ctx, memory, size, &user_font) == nk_true;
		return ctx;
	}

	/**
	 * @brief Initialize context with memory allocation callbacks for nuklear to allocate
	 * memory from. Used internally for nk_init_default and provides a kitchen sink allocation
	 * interface to nuklear. Can be useful for cases like monitoring memory consumption.
	 * @param allocator Previously allocated memory allocator.
	 * @param user_font Previously initialized font handle.
	 * @return Context object - always check @ref is_valid after the call.
	 */
	NUKLEUS_NODISCARD static context init(const nk_allocator& allocator, const nk_user_font& user_font)
	{
		context ctx;
		ctx.m_valid = nk_init(&ctx.m_ctx, &allocator, &user_font) == nk_true;
		return ctx;
	}

	/**
	 * @brief Initialize context from two different either fixed or growing buffers.
	 * The first buffer is for allocating draw commands while the second buffer is
	 * used for allocating windows, panels and state tables.
	 * @param cmds Previously initialized memory buffer either fixed or dynamic to store draw commands into.
	 * @param pool Previously initialized memory buffer either fixed or dynamic to store windows, panels and tables.
	 * @param user_font Previously initialized font handle.
	 * @return Context object - always check @ref is_valid after the call.
	 */
	NUKLEUS_NODISCARD static context init_custom(nk_buffer& cmds, nk_buffer& pool, const nk_user_font& user_font)
	{
		context ctx;
		ctx.m_valid = nk_init_custom(&ctx.m_ctx, &cmds, &pool, &user_font) == nk_true;
		return ctx;
	}

	/**
	 * @brief Initialize context from an already existing instance
	 * @details This function takes ownership of the context, similar to unique_ptr's pointer constructor.
	 * Freeing the same context outside the class will lead to undefined behavior.
	 * @param ctx Nuklear's %context object.
	 * @param valid Whether the object is valid.
	 */
	context(nk_context ctx, bool valid = true)
	: m_ctx(ctx), m_valid(valid)
	{}

	context(const context& other) = delete;
	context(context&& other) noexcept
	{
		swap(*this, other);
	}

	context& operator=(const context& other) = delete;
	context& operator=(context&& other) noexcept
	{
		swap(*this, other);
		return *this;
	}

	~context() { free(); }

	/**
	 * @brief Free all memory allocated by nuklear. Called automatically in the destructor.
	 */
	void free()
	{
		if (!is_valid())
			return;

		nk_free(&m_ctx);
		m_valid = false;
	}

	/**
	 * @brief Reset the context state at the end of the frame. This includes mostly garbage
	 * collector tasks like removing windows or table not called and therefore not used anymore.
	 * @details Context must be valid, @ref is_valid.
	 */
	void clear()
	{
		NUKLEUS_ASSERT(m_valid);
		nk_clear(&m_ctx);
	}

#ifdef NK_INCLUDE_COMMAND_USERDATA
	/**
	 * @brief Set the userdata passed down into each draw command. Requires `NK_INCLUDE_COMMAND_USERDATA`.
	 * @param h the value to set
	 */
	void set_user_data(handle h)
	{
		nk_set_user_data(&m_ctx, h);
	}
#endif
	explicit operator bool() && noexcept = delete;
	/**
	 * @copydoc is_valid
	 */
	explicit operator bool() const & noexcept
	{
		return is_valid();
	}

	/**
	 * @brief Check the state of this object.
	 * @return Whether initialization was successful.
	 */
	NUKLEUS_NODISCARD bool is_valid() const noexcept { return m_valid; }

	/**
	 * @brief Obtain a reference to internal context object.
	 * @return Nuklear's context object.
	 */
	const nk_context& get() const { return m_ctx; }

	/**
	 * @copydoc get() const
	 * @details Do not change initialization state, use @ref release in such case.
	 */
	nk_context& get() { return m_ctx; }

	/**
	 * @brief Release currently managed context, similarly to unique_ptr's release.
	 * @details After the call, @ref is_valid will return false.
	 * @return Nuklear's context object with associated resources (if any).
	 */
	NUKLEUS_NODISCARD nk_context release()
	{
		m_valid = false;
		return exchange(m_ctx, nk_context{});
	}

	/// @}

	/**
	 * @name Input
	 * Input getters should be used after the end of scoped input.
	 * @{
	 */

	/**
	 * @brief Start scoped input. `nk_input_end` is called automatically at the end of scope.
	 * @return input scope guard object, offering access to input functions
	 */
	NUKLEUS_NODISCARD event_input input_scoped() &
	{
		nk_input_begin(&m_ctx);
		return event_input(m_ctx, nk_input_end);
	}

	bool input_has_mouse_click(nk_buttons id) const
	{
		return nk_input_has_mouse_click(&m_ctx.input, id) == nk_true;
	}

	bool input_has_mouse_click_in_rect(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_has_mouse_click_in_rect(&m_ctx.input, id, bounds) == nk_true;
	}

	bool input_has_mouse_click_in_button_rect(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_has_mouse_click_in_button_rect(&m_ctx.input, id, bounds) == nk_true;
	}

	bool input_has_mouse_click_down_in_rect(nk_buttons id, rect<float> bounds, bool down) const
	{
		return nk_input_has_mouse_click_down_in_rect(&m_ctx.input, id, bounds, down) == nk_true;
	}

	bool input_is_mouse_click_in_rect(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_is_mouse_click_in_rect(&m_ctx.input, id, bounds) == nk_true;
	}

	bool input_is_mouse_click_down_in_rect(nk_buttons id, rect<float> bounds, bool down) const
	{
		return nk_input_is_mouse_click_down_in_rect(&m_ctx.input, id, bounds, down) == nk_true;
	}

	bool input_any_mouse_click_in_rect(rect<float> bounds) const
	{
		return nk_input_any_mouse_click_in_rect(&m_ctx.input, bounds) == nk_true;
	}

	bool input_is_mouse_prev_hovering_rect(rect<float> bounds) const
	{
		return nk_input_is_mouse_prev_hovering_rect(&m_ctx.input, bounds) == nk_true;
	}

	bool input_is_mouse_hovering_rect(rect<float> bounds) const
	{
		return nk_input_is_mouse_hovering_rect(&m_ctx.input, bounds) == nk_true;
	}

	bool input_mouse_clicked(nk_buttons id, rect<float> bounds) const
	{
		return nk_input_mouse_clicked(&m_ctx.input, id, bounds) == nk_true;
	}

	bool input_is_mouse_down(nk_buttons id) const
	{
		return nk_input_is_mouse_down(&m_ctx.input, id) == nk_true;
	}

	bool input_is_mouse_pressed(nk_buttons id) const
	{
		return nk_input_is_mouse_pressed(&m_ctx.input, id) == nk_true;
	}

	bool input_is_mouse_released(nk_buttons id) const
	{
		return nk_input_is_mouse_released(&m_ctx.input, id) == nk_true;
	}

	bool input_is_key_pressed(nk_keys key) const
	{
		return nk_input_is_key_pressed(&m_ctx.input, key) == nk_true;
	}

	bool input_is_key_released(nk_keys key) const
	{
		return nk_input_is_key_released(&m_ctx.input, key) == nk_true;
	}

	bool input_is_key_down(nk_keys key) const
	{
		return nk_input_is_key_down(&m_ctx.input, key) == nk_true;
	}

	/// @}

	/**
	 * @name Drawing
	 * This library was designed to be render backend agnostic so it does
	 * not draw anything to screen directly. Instead all drawn shapes, widgets
	 * are made of, are buffered into memory and make up a command queue.
	 * Each frame therefore fills the command buffer with draw commands
	 * that then need to be executed by the user and his own render backend.
	 * After that the command buffer needs to be cleared and a new frame can be
	 * started. It is probably important to note that the command buffer is the main
	 * drawing API and the optional vertex buffer API only takes this format and
	 * converts it into a hardware accessible format.
	 * To draw all draw commands accumulated over a frame you need your own render
	 * backend able to draw a number of 2D primitives. This includes at least
	 * filled and stroked rectangles, circles, text, lines, triangles and scissors.
	 * As soon as this criterion is met you can iterate over each draw command
	 * and execute each draw command in a interpreter like fashion.
	 *
	 * @{
	 */

	/**
	 * @brief get the list of Nuklear's drawing commands
	 * @return range object which supports C++11 range-based loops
	 * @details example use: `for (const nk_command& cmd : ctx.commands())`
	 */
	NUKLEUS_NODISCARD range<command_iterator> commands()
	{
		return {
			command_iterator(m_ctx, nk__begin(&m_ctx)),
			command_iterator(m_ctx, nullptr)
		};
	}

#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
	/**
	 * @brief Converts all internal draw commands into vertex draw commands and fills
	 * three buffers with vertexes, vertex draw commands and vertex indices. The vertex format
	 * as well as some other configuration values have to be configured by filling out a
	 * `nk_convert_config` struct. Requires `NK_INCLUDE_VERTEX_BUFFER_OUTPUT`.
	 * @param cmds previously initialized buffer to hold converted vertex draw commands
	 * @param vertices previously initialized buffer to hold all produced vertices
	 * @param elements previously initialized buffer to hold all produced vertex indices
	 * @param config filled out `nk_config` struct to configure the conversion process
	 * @return one of `nk_convert_result` error codes
	 */
	NUKLEUS_NODISCARD nk_convert_result convert(nk_buffer& cmds, nk_buffer& vertices, nk_buffer& elements, const nk_convert_config& config)
	{
		return static_cast<nk_convert_result>(nk_convert(&m_ctx, &cmds, &vertices, &elements, &config));
	}

	/**
	 * @copydoc convert(nk_buffer& cmds, nk_buffer& vertices, nk_buffer& elements, const nk_convert_config& config)
	 */
	NUKLEUS_NODISCARD nk_convert_result convert(buffer& cmds, buffer& vertices, buffer& elements, const nk_convert_config& config)
	{
		return convert(cmds.get(), vertices.get(), elements.get(), config);
	}

	/**
	 * @brief Get the list of Nuklear's vertex drawing commands. Requires `NK_INCLUDE_VERTEX_BUFFER_OUTPUT`.
	 * @param buf vertex draw command buffer filled previously by @ref convert
	 * @return range object which supports C++11 range-based loops
	 * @details example use: `for (const nk_draw_command& cmd : ctx.draw_commands(buf))`
	 */
	NUKLEUS_NODISCARD range<draw_command_iterator> draw_commands(const nk_buffer& buf) const
	{
		return {
			draw_command_iterator(m_ctx, buf, nk__draw_begin(&m_ctx, &buf)),
			draw_command_iterator(m_ctx, buf, nullptr)
		};
	}

	/**
	 * @copydoc draw_commands(const nk_buffer& buf) const
	 */
	NUKLEUS_NODISCARD range<draw_command_iterator> draw_commands(const buffer& buf) const
	{
		return draw_commands(buf.get());
	}
#endif

	/// @}

	/**
	 * @name Window
	 * Create and manage windows. Some additional functions are also available in this class
	 * because they are not tied to the currently processed window.
	 * @{
	 */

	/**
	 * @brief Create a window scope guard that does not manage any window.
	 * @return Blank window object.
	 * @details Normally you should not need to use this function.
	 */
	NUKLEUS_NODISCARD window window_blank()
	{
		return window::create_blank(m_ctx);
	}

	/**
	 * @brief Start a new window; needs to be called every frame for every
	 * window (unless hidden) or otherwise the window gets removed.
	 * @param title Window title and identifier. Needs to be persistent over frames to identify the window.
	 * @param bounds Initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame.
	 * @param flags Window flags defined in the nk_panel_flags section with a number of different window behaviors.
	 * @return Scope guard for the window, that should be immediately checked.
	 * @details Example use: `if (auto window = ctx.window_scoped(...); window)`.
	 */
	NUKLEUS_NODISCARD window window_scoped(const char* title, rect<float> bounds, nk_flags flags)
	{
		return window::create(m_ctx, nk_begin(&m_ctx, title, bounds, flags) == nk_true);
	}

	/**
	 * @brief Extended window start with separated title and identifier to allow multiple windows with same title but not name.
	 * @param name Window identifier. Needs to be persistent over frames to identify the window.
	 * @param title Window title displayed inside header if flag `NK_WINDOW_TITLE` or either `NK_WINDOW_CLOSABLE` or `NK_WINDOW_MINIMIZED` was set.
	 * @param bounds Initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame.
	 * @param flags Window flags defined in the nk_panel_flags section with a number of different window behaviors.
	 * @return Scope guard for the window, that should be immediately checked.
	 * @details Example use: `if (auto window = ctx.window_titled_scoped(...); window)`.
	 */
	NUKLEUS_NODISCARD window window_titled_scoped(const char* name, const char* title, rect<float> bounds, nk_flags flags)
	{
		return window::create(m_ctx, nk_begin_titled(&m_ctx, name, title, bounds, flags) == nk_true);
	}

	/**
	 * @copydoc window::window_find
	 */
	NUKLEUS_NODISCARD nk_window* window_find(const char* name) const
	{
		return nk_window_find(&m_ctx, name);
	}

	/**
	 * @copydoc window::window_is_collapsed
	 */
	NUKLEUS_NODISCARD bool window_is_collapsed(const char* name) const
	{
		return nk_window_is_collapsed(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_closed
	 */
	NUKLEUS_NODISCARD bool window_is_closed(const char* name) const
	{
		return nk_window_is_closed(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_hidden
	 */
	NUKLEUS_NODISCARD bool window_is_hidden(const char* name) const
	{
		return nk_window_is_hidden(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_active
	 */
	NUKLEUS_NODISCARD bool window_is_active(const char* name) const
	{
		return nk_window_is_active(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_any_hovered
	 */
	NUKLEUS_NODISCARD bool window_is_any_hovered() const
	{
		return nk_window_is_any_hovered(&m_ctx) == nk_true;
	}

	/**
	 * @copydoc window::item_is_any_active
	 */
	NUKLEUS_NODISCARD bool item_is_any_active() const
	{
		return nk_item_is_any_active(&m_ctx) == nk_true;
	}

	/**
	 * @copydoc window::window_set_bounds
	 */
	void window_set_bounds(const char* name, rect<float> bounds)
	{
		nk_window_set_bounds(&m_ctx, name, bounds);
	}

	/**
	 * @copydoc window::window_set_position
	 */
	void window_set_position(const char* name, vec2<float> pos)
	{
		nk_window_set_position(&m_ctx, name, pos);
	}

	/**
	 * @copydoc window::window_set_size
	 */
	void window_set_size(const char* name, vec2<float> size)
	{
		nk_window_set_size(&m_ctx, name, size);
	}

	/**
	 * @copydoc window::window_set_focus
	 */
	void window_set_focus(const char* name)
	{
		nk_window_set_focus(&m_ctx, name);
	}

	/**
	 * @copydoc window::window_close
	 */
	void window_close(const char* name)
	{
		nk_window_close(&m_ctx, name);
	}

	/**
	 * @copydoc window::window_collapse
	 */
	void window_collapse(const char* name, nk_collapse_states state)
	{
		nk_window_collapse(&m_ctx, name, state);
	}

	/**
	 * @copydoc window::window_collapse_if
	 */
	void window_collapse_if(const char* name, nk_collapse_states state, int cond)
	{
		nk_window_collapse_if(&m_ctx, name, state, cond);
	}

	/**
	 * @copydoc window::window_show
	 */
	void window_show(const char* name, nk_show_states state)
	{
		nk_window_show(&m_ctx, name, state);
	}

	/**
	 * @copydoc window::window_show_if
	 */
	void window_show_if(const char* name, nk_show_states state, int cond)
	{
		nk_window_show_if(&m_ctx, name, state, cond);
	}

	/// @}

private:
	NUKLEUS_NODISCARD scoped_override_guard make_override_scoped(nk_bool push_result, nk_bool (*pop_func)(nk_context*))
	{
		return scoped_override_guard(m_ctx, push_result == nk_true ? pop_func : nullptr);
	}

	void check_style_pointer(const void* p) const
	{
		const auto min = static_cast<const void*>(&m_ctx);
		const auto max = static_cast<const void*>(&m_ctx + 1);
		NUKLEUS_ASSERT(min <= p);
		NUKLEUS_ASSERT(p < max);
	}

public:
	/**
	 * @name Style
	 * How scoped overrides work:
	 * - pass a pointer to an object within this class data
	 * - pass a value that should override pointed object
	 *
	 * Use `get_*` functions to obtain access to this data and to get their addresses.
	 * @{
	 */

	void style_default()
	{
		nk_style_default(&m_ctx);
	}

	void style_from_table(const color_table& table)
	{
		nk_style_from_table(&m_ctx, table.get());
	}

	void style_from_table(const nk_color (&table)[NK_COLOR_COUNT])
	{
		nk_style_from_table(&m_ctx, table);
	}

	void style_load_cursor(nk_style_cursor cursor, const nk_cursor& c)
	{
		nk_style_load_cursor(&m_ctx, cursor, &c);
	}

	void style_load_all_cursors(nk_cursor (&cursors)[NK_CURSOR_COUNT])
	{
		nk_style_load_all_cursors(&m_ctx, cursors);
	}

	NUKLEUS_NODISCARD static const char* style_get_color_name(nk_style_colors c)
	{
		return nk_style_get_color_by_name(c);
	}

	void style_set_font(const nk_user_font& font)
	{
		nk_style_set_font(&m_ctx, &font);
	}

	NUKLEUS_NODISCARD bool style_set_cursor(enum nk_style_cursor cursor)
	{
		return nk_style_set_cursor(&m_ctx, cursor) == nk_true;
	}

	void style_show_cursor()
	{
		nk_style_show_cursor(&m_ctx);
	}

	void style_hide_cursor()
	{
		nk_style_hide_cursor(&m_ctx);
	}

	NUKLEUS_NODISCARD scoped_override_guard style_font_scoped(const nk_user_font& font)
	{
		return make_override_scoped(nk_style_push_font(&m_ctx, &font), &nk_style_pop_font);
	}

	NUKLEUS_NODISCARD scoped_override_guard style_override_scoped(float* p, float value)
	{
		check_style_pointer(p);
		return make_override_scoped(nk_style_push_float(&m_ctx, p, value), &nk_style_pop_float);
	}

	NUKLEUS_NODISCARD scoped_override_guard style_override_scoped(struct nk_vec2* p, struct nk_vec2 value)
	{
		check_style_pointer(p);
		return make_override_scoped(nk_style_push_vec2(&m_ctx, p, value), &nk_style_pop_vec2);
	}

	NUKLEUS_NODISCARD scoped_override_guard style_override_scoped(nk_style_item* p, nk_style_item value)
	{
		check_style_pointer(p);
		return make_override_scoped(nk_style_push_style_item(&m_ctx, p, value), &nk_style_pop_style_item);
	}

	NUKLEUS_NODISCARD scoped_override_guard style_override_scoped(nk_flags* p, nk_flags value)
	{
		check_style_pointer(p);
		return make_override_scoped(nk_style_push_flags(&m_ctx, p, value), &nk_style_pop_flags);
	}

	NUKLEUS_NODISCARD scoped_override_guard style_override_scoped(nk_color* p, nk_color value)
	{
		check_style_pointer(p);
		return make_override_scoped(nk_style_push_color(&m_ctx, p, value), &nk_style_pop_color);
	}

	/**
	 * @brief override a style with style (pun intended) by providing pointers to members
	 * @tparam StyleType type of a `nk_style` member
	 * @tparam DataType type of a member field within @p StyleType
	 * @param style_ptr pointer-to-member of `nk_style` struct
	 * @param field_ptr pointer-to-member of `nk_style.*style_ptr` struct
	 * @param value value to override with (uses type_identity to block deduction which)
	 * @details example use:
	 * ```cpp
	 * auto style_override = ctx.style_override_scoped(&nk_style::window, &nk_style_window::spacing, {0, 0});
	 * ```
	 * @note @ref type_identity_t is used to block deduction for the value parameter.
	 * Because pointers already deduce very well, this allows to force an argument of certain type
	 * which in turn can enable some type convertions (e.g. from `nk::rect` to `nk_rect`).
	 * Otherwise passing `nk::rect` would result in  deduction conflict errors.
	 */
	template <typename StyleType, typename DataType>
	NUKLEUS_NODISCARD scoped_override_guard style_override_scoped(
		StyleType nk_style::* style_ptr,
		DataType StyleType::* field_ptr,
		type_identity_t<DataType> value)
	{
		return style_override_scoped(&(get_style().*style_ptr.*field_ptr), value);
	}

	/// @}

	/**
	 * @name Public fields of the context struct
	 * @{
	 */

	      nk_input& get_input()       { return m_ctx.input; }
	const nk_input& get_input() const { return m_ctx.input; }

	      nk_style& get_style()       { return m_ctx.style; }
	const nk_style& get_style() const { return m_ctx.style; }

	      nk_buffer& get_memory()       { return m_ctx.memory; }
	const nk_buffer& get_memory() const { return m_ctx.memory; }

	      nk_clipboard& get_clipboard()       { return m_ctx.clip; }
	const nk_clipboard& get_clipboard() const { return m_ctx.clip; }

	nk_flags& get_last_widget_state()       { return m_ctx.last_widget_state; }
	nk_flags  get_last_widget_state() const { return m_ctx.last_widget_state; }

	enum nk_button_behavior& get_button_behavior()       { return m_ctx.button_behavior; }
	enum nk_button_behavior  get_button_behavior() const { return m_ctx.button_behavior; }

	      nk_configuration_stacks& get_configuration_stacks()       { return m_ctx.stacks; }
	const nk_configuration_stacks& get_configuration_stacks() const { return m_ctx.stacks; }

	float& get_delta_time_seconds()       { return m_ctx.delta_time_seconds; }
	float  get_delta_time_seconds() const { return m_ctx.delta_time_seconds; }

	/// @}

private:
	context() = default;

	nk_context m_ctx = {};
	bool m_valid = false;
};

/// @} // core

#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
/**
 * @defgroup draw_list Draw List
 * @brief Draw List API, requires `NK_INCLUDE_VERTEX_BUFFER_OUTPUT`
 * @details The optional vertex buffer draw list provides a 2D drawing context
 * with antialiasing functionality which takes basic filled or outlined shapes
 * or a path and outputs vertexes, elements and draw commands.
 * The actual draw list API is not required to be used directly while using this
 * library since converting the default library draw command output is done by
 * just calling `nk_convert` but Nuklear decided to still make this library
 * accessible since it can be useful.
 *
 * The draw list is based on a path buffering and polygon and polyline
 * rendering API which allows a lot of ways to draw 2D content to screen.
 * In fact it is probably more powerful than needed but allows even more crazy
 * things than this library provides by default.
 * @{
 */
class draw_list
{
public:
	/**
	 * @name Construction and setup
	 * @{
	 */
	draw_list()
	{
		reset();
	}

	draw_list(const nk_convert_config& config, nk_buffer& cmds, nk_buffer& vertices, nk_buffer& elements, nk_anti_aliasing line_aa, nk_anti_aliasing shape_aa)
	{
		setup(config, cmds, vertices, elements, line_aa, shape_aa);
	}

	draw_list(const nk_convert_config& config, buffer& cmds, buffer& vertices, buffer& elements, nk_anti_aliasing line_aa, nk_anti_aliasing shape_aa)
	: draw_list(config, cmds.get(), vertices.get(), elements.get(), line_aa, shape_aa)
	{}

	void setup(const nk_convert_config& config, nk_buffer& cmds, nk_buffer& vertices, nk_buffer& elements, nk_anti_aliasing line_aa, nk_anti_aliasing shape_aa)
	{
		reset();
		nk_draw_list_setup(&m_draw_list, &config, &cmds, &vertices, &elements, line_aa, shape_aa);
	}

	void setup(const nk_convert_config& config, buffer& cmds, buffer& vertices, buffer& elements, nk_anti_aliasing line_aa, nk_anti_aliasing shape_aa)
	{
		setup(config, cmds.get(), vertices.get(), elements.get(), line_aa, shape_aa);
	}

	void reset()
	{
		nk_draw_list_init(&m_draw_list);
	}

	/// @}

	/**
	 * @name Iteration
	 * @{
	 */

	/**
	 * @brief Get the list of Nuklear's vertex drawing commands.
	 * @param buf vertex draw command buffer
	 * @return range object which supports C++11 range-based loops
	 * @details example use: `for (const nk_draw_command& cmd : list.draw_commands(buf))`
	 */
	NUKLEUS_NODISCARD range<draw_list_iterator> draw_commands(const nk_buffer& buf) const
	{
		return {
			draw_list_iterator(m_draw_list, buf, nk__draw_list_begin(&m_draw_list, &buf)),
			draw_list_iterator(m_draw_list, buf, nullptr)
		};
	}

	/**
	 * @copydoc draw_commands(const nk_buffer& buf) const
	 */
	NUKLEUS_NODISCARD range<draw_list_iterator> draw_commands(const buffer& buf) const
	{
		return draw_commands(buf.get());
	}

	/// @}

	/**
	 * @name Drawing - Path
	 * @{
	 */

	/**
	 * @brief reset path state
	 */
	void path_clear()
	{
		nk_draw_list_path_clear(&m_draw_list);
	}

	void path_line_to(vec2<float> pos)
	{
		nk_draw_list_path_line_to(&m_draw_list, pos);
	}

	void path_arc_to_fast(vec2<float> center, float radius, int a_min, int a_max)
	{
		nk_draw_list_path_arc_to_fast(&m_draw_list, center, radius, a_min, a_max);
	}

	void path_arc_to(vec2<float> center, float radius, float a_min, float a_max, unsigned segments)
	{
		nk_draw_list_path_arc_to(&m_draw_list, center, radius, a_min, a_max, segments);
	}

	void path_rect_to(vec2<float> a, vec2<float> b, float rounding)
	{
		nk_draw_list_path_rect_to(&m_draw_list, a, b, rounding);
	}

	void path_curve_to(vec2<float> p2, vec2<float> p3, vec2<float> p4, unsigned num_segments)
	{
		nk_draw_list_path_curve_to(&m_draw_list, p2, p3, p4, num_segments);
	}

	/**
	 * @brief finish drawing the path by filling it with specified color
	 * @param col the color to use for filling
	 */
	void path_fill(color col)
	{
		nk_draw_list_path_fill(&m_draw_list, col);
	}

	/**
	 * @brief finish drawing the path by stroking its outline
	 * @param col color of the path
	 * @param closed if true, path has a connection back to the beginning
	 * @param thickness line thickness
	 */
	void path_stroke(color col, bool closed, float thickness)
	{
		nk_draw_list_path_stroke(&m_draw_list, col, closed ? NK_STROKE_CLOSED : NK_STROKE_OPEN, thickness);
	}

	/// @}

	/**
	 * @name Drawing - Stroke
	 * @{
	 */

	void stroke_line(vec2<float> a, vec2<float> b, color col, float thickness)
	{
		nk_draw_list_stroke_line(&m_draw_list, a, b, col, thickness);
	}

	void stroke_rect(rect<float> r, color col, float rounding, float thickness)
	{
		nk_draw_list_stroke_rect(&m_draw_list, r, col, rounding, thickness);
	}

	void stroke_triangle(vec2<float> a, vec2<float> b, vec2<float> c, color col, float thickness)
	{
		nk_draw_list_stroke_triangle(&m_draw_list, a, b, c, col, thickness);
	}

	void stroke_circle(vec2<float> center, float radius, color col, unsigned num_segments, float thickness)
	{
		nk_draw_list_stroke_circle(&m_draw_list, center, radius, col, num_segments, thickness);
	}

	void stroke_curve(vec2<float> p0, vec2<float> cp0, vec2<float> cp1, vec2<float> p1, color col, unsigned num_segments, float thickness)
	{
		nk_draw_list_stroke_curve(&m_draw_list, p0, cp0, cp1, p1, col, num_segments, thickness);
	}

	void stroke_poly_line(span<const struct nk_vec2> points, color col, bool closed, float thickness, enum nk_anti_aliasing aa)
	{
		nk_draw_list_stroke_poly_line(&m_draw_list, points.data(), static_cast<unsigned>(points.size()), col, closed ? NK_STROKE_CLOSED : NK_STROKE_OPEN, thickness, aa);
	}


	/// @}

	/**
	 * @name Drawing - Fill
	 * @{
	 */

	void fill_rect(rect<float> r, color col, float rounding)
	{
		return nk_draw_list_fill_rect(&m_draw_list, r, col, rounding);
	}

	void fill_rect_multi_color(rect<float> r, color left, color top, color right, color bottom)
	{
		return nk_draw_list_fill_rect_multi_color(&m_draw_list, r, left, top, right, bottom);
	}

	void fill_triangle(vec2<float> a, vec2<float> b, vec2<float> c, color col)
	{
		return nk_draw_list_fill_triangle(&m_draw_list, a, b, c, col);
	}

	void fill_circle(vec2<float> center, float radius, color col, unsigned num_segments)
	{
		return nk_draw_list_fill_circle(&m_draw_list, center, radius, col, num_segments);
	}

	void fill_poly_convex(span<const struct nk_vec2> points, color col, nk_anti_aliasing aa)
	{
		return nk_draw_list_fill_poly_convex(&m_draw_list, points.data(), static_cast<unsigned>(points.size()), col, aa);
	}

	/// @}

	/**
	 * @name Drawing - Misc
	 * @{
	 */

	void add_image(image texture, rect<float> r, color col)
	{
		nk_draw_list_add_image(&m_draw_list, texture, r, col);
	}

	void add_text(const nk_user_font& font, rect<float> r, const char* text, int text_len, float font_height, color col)
	{
		nk_draw_list_add_text(&m_draw_list, &font, r, text, text_len, font_height, col);
	}

#ifdef NK_INCLUDE_COMMAND_USERDATA
	/**
	 * @brief set user data in the draw list, requires `NK_INCLUDE_COMMAND_USERDATA`
	 * @param userdata the handle to set
	 * @details this is more of a set than push (only 1 object can be assigned),
	 * thus the name differs from Nuklear
	 */
	void set_userdata(handle userdata)
	{
		nk_draw_list_push_userdata(&m_draw_list, userdata);
	}
#endif

	/// @}

	/**
	 * @name Access
	 * @{
	 */

	const nk_draw_list& get() const { return m_draw_list; }
	      nk_draw_list& get()       { return m_draw_list; }

	/// @}

private:
	nk_draw_list m_draw_list = {};
};

/// @} // draw_list
#endif // NK_INCLUDE_VERTEX_BUFFER_OUTPUT

/// @} // main

} // namespace nk
