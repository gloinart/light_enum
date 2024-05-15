#pragma once
#include "light_enum_config.hpp"
#include "light_enum.hpp"
#include "magic_enum/magic_enum.hpp"
#include <type_traits>

// Public interface

// Use light_enum::register_enum<E>() where suitable to register an enum
namespace light_enum { template <typename E> auto register_enum() -> void; }

// Use LIGHT_ENUM_REGISTER(E) macro to register enum at global scope, 
// before main is executed.
#define LIGHT_ENUM_REGISTER(E)                                   \
	[[maybe_unused]] const auto                                    \
	LIGHT_ENUM_INTERNAL_CONCAT(light_enum_registered_, __LINE__) = \
	[]() {                                                         \
		light_enum::register_enum<E>();                              \
		return light_enum::detail::light_enum_registered_tag{};      \
	}();





// Use LIGHT_ENUM_REGISTER_RANGE(E) macro to register enum at global scope, 
// before main is executed.
#define LIGHT_ENUM_REGISTER_RANGE(E, MINVALUE, MAXVALUE)               \
	template <>                                                          \
	struct magic_enum::customize::enum_range<E> {                        \
		static constexpr int min = (MINVALUE);                             \
		static constexpr int max = (MAXVALUE);                             \
	};                                                                   \
	[[maybe_unused]] const auto                                          \
	LIGHT_ENUM_INTERNAL_CONCAT(light_enum_registered_range_, __LINE__) = \
	[]() {                                                               \
		light_enum::register_enum<E>();                                    \
		return light_enum::detail::light_enum_registered_tag{};            \
	}();






















// Implementation
namespace light_enum::detail { struct light_enum_registered_tag {}; }
#define LIGHT_ENUM_INTERNAL_CONCAT_IMPL(a, b) a ## b
#define LIGHT_ENUM_INTERNAL_CONCAT(a, b) LIGHT_ENUM_INTERNAL_CONCAT_IMPL(a, b)



namespace light_enum {

template <typename E>
inline auto register_enum() -> void {
	static_assert(std::is_enum_v<E>, "E is not an enum");
	const auto type_index = detail::make_type_index<E>();
	using underlying_type = std::underlying_type_t<E>;
	constexpr auto magic_values = magic_enum::enum_values<E>();
	constexpr auto magic_names = magic_enum::enum_names<E>();
	constexpr auto num_elements = magic_values.size();
	constexpr auto enum_size_bytes = sizeof(E);
	constexpr auto num_bytes = num_elements * enum_size_bytes;
	static_assert(
		num_elements > 0,
		"light_enum error: cannot register an enum without values"
	);
#if LIGHT_ENUM_INTERNAL_ABORT_ON_SMALL_RANGE == 1
	{
		// Verify range-max is large enough
		{
			constexpr int max_value = magic_enum::customize::enum_range<E>::max;
			constexpr auto highest_value = static_cast<int>(magic_values.back());
			static_assert(
				highest_value < max_value,
				"\n"
				"Enums migh be missing, enlarge max value for range\n"
				"Use LIGHT_ENUM_REGISTER_RANGE(enum, min_value, max_value+1)"
				"\n"
			);
		}
		// Verify range-max is small enough
		{
			constexpr int min_value = magic_enum::customize::enum_range<E>::min;
			constexpr auto smallest_value = static_cast<int>(magic_values.front());
			constexpr auto is_small_enough =
				smallest_value >= 0 ? true :
				smallest_value > min_value ? true :
				false;
			static_assert(
				is_small_enough,
				"\n"
				"Enums migh be missing, smallen min value for range\n"
				"\n"
			);
		}
	}
#endif
	// Blob of raw values
	const auto blob = detail::simple_span<detail::byte_t>{
		reinterpret_cast<const detail::byte_t*>(magic_values.data()),
		num_bytes
	};
	// Values as integers
	auto values_int = std::vector<detail::underlying_int_t>{};
	values_int.reserve(num_elements);
	for (const auto& e : magic_values) {
		const auto underlying_int = static_cast<detail::underlying_int_t>(e);
		values_int.emplace_back(underlying_int);
	}
	// Names
	const auto names = detail::simple_span<std::string_view>{
		magic_names.data(),
		magic_names.size()
	};
	registry::register_enum(
		magic_enum::enum_type_name<E>(),
		type_index, 
		detail::make_type_index<underlying_type, false>(),
		enum_size_bytes,
		std::is_signed_v<underlying_type>,
		blob, 
		std::move(values_int), 
		names
	);

	// Test
#if LIGHT_ENUM_INTERNAL_DEBUG_MODE == 1
	{
		const auto registered_values = light_enum::enum_values<E>();
		const auto registered_names = light_enum::enum_names<E>();
		detail::verify_registered_enum(
			magic_enum::enum_type_name<E>(),
			detail::simple_span<std::string_view>{ magic_names.data(), magic_names.size() },
			registered_names,
			detail::simple_span{ magic_values.data(), magic_values.size() }.as_bytes(),
			registered_values.as_bytes(),
			sizeof(E)
		);
	}
#endif
}



}


