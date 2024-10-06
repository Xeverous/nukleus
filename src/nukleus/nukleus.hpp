/**
 * @file nukleus.hpp
 * @brief Nukleus only file.
 */
#pragma once

// -------- compiler configuration --------

#if __cplusplus >= 201402L
#define NUKLEUS_CPP14_CONSTEXPR constexpr
#else
#define NUKLEUS_CPP14_CONSTEXPR
#endif

#if __cplusplus >= 201703L
#define NUKLEUS_CPP17_CONSTEXPR constexpr
#else
#define NUKLEUS_CPP17_CONSTEXPR
#endif

#if __cplusplus >= 202002L
#define NUKLEUS_CPP20_CONSTEXPR constexpr
#else
#define NUKLEUS_CPP20_CONSTEXPR
#endif

#if __cplusplus >= 202302L
#define NUKLEUS_CPP23_CONSTEXPR constexpr
#else
#define NUKLEUS_CPP23_CONSTEXPR
#endif

// -------- build configuration --------

/**
 * Default implementation for non-boolean build options for Nuklear.
 * Don't edit these, instead TODO CMake
 */

// ---- handled through CMake ----
// (not needed to be defined here)

// #define NK_PRIVATE

// stdlib options - only implementation (no changes in the API or behavior)
// #define NK_INCLUDE_FIXED_TYPES
// #define NK_INCLUDE_STANDARD_BOOL

// stdlib options - API additions
// #define NK_INCLUDE_DEFAULT_ALLOCATOR
// #define NK_INCLUDE_STANDARD_IO
// #define NK_INCLUDE_STANDARD_VARARGS

// font options
// #define NK_INCLUDE_FONT_BAKING
// #define NK_INCLUDE_DEFAULT_FONT

// rendering options
// #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
// #define NK_INCLUDE_COMMAND_USERDATA
// #define NK_UINT_DRAW_INDEX

// behavior options
// #define NK_BUTTON_TRIGGER_ON_RELEASE
// #define NK_KEYSTATE_BASED_INPUT
// #define NK_ZERO_COMMAND_MEMORY

// ---- implementation limits ----
// (all on default)

// #define NK_MAX_NUMBER_BUFFER
// #define NK_BUFFER_DEFAULT_INITIAL_SIZE
// #define NK_INPUT_MAX

// ---- implementation dependencies ----

// If you don't define this, nuklear will use assert().
// #define NK_ASSERT

// All of following options (if defined) should be only defined for the implementation mode.
#ifdef NK_IMPLEMENTATION

#include <string.h>

// You can define this to 'memset' or your own memset implementation replacement.
// If not nuklear will use its own version.
#ifndef NK_MEMSET
#define NK_MEMSET memset
#endif

// You can define this to 'memcpy' or your own memcpy implementation replacement.
// If not nuklear will use its own version.
#ifndef NK_MEMCPY
#define NK_MEMCPY memcpy
#endif

// You can define this to your own inverse sqrt implementation replacement.
// If not nuklear will use its own slow and not highly accurate version.
// #define NK_INV_SQRT

// You can define this to 'sinf' or your own sine implementation replacement.
// If not nuklear will use its own approximation implementation.
// #define NK_SIN

// You can define this to 'cosf' or your own cosine implementation replacement.
// If not nuklear will use its own approximation implementation.
// #define NK_COS

#include <stdlib.h>

// You can define this to strtod or your own string to double conversion implementation replacement.
// If not defined nuklear will use its own imprecise and possibly unsafe version (does not handle nan or infinity!).
// TODO const problem
// #ifndef NK_STRTOD
// #define NK_STRTOD strtod
// #endif

// You can define this to dtoa or your own double to string conversion implementation replacement.
// If not defined nuklear will use its own imprecise and possibly unsafe version (does not handle nan or infinity!).
// TODO implement the function with snprintf
// #ifndef NK_DTOA
// #define NK_DTOA dtoa
// #endif

// If you define NK_INCLUDE_STANDARD_VARARGS as well as NK_INCLUDE_STANDARD_IO and want to be safe define this to
// vsnprintf on compilers supporting later versions of C or C++. By default nuklear will check for your stdlib version
// in C as well as compiler version in C++. if vsnprintf is available it will define it to vsnprintf directly.
// If not defined and if you have older versions of C or C++ it will be defined to vsprintf which is unsafe.
#ifndef NK_VSNPRINTF
#define NK_VSNPRINTF vsnprintf
#endif

#endif // NK_IMPLEMENTATION

// Silence some warnings from nuklear.
// The library implementation seems to not care about few minor ones.
#if defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wfloat-equal"
	#pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__GNUG__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wfloat-equal"
	#pragma GCC diagnostic ignored "-Wsign-conversion"
	#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#endif

#include <nuklear.h>

#if defined(__clang__)
	#pragma clang diagnostic pop
#elif defined(__GNUG__)
	#pragma GCC diagnostic pop
#endif

/*
TODO possible improvements:
- deduplicate docs for function parameters with same name
- document each blank function that should not be used
- LIST VIEW (3089): nk_list_view_begin, nk_list_view_end - not implemented - no docs and no examples
- COMBOBOX - unclear overloads, const issues
- IMAGE, 9-SLICE - unclear what the API does
- nk_flags - an alias for uint, not type safe
- think whether it would make sense to implement point+extent (even further from nk math types)
- UTF-8 - undocumented
*/

namespace nk {

// -------- type traits --------

/**
 * @defgroup type_traits Type Traits
 * @brief Implementation of some traits to avoid including \<type_traits\>.
 * @{
 */

// ---- detection traits ----

/**
 * @defgroup detection_traits Detection Traits
 * @{
 */

struct true_type  { static constexpr bool value = true; };
struct false_type { static constexpr bool value = false; };

template <typename T, typename U> struct is_same       : false_type {};
template <typename T>             struct is_same<T, T> : true_type {};
template <typename T, typename U> constexpr bool is_same_v = is_same<T, U>::value;

template <typename T> struct is_pointer      : false_type {};
template <typename T> struct is_pointer<T*>  : true_type {};
template <typename T> constexpr bool is_pointer_v = is_pointer<T>::value;

template <typename>   struct is_lvalue_reference     : false_type {};
template <typename T> struct is_lvalue_reference<T&> : true_type {};
template <typename T> constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <typename>   struct is_rvalue_reference      : false_type {};
template <typename T> struct is_rvalue_reference<T&&> : true_type {};
template <typename T> constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <typename T> struct is_reference      : false_type {};
template <typename T> struct is_reference<T&>  : true_type {};
template <typename T> struct is_reference<T&&> : true_type {};
template <typename T> constexpr bool is_reference_v = is_reference<T>::value;

template <typename>   struct is_const          : false_type {};
template <typename T> struct is_const<const T> : true_type {};
template <typename T> constexpr bool is_const_v = is_const<T>::value;

template <typename>   struct is_volatile             : false_type {};
template <typename T> struct is_volatile<volatile T> : true_type {};
template <typename T> constexpr bool is_volatile_v = is_volatile<T>::value;

///@}

// ---- transformation traits ----

/**
 * @defgroup transformation_traits Transformation Traits
 * @{
 */

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

///@}

// ---- meta traits ----

/**
 * @defgroup meta_traits Meta Traits
 * @{
 */

template <bool B, typename T = void> struct enable_if {};
template <typename T>                struct enable_if<true, T> { using type = T; };
template <bool B, typename T = void> using enable_if_t = typename enable_if<B, T>::type;

template <typename... Ts> struct void_t_impl { using type = void; };
template <typename... Ts> using void_t = typename void_t_impl<Ts...>::type;

///@}

///@}

// -------- utility --------

/**
 * @defgroup utility Utility functions
 * @brief Implementation of some functions from \<utility\>.
 * @{
 */

template <typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept
{
	return static_cast<remove_reference_t<T>&&>(t);
}

template<typename T>
constexpr T&& forward(remove_reference_t<T>& t) noexcept
{
	return static_cast<T&&>(t);
}

template<typename T>
constexpr T&& forward(remove_reference_t<T>&& t) noexcept
{
	static_assert(!is_lvalue_reference_v<T>, "forward must not be used to convert an rvalue to an lvalue");
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
	output_bool(bool& b)
	: m_b(b)
	{}

	operator nk_bool*()
	{
		return &m_b;
	}

	bool& m_b;
#else // nk_bool is int
	output_bool(bool& b)
	: m_b(b)
	, m_nk_b(b)
	{}

	~output_bool()
	{
		m_b = m_nk_b == nk_true;
	}

	operator nk_bool*()
	{
		return &m_nk_b;
	}

	bool& m_b;
	nk_bool m_nk_b;
#endif
};

}

///@}

// -------- string utilities --------

/**
 * @defgroup str_utility String utility functions
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
 * @return <0 if s1 < s2, 0 is s1 == s2, >0 if s1 > s2
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
 * @return value or 0 if convertion failed
 * @details this function skips second `nk_strtoi` parameter due to API stability issue caused by const
 */
inline int strtoi(const char* str) { return nk_strtoi(str, nullptr); }

/**
 * @brief string to float
 * @param str string to convert
 * @return value or 0 if convertion failed
 * @details this function skips second `nk_strtof` parameter due to API stability issue caused by const
 */
inline float strtof(const char* str) { return nk_strtof(str, nullptr); }

#ifndef NK_STRTOD
/**
 * @brief string to double
 * @param str string to convert
 * @return value or 0 if convertion failed
 * @details this function skips second `nk_strtod` parameter due to API stability issue caused by const
 */
inline double strtod(const char* str) { return nk_strtod(str, nullptr); }
#endif

// TODO this is undocumented (implements very narrow set of regex operators)
// strfilter(const char* text, const char* regexp);

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
 * @param str_len length of @p str
 * @copydoc strmatch_fuzzy_string
 */
inline bool strmatch_fuzzy_text(const char* str, int str_len, const char* pattern, int* out_score = nullptr)
{
	return nk_strmatch_fuzzy_text(str, str_len, pattern, out_score) == nk_true;
}

///@}

// -------- basic types --------

/**
 * @defgroup basic_type Basic Types
 * @brief Additional types to complement Nuklear.
 * @details Nukleus types are implicitly convertible to/from Nuklear types if possible.
 * @{
 */

using byte = nk_byte;
using hash = nk_hash;

inline hash murmur_hash(const void* data, int len, hash seed)
{
	return nk_murmur_hash(data, len, seed);
}

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
	static color from_rgb(int r, int g, int b) { return nk_rgb(r, g, b); }
	static color from_rgb(nk_byte r, nk_byte g, nk_byte b) { return nk_color{r, g, b, static_cast<nk_byte>(255)}; }
	/// @brief clamps values in range [0, 255]; [rgb, rgb+3) must be a valid range
	static color from_rgb(const int* rgb) { return nk_rgb_iv(rgb); }
	/// @brief [rgb, rgb+3) must be a valid range
	static color from_rgb(const nk_byte* rgb) { return nk_rgb_bv(rgb); }

	///@}

	/**
	 * @name from RGB (float)
	 * @{
	 */

	/// @brief clamps values in range [0, 1.0]
	static color from_rgb(float r, float g, float b) { return nk_rgb_f(r, g, b); }
	/// @brief clamps values in range [0, 1.0]; [rgb, rgb+3) must be a valid range
	static color from_rgb(const float* rgb) { return nk_rgb_fv(rgb); }

	///@}

	/**
	 * @name from RGBA (int)
	 * @{
	 */

	/// @brief clamps values in range [0, 255]
	static color from_rgba(int r, int g, int b, int a) { return nk_rgba(r, g, b, a); }
	static color from_rgba(nk_byte r, nk_byte g, nk_byte b, nk_byte a) { return nk_color{r, g, b, a}; }
	/// @brief clamps values in range [0, 255]; [rgba, rgba+4) must be a valid range
	static color from_rgba(const int* rgba) { return nk_rgba_iv(rgba); }
	/// @brief [rgba, rgba+4) must be a valid range
	static color from_rgba(const nk_byte* rgba) { return nk_rgba_bv(rgba); }

	static color from_rgba(nk_uint rgba) { return nk_rgba_u32(rgba); }
	/// @brief multiply other color by a factor, alpha unchanged
	static color from_factor(nk_color col, float factor) { return nk_rgb_factor(col, factor); }

	///@}

	/**
	 * @name from RGBA (float)
	 * @{
	 */

	/// @brief clamps values in valid range [0, 1.0]
	static color from_rgba(float r, float g, float b, float a) { return nk_rgba_f(r, g, b, a); }
	/// @brief clamps values in range [0, 1.0]; [rgba, rgba+4) must be a valid range
	static color from_rgba(const float* rgba) { return nk_rgba_fv(rgba); }

	///@}

	/**
	 * @name from HSV (int)
	 * @{
	 */

	/// @brief clamps values in range [0, 255]
	static color from_hsv(int h, int s, int v) { return nk_hsv(h, s, v); }
	static color from_hsv(nk_byte h, nk_byte s, nk_byte v) { return nk_hsva_f(h / 255.0f, s / 255.0f, v / 255.0f, 1.0f); }
	/// @brief clamps values in range [0, 255]; [hsv, hsv+3) must be a valid range
	static color from_hsv(const int* hsv) { return nk_hsv_iv(hsv); }
	/// @brief [hsv, hsv+3) must be a valid range
	static color from_hsv(const nk_byte* hsv) { return nk_hsv_bv(hsv); }

	///@}

	/**
	 * @name from HSV (float)
	 * @{
	 */

	static color from_hsv(float h, float s, float v) { return nk_hsv_f(h, s, v); }
	/// @brief [hsv, hsv+3) must be a valid range
	static color from_hsv(const float* hsv) { return nk_hsv_fv(hsv); }

	///@}

	/**
	 * @name from HSVA (int)
	 * @{
	 */

	/// @brief clamps values in range [0, 255]
	static color from_hsva(int h, int s, int v, int a) { return nk_hsva(h, s, v, a); }
	static color from_hsva(nk_byte h, nk_byte s, nk_byte v, nk_byte a) { return nk_hsva_f(h / 255.0f, s / 255.0f, v / 255.0f, a / 255.0f); }
	/// @brief clamps values in range [0, 255]; [hsv, hsv+4) must be a valid range
	static color from_hsva(const int* hsva) { return nk_hsva_iv(hsva); }
	/// @brief [hsva, hsva+4) must be a valid range
	static color from_hsva(const nk_byte* hsva) { return nk_hsva_bv(hsva); }

	///@}

	/**
	 * @name from HSVA (float)
	 * @{
	 */

	static color from_hsva(float h, float s, float v, float a) { return nk_hsva_f(h, s, v, a); }
	/// @brief [hsva, hsva+4) must be a valid range
	static color from_hsva(const float* hsva) { return nk_hsva_fv(hsva); }

	///@}

	/**
	 * @name from HEX
	 * @{
	 */

	/// @brief string should have the form "#RRGGBB" or "RRGGBB"
	static color from_rgb_hex(const char* rgb) { return nk_rgb_hex(rgb); }
	/// @brief string should have the form "#RRGGBBAA" or "RRGGBBAA"
	static color from_rgba_hex(const char* rgba) { return nk_rgba_hex(rgba); }

	///@}

	/**
	 * @name to HEX
	 * @{
	 */

	/// @brief [output, output+7) must be a valid range
	void to_rgb_hex(char* output) const { nk_color_hex_rgb(output, *this); }
	/// @brief [output, output+9) must be a valid range
	void to_rgba_hex(char* output) const { nk_color_hex_rgba(output, *this); }

	///@}

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

	static colorf from_hsva(float h, float s, float v, float a) { return nk_hsva_colorf(h, s, v, a); }
	static colorf from_hsva(const float* hsva) { return nk_hsva_colorf(hsva[0], hsva[1], hsva[2], hsva[3]); } // should be nk_hsva_colorfv but it has a const issue

	///@}

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

