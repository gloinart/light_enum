Note, consider this beta right now. Bugfixes and ideas for improvement are more than welcome. 

# Light-Enum
Light-Enum is an "add-on" for Magic-Enum aiming to provide the same (almost) functionality with low compile time.
The idea is to introspect enums (using Magic Enum) at application start and store them all in a global hash map.
This means that the heavey Magic Enum introspection only needs to be compiled in a single translation unit/cpp file rather than every time an enum is introspected.
See example below.

### Magic-Enum
As mentioned, this just a simple add-on for Magic Enum. All the hard work for introspecting enums shall be credited to the creator of Magic Enum.
<em>[Magic Enum](https://github.com/Neargye/magic_enum)</em>

### Summary
<em>light_enum.hpp</em> - Light header file used when accessing enums. Suitable for adding to precompiled headers.
<em>light_enum_register.hpp</em> - Heavy header file only to be used in .cpp files where enums are registered.
 
### Interface

#### Introspecting enums
All these are similar to the corresponding function in magic_enum, however a span or a std::vector& is returned instead of an array.
Trying to access an enum not registred throws an out_of_bounds exception.
```cpp
template <typename E> auto enum_cast(const std::string_view& name) -> std::optional<E>;
template <typename E> auto enum_values() -> detail::span<E>;
template <typename E> auto enum_names() -> const std::vector<std::string>&;
template <typename E> auto enum_name(const E& e) -> std::string_view; // Returns an empty string_view if invalid enum
template <typename E> auto enum_count() -> size_t;
template <typename E> auto enum_index(const E& e) -> std::optional<size_t>;
template <typename E> auto enum_contains(const E& e) -> bool;
```

#### Adding enums to registry
```cpp
template <typename E> [[nodiscard]] auto is_registred() -> bool;
template <typename E> auto register_enum() -> void;
LIGHT_ENUM_REGISTER(E) // Convenience macro for putting in global namespace 
```

### Run-time performance
Every introspection 
	* Constructs a std::type_index 
	* Reads a static variable
	* Performs a std::unordered_map lookup
RTTI is required to be enabled in order to use std::type_index. Future updates will be able to use boost::type_index which does not depend on RTTI.


## Usage example

Fruit.h
```cpp
// Define enum as ususal
enum class Fruit{Apple, Orange, Banana};
```

Fruit.cpp
```cpp
// Only include light_enum_register.hpp where you register enums
#include "Fruit.h"
#include "light_enum/light_enum_register.hpp"
LIGHT_ENUM_REGISTER(Fruit);
```

Application.cpp
```cpp
#include "Fruit.h"
// Only light_enum.hpp is required for accessing enums
#include "light_enum/light_enum.hpp"
auto function() {
	// Introspect enums just as Magic Enum
	const auto apple = Fruit::Apple;
	assert(light_enum::enum_count<Fruit> == 3);
	assert(light_enum::enum_name(apple) == "Apple");
	const auto values = light_enum::enum_values<Fruit>();
	assert(values[0] == Fruit::Apple);
	assert(values[1] == Fruit::Orange);
	assert(values[2] == Fruit::Banana);
}
```







## Installation
Light-Enum consists of a single .cpp file which needs to be added to the project.
As mentioned earlier, only include "light_enum_register.hpp" in the .cpp files where you register enums.

## Future improvements
These improvements are planned for the future
* Ability to change std::type_index to boost::typeindex in order to not require RTTI
* Ability to change the underlying std::unordered_map to a faster hash-map (tsl::hopscotch_map for example)

## License
MIT License.
Use however you like, if you're using it in something released I would be happy for a mail telling me about it :)

## Contact
I'm available at viktor.sehr(at)gmail.com
