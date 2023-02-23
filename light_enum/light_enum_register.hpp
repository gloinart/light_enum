#pragma once
#include "light_enum.hpp"
#include "magic_enum/magic_enum.hpp"

// Interface
namespace light_enum {
template <typename E> auto register_enum() -> void;
template <typename E> [[nodiscard]] auto is_registred() -> bool;
}
#define LIGHT_ENUM_REGISTER(E)                                    \
	[[maybe_unused]] const auto light_enum_initialized_##E = []() { \
		light_enum::register_enum<E>();                               \
		return true;                                                  \
	}();







// Implementation
namespace light_enum {

template <typename E>
auto register_enum() -> void {
	const auto values = magic_enum::enum_values<E>();
	const auto enum_size = sizeof(E);
	const auto num_elements = values.size();
	const auto num_bytes = num_elements*enum_size;
	auto blob = std::vector<detail::byte_t>{};
	blob.resize(num_bytes);
	if (num_bytes > 0) {
		std::memcpy(
			blob.data(),
			values.data(),
			num_bytes
		);
	}
	auto values_int = std::vector<detail::generic_int_t>{};
	values_int.reserve(num_elements);
	for (const auto& e : values) {
		values_int.push_back(static_cast<detail::generic_int_t>(e));
	}
	const auto type_index = std::type_index{ typeid(E) };
	auto names = std::vector<std::string>{};
	names.reserve(num_elements);
	for (const auto& name : magic_enum::enum_names<E>()) {
		names.emplace_back(name);
	}
	detail::registry::register_enum(type_index, enum_size, blob, values_int, names);
}

template <typename E> auto is_registred() -> bool {
	const auto type_index = std::type_index{ typeid(E) };
	return detail::registry::is_registered(type_index);
}


}


