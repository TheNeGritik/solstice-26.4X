// Compatibility shim for building Crypto++ with Visual Studio 18 (MSVC 19.50+).
//
// Visual Studio 18 removed the legacy stdext::checked_array_iterator /
// stdext::unchecked_array_iterator helpers (see STL4043), but Crypto++ still
// references them in integer.cpp and zdeflate.cpp. The algorithms they are
// passed to accept raw pointers directly, so returning the pointer unchanged
// preserves the previous behaviour.
//
// This header is force-included into the Crypto++ target only (see
// CMakeLists.txt). On toolchains that still provide the helpers it expands to
// nothing.
#pragma once

#if defined(_MSC_VER) && _MSC_VER >= 1950

#include <cstddef>

namespace stdext
{
template <class T>
constexpr T* make_checked_array_iterator(T* ptr, std::size_t /*size*/) noexcept
{
    return ptr;
}

template <class T>
constexpr T* make_unchecked_array_iterator(T* ptr) noexcept
{
    return ptr;
}
} // namespace stdext

#endif
