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

#pragma once

#include <atomic>
#include <boost/endian/arithmetic.hpp>
#include <cmath>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

//! @brief Class mimicing python's pack module
class struc
{
public:
   //! @brief Constructor
   explicit struc(const std::string& pattern);

   //! @brief Like python's struct.pack
   //! @{
   template <typename... T>
   void pack(char* buffer, const T&... t) const;
   template <typename... T>
   void pack(char* buffer, const std::tuple<T...>& t) const;
   template <typename... T>
   static std::vector<char> pack(const std::string& pattern, const T&... t);
   template <typename... T>
   static std::vector<char> pack(const std::string& pattern,
                                 const std::tuple<T...>& t);
   //! @}

   //! @brief Like python's struct.unpack
   //! @{
   template <typename... T>
   void unpack(const char* buffer, T&... t) const;
   template <typename... T>
   void unpack(const char* buffer, std::tuple<T...>& t) const;
   template <typename... T>
   static void unpack(const std::string& pattern, const char* buffer, T&... t);
   template <typename... T>
   static void unpack(const std::string& pattern,
                      const char* buffer,
                      std::tuple<T...>& t);
   //! @}

   //! @brief Like python's struct.calcsize
   //! @{
   size_t calcsize() const;
   static size_t calcsize(const std::string& pattern);
   //! @}

private:
   enum control
   {
      native,
      standard,
      litte_endian,
      big_endian,
   };

   template <typename I>
   static typename std::enable_if<std::is_integral<I>::value, void>::type
      to_endian(control c, I& i);

   template <typename F>
   static typename std::enable_if<std::is_floating_point<F>::value, void>::type
      to_endian(control c, F& f);

   template <typename I>
   static typename std::enable_if<std::is_integral<I>::value, void>::type
      from_endian(control c, I& i);

   template <typename F>
   static typename std::enable_if<std::is_floating_point<F>::value, void>::type
      from_endian(control c, F& f);

   template <typename S>
   static
      typename std::enable_if<std::is_same<std::string, S>::value, void>::type
      check_scalar(size_t num, const S& s);

   template <typename S>
   static typename std::enable_if<std::is_constructible<std::string, S>::value
                                     && !std::is_null_pointer<S>::value
                                     && !std::is_same<std::string, S>::value,
                                  void>::type
      check_scalar(size_t num, const S& s);

   template <typename S>
   static typename std::enable_if<!std::is_constructible<std::string, S>::value
                                     || std::is_null_pointer<S>::value,
                                  void>::type
      check_scalar(size_t num, const S& s);

   template <typename S>
   static
      typename std::enable_if<std::is_same<std::string, S>::value, void>::type
      prep_scalar(size_t num, S& s);

   template <typename S>
   static typename std::enable_if<std::is_constructible<std::string, S>::value
                                     && !std::is_null_pointer<S>::value
                                     && !std::is_same<std::string, S>::value,
                                  void>::type
      prep_scalar(size_t num, S& s);

   template <typename S>
   static typename std::enable_if<!std::is_constructible<std::string, S>::value
                                     || std::is_null_pointer<S>::value,
                                  void>::type
      prep_scalar(size_t num, S& s);

   template <typename T>
   static size_t padding(const size_t& sz);

   static size_t native_padding(const size_t& sz, char type);

   static size_t native_alignment(char type);

   template <typename F>
   static typename std::enable_if<std::is_same<F, float>::value, bool>::type
      is_ieee();

   template <typename F>
   static typename std::enable_if<std::is_same<F, double>::value, bool>::type
      is_ieee();

   template <typename F>
   static typename std::enable_if<!std::is_same<F, float>::value
                                     && !std::is_same<F, double>::value,
                                  bool>::type
      is_ieee();

   template <typename F>
   static typename std::enable_if<std::is_same<F, float>::value, void>::type
      pack_non_ieee(bool litte_endian, char* buffer, size_t& offset, F f);

   template <typename F>
   static typename std::enable_if<std::is_same<F, double>::value, void>::type
      pack_non_ieee(bool litte_endian, char* buffer, size_t& offset, F f);
   template <typename F>

   static typename std::enable_if<!std::is_same<F, float>::value
                                     && !std::is_same<F, double>::value,
                                  void>::type
      pack_non_ieee(bool litte_endian, char* buffer, size_t& offset, F f);

   template <typename I, typename T, typename U = T>
   static typename std::enable_if<std::is_integral<T>::value, void>::type
      pack_native(char* buffer, size_t& offset, const I& i);

   template <typename F, typename T, typename U = T>
   static typename std::enable_if<std::is_floating_point<T>::value, void>::type
      pack_native(char* buffer, size_t& offset, const F& f);

   template <typename I, typename T, typename U = T>
   static typename std::enable_if<std::is_integral<T>::value, void>::type
      pack_non_native(control c, char* buffer, size_t& offset, const I& i);

   template <typename F, typename T, typename U = T>
   static typename std::enable_if<std::is_floating_point<T>::value, void>::type
      pack_non_native(control c, char* buffer, size_t& offset, const F& f);

   template <typename I>
   static typename std::enable_if<std::is_arithmetic<I>::value, void>::type
      pack_scalar(control c,
                  std::pair<size_t, char>& cur,
                  char* buffer,
                  size_t& offset,
                  const I& i);

   template <typename A>
   static typename std::enable_if<std::is_array<A>::value
                                     && !std::is_constructible<std::string,
                                                               A>::value,
                                  void>::type
      pack_scalar(control c,
                  std::pair<size_t, char>& cur,
                  char* buffer,
                  size_t& offset,
                  const A& a);

   template <typename S>
   static typename std::enable_if<std::is_constructible<std::string, S>::value
                                     && !std::is_null_pointer<S>::value,
                                  void>::type
      pack_scalar(control c,
                  std::pair<size_t, char>& cur,
                  char* buffer,
                  size_t& offset,
                  const S& s);

   template <typename P>
   static
      typename std::enable_if<(std::is_pointer<P>::value
                               && !std::is_constructible<std::string, P>::value)
                                 || std::is_null_pointer<P>::value
                                 || std::is_member_pointer<P>::value,
                              void>::type
      pack_scalar(control c,
                  std::pair<size_t, char>& cur,
                  char* buffer,
                  size_t& offset,
                  const P& p);

   template <typename F>
   static typename std::enable_if<std::is_same<F, float>::value, void>::type
      unpack_non_ieee(bool litte_endian,
                      const char* buffer,
                      size_t& offset,
                      F& f);

   template <typename F>
   static typename std::enable_if<std::is_same<F, double>::value, void>::type
      unpack_non_ieee(bool litte_endian,
                      const char* buffer,
                      size_t& offset,
                      F& f);

   template <typename F>
   static typename std::enable_if<!std::is_same<F, float>::value
                                     && !std::is_same<F, double>::value,
                                  void>::type
      unpack_non_ieee(bool litte_endian,
                      const char* buffer,
                      size_t& offset,
                      F& f);

   template <typename I, typename T, typename U = T>
   static typename std::enable_if<std::is_integral<T>::value, void>::type
      unpack_native(const char* buffer, size_t& offset, I& i);

   template <typename F, typename T, typename U = T>
   static typename std::enable_if<std::is_floating_point<T>::value, void>::type
      unpack_native(const char* buffer, size_t& offset, F& f);