using color_format = nk_color_format;
using image = struct nk_image;
using symbol_type = nk_symbol_type;
using heading = nk_heading;

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

	template <enable_if<is_same_v<T, float>>* = nullptr>
	vec2(struct nk_vec2 v)
	: x(v.x), y(v.y)
	{}

	template <enable_if<is_same_v<T, short>>* = nullptr>
	vec2(struct nk_vec2i v)
	: x(v.x), y(v.y)
	{}

	template <enable_if<is_same_v<T, float>>* = nullptr>
	operator struct nk_vec2() const { return {x, y}; }

	template <enable_if<is_same_v<T, short>>* = nullptr>
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

	template <enable_if<is_same_v<T, float>>* = nullptr>
	rect(struct nk_rect r)
	: x(r.x), y(r.y), w(r.w), h(r.h)
	{}

	template <enable_if<is_same_v<T, short>>* = nullptr>
	rect(struct nk_recti r)
	: x(r.x), y(r.y), w(r.w), h(r.h)
	{}

	vec2<T> pos() const { return {x, y}; }
	vec2<T> size() const { return {w, h}; }

	template <enable_if<is_same_v<T, float>>* = nullptr>
	operator struct nk_rect() const { return {x, y, w, h}; }

	template <enable_if<is_same_v<T, short>>* = nullptr>
	operator struct nk_recti() const { return {x, y, w, h}; }

	T x{};
	T y{};
	T w{};
	T h{};
};

/* TODO this actually returns 3 vec2
inline vec2<float> triangle_from_direction(rect<float> r, float pad_x, float pad_y, heading direction)
{
	struct nk_vec2 result[3];
	nk_triangle_from_direction(&result, r, pad_x, pad_y, direction);
	return result;
}
*/

using widget_layout_states = nk_widget_layout_states;
using buttons = nk_buttons;
using text_align = nk_text_align;
using style_button = nk_style_button;
using button_behavior = enum nk_button_behavior;

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
		NK_ASSERT(color_index < NK_COLOR_COUNT);
		NK_ASSERT(color_index >= 0);
		return m_table[color_index];
	}

	nk_color operator[](nk_style_colors color_index) const
	{
		NK_ASSERT(color_index < NK_COLOR_COUNT);
		NK_ASSERT(color_index >= 0);
		return m_table[color_index];
	}

	      nk_color* get()       { return m_table; }
	const nk_color* get() const { return m_table; }

private:
	nk_color m_table[NK_COLOR_COUNT] = {};
};

///@}

// -------- font handling --------

/**
 * @defgroup font_handling Font Handling
 * @details Font handling in this library was designed to be quite customizable and let
 * you decide what you want to use and what you want to provide. There are three
 * different ways to use the font atlas:
 * 1. (simplest) your font handling scheme. Requires only essential data.
 * 2. Vertex buffer output. Requires only essential data + `NK_INCLUDE_VERTEX_BUFFER_OUTPUT`.
 * 3. (most complex) Nuklear's font baking API. Requires `NK_INCLUDE_FONT_BAKING`.
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
 * nk_font* font = atlas.add_from_file("Path/To/Your/TTF_Font.ttf", 13);
 * nk_font* font2 = atlas.add_from_file("Path/To/Your/TTF_Font2.ttf", 16);
 * nk::v
 * const void* img = atlas.bake(&img_width, &img_height, NK_FONT_ATLAS_RGBA32);
 * atlas.end(nk_handle_id(texture), 0);
 *
 * auto ctx = nk::context::init_default(font->handle);
 * ```
 *
 * The font baker API is probably the most complex API inside this library and
 * Nuklear suggests reading examples in the `example/` directory to get a grip on how
 * to use the font atlas. There is a number of details which are left out. For example:
 * how to merge fonts, configure a font with `nk_font_config` to use other languages,
 * use another texture coordinate format and a lot more:
 *
 * ```cpp
 * struct nk_font_config cfg = nk_font_config(font_pixel_height);
 * cfg.merge_mode = nk_false or nk_true;
 * cfg.range = nk_font_korean_glyph_ranges();
 * cfg.coord_type = NK_COORD_PIXEL;
 * nk_font* font = atlas.add_from_file("Path/To/Your/TTF_Font.ttf", 13, &cfg);
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

	      struct nk_font_config& get_config()       { return m_config; }
	const struct nk_font_config& get_config() const { return m_config; }

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
	static font_atlas init_default()
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
	static font_atlas init(const nk_allocator& allocator)
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
	static font_atlas init_custom(const nk_allocator& persistent, const nk_allocator& transient)
	{
		font_atlas atlas;
		nk_font_atlas_init_custom(&atlas.m_atlas, &persistent, &transient);
		atlas.m_initialized = true;
		return atlas;
	}

	///@}

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

	[[nodiscard]] nk_font* add(const struct nk_font_config& config) // TODO needs documentation
	{
		return nk_font_atlas_add(&m_atlas, &config); // (config can not be null here)
	}

	[[nodiscard]] nk_font* add(const font_config& config) // TODO needs documentation
	{
		return add(config.get_config());
	}

#ifdef NK_INCLUDE_DEFAULT_FONT
	/**
	 * @brief Add Nuklear's built-in ProggyClean font. Requires `NK_INCLUDE_DEFAULT_FONT`.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	[[nodiscard]] nk_font* add_default(float height, const struct nk_font_config* config = nullptr)
	{
		return nk_font_atlas_add_default(&m_atlas, height, config);
	}

	/**
	 * @copydoc add_default(float, const struct nk_font_config*)
	 */
	[[nodiscard]] nk_font* add_default(float height, const font_config& config)
	{
		return add_default(height, &config.get_config());
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
	[[nodiscard]] nk_font* add_from_memory(const void* memory, nk_size size, float height, const struct nk_font_config* config = nullptr)
	{
		// const_cast is fine: the library never modifies the data (actually copies it) - API const issue?
		return nk_font_atlas_add_from_memory(&m_atlas, const_cast<void*>(memory), size, height, config);
	}

	/**
	 * @copydoc add_from_memory(const void*, nk_size, float, const struct nk_font_config*)
	 */
	[[nodiscard]] nk_font* add_from_memory(const void* memory, nk_size size, float height, const font_config& config)
	{
		return add_from_memory(memory, size, height, &config.get_config());
	}

	/**
	 * @brief Add a compressed font from memory.
	 * @param memory Address of the memory block.
	 * @param size length of the memory block.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	[[nodiscard]] nk_font* add_compressed(const void* memory, nk_size size, float height, const struct nk_font_config* config = nullptr)
	{
		// const_cast is fine: the library never modifies the data (actually copies it) - API const issue?
		return nk_font_atlas_add_compressed(&m_atlas, const_cast<void*>(memory), size, height, config);
	}

	/**
	 * @copydoc add_compressed(const void*, nk_size, float, const struct nk_font_config*)
	 */
	[[nodiscard]] nk_font* add_compressed(const void* memory, nk_size size, float height, const font_config& config)
	{
		return add_compressed(memory, size, height, &config.get_config());
	}

	/**
	 * @brief Add a compressed font from memory which is base-85 encoded on top.
	 * @param data Null-terminated base-85 string.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	[[nodiscard]] nk_font* add_compressed_base85(const char* data, float height, const struct nk_font_config* config = nullptr)
	{
		return nk_font_atlas_add_compressed_base85(&m_atlas, data, height, config);
	}

	/**
	 * @copydoc add_compressed_base85(const char*, float, const struct nk_font_config*)
	 */
	[[nodiscard]] nk_font* add_compressed_base85(const char* data, float height, const font_config& config)
	{
		return add_compressed_base85(data, height, &config.get_config());
	}

#ifdef NK_INCLUDE_STANDARD_IO
	/**
	 * @brief Add a font from file. Requires `NK_INCLUDE_STANDARD_IO`.
	 * @param file_path Path to the file with font data.
	 * @param height Height of the font in pixels. Overrides configuration's size.
	 * @param config Optional configuration. If null, @p height will be used to generate a default one.
	 * @return Pointer to font object, managed by the atlas.
	 */
	[[nodiscard]] nk_font* add_from_file(const char* file_path, float height, const struct nk_font_config* config = nullptr)
	{
		return nk_font_atlas_add_from_file(&m_atlas, file_path, height, config);
	}

	/**
	 * @copydoc add_from_file(const char*, float, const struct nk_font_config*)
	 */
	[[nodiscard]] nk_font* add_from_file(const char* file_path, float height, const font_config& config)
	{
		return add_from_file(file_path, height, &config.get_config());
	}
#endif

	/**
	 * @brief Perform the baking process.
	 * @param dimentions Resulting image dimentions.
	 * @param fmt One of available atlas formats.
	 * @return Pointer to resulting image.
	 * @attention This function must be called between @ref begin and @ref end.
	 */
	[[nodiscard]] const void* bake(vec2<int>& dimentions, nk_font_atlas_format fmt)
	{
		return nk_font_atlas_bake(&m_atlas, &dimentions.x, &dimentions.y, fmt);
	}

	void end(nk_handle texture, struct nk_draw_null_texture* texture_null = nullptr) // TODO needs documentation
	{
		nk_font_atlas_end(&m_atlas, texture, texture_null);
	}

	/**
	 * @brief Free any resources that were allocated for the baking process. Can be called after @ref end.
	 */
	void cleanup()
	{
		nk_font_atlas_cleanup(&m_atlas);
	}

	///@}

private:
	font_atlas() = default;

	nk_font_atlas m_atlas = {};
	bool m_initialized = false;
};

#endif // NK_INCLUDE_FONT_BAKING

///@}

// -------- memory buffer --------

/**
 * @defgroup memory Raw Memory Buffer
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

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
	static buffer init_default()
	{
		buffer buf;
		nk_buffer_init_default(&buf.m_buffer);
		buf.m_initialized = true;
		return buf;
	}
#endif

	static buffer init(const nk_allocator& alloc, nk_size size)
	{
		buffer buf;
		nk_buffer_init(&buf.m_buffer, &alloc, size);
		buf.m_initialized = true;
		return buf;
	}

	static buffer init_fixed(void* memory, nk_size size)
	{
		buffer buf;
		nk_buffer_init_fixed(&buf.m_buffer, memory, size);
		buf.m_initialized = true;
		return buf;
	}

	///@}

	/**
	 * @name Buffer Management
	 * @{
	 */

	nk_memory_status info() // TODO const qualifier
	{
		nk_memory_status status;
		nk_buffer_info(&status, &m_buffer);
		return status;
	}

	nk_size total() // TODO const qualifier
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

	void free()
	{
		if (!m_initialized)
			return;

		nk_buffer_free(&m_buffer);
		m_initialized = false;
	}

	///@}

	/**
	 * @name Access
	 * @{
	 */

	      void* memory()       { return nk_buffer_memory(&m_buffer); }
	const void* memory() const { return nk_buffer_memory_const(&m_buffer); }

	      nk_buffer& get_buffer()       { return m_buffer; }
	const nk_buffer& get_buffer() const { return m_buffer; }

	///@}

