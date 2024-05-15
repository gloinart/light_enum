
/*
Include this file *ONCE* in any .cpp file in your project.
*/


#ifdef LIGHT_ENUM_INTERNAL_IMPL
	static_assert(false, "light_enum_impl.hpp already included");
#endif
#define LIGHT_ENUM_INTERNAL_IMPL

#include "light_enum_config.hpp"
#include "light_enum.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <cctype>
#include <numeric>
#include <string>


static_assert(
	LIGHT_ENUM_INTERNAL_DEBUG_MODE == 0 || 
	LIGHT_ENUM_INTERNAL_DEBUG_MODE == 1
);



#if __cplusplus >= 202002L
	#define LIGHT_ENUM_LIKELY [[likely]]
	#define LIGHT_ENUM_UNLIKELY [[unlikely]]
#else
	#define LIGHT_ENUM_LIKELY
	#define LIGHT_ENUM_UNLIKELY
#endif



#ifndef LIGHT_ENUM_INTERNAL_ALIAS_MAP_BASE
	#include <unordered_map>
	#define LIGHT_ENUM_INTERNAL_ALIAS_MAP_BASE std::unordered_map
#endif


static_assert(
	LIGHT_ENUM_INTERNAL_USE_BOOST_TYPEINDEX == 0 || 
	LIGHT_ENUM_INTERNAL_USE_BOOST_TYPEINDEX == 1
);
#if LIGHT_ENUM_INTERNAL_USE_BOOST_TYPEINDEX == 1
namespace {
struct typeindex_hash_f {
	[[nodiscard]] auto operator()(
		const light_enum::type_index_t& ti
	) const noexcept -> size_t {
		return ti.hash_code();
	}
};
}
#else
namespace {
	using typeindex_hash_f = std::hash<std::type_index>;
}
#endif



namespace {
using namespace std::string_view_literals;

// Only here to reduce allocations
[[nodiscard]] auto concat_strings(
	const std::string_view& a,
	const std::string_view& b = {},
	const std::string_view& c = {},
	const std::string_view& d = {},
	const std::string_view& e = {},
	const std::string_view& f = {}
) -> std::string {
	auto str = std::string{};
	str.reserve(a.size() + b.size() + c.size() + d.size() + e.size() + f.size());
	str.append(a);
	str.append(b);
	str.append(c);
	str.append(d);
	str.append(e);
	str.append(f);
	return str;
}

template <typename target_t, typename src_t>
[[nodiscard]] auto internal_cast(const src_t& src) noexcept -> target_t {
	static_assert(std::is_integral_v<target_t>, "");
	static_assert(std::is_integral_v<src_t>, "");
	const auto casted = static_cast<target_t>(src);
	LIGHT_ENUM_INTERNAL_ASSERT(static_cast<src_t>(casted) == src);
	return casted;
}

template <typename T>
[[nodiscard]] auto internal_tolower(const T& c) -> T {
	return internal_cast<T>(std::tolower(c));
}

[[nodiscard]] auto to_lowercase(const std::string_view& sv) -> std::string {
	auto lowercase = std::string{};
	lowercase.resize(sv.size());
	std::transform(
		sv.begin(),
		sv.end(),
		lowercase.begin(),
		[](const auto& c) noexcept { return internal_tolower(c); }
	);
	return lowercase;
}

using byte_t = light_enum::detail::byte_t;
using underlying_int_t = light_enum::detail::underlying_int_t;
template <typename T>
using simple_span = light_enum::detail::simple_span<T>;
using type_index_t = light_enum::type_index_t;
enum class is_index_equal_to_value_t { False, True };
enum class is_signed_t { False, True };
using stringview_to_value_map_t = LIGHT_ENUM_INTERNAL_ALIAS_MAP_BASE<
	std::string_view, 
	underlying_int_t
>;
using string_to_value_map_t = LIGHT_ENUM_INTERNAL_ALIAS_MAP_BASE<
	std::string,
	underlying_int_t
>;
using enumname_to_typeindices_map_t = LIGHT_ENUM_INTERNAL_ALIAS_MAP_BASE<
	std::string_view,
	std::vector<type_index_t>
>;

struct enum_data_t {
	// Enum type info
	std::unique_ptr<std::string> enumname_{}; // unique_ptr in order to ensure fixed in memory
	std::string_view enumname_string_view_{};
	type_index_t underlying_typeindex_;
	size_t enum_bytesize_{};
	is_signed_t is_signed_{};
	is_index_equal_to_value_t is_index_equals_to_value_{};