   template <typename I, typename T, typename U = T>
   static typename std::enable_if<std::is_integral<T>::value, void>::type
      unpack_non_native(control c, const char* buffer, size_t& offset, I& i);

   template <typename F, typename T, typename U = T>
   static typename std::enable_if<std::is_floating_point<T>::value, void>::type
      unpack_non_native(control c, const char* buffer, size_t& offset, F& f);

   template <typename I>
   static typename std::enable_if<std::is_arithmetic<I>::value, void>::type
      unpack_scalar(control c,
                    std::pair<size_t, char>& cur,
                    const char* buffer,
                    size_t& offset,
                    I& i);

   template <typename A>
   static typename std::enable_if<std::is_array<A>::value
                                     && !std::is_constructible<std::string,
                                                               A>::value,
                                  void>::type
      unpack_scalar(control c,
                    std::pair<size_t, char>& cur,
                    const char* buffer,
                    size_t& offset,
                    A& a);

   template <typename S>
   static
      typename std::enable_if<std::is_same<std::string, S>::value, void>::type
      unpack_scalar(control c,
                    std::pair<size_t, char>& cur,
                    const char* buffer,
                    size_t& offset,
                    S& s);

   template <typename S>
   static typename std::enable_if<std::is_constructible<std::string, S>::value
                                     && !std::is_null_pointer<S>::value
                                     && !std::is_same<std::string, S>::value,
                                  void>::type
      unpack_scalar(control c,
                    std::pair<size_t, char>& cur,
                    const char* buffer,
                    size_t& offset,
                    S& s);

   template <typename P>
   static
      typename std::enable_if<(std::is_pointer<P>::value
                               && !std::is_constructible<std::string, P>::value)
                                 || std::is_member_pointer<P>::value,
                              void>::type
      unpack_scalar(control c,
                    std::pair<size_t, char>& cur,
                    const char* buffer,
                    size_t& offset,
                    P& p);

   template <typename T>
   size_t pack_helper(std::pair<size_t, size_t>& pos,
                      std::pair<size_t, char>& cur,
                      char*& buffer,
                      size_t& offset,
                      const T& t) const;

   template <typename T, typename... Ts>
   size_t pack_helper(std::pair<size_t, size_t>& pos,
                      std::pair<size_t, char>& cur,
                      char*& buffer,
                      size_t& offset,
                      const T& t,
                      const Ts&... ts) const;

   template <size_t I = 0, typename... T>
   typename std::enable_if<I == sizeof...(T), size_t>::type pack_helper_t(
      std::pair<size_t, size_t>& pos,
      std::pair<size_t, char>& cur,
      char*& buffer,
      size_t& offset,
      const std::tuple<T...>& t) const;

   template <size_t I = 0, typename... T>
      typename std::enable_if < I<sizeof...(T), size_t>::type pack_helper_t(
                                   std::pair<size_t, size_t>& pos,
                                   std::pair<size_t, char>& cur,
                                   char*& buffer,
                                   size_t& offset,
                                   const std::tuple<T...>& t) const;

   template <typename T>
   size_t unpack_helper(std::pair<size_t, size_t>& pos,
                        std::pair<size_t, char>& cur,
                        const char*& buffer,
                        size_t& offset,
                        T& t) const;

   template <typename T, typename... Ts>
   size_t unpack_helper(std::pair<size_t, size_t>& pos,
                        std::pair<size_t, char>& cur,
                        const char*& buffer,
                        size_t& offset,
                        T& t,
                        Ts&... ts) const;

   template <size_t I = 0, typename... T>
   typename std::enable_if<I == sizeof...(T), size_t>::type unpack_helper_t(
      std::pair<size_t, size_t>& pos,
      std::pair<size_t, char>& cur,
      const char*& buffer,
      size_t& offset,
      std::tuple<T...>& t) const;

   template <size_t I = 0, typename... T>
      typename std::enable_if < I<sizeof...(T), size_t>::type unpack_helper_t(
                                   std::pair<size_t, size_t>& pos,
                                   std::pair<size_t, char>& cur,
                                   const char*& buffer,
                                   size_t& offset,
                                   std::tuple<T...>& t) const;

   size_t calcsize_helper(std::pair<size_t, size_t>& pos,
                          size_t& no_of_items) const;

   std::string pattern;
   control c;
};

template <typename I>
inline typename std::enable_if<std::is_integral<I>::value, void>::type
   struc::to_endian(control c, I& i)
{
   if (c == litte_endian)
   {
      boost::endian::native_to_little_inplace(i);
   }
   else
   {
      boost::endian::native_to_big_inplace(i);
   }
}

template <typename F>
inline typename std::enable_if<std::is_floating_point<F>::value, void>::type
   struc::to_endian(control, F&)
{
   throw std::runtime_error(
      "Internal error: Floating point endian conversion not allowed");
}

template <typename I>
inline typename std::enable_if<std::is_integral<I>::value, void>::type
   struc::from_endian(control c, I& i)
{
   if (c == litte_endian)
   {
      boost::endian::little_to_native_inplace(i);
   }
   else
   {
      boost::endian::big_to_native_inplace(i);
   }
}

template <typename F>
inline typename std::enable_if<std::is_floating_point<F>::value, void>::type
   struc::from_endian(control, F&)
{
   throw std::runtime_error(
      "Internal error: Floating point endian conversion not allowed");
}

template <typename S>
inline typename std::enable_if<std::is_same<std::string, S>::value, void>::type
   struc::check_scalar(size_t num, const S& s)
{
   if (s.size() != num)
   {
      throw std::logic_error(std::string("String has wrong length ")
                             + std::to_string(s.size())
                             + ", expected "
                             + std::to_string(num));
   }
}

template <typename S>
inline typename std::enable_if<std::is_constructible<std::string, S>::value
                                  && !std::is_null_pointer<S>::value
                                  && !std::is_same<std::string, S>::value,
                               void>::type
   struc::check_scalar(size_t num, const S& s)
{
   if (std::strlen(s) != num)
   {
      throw std::logic_error(std::string("String has wrong length ")
                             + std::to_string(std::strlen(s))
                             + ", expected "
                             + std::to_string(num));
   }
}

template <typename S>
inline typename std::enable_if<!std::is_constructible<std::string, S>::value
                                  || std::is_null_pointer<S>::value,
                               void>::type

   struc::check_scalar(size_t, const S&)
{
}

template <typename S>
inline typename std::enable_if<std::is_same<std::string, S>::value, void>::type
   struc::prep_scalar(size_t num, S& s)
{
   s = S(num, '\0');
}

template <typename S>
inline typename std::enable_if<std::is_constructible<std::string, S>::value
                                  && !std::is_null_pointer<S>::value
                                  && !std::is_same<std::string, S>::value,
                               void>::type
   struc::prep_scalar(size_t num, S& s)
{
   for (size_t i = 0; i < num; ++i)
   {
      s[i] = ' ';
   }
   s[num] = '\0';
}

template <typename S>
inline typename std::enable_if<!std::is_constructible<std::string, S>::value
                                  || std::is_null_pointer<S>::value,
                               void>::type
   struc::prep_scalar(size_t, S&)
{
}

