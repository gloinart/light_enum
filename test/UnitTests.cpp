#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "TestConfig.hpp"
#include "Fruit.hpp"
#include "BigEnum.hpp"
#include "light_enum/light_enum.hpp"
#include "light_enum/light_enum_register.hpp"
#include <vector>



TEST_CASE("enum_name") {
	REQUIRE(light_enum::is_registred<Fruit>());
	REQUIRE(light_enum::enum_name(Fruit::Apple) == "Apple");
	REQUIRE(light_enum::enum_name(Fruit::Orange) == "Orange");
	REQUIRE(light_enum::enum_name(Fruit::Banana) == "Banana");
	REQUIRE(light_enum::enum_name(Fruit::Grape) == "Grape");
	for (const auto& undefined_fruit : kundefined_fruits) {
		REQUIRE(light_enum::enum_name(undefined_fruit) == "");
	}
}

TEST_CASE("enum_cast from string_view") {
	REQUIRE(light_enum::is_registred<Fruit>());
	REQUIRE(light_enum::enum_cast<Fruit>("Apple").value() == Fruit::Apple);
	REQUIRE(light_enum::enum_cast<Fruit>("Orange").value() == Fruit::Orange);
	REQUIRE(light_enum::enum_cast<Fruit>("Banana").value() == Fruit::Banana);
	REQUIRE(light_enum::enum_cast<Fruit>("Grape").value() == Fruit::Grape);
	REQUIRE(light_enum::enum_cast<Fruit>("Dummy").has_value() == false);
}

TEST_CASE("enum_cast from integer") {
	REQUIRE(light_enum::is_registred<Fruit>());
	{
		using value_type = char;
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(1)).value() == Fruit::Apple);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(5)).value() == Fruit::Orange);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(9)).value() == Fruit::Banana);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(13)).value() == Fruit::Grape);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-100)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-1)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(2)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(6)).has_value() == false);
	}
	{
		using value_type = int64_t;
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(1)).value() == Fruit::Apple);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(5)).value() == Fruit::Orange);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(9)).value() == Fruit::Banana);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(13)).value() == Fruit::Grape);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-100)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-1)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(2)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(6)).has_value() == false);
	}
	{
		using value_type = int32_t;
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(1)).value() == Fruit::Apple);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(5)).value() == Fruit::Orange);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(9)).value() == Fruit::Banana);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(13)).value() == Fruit::Grape);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-100)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-1)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(2)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(6)).has_value() == false);
	}
	{
		using value_type = wchar_t;
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(1)).value() == Fruit::Apple);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(5)).value() == Fruit::Orange);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(9)).value() == Fruit::Banana);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(13)).value() == Fruit::Grape);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-100)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(-1)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(2)).has_value() == false);
		REQUIRE(light_enum::enum_cast<Fruit>(value_type(6)).has_value() == false);
	}
}


TEST_CASE("enum_count") {
	REQUIRE(light_enum::is_registred<Fruit>());
	REQUIRE(light_enum::enum_count<Fruit>() == 4);
}

TEST_CASE("enum_values") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto& values = light_enum::enum_values<Fruit>();
	const auto facit = std::vector<Fruit>{ 
		Fruit::Apple, 
		Fruit::Orange, 
		Fruit::Banana, 
		Fruit::Grape 
	};
	REQUIRE(values.size() == facit.size());
	REQUIRE(std::equal(values.begin(), values.end(), facit.begin()));
}

TEST_CASE("enum_names") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto facit = std::vector<std::string>{
		"Apple",
		"Orange",
		"Banana",
		"Grape"
	};
	const auto& names = light_enum::enum_names<Fruit>();
	REQUIRE(names.size() == facit.size());
	REQUIRE(std::equal(names.begin(), names.end(), facit.begin()));
}

TEST_CASE("enum_contains") {
	REQUIRE(light_enum::is_registred<Fruit>());
	REQUIRE(light_enum::enum_contains(Fruit::Apple) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Orange) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Banana) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Grape) == true);
	for (const auto& undefined_fruit : kundefined_fruits) {
		REQUIRE(light_enum::enum_contains(undefined_fruit) == false);
	}
}

TEST_CASE("enum_index") {
	REQUIRE(light_enum::is_registred<Fruit>());
	REQUIRE(light_enum::enum_index(Fruit::Apple) == 0);
	REQUIRE(light_enum::enum_index(Fruit::Orange) == 1);
	REQUIRE(light_enum::enum_index(Fruit::Banana) == 2);
	REQUIRE(light_enum::enum_index(Fruit::Grape) == 3);
	for (const auto& undefined_fruit : kundefined_fruits) {
		REQUIRE(light_enum::enum_index(undefined_fruit) == std::nullopt);
	}
}

