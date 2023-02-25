#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "Fruit.hpp"
#include "light_enum/light_enum.hpp"
#include <vector>



TEST_CASE("enum_name") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto dummy_fruit = static_cast<Fruit>(404);
	REQUIRE(light_enum::enum_name(Fruit::Apple) == "Apple");
	REQUIRE(light_enum::enum_name(Fruit::Orange) == "Orange");
	REQUIRE(light_enum::enum_name(Fruit::Banana) == "Banana");
	REQUIRE(light_enum::enum_name(Fruit::Grape) == "Grape");
	REQUIRE(light_enum::enum_name(dummy_fruit) == "");
}

TEST_CASE("enum_cast") {
	REQUIRE(light_enum::is_registred<Fruit>());
	REQUIRE(light_enum::enum_cast<Fruit>("Apple").value() == Fruit::Apple);
	REQUIRE(light_enum::enum_cast<Fruit>("Orange").value() == Fruit::Orange);
	REQUIRE(light_enum::enum_cast<Fruit>("Banana").value() == Fruit::Banana);
	REQUIRE(light_enum::enum_cast<Fruit>("Grape").value() == Fruit::Grape);
	REQUIRE(light_enum::enum_cast<Fruit>("Dummy").has_value() == false);
}

TEST_CASE("enum_count") {
	REQUIRE(light_enum::is_registred<Fruit>());
	REQUIRE(light_enum::enum_count<Fruit>() == 4);
}

TEST_CASE("enum_values") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto& values = light_enum::enum_values<Fruit>();
	const auto facit = std::vector<Fruit>{ Fruit::Apple, Fruit::Orange, Fruit::Banana, Fruit::Grape };
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
	REQUIRE(names == facit);
}

TEST_CASE("enum_contains") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto dummy_fruit = static_cast<Fruit>(404);
	REQUIRE(light_enum::enum_contains(Fruit::Apple) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Orange) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Banana) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Grape) == true);
	REQUIRE(light_enum::enum_contains(dummy_fruit) == false);
}

TEST_CASE("enum_index") {
	REQUIRE(light_enum::is_registred<Fruit>());
	const auto dummy_fruit = static_cast<Fruit>(404);
	REQUIRE(light_enum::enum_index(Fruit::Apple) == 0);
	REQUIRE(light_enum::enum_index(Fruit::Orange) == 1);
	REQUIRE(light_enum::enum_index(Fruit::Banana) == 2);
	REQUIRE(light_enum::enum_index(Fruit::Grape) == 3);
	REQUIRE(light_enum::enum_index(dummy_fruit) == std::nullopt);
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
		A,B
	};
	REQUIRE(!light_enum::is_registred<UnregistredEnum>());
	REQUIRE_THROWS(light_enum::enum_count<UnregistredEnum>());
}

TEST_CASE("enum class:bool") {
	REQUIRE(light_enum::is_registred<Eatable>());
	REQUIRE(light_enum::enum_count<Eatable>() == 2);
	REQUIRE(light_enum::enum_name(Eatable::Yes) == "Yes");
	REQUIRE(light_enum::enum_name(Eatable::No) == "No");
}