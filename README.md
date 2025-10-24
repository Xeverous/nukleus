# Nukleus - a modern C++ wrapper library for Nuklear IMGUI C Library

The goal of this library is to offer a richer and more convenient interface to Nuklear's API while preserving Nuklear's design goals and accessibility to the lower layer C API.

Goals and features:

- Support header-only use with one file containing `NK_IMPLEMENTATION` (just like Nuklear).
- Honor/follow all defines that Nuklear uses to control build/implementation configuration.
- Just like Nuklear, this library supports a use case where no C or C++ standard library headers are used.
- No closed interface (every wrapper offers `.release()` and `.get()` like `std::unique_ptr`).
- Full API cooperation (you can mix C++ API calls with C API calls).
- No global or hidden state.
- Consistent naming (`nk_xyz => nk::xyz`, `nk_xyz(ctx, param) => ctx.xyz(param)`) where possible/appropriate.
- Support for building without exceptions.
- Continued application of features such as `__attribute__((format(__printf__, n, n + 1)))` on text-formatting functions.

Additions:

- Doxygen documentation (with descriptions and examples in C++).
- C++ object syntax: `nk_button_label(ctx, "text");` => `win.button_label("text");`.
- RAII and other encapsulation: `if (auto grp = win.group("group_button", NK_WINDOW_BORDER); grp)` - eliminates the need to manually call `nk_*begin`, `nk_*end` functions.
- `[[nodiscard]]` to avoid losing important state and resources.
- Default arguments and function templates to cover multiple C function "overloads" in one C++ function.
- Improved type safety: Nukleus C++ API uses references where Nuklear's C API does not accept null pointers.
- Replaceable `NUKLEUS_ASSERT` and `NUKLEUS_ASSERT_MSG` macros (Nuklear exposes its assertion macro only under `NK_IMPLEMENTATION`).
- If not defined, some of Nuklear's macros (e.g. `NK_MEMSET`, `NK_SIN`) are defaulted to the standard library, which offers better implementation than Nuklear's handcrafted functions. If you want to use Nuklear's implementation, define `NUKLEUS_AVOID_STDLIB`.
- Demo applications using the C++ API.

Requirements:

- C++11 (recommended C++17)
- (for demo application) SDL >= 2.0.18

## Building

Nukleus is a single-file header-only library. Nuklear has a source component and requires one file with `NK_IMPLEMENTATION` defined. So in total, 2 headers (Nukleus + Nuklear) and 1 source (to compile Nuklear) is needed.

The CMake recipe in this repository offers 2 targets:

- `xev::nukleus` which is a configurable library object with the implementation part. It has many options, including a replica of Nuklear's core build options. All of Nuklear-based options have defaults which are optimized for richest set of features and simplest way of integration. `target_link_libraries` will forward inclusion paths and all configured defines.
- `xev::nukleus_headers` which is a non-configurable `INTERFACE` target. `target_link_libraries` will only forward inclusion paths. You are responsible for `NK_IMPLEMENTATION` and any other defines.

