#pragma once
#include "light_enum_config.hpp"
#include "light_enum_types.hpp"
#include <vector>
#include <string_view>
#include <optional>



// Introspecting enums directly
namespace light_enum {
template <typename E> [[nodiscard]] auto enum_cast(const std::string_view& name) -> std::optional<E>;
template <typename E> [[nodiscard]] auto enum_cast_ci(const std::string_view& name) -> std::optional<E>;
template <typename E> [[nodiscard]] auto enum_cast(const int value) -> std::optional<E>;
template <typename E> [[nodiscard]] auto enum_values() -> detail::simple_span<E>;
template <typename E> [[nodiscard]] auto enum_names() -> detail::simple_span<std::string_view>;
template <typename E> [[nodiscard]] auto enum_names_lowercase() -> detail::simple_span<std::string_view>;
template <typename E> [[nodiscard]] auto enum_name(const E& e) -> std::string_view; // Returns an empty string_view if enum is invalid
template <typename E> [[nodiscard]] auto enum_name_lowercase(const E& e) -> std::string_view; // Returns an empty string_view if enum is invalid
template <typename E> [[nodiscard]] auto enum_count() -> size_t;
template <typename E> [[nodiscard]] auto enum_index(const E& e) -> std::optional<size_t>;
template <typename E> [[nodiscard]] auto enum_contains(const E& e) -> bool;
template <typename E> [[nodiscard]] auto enum_next(const E& e, bool wrap = true) -> std::optional<E>;
template <typename E> [[nodiscard]] auto enum_prev(const E& e, bool wrap = true) -> std::optional<E>;
template <typename E> [[nodiscard]] auto is_registred() -> bool;
}


// Introspecting enums using their typeindex
namespace light_enum::registry {
[[nodiscard]] auto enum_cast(const type_index_t& ti, const std::string_view& name) -> std::optional<detail::underlying_int_t>;
[[nodiscard]] auto enum_cast_ci(const type_index_t& ti, const std::string_view& name) -> std::optional<detail::underlying_int_t>;
[[nodiscard]] auto enum_contains(const type_index_t& ti, detail::underlying_int_t v) -> bool;
[[nodiscard]] auto enum_index(const type_index_t& ti, detail::underlying_int_t v) -> std::optional<size_t>;
[[nodiscard]] auto enum_count(const type_index_t& ti) -> size_t;
[[nodiscard]] auto enum_names(const type_index_t& ti) -> detail::simple_span<std::string_view>;
[[nodiscard]] auto enum_names_lowercase(const type_index_t& ti) -> detail::simple_span<std::string_view>;
[[nodiscard]] auto enum_name(const type_index_t& ti, detail::underlying_int_t value) -> std::string_view;
[[nodiscard]] auto enum_name_lowercase(const type_index_t& ti, detail::underlying_int_t value) -> std::string_view;
[[nodiscard]] auto enum_step(const type_index_t& ti, detail::underlying_int_t value, int32_t steps, bool wrap) -> std::optional<detail::underlying_int_t>;
[[nodiscard]] auto enum_values_raw(const type_index_t& ti) -> detail::simple_span<detail::byte_t>;
[[nodiscard]] auto enum_values(const type_index_t& ti) -> detail::simple_span<detail::underlying_int_t>;
}


// Registry access
namespace light_enum::registry {
[[nodiscard]] auto is_registered(const type_index_t& ti) -> bool;
[[nodiscard]] auto list_registry() -> std::vector<std::pair<std::string_view, type_index_t>>;
[[nodiscard]] auto enumname_to_typeindices(const std::string_view& name) -> const std::vector<type_index_t>&; // A name might correspond to several enums
[[nodiscard]] auto typeindex_to_enumname(const type_index_t& ti) -> std::optional<std::string_view>;
auto register_enum(
	std::string_view name,
	const type_index_t& typeindex_enum,
	const type_index_t& typeindex_underlying,
	size_t enum_bytesize,
	bool is_signed,
	detail::simple_span<detail::byte_t> blob,
	std::vector<detail::underlying_int_t>&& values,
	detail::simple_span<std::string_view> names
) -> void;
};





