template <typename T>
inline size_t struc::padding(const size_t& sz)
{
   size_t pad = sz % alignof(T);
   return pad == 0 ? pad : alignof(T) - pad;
}

inline size_t struc::native_padding(const size_t& sz, char type)
{
   switch (type)
   {
   case 'h':
      return padding<short>(sz);
   case 'H':
      return padding<unsigned short>(sz);
   case 'i':
      return padding<int>(sz);
   case 'I':
      return padding<unsigned int>(sz);
   case 'l':
      return padding<long>(sz);
   case 'L':
      return padding<unsigned long>(sz);
   case 'q':
      return padding<long long>(sz);
   case 'Q':
      return padding<unsigned long long>(sz);
   case 'f':
      return padding<float>(sz);
   case 'd':
      return padding<double>(sz);
   case 'P':
      return padding<void*>(sz);
   case 'x':
   case 'c':
   case 'b':
   case 'B':
   case '?':
   case 's':
   case 'p':
      return 0;
   default:
      throw std::logic_error(std::string("Encountered illegal type: ") + type);
   }
}

inline size_t struc::native_alignment(char type)
{
   switch (type)
   {
   case 'h':
      return alignof(short);
   case 'H':
      return alignof(unsigned short);
   case 'i':
      return alignof(int);
   case 'I':
      return alignof(unsigned int);
   case 'l':
      return alignof(long);
   case 'L':
      return alignof(unsigned long);
   case 'q':
      return alignof(long long);
   case 'Q':
      return alignof(unsigned long long);
   case 'f':
      return alignof(float);
   case 'd':
      return alignof(double);
   case 'P':
      return alignof(void*);
   case 'x':
   case 'c':
   case 'b':
   case 'B':
   case '?':
   case 's':
   case 'p':
      return 1;
   default:
      throw std::logic_error(std::string("Encountered illegal type: ") + type);
   }
}

template <typename F>
inline typename std::enable_if<std::is_same<F, float>::value, bool>::type
   struc::is_ieee()
{
   static std::atomic_flag flag = ATOMIC_FLAG_INIT;
   static bool is_ieee_float = false;
   if (!flag.test_and_set())
   {
      if (sizeof(double) == 8)
      {
         float x = 16711938.0f;
#ifdef BOOST_BIG_ENDIAN
         is_ieee_float = memcmp(&x, "\x4b\x7f\x01\x02", 4) == 0;
#else
         is_ieee_float = memcmp(&x, "\x02\x01\x7f\x4b", 4) == 0;
#endif
      }
      else
      {
         is_ieee_float = false;
      }
   }
   return is_ieee_float;
}

template <typename F>
inline typename std::enable_if<std::is_same<F, double>::value, bool>::type
   struc::is_ieee()
{
   static std::atomic_flag flag = ATOMIC_FLAG_INIT;
   static bool is_ieee_double = false;
   if (!flag.test_and_set())
   {
      if (sizeof(double) == 8)
      {
         double x = 9006104071832581.0;
#ifdef BOOST_BIG_ENDIAN
         is_ieee_double =
            memcmp(&x, "\x43\x3f\xff\x01\x02\x03\x04\x05", 8) == 0;
#else
         is_ieee_double =
            memcmp(&x, "\x05\x04\x03\x02\x01\xff\x3f\x43", 8) == 0;
#endif
      }
      else
      {
         is_ieee_double = false;
      }
   }
   return is_ieee_double;
}

template <typename F>
inline typename std::enable_if<!std::is_same<F, float>::value
                                  && !std::is_same<F, double>::value,
                               bool>::type
   struc::is_ieee()
{
   throw std::runtime_error(
      "Internal error: Only float or double allowed for ieee check");
}

template <typename F>
inline typename std::enable_if<std::is_same<F, float>::value, void>::type
   struc::pack_non_ieee(bool litte_endian, char* buffer, size_t& offset, F f)
{
   uint8_t sign = f < 0 ? 1 : 0;
   if (sign)
   {
      f = -f;
   }
   int e;
   auto d = std::frexp(f, &e);
   if (0.5 <= d && d < 1.0)
   {
      d *= 2.0;
      e--;
   }
   else if (d == 0.0)
   {
      e = 0;
   }
   else
   {
      throw std::runtime_error("frexp() result is out of range");
   }
   if (e >= 128)
   {
      throw std::overflow_error("float is too large to pack into ieee format");
   }
   else if (e < -126)
   {
      d = std::ldexp(d, 126 + e);
      e = 0;
   }
   else if (!(e == 0 && d == 0.0))
   {
      e += 127;
      d -= 1.0;
   }
   d *= 8388608.0;
   uint32_t bits = static_cast<uint32_t>(d + 0.5);
   if (bits >> 23 > 0)
   {
      bits = 0;
      ++e;
      if (e >= 255)
      {
         throw std::overflow_error(
            "float is too large to pack into ieee format");
      }
   }
   if (litte_endian)
   {
      buffer[offset + 3] = static_cast<char>((sign << 7) | (e >> 1));
      buffer[offset + 2] = static_cast<char>(
         ((static_cast<unsigned int>(e) & 0x1) << 7) | (bits >> 16));
      buffer[offset + 1] = static_cast<char>((bits >> 8) & 0xff);
      buffer[offset] = static_cast<char>(bits & 0xff);
   }
   else
   {
      buffer[offset] = static_cast<char>((sign << 7) | (e >> 1));
      buffer[offset + 1] = static_cast<char>(
         ((static_cast<unsigned int>(e) & 0x1) << 7) | (bits >> 16));
      buffer[offset + 2] = static_cast<char>((bits >> 8) & 0xff);
      buffer[offset + 3] = static_cast<char>(bits & 0xff);
   }
   offset += 4;
}

template <typename F>
inline typename std::enable_if<std::is_same<F, double>::value, void>::type
   struc::pack_non_ieee(bool litte_endian, char* buffer, size_t& offset, F f)
{
   uint8_t sign = f < 0 ? 1 : 0;
   if (sign)
   {
      f = -f;
   }
   int e;
   auto d = std::frexp(f, &e);
   if (0.5 <= d && d < 1.0)
   {
      d *= 2.0;
      e--;
   }
   else if (d == 0.0)
   {
      e = 0;
   }
   else
   {
      throw std::runtime_error("frexp() result is out of range");
   }
   if (e >= 1024)
   {
      throw std::overflow_error("double is too large to pack into ieee format");
   }
   else if (e < -1022)
   {
      d = std::ldexp(d, 1022 + e);
      e = 0;
   }
   else if (!(e == 0 && d == 0.0))
   {
      e += 1023;
      d -= 1.0;
   }
   d *= 268435456.0;
   uint32_t hi = static_cast<uint32_t>(d);
   d -= static_cast<double>(hi);
   d *= 16777216.0;
   uint32_t lo = static_cast<uint32_t>(d + 0.5);
   if (lo >> 24 > 0)
   {
      lo = 0;
      ++hi;
      if (hi >> 28 > 0)
      {
         hi = 0;
         ++e;
         if (e >= 2047)
         {
            throw std::overflow_error(
               "double is too large to pack into ieee format");
         }
      }
   }
   if (litte_endian)
   {
      buffer[offset + 7] = static_cast<char>((sign << 7) | (e >> 4));
      buffer[offset + 6] = static_cast<char>(
         ((static_cast<uint32_t>(e) & 0xf) << 4) | (hi >> 24));
      buffer[offset + 5] = static_cast<char>((hi >> 16) & 0xff);
      buffer[offset + 4] = static_cast<char>((hi >> 8) & 0xff);
      buffer[offset + 3] = static_cast<char>(hi & 0xff);
      buffer[offset + 2] = static_cast<char>((lo >> 16) & 0xff);
      buffer[offset + 1] = static_cast<char>((lo >> 8) & 0xff);
      buffer[offset] = static_cast<char>(lo & 0xff);
   }
   else
   {
      buffer[offset] = static_cast<char>((sign << 7) | (e >> 4));
      buffer[offset + 1] = static_cast<char>(
         ((static_cast<uint32_t>(e) & 0xf) << 4) | (hi >> 24));
      buffer[offset + 2] = static_cast<char>((hi >> 16) & 0xff);
      buffer[offset + 3] = static_cast<char>((hi >> 8) & 0xff);
      buffer[offset + 4] = static_cast<char>(hi & 0xff);
      buffer[offset + 5] = static_cast<char>((lo >> 16) & 0xff);
      buffer[offset + 6] = static_cast<char>((lo >> 8) & 0xff);
      buffer[offset + 7] = static_cast<char>(lo & 0xff);
   }
   offset += 8;
}

