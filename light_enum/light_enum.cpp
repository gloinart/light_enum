#include "light_enum.hpp"
#include <unordered_map>
#include <algorithm>

#if __cplusplus >= 202002L
	#define LIGHT_ENUM_LIKELY [[likely]]
	#define LIGHT_ENUM_UNLIKELY [[unlikely]]
#else
	#define LIGHT_ENUM_LIKELY
	#define LIGHT_ENUM_UNLIKELY
#endif






namespace {

struct enum_data_t {
	std::string name_{};
	std::vector<light_enum::detail::underlying_int_t> values_{};
	std::vector<std::string> names_{};
	std::vector<light_enum::detail::byte_t> blob_{};
	size_t enum_bytesize_{};
	bool is_signed_{};
};

template <typename K, typename V>
using map_t = std::unordered_map<K, V>;

using database_t = map_t<
	std::type_index,
	enum_data_t
>;

[[nodiscard]] auto get_database_mut() -> database_t& {
	static auto s_database = database_t{};
	return s_database;
}
[[nodiscard]] auto get_database() -> const database_t& {
	return std::as_const(get_database_mut());
}

[[nodiscard]] auto get_enum_data(const std::type_index& ti) -> const enum_data_t& {
	const auto& database = get_database();
	const auto it = database.find(ti);
	if (it == database.end()) LIGHT_ENUM_UNLIKELY {
		auto msg = std::string{ "enum not registered: " } + ti.name();
		throw std::out_of_range{ msg };
	}
	return it->second;
}

}














auto light_enum::detail::registry::enum_cast(
	const std::type_index& ti, 
	const std::string_view& name
) -> std::optional<detail::underlying_int_t> {
	const auto& enum_data = get_enum_data(ti);
	for (size_t i = 0, i_end = enum_data.names_.size(); i < i_end; ++i) {
		if (enum_data.names_[i] == name) LIGHT_ENUM_UNLIKELY {
			return enum_data.values_[i];
		}
	}
	return std::nullopt;
}

auto light_enum::detail::registry::enum_contains(
	const std::type_index& ti, 
	detail::underlying_int_t v
) -> bool {
	const auto& enum_data = get_enum_data(ti);
	for (const auto& candidate : enum_data.values_) {
		if (candidate == v) {
			return true;
		}
	}
	return false;
}

auto light_enum::detail::registry::enum_index(
	const std::type_index& ti,
	detail::underlying_int_t v
) -> std::optional<size_t> {
	const auto& enum_data = get_enum_data(ti);
	for (size_t i = 0, i_end = enum_data.values_.size(); i < i_end; ++i) {
		if (enum_data.values_[i] == v) {
			return i;
		}
	}
	return std::nullopt;
}

auto light_enum::detail::registry::enum_count(const std::type_index& ti) -> size_t {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.values_.size();
}

auto light_enum::detail::registry::enum_values_int(
	const std::type_index& ti
) -> const std::vector<detail::underlying_int_t>& {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.values_;
}

auto light_enum::detail::registry::get_blob(
	const std::type_index& ti
) -> const std::vector<detail::byte_t>& {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.blob_;
}

auto light_enum::detail::registry::enum_names(
	const std::type_index& ti
) -> const std::vector<std::string>& {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.names_;
}

auto light_enum::detail::registry::enum_name(
	const std::type_index& ti, 
	const detail::underlying_int_t value
) -> std::string_view {
	const auto& enum_data = get_enum_data(ti);
	for (size_t i = 0, i_end = enum_data.values_.size(); i < i_end; ++i) {
		if (enum_data.values_[i] == value) LIGHT_ENUM_UNLIKELY {
			return std::string_view{ enum_data.names_[i] };
		}
	}
	return {};
}
















auto light_enum::detail::registry::register_enum(
	const std::type_index& ti, 
	size_t enum_bytesize,
	bool is_signed,
	std::vector<detail::byte_t> blob,
	std::vector<detail::underlying_int_t> values,
	std::vector<std::string> names
) -> void {
	if (values.size() != names.size()) LIGHT_ENUM_UNLIKELY {
		auto msg = std::string{"enum error: "} + ti.name();
		throw std::logic_error{ msg };
	}
	auto enum_data = enum_data_t{
		ti.name(),
		std::move(values),
		std::move(names),
		std::move(blob),
		enum_bytesize,
		is_signed
	};
	get_database_mut().emplace(
		ti, 
		std::move(enum_data)
	);
}

auto light_enum::detail::registry::is_registered(const std::type_index& ti) -> bool {
	return get_database().count(ti) > 0;
}