	// Values
	std::unique_ptr<std::vector<underlying_int_t>> values_{};// unique_ptr in order to ensure fixed in memory
	simple_span<underlying_int_t> values_span_{}; // points to values_, only here as a micro-optimization

	// Names
	std::unique_ptr<std::string> bigstring_names_{}; // a single string is used instead of a vector of std::string for optimization purposes, unique_ptr in order to ensure fixed in memory
	std::unique_ptr<std::vector<std::string_view>> names_stringviews_{}; // unique_ptr in order to ensure fixed in memory, points to names_as_strings_
	simple_span<std::string_view> names_span_{}; // Points to names_stringviews_, only here as a micro-optimization
	stringview_to_value_map_t name_to_value_{}; // For fast lookup of large enums

	// Names lowercase
	std::unique_ptr<const std::string> bigstring_names_lowercase_{}; // a single string is used instead of a vector of std::string for optimization purposes, unique_ptr in order to ensure fixed in memory
	std::unique_ptr<std::vector<std::string_view>> names_lowercase_stringviews_{}; // unique_ptr in order to ensure fixed in memory, points to names_as_strings_
	simple_span<std::string_view> names_lowercase_span_{}; // Points to names_lowercase_stringviews_, only here as a micro-optimization
	stringview_to_value_map_t name_lowercase_to_value_{}; // For enum_cast_ci

	// Raw blob (used to reinterpret_cast to enum E)
	std::unique_ptr<underlying_int_t[]> rawblob_{}; // unique_ptr in order to ensure fixed in memory, underlying_int for 4 byte alignment
	simple_span<byte_t> rawblob_span_{}; // Points to rawblob_
};
using typeindex_to_enumdata_map_t = LIGHT_ENUM_INTERNAL_ALIAS_MAP_BASE<
	type_index_t,
	enum_data_t,
	typeindex_hash_f
>;


struct internal_state {
	typeindex_to_enumdata_map_t typeindex_to_enumdata_{};
	enumname_to_typeindices_map_t enumname_to_typeindices_{};
};

[[nodiscard]] auto get_state_mut() -> internal_state& {
	static auto s_state = internal_state{};
	return s_state;
}
[[nodiscard]] auto get_state() -> const internal_state& {
	return std::as_const(get_state_mut());
}

[[nodiscard]] auto get_enum_data(const type_index_t& ti) -> const enum_data_t& {
	const auto& typeindex_to_enumdata = get_state().typeindex_to_enumdata_;
	const auto it = typeindex_to_enumdata.find(ti);
	if (it == typeindex_to_enumdata.end()) LIGHT_ENUM_UNLIKELY {
		throw std::logic_error{ concat_strings("enum not registered: "sv, ti.name())};
	}
	return it->second;
}





[[nodiscard]] auto value_to_index(
	const enum_data_t& enum_data, 
	const underlying_int_t& value
) noexcept -> std::optional<size_t> {
	const auto& values = enum_data.values_span_;
	LIGHT_ENUM_INTERNAL_ASSERT(std::is_sorted(values.begin(), values.end()));
	// Simply cast if value and index is equal
	if (enum_data.is_index_equals_to_value_ == is_index_equal_to_value_t::True) LIGHT_ENUM_LIKELY {
		const auto sz = internal_cast<int32_t>(values.size());
		const auto is_value_found = value >= 0 && value < sz;
		return is_value_found ?
			std::optional<size_t>{internal_cast<size_t>(value)} :
			std::nullopt;
	}
	// Note that values are ordered
	auto linear_search_f = [&]() noexcept {
		return std::find_if(
			values.begin(),
			values.end(),
			[&value](const underlying_int_t& candidate) noexcept -> bool {
				return candidate >= value;
			}
		);
	};
	auto binary_search_f = [&]() noexcept {
		return std::lower_bound(values.begin(), values.end(), value);
	};
	// Select search method depending on number of values (micro-optimization)
	constexpr auto klinear_search_threshold = size_t{ 32 };
	const auto it = values.size() <= klinear_search_threshold ?
		linear_search_f() :
		binary_search_f();
	const auto is_value_found =
		it == values.end() ? false :
		*it != value ? false :
		true;
	return is_value_found ?
		std::optional<size_t>{internal_cast<size_t>(it - values.begin())} :
		std::nullopt;
}





}

