template <typename F>
inline typename std::enable_if<!std::is_same<F, float>::value
                                  && !std::is_same<F, double>::value,
                               void>::type
   struc::pack_non_ieee(bool, char*, size_t&, F)
{
   throw std::runtime_error(
      "Internal error: Only float or double allowed for ieee packing");
}

template <typename I, typename T, typename U>
inline typename std::enable_if<std::is_integral<T>::value, void>::type
   struc::pack_native(char* buffer, size_t& offset, const I& i)
{
   T i_ = static_cast<T>(i);
   size_t pad = padding<T>(offset);
   if (pad > 0)
   {
      offset += pad;
   }
   std::memcpy(buffer + offset, &i_, sizeof(i_));
   offset += sizeof(i_);
}

template <typename F, typename T, typename U>
inline typename std::enable_if<std::is_floating_point<T>::value, void>::type
   struc::pack_native(char* buffer, size_t& offset, const F& f)
{
   T f_ = static_cast<T>(f);
   size_t pad = padding<T>(offset);
   if (pad > 0)
   {
      offset += pad;
   }
   if (is_ieee<T>())
   {
      std::memcpy(buffer + offset, &f_, sizeof(f_));
      offset += sizeof(f_);
   }
   else
   {
#ifdef BOOST_BIG_ENDIAN
      pack_non_ieee<T>(false, buffer, offset, f_);
#else
      pack_non_ieee<T>(true, buffer, offset, f_);
#endif
   }
}

template <typename I, typename T, typename U>
inline typename std::enable_if<std::is_integral<T>::value, void>::type
   struc::pack_non_native(control c, char* buffer, size_t& offset, const I& i)
{
   T i_ = static_cast<T>(i);
   to_endian(c, i_);
   std::memcpy(buffer + offset, &i_, sizeof(i_));
   offset += sizeof(i_);
}

template <typename F, typename T, typename U>
inline typename std::enable_if<std::is_floating_point<T>::value, void>::type
   struc::pack_non_native(control c, char* buffer, size_t& offset, const F& f)
{
   static_assert(sizeof(T) == sizeof(U), "Types T and U must have equal size");
   T f_ = static_cast<T>(f);
   if (is_ieee<T>())
   {
      U* i = reinterpret_cast<U*>(&f_);
      to_endian(c, *i);
      std::memcpy(buffer + offset, &f_, sizeof(f_));
      offset += sizeof(f_);
   }
   else
   {
      pack_non_ieee<T>(c == litte_endian, buffer, offset, f_);
   }
}

template <typename I>
inline typename std::enable_if<std::is_arithmetic<I>::value, void>::type
   struc::pack_scalar(control c,
                      std::pair<size_t, char>& cur,
                      char* buffer,
                      size_t& offset,
                      const I& i)
{
   switch (cur.second)
   {
   case 'c':
   case 'b':
   case 'B':
   case '?':
      std::memcpy(buffer + offset, &i, 1);
      offset += 1;
      break;
   case 'h':
      c == native ? pack_native<I, short>(buffer, offset, i) :
                    pack_non_native<I, int16_t>(c, buffer, offset, i);
      break;
   case 'H':
      c == native ? pack_native<I, unsigned short>(buffer, offset, i) :
                    pack_non_native<I, uint16_t>(c, buffer, offset, i);
      break;
   case 'i':
      c == native ? pack_native<I, int>(buffer, offset, i) :
                    pack_non_native<I, int32_t>(c, buffer, offset, i);
      break;
   case 'I':
      c == native ? pack_native<I, unsigned int>(buffer, offset, i) :
                    pack_non_native<I, uint32_t>(c, buffer, offset, i);
      break;
   case 'l':
      c == native ? pack_native<I, long>(buffer, offset, i) :
                    pack_non_native<I, int32_t>(c, buffer, offset, i);
      break;
   case 'L':
      c == native ? pack_native<I, unsigned long>(buffer, offset, i) :
                    pack_non_native<I, uint32_t>(c, buffer, offset, i);
      break;
   case 'q':
      c == native ? pack_native<I, long long>(buffer, offset, i) :
                    pack_non_native<I, int64_t>(c, buffer, offset, i);
      break;
   case 'Q':
      c == native ? pack_native<I, unsigned long long>(buffer, offset, i) :
                    pack_non_native<I, uint64_t>(c, buffer, offset, i);
      break;
   case 'f':
      c == native ? pack_native<I, float>(buffer, offset, i) :
                    pack_non_native<I, float, uint32_t>(c, buffer, offset, i);
      break;
   case 'd':
      c == native ? pack_native<I, double>(buffer, offset, i) :
                    pack_non_native<I, double, uint64_t>(c, buffer, offset, i);
      break;
   default:
      throw std::logic_error(std::string("Encountered illegal type: ")
                             + cur.second);
   }
   cur.first--;
}

template <typename A>
inline
   typename std::enable_if<std::is_array<A>::value
                              && !std::is_constructible<std::string, A>::value,
                           void>::type
   struc::pack_scalar(control c,
                      std::pair<size_t, char>& cur,
                      char* buffer,
                      size_t& offset,
                      const A& a)
{
   if (std::extent<A>::value < cur.first)
   {
      throw std::underflow_error(std::string("Provided array too small (")
                                 + std::to_string(std::extent<A>::value)
                                 + "), expected "
                                 + std::to_string(cur.first));
   }
   else if (std::extent<A>::value > cur.first)
   {
      throw std::overflow_error(std::string("Provided array too large (")
                                + std::to_string(std::extent<A>::value)
                                + "), expected "
                                + std::to_string(cur.first));
   }
   for (size_t i = 0; i < std::extent<A>::value; ++i)
   {
      pack_scalar(c, cur, buffer, offset, a[i]);
   }
}