private:
	buffer() = default;

	nk_buffer m_buffer = {};
	bool m_initialized = false;
};

///@}

// -------- iterators/ranges --------

/**
 * @defgroup iterators_ranges Iterators and Ranges
 * @{
 */

// ---- iterators ----

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
	draw_command_iterator(const nk_context& ctx, const nk_buffer* buf, const nk_draw_command* cmd)
	: m_ctx(&ctx)
	, m_buf(buf)
	, m_cmd(cmd)
	{}

	const nk_draw_command& operator*() const noexcept
	{
		return *m_cmd;
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

// ---- ranges ----

template <typename Iterator>
class range
{
public:
	range(Iterator b, Iterator e)
	: m_begin(b)
	, m_end(e)
	{}

	Iterator begin() const
	{
		return m_begin;
	}

	Iterator end() const
	{
		return m_end;
	}

private:
	Iterator m_begin;
	Iterator m_end;
};

template <typename Iterator>
class span
{
public:
	span(Iterator b, int length)
	: m_begin(b)
	, m_size(length)
	{}

	Iterator begin() const
	{
		return m_begin;
	}

	Iterator end() const
	{
		return m_begin + m_size;
	}

	int size() const
	{
		return m_size;
	}

private:
	Iterator m_begin;
	int m_size;
};

///@}

// -------- helpers for object lifetime management --------

/**
 * @defgroup lifetime RAII support
 * @brief Types that implement support for scoped begin/end calls.
 * @{
 */

// Added bool invoke to constructor and factory function to be able to
// immediately cancel actions (useful for negative returns in Nuklear).

/**
 * @brief Based on Core Guidelines `gsl::final_action`.
 * @tparam F function object type
 */
template <typename F>
class scoped_action
{
public:
	static_assert(
		!is_reference_v<F> && !is_const_v<F> && !is_volatile_v<F>,
		"scoped_action should store its callable by value");

	explicit scoped_action(F f, bool invoke = true) noexcept
	: m_f(move(f))
	, m_invoke(invoke)
	{}

	scoped_action(scoped_action&& other) noexcept
	: m_f(move(other.m_f))
	, m_invoke(exchange(other.m_invoke, false))
	{}

	// For some reason, gsl::final_action deletes move assignment.
	// Perhaps scope guards should never be reset and always perform their action at the end of scope.
	scoped_action& operator=(scoped_action&&) noexcept = delete;

	scoped_action(const scoped_action&) = delete;
	scoped_action& operator=(const scoped_action&) = delete;

	~scoped_action()
	{
		if (m_invoke)
			m_f();
	}

private:
	F m_f;
	bool m_invoke = true;
};

/**
 * @brief Adds bool state and a convertion to bool to scoped_action.
 * This allows to hold returns from Nuklear for additional checking.
 * Example: `if (auto result = ctx.f(); result)`
 * @tparam F function object type
 */
template <typename F>
class scoped_action_with_bool : public scoped_action<F>
{
public:
	explicit scoped_action_with_bool(F f, bool state)
	: scoped_action<F>(move(f))
	, m_state(state)
	{}

	scoped_action_with_bool(scoped_action_with_bool&& other) noexcept
	: scoped_action<F>(move(other))
	, m_state(other.m_state)
	{}

	scoped_action_with_bool(const scoped_action_with_bool&) = delete;
	scoped_action_with_bool& operator=(const scoped_action_with_bool&) = delete;
	scoped_action_with_bool& operator=(scoped_action_with_bool&&) noexcept = delete;

	explicit operator bool() const noexcept
	{
		return m_state;
	}

private:
	bool m_state;
};

template <typename F>
[[nodiscard]] scoped_action<remove_cvref_t<F>>
make_scoped(F&& f, bool invoke = true) noexcept
{
	return scoped_action<remove_cvref_t<F>>(forward<F>(f), invoke);
}

template <typename F>
[[nodiscard]] scoped_action_with_bool<remove_cvref_t<F>>
make_scoped_with_bool(F&& f, bool state) noexcept
{
	return scoped_action_with_bool<remove_cvref_t<F>>(forward<F>(f), state);
}

/**
 * @brief Base class for implementing scope guards.
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
	bool is_scope_active() const
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
	[[nodiscard]] func_type* release()
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
		NK_ASSERT(m_func == nullptr);
		m_func = func;
	}

private:
	nk_context* m_ctx;
	func_type* m_func = nullptr;
};

/**
 * @brief default scope guard, just calls the function in dtor
 */
class scope_guard : public scope_guard_base<void (nk_context*)>
{
public:
	// bring 2-argument ctor
	using scope_guard_base::scope_guard_base;

	// required because explicit destructor disables move (rule of 5)
	scope_guard(const scope_guard&) = delete;
	scope_guard(scope_guard&&) noexcept = default;
	scope_guard& operator=(const scope_guard&) = delete;
	scope_guard& operator=(scope_guard&&) noexcept = delete;

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

	~scope_guard()
	{
		reset();
	}
};

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

	explicit operator bool() const
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
			NK_ASSERT(result == nk_true); // if pop fails, something must gone really wrong
		}
	}
};

///@}

// -------- library implementation --------

/**
 * @defgroup main Main library types
 * @brief Main contents of the library.
 * @details The library's core are simple classes that wrap underlying begin/end calls.
 * In Nuklear, pretty much every function uses the `nk_context` struct.
 * In Nukleus, they have been grouped by usage patterns and lifetime requirements.
 * For example, you can not call a widget or layout function without first starting a window.
 * For this reason, Nukleus does not bundle all functions in the context class.
 * Instead, each class (including context) returns scope guard objects that give access to specific functions.
 * Each scope object manages the lifetime of a specific entity (context, window, layout, etc.).
 * First create a context object, then a window object from it and then you can create widgets and layout from
 * the window object.
 *
 * This design:
 * - Nicely groups related functions together.
 * - Prevents calling specific functions when they should not be called.
 * - Automatically manages begin/end calls through RAII.
 *
 * Object creation tree:
 * - @ref context
 *   - @ref input
 *   - @ref window
 *     - @ref tree
 *     - @ref layout
 *       - @ref group
 *
 * Each class also gives an access to the underlying context object if you ever have a need to use Nuklear directly.
 *
 * @attention Normally you should not need to call constructors of any of these types except for context.
 * The constructors assume `nk_xxx_begin` was already called. Constructors are public for implementation reasons
 * and generally are not intended for public use.
 * After constructing a context object, you should create all descendant objects through dedicated functions.
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
 * Input state needs to be provided to nuklear by first calling `nk_input_begin`
 * which resets internal state like delta mouse position and button transitions.
 * After `nk_input_begin` all current input state needs to be provided. This includes
 * mouse motion, button and key pressed and released, text input and scrolling.
 * Both event- or state-based input handling are supported by this API
 * and should work without problems. Finally after all input state has been
 * mirrored `nk_input_end` needs to be called to finish input process.
 */
class input : public scope_guard
{
public:
	using scope_guard::scope_guard;

	/**
	 * @brief Mirrors current mouse position to nuklear
	 * @param x current mouse cursor x-position
	 * @param y current mouse cursor y-position
	 */
	void input_motion(int x, int y)
	{
		nk_input_motion(&get_context(), x, y);
	}

	/**
	 * @brief Mirrors the state of a specific key to nuklear
	 * @param key any value specified in enum nk_keys that needs to be mirrored
	 * @param down false for key being up and true for key being down
	 */
	void input_key(nk_keys key, bool down)
	{
		nk_input_key(&get_context(), key, down);
	}

	/**
	 * @brief Mirrors the state of a specific mouse button to nuklear
	 * @param button any value specified in enum nk_buttons that needs to be mirrored
	 * @param x mouse cursor x-position on click up/down
	 * @param y mouse cursor y-position on click up/down
	 * @param down false for key being up and true for key being down
	 */
	void input_button(nk_buttons button, int x, int y, bool down)
	{
		nk_input_button(&get_context(), button, x, y, down);
	}

	/**
	 * @brief Copies the last mouse scroll value to nuklear.
	 * @param val vector with both X- as well as Y-scroll value
	 */
	void input_scroll(vec2<float> val)
	{
		nk_input_scroll(&get_context(), val);
	}

	/**
	 * @brief Copies a single ASCII character into an internal text buffer
	 * This is basically a helper function to quickly push ASCII characters into nuklear.
	 * @param c an ASCII character, preferably one that can be printed
	 * @attention Stores up to NK_INPUT_MAX bytes between input_begin and input_end.
	 */
	void input_char(char c)
	{
		nk_input_char(&get_context(), c);
	}

	/**
	 * @brief Converts an encoded unicode rune into UTF-8 and copies the result into an
	 * internal text buffer.
	 * @param g UTF-32 unicode codepoint
	 * @attention Stores up to NK_INPUT_MAX bytes between input_begin and input_end.
	 */
	void input_glyph(const nk_glyph g)
	{
		nk_input_glyph(&get_context(), g);
	}

	/**
	 * @brief Converts a unicode rune into UTF-8 and copies the result
	 * into an internal text buffer.
	 * @param rune UTF-32 unicode codepoint
	 * @attention Stores up to NK_INPUT_MAX bytes between input_begin and input_end.
	 */
	void input_unicode(nk_rune rune)
	{
		nk_input_unicode(&get_context(), rune);
	}
};

