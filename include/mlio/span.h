/*
 * Copyright 2019-2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not use this file except in compliance with the License. A copy of
 * the License is located at
 *
 *      http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * ANY KIND, either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 */

#pragma once

#include <cstddef>
#include <type_traits>

#include "mlio/config.h"
#include "mlio/type_traits.h"

namespace mlio {
inline namespace abi_v1 {
namespace stdx {
namespace detail {

using mlio::detail::Is_container;

template<typename Container, typename Element_type, typename = void>
struct Is_compatible_container : std::false_type {};

template<typename Container, typename Element_type>
struct Is_compatible_container<
    Container,
    Element_type,
    std::enable_if_t<
        std::is_convertible<std::remove_pointer_t<decltype(std::declval<Container>().data())> (*)[],
                            Element_type (*)[]>::value>> : std::true_type {};

}  // namespace detail

template<typename T>
class MLIO_API span {
public:
    using element_type = T;
    using value_type = typename std::remove_cv<T>::type;
    using index_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = T *;
    using const_iterator = const T *;

    constexpr span() noexcept = default;

    constexpr span(const span &other) noexcept = default;

    constexpr span &operator=(const span &) noexcept = default;

    template<typename U>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(const span<U> &other) noexcept : data_{other.data()}, size_{other.size()}
    {
        static_assert(std::is_convertible<U(*)[], element_type(*)[]>::value,
                      "The element type of U must be convertible to the element type of the span.");
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(pointer data, index_type size) noexcept : data_{data}, size_{size}
    {}

    constexpr span(pointer first, pointer last) noexcept
        : data_{first}, size_{static_cast<index_type>(last - first)}
    {}

    template<typename Container>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(Container &container)
        : data_{container.data()}, size_{static_cast<index_type>(container.size())}
    {
        static_assert(detail::Is_container<Container>::value,
                      "Container must have data() and size() accessors.");

        static_assert(
            detail::Is_compatible_container<Container, element_type>::value,
            "The element type of Container must be convertible to the element type of the span.");
    }

    template<typename Container>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(const Container &container)
        : data_{container.data()}, size_{static_cast<index_type>(container.size())}
    {
        static_assert(detail::Is_container<Container>::value,
                      "Container must have data() and size() accessors.");

        static_assert(
            detail::Is_compatible_container<Container const, element_type>::value,
            "The element type of Container must be convertible to the element type of the span.");
    }

    ~span() = default;

    constexpr span<element_type> subspan(index_type offset) const
    {
        return {data_ + offset, size_ - offset};
    }

    constexpr span<element_type> subspan(index_type offset, index_type count) const
    {
        return {data_ + offset, count};
    }

    constexpr span<element_type> first(index_type count) const
    {
        return {data_, count};
    }

    constexpr span<element_type> last(index_type count) const
    {
        return {data_ + size_ - count, count};
    }

    constexpr reference operator[](index_type index) const
    {
        return data_[index];
    }

    constexpr iterator begin() const noexcept
    {
        return data_;
    }

    constexpr iterator end() const noexcept
    {
        return data_ + size_;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return data_;
    }

    constexpr const_iterator cend() const noexcept
    {
        return data_ + size_;
    }

    constexpr pointer data() const noexcept
    {
        return data_;
    }

    constexpr index_type size() const noexcept
    {
        return size_;
    }

    constexpr index_type size_bytes() const noexcept
    {
        return size_ * sizeof(element_type);
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size_ == 0;
    }

private:
    pointer data_{};
    index_type size_{};
};

template<class T>
MLIO_API
inline constexpr span<const std::byte> as_bytes(span<T> s) noexcept
{
    return {reinterpret_cast<const std::byte *>(s.data()), s.size_bytes()};
}

template<class T>
MLIO_API
inline constexpr span<std::byte> as_writable_bytes(span<T> s) noexcept
{
    static_assert(!std::is_const<T>::value,
                  "The element type of the specified span must be non-const.");

    return {reinterpret_cast<std::byte *>(s.data()), s.size_bytes()};
}

}  // namespace stdx

template<typename T, typename U>
MLIO_API
inline constexpr stdx::span<T> as_span(stdx::span<U> s) noexcept
{
    static_assert(std::is_const<T>::value || !std::is_const<U>::value,
                  "T must be const or the element type of the specified span must be non-const.");

    return {reinterpret_cast<T *>(s.data()), s.size_bytes() / sizeof(T)};
}

template<typename Container>
MLIO_API
inline constexpr decltype(auto) make_span(Container &container) noexcept
{
    static_assert(detail::Is_container<Container>::value,
                  "Container must have data() and size() accessors.");

    return stdx::span<std::remove_pointer_t<decltype(container.data())>>{container};
}

using Memory_span = stdx::span<const std::byte>;

using Mutable_memory_span = stdx::span<std::byte>;

}  // namespace abi_v1
}  // namespace mlio