// Implementation of public interface
namespace light_enum {

template <typename E>
auto enum_values() -> detail::simple_span<E> {
	const auto type_index = detail::make_type_index<E>();
	const auto blob = registry::enum_values_raw(type_index);
	constexpr auto enum_size = sizeof(E);
	const auto num_elements = blob.size() / enum_size;
	return detail::simple_span<E>{
		reinterpret_cast<const E*>(blob.data()),
		num_elements
	};
}

template <typename E>
auto enum_names() -> detail::simple_span<std::string_view> {
	const auto type_index = detail::make_type_index<E>();
	return registry::enum_names(type_index);
}

template <typename E>
auto enum_names_lowercase() -> detail::simple_span<std::string_view> {
	const auto type_index = detail::make_type_index<E>();
	return registry::enum_names_lowercase(type_index);
}

template <typename E>
auto enum_name(const E& e) -> std::string_view {
	const auto type_index = detail::make_type_index<E>();
	const auto underlying_int = static_cast<detail::underlying_int_t>(e);
	return registry::enum_name(type_index, underlying_int);
}

template <typename E>
auto enum_name_lowercase(const E& e) -> std::string_view {
	const auto type_index = detail::make_type_index<E>();
	const auto underlying_int = static_cast<detail::underlying_int_t>(e);
	return registry::enum_name_lowercase(type_index, underlying_int);
}

template <typename E>
auto enum_count() -> size_t {
	const auto type_index = detail::make_type_index<E>();
	return registry::enum_count(type_index);
}

template <typename E>
auto enum_contains(const E& e) -> bool {
	const auto type_index = detail::make_type_index<E>();
	const auto underlying_int = static_cast<detail::underlying_int_t>(e);
	return registry::enum_contains(type_index, underlying_int);
}

template <typename E>
auto enum_index(const E& e) -> std::optional<size_t> {
	const auto type_index = detail::make_type_index<E>();
	const auto underlying_int = static_cast<detail::underlying_int_t>(e);
	return registry::enum_index(type_index, underlying_int);
}

template <typename E> 
auto enum_cast(const std::string_view& name) -> std::optional<E> {
	const auto type_index = detail::make_type_index<E>();
	const auto v = registry::enum_cast(type_index, name);
	return v.has_value() ?
		std::optional<E>{static_cast<E>(*v)} :
		std::nullopt;
}

template <typename E>
auto enum_cast_ci(const std::string_view& name) -> std::optional<E> {
	const auto type_index = detail::make_type_index<E>();
	const auto v = registry::enum_cast_ci(type_index, name);
	return v.has_value() ?
		std::optional<E>{static_cast<E>(*v)} :
		std::nullopt;
}

template <typename E>
auto enum_cast(const int value) -> std::optional<E> {
	const auto as_enum = static_cast<E>(value);
	return enum_contains(as_enum) ?
		std::optional<E>{as_enum} :
		std::nullopt;
}

template <typename E>
auto enum_next(const E& value, bool wrap) -> std::optional<E> {
	const auto type_index = detail::make_type_index<E>();
	const auto next_value_int = registry::enum_step(
		type_index,
		static_cast<detail::underlying_int_t>(value),
		1,
		wrap
	);
	return next_value_int.has_value() ?
		std::optional<E>{static_cast<E>(*next_value_int)} :
		std::nullopt;
}

template <typename E>
auto enum_prev(const E& value, bool wrap) -> std::optional<E> {
	const auto type_index = detail::make_type_index<E>();
	const auto prev_value_int = registry::enum_step(
		type_index,
		static_cast<detail::underlying_int_t>(value),
		-1,
		wrap
	);
	return prev_value_int.has_value() ?
		std::optional<E>{static_cast<E>(*prev_value_int)} :
		std::nullopt;
}

template <typename E> 
auto is_registred() -> bool {
	const auto type_index = detail::make_type_index<E>();
	return registry::is_registered(type_index);
}

}


#if LIGHT_ENUM_INTERNAL_DEBUG_MODE == 1
namespace light_enum::detail {
auto verify_registered_enum(
	std::string_view enumname,
	simple_span<std::string_view> magic_names,
	simple_span<std::string_view> registered_names,
	simple_span<byte_t> magic_values,
	simple_span<byte_t> registered_values,
	size_t bytes_per_enum
) -> void;
}
#endif