/**
 * @brief Grouping API
 * @details Groups are basically windows inside windows. They allow to subdivide
 * space in a window to layout widgets as a group. Almost all more complex widget
 * layouting requirements can be solved using groups and basic layouting
 * fuctionality. Groups just like windows are identified by an unique name and
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

	// group(group&&) noexcept = default; // XXX

	/**
	 * @name Group creation
	 * @{
	 */

	/**
	 * @copydoc layout::blank_group
	 */
	[[nodiscard]] group blank_subgroup()
	{
		return group(get_context(), nullptr);
	}

	/**
	 * @brief Return whether this scope-widget should be processed.
	 * @return `true` if visible and fillable with widgets, otherwise `false`.
	 * @details Some scope widgets, unlike windows, need `nk_xxx_end` to be only called if the corresponding
	 * `nk_xxx_begin_xxx` call returns nk_true. Thus, the implementation here differs from the window class.
	 */
	explicit operator bool() const &
	{
		return is_scope_active();
	}

	/**
	 * @copydoc layout::scoped_group
	 */
	[[nodiscard]] group scoped_subgroup(const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin(&get_context(), title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @copydoc layout::scoped_group_titled
	 */
	[[nodiscard]] group scoped_subgroup_titled(const char* name, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin_titled(&get_context(), name, title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @copydoc layout::scoped_group_scrolled_offset
	 */
	[[nodiscard]] group scoped_subgroup_scrolled_offset(nk_uint& x_offset, nk_uint& y_offset, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_scrolled_offset_begin(
				&get_context(), &x_offset, &y_offset, title, flags
			) == nk_true ? &nk_group_scrolled_end : nullptr);
	}

	/**
	 * @copydoc layout::scoped_group_scrolled
	 */
	[[nodiscard]] group scoped_subgroup_scrolled(nk_scroll& off, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_scrolled_begin(
				&get_context(), &off, title, flags
			) == nk_true ? &nk_group_scrolled_end : nullptr);
	}

	///@}
};

/**
 * @brief Base class for layouts. Specific layouts (derived types) offer additional functions.
 * @details Example use:
 *     // first row with height: 25 composed of two widgets with width 60 and 40
 *     auto layout = ctx.scoped_layout_row(NK_STATIC, 25, 2);
 *     ctx.layout_row_push(60);
 *     ctx.widget(...);
 *     ctx.layout_row_push(40);
 *     ctx.widget(...);
 *
 *     // second row with height: 25 composed of two widgets with window ratio 0.25 and 0.75
 *     layout.scoped_layout_row(NK_DYNAMIC, 25, 2);
 *     ctx.layout_row_push(0.25f);
 *     ctx.widget(...);
 *     ctx.layout_row_push(0.75f);
 *     ctx.widget(...);
 *
 *     // third row with auto generated height: composed of two widgets with window ratio 0.25 and 0.75
 *     layout.scoped_layout_row(NK_DYNAMIC, 0, 2);
 *     ctx.layout_row_push(0.25f);
 *     ctx.widget(...);
 *     ctx.layout_row_push(0.75f);
 *     ctx.widget(...);
 *
 *     // static row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
 *     layout.scoped_layout_space(ctx, NK_STATIC, 500, INT_MAX);
 *     ctx.layout_space_push(ctx, nk_rect(0, 0, 150, 200));
 *     ctx.widget(...);
 *     ctx.layout_space_push(ctx, nk_rect(200, 200, 100, 200));
 *     ctx.widget(...);
 *
 *     // dynamic row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
 *     layout.scoped_layout_space(ctx, NK_DYNAMIC, 500, INT_MAX);
 *     ctx.layout_space_push(ctx, nk_rect(0.5, 0.5, 0.1, 0.1));
 *     ctx.widget(...);
 *     ctx.layout_space_push(ctx, nk_rect(0.7, 0.6, 0.1, 0.1));
 *     ctx.widget(...);
 */
class layout : public scope_guard
{
public:
	using scope_guard::scope_guard;

	/**
	 * @name Group API
	 * Make a group. All groups requires a previous layouting function to specify a pos/size.
	 * TODO better requirement description?
	 * @{
	 */

	/**
	 * @brief Create a group scope guard to gain access to group's API.
	 * @return A group guard that does not manage a group.
	 * @details Normally you should not need to use this function.
	 */
	group blank_group()
	{
		return group(get_context(), nullptr);
	}

	/**
	 * @brief Start a new widget group. Requires a previous layouting function to specify a pos/size.
	 * @param title Unique group title used to both identify and display in the group header.
	 * @param flags Window flags defined in the nk_panel_flags section with a number of different group behaviors.
	 * @return scope guard that should be immediately checked
	 */
	group scoped_group(const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin(&get_context(), title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @copybrief layout::scoped_group
	 * @param name Unique identifier for this group.
	 * @param title Group header title.
	 * @param flags Window flags defined in the nk_panel_flags section with a number of different group behaviors.
	 * @return scope guard that should be immediately checked
	 */
	group scoped_group_titled(const char* name, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_begin_titled(&get_context(), name, title, flags) == nk_true ? &nk_group_end : nullptr);
	}

	/**
	 * @brief Start a new widget group. Requires a previous layouting function to specify
	 * a size. Does not keep track of scrollbar. TODO lifetime requirements? Scrollbar externally?
	 * @param x_offset Scrollbar x-offset to offset all widgets inside the group horizontally.
	 * @param y_offset Scrollbar y-offset to offset all widgets inside the group vertically.
	 * @param title Unique group title used to both identify and display in the group header.
	 * @param flags Window flags from the nk_panel_flags section
	 * @return scope guard that should be immediately checked
	 */
	group scoped_group_scrolled_offset(nk_uint& x_offset, nk_uint& y_offset, const char* title, nk_flags flags) &
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
	group scoped_group_scrolled(nk_scroll& off, const char* title, nk_flags flags) &
	{
		return group(
			get_context(),
			nk_group_scrolled_begin(
				&get_context(), &off, title, flags
			) == nk_true ? &nk_group_scrolled_end : nullptr);
	}

	///@}
};

class layout_row : public layout
{
public:
	explicit layout_row(nk_context& ctx)
	: layout(ctx, &nk_layout_row_end)
	{}

	/**
	 * @brief Specifies either window ratio or width of a single column.
	 * @param value Either a window ratio or fixed width depending on `fmt` in previous scoped_row call.
	 */
	void row_push(float value) &
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
	[[nodiscard]] rect<float> bounds()
	{
		return nk_layout_space_bounds(&get_context());
	}

	/**
	 * @brief Convert vector from nk_layout_space coordinate space into screen space.
	 * @param vec Position to convert from layout space into screen coordinate space.
	 * @return Screen space coordinates.
	 */
	[[nodiscard]] vec2<float> space_to_screen(vec2<float> vec)
	{
		return nk_layout_space_to_screen(&get_context(), vec);
	}

	/**
	 * @brief Convert vector from screen space into layout space.
	 * @param vec Position to convert from screen space into layout coordinate space.
	 * @return Layout space coordinates.
	 */
	[[nodiscard]] vec2<float> space_to_local(vec2<float> vec)
	{
		return nk_layout_space_to_local(&get_context(), vec);
	}

	/**
	 * @brief Convert rectangle from layout space into screen space.
	 * @param bounds Rectangle to convert from layout space into screen space.
	 * @return Screen space coordinates.
	 */
	[[nodiscard]] rect<float> space_rect_to_screen(rect<float> bounds)
	{
		return nk_layout_space_rect_to_screen(&get_context(), bounds);
	}

	/**
	 * @brief Convert rectangle from screen space into layout space.
	 * @param bounds Rectangle to convert from screen space into layout space.
	 * @return Layout space coordinates.
	 */
	[[nodiscard]] rect<float> space_rect_to_local(rect<float> bounds)
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

/**
 * @brief tree widget scope guard
 */
class tree : public scope_guard
{
public:
	using scope_guard::scope_guard;

	explicit operator bool() const &
	{
		return is_scope_active();
	}
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

	[[nodiscard]] nk_flags push(float value)
	{
		return nk_chart_push(&get_context(), value);
	}

	[[nodiscard]] nk_flags push_slot(float value, int slot)
	{
		return nk_chart_push_slot(&get_context(), value, slot);
	}

	/**
	 * @brief Return whether this chart should be processed.
	 * @return `true` if visible and fillable with widgets, otherwise `false`.
	 */
	explicit operator bool() const &
	{
		return is_scope_active();
	}
};

class popup : public scope_guard
{
public:
	using scope_guard::scope_guard;

	explicit operator bool() const &
	{
		return is_scope_active();
	}

	void close()
	{
		nk_popup_close(&get_context());
	}

	void get_scroll(nk_uint* offset_x, nk_uint* offset_y)
	{
		nk_popup_get_scroll(&get_context(), offset_x, offset_y);
	}

	[[nodiscard]] vec2<nk_uint> get_scroll()
	{
		vec2<nk_uint> result;
		nk_popup_get_scroll(&get_context(), &result.x, &result.y);
		return result;
	}

	void set_scroll(nk_uint offset_x, nk_uint offset_y)
	{
		nk_popup_set_scroll(&get_context(), offset_x, offset_y);
	}
};

class combobox : public scope_guard
{
public:
	using scope_guard::scope_guard;

	explicit operator bool() const &
	{
		return is_scope_active();
	}

	[[nodiscard]] bool combo_item_label(const char* label, nk_flags alignment)
	{
		return nk_combo_item_label(&get_context(), label, alignment) == nk_true;
	}

	[[nodiscard]] bool combo_item_text(const char* text, int len, nk_flags alignment)
	{
		return nk_combo_item_text(&get_context(), text, len, alignment) == nk_true;
	}

	[[nodiscard]] bool combo_item_image_label(image img, const char* text, nk_flags alignment)
	{
		return nk_combo_item_image_label(&get_context(), img, text, alignment) == nk_true;
	}

	[[nodiscard]] bool combo_item_image_text(image img, const char* text, int len, nk_flags alignment)
	{
		return nk_combo_item_image_text(&get_context(), img, text, len, alignment) == nk_true;
	}

	[[nodiscard]] bool combo_item_symbol_label(nk_symbol_type symbol, const char* label, nk_flags alignment)
	{
		return nk_combo_item_symbol_label(&get_context(), symbol, label, alignment) == nk_true;
	}

	[[nodiscard]] bool item_symbol_text(nk_symbol_type symbol, const char* text, int len, nk_flags alignment)
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

	explicit operator bool() const &
	{
		return is_scope_active();
	}

	[[nodiscard]] bool item_text(const char* text, int len, nk_flags align)
	{
		return nk_contextual_item_text(&get_context(), text, len, align) == nk_true;
	}

	[[nodiscard]] bool item_label(const char* label, nk_flags align)
	{
		return nk_contextual_item_label(&get_context(), label, align) == nk_true;
	}

	[[nodiscard]] bool item_image_label(nk::image img, const char* label, nk_flags alignment)
	{
		return nk_contextual_item_image_label(&get_context(), img, label, alignment) == nk_true;
	}

	[[nodiscard]] bool item_image_text(nk::image img, const char* text, int len, nk_flags alignment)
	{
		return nk_contextual_item_image_text(&get_context(), img, text, len, alignment) == nk_true;
	}

	[[nodiscard]] bool item_symbol_label(nk_symbol_type symbol, const char* label, nk_flags alignment)
	{
		return nk_contextual_item_symbol_label(&get_context(), symbol, label, alignment) == nk_true;
	}

	[[nodiscard]] bool item_symbol_text(nk_symbol_type symbol, const char* text, int len, nk_flags alignment)
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

	explicit operator bool() const &
	{
		return is_scope_active();
	}

	[[nodiscard]] bool item_text(const char* text, int len, nk_flags alignment)
	{
		return nk_menu_item_text(&get_context(), text, len, alignment) == nk_true;
	}

	[[nodiscard]] bool item_label(const char* label, nk_flags alignment)
	{
		return nk_menu_item_label(&get_context(), label, alignment) == nk_true;
	}

	[[nodiscard]] bool item_image_label(nk::image img, const char* label, nk_flags alignment)
	{
		return nk_menu_item_image_label(&get_context(), img, label, alignment) == nk_true;
	}

	[[nodiscard]] bool item_image_text(nk::image img, const char* text, int len, nk_flags alignment)
	{
		return nk_menu_item_image_text(&get_context(), img, text, len, alignment) == nk_true;
	}

	[[nodiscard]] bool item_symbol_text(nk_symbol_type symbol, const char* text, int len, nk_flags alignment)
	{
		return nk_menu_item_symbol_text(&get_context(), symbol, text, len, alignment) == nk_true;
	}

	[[nodiscard]] bool item_symbol_label(nk_symbol_type symbol, const char* label, nk_flags alignment)
	{
		return nk_menu_item_symbol_label(&get_context(), symbol, label, alignment) == nk_true;
	}

	void close()
	{
		nk_menu_close(&get_context());
	}
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
 * To change window position inside the stack occurs either automatically by
 * user input by being clicked on or programmatically by calling `nk_window_focus`.
 * Windows by default are visible unless explicitly being defined with flag
 * `NK_WINDOW_HIDDEN`, the user clicked the close button on windows with flag
 * `NK_WINDOW_CLOSABLE` or if a window was explicitly hidden by calling
 * `nk_window_show`. To explicitly close and destroy a window call `nk_window_close`.
 *
 * *Usage:*
 * To create and keep a window you have to call one of the two `nk_begin_xxx`
 * functions to start window declarations and `nk_end` at the end. Furthermore it
 * is recommended to check the return value of `nk_begin_xxx` and only process
 * widgets inside the window if the value is not 0. Either way you have to call
 * `nk_end` at the end of window declarations. Furthermore, do not attempt to
 * nest `nk_begin_xxx` calls which will hopefully result in an assert or if not
 * in a segmentation fault.
 */
class window : public scope_guard
{
private:
	window(nk_context& ctx, func_type* func, bool valid)
	: scope_guard(ctx, func)
	, m_valid(valid)
	{}

public:
	static window create(nk_context& ctx, bool valid)
	{
		return window(ctx, &nk_end, valid);
	}

	static window create_blank(nk_context& ctx)
	{
		return window(ctx, nullptr, false);
	}

	window(window&& other) noexcept
	: scope_guard(move(other))
	, m_valid(exchange(other.m_valid, false))
	{}

	window(const window&) = delete;
	window& operator=(const window&) = delete;
	window& operator=(window&&) noexcept = delete;

	/**
	 * @brief Return whether this window should be processed.
	 * @return `true` if `nk_begin_xxx` returned `true` and this object manages window lifetime.
	 * @details Do not confuse with is_scope_active which checks only for lifetime.
	 * A window always needs an end call (`nk_end`) but if `begin` returned `false`
	 * then the window should not be processed.
	 */
	explicit operator bool() const noexcept
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
	[[nodiscard]] nk_window* window_find(const char* name)
	{
		return nk_window_find(&get_context(), name);
	}

	/**
	 * @brief Return whether the window with given name is currently minimized/collapsed.
	 * @param name Window identifier.
	 * @return `true` if the window is minimized and `false` if not minimized or not found.
	 */
	[[nodiscard]] bool window_is_collapsed(const char* name)
	{
		return nk_window_is_collapsed(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Return whether the window with given name was closed by calling `nk_close`.
	 * @param name Window identifier.
	 * @return `true` if the window was closed and `false` if not closed or not found.
	 */
	[[nodiscard]] bool window_is_closed(const char* name)
	{
		return nk_window_is_closed(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Return whether the window with given name is hidden.
	 * @param name Window identifier.
	 * @return `true` if the window is hidden and `false` if not hidden or not found.
	 */
	[[nodiscard]] bool window_is_hidden(const char* name)
	{
		return nk_window_is_hidden(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Same as `window_has_focus` for some reason.
	 * @param name Window identifier.
	 * @return `true` if the window is active and `false` if not active or not found.
	 */
	[[nodiscard]] bool window_is_active(const char* name)
	{
		return nk_window_is_active(&get_context(), name) == nk_true;
	}

	/**
	 * @brief Return whether any window is being hovered.
	 * @return `true` if any window is hovered or `false` otherwise.
	 */
	[[nodiscard]] bool window_is_any_hovered()
	{
		return nk_window_is_any_hovered(&get_context()) == nk_true;
	}

	/**
	 * @brief Returns if the any window is being hovered or any widget is currently active.
	 * Can be used to decide if input should be processed by UI or your specific input handling.
	 * Example could be UI and 3D camera to move inside a 3D space.
	 * @return `true` if any window is active or `false` otherwise.
	 */
	[[nodiscard]] bool item_is_any_active()
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

	///@}

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
	[[nodiscard]] rect<float> get_bounds() const
	{
		NK_ASSERT(m_valid);
		return nk_window_get_bounds(&get_context());
	}

	/**
	 * @brief Return the position of the currently processed window.
	 * @return Window upper left position.
	 */
	[[nodiscard]] vec2<float> get_position() const
	{
		NK_ASSERT(m_valid);
		return nk_window_get_position(&get_context());
	}

	/**
	 * @brief Return the size with width and height of the currently processed window.
	 * @return Window width and height.
	 */
	[[nodiscard]] vec2<float> get_size() const
	{
		NK_ASSERT(m_valid);
		return nk_window_get_size(&get_context());
	}

	/**
	 * @brief Return the width of the currently processed window.
	 * @return Current window width.
	 */
	[[nodiscard]] float get_width() const
	{
		NK_ASSERT(m_valid);
		return nk_window_get_width(&get_context());
	}

	/**
	 * @brief Return the height of the currently processed window.
	 * @return Current window height.
	 */
	[[nodiscard]] float get_height() const
	{
		NK_ASSERT(m_valid);
		return nk_window_get_height(&get_context());
	}

	/**
	 * @brief Return the underlying panel which contains all processing state of the current window.
	 * @return Pointer to window internal `nk_panel` state.
	 * @attention Do not keep the returned pointer around, it is only valid until end of window's scope.
	 */
	[[nodiscard]] nk_panel* get_panel() &
	{
		NK_ASSERT(m_valid);
		return nk_window_get_panel(&get_context());
	}

	/**
	 * @brief Return the position and size of the currently visible and non-clipped space inside the currently processed window.
	 * @return Screen position and size (no scrollbar offset) of the visible space inside the current window.
	 */
	[[nodiscard]] rect<float> get_content_region()
	{
		NK_ASSERT(m_valid);
		return nk_window_get_content_region(&get_context());
	}

	/**
	 * @brief Return the upper left position of the currently visible and non-clipped space inside the currently processed window.
	 * @return Upper left screen position (no scrollbar offset) of the visible space inside the current window.
	 */
	[[nodiscard]] vec2<float> get_content_region_min()
	{
		NK_ASSERT(m_valid);
		return nk_window_get_content_region_min(&get_context());
	}

	/**
	 * @brief Returns the lower right screen position of the currently visible and non-clipped space inside the currently processed window.
	 * @return Lower right screen position (no scrollbar offset) of the visible space inside the current window
	 */
	[[nodiscard]] vec2<float> get_content_region_max()
	{
		NK_ASSERT(m_valid);
		return nk_window_get_content_region_max(&get_context());
	}

	/**
	 * @brief Return the size of the currently visible and non-clipped space inside the currently processed window.
	 * @return Size of the visible space inside the current window.
	 */
	[[nodiscard]] vec2<float> get_content_region_size()
	{
		NK_ASSERT(m_valid);
		return nk_window_get_content_region_size(&get_context());
	}

	/**
	 * @brief Return the draw command buffer. Can be used to draw custom widgets.
	 * @return Pointer to window internal `nk_command_buffer` struct used as drawing canvas. Can be used to do custom drawing.
	 * @attention Do not keep the returned pointer around, it is only valid until end of window's scope.
	 */
	[[nodiscard]] nk_command_buffer* get_canvas() &
	{
		NK_ASSERT(m_valid);
		return nk_window_get_canvas(&get_context());
	}

	/**
	 * @brief Get the scroll offset for the current window.
	 * @param offset_x x offset output (use nullptr to ignore)
	 * @param offset_y y offset output (use nullptr to ignore)
	 */
	void get_scroll(nk_uint* offset_x, nk_uint* offset_y)
	{
		NK_ASSERT(m_valid);
		nk_window_get_scroll(&get_context(), offset_x, offset_y);
	}

	/**
	 * @brief Get the scroll offset for the current window.
	 * @return Scroll offset for the current window.
	 */
	[[nodiscard]] vec2<nk_uint> get_scroll()
	{
		NK_ASSERT(m_valid);
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
	 * @brief Return whether the currently processed window is currently active.
	 * @return `true` if active.
	 */
	[[nodiscard]] bool has_focus() const
	{
		NK_ASSERT(m_valid);
		return nk_window_has_focus(&get_context()) == nk_true;
	}

	/**
	 * @brief Return whether the currently processed window is being hovered.
	 * @return `true` if hovered.
	 */
	[[nodiscard]] bool is_hovered()
	{
		NK_ASSERT(m_valid);
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

	///@}

	/**
	 * @name Layout
	 * Layouting in general describes placing widget inside a window with position and size.
	 * While in this particular implementation there are five different APIs for layouting
	 * each with different trade offs between control and ease of use.
	 *
	 * All layouting methods in this library are based around the concept of a row.
	 * A row has a height the window content grows by and a number of columns and each
	 * layouting method specifies how each widget is placed inside the row.
	 * After a row has been allocated by calling a layouting functions and then
	 * filled with widgets will advance an internal pointer over the allocated row.
	 *
	 * To actually define a layout you just call the appropriate layouting function
	 * and each subsequent widget call will place the widget as specified. Important
	 * here is that if you define more widgets then columns defined inside the layout
	 * functions it will allocate the next row without you having to make another layouting
	 * call.
	 *
	 * Biggest limitation with using all these APIs outside the `nk_layout_space_xxx` API
	 * is that you have to define the row height for each. However the row height
	 * often depends on the height of the font.
	 *
	 * To fix that internally nuklear uses a minimum row height that is set to the
	 * height plus padding of currently active font and overwrites the row height
	 * value if zero.
	 *
	 * If you manually want to change the minimum row height then
	 * use nk_layout_set_min_row_height, and use nk_layout_reset_min_row_height to
	 * reset it back to be derived from font height.
	 *
	 * Also if you change the font in nuklear it will automatically change the minimum
	 * row height for you and. This means if you change the font but still want
	 * a minimum row height smaller than the font you have to repush your value.
	 *
	 * For actually more advanced UI I would even recommend using the `nk_layout_space_xxx`
	 * layouting method in combination with a cassowary constraint solver (there are
	 * some versions on github with permissive license model) to take over all control over widget
	 * layouting yourself. However for quick and dirty layouting using all the other layouting
	 * functions should be fine.
	 *
	 * @attention Some layouts are automatically repeating (no `scoped_` in name) and do not need an
	 * end all. Therefore for such layouts you can do just `ctx.row_static(...);` as the
	 * guard's destructor wouldn't do anything. For scoped layouts, you need to keep them alive:
	 *
	 *     auto layout = ctx.layout();
	 *     layout.scoped_row(...);
	 *     // (place some widgets)
	 *     layout.row(...); // will call `nk_layout_row_end`
	 *     // (place some widgets)
	 *     layout.scoped_space(); // previous layout does not need an end call
	 *     // (place some widgets)
	 *     // (in destructor, will call `nk_layout_space_end`)
	 *
	 * @{
	 */

	/**
	 * @brief Create a layout scope guard that does not manage any layout.
	 * @return Blank layout object.
	 * @attention Use this function when you want to gain access to layout functions (e.g. groups)
	 * but do not want to change or create a new layout.
	 */
	[[nodiscard]] layout blank_layout()
	{
		return layout(get_context(), nullptr);
	}

	/**
	 * @brief Dynamic row - repeating layout of same-sized growing columns.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param cols Number of widgets inside row.
	 * @return Blank layout guard object
	 * @attention This layout is autorepeating and does not need cleanup. Therefore you don't need to keep the
	 * result of this function. You may still want to save it to gain access to layout functions (e.g. groups).
	 * @details Sets current row layout to share horizontal space between @p cols number of widgets evenly.
	 * Once called all subsequent widget calls greater than @p cols will allocate a new row with same layout.
	 *
	 * The easiest layouting method. Each widget is provided with same horizontal space inside the row and dynamically
	 * grows if the owning window grows in width. The number of columns dictates the size of each widget by formula:
	 * `widget_width = (window_width - padding - spacing) / column_count`.
	 * Just like all other layouting APIs if you define more widgets than columns this
	 * library will allocate a new row and keep all layouting parameters previously defined.
	 *
	 * if (auto window = ctx.scoped_window(...); window) {
	 *     // first row with height: 30 composed of two widgets
	 *     window.layout().row_dynamic(30, 2);
	 *     ctx.widget(...);
	 *     nk_widget(...);
	 *     //
	 *     // second row with same parameter as defined above
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     //
	 *     // third row uses 0 for height which will use auto layouting
	 *     nk_layout_row_dynamic(&ctx, 0, 2);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 * }
	 * nk_end(...);
	 */
	layout row_dynamic(float height, int cols) &
	{
		nk_layout_row_dynamic(&get_context(), height, cols);
		return blank_layout();
	}

	/**
	 * @brief Static row - repeating layout of fixed-size columns.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param item_width Pixel width of each widget in the row.
	 * @param cols Number of widgets inside row.
	 * @return Blank layout guard object
	 * @attention This layout is autorepeating and does not need cleanup. Therefore you don't need to keep the
	 * result of this function. You may still want to save it to gain access to layout functions (e.g. groups).
	 * @details Sets current row layout to fill @p cols number of widgets
	 * in row with same @p item_width horizontal size. Once called all subsequent
	 * widget calls greater than @p cols will allocate a new row with same layout.
	 *
	 * Another easy layouting method. Each widget is provided with same horizontal pixel width inside the row and
	 * does not grow if the owning window scales smaller or bigger.
	 *
	 * if (nk_begin_xxx(...) {
	 *     // first row with height: 30 composed of two widgets with width: 80
	 *     nk_layout_row_static(&ctx, 30, 80, 2);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     //
	 *     // second row with same parameter as defined above
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     //
	 *     // third row uses 0 for height which will use auto layouting
	 *     nk_layout_row_static(&ctx, 0, 80, 2);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 * }
	 * nk_end(...);
	 */
	layout row_static(float height, int item_width, int cols) &
	{
		nk_layout_row_static(&get_context(), height, item_width, cols);
		return blank_layout();
	}

	/**
	 * @brief Scoped row - scoped layout where each widget can have a unique size.
	 * @param fmt Either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns.
	 * @param row_height Height of each widget in row or zero for auto layouting.
	 * @param cols Number of widgets inside row.
	 * @return scope guard object for this %layout.
	 * @details Start a new dynamic or fixed row with given height and columns.
	 * Can be statically or dynamically sized.
	 *
	 * A little bit more advanced layouting API. Allows to directly
	 * specify each column pixel or window ratio in a row. It supports either
	 * directly setting per column pixel width or widget window ratio but not
	 * both. Furthermore it is a immediate mode API so each value is directly
	 * pushed before calling a widget. Therefore the layout is not automatically
	 * repeating like the last two layouting methods.
	 * if (nk_begin_xxx(...) {
	 *     // first row with height: 25 composed of two widgets with width 60 and 40
	 *     nk_layout_row_begin(ctx, NK_STATIC, 25, 2);
	 *     nk_layout_row_push(ctx, 60);
	 *     nk_widget(...);
	 *     nk_layout_row_push(ctx, 40);
	 *     nk_widget(...);
	 *     nk_layout_row_end(ctx);
	 *     //
	 *     // second row with height: 25 composed of two widgets with window ratio 0.25 and 0.75
	 *     nk_layout_row_begin(ctx, NK_DYNAMIC, 25, 2);
	 *     nk_layout_row_push(ctx, 0.25f);
	 *     nk_widget(...);
	 *     nk_layout_row_push(ctx, 0.75f);
	 *     nk_widget(...);
	 *     nk_layout_row_end(ctx);
	 *     //
	 *     // third row with auto generated height: composed of two widgets with window ratio 0.25 and 0.75
	 *     nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 2);
	 *     nk_layout_row_push(ctx, 0.25f);
	 *     nk_widget(...);
	 *     nk_layout_row_push(ctx, 0.75f);
	 *     nk_widget(...);
	 *     nk_layout_row_end(ctx);
	 * }
	 * nk_end(...);
	 */
	[[nodiscard]] layout_row scoped_row(nk_layout_format fmt, float row_height, int cols) &
	{
		nk_layout_row_begin(&get_context(), fmt, row_height, cols);
		return layout_row(get_context());
	}

	/**
	 * @brief Premade row - repeating layout where each widget has its unique size specified in advance.
	 * @param fmt Either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param cols Number of widgets inside row.
	 * @param ratio TODO
	 * @return Blank layout guard object
	 * @attention This layout is autorepeating and does not need cleanup. Therefore you don't need to keep the
	 * result of this function. You may still want to save it to gain access to layout functions (e.g. groups).
	 * @details Specifies row columns in array as either window ratio or size.
	 * Can be statically or dynamically sized.
	 *
	 * The array counterpart to scoped_row. Instead of pushing either pixel or window ratio for
	 * every widget it allows to define it by an array. The trade of for less control is that
	 * it is automatically repeating. Otherwise the behavior is the same.
	 * if (nk_begin_xxx(...) {
	 *     // two rows with height: 30 composed of two widgets with width 60 and 40
	 *     const float ratio[] = {60,40};
	 *     nk_layout_row(ctx, NK_STATIC, 30, 2, ratio);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     //
	 *     // two rows with height: 30 composed of two widgets with window ratio 0.25 and 0.75
	 *     const float ratio[] = {0.25, 0.75};
	 *     nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratio);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     //
	 *     // two rows with auto generated height composed of two widgets with window ratio 0.25 and 0.75
	 *     const float ratio[] = {0.25, 0.75};
	 *     nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratio);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 * }
	 * nk_end(...);
	 */
	layout row(nk_layout_format fmt, float height, int cols, const float* ratio) &
	{
		nk_layout_row(&get_context(), fmt, height, cols, ratio);
		return blank_layout();
	}

	/**
	 * @brief Template row - scoped and repeating layout where a set of layout behaviors is specified in advance.
	 * @param row_height Height of each widget in row or zero for auto layouting.
	 * @return scope guard object for this %layout.
	 * @details Begin the row template scope.
	 * Each widget can be separately statically or dynamically sized.
	 * This layout must be completed before widgets are added.
	 * Either destroy the layout guard object or call reset before adding widgets.
	 *
	 * The most complex and second most flexible API is a simplified flexbox version without line
	 * wrapping and weights for dynamic widgets. It is an immediate mode API but unlike scoped_row
	 * it has auto repeat behavior and needs to be completed before calling the templated widgets.
	 *
	 * The row template layout has three different per widget size specifiers:
	 *
	 * - Static: fixed widget pixel width. They do not grow if the row grows and will always stay the same.
	 * - Variable: defines a minimum widget size but it also can grow if more space is available not taken by other widgets.
	 * - Dynamic: completely flexible and unlike variable widgets can even shrink to zero if not enough space is provided.
	 *
	 * if (nk_begin_xxx(...) {
	 *     // two rows with height: 30 composed of three widgets
	 *     nk_layout_row_template_begin(ctx, 30);
	 *     nk_layout_row_template_push_dynamic(ctx);
	 *     nk_layout_row_template_push_variable(ctx, 80);
	 *     nk_layout_row_template_push_static(ctx, 80);
	 *     nk_layout_row_template_end(ctx);
	 *     //
	 *     // first row
	 *     nk_widget(...); // dynamic widget can go to zero if not enough space
	 *     nk_widget(...); // variable widget with min 80 pixel but can grow bigger if enough space
	 *     nk_widget(...); // static widget with fixed 80 pixel width
	 *     //
	 *     // second row same layout
	 *     nk_widget(...);
	 *     nk_widget(...);
	 *     nk_widget(...);
	 * }
	 * nk_end(...);
	 */
	[[nodiscard]] layout_row_template scoped_row_template(float row_height) &
	{
		nk_layout_row_template_begin(&get_context(), row_height);
		return layout_row_template(get_context());
	}

	/**
	 * @brief Space layout - unconstrained, individual widget placement.
	 * @param fmt Either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns.
	 * @param height Height of each widget in row or zero for auto layouting.
	 * @param widget_count Number of widgets inside row.
	 * @return scope guard object for this %layout.
	 * @details Begin a new layouting space that allows to specify each widget's position and size.
	 * Allows to directly place widgets inside the window. The space layout API is an immediate mode API which
	 * does not support row auto repeat and directly sets position and size of a widget. Position and size
	 * hereby can be either specified as ratio of allocated space or allocated space local position and pixel size.
	 * Since this API is quite powerful there are a number of utility functions to get the available space and
	 * convert between local allocated space and screen space.
	 *
	 * if (nk_begin_xxx(...) {
	 *     // static row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
	 *     nk_layout_space_begin(ctx, NK_STATIC, 500, INT_MAX);
	 *     nk_layout_space_push(ctx, nk_rect(0,0,150,200));
	 *     nk_widget(...);
	 *     nk_layout_space_push(ctx, nk_rect(200,200,100,200));
	 *     nk_widget(...);
	 *     nk_layout_space_end(ctx);
	 *     //
	 *     // dynamic row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
	 *     nk_layout_space_begin(ctx, NK_DYNAMIC, 500, INT_MAX);
	 *     nk_layout_space_push(ctx, nk_rect(0.5,0.5,0.1,0.1));
	 *     nk_widget(...);
	 *     nk_layout_space_push(ctx, nk_rect(0.7,0.6,0.1,0.1));
	 *     nk_widget(...);
	 * }
	 * nk_end(...);
	 */
	[[nodiscard]] layout_space scoped_space(nk_layout_format fmt, float height, int widget_count) &
	{
		nk_layout_space_begin(&get_context(), fmt, height, widget_count);
		return layout_space(get_context());
	}

	///@}

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
	[[nodiscard]] rect<float> layout_widget_bounds()
	{
		return nk_layout_widget_bounds(&get_context());
	}

	/**
	 * @brief Utility function to calculate window ratio from pixel size.
	 * @param pixel_width Pixel width to convert to window ratio.
	 * @return Window ratio.
	 */
	[[nodiscard]] float layout_ratio_from_pixel(float pixel_width)
	{
		return nk_layout_ratio_from_pixel(&get_context(), pixel_width);
	}

	///@}

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
	void group_get_scroll(const char* id, nk_uint* x_offset, nk_uint* y_offset)
	{
		nk_group_get_scroll(&get_context(), id, x_offset, y_offset);
	}

	/**
	 * @brief Get the scroll position of the given group.
	 * @param id The id of the group to get the scroll position of
	 * @return x and y offsets.
	 */
	vec2<nk_uint> group_get_scroll(const char* id)
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

	///@}

private:
	tree scoped_tree_internal(nk_bool result)
	{
		return tree(get_context(), result == nk_true ? &nk_tree_pop : nullptr);
	}

	tree scoped_tree_state_internal(nk_bool result)
	{
		return tree(get_context(), result == nk_true ? &nk_tree_state_pop : nullptr);
	}

	tree scoped_tree_element_internal(nk_bool result)
	{
		return tree(get_context(), result == nk_true ? &nk_tree_element_pop : nullptr);
	}

public:
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
	 * the nk_collapse_states section.
	 *
	 * The runtime state of the tree is either stored outside the library by the caller
	 * or inside which requires a unique ID. The unique ID can either be generated
	 * automatically from `__FILE__` and `__LINE__` with function `nk_tree_push`,
	 * by `__FILE__` and a user provided ID generated for example by loop index with
	 * function `nk_tree_push_id` or completely provided from outside by user with
	 * function `nk_tree_push_hashed`.
	 *
	 * *Usage:*
	 * Each tree starting function will return a scope_guard object which must be checked.
	 * If the check fails, the tree is collapsed or hidden and therefore does not need
	 * to be filled with content.
	 *
	 * The tree header does not require and layouting function and instead
	 * calculates an auto height based on the currently used font size.
	 * @{
	 */

	// TODO nk_tree_push, nk_tree_push_id, nk_tree_image_push, nk_tree_image_push_id, nk_tree_element_push, nk_tree_element_push_id

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
	[[nodiscard]] tree scoped_tree_hashed(
		nk_tree_type type, const char* title, nk_collapse_states initial_state, const char* hash, int len, int seed = 0)
	{
		return scoped_tree_internal(nk_tree_push_hashed(&get_context(), type, title, initial_state, hash, len, seed));
	}

	/**
	 * @copydoc scoped_tree_hashed
	 * @param img Image to display inside the header on the left of the label.
	 */
	[[nodiscard]] tree scoped_tree_image_hashed(
		nk_tree_type type, image img, const char* title, nk_collapse_states initial_state, const char* hash, int len, int seed = 0)
	{
		return scoped_tree_internal(nk_tree_image_push_hashed(&get_context(), type, img, title, initial_state, hash, len, seed));
	}

	/**
	 * @brief Start a collapsible UI section with external state management.
	 * @param type Visually mark a tree node header as either a collapsible UI section or tree node.
	 * @param title Label printed in the tree header.
	 * @param state Persistent state to update.
	 * @return Tree scope guard object that should be immediately checked.
	 */
	[[nodiscard]] tree scoped_tree_state(nk_tree_type type, const char* title, nk_collapse_states& state)
	{
		return scoped_tree_state_internal(nk_tree_state_push(&get_context(), type, title, &state));
	}

	/**
	 * @copydoc scoped_tree_state(nk_tree_type, const char*, nk_collapse_states&)
	 */
	[[nodiscard]] tree scoped_tree_state(nk_tree_type type, const char* title, bool& state)
	{
		nk_collapse_states collapse_state = state ? NK_MAXIMIZED : NK_MINIMIZED;
		tree result = scoped_tree_state(type, title, collapse_state);
		state = collapse_state == NK_MAXIMIZED;
		return result;
	}

	/**
	 * @copydoc scoped_tree_state(nk_tree_type, const char*, nk_collapse_states&)
	 * @param img Image to display inside the header on the left of the label.
	 */
	[[nodiscard]] tree scoped_tree_state_image(
		nk_tree_type type, image img, const char* title, nk_collapse_states& state)
	{
		return scoped_tree_state_internal(nk_tree_state_image_push(&get_context(), type, img, title, &state));
	}

	/**
	 * @copydoc scoped_tree_state_image(nk_tree_type, image, const char*, nk_collapse_states&)
	 */
	[[nodiscard]] tree scoped_tree_state_image(
		nk_tree_type type, image img, const char* title, bool& state)
	{
		nk_collapse_states collapse_state = state ? NK_MAXIMIZED : NK_MINIMIZED;
		tree result = scoped_tree_state_image(type, img, title, collapse_state);
		state = collapse_state == NK_MAXIMIZED;
		return result;
	}

	/**
	 * @brief TODO
	 * @param type
	 * @param title
	 * @param initial_state
	 * @param selected
	 * @param hash
	 * @param len
	 * @param seed
	 * @return
	 */
	[[nodiscard]] tree scoped_tree_element_hashed(
		nk_tree_type type, const char* title, nk_collapse_states initial_state, bool& selected, const char* hash, int len, int seed = 0)
	{
		return scoped_tree_element_internal(nk_tree_element_push_hashed(
			&get_context(), type, title, initial_state, detail::output_bool(selected), hash, len, seed));
	}

	/**
	 * @brief TODO
	 * @param type
	 * @param img
	 * @param title
	 * @param initial_state
	 * @param selected
	 * @param hash
	 * @param len
	 * @param seed
	 * @return
	 */
	[[nodiscard]] tree scoped_tree_element_image_hashed(
		nk_tree_type type, image img, const char* title, nk_collapse_states initial_state, bool& selected, const char* hash, int len, int seed = 0)
	{
		return scoped_tree_element_internal(nk_tree_element_image_push_hashed(
			&get_context(), type, img, title, initial_state, detail::output_bool(selected), hash, len, seed));
	}

	///@}

	/**
	 * @name Widget
	 * UNDOCUMENTED
	 * @{
	 */

	widget_layout_states widget(rect<float>& bounds) const
	{
		struct nk_rect r{};
		auto result = nk_widget(&r, &get_context());
		bounds = r;
		return result;
	}

	widget_layout_states widget_fitting(rect<float>& bounds, vec2<float> item_padding)
	{
		struct nk_rect r{};
		auto result = nk_widget_fitting(&r, &get_context(), item_padding);
		bounds = r;
		return result;
	}

	rect<float> widget_bounds()
	{
		return nk_widget_bounds(&get_context());
	}

	vec2<float> widget_position()
	{
		return nk_widget_position(&get_context());
	}

	vec2<float> widget_size()
	{
		return nk_widget_size(&get_context());
	}

	float widget_width()
	{
		return nk_widget_width(&get_context());
	}

	float widget_height()
	{
		return nk_widget_height(&get_context());
	}

	bool widget_is_hovered()
	{
		return nk_widget_is_hovered(&get_context()) == nk_true;
	}

	bool widget_is_mouse_clicked(buttons btn)
	{
		return nk_widget_is_mouse_clicked(&get_context(), btn) == nk_true;
	}

	bool widget_has_mouse_click_down(buttons btn, bool down)
	{
		return nk_widget_has_mouse_click_down(&get_context(), btn, down) == nk_true;
	}

	void spacing(int cols)
	{
		nk_spacing(&get_context(), cols);
	}

	[[nodiscard]] auto scoped_widget_disable()
	{
		nk_widget_disable_begin(&get_context());
		return make_scoped([ctx = &get_context()]() { nk_widget_disable_end(ctx); });
	}

	///@}

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

	///@}

#ifdef NK_INCLUDE_STANDARD_VARARGS
	/**
	 * @name Text (formatted)
	 * Requires to `#define NK_INCLUDE_STANDARD_VARARGS`.
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
		// there is no nk_value_double but the implementation of nk_value_float supports them
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

	///@}
#endif // NK_INCLUDE_STANDARD_VARARGS

	/**
	 * @name Button
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] bool button_text(const char* title, int len)
	{
		return nk_button_text(&get_context(), title, len) == nk_true;
	}

	[[nodiscard]] bool button_label(const char* title)
	{
		return nk_button_label(&get_context(), title) == nk_true;
	}

	[[nodiscard]] bool button_color(color col)
	{
		return nk_button_color(&get_context(), col) == nk_true;
	}

	[[nodiscard]] bool button_symbol(symbol_type symbol)
	{
		return nk_button_symbol(&get_context(), symbol) == nk_true;
	}

	[[nodiscard]] bool button_image(nk::image img)
	{
		return nk_button_image(&get_context(), img) == nk_true;
	}

	[[nodiscard]] bool button_symbol_label(symbol_type symbol, const char* label, nk_flags align)
	{
		return nk_button_symbol_label(&get_context(), symbol, label, align) == nk_true;
	}

	[[nodiscard]] bool button_symbol_text(symbol_type symbol, const char* text, int len, nk_flags align)
	{
		return nk_button_symbol_text(&get_context(), symbol, text, len, align) == nk_true;
	}

	[[nodiscard]] bool button_image_label(nk::image img, const char* label, nk_flags align)
	{
		return nk_button_image_label(&get_context(), img, label, align) == nk_true;
	}

	[[nodiscard]] bool button_image_text(nk::image img, const char* text, int len, nk_flags align)
	{
		return nk_button_image_text(&get_context(), img, text, len, align) == nk_true;
	}

	[[nodiscard]] bool button_text_styled(const style_button& style, const char* title, int len)
	{
		return nk_button_text_styled(&get_context(), &style, title, len) == nk_true;
	}

	[[nodiscard]] bool button_label_styled(const style_button& style, const char* title)
	{
		return nk_button_label_styled(&get_context(), &style, title) == nk_true;
	}

	[[nodiscard]] bool button_symbol_styled(const style_button& style, symbol_type symbol)
	{
		return nk_button_symbol_styled(&get_context(), &style, symbol) == nk_true;
	}

	[[nodiscard]] bool button_image_styled(const style_button& style, nk::image img)
	{
		return nk_button_image_styled(&get_context(), &style, img) == nk_true;
	}

	[[nodiscard]] bool button_symbol_text_styled(const style_button& style, symbol_type symbol, const char* text, int len, nk_flags align)
	{
		return nk_button_symbol_text_styled(&get_context(), &style, symbol, text, len, align) == nk_true;
	}

	[[nodiscard]] bool button_symbol_label_styled(const style_button& style, symbol_type symbol, const char* title, nk_flags align)
	{
		return nk_button_symbol_label_styled(&get_context(), &style, symbol, title, align) == nk_true;
	}

	[[nodiscard]] bool button_image_label_styled(const style_button& style, nk::image img, const char* label, nk_flags align)
	{
		return nk_button_image_label_styled(&get_context(), &style, img, label, align) == nk_true;
	}

	[[nodiscard]] bool button_image_text_styled(const style_button& style, nk::image img, const char* text, int len, nk_flags align)
	{
		return nk_button_image_text_styled(&get_context(), &style, img, text, len, align) == nk_true;
	}

	void button_set_behavior(button_behavior behavior)
	{
		nk_button_set_behavior(&get_context(), behavior);
	}

	[[nodiscard]] scoped_override_guard scoped_button_behavior(button_behavior behavior)
	{
		return scoped_override_guard(
			get_context(),
			nk_button_push_behavior(&get_context(), behavior) == nk_true ? &nk_button_pop_behavior : nullptr);
	}

	///@}

	/**
	 * @name Checkbox
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] bool check_label(const char* label, bool active)
	{
		return nk_check_label(&get_context(), label, active) == nk_true;
	}

	[[nodiscard]] bool check_text(const char* text, int len, bool active)
	{
		return nk_check_text(&get_context(), text, len, active) == nk_true;
	}

	[[nodiscard]] bool check_text_align(const char* text, int len, bool active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_check_text_align(&get_context(), text, len, active, widget_alignment, text_alignment) == nk_true;
	}

	[[nodiscard]] unsigned check_flags_label(const char* label, unsigned flags, unsigned value)
	{
		return nk_check_flags_label(&get_context(), label, flags, value);
	}

	[[nodiscard]] unsigned check_flags_text(const char* text, int len, unsigned flags, unsigned value)
	{
		return nk_check_flags_text(&get_context(), text, len, flags, value);
	}

	[[nodiscard]] bool checkbox_label(const char* label, bool& active)
	{
		return nk_checkbox_label(&get_context(), label, detail::output_bool(active)) == nk_true;
	}

	[[nodiscard]] bool checkbox_label_align(const char* label, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_checkbox_label_align(&get_context(), label, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	[[nodiscard]] bool checkbox_text(const char* text, int len, bool& active)
	{
		return nk_checkbox_text(&get_context(), text, len, detail::output_bool(active)) == nk_true;
	}

	[[nodiscard]] bool checkbox_text_align(const char* text, int len, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_checkbox_text_align(&get_context(), text, len, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	[[nodiscard]] bool checkbox_flags_label(const char* label, unsigned& flags, unsigned value)
	{
		return nk_checkbox_flags_label(&get_context(), label, &flags, value) == nk_true;
	}

	[[nodiscard]] bool checkbox_flags_text(const char* text, int len, unsigned& flags, unsigned value)
	{
		return nk_checkbox_flags_text(&get_context(), text, len, &flags, value) == nk_true;
	}

	///@}

	/**
	 * @name Radio Button
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] bool radio_label(const char* label, bool& active)
	{
		return nk_radio_label(&get_context(), label, detail::output_bool(active)) == nk_true;
	}
	[[nodiscard]] bool radio_label_align(const char* label, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_radio_label_align(&get_context(), label, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	[[nodiscard]] bool radio_text(const char* text, int len, bool& active)
	{
		return nk_radio_text(&get_context(), text, len, detail::output_bool(active)) == nk_true;
	}

	[[nodiscard]] bool radio_text_align(const char* text, int len, bool& active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_radio_text_align(&get_context(), text, len, detail::output_bool(active), widget_alignment, text_alignment) == nk_true;
	}

	[[nodiscard]] bool option_label(const char* label, bool active)
	{
		return nk_option_label(&get_context(), label, active) == nk_true;
	}

	[[nodiscard]] bool option_label_align(const char* label, bool active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_option_label_align(&get_context(), label, active, widget_alignment, text_alignment) == nk_true;
	}

	[[nodiscard]] bool option_text(const char* text, int len, bool active)
	{
		return nk_option_text(&get_context(), text, len, active) == nk_true;
	}

	[[nodiscard]] bool option_text_align(const char* text, int len, bool active, nk_flags widget_alignment, nk_flags text_alignment)
	{
		return nk_option_text_align(&get_context(), text,  len, active, widget_alignment, text_alignment) == nk_true;
	}

	///@}

	/**
	 * @name Selectable
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] bool selectable_label(const char* str, nk_flags align, bool& value)
	{
		return nk_selectable_label(&get_context(), str, align, detail::output_bool(value)) == nk_true;
	}

	[[nodiscard]] bool selectable_text(const char* str, int len, nk_flags align, bool& value)
	{
		return nk_selectable_text(&get_context(), str, len, align, detail::output_bool(value));
	}

	[[nodiscard]] bool selectable_image_label(nk::image img, const char* str, nk_flags align, bool& value)
	{
		return nk_selectable_image_label(&get_context(), img, str, align, detail::output_bool(value));
	}

	[[nodiscard]] bool selectable_image_text(nk::image img, const char* str, int len, nk_flags align, bool& value)
	{
		return nk_selectable_image_text(&get_context(), img, str, len, align, detail::output_bool(value));
	}

	[[nodiscard]] bool selectable_symbol_label(symbol_type symbol, const char* str, nk_flags align, bool& value)
	{
		return nk_selectable_symbol_label(&get_context(), symbol, str, align, detail::output_bool(value));
	}

	[[nodiscard]] bool selectable_symbol_text(symbol_type symbol, const char* str, int len, nk_flags align, bool& value)
	{
		return nk_selectable_symbol_text(&get_context(), symbol, str, len, align, detail::output_bool(value));
	}

	[[nodiscard]] bool select_label(const char* str, nk_flags align, bool value)
	{
		return nk_select_label(&get_context(), str, align, value) == nk_true;
	}

	[[nodiscard]] bool select_text(const char* str, int len, nk_flags align, bool value)
	{
		return nk_select_text(&get_context(), str, len, align, value) == nk_true;
	}

	[[nodiscard]] bool select_image_label(nk::image img, const char* str, nk_flags align, bool value)
	{
		return nk_select_image_label(&get_context(), img, str, align, value) == nk_true;
	}

	[[nodiscard]] bool select_image_text(nk::image img, const char* str, int len, nk_flags align, bool value)
	{
		return nk_select_image_text(&get_context(), img, str, len, align, value) == nk_true;
	}

	[[nodiscard]] bool select_symbol_label(symbol_type symbol, const char* str, nk_flags align, bool value)
	{
		return nk_select_symbol_label(&get_context(), symbol, str, align, value) == nk_true;
	}

	[[nodiscard]] bool select_symbol_text(symbol_type symbol, const char* str, int len, nk_flags align, bool value)
	{
		return nk_select_symbol_text(&get_context(), symbol, str, len, align, value) == nk_true;
	}

	///@}

	/**
	 * @name Slider
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] float slide(float min, float value, float max, float step)
	{
		return nk_slide_float(&get_context(), min, value, max, step);
	}

	[[nodiscard]] int slide(int min, int value, int max, int step)
	{
		return nk_slide_int(&get_context(), min, value, max, step);
	}

	[[nodiscard]] bool slider(float min, float& value, float max, float step)
	{
		return nk_slider_float(&get_context(), min, &value, max, step);
	}

	[[nodiscard]] bool slider(int min, int& value, int max, int step)
	{
		return nk_slider_int(&get_context(), min, &value, max, step);
	}

	///@}

	/**
	 * @name Knob
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] bool knob(float min, float& value, float max, float step, heading zero_direction, float dead_zone_degrees)
	{
		return nk_knob_float(&get_context(), min, &value, max, step, zero_direction, dead_zone_degrees) == nk_true;
	}

	[[nodiscard]] bool knob(int min, int& value, int max, int step, enum nk_heading zero_direction, float dead_zone_degrees)
	{
		return nk_knob_int(&get_context(), min, &value, max, step, zero_direction, dead_zone_degrees) == nk_true;
	}

	///@}

	/**
	 * @name Progress Bar
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] nk_bool progress(nk_size& cur, nk_size max, bool modifyable)
	{
		return nk_progress(&get_context(), &cur, max, modifyable) == nk_true;
	}

	[[nodiscard]] nk_size prog(nk_size cur, nk_size max, bool modifyable)
	{
		return nk_prog(&get_context(), cur, max, modifyable);
	}

	///@}

	/**
	 * @name Color Picker
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] colorf color_picker(colorf col, color_format fmt)
	{
		return nk_color_picker(&get_context(), col, fmt);
	}

	[[nodiscard]] bool color_pick(colorf& col, color_format fmt)
	{
		struct nk_colorf c{};
		bool result = nk_color_pick(&get_context(), &c, fmt) == nk_true;
		col = c;
		return result;
	}

	///@}

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
	void property(const char* name, int min, int& val, int max, int step = 1, float inc_per_pixel = 1.0f)
	{
		nk_property_int(&get_context(), name, min, &val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc property(const char*, int, int&, int, int, float)
	 */
	void property(const char* name, float min, float& val, float max, float step = 1.0f, float inc_per_pixel = 1.0f)
	{
		nk_property_float(&get_context(), name, min, &val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc property(const char*, int, int&, int, int, float)
	 */
	void property(const char* name, double min, double& val, double max, double step = 1.0, float inc_per_pixel = 1.0f)
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
	[[nodiscard]] int propertyi(const char* name, int min, int val, int max, int step = 1, float inc_per_pixel = 1.0f)
	{
		return nk_propertyi(&get_context(), name, min, val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc propertyi
	 */
	[[nodiscard]] float propertyf(const char* name, float min, float val, float max, float step = 1.0f, float inc_per_pixel = 1.0f)
	{
		return nk_propertyf(&get_context(), name, min, val, max, step, inc_per_pixel);
	}

	/**
	 * @copydoc propertyi
	 */
	[[nodiscard]] double propertyd(const char* name, double min, double val, double max, double step = 1.0, float inc_per_pixel = 1.0f)
	{
		return nk_propertyd(&get_context(), name, min, val, max, step, inc_per_pixel);
	}

	///@}

	/**
	 * @name Text Edit
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] nk_flags edit_string(nk_flags flags, char* buffer, int& len, int max, nk_plugin_filter filter)
	{
		return nk_edit_string(&get_context(), flags, buffer, &len, max, filter);
	}

	[[nodiscard]] nk_flags edit_string_zero_terminated(nk_flags flags, char* buffer, int max, nk_plugin_filter filter)
	{
		return nk_edit_string_zero_terminated(&get_context(), flags, buffer, max, filter);
	}

	[[nodiscard]] nk_flags edit_buffer(nk_flags flags, nk_text_edit& edit, nk_plugin_filter filter)
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

	///@}

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
	 * For the immediate mode version you start by calling `scoped_chart`
	 * and need to provide min and max values for scaling on the Y-axis.
	 * and then call `push` on it to push values into the chart.
	 * @{
	 */

	[[nodiscard]] chart scoped_chart(nk_chart_type type, int count, float min, float max)
	{
		return chart(
			get_context(),
			nk_chart_begin(&get_context(), type, count, min, max) == nk_true ? &nk_chart_end : nullptr);
	}

	[[nodiscard]] chart scoped_chart_colored(nk_chart_type type, color col, color highlight, int count, float min, float max)
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

	template <typename F>
	void plot_function(nk_chart_type type, F&& f, int count, int offset) // TODO needs test
	{
		static_assert(!is_pointer_v<remove_reference_t<F>>, "pass a function, not a function pointer");
		auto value_getter = [](void* userdata, int index) -> float {
			return (*static_cast<remove_reference_t<F>*>(userdata))(index);
		};
		plot_function(type, static_cast<void*>(&f), value_getter, count, offset);
	}

	///@}

	/**
	 * @name Popup
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] popup scoped_popup(nk_popup_type type, const char* title, nk_flags flags, rect<float> bounds)
	{
		return popup(
			get_context(),
			nk_popup_begin(&get_context(), type, title, flags, bounds) == nk_true ? &nk_popup_end : nullptr);
	}

	///@}

private:
	[[nodiscard]] combobox scoped_combo_internal(nk_bool result)
	{
		return combobox(get_context(), result == nk_true ? &nk_combo_end : nullptr);
	}

public:
	/**
	 * @name Abstract Combobox
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] combobox scoped_combo_text(const char* selected, int len, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_text(&get_context(), selected, len, size));
	}

	[[nodiscard]] combobox scoped_combo_label(const char* selected, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_label(&get_context(), selected, size));
	}

	[[nodiscard]] combobox scoped_combo_color(color col, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_color(&get_context(), col, size));
	}

	[[nodiscard]] combobox scoped_combo_symbol(nk_symbol_type symbol, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_symbol(&get_context(), symbol, size));
	}

	[[nodiscard]] combobox scoped_combo_symbol_label(const char* selected, nk_symbol_type symbol, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_symbol_label(&get_context(), selected, symbol, size));
	}

	[[nodiscard]] combobox scoped_combo_symbol_text(const char* selected, int len, nk_symbol_type symbol, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_symbol_text(&get_context(), selected, len, symbol, size));
	}

	[[nodiscard]] combobox scoped_combo_image(nk::image img, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_image(&get_context(), img, size));
	}

	[[nodiscard]] combobox scoped_combo_image_label(const char* selected, nk::image img, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_image_label(&get_context(), selected, img, size));
	}

	[[nodiscard]] combobox scoped_combo_image_text(const char* selected, int len, nk::image img, vec2<float> size)
	{
		return scoped_combo_internal(nk_combo_begin_image_text(&get_context(), selected, len, img, size));
	}

	///@}

	/**
	 * @name Contextual
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] contextual scoped_contextual(nk_flags flags, vec2<float> size, rect<float> trigger_bounds)
	{
		return contextual(
			get_context(),
			nk_contextual_begin(&get_context(), flags, size, trigger_bounds) == nk_true ? &nk_contextual_end : nullptr);
	}

	///@}

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
	void tooltipf(NK_PRINTF_FORMAT_STRING const char* fmt, ...) NK_PRINTF_VARARG_FUNC(2)
	{
		va_list args;
		va_start(args, fmt);
		nk_tooltipfv(&get_context(), fmt, args);
		va_end(args);
	}

	void tooltipfv(NK_PRINTF_FORMAT_STRING const char* fmt, va_list args) NK_PRINTF_VALIST_FUNC(2)
	{
		nk_tooltipfv(&get_context(), fmt, args);
	}
#endif

	scope_guard scoped_tooltip(float width)
	{
		return scope_guard(get_context(), nk_tooltip_begin(&get_context(), width) == nk_true ? &nk_tooltip_end : nullptr);
	}

	///@}

private:
	[[nodiscard]] menu scoped_menu_internal(nk_bool result)
	{
		return menu(get_context(), result == nk_true ? &nk_menu_end : nullptr);
	}

public:
	/**
	 * @name Menu
	 * UNDOCUMENTED
	 * @{
	 */

	[[nodiscard]] scope_guard scoped_menubar()
	{
		nk_menubar_begin(&get_context()); // this one returns void - always succeeds
		return scope_guard(get_context(), &nk_menubar_end);
	}

	[[nodiscard]] menu menu_begin_text(const char* text, int len, nk_flags alignment, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_text(&get_context(), text, len, alignment, size));
	}

	[[nodiscard]] menu menu_begin_label(const char* label, nk_flags alignment, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_label(&get_context(), label, alignment, size));
	}

	[[nodiscard]] menu menu_begin_image(const char* id, nk::image img, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_image(&get_context(), id, img, size));
	}

	[[nodiscard]] menu menu_begin_image_text(const char* text, int len, nk_flags alignment, nk::image img, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_image_text(&get_context(), text, len, alignment, img, size));
	}

	[[nodiscard]] menu menu_begin_image_label(const char* label, nk_flags alignment, nk::image img, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_image_label(&get_context(), label, alignment, img, size));
	}

	[[nodiscard]] menu menu_begin_symbol(const char* id, nk_symbol_type symbol, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_symbol(&get_context(), id, symbol, size));
	}

	[[nodiscard]] menu menu_begin_symbol_text(const char* text, int len, nk_flags alignment, nk_symbol_type symbol, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_symbol_text(&get_context(), text, len, alignment, symbol, size));
	}

	[[nodiscard]] menu menu_begin_symbol_label(const char* label, nk_flags alignment, nk_symbol_type symbol, vec2<float> size)
	{
		return scoped_menu_internal(nk_menu_begin_symbol_label(&get_context(), label, alignment, symbol, size));
	}

	///@}

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
	 * @brief Initialize context with default standard library allocator.
	 * @details Should be used if you don't want to be bothered with memory management in nuklear.
	 * @param user_font Previously initialized font handle.
	 * @return Context object - always check @ref is_valid after the call.
	 */
	static context init_default(const nk_user_font& user_font)
	{
		context ctx;
		ctx.m_valid = nk_init_default(&ctx.m_ctx, &user_font) == nk_true;
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
	 * TODO warning: make sure the passed memory block is aligned correctly for `nk_draw_commands`.
	 */
	static context init_fixed(void* memory, nk_size size, const nk_user_font& user_font)
	{
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
	static context init(const nk_allocator& allocator, const nk_user_font& user_font)
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
	static context init_custom(nk_buffer& cmds, nk_buffer& pool, const nk_user_font& user_font)
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
		NK_ASSERT(m_valid);
		nk_clear(&m_ctx);
	}

#ifdef NK_INCLUDE_COMMAND_USERDATA
	/**
	 * @brief Set the userdata passed down into each draw command.
	 */
	void set_user_data(nk_handle handle)
	{
		nk_set_user_data(&m_ctx, handle);
	}
#endif
	/**
	 * @copydoc is_valid
	 */
	explicit operator bool() const noexcept
	{
		return is_valid();
	}

	/**
	 * @brief Check the state of this object.
	 * @return Whether initialization was successful.
	 */
	[[nodiscard]] bool is_valid() const { return m_valid; }

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
	[[nodiscard]] nk_context release()
	{
		m_valid = false;
		return exchange(m_ctx, nk_context{});
	}

	///@}

	/**
	 * @name Input
	 * @{
	 */

	/**
	 * @brief Start scoped input. input_end is called automatically at the end of scope.
	 * @return input object, offering access to input functions
	 */
	[[nodiscard]] input scoped_input() &
	{
		nk_input_begin(&m_ctx);
		return input(m_ctx, nk_input_end);
	}

	///@}

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
	 * @{
	 */

	/**
	 * @brief get the list of Nuklear's drawing commands
	 * @return range object which supports C++11 range-based loops
	 * @details example use: `for (const nk_command& cmd : ctx.commands())`
	 */
	[[nodiscard]] range<command_iterator> commands()
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
	 * `nk_convert_config` struct.
	 * @param cmds previously initialized buffer to hold converted vertex draw commands
	 * @param vertices previously initialized buffer to hold all produced vertices
	 * @param elements previously initialized buffer to hold all produced vertex indices
	 * @param config filled out `nk_config` struct to configure the conversion process
	 * @return one of `nk_convert_result` error codes
	 */
	nk_convert_result convert(nk_buffer* cmds, nk_buffer* vertices, nk_buffer* elements, const nk_convert_config& config)
	{
		return static_cast<nk_convert_result>(nk_convert(&m_ctx, cmds, vertices, elements, &config));
	}

	/**
	 * @brief get the list of Nuklear's vertex drawing commands
	 * @param buf previously by `nk_convert` filled out vertex draw command buffer
	 * @return range object which supports C++11 range-based loops
	 * @details example use: `for (const nk_draw_command& cmd : ctx.draw_commands())`
	 */
	[[nodiscard]] range<draw_command_iterator> draw_commands(const nk_buffer* buf) const
	{
		return {
			draw_command_iterator(m_ctx, buf, nk__draw_begin(&m_ctx, buf)),
			draw_command_iterator(m_ctx, buf, nullptr)
		};
	}
#endif

	///@}

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
	[[nodiscard]] window blank_window()
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
	 * @details Example use: `if (auto window = ctx.scoped_window(...); window)`.
	 */
	[[nodiscard]] window scoped_window(const char* title, rect<float> bounds, nk_flags flags)
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
	 * @details Example use: `if (auto window = ctx.scoped_window_titled(...); window)`.
	 */
	[[nodiscard]] window scoped_window_titled(const char* name, const char* title, rect<float> bounds, nk_flags flags)
	{
		return window::create(m_ctx, nk_begin_titled(&m_ctx, name, title, bounds, flags) == nk_true);
	}

	/**
	 * @copydoc window::window_find
	 */
	[[nodiscard]] nk_window* window_find(const char* name)
	{
		return nk_window_find(&m_ctx, name);
	}

	/**
	 * @copydoc window::window_is_collapsed
	 */
	[[nodiscard]] bool window_is_collapsed(const char* name)
	{
		return nk_window_is_collapsed(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_closed
	 */
	[[nodiscard]] bool window_is_closed(const char* name)
	{
		return nk_window_is_closed(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_hidden
	 */
	[[nodiscard]] bool window_is_hidden(const char* name)
	{
		return nk_window_is_hidden(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_active
	 */
	[[nodiscard]] bool window_is_active(const char* name)
	{
		return nk_window_is_active(&m_ctx, name) == nk_true;
	}

	/**
	 * @copydoc window::window_is_any_hovered
	 */
	[[nodiscard]] bool window_is_any_hovered()
	{
		return nk_window_is_any_hovered(&m_ctx) == nk_true;
	}

	/**
	 * @copydoc window::item_is_any_active
	 */
	[[nodiscard]] bool item_is_any_active()
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

	///@}

private:
	[[nodiscard]] scoped_override_guard make_scoped_override(nk_bool push_result, nk_bool (*pop_func)(nk_context*))
	{
		return scoped_override_guard(m_ctx, push_result ? pop_func : nullptr);
	}

	void check_style_pointer(const void* p) const
	{
		const auto min = static_cast<const void*>(&m_ctx);
		const auto max = static_cast<const void*>(&m_ctx + 1);
		NK_ASSERT(min <= p);
		NK_ASSERT(p < max);
	}

public:
	/**
	 * @name Style
	 * UNDOCUMENTED
	 * How scoped overrides work:
	 * - pass a pointer to an object within this class data
	 * - pass a value that should override pointed object
	 * Use get_* functions to obtain access to this data and to get their addresses.
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

	[[nodiscard]] static const char* style_get_color_name(nk_style_colors c)
	{
		return nk_style_get_color_by_name(c);
	}

	void style_set_font(const nk_user_font& font)
	{
		nk_style_set_font(&m_ctx, &font);
	}

	[[nodiscard]] bool style_set_cursor(enum nk_style_cursor cursor)
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

	[[nodiscard]] scoped_override_guard style_scoped_font(const nk_user_font& font)
	{
		return make_scoped_override(nk_style_push_font(&m_ctx, &font), &nk_style_pop_font);
	}

	[[nodiscard]] scoped_override_guard style_push_float(float* p, float value)
	{
		check_style_pointer(p);
		return make_scoped_override(nk_style_push_float(&m_ctx, p, value), &nk_style_pop_float);
	}

	[[nodiscard]] scoped_override_guard style_push_vec2(struct nk_vec2* p, struct nk_vec2 value)
	{
		check_style_pointer(p);
		return make_scoped_override(nk_style_push_vec2(&m_ctx, p, value), &nk_style_pop_vec2);
	}

	[[nodiscard]] scoped_override_guard style_push_style_item(nk_style_item* p, nk_style_item value)
	{
		check_style_pointer(p);
		return make_scoped_override(nk_style_push_style_item(&m_ctx, p, value), &nk_style_pop_style_item);
	}

	[[nodiscard]] scoped_override_guard style_push_flags(nk_flags* p, nk_flags value)
	{
		check_style_pointer(p);
		return make_scoped_override(nk_style_push_flags(&m_ctx, p, value), &nk_style_pop_flags);
	}

	[[nodiscard]] scoped_override_guard style_push_color(nk_color* p, nk_color value)
	{
		check_style_pointer(p);
		return make_scoped_override(nk_style_push_color(&m_ctx, p, value), &nk_style_pop_color);
	}

	///@}

	/**
	 * @name Public fields of the context struct
	 * @{
	 */

	nk_input& get_input() { return m_ctx.input; }
	const nk_input& get_input() const { return m_ctx.input; }

	nk_style& get_style() { return m_ctx.style; }
	const nk_style& get_style() const { return m_ctx.style; }

	nk_buffer& get_memory() { return m_ctx.memory; }
	const nk_buffer& get_memory() const { return m_ctx.memory; }

	nk_clipboard& get_clipboard() { return m_ctx.clip; }
	const nk_clipboard& get_clipboard() const { return m_ctx.clip; }

	nk_flags& get_last_widget_state() { return m_ctx.last_widget_state; }
	nk_flags get_last_widget_state() const { return m_ctx.last_widget_state; }

	enum nk_button_behavior& get_button_behavior() { return m_ctx.button_behavior; }
	enum nk_button_behavior get_button_behavior() const { return m_ctx.button_behavior; }

	nk_configuration_stacks& get_configuration_stacks() { return m_ctx.stacks; }
	const nk_configuration_stacks& get_configuration_stacks() const { return m_ctx.stacks; }

	float& get_delta_time_seconds() { return m_ctx.delta_time_seconds; }
	float get_delta_time_seconds() const { return m_ctx.delta_time_seconds; }

	///@}

private:
	context() = default;

	nk_context m_ctx = {};
	bool m_valid = false;
};

///@}

} // namespace nk
