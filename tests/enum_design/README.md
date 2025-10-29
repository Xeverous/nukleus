# Enumeration Design

The following directory showcases different possible implementations of rich enumerations in Nukleus.

Goals:

- type safety (no unsafe implicit convertions)
- support for common operations (e.g. contextual convertion to `bool` if needed, bitwise operations for enum-flags)
- relatively easy (but not implicit) convertion to/from nuklear's unscoped C enums
- relatively easy (but not implicit) convertion to/from `nk_flags` for flag-enum types

How to test that compilation works:

- `g++ -std=c++11 -Wall -Wextra -Wpedantic enum_simple.cpp enum_wrap.cpp enum_crtp.cpp main.cpp`
- `g++ -std=c++14 -Wall -Wextra -Wpedantic enum_simple.cpp enum_wrap.cpp enum_crtp.cpp main.cpp`

Almost every file is a source file in order to verify that any particular implementation does not contain any linker bugs (e.g. lack of `inline` or improperly defined `static` constants).

# Notes

A class can not easily have static const instances of its own type (only functions) because declaring such members requires the class to be complete. There are some workarounds like splitting declaration/definition (or using functions or a global reference) but these break header-only library idea.

- https://stackoverflow.com/questions/29432283/c-static-constexpr-field-with-incomplete-type
- https://stackoverflow.com/questions/11928089/static-constexpr-member-of-same-type-as-class-being-defined
- https://stackoverflow.com/questions/21008861/initializing-a-static-constexpr-from-an-incomplete-type-because-of-a-template-ba

A class can inherit `friend` free function operator overloads. This is useful with templates and inheritance.

# Summary

Below is a summary of all experiments and a comparison between implementations.

Legend:

- `C` - class type (some implementations use additional class)
- `V` - enum value type (type of enumerations themselves - in some implementations they can be real `enum` or `enum class`, in other - `static` constants of some non-enum type)
- `nk_flags` - Nuklear's `nk_flags` type (an alias for an unsigned type)
- `nk_enum` - Nuklear's C unscoped enumeration type (type unsafe by default)

Implementations:

| Name   | Description |
| :----- | :---------- |
| Simple | `enum class` + overloaded operators |
| Wrap   | `enum class` + support class for operators and convertions |
| CRTP   | advanced implementation (ab)using templates and inheritance

Because some implementations use additional classes, there are additional convertion cases in the table below.

Tradeoffs:

| Feature | Simple | Wrap | CRTP |
| :-----: | :----: | :--: | :--: |
| `V => nk_enum` | free function | free function or atypical operator | (implicit) member function
| `C => nk_enum` | - | (implicit) member function | (implicit) member function
| `nk_enum => V/C` | free function | (implicit) member function | (implicit) member function
| `V => nk_flags` | free function | free function or atypical operator| member function
| `C => nk_flags` | - | member function | member function
| `nk_flags => V/C` | free function | member function | member function
| bitwise operators | 7 | 20 | 7
| `V/C => bool` | no or atypical operator | yes | yes

Description:

- free function - worse than member function
- (implicit) member function - member function with `explicit` convertion control
- bitwise operators - amount of necessary operators to implement (Wrap has to support both types)
- no/yes - support for convertion of C/V to `bool`

Conclusions:

- Simple implementation relies completely on supportive free functions (worst set of features) but is the simplest to use (1 macro that implements everything). Currently used by Nukleus.
- Wrapper-based implementation is equal or worse to CRTP on every feature. Additionally it suffers the necessity to implement additional combinations of operators accepting mixed V and C.
- CRTP implements the richest and nicest set of features, particulary because CRTP itself causes both V and C to be classes, not enums which allows to implement the highest amount of additional functions (including convertions, operators and constructors) which are then inherited. Nukleus might switch to this implementation in the future.