template <typename S>
inline typename std::enable_if<std::is_constructible<std::string, S>::value
                                  && !std::is_null_pointer<S>::value,
                               void>::type
   struc::pack_scalar(control c,
                      std::pair<size_t, char>& cur,
                      char* buffer,
                      size_t& offset,
                      const S& s)
{
   switch (cur.second)
   {
   case 's':
   case 'p':
   {
      std::string s_(s);
      std::memcpy(buffer + offset, s_.data(), s_.size());
      offset += s_.size();
      cur.first--;
      break;
   }
   case 'c':
   {
      if (!std::is_array<S>::value)
      {
         throw std::logic_error("Expected array of char");
      }
      if (std::extent<S>::value < cur.first)
      {
         throw std::underflow_error(std::string("Provided array too small (")
                                    + std::to_string(std::extent<S>::value)
                                    + "), expected "
                                    + std::to_string(cur.first));
      }
      else if (std::extent<S>::value > cur.first)
      {
         throw std::overflow_error(std::string("Provided array too large (")
                                   + std::to_string(std::extent<S>::value)
                                   + "), expected "
                                   + std::to_string(cur.first));
      }
      for (size_t i = 0; i < std::extent<S>::value; ++i)
      {
         pack_scalar(c, cur, buffer, offset, s[i]);
      }
      break;
   }
   default:
      throw std::logic_error(std::string("Encountered illegal type: ")
                             + cur.second);
   }
}

template <typename P>
inline
   typename std::enable_if<(std::is_pointer<P>::value
                            && !std::is_constructible<std::string, P>::value)
                              || std::is_null_pointer<P>::value
                              || std::is_member_pointer<P>::value,
                           void>::type
   struc::pack_scalar(control c,
                      std::pair<size_t, char>& cur,
                      char* buffer,
                      size_t& offset,
                      const P& p)
{
   if (cur.second == 'P')
   {
      if (c == native)
      {
         void* p_ = static_cast<void*>(p);
         size_t pad = padding<void*>(offset);
         if (pad > 0)
         {
            offset += pad;
         }
         std::memcpy(buffer + offset, &p_, sizeof(p_));
         offset += sizeof(p_);
         cur.first--;
      }
      else
      {
         throw std::logic_error(
            std::string("native byte order is required for the P format"));
      }
   }
   else
   {
      throw std::logic_error(std::string("Encountered illegal type: ")
                             + cur.second);
   }
}

template <typename T>
inline size_t struc::pack_helper(std::pair<size_t, size_t>& pos,
                                 std::pair<size_t, char>& cur,
                                 char*& buffer,
                                 size_t& offset,
                                 const T& t) const
{
   if (cur.first > 0)
   {
      pack_scalar(c, cur, buffer, offset, t);
      auto alignment = c == native ? native_alignment(cur.second) : 0;
      pos.second = std::max(pos.second, alignment);
      return 1;
   }
   cur.first = 1;
   std::ostringstream ss;
   while (pos.first < pattern.size())
   {
      cur.second = pattern[pos.first++];
      if (std::isspace(cur.second))
      {
         continue;
      }
      if (std::isdigit(cur.second))
      {
         ss << cur.second;
         continue;
      }
      auto n = ss.str();
      ss.clear();
      ss.str("");
      if (n.size() > 0)
      {
         try
         {
            cur.first = std::stoul(n);
         }
         catch (...)
         {
            cur.first = 1;
         }
      }
      if (cur.second == 'x')
      {
         offset += cur.first;
         cur.first = 1;
         continue;
      }
      else if (cur.second == 's' || cur.second == 'p')
      {
         check_scalar(cur.first, t);
         cur.first = 1;
      }
      if (cur.first > 0)
      {
         pack_scalar(c, cur, buffer, offset, t);
         auto alignment = c == native ? native_alignment(cur.second) : 0;
         pos.second = std::max(pos.second, alignment);
      }
      else if (c == native)
      {
         offset += native_padding(offset, cur.second);
         pos.second = std::max(pos.second, native_alignment(cur.second));
         continue;
      }
      return 1;
   }
   return 0;
}

template <typename T, typename... Ts>
inline size_t struc::pack_helper(std::pair<size_t, size_t>& pos,
                                 std::pair<size_t, char>& cur,
                                 char*& buffer,
                                 size_t& offset,
                                 const T& t,
                                 const Ts&... ts) const
{
   auto sz = pack_helper(pos, cur, buffer, offset, t);
   sz += pack_helper(pos, cur, buffer, offset, ts...);
   return sz;
}

template <typename... T>
inline void struc::pack(char* buffer, const T&... t) const
{
   size_t no_of_items, offset = 0;
   std::pair<size_t, size_t> pos(0, 1);
   std::pair<size_t, char> cur(0, 'x');
   auto packed_items = pack_helper(pos, cur, buffer, offset, t...);
   if (packed_items < sizeof...(T))
   {
      throw std::overflow_error(std::string("Extra ")
                                + std::to_string(sizeof...(T)-packed_items)
                                + " arguments to pack");
   }
   calcsize_helper(pos, no_of_items);
   no_of_items += cur.first;
   if (no_of_items > 0)
   {
      throw std::underflow_error(std::string("Missing ")
                                 + std::to_string(no_of_items)
                                 + " arguments to pack");
   }
}

template <typename... T>
inline std::vector<char> struc::pack(const std::string& pattern, const T&... t)
{
   struc s(pattern);
   std::vector<char> v(s.calcsize(), '\0');
   s.pack(&v[0], t...);
   return v;
}

template <size_t I, typename... T>
inline typename std::enable_if<I == sizeof...(T), size_t>::type
   struc::pack_helper_t(std::pair<size_t, size_t>&,
                        std::pair<size_t, char>&,
                        char*&,
                        size_t&,
                        const std::tuple<T...>&) const
{
   return 0;
}

template <size_t I, typename... T>
   inline typename std::enable_if
   < I<sizeof...(T), size_t>::type struc::pack_helper_t(
        std::pair<size_t, size_t>& pos,
        std::pair<size_t, char>& cur,
        char*& buffer,
        size_t& offset,
        const std::tuple<T...>& t) const
{
   auto sz = pack_helper(pos, cur, buffer, offset, std::get<I>(t));
   sz += pack_helper_t<I + 1, T...>(pos, cur, buffer, offset, t);
   return sz;
}

template <typename... T>
inline void struc::pack(char* buffer, const std::tuple<T...>& t) const
{
   size_t no_of_items, offset = 0;
   std::pair<size_t, size_t> pos(0, 1);
   std::pair<size_t, char> cur(0, 'x');
   auto packed_items = pack_helper_t(pos, cur, buffer, offset, t);
   if (packed_items < sizeof...(T))
   {
      throw std::overflow_error(std::string("Extra ")
                                + std::to_string(sizeof...(T)-packed_items)
                                + " arguments to pack");
   }
   calcsize_helper(pos, no_of_items);
   no_of_items += cur.first;
   if (no_of_items > 0)
   {
      throw std::underflow_error(std::string("Missing ")
                                 + std::to_string(no_of_items)
                                 + " arguments to pack");
   }
}

template <typename... T>
inline std::vector<char> struc::pack(const std::string& pattern,
                                     const std::tuple<T...>& t)
{
   struc s(pattern);
   size_t no_of_items, align = 1;
   std::pair<size_t, size_t> pos(0, 1);
   auto sz = s.calcsize_helper(pos, no_of_items);
   if (s.c == native)
   {
      align = pos.second;
   }
   std::vector<char> v(sz, '\0');
   s.pack(&v[0], t);
   return v;
}

