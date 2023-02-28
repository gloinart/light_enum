#include <array>

enum class Fruit {
  Apple = 1,
  Orange = 5,
  Banana = 9,
  Grape = 13
};
constexpr auto kundefined_fruits = std::array{
  static_cast<Fruit>(0),
  static_cast<Fruit>(3),
  static_cast<Fruit>(7),
  static_cast<Fruit>(11),
  static_cast<Fruit>(15),
};

enum class UnusedFruit {
  Eggplant
};

enum class Eatable :bool {
  No,
  Yes
};

namespace ns_other_fruit {
  enum class Fruit {
    Pear,
    Citrus
  };
}

namespace ns_nestled0::ns_nestled1::ns_nestled2 {
  enum class Fruit {
    Pear,
    Citrus,
    Mango
  };
}