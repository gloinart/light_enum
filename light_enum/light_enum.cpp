#include "light_enum.hpp"
#include <unordered_map>
#include <algorithm>
#include <memory>

#ifndef LIGHT_ENUM_ASSERT
	#ifdef NDEBUG
		#define LIGHT_ENUM_ASSERT(x)
	#else
		#include <cassert>
		#define LIGHT_ENUM_ASSERT(x) assert(x)
	#endif
#endif

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
	std::unique_ptr<std::vector<std::string>> names_as_strings_{}; // Needs to stay fixed in memory even if map moves objects around
	std::vector<std::string_view> names_as_string_views_{}; // Points to names_as_strings_
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
	LIGHT_ENUM_ASSERT(enum_data.names_as_strings_ != nullptr);
	const auto& names = *enum_data.names_as_strings_;
	const auto it = std::find(
		names.begin(),
		names.end(),
		name
	);
	if (it == names.end()) LIGHT_ENUM_UNLIKELY {
		return std::nullopt;
	}
	const auto idx = static_cast<size_t>(it - names.begin());
	LIGHT_ENUM_ASSERT(idx < enum_data.values_.size());
	return enum_data.values_[idx];
}

auto light_enum::detail::registry::enum_contains(
	const std::type_index& ti,
	const detail::underlying_int_t value
) -> bool {
	const auto& values = get_enum_data(ti).values_;
	// Note that values are ordered
	const auto it = std::find_if(
		values.begin(),
		values.end(),
		[&value](const auto& candidate) noexcept { return candidate >= value; }
	);
	return
		it == values.end() ? false :
		*it == value ? true :
		false;
}

auto light_enum::detail::registry::enum_index(
	const std::type_index& ti,
	const detail::underlying_int_t value
) -> std::optional<size_t> {
	const auto& values = get_enum_data(ti).values_;
	// Note that values are ordered
	const auto it = std::find_if(
		values.begin(),
		values.end(),
		[&value](const auto& candidate) noexcept { return candidate >= value; }
	);
	const auto idx =
		it == values.end() ? std::optional<size_t>{} :
		*it != value ? std::optional<size_t>{} :
		static_cast<size_t>(it - values.begin());
	LIGHT_ENUM_ASSERT(
		idx == std::nullopt ||
		idx.value() < values.size()
	);
	return idx;
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
) -> detail::span<std::string_view> {
	const auto& enum_data = get_enum_data(ti);
	const auto& svs = enum_data.names_as_string_views_;
	return detail::span<std::string_view>{
		svs.data(),
		svs.data() + svs.size()
	};
}

auto light_enum::detail::registry::enum_name(
	const std::type_index& ti, 
	const detail::underlying_int_t value
) -> std::string_view {
	const auto& enum_data = get_enum_data(ti);
	const auto& values = enum_data.values_;
	// Note that values are ordered
	const auto it = std::find_if(
		values.begin(),
		values.end(),
		[&value](const auto& candidate) noexcept { return candidate >= value; }
	);
	const auto value_found =
		it == values.end() ? false :
		*it != value ? false :
		true;
	if(!value_found) LIGHT_ENUM_UNLIKELY {
		return std::string_view{};
	}
	const auto idx = static_cast<size_t>(it - values.begin());
	LIGHT_ENUM_ASSERT(idx < values.size());
	LIGHT_ENUM_ASSERT(idx < enum_data.names_as_string_views_.size());
	LIGHT_ENUM_ASSERT(
		enum_data.names_as_string_views_[idx] == 
		enum_data.names_as_strings_->at(idx)
	);
	return enum_data.names_as_string_views_[idx];
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
	// Using a unique_ptr to force the vector to stay fixed in memory
	// as string_views are pointers to them.
	auto names_as_strings_uptr = std::make_unique<std::vector<std::string>>(
		std::move(names)
	);
	auto names_as_string_views = std::vector<std::string_view>{};
	names_as_string_views.reserve(names_as_strings_uptr->size());
	for (const auto& name : *names_as_strings_uptr) {
		names_as_string_views.emplace_back(name);
	}
	auto enum_data = enum_data_t{
		ti.name(),
		std::move(values),
		std::move(names_as_strings_uptr),
		std::move(names_as_string_views),
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