template <typename F>
inline typename std::enable_if<std::is_same<F, float>::value, void>::type
   struc::unpack_non_ieee(bool little_endian,
                          const char* buffer,
                          size_t& offset,
                          F& f)
{
   uint8_t sign;
   int e;
   uint32_t bits;
   if (little_endian)
   {
      sign = (static_cast<uint8_t>(buffer[offset + 3]) >> 7) & 0x1;
      e = static_cast<int>((static_cast<uint8_t>(buffer[offset + 3]) & 0x7f)
                           << 1);
      e |= (static_cast<uint8_t>(buffer[offset + 2]) >> 7) & 0x1;
      bits = (static_cast<uint32_t>(buffer[offset + 2]) & 0x7f) << 16;
   }
   else
   {
      sign = (static_cast<uint8_t>(buffer[offset]) >> 7) & 0x1;
      e = static_cast<int>((static_cast<uint8_t>(buffer[offset]) & 0x7f) << 1);
      e |= (static_cast<uint8_t>(buffer[offset + 1]) >> 7) & 0x1;
      bits = (static_cast<uint32_t>(buffer[offset + 1]) & 0x7f) << 16;
   }
   if (e == 255)
   {
      throw std::runtime_error("Can't unpack ieee special value");
   }
   if (little_endian)
   {
      bits |= static_cast<uint32_t>(buffer[offset + 1]) << 8;
      bits |= static_cast<uint32_t>(buffer[offset]);
   }
   else
   {
      bits |= static_cast<uint32_t>(buffer[offset + 2]) << 8;
      bits |= static_cast<uint32_t>(buffer[offset + 3]);
   }
   f = static_cast<double>(bits) / 8388608.0;
   if (e == 0)
   {
      e = -126;
   }
   else
   {
      f += 1.0;
      e -= 127;
   }
   f = std::ldexp(f, e);
   if (sign)
   {
      f = -f;
   }
   offset += 4;
}

template <typename F>
inline typename std::enable_if<std::is_same<F, double>::value, void>::type
   struc::unpack_non_ieee(bool litte_endian,
                          const char* buffer,
                          size_t& offset,
                          F& f)
{
   uint8_t sign;
   int e;
   uint32_t hi;
   if (litte_endian)
   {
      sign = (static_cast<uint8_t>(buffer[offset + 7]) >> 7) & 0x1;
      e = static_cast<int>((static_cast<uint32_t>(buffer[offset + 7]) & 0x7f)
                           << 4);
      e |= (static_cast<uint8_t>(buffer[offset + 6]) >> 4) & 0xf;
      hi = (static_cast<uint32_t>(buffer[offset + 6]) & 0xf) << 24;
   }
   else
   {
      sign = (static_cast<uint8_t>(buffer[offset]) >> 7) & 0x1;
      e = static_cast<int>((static_cast<uint32_t>(buffer[offset]) & 0x7f) << 4);
      e |= (static_cast<uint8_t>(buffer[offset + 1]) >> 4) & 0xf;
      hi = (static_cast<uint32_t>(buffer[offset + 1]) & 0xf) << 24;
   }
   if (e == 2047)
   {
      throw std::runtime_error("Can't unpack ieee special value");
   }
   uint32_t lo;
   if (litte_endian)
   {
      hi |= static_cast<uint32_t>(buffer[offset + 5]) << 16;
      hi |= static_cast<uint32_t>(buffer[offset + 4]) << 8;
      hi |= static_cast<uint32_t>(buffer[offset + 3]);
      lo = static_cast<uint32_t>(buffer[offset + 2]) << 16;
      lo |= static_cast<uint32_t>(buffer[offset + 1]) << 8;
      lo |= static_cast<uint32_t>(buffer[offset]);
   }
   else
   {
      hi |= static_cast<uint32_t>(buffer[offset + 2]) << 16;
      hi |= static_cast<uint32_t>(buffer[offset + 3]) << 8;
      hi |= static_cast<uint32_t>(buffer[offset + 4]);
      lo = static_cast<uint32_t>(buffer[offset + 5]) << 16;
      lo |= static_cast<uint32_t>(buffer[offset + 6]) << 8;
      lo |= static_cast<uint32_t>(buffer[offset + 7]);
   }
   f = static_cast<double>(hi) + static_cast<double>(lo) / 16777216.0;
   f /= 268435456.0;
   if (e == 0)
   {
      e = -1022;
   }
   else
   {
      f += 1.0;
      e -= 1023;
   }
   f = std::ldexp(f, e);
   if (sign)
   {
      f = -f;
   }
   offset += 8;
}

template <typename F>
inline typename std::enable_if<!std::is_same<F, float>::value
                                  && !std::is_same<F, double>::value,
                               void>::type
   struc::unpack_non_ieee(bool, const char*, size_t&, F&)
{
   throw std::runtime_error(
      "Internal error: Only float or double allowed for ieee unpacking");
}

template <typename I, typename T, typename U>
inline typename std::enable_if<std::is_integral<T>::value, void>::type
   struc::unpack_native(const char* buffer, size_t& offset, I& i)
{
   T i_;
   size_t pad = padding<T>(offset);
   if (pad > 0)
   {
      offset += pad;
   }
   std::memcpy(&i_, buffer + offset, sizeof(i_));
   i = static_cast<I>(i_);
   offset += sizeof(i_);
}

template <typename F, typename T, typename U>
inline typename std::enable_if<std::is_floating_point<T>::value, void>::type
   struc::unpack_native(const char* buffer, size_t& offset, F& f)
{
   T f_;
   size_t pad = padding<T>(offset);
   if (pad > 0)
   {
      offset += pad;
   }
   if (is_ieee<T>())
   {
      std::memcpy(&f_, buffer + offset, sizeof(f_));
      f = static_cast<F>(f_);
      offset += sizeof(f_);
   }
   else
   {
#ifdef BOOST_BIG_ENDIAN
      unpack_non_ieee<T>(false, buffer, offset, f_);
#else
      unpack_non_ieee<T>(true, buffer, offset, f_);
#endif
      f = static_cast<F>(f_);
   }
}

template <typename I, typename T, typename U>
inline typename std::enable_if<std::is_integral<T>::value, void>::type
   struc::unpack_non_native(control c, const char* buffer, size_t& offset, I& i)
{
   T i_;
   std::memcpy(&i_, buffer + offset, sizeof(i_));
   from_endian(c, i_);
   i = static_cast<I>(i_);
   offset += sizeof(i_);
}

template <typename F, typename T, typename U>
inline typename std::enable_if<std::is_floating_point<T>::value, void>::type
   struc::unpack_non_native(control c, const char* buffer, size_t& offset, F& f)
{
   static_assert(sizeof(T) == sizeof(U), "Size of T and U must be equal");
   T f_;
   if (is_ieee<T>())
   {
      U* i = reinterpret_cast<U*>(&f_);
      std::memcpy(&f_, buffer + offset, sizeof(f_));
      from_endian(c, *i);
      f = static_cast<F>(f_);
      offset += sizeof(f_);
   }
   else
   {
      unpack_non_ieee<T>(c == litte_endian, buffer, offset, f_);
      f = static_cast<F>(f_);
   }
}

