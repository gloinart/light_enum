#pragma once
#include "light_enum.hpp"
#include "magic_enum/magic_enum.hpp"
#include <type_traits>

// Public interface
namespace light_enum {
template <typename E> auto register_enum() -> void;
}

#define LIGHT_ENUM_REGISTER(E)                                \
	[[maybe_unused]] const auto                                 \
	LIGHT_ENUM_DETAIL_CONCAT(light_enum_registered, __LINE__) = \
	[]() {                                                      \
		light_enum::register_enum<E>();                           \
		return true;                                              \
	}();
















// Implementation
#define LIGHT_ENUM_DETAIL_CONCAT_IMPL(a, b) a ## b
#define LIGHT_ENUM_DETAIL_CONCAT(a, b) LIGHT_ENUM_DETAIL_CONCAT_IMPL(a, b)

namespace light_enum {

template <typename E>
auto register_enum() -> void {
	static_assert(std::is_enum_v<E>);
	const auto type_index = std::type_index{ typeid(E) };
	if (detail::registry::is_registered(type_index)) {
		return;
	}
	using underlying_type = std::underlying_type_t<E>;
	constexpr auto values = magic_enum::enum_values<E>();
	constexpr auto enum_size = sizeof(E);
	constexpr auto num_elements = magic_enum::enum_count<E>();
	constexpr auto num_bytes = num_elements*enum_size;
	static_assert(
		num_elements > 0,
		"light_enum error: cannot register an enum without values"
	);
	auto blob = std::vector<detail::byte_t>{};
	blob.resize(num_bytes);
	if (num_bytes > 0) {
		std::memcpy(
			blob.data(),
			values.data(),
			num_bytes
		);
	}
	// Values as integers
	auto values_int = std::vector<detail::underlying_int_t>{};
	values_int.reserve(num_elements);
	for (const auto& e : values) {
		values_int.emplace_back(static_cast<detail::underlying_int_t>(e));
	}
	// Names
	auto names = std::vector<std::string>{};
	names.reserve(num_elements);
	for (const auto& name : magic_enum::enum_names<E>()) {
		names.emplace_back(name);
	}
	detail::registry::register_enum(
		type_index, 
		enum_size, 
		std::is_signed_v<underlying_type>,
		blob, 
		values_int, 
		names
	);
}



}


