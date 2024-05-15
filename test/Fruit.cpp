#include "TestConfig.hpp"
#include "Fruit.hpp"
#include "light_enum/light_enum_register.hpp"

LIGHT_ENUM_REGISTER(Fruit);
LIGHT_ENUM_REGISTER(Eatable);
LIGHT_ENUM_REGISTER(UnusedFruit);
LIGHT_ENUM_REGISTER(ns_other_fruit::Fruit);
LIGHT_ENUM_REGISTER(ns_nestled0::ns_nestled1::ns_nestled2::Fruit);


