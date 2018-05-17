# struc 

struc is a C++11 implementation of python's struct module. It depends only of the Boost.Endian header only library and the math library.

## Usage

struc is header only. You can just drop the [struc.hpp](include/struc.hpp) wherever you like. Or you can add this repository as a git submodule and include it via CMake's add_subdirectory command.
struc needs the math library so if you do not use struc with CMake's add_subdirectory, you need to link the math library to the final build artifact.

## Example

```cpp
#include <iostream>
#include "struc.hpp"

int main(int argc, char** argv)
{
   auto data = struc::pack(std::string("hhl"), 1, 2, 3);
   short h1, h2;
   long l1;
   struc::unpack("hhl", &data[0], h1, h2, l1);
   std::cout << h1 << std::endl;
   std::cout << h2 << std::endl;
   std::cout << l1 << std::endl;
   return 0;
}
```
output:
```
1
2
3
```

```cpp
#include <iostream>
#include "struc.hpp"

int main(int argc, char** argv)
{
   std::string record("raymond   \x32\x12\x08\x01\x08");
   std::string name;
   unsigned short serialnum, school;
   char gradelevel;
   struc::unpack(std::string("<10sHHb"), record.c_str(), name, serialnum, school, gradelevel);
   std::cout << name << std::endl;
   std::cout << serialnum << std::endl;
   std::cout << school << std::endl;
   std::cout << (int)gradelevel << std::endl;
   return 0;
}
```
output:
```
raymond
4658
264
8
```

```cpp
#include <iostream>
#include <iomanip>
#include "struc.hpp"

int main(int argc, char** argv)
{
   std::cout << struc::calcsize("ci") << std::endl;
   auto data = struc::pack(std::string("ci"), '*', 0x12131415);
   std::cout << "0x" << std::hex;
   for (const auto& c : data)
   {
      std::cout << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
   }
   std::cout << std::endl;
   std::cout << struc::calcsize("ic") << std::endl;
   data = struc::pack(std::string("ic"), 0x12131415, '*');
   std::cout << "0x" << std::hex;
   for (const auto& c : data)
   {
      std::cout << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
   }
   std::cout << std::endl;
   return 0;
}
```
output:
```
8
0x2a00000015141312
5
0x151413122a
```

```cpp
#include <iostream>
#include <iomanip>
#include "struc.hpp"

int main(int argc, char** argv)
{
   std::cout << struc::calcsize("iih0i") << std::endl;
   auto data = struc::pack(std::string("iih0i"), 1, 2, 3);
   std::cout << "0x" << std::hex;
   for (const auto& c : data)
   {
      std::cout << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
   }
   std::cout << std::endl;
   return 0;
}
```
output:
```
12
0x010000000200000003000000
```

