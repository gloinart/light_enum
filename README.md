# Light-Enum
Light-Enum is an "add-on" for Magic-Enum aiming to reduce compile times.
The idea is to introspect enums (using Magic Enum) at application start and store them all in a hash map, introspection of enums is then performed by looking up enums in the hash map.
This means that the heavey Magic Enum introspection only needs to be compiled in a single translation unit/cpp file rather than every time an enum is introspected.
<em>[Magic Enum](https://github.com/Neargye/magic_enum)</em>

### Summary
<em>light_enum.hpp</em> - Light header file used when accessing enums. I'd suggest to put it in precompiled headers.
<em>light_enum_register.hpp</em> - Heavy header file only to be used in .cpp files where enums are registered.
 
### Interface
The interface for light_enum is similar to magic_enum. The difference is that introspection is not performed in compile time.

### Run-time performance
Every introspection constructs a std::type_index and performs a std::unordered_map lookup.



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
Light-Enum consists of a single .cpp file which needs to be added to your project.

## Future improvements
These improvements are planned for the future
* Ability to change std::type_index to boost::typeindex in order to not require RTTI
* Ability to change the underlying std::unordered_map to a faster hash-map (tsl::hopscotch_map for example)



## Contact
I'm available at viktor.sehr(at)gmail.com
