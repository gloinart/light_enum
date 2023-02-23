#pragma once
#include <typeindex>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <optional>

// Type definitions
namespace light_enum::detail {
using generic_int_t = int64_t;
using byte_t = uint8_t;
template <typename T> class span;
}

// Public interface
namespace light_enum {
template <typename E> [[nodiscard]] auto enum_cast(const std::string_view& name) -> std::optional<E>;
template <typename E> [[nodiscard]] auto enum_values() -> detail::span<E>;
template <typename E> [[nodiscard]] auto enum_names() -> const std::vector<std::string>&;
template <typename E> [[nodiscard]] auto enum_name(const E& e) -> std::string_view; // Returns empty string_view if invalid enum
template <typename E> [[nodiscard]] auto enum_count() -> size_t;
template <typename E> [[nodiscard]] auto enum_contains(const E& e) -> bool;
}






// Span
namespace light_enum::detail {
template <typename T>
class span {
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	constexpr span(const T* ptr, size_t num_elements) noexcept
	: ptr_{ ptr }
	, num_elements_{ num_elements }
	{}
	[[nodiscard]] constexpr auto begin() const noexcept { return ptr_; }
	[[nodiscard]] constexpr auto end() const noexcept { return ptr_ + num_elements_; }
	[[nodiscard]] constexpr auto size() const noexcept { return num_elements_;}
	[[nodiscard]] constexpr auto empty() const noexcept { return num_elements_ == 0;}
	[[nodiscard]] constexpr auto& operator[](size_t idx) const noexcept { return *(ptr_ + idx);}
	[[nodiscard]] auto& at(size_t idx) const {
		if (idx >= num_elements_) {
			throw std::out_of_range{ "out of range" };
		}
		return *(ptr_ + idx);
	}
private:
	const T* ptr_{nullptr};
	size_t num_elements_{0};
};
}



// Registry
namespace light_enum::detail::registry {
[[nodiscard]] auto enum_cast(const std::type_index& ti, const std::string_view& name) -> std::optional<detail::generic_int_t>;
[[nodiscard]] auto enum_contains(const std::type_index& ti, detail::generic_int_t v) -> bool;
[[nodiscard]] auto enum_count(const std::type_index& ti) -> size_t;
[[nodiscard]] auto enum_values_int(const std::type_index& ti) -> const std::vector<detail::generic_int_t>&;
[[nodiscard]] auto enum_names(const std::type_index& ti) -> const std::vector<std::string>&;
[[nodiscard]] auto enum_name(const std::type_index& ti, detail::generic_int_t value) -> std::string_view;
[[nodiscard]] auto get_blob(const std::type_index& ti) -> const std::vector<detail::byte_t>&;
auto register_enum(
	const std::type_index& ti,
	size_t enum_bytesize,
	std::vector<detail::byte_t> blob,
	std::vector<detail::generic_int_t> values,
	std::vector<std::string> names
) -> void;
[[nodiscard]] auto is_registered(const std::type_index& ti) -> bool;
};




// Public interface implementations
namespace light_enum {

template <typename E>
[[nodiscard]] auto enum_values() -> detail::span<E> {
	const auto type_index = std::type_index{typeid(E)};
	const auto& blob = detail::registry::get_blob(type_index);
	const auto enum_size = sizeof(E);
	const auto num_elements = blob.size() / enum_size;
	const auto* ptr = reinterpret_cast<const E*>(blob.data());
	return detail::span<E>{ ptr, num_elements };
}

template <typename E>
[[nodiscard]] auto enum_names() -> const std::vector<std::string>& {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_names(type_index);
}

template <typename E>
[[nodiscard]] auto enum_name(const E& e) -> std::string_view {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_name(type_index, static_cast<detail::generic_int_t>(e));
}

template <typename E>
[[nodiscard]] auto enum_count() -> size_t {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_count(type_index);
}

template <typename E>
[[nodiscard]] auto enum_contains(const E& e) -> bool {
	const auto type_index = std::type_index{typeid(E)};
	return detail::registry::enum_contains(type_index, static_cast<detail::generic_int_t>(e));
}

template <typename E> 
[[nodiscard]] auto enum_cast(const std::string_view& name) -> std::optional<E> {
	const auto type_index = std::type_index{typeid(E)};
	const auto v = detail::registry::enum_cast(type_index, name);
	return v.has_value() ?
		std::optional<E>{static_cast<E>(*v)} :
		std::nullopt;
}


}