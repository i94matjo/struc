/*
    struc, A C++11 implementation of python's struct module.

    struc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (c) 2018, emJay Software Consulting AB, See AUTHORS for details.
*/

#include <cstdio>
#include "struc.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

std::string to_hex(const std::vector<char>& data)
{
   std::ostringstream ss;
   ss << std::hex;
   for (const auto& c : data)
   {
      unsigned char uc = static_cast<unsigned char>(c);
      ss << std::setw(2) << std::setfill('0') << static_cast<int>(uc);
   }
   return ss.str();
}

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
   py_arg(const T& t)
{
   return std::string("'" + t + "'");
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type py_arg(
   const T& t)
{
   return std::to_string(t);
}

template <>
std::string py_arg<bool>(const bool& t)
{
   return t ? "True" : "False";
}

template <>
std::string py_arg<char>(const char& t)
{
   return std::string("'") + t + "'";
}

template <typename T>
typename std::enable_if<std::is_pointer<T>::value, std::string>::type py_arg(
   const T& t)
{
   const void* address = static_cast<const void*>(t);
   std::ostringstream ss;
   ss << "0x" << address;
   return ss.str();
}

template <size_t I = 0, typename... T>
typename std::enable_if<I == sizeof...(T), std::string>::type py_arg_t(
   const std::tuple<T...>&)
{
   return "";
}

template <size_t I = 0, typename... T>
   typename std::enable_if
   < I<sizeof...(T), std::string>::type py_arg_t(const std::tuple<T...>& t)
{
   auto s = py_arg(std::get<I>(t));
   auto n = py_arg_t<I + 1, T...>(t);
   if (!n.empty())
   {
      s += ", " + n;
   }
   return s;
}

template <typename... T>
std::string python_struct_pack(const std::string& pattern,
                               const std::tuple<T...>& t)
{
   std::ostringstream ss;
   ss << "import struct; print(struct.pack('" << pattern << "', " << py_arg_t(t)
      << ").encode('hex'))";
   return ss.str();
}

template <typename... T>
std::string python_struct_calcsize(const std::string& pattern,
                                   const std::tuple<T...>& t)
{
   std::ostringstream ss;
   ss << "import struct; print(str(struct.calcsize('" << pattern << "')))";
   return ss.str();
}

std::string exec(const std::string& cmd)
{
   std::array<char, 128> buffer;
   std::string result;
   std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
   if (!pipe)
   {
      return "";
   }
   while (!feof(pipe.get()))
   {
      if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
      {
         result += buffer.data();
      }
   }
   result.erase(std::find_if(result.rbegin(),
                             result.rend(),
                             [](int ch) { return !std::isspace(ch); })
                   .base(),
                result.end());
   return result;
}

std::string python(const std::string& python_cmd)
{
   char* _python_interp = std::getenv("STRUC_PYTHON_EXECUTABLE");
   std::string python_interp;
   if (_python_interp)
   {
      if (access(_python_interp, X_OK) == 0)
      {
         python_interp = _python_interp;
      }
   }
#ifdef STRUC_PYTHON_EXECUTABLE
   if (python_interp.empty())
   {
      if (access(STRUC_PYTHON_EXECUTABLE, X_OK) == 0)
      {
         python_interp = STRUC_PYTHON_EXECUTABLE;
      }
   }
#endif
   if (!python_interp.empty())
   {
      std::ostringstream ss;
      ss << python_interp << " -c \"" << python_cmd << "\"";
      return exec(ss.str());
   }
   return "";
}

TEST_CASE("Native storage", "[struc]")
{
   typedef std::tuple<bool,
                      char,
                      char,
                      unsigned short,
                      unsigned long,
                      int,
                      unsigned long long,
                      long long,
                      std::string,
                      float,
                      float,
                      double,
                      void*>
      T;
   std::string pattern("?2cH2xLiQq20s2fdP");
   auto sz = struc::calcsize(pattern);
#if INTPTR_MAX == INT64_MAX
   REQUIRE(sz == 88);
#else
   REQUIRE(sz == 72);
#endif

   T t1(true,
        'a',
        'b',
        0x0c,
        1000,
        -34,
        9000,
        -9000,
        "01234567890123456789",
        2.0,
        -1.0,
        1000.0,
        nullptr);
   auto v = struc::pack(pattern, t1);
   REQUIRE(v.size() == sz);

   T t2;
   struc::unpack(pattern, &v[0], t2);
   CHECK(t1 == t2);

   // parameter pack
   v = struc::pack(pattern,
                   true,
                   'a',
                   'b',
                   0x0c,
                   1000,
                   -34,
                   9000,
                   -9000,
                   "01234567890123456789",
                   2.0,
                   -1.0,
                   1000,
                   nullptr);
   REQUIRE(v.size() == sz);
   T t3;
   struc::unpack(pattern, &v[0], t3);
   CHECK(t1 == t3);

#ifdef STRUC_CHECK_PYTHON
   std::string python_sz = python(python_struct_calcsize(pattern, t1));
   CHECK(std::stoul(python_sz) == sz);
   std::string python_data = python(python_struct_pack(pattern, t1));
   CHECK(python_data == to_hex(v));
#endif
}

TEST_CASE("Little-endian storage", "[struc]")
{
   typedef std::tuple<bool,
                      char,
                      char,
                      unsigned short,
                      unsigned long,
                      int,
                      unsigned long long,
                      long long,
                      std::string,
                      float,
                      float,
                      double>
      T;
   std::string pattern("<?2cH2xLiQq20s2fd");
   auto sz = struc::calcsize(pattern);
   REQUIRE(sz == 67);

   T t1(true,
        'a',
        'b',
        0x0c,
        1000,
        -34,
        9000,
        -9000,
        "01234567890123456789",
        2.0,
        -1.0,
        1000.0);
   auto v = struc::pack(pattern, t1);
   REQUIRE(v.size() == sz);

   T t2;
   struc::unpack(pattern, &v[0], t2);
   CHECK(t1 == t2);

   // parameter pack
   v = struc::pack(pattern,
                   true,
                   'a',
                   'b',
                   0x0c,
                   1000,
                   -34,
                   9000,
                   -9000,
                   "01234567890123456789",
                   2.0,
                   -1.0,
                   1000);
   REQUIRE(v.size() == sz);
   T t3;
   struc::unpack(pattern, &v[0], t3);
   CHECK(t1 == t3);

#ifdef STRUC_CHECK_PYTHON
   std::string python_sz = python(python_struct_calcsize(pattern, t1));
   CHECK(std::stoul(python_sz) == sz);
   std::string python_data = python(python_struct_pack(pattern, t1));
   CHECK(python_data == to_hex(v));
#endif
}

TEST_CASE("Big-endian storage", "[struc]")
{
   typedef std::tuple<bool,
                      char,
                      char,
                      unsigned short,
                      unsigned long,
                      int,
                      unsigned long long,
                      long long,
                      std::string,
                      float,
                      float,
                      double>
      T;
   std::string pattern("!?2cH2xLiQq20s2fd");
   auto sz = struc::calcsize(pattern);
   REQUIRE(sz == 67);

   T t1(true,
        'a',
        'b',
        0x0c,
        1000,
        -34,
        9000,
        -9000,
        "01234567890123456789",
        2.0,
        -1.0,
        1000.0);
   auto v = struc::pack(pattern, t1);
   REQUIRE(v.size() == sz);

   T t2;
   struc::unpack(pattern, &v[0], t2);

   CHECK(t1 == t2);

#ifdef STRUC_CHECK_PYTHON
   std::string python_sz = python(python_struct_calcsize(pattern, t1));
   CHECK(std::stoul(python_sz) == sz);
   std::string python_data = python(python_struct_pack(pattern, t1));
   CHECK(python_data == to_hex(v));
#endif
}

TEST_CASE("Little-endian conversion", "[struc]")
{
   std::string pattern("<L");
   uint32_t i = 0x76451298;
   auto v = struc::pack(pattern, i);
   REQUIRE(v.size() == 4);
   CHECK(v[0] == '\x98');
   CHECK(v[1] == '\x12');
   CHECK(v[2] == '\x45');
   CHECK(v[3] == '\x76');
}

TEST_CASE("Big-endian conversion", "[struc]")
{
   std::string pattern(">L");
   uint32_t i = 0x76451298;
   auto v = struc::pack(pattern, i);
   REQUIRE(v.size() == 4);
   CHECK(v[3] == '\x98');
   CHECK(v[2] == '\x12');
   CHECK(v[1] == '\x45');
   CHECK(v[0] == '\x76');
}

TEST_CASE("Special patterns", "[struc]")
{
   std::string pattern("0s");
   REQUIRE(struc::calcsize(pattern) == 0);
   auto v = struc::pack(pattern, "");
   REQUIRE(v.size() == 0);
   std::string s = "N/A";
   struc::unpack(pattern, &v[0], s);
   CHECK(s == "");

   pattern = "0c";
   REQUIRE(struc::calcsize(pattern) == 0);

   pattern = "ci";
   auto sz = struc::calcsize(pattern);
   REQUIRE(sz == 8);
   v = struc::pack(pattern, '*', 0x12131415);
   REQUIRE(v.size() == sz);
   CHECK(v[0] == 0x2a);
   CHECK(v[1] == 0x00);
   CHECK(v[2] == 0x00);
   CHECK(v[3] == 0x00);
#ifdef BOOST_BIG_ENDIAN
   CHECK(v[4] == 0x12);
   CHECK(v[5] == 0x13);
   CHECK(v[6] == 0x14);
   CHECK(v[7] == 0x15);
#else
   CHECK(v[4] == 0x15);
   CHECK(v[5] == 0x14);
   CHECK(v[6] == 0x13);
   CHECK(v[7] == 0x12);
#endif

   pattern = "ic";
   sz = struc::calcsize(pattern);
   REQUIRE(sz == 5);
   v = struc::pack(pattern, 0x12131415, '*');
   REQUIRE(v.size() == sz);
#ifdef BOOST_BIG_ENDIAN
   CHECK(v[0] == 0x12);
   CHECK(v[1] == 0x13);
   CHECK(v[2] == 0x14);
   CHECK(v[3] == 0x15);
#else
   CHECK(v[0] == 0x15);
   CHECK(v[1] == 0x14);
   CHECK(v[2] == 0x13);
   CHECK(v[3] == 0x12);
#endif
   CHECK(v[4] == 0x2a);

   pattern = "llh0l";
   sz = struc::calcsize(
      pattern); // pads 2 (32-bit arch) or 6 bytes (64-bit arch) at end
#if INTPTR_MAX == INT64_MAX
   REQUIRE(sz == 24);
#else
   REQUIRE(sz == 12);
#endif

   pattern = "2h \t  2h";
   sz = struc::calcsize(pattern);
   REQUIRE(sz == 8);
   v = struc::pack(pattern, 1, 2, 3, 4);
   REQUIRE(v.size() == sz);
   short h1, h2, h3, h4;
   struc::unpack(pattern, &v[0], h1, h2, h3, h4);
   CHECK(h1 == 1);
   CHECK(h2 == 2);
   CHECK(h3 == 3);
   CHECK(h4 == 4);
}

TEST_CASE("C-style strings", "[struc}")
{
   std::string pattern("10s");
   const size_t sz = 10;
   char arr[11] = "0123456789";
   const char* pnt = "9876543210";
   char buf[128];
   char* pbuf = new char[128];
   auto v = struc::pack(pattern, arr);
   REQUIRE(v.size() == sz);
   struc::unpack(pattern, &v[0], buf);
   CHECK(std::strcmp(buf, arr) == 0);
   struc::unpack(pattern, &v[0], pbuf);
   CHECK(std::strcmp(pbuf, arr) == 0);
   v = struc::pack(pattern, pnt);
   REQUIRE(v.size() == sz);
   struc::unpack(pattern, &v[0], buf);
   CHECK(std::strcmp(buf, pnt) == 0);
   struc::unpack(pattern, &v[0], pbuf);
   CHECK(std::strcmp(pbuf, pnt) == 0);
   delete[] pbuf;
}

TEST_CASE("Too many arguments throws exception", "[struc]")
{
   typedef std::tuple<short, short, short, short, short> T;
   std::string pattern("2h");
   T t1(1, 2, 3, 4, 5);
   CHECK_THROWS_AS(struc::pack(pattern, 1, 2, 3, 4, 5), std::overflow_error);
   CHECK_THROWS_AS(struc::pack(pattern, t1), std::overflow_error);
   auto v = struc::pack(pattern, 1, 2);
   short h1 = 0, h2 = 0, h3 = 0, h4 = 0, h5 = 0;
   CHECK_THROWS_AS(struc::unpack(pattern, &v[0], h1, h2, h3, h4, h5),
                   std::overflow_error);
   T t2;
   CHECK_THROWS_AS(struc::unpack(pattern, &v[0], t2), std::overflow_error);
}

TEST_CASE("Too few arguments throws exception", "[struc]")
{
   typedef std::tuple<short, short> T;
   std::string pattern("5h");
   T t1(1, 2);
   CHECK_THROWS_AS(struc::pack(pattern, 1, 2), std::underflow_error);
   CHECK_THROWS_AS(struc::pack(pattern, t1), std::underflow_error);
   auto v = struc::pack(pattern, 1, 2, 3, 4, 5);
   short h1 = 0, h2 = 0;
   CHECK_THROWS_AS(struc::unpack(pattern, &v[0], h1, h2), std::underflow_error);
   T t2;
   CHECK_THROWS_AS(struc::unpack(pattern, &v[0], t2), std::underflow_error);
}

TEST_CASE("Illegal type throws exception", "[struc]")
{
   std::string pattern("10sPdh");
   std::string s1 = "0123456789";
   std::string s2 = "012345";
   std::vector<char> v;
   CHECK_NOTHROW(v = struc::pack(pattern, s1, &s1, 1.0, 1));
   // string of wrong size
   CHECK_THROWS_AS(struc::pack(pattern, s2, &s2, 1.0, 1), std::logic_error);
   // pointer is not pointer
   CHECK_THROWS_AS(struc::pack(pattern, s2, 1, 1.0, 1), std::logic_error);
   // double is not artithmetic
   CHECK_THROWS_AS(struc::pack(pattern, s1, &s1, s2, 1), std::logic_error);
   // short is not artithmetic
   CHECK_THROWS_AS(struc::pack(pattern, s1, &s1, 1.0, s2), std::logic_error);
}

TEST_CASE("Illegal pattern throw exception", "[struc]")
{
   std::string pattern("@?2cH2xLiQq20s2fdPa");
   CHECK_THROWS_AS(struc::calcsize(pattern), std::logic_error);
   pattern = ">P";
   CHECK_THROWS_AS(struc::calcsize(pattern), std::logic_error);
   pattern = "@P";
   CHECK_NOTHROW(struc::calcsize(pattern));
}