auto light_enum::registry::enum_cast(
	const type_index_t& ti, 
	const std::string_view& name
) -> std::optional<detail::underlying_int_t> {
	const auto& enum_data = get_enum_data(ti);
	LIGHT_ENUM_INTERNAL_ASSERT(enum_data.bigstring_names_ != nullptr);
	// Use hash table for large enums
	constexpr auto klinear_search_threshold = size_t{ 8 };
	if (enum_data.values_span_.size() > klinear_search_threshold) {
		const auto it = enum_data.name_to_value_.find(name);
		return it != enum_data.name_to_value_.end() ?
			std::optional<underlying_int_t>{ it->second } :
			std::nullopt;
	}
	// Use linear search for small enums
	else {
		const auto& names = enum_data.names_span_;
		const auto it = std::find(
			names.begin(),
			names.end(),
			name
		);
		if (it == names.end()) LIGHT_ENUM_UNLIKELY {
			return std::nullopt;
		}
		const auto idx = internal_cast<size_t>(it - names.begin());
		LIGHT_ENUM_INTERNAL_ASSERT(idx < enum_data.values_span_.size());
		return enum_data.values_span_[idx];
	}
}

auto light_enum::registry::enum_cast_ci(
	const type_index_t& ti, 
	const std::string_view& name
) -> std::optional<detail::underlying_int_t> {
	if (name.empty()) LIGHT_ENUM_UNLIKELY {
		return std::nullopt;
	}
	const auto first_lower = internal_tolower(name.front());
	auto is_match_f = [&](const std::string_view& candidate) noexcept -> bool {
		LIGHT_ENUM_INTERNAL_ASSERT(!candidate.empty());
		// Quick compare size
		if (name.size() != candidate.size()) LIGHT_ENUM_LIKELY {
			return false;
		}
		// Quick compare first character
		if (first_lower != candidate.front()) LIGHT_ENUM_LIKELY {
			return false;
		}
		// Compare the rest
		return std::equal(
			std::next(name.begin()),
			name.end(),
			std::next(candidate.begin()),
			[](const auto& a, const auto& b) noexcept {
				return internal_tolower(a) == b;
			}
		);
	};
	const auto& enum_data = get_enum_data(ti);
	const auto& names = enum_data.names_lowercase_span_;
	const auto it = std::find_if(
		names.begin(),
		names.end(),
		is_match_f
	);
	if (it == names.end()) LIGHT_ENUM_UNLIKELY {
		return std::nullopt;
	}
	const auto idx = internal_cast<size_t>(it - names.begin());
	LIGHT_ENUM_INTERNAL_ASSERT(idx < enum_data.values_span_.size());
	return enum_data.values_span_[idx];
}



auto light_enum::registry::enum_contains(
	const type_index_t& ti,
	const detail::underlying_int_t value
) -> bool {
	const auto& enum_data = get_enum_data(ti);
	return value_to_index(enum_data, value).has_value();
}



auto light_enum::registry::enum_index(
	const type_index_t& ti,
	const detail::underlying_int_t value
) -> std::optional<size_t> {
	const auto& enum_data = get_enum_data(ti);
	return value_to_index(enum_data, value);
}



auto light_enum::registry::enum_count(const type_index_t& ti) -> size_t {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.values_span_.size();
}


auto light_enum::registry::enum_values_raw(
	const type_index_t& ti
) -> detail::simple_span<byte_t> {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.rawblob_span_;
}


auto light_enum::registry::enum_values(
	const type_index_t& ti
) -> detail::simple_span<detail::underlying_int_t> {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.values_span_;
}


auto light_enum::registry::enum_names(
	const type_index_t& ti
) -> detail::simple_span<std::string_view> {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.names_span_;
}

auto light_enum::registry::enum_names_lowercase(
	const type_index_t& ti
) -> detail::simple_span<std::string_view> {
	const auto& enum_data = get_enum_data(ti);
	return enum_data.names_lowercase_span_;
}



auto light_enum::registry::enum_name(
	const type_index_t& ti, 
	const detail::underlying_int_t value
) -> std::string_view {
	const auto& enum_data = get_enum_data(ti);
	const auto idx = value_to_index(enum_data, value);
	LIGHT_ENUM_INTERNAL_ASSERT(
		idx.value_or(0) < enum_data.names_span_.size()
	);
	return idx.has_value() ?
		enum_data.names_span_[*idx] :
		std::string_view{};
}


auto light_enum::registry::enum_name_lowercase(
	const type_index_t& ti,
	const detail::underlying_int_t value
) -> std::string_view {
	const auto& enum_data = get_enum_data(ti);
	const auto idx = value_to_index(enum_data, value);
	LIGHT_ENUM_INTERNAL_ASSERT(
		idx.value_or(0) < enum_data.names_span_.size()
	);
	return idx.has_value() ?
		enum_data.names_lowercase_span_[*idx] :
		std::string_view{};
}



