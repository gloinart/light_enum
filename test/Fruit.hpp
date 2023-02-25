enum class Fruit {
  Apple = 1,
  Orange = 5,
  Banana = 9,
  Grape = 13
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