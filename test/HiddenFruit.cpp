#include "TestConfig.hpp"
#include <cstdint>
#include "light_enum/light_enum_register.hpp"

namespace ns_hidden {
	enum class HiddenFruit : uint32_t {
		HiddenApple,
		HiddenBanana
	};
}
LIGHT_ENUM_REGISTER_RANGE(ns_hidden::HiddenFruit, 0, 3);