TEST_CASE("enum_next") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto values = light_enum::enum_values<Fruit>();
	auto e = values.front();
	for (size_t i = 0; i < 200; ++i) {
		REQUIRE(
			e ==
			values[i % values.size()]
		);
		e = light_enum::enum_next(e).value();
	}
	for (const auto& undefined_fruit : kundefined_fruits) {
		REQUIRE(light_enum::enum_next(undefined_fruit) == std::nullopt);
	}
}

TEST_CASE("enum_prev") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto values = light_enum::enum_values<Fruit>();
	int idx = 0;
	auto e = values[idx];
	for (size_t i = 0; i < 200; ++i) {
		REQUIRE(
			e ==
			values[idx]
		);
		e = light_enum::enum_prev(e).value();
		idx -= 1;
		idx += static_cast<int>(values.size());
		idx %= static_cast<int>(values.size());
	}
	for (const auto& undefined_fruit : kundefined_fruits) {
		REQUIRE(light_enum::enum_prev(undefined_fruit) == std::nullopt);
	}
}


TEST_CASE("type abbreviations") {
	enum class Dummy{A,B};
	REQUIRE(!light_enum::is_registred<Dummy>());
	REQUIRE(light_enum::is_registred<Fruit>());
	//REQUIRE(light_enum::is_registred<Fruit&>());
	//REQUIRE(light_enum::is_registred<Fruit&&>());
	REQUIRE(light_enum::is_registred<const Fruit>());
	//REQUIRE(light_enum::is_registred<const Fruit&>());
	//REQUIRE(light_enum::is_registred<const Fruit&&>());
	//REQUIRE(!light_enum::is_registred<Fruit*>());
	//REQUIRE(!light_enum::is_registred<const Fruit*>());
}


TEST_CASE("enum in other namespaces namespace") {
	// A
	REQUIRE(light_enum::is_registred<ns_other_fruit::Fruit>());
	REQUIRE(light_enum::enum_count<ns_other_fruit::Fruit>() == 2);
	// B
	REQUIRE(light_enum::is_registred<ns_nestled0::ns_nestled1::ns_nestled2::Fruit>());
	REQUIRE(light_enum::enum_count<ns_nestled0::ns_nestled1::ns_nestled2::Fruit>() == 3);
}


TEST_CASE("second enum") {
	REQUIRE(light_enum::is_registred<UnusedFruit>());
	REQUIRE(light_enum::enum_count<UnusedFruit>() == 1);
	REQUIRE(light_enum::enum_name(UnusedFruit::Eggplant) == "Eggplant");
}

TEST_CASE("unregistered enum") {
	enum class UnregistredEnum {
		A,
		B
	};
	REQUIRE(!light_enum::is_registred<UnregistredEnum>());
	REQUIRE_THROWS(light_enum::enum_count<UnregistredEnum>());
	light_enum::register_enum<UnregistredEnum>();
	REQUIRE(light_enum::is_registred<UnregistredEnum>());
}

TEST_CASE("enum class:bool") {
	REQUIRE(light_enum::is_registred<Eatable>());
	REQUIRE(light_enum::enum_count<Eatable>() == 2);
	REQUIRE(light_enum::enum_name(Eatable::No) == "No");
	REQUIRE(light_enum::enum_name(Eatable::Yes) == "Yes");
}


namespace ns_hidden {
	enum class HiddenFruit : uint32_t;
}
TEST_CASE("undefined enum in context") {
	using namespace ns_hidden;
	REQUIRE(light_enum::is_registred<HiddenFruit>());
	REQUIRE(light_enum::enum_count<HiddenFruit>() == 2);
	const auto names = light_enum::enum_names<HiddenFruit>();
	REQUIRE(
		names[0] == "HiddenApple"
	);
	REQUIRE(
		names[1] == "HiddenBanana"
	);
}




TEST_CASE("registry::typeindex_to_enumname") {
	REQUIRE(
		light_enum::registry::typeindex_to_enumname(light_enum::detail::make_type_index<Fruit>()) ==
		"Fruit"
	);
	REQUIRE(
		light_enum::registry::typeindex_to_enumname(light_enum::detail::make_type_index<ns_hidden::HiddenFruit>()) ==
		"HiddenFruit"
	);	
	REQUIRE(
		light_enum::registry::typeindex_to_enumname(light_enum::detail::make_type_index<Eatable>()) ==
		"Eatable"
	);
	REQUIRE(
		light_enum::registry::typeindex_to_enumname(light_enum::detail::make_type_index<ns_other_fruit::Fruit>()) ==
		"Fruit"
	);
}







