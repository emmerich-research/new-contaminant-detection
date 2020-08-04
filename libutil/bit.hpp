#ifndef LIB_UTIL_BIT_HPP_
#define LIB_UTIL_BIT_HPP_

/** @file bit.hpp
 *  @brief bit helper definitions
 */

#include <array>
#include <cstdint>
#include <type_traits>

#include "macros.hpp"

namespace util {
/**
 * Convert bits from higher to lower type
 *
 * @tparam From        from type
 * @tparam To          to type
 *
 * @param buffer     buffer to write
 * @param value      value to split HI / LOW
 * @param start_addr start address of buffer that will be splitted
 */
template <typename From,
          typename To,
          typename = std::enable_if_t<std::is_integral_v<From> &&
                                      std::is_integral_v<To>>>
static void convert_bits(From*               buffer,
                         const From&         value,
                         const unsigned int& start_addr = 0,
                         bool                reverse = false) {
  if constexpr (std::is_same_v<From, std::uint16_t>) {
    if constexpr (std::is_same_v<To, std::uint8_t>) {
      if (reverse) {
        buffer[start_addr + 1] = value >> 8;
        buffer[start_addr] = value & 0x00FF;
      } else {
        buffer[start_addr] = value >> 8;
        buffer[start_addr + 1] = value & 0x00FF;
      }
    }
  } else if constexpr (std::is_same_v<From, std::uint32_t>) {
    if constexpr (std::is_same_v<To, std::uint16_t>) {
      if (reverse) {
        buffer[start_addr + 1] = value >> 16;
        buffer[start_addr] = value; 
      } else {
        buffer[start_addr] = value >> 16;
        buffer[start_addr + 1] = value;
      }                       
    }
  } else if constexpr (std::is_same_v<From, std::uint64_t>) {
    if constexpr (std::is_same_v<To, std::uint16_t>) {
      if (reverse) {
        buffer[start_addr + 3] = value >> 48;
        buffer[start_addr + 2] = value >> 32;
        buffer[start_addr + 1] = value >> 16;
        buffer[start_addr] = value; 
      } else {
        buffer[start_addr] = value >> 48;
        buffer[start_addr + 1] = value >> 32;
        buffer[start_addr + 2] = value >> 16;
        buffer[start_addr + 3] = value; 
      }
    }
  }
}

/**
 * Convert bits from higher to lower type
 *
 * @tparam From        from type
 * @tparam To          to type
 *
 * @param buffer     buffer to write
 * @param value      value to split HI / LOW
 * @param start_addr start address of buffer that will be splitted
 */
template <typename From,
          typename To,
          std::size_t Size,
          typename = std::enable_if_t<std::is_integral_v<From> &&
                                      std::is_integral_v<To>>>
static void convert_bits(std::array<From, Size>& buffer,
                         const From&             value,
                         const unsigned int&     start_addr = 0,
                         bool reverse = false) {
  massert(start_addr < buffer.max_size(), "sanity");

  if constexpr (std::is_same_v<From, std::uint16_t>) {
    if constexpr (std::is_same_v<To, std::uint8_t>) {
      if (reverse) {
        buffer[start_addr + 1] = value >> 8;
        buffer[start_addr] = value & 0x00FF;  
      } else {
        buffer[start_addr] = value >> 8;
        buffer[start_addr + 1] = value & 0x00FF;  
      }
    }
  } else if constexpr (std::is_same_v<From, std::uint32_t>) {
    if constexpr (std::is_same_v<To, std::uint16_t>) {
      if (reverse) {
        buffer[start_addr + 1] = value >> 16;
        buffer[start_addr] = value;
      } else {
        buffer[start_addr] = value >> 16;
        buffer[start_addr + 1] = value;
      }
    }
  } else if constexpr (std::is_same_v<From, std::uint64_t>) {
    if constexpr (std::is_same_v<To, std::uint16_t>) {
      static_assert(start_addr + 3 < Size, "size is bigger than expected size");
      if (reverse) {
        buffer[start_addr + 3] = value >> 48;
        buffer[start_addr + 2] = value >> 32;
        buffer[start_addr + 1] = value >> 16;
        buffer[start_addr] = value;
      } else {
        buffer[start_addr] = value >> 48;
        buffer[start_addr + 1] = value >> 32;
        buffer[start_addr + 2] = value >> 16;
        buffer[start_addr + 3] = value;
      }
    }
  }
}

/**
 * Convert bits from lower to higher type
 *
 * @tparam From        from type
 * @tparam To          to type
 *
 * @param buffer     buffer to read
 * @param start_addr start address of buffer that will be grouped
 *
 * @return value in To type bit
 */
template <typename From,
          typename To,
          typename = std::enable_if_t<std::is_integral_v<From> &&
                                      std::is_integral_v<To>>>
static To convert_bits(const From* buffer, const unsigned int& start_addr = 0, bool reverse = false) {
  if constexpr (std::is_same_v<From, std::uint16_t>) {
    if constexpr (std::is_same_v<To, std::uint32_t>) {
      return static_cast<To>(buffer[start_addr] << 16) | buffer[start_addr + 1];
    } else if constexpr (std::is_same_v<To, std::uint64_t>) {
      if (reverse) {
        return static_cast<To>(
          (buffer[start_addr + 3] << 48) | (buffer[start_addr + 2] << 32) |
          (buffer[start_addr + 1] << 16) | buffer[start_addr]);   
      } else {
        return static_cast<To>(
          (buffer[start_addr] << 48) | (buffer[start_addr + 1] << 32) |
          (buffer[start_addr + 2] << 16) | buffer[start_addr + 3]);
      }
    }
  } else if constexpr (std::is_same_v<From, std::uint8_t>) {
    if constexpr (std::is_same_v<To, std::uint16_t>) {
      if (reverse) {
        return static_cast<To>(buffer[start_addr + 1] << 8) | buffer[start_addr];
      } else {
        return static_cast<To>(buffer[start_addr] << 8) | buffer[start_addr + 1];
      }
    }
  }

  return 0;
}

/**
 * Convert bits from higher to lower type
 *
 * @tparam From        from type
 * @tparam To          to type
 * @tparam Size        array size
 *
 * @param buffer     buffer to read
 * @param start_addr start address of buffer that will be grouped
 *
 * @return value in To type bit
 */
template <typename From,
          typename To,
          std::size_t Size,
          typename = std::enable_if_t<std::is_integral_v<From> &&
                                      std::is_integral_v<To>>>
static To convert_bits(const std::array<From, Size>& buffer,
                       const unsigned int&           start_addr = 0,
                       bool reverse = false) {
  massert(start_addr < buffer.max_size(), "sanity");
  if constexpr (std::is_same_v<From, std::uint16_t>) {
    if constexpr (std::is_same_v<To, std::uint32_t>) {
      return static_cast<To>(buffer[start_addr] << 16) | buffer[start_addr + 1];
    } else if constexpr (std::is_same_v<To, std::uint64_t>) {
      if (reverse) {
        return static_cast<To>(
          (buffer[start_addr + 3] << 48) | (buffer[start_addr + 2] << 32) |
          (buffer[start_addr + 1] << 16) | buffer[start_addr]);
      } else {
        return static_cast<To>(
          (buffer[start_addr] << 48) | (buffer[start_addr + 1] << 32) |
          (buffer[start_addr + 2] << 16) | buffer[start_addr + 3]); 
      }
    }
  } else if constexpr (std::is_same_v<From, std::uint8_t>) {
    if constexpr (std::is_same_v<To, std::uint16_t>) {
      if (reverse) {
        return static_cast<To>(buffer[start_addr + 1] << 8) | buffer[start_addr];
      } else {
        return static_cast<To>(buffer[start_addr] << 8) | buffer[start_addr + 1];
      }
    }
  }

  return 0;
}
}  // namespace util

#endif  // LIB_UTIL_BIT_HPP_
