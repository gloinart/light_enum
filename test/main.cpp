


#include <memory>
#include <vector>
#include <typeindex>
#include "TestConfig.hpp"
#include "tsl/hopscotch_map.h"
#include "light_enum/light_enum_types.hpp"
namespace light_enum::detail {
	/*
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
*/
struct test_t {

	tsl::hopscotch_map<int, int> foo_{};

	// Enum type info
	std::unique_ptr<std::string> enumname_{}; // unique_ptr in order to ensure fixed in memory
	std::string_view enumname_string_view_{};
	type_index_t underlying_typeindex_;
	size_t enum_bytesize_{};
	//is_signed_t is_signed_{};
	//is_index_equal_to_value_t is_index_equals_to_value_{};

	// Values
	std::unique_ptr<std::vector<underlying_int_t>> values_{};// unique_ptr in order to ensure fixed in memory
	simple_span<underlying_int_t> values_span_{}; // points to values_, only here as a micro-optimization

	// Names
	std::unique_ptr<std::string> bigstring_names_{}; // a single string is used instead of a vector of std::string for optimization purposes, unique_ptr in order to ensure fixed in memory
	std::unique_ptr<std::vector<std::string_view>> names_stringviews_{}; // unique_ptr in order to ensure fixed in memory, points to names_as_strings_
	simple_span<std::string_view> names_span_{}; // Points to names_stringviews_, only here as a micro-optimization
#if 0
	stringview_to_value_map_t name_to_value_{}; // For fast lookup of large enums

	// Names lowercase
	std::unique_ptr<const std::string> bigstring_names_lowercase_{}; // a single string is used instead of a vector of std::string for optimization purposes, unique_ptr in order to ensure fixed in memory
	std::unique_ptr<std::vector<std::string_view>> names_lowercase_stringviews_{}; // unique_ptr in order to ensure fixed in memory, points to names_as_strings_
	simple_span<std::string_view> names_lowercase_span_{}; // Points to names_lowercase_stringviews_, only here as a micro-optimization
	stringview_to_value_map_t name_lowercase_to_value_{}; // For enum_cast_ci
#endif

	// Raw blob (used to reinterpret_cast to enum E)
	std::unique_ptr<underlying_int_t[]> rawblob_{}; // unique_ptr in order to ensure fixed in memory, underlying_int for 4 byte alignment
	simple_span<byte_t> rawblob_span_{}; // Points to rawblob_
};


}

auto func() {
	using namespace light_enum::detail;
	auto a = test_t{};
	auto b = std::move(a);


	//auto m = tsl::hopscotch_map<int, test_t>{};
}










#if 0
#include "config.hpp"
#include <light_enum/light_enum.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <test/Fruit.hpp>

#define LOG(x) std::cout << x << std::endl

template <typename T, const T value>
constexpr auto name() -> std::string_view {
	return __FUNCSIG__;
}

template <typename T, int32_t Start, int32_t Stop, int32_t V>
constexpr auto names() {
	if constexpr (V >= Stop) {
		return;
	}
	else {
		constexpr auto n = name<T, static_cast<T>(V)>();
		std::cout << n << std::endl;
		names<T, Start, Stop, V + 1>();
	}
}



template<typename E, int32_t Offset, typename F, int32_t... I>
auto namesss(F f, std::integer_sequence<int32_t, I...>)
{
	((f(I+Offset, name<E, static_cast<E>(I+Offset) >())), ...);
}

enum E {
	aa = -2,bb=3,cc,dd,ee
};

enum class EE {
	aa, bb, cc = 7, dd, ee
};

auto main() -> int {
	//namess<EE>(std::make_integer_sequence<int32_t, 10>{});
	
	auto strs = std::vector<std::string>{};
	namesss<EE, -10>(
		[&](int32_t v, const std::string_view sv) { LOG("["<<v << "] " << sv); },
		std::make_integer_sequence<int32_t, 20>{}
	);

	//names<E, 0, 10, 0>();
	//names<EE, 0, 10, 0>();

	//light_enum::enum_name(E::aa);


	for (auto&& v : light_enum::registry::list_registry()) {
		LOG(v.first);
	}

}

#endif