Use `xev::nukleus` target from supplied CMake file if provided options are sufficient for you and you are fine with it not being header-only. Otherwise use `xev::nukleus_headers` in your project (it will be affected by your project's defines). Lastly, you can always integrate the code in a copy-paste manner in your own project with a completely different build system.

If you either:

- use the `xev::nukleus_headers` target from supplied CMake
- don't use supplied CMake or use a different build system

...then make sure that all `NK_*` macros are consistent for both headers and the implementation file - either add them to Nuklear's header (which is used by Nukleus) or use same compiler options.

## Generating documentation

```
doxygen Doxyfile
```

Then start browsing from `doc/html/index.html`. Various parts of the library have longer descriptions in the "Topics" section. The entire `doc` directory is `.gitignore`d.

## FAQ

**Does C++ standard affect the API?**

There are no API changes but new standards may enable additional API in the future. Also, see question below.

**Why is C++17 recommended?**

- C++17 allows very neat code like `if (auto grp = ctx.group("group_button", NK_WINDOW_BORDER); grp)`.
- Before C++17, `NUKLEUS_NODISCARD` macro will be replaced to something like `__attribute__((warn_unused_result))` if available. With C++17 it's defined to be `[[nodiscard]]`.
- Default `NK_DTOA` implementation can use `<charconv>` (instead of `snprintf`) when on C++17 or later (because not all compilers implement it even with C++17, you need to define `NUKLEUS_USE_CHARCONV`).
- Higher C++ standard versions add more `constexpr`.

**Why some code uses `struct` or `enum` (aka elaborated-type-specifier) in front of type names?**

Using `struct`, `enum`, `union` in front of types is required in C (unless they are defined through `typedef struct { /*...*/ } name_t;`). Obviously it is unnecessary in C++, however some types and functions in Nuklear use identical names. In such case C++ assumes these names refer to functions by default and requires type keywords to treat them as types.

A more well-known example of this is POSIX function `stat` which fills `struct stat` with its results.

- https://linux.die.net/man/3/stat
- https://linux.die.net/man/2/stat
- https://www.man7.org/linux/man-pages/man2/stat.2.html

Needless to say, when you write your own code you should avoid such name clashes.

**Why do scope guard types have `explicit operator bool() && = delete;` overload?**

Thanks to the `&&` qualifier, this overload is selected only when the function is called on an *rvalue*. Deleting such overload prevents code like this from compiling:

```cpp
// oops, forgot to hold the window, it will be checked but will die immediately
if (ctx.window_scoped("Title", r, flags)) // error: operator bool() && is deleted
    // code here is outside window
```

and forces to write code like this:

```cpp
// now window has a proper lifetime
// "if" will call "explicit operator bool() const &"
if (auto window = ctx.window_scoped("Title", r, flags); w)
```

If you don't have C++17 2-statement `if`, you can do this:

```cpp
// create an object and use returned value in the same expression
// double () silences "assignment in condition" warning on GCC and Clang
// (double () is not used here because = here means initialization, not assignment)
if (auto window = ctx.window_scoped("Title", r, flags))
```

or this:

```cpp
{ // unnamed scope
    auto window = ctx.window_scoped("Title", r, flags);
    if (window)
        // ...
}
```

**How do I initialize this library if I only got `nk_context* ptr`?**

```cpp
void use_ctx(nk_context* ptr)
{
    nk::context ctx(*ptr);
    // use ctx...
    *ptr = ctx.release();
}
```

**Any built-in backend? How do I add a backend?**

This library does not add any backend by itself. Follow Nuklear's documentation. Nukleus is only a wrapper library and is not affected by backends in any way.

**What do I need to avoid including C and C++ standard library headers?**

- satisfy Nuklear's no-stdlib requirements
- define your own `NUKLEUS_ASSERT` implementation
- define `NUKLEUS_AVOID_STDLIB`

**How do I translate particular Nuklear's C to Nukleus C++?**

Majority of time you simply change `nk_func(ctx, other_args);` to `win.func(other_args);`. Majority of functions are present in the `nk::context` and `nk::window` classes.

In Nuklear, you can pretty much call any function as long as you have the `nk_context` object. However, many functions are not allowed to be called when their respective scope has not been started (`nk_*begin()`) or has just finished (`nk_*end()`). Because of this, Nukleus puts access to such functions in respective classes (many inherit `scope_guard`), requiring to create them first.

General naming convention in the library:

- `*_flags` - if an enum name has such suffix, it means values act as bit flags and multiple can be active at the same time. Such enumerations also have overloaded operators: `&`, `|`, `^`, `~`, `&=`, `|=`, `^=`. There are also `%` overloads that return `bool` and are indented for flag "contains" tests like `if (flags % nk::panel_flags::border)`.
- `*_scoped` - the function returns a scope guard. Scope guards automatically manage `nk_*begin()` and `nk_*end()` calls. Most scope guards should be immediately checked (some do not have overloaded `operator bool` - in such case no check is required as their begin call always succeeds).
- `*_in_place` - the widget modifies supplied argument in-place (e.g. takes `int&` and returns `void` or `bool` instead of taking `int` and returning `int`). Some widgets have both forms.

More complex cases:

```
nk_vec2 => nk::vec2<float>
nk_vec2i => nk::vec2<short>

nk_rect => nk::rect<float>
nk_recti => nk::rect<short>

nk_abc_init_xyz => nk::abc constructor or nk::abc::init_xyz static function

nk_begin => context::window_scoped

nk_handle_*, nk_image_*, nk_nine_slice_*, nk_sub9slice_* => Image and 9-slice make_* functions

nk_tree_state_        push        => window::tree_state_scoped
nk_tree_state_  image_push        => window::tree_state_image_scoped
nk_tree_              push_hashed => window::tree_hashed_scoped
nk_tree_        image_push_hashed => window::tree_image_hashed_scoped
nk_tree_element_      push_hashed => window::tree_element_hashed_scoped
nk_tree_element_image_push_hashed => window::tree_element_image_hashed_scoped

nk_tree_push            => NUKLEUS_TREE_SCOPED
nk_tree_push_id         => NUKLEUS_TREE_ID_SCOPED
nk_tree_image_push      => NUKLEUS_TREE_IMAGE_SCOPED
nk_tree_image_push_id   => NUKLEUS_TREE_IMAGE_ID_SCOPED
nk_tree_element_push    => NUKLEUS_TREE_ELEMENT_SCOPED
nk_tree_element_push_id => NUKLEUS_TREE_ELEMENT_ID_SCOPED
```
