#pragma once
#include "light_enum_config.hpp"
#include <cstddef> // For std::byte
#include <type_traits> // For std::is_enum_v
#include <stdexcept> // For std::logic_error

// Basic types
namespace light_enum::detail {
	using underlying_int_t = int16_t;
	using byte_t = std::byte;
}

// Typeindex
#if LIGHT_ENUM_INTERNAL_USE_BOOST_TYPEINDEX == 1
#include <boost/type_index.hpp>
namespace light_enum { using type_index_t = boost::typeindex::type_index; }
namespace light_enum::detail {
	template <
		typename E, bool OnlyAcceptEnum = true
	>
	[[nodiscard]] auto make_type_index() noexcept -> type_index_t {
		static_assert(std::is_enum_v<E> || OnlyAcceptEnum == false);
		return type_index_t{ boost::typeindex::type_id<E>() };
	}
}
#else
#include <typeindex>
namespace light_enum { using type_index_t = std::type_index; }
namespace light_enum::detail {
	template <
		typename E, bool OnlyAcceptEnum = true
	>
	[[nodiscard]] auto make_type_index() noexcept -> type_index_t {
		static_assert(std::is_enum_v<E> || OnlyAcceptEnum == false);
		return type_index_t{ typeid(E) };
	}
}
#endif




// Simple const span
namespace light_enum::detail {

template <typename T>
class simple_span {
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = pointer;
	using const_iterator = const_pointer;
	constexpr simple_span() noexcept {}
	constexpr explicit simple_span(const_pointer first, size_t n) noexcept
	: first_{ first }, last_{ first + n } {}

	[[nodiscard]] constexpr auto begin() const noexcept { return first_; }
	[[nodiscard]] constexpr auto end() const noexcept { return last_; }
	[[nodiscard]] constexpr auto size() const noexcept { return static_cast<size_t>(last_ - first_); }
	[[nodiscard]] constexpr auto empty() const noexcept { return first_ == last_; }
	[[nodiscard]] constexpr auto data() const noexcept { return first_; }
	[[nodiscard]] constexpr auto& operator[](size_t idx) const noexcept { 
		LIGHT_ENUM_INTERNAL_ASSERT(idx < size());
		return *(first_ + idx); 
	}
	[[nodiscard]] constexpr auto& front() const noexcept { 
		LIGHT_ENUM_INTERNAL_ASSERT(!empty());
		return *first_; 
	}
	[[nodiscard]] constexpr auto& back() const noexcept { 
		LIGHT_ENUM_INTERNAL_ASSERT(!empty());
		return *(last_ - 1); 
	}
	[[nodiscard]] auto at(size_t idx) const -> const_reference {
		if (idx >= size()) {
			throw std::out_of_range{ "out of range" };
		}
		return *(first_ + idx);
	}
	[[nodiscard]] auto as_bytes() const noexcept {
		return simple_span<byte_t>{
			reinterpret_cast<const byte_t*>(first_),
			size() * sizeof(value_type)
		};
	}
private:
	const_pointer first_{ nullptr };
	const_pointer last_{ nullptr };
};


}