auto light_enum::registry::enum_step(
	const type_index_t& ti, 
	const detail::underlying_int_t value,
	const int32_t steps,
	const bool wrap
) -> std::optional<detail::underlying_int_t> {
	const auto& enum_data = get_enum_data(ti);
	const auto idx = value_to_index(enum_data, value);
	if (!idx.has_value()) LIGHT_ENUM_UNLIKELY {
		return std::nullopt;
	}
	const auto& values = enum_data.values_span_;
	if (wrap == false) LIGHT_ENUM_UNLIKELY{
		const auto idx_stepped = internal_cast<int32_t>(*idx) + steps;
		const auto is_out_of_bounds =
			idx_stepped < 0 ||
			idx_stepped >= internal_cast<int32_t>(values.size());
		if (is_out_of_bounds) {
			return std::nullopt;
		}
		LIGHT_ENUM_INTERNAL_ASSERT(internal_cast<size_t>(idx_stepped) < values.size());
		return values[internal_cast<size_t>(idx_stepped)];
	}
	else if (steps >= -1 && steps <= 1) LIGHT_ENUM_LIKELY { // Optimization for single step
		LIGHT_ENUM_INTERNAL_ASSERT(values.size() > 0);
		const auto idx_stepped = internal_cast<int32_t>(*idx) + steps;
		const auto sz = internal_cast<int32_t>(values.size());
		const auto idx_valid =
			idx_stepped < 0 ? internal_cast<size_t>(values.size() - 1) :
			idx_stepped >= sz ? internal_cast<size_t>(0) :
			internal_cast<size_t>(idx_stepped);
		LIGHT_ENUM_INTERNAL_ASSERT(idx_valid < values.size());
		return values[idx_valid];
	}
	else {
		auto signed_mod_f = [](
			const int32_t idx,
			const int32_t n
		) noexcept -> int32_t {
			LIGHT_ENUM_INTERNAL_ASSERT(n > 0);
			const auto mod_f = [](int32_t v, int32_t n) noexcept {
				LIGHT_ENUM_INTERNAL_ASSERT(v >= 0);
				LIGHT_ENUM_INTERNAL_ASSERT(n > 0);
				return v % n;
			};
			const auto idx_in_range = idx >= 0 ?
				mod_f(idx, n) :
				mod_f(n - mod_f(-idx, n), n);
			LIGHT_ENUM_INTERNAL_ASSERT(idx_in_range >= 0);
			LIGHT_ENUM_INTERNAL_ASSERT(idx_in_range < n);
			return idx_in_range;
		};
		const auto idx_stepped = signed_mod_f(
			internal_cast<int32_t>(*idx) + steps,
			internal_cast<int32_t>(values.size())
		);
		LIGHT_ENUM_INTERNAL_ASSERT(idx_stepped >= 0);
		LIGHT_ENUM_INTERNAL_ASSERT(idx_stepped < values.size());
		return values[internal_cast<size_t>(idx_stepped)];
	}
}

