TEST_CASE("big_enum") {
	REQUIRE(light_enum::enum_count<BigEnum>() == 10 * 5);
	REQUIRE(light_enum::enum_cast<BigEnum>("e16") == BigEnum::e16);
	REQUIRE(light_enum::enum_name<BigEnum>(BigEnum::e16) == "e16");

	auto values = light_enum::enum_values<BigEnum>();
	for (size_t i = 0; i < values.size(); ++i) {
		REQUIRE(i == static_cast<size_t>(values[i]));
	}
	{
		auto x = static_cast<BigEnum>(light_enum::enum_count<BigEnum>());
		REQUIRE(light_enum::enum_contains(x) == false);
		REQUIRE(light_enum::enum_name(x) == "");

	}

}


TEST_CASE("lowercase") {
	const auto a = light_enum::enum_names<Fruit>();
	const auto b = light_enum::enum_names_lowercase<Fruit>();
	REQUIRE(a.size() == b.size());
	for (size_t i = 0; i < a.size(); ++i) {
		const auto aa = a.at(i);
		auto aa_lowercase = std::string{ aa };
		for (auto& c : aa_lowercase) {
			c = static_cast<char>(std::tolower(c));
		}
		auto bb = b.at(i);
		REQUIRE(aa_lowercase == bb);

		REQUIRE(
			light_enum::enum_cast<Fruit>(aa) ==
			light_enum::enum_cast_ci<Fruit>(bb)
		);
	}
}


TEST_CASE("enum_cast_ci") {

	for (const auto& name : light_enum::enum_names<Fruit>()) {
		const auto v0 = light_enum::enum_cast<Fruit>(name);
		// Upper
		auto str = std::string{ name };
		for (auto& c : str) {
			c = static_cast<char>(std::toupper(c));
		}
		const auto v1 = light_enum::enum_cast_ci<Fruit>(str);
		REQUIRE(v0 == v1);
		// Lower
		for (auto& c : str) {
			c = static_cast<char>(std::tolower(c));
		}
		const auto v2 = light_enum::enum_cast_ci<Fruit>(str);
		REQUIRE(v0 == v2);
	}



};



TEST_CASE("test allocation 8 byte") {
	using base_t = std::uint8_t;
	enum class E1 :base_t { _0};
	enum class E2 :base_t { _0,_1 };
	enum class E3 :base_t { _0, _1, _2 };
	enum class E4 :base_t { _0, _1, _2, _3 };
	enum class E5 :base_t { _0, _1, _2, _3, _4 };
	enum class E6 :base_t { _0, _1, _2, _3, _4, _5 };
	enum class E7 :base_t { _0, _1, _2, _3, _4, _5, _6 };
	enum class E8 :base_t { _0, _1, _2, _3, _4, _5, _6, _7 };
	enum class E9 :base_t { _0, _1, _2, _3, _4, _5, _6, _7, _8 };
	light_enum::register_enum<E1>();
	light_enum::register_enum<E2>();
	light_enum::register_enum<E3>();
	light_enum::register_enum<E4>();
	light_enum::register_enum<E5>();
	light_enum::register_enum<E6>();
	light_enum::register_enum<E7>();
	light_enum::register_enum<E8>();
	light_enum::register_enum<E9>();



};

TEST_CASE("test allocation 16 byte") {
	using base_t = std::uint16_t;

	enum class E1 :base_t { _0 };
	enum class E2 :base_t { _0, _1 };
	enum class E3 :base_t { _0, _1, _2 };
	enum class E4 :base_t { _0, _1, _2, _3 };
	enum class E5 :base_t { _0, _1, _2, _3, _4 };
	enum class E6 :base_t { _0, _1, _2, _3, _4, _5 };
	enum class E7 :base_t { _0, _1, _2, _3, _4, _5, _6 };
	enum class E8 :base_t { _0, _1, _2, _3, _4, _5, _6, _7 };
	enum class E9 :base_t { _0, _1, _2, _3, _4, _5, _6, _7, _8 };
	light_enum::register_enum<E1>();
	light_enum::register_enum<E2>();
	light_enum::register_enum<E3>();
	light_enum::register_enum<E4>();
	light_enum::register_enum<E5>();
	light_enum::register_enum<E6>();
	light_enum::register_enum<E7>();
	light_enum::register_enum<E8>();
	light_enum::register_enum<E9>();
};


TEST_CASE("simple_span") {


	auto vec = std::vector<int>{};
	REQUIRE_THROWS(light_enum::detail::simple_span(vec.data(), vec.size()).at(0));
	vec.resize(10, 32);
	auto span = light_enum::detail::simple_span(vec.data(), vec.size());
	for (auto&& v : span) {
		REQUIRE(v == 32);
	}
	REQUIRE_THROWS(span.at(10));


};
