#include "light_enum.hpp"
#include <unordered_map>

namespace {

struct enum_data_t {
	std::vector<light_enum::detail::generic_int_t> values_{};
	std::vector<std::string> names_{};
	std::vector<light_enum::detail::byte_t> blob_{};
	size_t size_{};
};


template <typename K, typename V>
using map_t = std::unordered_map<K, V>;

using database_t = map_t<
	std::type_index,
	enum_data_t
>;

auto& get_database_mut() {
	static auto s_database = database_t{};
	return s_database;
}
const auto& get_database() {
	return std::as_const(get_database_mut());
}

[[nodiscard]] auto get_enum_data(const std::type_index& ti) -> const enum_data_t& {
	const auto& database = get_database();
	const auto it = database.find(ti);
	if (it == database.end()) {
		throw std::out_of_range{ "enum not registered" };
	}
	return it->second;
}


}


auto light_enum::detail::registry::enum_cast(
	const std::type_index& ti, 
	const std::string_view& name
) -> std::optional<detail::generic_int_t> {
	const auto& enum_data = get_enum_data(ti);
	for (size_t i = 0; i < enum_data.names_.size(); ++i) {
		if (enum_data.names_[i] == name) {
			return enum_data.values_[i];
		}
	}
	return std::nullopt;
}

auto light_enum::detail::registry::enum_contains(
	const std::type_index& ti, 
	detail::generic_int_t v
) -> bool {
	const auto& enum_data = get_enum_data(ti);
	for (const auto& candidate : enum_data.values_) {
		if (candidate == v) {
			return true;
		}
	}
	return false;
}

auto light_enum::detail::registry::enum_count(const std::type_index& ti) -> size_t {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.values_.size();
}

auto light_enum::detail::registry::enum_values_int(
	const std::type_index& ti
) -> const std::vector<detail::generic_int_t>& {
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
	const detail::generic_int_t value
) -> std::string_view {
	const auto& enum_data = get_enum_data(ti);
	const auto& values = enum_data.values_;
	const auto& names = enum_data.names_;
	for (size_t i = 0; i < values.size(); ++i) {
		if (values[i] == value) {
			return std::string_view{ names[i] };
		}
	}
	return {};
}

auto light_enum::detail::registry::register_enum(
	const std::type_index& ti, 
	size_t size, 
	std::vector<detail::byte_t> blob,
	std::vector<detail::generic_int_t> values,
	std::vector<std::string> names
) -> void {
	auto enum_data = enum_data_t{
		std::move(values),
		std::move(names),
		std::move(blob),
		size
	};
	get_database_mut().emplace(
		ti, 
		std::move(enum_data)
	);
}

auto light_enum::detail::registry::is_registered(const std::type_index& ti) -> bool {
	return get_database().count(ti) > 0;
}