template <typename I>
inline typename std::enable_if<std::is_arithmetic<I>::value, void>::type
   struc::unpack_scalar(control c,
                        std::pair<size_t, char>& cur,
                        const char* buffer,
                        size_t& offset,
                        I& i)
{
   switch (cur.second)
   {
   case 'c':
   case 'b':
   case 'B':
   case '?':
      std::memcpy(&i, buffer + offset, 1);
      offset += 1;
      break;
   case 'h':
      c == native ? unpack_native<I, short>(buffer, offset, i) :
                    unpack_non_native<I, int16_t>(c, buffer, offset, i);
      break;
   case 'H':
      c == native ? unpack_native<I, unsigned short>(buffer, offset, i) :
                    unpack_non_native<I, uint16_t>(c, buffer, offset, i);
      break;
   case 'i':
      c == native ? unpack_native<I, int>(buffer, offset, i) :
                    unpack_non_native<I, int32_t>(c, buffer, offset, i);
      break;
   case 'I':
      c == native ? unpack_native<I, unsigned int>(buffer, offset, i) :
                    unpack_non_native<I, uint32_t>(c, buffer, offset, i);
      break;
   case 'l':
      c == native ? unpack_native<I, long>(buffer, offset, i) :
                    unpack_non_native<I, int32_t>(c, buffer, offset, i);
      break;
   case 'L':
      c == native ? unpack_native<I, unsigned long>(buffer, offset, i) :
                    unpack_non_native<I, uint32_t>(c, buffer, offset, i);
      break;
   case 'q':
      c == native ? unpack_native<I, long long>(buffer, offset, i) :
                    unpack_non_native<I, int64_t>(c, buffer, offset, i);
      break;
   case 'Q':
      c == native ? unpack_native<I, unsigned long long>(buffer, offset, i) :
                    unpack_non_native<I, uint64_t>(c, buffer, offset, i);
      break;
   case 'f':
      c == native ? unpack_native<I, float>(buffer, offset, i) :
                    unpack_non_native<I, float, uint32_t>(c, buffer, offset, i);
      break;
   case 'd':
      c == native ?
         unpack_native<I, double>(buffer, offset, i) :
         unpack_non_native<I, double, uint64_t>(c, buffer, offset, i);
      break;
   default:
      throw std::logic_error(std::string("Encountered illegal type: ")
                             + cur.second);
   }
   cur.first--;
}

template <typename A>
inline
   typename std::enable_if<std::is_array<A>::value
                              && !std::is_constructible<std::string, A>::value,
                           void>::type
   struc::unpack_scalar(control c,
                        std::pair<size_t, char>& cur,
                        const char* buffer,
                        size_t& offset,
                        A& a)
{
   if (std::extent<A>::value < cur.first)
   {
      throw std::underflow_error(std::string("Provided array too small (")
                                 + std::to_string(std::extent<A>::value)
                                 + "), expected "
                                 + std::to_string(cur.first));
   }
   else if (std::extent<A>::value > cur.first)
   {
      throw std::overflow_error(std::string("Provided array too large (")
                                + std::to_string(std::extent<A>::value)
                                + "), expected "
                                + std::to_string(cur.first));
   }
   for (size_t i = 0; i < std::extent<A>::value; ++i)
   {
      unpack_scalar(c, cur, buffer, offset, a[i]);
   }
}

template <typename S>
inline typename std::enable_if<std::is_same<std::string, S>::value, void>::type
   struc::unpack_scalar(control,
                        std::pair<size_t, char>& cur,
                        const char* buffer,
                        size_t& offset,
                        S& s)
{
   switch (cur.second)
   {
   case 's':
   case 'p':
   {
      size_t sz = s.size();
      s = std::string(buffer + offset, sz);
      offset += sz;
      cur.first--;
      break;
   }
   default:
      throw std::logic_error(std::string("Encountered illegal type: ")
                             + cur.second);
   }
}

template <typename S>
inline typename std::enable_if<std::is_constructible<std::string, S>::value
                                  && !std::is_null_pointer<S>::value
                                  && !std::is_same<std::string, S>::value,
                               void>::type
   struc::unpack_scalar(control c,
                        std::pair<size_t, char>& cur,
                        const char* buffer,
                        size_t& offset,
                        S& s)
{
   switch (cur.second)
   {
   case 's':
   case 'p':
   {
      size_t sz = strlen(s);
      std::memcpy(s, buffer + offset, sz);
      offset += sz;
      cur.first--;
      break;
   }
   case 'c':
   {
      if (!std::is_array<S>::value)
      {
         throw std::logic_error("Expected array of char");
      }
      if (std::extent<S>::value < cur.first)
      {
         throw std::underflow_error(std::string("Provided array too small (")
                                    + std::to_string(std::extent<S>::value)
                                    + "), expected "
                                    + std::to_string(cur.first));
      }
      else if (std::extent<S>::value > cur.first)
      {
         throw std::overflow_error(std::string("Provided array too large (")
                                   + std::to_string(std::extent<S>::value)
                                   + "), expected "
                                   + std::to_string(cur.first));
      }
      for (size_t i = 0; i < std::extent<S>::value; ++i)
      {
         unpack_scalar(c, cur, buffer, offset, s[i]);
      }
      break;
   }
   default:
      throw std::logic_error(std::string("Encountered illegal type: ")
                             + cur.second);
   }
}

template <typename P>
inline
   typename std::enable_if<(std::is_pointer<P>::value
                            && !std::is_constructible<std::string, P>::value)
                              || std::is_member_pointer<P>::value,
                           void>::type
   struc::unpack_scalar(control c,
                        std::pair<size_t, char>& cur,
                        const char* buffer,
                        size_t& offset,
                        P& p)
{
   if (cur.second == 'P')
   {
      if (c == native)
      {
         void* p_;
         size_t pad = padding<void*>(offset);
         if (pad > 0)
         {
            offset += pad;
         }
         std::memcpy(&p_, buffer + offset, sizeof(p_));
         p = static_cast<P>(p_);
         offset += sizeof(p_);
         cur.first--;
      }
      else
      {
         throw std::logic_error(
            std::string("native byte order is required for the P format"));
      }
   }
   else
   {
      throw std::logic_error(std::string("Encountered illegal type: ")
                             + cur.second);
   }
}

template <typename T>
inline size_t struc::unpack_helper(std::pair<size_t, size_t>& pos,
                                   std::pair<size_t, char>& cur,
                                   const char*& buffer,
                                   size_t& offset,
                                   T& t) const
{
   if (cur.first > 0)
   {
      unpack_scalar(c, cur, buffer, offset, t);
      auto alignment = c == native ? native_alignment(cur.second) : 0;
      pos.second = std::max(pos.second, alignment);
      return 1;
   }
   cur.first = 1;
   std::ostringstream ss;
   while (pos.first < pattern.size())
   {
      cur.second = pattern[pos.first++];
      if (std::isspace(cur.second))
      {
         continue;
      }
      if (std::isdigit(cur.second))
      {
         ss << cur.second;
         continue;
      }
      auto n = ss.str();
      ss.clear();
      ss.str("");
      if (n.size() > 0)
      {
         try
         {
            cur.first = std::stoul(n);
         }
         catch (...)
         {
            cur.first = 1;
         }
      }
      if (cur.second == 'x')
      {
         offset += cur.first;
         cur.first = 1;
         continue;
      }
      else if (cur.second == 's' || cur.second == 'p')
      {
         prep_scalar(cur.first, t);
         cur.first = 1;
      }
      if (cur.first > 0)
      {
         unpack_scalar(c, cur, buffer, offset, t);
         auto alignment = c == native ? native_alignment(cur.second) : 0;
         pos.second = std::max(pos.second, alignment);
      }
      else if (c == native)
      {
         offset += native_padding(offset, cur.second);
         pos.second = std::max(pos.second, native_alignment(cur.second));
         continue;
      }
      return 1;
   }
   return 0;
}