auto light_enum::registry::register_enum(
	const std::string_view enum_name,
	const type_index_t& typeindex_enum,
	const type_index_t& typeindex_underlying,
	const size_t enum_bytesize,
	const bool is_signed,
	const simple_span<detail::byte_t> blob,
	std::vector<detail::underlying_int_t>&& values,
	const simple_span<std::string_view> names
) -> void {
	auto& state = get_state_mut();
	auto& typeindex_to_enumdata = state.typeindex_to_enumdata_;
	if (typeindex_to_enumdata.count(typeindex_enum) > 0) LIGHT_ENUM_UNLIKELY {
#if LIGHT_ENUM_INTERNAL_ABORT_ON_DUPLICATE == 1
		throw std::logic_error{ concat_strings("enum already registered: "sv, enum_name )};
#else
		return;
#endif
	}
	if (values.size() != names.size()) LIGHT_ENUM_UNLIKELY {
		LIGHT_ENUM_INTERNAL_ASSERT(false && "enum error");
		throw std::logic_error{ concat_strings("enum error: "sv, typeindex_enum.name())};
	}
	const auto num_values = values.size();

	// Names
	const auto bigstring_size = std::accumulate(
		names.begin(), 
		names.end(), 
		size_t{ 0 }, 
		[](const size_t sz, const std::string_view& name) { return sz + name.size(); }
	);

	// Create offsets which points into a single string of all enums
	const auto offsets = [&names]() {
		struct offset_t {
			size_t offset{};
			size_t size{};
		};
		auto ret = std::vector<offset_t>{};
		ret.reserve(names.size());
		for (const auto& name : names) {
			const auto offset = ret.empty() ?
				size_t{ 0 } :
				ret.back().offset + ret.back().size;
			ret.push_back({ offset, name.size() });
		}
		LIGHT_ENUM_INTERNAL_ASSERT(ret.size() == names.size());
		return ret;
	}();

	// Make a big string containing all enums names
	auto big_string = [&names, bigstring_size]() {
		auto big_string = std::make_unique<std::string>();
		big_string->reserve(bigstring_size);
		for (const auto& name : names) {
			big_string->append(name);
		}
		return big_string;
	}();
	// Create string_views pointing into the big string
	auto names_as_string_views = [&big_string, &offsets]() {
		auto names_as_string_views = std::make_unique<std::vector<std::string_view>>();
		names_as_string_views->reserve(offsets.size());
		for (const auto& chunk : offsets) {
			const auto sv = std::string_view{
				big_string->data() + chunk.offset,
				chunk.size
			};
			names_as_string_views->emplace_back(sv);
		}
		return names_as_string_views;
	}();
	const auto names_span = detail::simple_span<std::string_view>{
		names_as_string_views->data(),
		num_values
	};
	auto name_to_value = [&names_span, &values, &num_values]() -> stringview_to_value_map_t {
		auto ret = stringview_to_value_map_t{};
		ret.reserve(num_values);
		for (size_t i = 0; i < num_values; ++i) {
			ret.emplace(names_span[i], values[i]);
		}
		return ret;
	}();

	// Make lowercase equivalents
	auto big_string_lowercase = std::make_unique<const std::string>(
		to_lowercase(*big_string)
	);
	auto nameslowercase_as_string_views = [&big_string_lowercase, &offsets]() {
		auto nameslowercase_as_string_views = std::make_unique<std::vector<std::string_view>>();
		nameslowercase_as_string_views->reserve(offsets.size());
		for (const auto& chunk : offsets) {
			const auto sv = std::string_view{
				big_string_lowercase->data() + chunk.offset,
				chunk.size
			};
			nameslowercase_as_string_views->emplace_back(sv);
		}
		return nameslowercase_as_string_views;
	}();
	const auto nameslowercase_span = detail::simple_span<std::string_view>{
		nameslowercase_as_string_views->data(),
		num_values
	};
	auto namelowercase_to_value = [&nameslowercase_span, &values, &num_values]() -> stringview_to_value_map_t {
		auto ret = stringview_to_value_map_t{};
		ret.reserve(num_values);
		for (size_t i = 0; i < num_values; ++i) {
			ret.emplace(nameslowercase_span[i], values[i]);
		}
		return ret;
	}();


	// Raw blob of enum values, using underlying_int_t in order to always have four byte alignment
	const auto blob_num_elements = (blob.size() % sizeof(detail::underlying_int_t) == 0) ?
		(blob.size() / sizeof(detail::underlying_int_t)) :
		(blob.size() / sizeof(detail::underlying_int_t)) + 1;
	LIGHT_ENUM_INTERNAL_ASSERT(
		blob_num_elements * sizeof(detail::underlying_int_t) >= // Have
		blob.size() // Want
	);
	auto rawblob_uptr = std::make_unique<underlying_int_t[]>(blob_num_elements);
	std::memcpy(rawblob_uptr.get(), blob.data(), blob.size());
	const auto rawblob_span = detail::simple_span<detail::byte_t>{
		reinterpret_cast<detail::byte_t*>(rawblob_uptr.get()),
		blob.size()
	};

	// Enum name
	auto enumname_uptr = std::make_unique<std::string>(enum_name);
	const auto enumname_sv = std::string_view{ *enumname_uptr };

	// Is index equal to value (used for faster lookup)
	const auto is_index_equal_to_value = [&values]() noexcept {
		for (size_t i = 0; i < values.size(); ++i) {
			if (values[i] != internal_cast<underlying_int_t>(i)) {
				return is_index_equal_to_value_t::False;
			}
		}
		return is_index_equal_to_value_t::True;
	}();

	// Values
	auto values_uptr = std::make_unique<std::vector<underlying_int_t>>(std::move(values));
	const auto values_span = detail::simple_span<underlying_int_t>{
		values_uptr->data(),
		values_uptr->size()
	};

	// Insert
	typeindex_to_enumdata.emplace(
		typeindex_enum,
		enum_data_t{
			std::move(enumname_uptr),
			enumname_sv,
			typeindex_underlying,
			enum_bytesize,
			is_signed ? is_signed_t::True : is_signed_t::False,
			is_index_equal_to_value,
			std::move(values_uptr),
			values_span,
			std::move(big_string),
			std::move(names_as_string_views),
			names_span,
			std::move(name_to_value),
			std::move(big_string_lowercase),
			std::move(nameslowercase_as_string_views),
			nameslowercase_span,
			std::move(namelowercase_to_value),
			std::move(rawblob_uptr),
			rawblob_span
		}
	);
	state.enumname_to_typeindices_[enumname_sv].emplace_back(typeindex_enum);
}



