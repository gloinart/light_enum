#pragma once
#include <typeindex>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <optional>

// Type definitions
namespace light_enum::detail {
using underlying_int_t = int16_t;
using byte_t = std::byte;
template <typename T> class span;
}

// Public interface
namespace light_enum {
template <typename E> [[nodiscard]] auto enum_cast(const std::string_view& name) -> std::optional<E>;
template <typename E> [[nodiscard]] auto enum_values() -> detail::span<E>;
template <typename E> [[nodiscard]] auto enum_names() -> const std::vector<std::string>&;
template <typename E> [[nodiscard]] auto enum_name(const E& e) -> std::string_view; // Returns an empty string_view if invalid enum
template <typename E> [[nodiscard]] auto enum_count() -> size_t;
template <typename E> [[nodiscard]] auto enum_index(const E& e) -> std::optional<size_t>;
template <typename E> [[nodiscard]] auto enum_contains(const E& e) -> bool;
template <typename E> [[nodiscard]] auto is_registred() -> bool;
}

















// Simple span
namespace light_enum::detail {
template <typename T>
class span {
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	constexpr span(const_pointer first, const_pointer last) noexcept
	: first_{ first }, last_{ last } {}
	[[nodiscard]] constexpr auto begin() const noexcept { return first_; }
	[[nodiscard]] constexpr auto end() const noexcept { return last_; }
	[[nodiscard]] constexpr auto size() const noexcept { return last_ - first_;}
	[[nodiscard]] constexpr auto empty() const noexcept { return first_ == last_ ;}
	[[nodiscard]] constexpr auto& operator[](size_t idx) const noexcept { return *(first_ + idx);}
	[[nodiscard]] auto at(size_t idx) const -> const_reference {
		if (idx >= size()) {
			throw std::out_of_range{ "out of range" };
		}
		return *(first_ + idx);
	}
private:
	const_pointer first_{nullptr};
	const_pointer last_{nullptr};
};
}



// Registry
namespace light_enum::detail::registry {
[[nodiscard]] auto enum_cast(const std::type_index& ti, const std::string_view& name) -> std::optional<detail::underlying_int_t>;
[[nodiscard]] auto enum_contains(const std::type_index& ti, detail::underlying_int_t v) -> bool;
[[nodiscard]] auto enum_index(const std::type_index& ti, detail::underlying_int_t v) -> std::optional<size_t>;
[[nodiscard]] auto enum_count(const std::type_index& ti) -> size_t;
[[nodiscard]] auto enum_values_int(const std::type_index& ti) -> const std::vector<detail::underlying_int_t>&;
[[nodiscard]] auto enum_names(const std::type_index& ti) -> const std::vector<std::string>&;
[[nodiscard]] auto enum_name(const std::type_index& ti, detail::underlying_int_t value) -> std::string_view;
[[nodiscard]] auto get_blob(const std::type_index& ti) -> const std::vector<detail::byte_t>&;
auto register_enum(
	const std::type_index& ti,
	size_t enum_bytesize,
	bool is_signed,
	std::vector<detail::byte_t> blob,
	std::vector<detail::underlying_int_t> values,
	std::vector<std::string> names
) -> void;
[[nodiscard]] auto is_registered(const std::type_index& ti) -> bool;
};




// Implementation
namespace light_enum {

template <typename E>
auto enum_values() -> detail::span<E> {
	const auto type_index = std::type_index{typeid(E)};
	const auto& blob = detail::registry::get_blob(type_index);
	constexpr auto enum_size = sizeof(E);
	const auto num_elements = blob.size() / enum_size;
	const auto* first = reinterpret_cast<const E*>(blob.data());
	const auto* last = first + num_elements;
	return detail::span<E>{ first, last };
}

template <typename E>
auto enum_names() -> const std::vector<std::string>& {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_names(type_index);
}

template <typename E>
 auto enum_name(const E& e) -> std::string_view {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_name(type_index, static_cast<detail::underlying_int_t>(e));
}

template <typename E>
auto enum_count() -> size_t {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_count(type_index);
}

template <typename E>
auto enum_contains(const E& e) -> bool {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_contains(type_index, static_cast<detail::underlying_int_t>(e));
}

template <typename E>
auto enum_index(const E& e) -> std::optional<size_t> {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_index(type_index, static_cast<detail::underlying_int_t>(e));
}

template <typename E> 
auto enum_cast(const std::string_view& name) -> std::optional<E> {
	const auto type_index = std::type_index{typeid(E)};
	const auto v = detail::registry::enum_cast(type_index, name);
	return v.has_value() ?
		std::optional<E>{static_cast<E>(*v)} :
		std::nullopt;
}

template <typename E> 
auto is_registred() -> bool {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::is_registered(type_index);
}

}