template <typename T, typename... Ts>
inline size_t struc::unpack_helper(std::pair<size_t, size_t>& pos,
                                   std::pair<size_t, char>& cur,
                                   const char*& buffer,
                                   size_t& offset,
                                   T& t,
                                   Ts&... ts) const
{
   auto sz = unpack_helper(pos, cur, buffer, offset, t);
   sz += unpack_helper(pos, cur, buffer, offset, ts...);
   return sz;
}

template <typename... T>
inline void struc::unpack(const char* buffer, T&... t) const
{
   size_t no_of_items, offset = 0;
   std::pair<size_t, size_t> pos(0, 1);
   std::pair<size_t, char> cur(0, 'x');
   auto unpacked_items = unpack_helper(pos, cur, buffer, offset, t...);
   if (unpacked_items < sizeof...(T))
   {
      throw std::overflow_error(std::string("Extra ")
                                + std::to_string(sizeof...(T)-unpacked_items)
                                + " arguments to unpack");
   }
   calcsize_helper(pos, no_of_items);
   no_of_items += cur.first;
   if (no_of_items > 0)
   {
      throw std::underflow_error(std::string("Missing ")
                                 + std::to_string(no_of_items)
                                 + " arguments to unpack");
   }
}

template <typename... T>
inline void struc::unpack(const std::string& pattern,
                          const char* buffer,
                          T&... t)
{
   struc s(pattern);
   s.unpack(buffer, t...);
}

template <size_t I, typename... T>
inline typename std::enable_if<I == sizeof...(T), size_t>::type
   struc::unpack_helper_t(std::pair<size_t, size_t>&,
                          std::pair<size_t, char>&,
                          const char*&,
                          size_t&,
                          std::tuple<T...>&) const
{
   return 0;
}

template <size_t I, typename... T>
   inline typename std::enable_if
   < I<sizeof...(T), size_t>::type struc::unpack_helper_t(
        std::pair<size_t, size_t>& pos,
        std::pair<size_t, char>& cur,
        const char*& buffer,
        size_t& offset,
        std::tuple<T...>& t) const
{
   auto sz = unpack_helper(pos, cur, buffer, offset, std::get<I>(t));
   sz += unpack_helper_t<I + 1, T...>(pos, cur, buffer, offset, t);
   return sz;
}

template <typename... T>
inline void struc::unpack(const char* buffer, std::tuple<T...>& t) const
{
   size_t no_of_items, offset = 0;
   std::pair<size_t, size_t> pos(0, 1);
   std::pair<size_t, char> cur(0, 'x');
   auto unpacked_items = unpack_helper_t(pos, cur, buffer, offset, t);
   if (unpacked_items < sizeof...(T))
   {
      throw std::overflow_error(std::string("Extra ")
                                + std::to_string(sizeof...(T)-unpacked_items)
                                + " arguments to unpack");
   }
   calcsize_helper(pos, no_of_items);
   no_of_items += cur.first;
   if (no_of_items > 0)
   {
      throw std::underflow_error(std::string("Missing ")
                                 + std::to_string(no_of_items)
                                 + " arguments to unpack");
   }
}

template <typename... T>
inline void struc::unpack(const std::string& pattern,
                          const char* buffer,
                          std::tuple<T...>& t)
{
   struc s(pattern);
   s.unpack(buffer, t);
}

inline struc::struc(const std::string& pattern_)
: pattern(pattern_)
, c(native)
{
   if (pattern.find_first_of("@=<>!") == 0)
   {
      switch (pattern[0])
      {
      case '@':
         c = native;
         break;
      case '=':
         c = standard;
         break;
      case '<':
         c = litte_endian;
         break;
      case '>':
      case '!':
         c = big_endian;
         break;
      default:
         break;
      }
      pattern.erase(0, 1);
   }
}

inline size_t struc::calcsize_helper(std::pair<size_t, size_t>& pos,
                                     size_t& no_of_items) const
{
   size_t size = 0;
   no_of_items = 0;
   std::ostringstream ss;
   while (pos.first < pattern.size())
   {
      char type = pattern[pos.first++];
      if (std::isspace(type))
      {
         continue;
      }
      if (std::isdigit(type))
      {
         ss << type;
         continue;
      }
      auto n = ss.str();
      ss.clear();
      ss.str("");
      size_t num = 1;
      if (n.size() > 0)
      {
         try
         {
            num = std::stoul(n);
         }
         catch (...)
         {
            num = 1;
         }
      }
      size_t sz;
      size_t pad = c == native ? native_padding(size, type) : 0;
      switch (type)
      {
      case 'h':
         sz = c == native ? sizeof(short) : sizeof(int16_t);
         break;
      case 'H':
         sz = c == native ? sizeof(unsigned short) : sizeof(int16_t);
         break;
      case 'i':
         sz = c == native ? sizeof(int) : sizeof(int32_t);
         break;
      case 'I':
         sz = c == native ? sizeof(unsigned int) : sizeof(int32_t);
         break;
      case 'l':
         sz = c == native ? sizeof(long) : sizeof(int32_t);
         break;
      case 'L':
         sz = c == native ? sizeof(unsigned long) : sizeof(int32_t);
         break;
      case 'q':
         sz = c == native ? sizeof(long long) : sizeof(int64_t);
         break;
      case 'Q':
         sz = c == native ? sizeof(unsigned long long) : sizeof(int64_t);
         break;
      case 'f':
         sz = c == native ? sizeof(float) : sizeof(float);
         break;
      case 'd':
         sz = c == native ? sizeof(double) : sizeof(double);
         break;
      case 'P':
         if (c != native)
         {
            throw std::logic_error(
               std::string("native byte order is required for the P format"));
         }
         sz = sizeof(void*);
         break;
      case 'x':
      case 'c':
      case 'b':
      case 'B':
      case '?':
         sz = 1;
         break;
      case 's':
      case 'p':
         sz = num;
         num = 1;
         break;
      default:
         throw std::logic_error(std::string("Encountered illegal type: ")
                                + type);
      }
      size += pad + sz * num;
      if (type != 'x')
      {
         no_of_items += num;
      }
      auto alignment = c == native ? native_alignment(type) : 0;
      pos.second = std::max(pos.second, alignment);
   }
   return size;
}

inline size_t struc::calcsize() const
{
   size_t no_of_items;
   std::pair<size_t, size_t> pos(0, 1);
   return calcsize_helper(pos, no_of_items);
}

inline size_t struc::calcsize(const std::string& pattern)
{
   struc s(pattern);
   return s.calcsize();
}
