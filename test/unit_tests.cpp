
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Fruit.hpp"
#include "light_enum/light_enum.hpp"
#include <vector>



TEST_CASE("enum_name") {
	const auto dummy_fruit = static_cast<Fruit>(404);
	REQUIRE(light_enum::enum_name(Fruit::Apple) == "Apple");
	REQUIRE(light_enum::enum_name(Fruit::Orange) == "Orange");
	REQUIRE(light_enum::enum_name(Fruit::Banana) == "Banana");
	REQUIRE(light_enum::enum_name(Fruit::Grape) == "Grape");
	REQUIRE(light_enum::enum_name(dummy_fruit) == "");
}

TEST_CASE("enum_cast") {
	REQUIRE(light_enum::enum_cast<Fruit>("Apple").value() == Fruit::Apple);
	REQUIRE(light_enum::enum_cast<Fruit>("Orange").value() == Fruit::Orange);
	REQUIRE(light_enum::enum_cast<Fruit>("Banana").value() == Fruit::Banana);
	REQUIRE(light_enum::enum_cast<Fruit>("Grape").value() == Fruit::Grape);
	REQUIRE(light_enum::enum_cast<Fruit>("Dummy").has_value() == false);
}

TEST_CASE("enum_count") {
	REQUIRE(light_enum::enum_count<Fruit>() == 4);
}

TEST_CASE("enum_values") {
	const auto& values = light_enum::enum_values<Fruit>();
	const auto facit = std::vector<Fruit>{ Fruit::Apple, Fruit::Orange, Fruit::Banana, Fruit::Grape };
	REQUIRE(values.size() == facit.size());
	REQUIRE(std::equal(values.begin(), values.end(), facit.begin()));
}

TEST_CASE("enum_names") {
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
	const auto dummy_fruit = static_cast<Fruit>(404);
	REQUIRE(light_enum::enum_contains(Fruit::Apple) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Orange) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Banana) == true);
	REQUIRE(light_enum::enum_contains(Fruit::Grape) == true);
	REQUIRE(light_enum::enum_contains(dummy_fruit) == false);
}



