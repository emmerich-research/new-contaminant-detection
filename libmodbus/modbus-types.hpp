#ifndef LIB_MODBUS_MODBUS_TYPES_HPP_
#define LIB_MODBUS_MODBUS_TYPES_HPP_

#include <cstdint>
#include <exception>
#include <type_traits>
#include <vector>

#include "modbus-constants.hpp"

namespace modbus {
class header_t;
class num_bits_t;
class num_regs_t;

struct header_t {
  /**
   * Transaction id
   */
  std::uint16_t transaction = 0;
  /**
   * Length of PDU
   */
  std::uint16_t length = 0;
  /**
   * Unit id
   */
  std::uint8_t unit = 0;
};

namespace internal {
/**
 * Packet type
 */
typedef std::vector<std::uint8_t> packet_t;

/**
 * Base metadata
 *
 * @tparam T metadata type
 */
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
class base_metadata_t {
 public:
  /**
   * base_metadata constructor
   *
   * @param value value to set
   */
  inline explicit constexpr base_metadata_t(T value = 0) noexcept
      : value_{value} {}

  /**
   * Set value
   *
   * @param value value to set
   */
  inline void set(T value) noexcept { value_ = value; }

  /**
   * Set value [operator()]
   *
   * @param other other metadata
   *
   * @param value value to set
   */
  inline void operator()(T value) noexcept { set(value); }

  /**
   * Get value
   *
   * @param other other metadata
   *
   * @return value
   */
  inline constexpr T get() const noexcept { return value_; }

  /**
   * Get value [operator()]
   *
   * @param other other metadata
   *
   * @return value
   */
  inline constexpr T operator()() const noexcept { return get(); }

  /**
   * Equality check
   *
   * @param other other metadata
   *
   * @return true if value is equal with other
   */
  inline bool operator==(const base_metadata_t& other) const noexcept {
    return value_ == other.value_;
  }

  /**
   * Less than check
   *
   * @return true if value is less than with other
   */
  inline bool operator<(const base_metadata_t& other) const noexcept {
    return value_ < other.value_;
  }

  /**
   * Less than equal check
   *
   * @param other other metadata
   *
   * @return true if value is less than equal with other
   */
  inline bool operator<=(const base_metadata_t& other) const noexcept {
    return value_ <= other.value_;
  }

  /**
   * More than check
   *
   * @return true if value is more than with other
   */
  inline bool operator>(const base_metadata_t& other) const noexcept {
    return value_ > other.value_;
  }

  /**
   * More than equal check
   *
   * @param other other metadata
   *
   * @return true if value is more than equal with other
   */
  inline bool operator>=(const base_metadata_t& other) const noexcept {
    return value_ >= other.value_;
  }

  /**
   * Inequality check
   *
   * @param other other metadata
   *
   * @return true if value is not equal with other
   */
  inline bool operator!=(const base_metadata_t& other) const noexcept {
    return value_ != other.value_;
  }

  /**
   * Minus operator
   *
   * @param other other metadata
   *
   * @return new metadata
   */
  inline base_metadata_t operator-(
      const base_metadata_t& other) const noexcept {
    return base_metadata_t(get() - other());
  }

  /**
   * Plus operator
   *
   * @param other other metadata
   *
   * @return new metadata
   */
  inline base_metadata_t operator+(
      const base_metadata_t& other) const noexcept {
    return base_metadata_t(get() + other());
  }

  /**
   * Increment operator
   *
   * @param other other metadata
   *
   * @return instance of metadata
   */
  inline base_metadata_t& operator++() noexcept {
    value_++;
    return *this;
  }

  /**
   * Decrement operator
   *
   * @param other other metadata
   *
   * @return instance of metadata
   */
  inline base_metadata_t& operator--() noexcept {
    value_--;
    return *this;
  }

  /**
   * Validate value
   *
   * @param value value to test
   *
   * @return true if pass the test
   */
  inline static constexpr bool validate([[maybe_unused]] T value) noexcept {
    return true;
  }

  /**
   * Ostream operator
   *
   * @param os  ostream
   * @param obj metadata instance
   *
   * @return stream
   */
  template <typename ostream>
  inline friend ostream& operator<<(ostream& os, const base_metadata_t& obj) {
    return os << obj();
  }

 private:
  /**
   * Value
   */
  T value_;
};
}  // namespace internal

/**
 * Address type
 */
using address_t = internal::base_metadata_t<std::uint16_t>;

class num_bits_t : public internal::base_metadata_t<std::uint16_t> {
 public:
  /**
   * num_bits constructor
   *
   * @param value value to set
   *
   * @return true if pass the test
   */
  inline explicit num_bits_t(std::uint16_t value = 0) noexcept
      : internal::base_metadata_t<std::uint16_t>{value} {}

  /**
   * Validate value
   *
   * @param value value to test
   *
   * @return true if pass the test
   */
  inline static constexpr bool validate(std::uint16_t value) noexcept {
    return value > 0 && value <= constants::max_num_bits_read;
  }
};

class num_regs_t : public internal::base_metadata_t<std::uint16_t> {
 public:
  /**
   * num_regs constructor
   *
   * @param value value to set
   *
   * @return true if pass the test
   */
  inline explicit num_regs_t(std::uint16_t value = 0) noexcept
      : internal::base_metadata_t<std::uint16_t>{value} {}

  /**
   * Validate value
   *
   * @param value value to test
   *
   * @return true if pass the test
   */
  inline static constexpr bool validate(std::uint16_t value) noexcept {
    return value > 0 && value <= constants::max_num_regs_read;
  }
};
}  // namespace modbus

#endif // LIB_MODBUS_MODBUS_TYPES_HPP_