auto light_enum::registry::is_registered(const type_index_t& ti) -> bool {
	return get_state().typeindex_to_enumdata_.count(ti) > 0;
}



auto light_enum::registry::list_registry() -> std::vector<std::pair<std::string_view, type_index_t>> {
	const auto& typeindex_to_enumdata = get_state().typeindex_to_enumdata_;
	auto entries = std::vector< std::pair<std::string_view, type_index_t> >{};
	entries.reserve(typeindex_to_enumdata.size());
	for (const auto& kvp : typeindex_to_enumdata) {
		entries.emplace_back(kvp.second.enumname_string_view_, kvp.first);
	}
	return entries;
}



auto light_enum::registry::enumname_to_typeindices(
	const std::string_view& name
) -> const std::vector<type_index_t>& {
	const auto& enumname_to_typeindices = get_state().enumname_to_typeindices_;
	const auto it = enumname_to_typeindices.find(name);
	if (it != enumname_to_typeindices.end()) LIGHT_ENUM_LIKELY {
		return it->second;
	}
	// In order to return a reference
	const static auto s_empty = enumname_to_typeindices_map_t::mapped_type{};
	return s_empty;
}



auto light_enum::registry::typeindex_to_enumname(
	const type_index_t& ti
) -> std::optional<std::string_view> {
	const auto& typeindex_to_enumdata = get_state().typeindex_to_enumdata_;
	const auto it = typeindex_to_enumdata.find(ti);
	return it != typeindex_to_enumdata.end() ?
		std::optional<std::string_view>{it->second.enumname_string_view_} :
		std::nullopt;

}











#if LIGHT_ENUM_INTERNAL_DEBUG_MODE == 1
auto light_enum::detail::verify_registered_enum(
	const std::string_view enumname,
	const simple_span<std::string_view> magic_names,
	const simple_span<std::string_view> registered_names,
	const simple_span<byte_t> magic_values,
	const simple_span<byte_t> registered_values,
	const size_t bytes_per_enum
) -> void {

	auto to_string_f = [](auto&& v) -> std::string {
		using value_type = std::decay_t<decltype(v)>;
		if constexpr (std::is_same_v<std::string_view, value_type>) {
			return std::string{ v };
		}
		else if constexpr (std::is_same_v<std::string, value_type>) {
			return std::string{ v };
		}
		else if constexpr (std::is_same_v<std::byte, value_type>) {
			return std::to_string(static_cast<uint8_t>(v));
		}
		else {
			return std::to_string(v);
		}
	};

	auto verify_equal_f = [enumname, &to_string_f](auto&& a, auto&& b) -> void {
		if (a == b) LIGHT_ENUM_LIKELY{
			return;
		}
		const auto msg = concat_strings(
			"light_enum register error ["sv, enumname, "] "sv,
			to_string_f(a), " != "sv, to_string_f(b)
		);
		throw std::logic_error{ msg };
	};

	verify_equal_f(registered_values.size() % bytes_per_enum, 0);
	verify_equal_f(magic_values.size() % bytes_per_enum, 0);
	verify_equal_f(
		registered_values.size() / bytes_per_enum, 
		registered_names.size()
	);
	{
		verify_equal_f(registered_names.size(), magic_names.size());
		const auto sz = registered_names.size();
		for (size_t i = 0; i < sz; ++i) {
			verify_equal_f(registered_names.at(i), magic_names.at(i));
		}
	}
	{
		verify_equal_f(registered_values.size(), magic_values.size());
		const auto sz = registered_values.size();
		for (size_t i = 0; i < sz; ++i) {
			verify_equal_f(registered_values.at(i), magic_values.at(i));
		}
	}


}
#endif