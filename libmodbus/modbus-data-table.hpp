#ifndef LIB_MODBUS_MODBUS_DATA_TABLE_HPP_
#define LIB_MODBUS_MODBUS_DATA_TABLE_HPP_

#include <cstdlib>
#include <type_traits>
#include <utility>
#include <vector>

#include "modbus-types.hpp"

namespace modbus {
// forward declarations
namespace block {
template <template <class...> class container_t,
          typename data_t,
          typename count_t>
class base;

template <typename data_t, typename count_t>
class sequential;
}  // namespace block
class table;

namespace block {
/**
 * @brief base block class
 *
 * @author   Ray Andrew
 * @ingroup  Modbus
 * @date     August 2020
 *
 * @tparam base_container_t base container type
 * @tparam data_t           data type
 */
template <template <class...> class base_container_t,
          typename data_t,
          typename count_t>
class base {
  static_assert(
      std::is_base_of_v<internal::base_metadata_t<std::uint16_t>, count_t>,
      "count_t must extends internal::base_metadata_t");

 public:
  /**
   * Data type
   */
  typedef data_t data_type;

  /**
   * Container type
   */
  typedef base_container_t<data_type> container_type;

  /**
   * Iterator type
   */
  typedef typename container_type::const_iterator iterator_type;

  /**
   * Slice type
   *
   * Containing begin and end that denotes slice of container
   */
  typedef std::pair<iterator_type, iterator_type> slice_type;

  /**
   * Size type
   */
  typedef typename container_type::size_type size_type;

  /**
   * Max capacity
   */
  static constexpr size_type max_capacity = 65535;

  /**
   * Base constructor
   *
   * @param starting_address starting address of container
   * @param default_value    default value of container
   */
  inline explicit constexpr base(const address_t& starting_address,
                                 size_type        capacity = max_capacity,
                                 data_type        default_value = 0) noexcept;

  /**
   * Base constructor
   *
   * @param starting_address starting address of container
   * @param container        container initializer
   */
  inline explicit constexpr base(const address_t&      starting_address,
                                 const container_type& container) noexcept;

  /**
   * Get slice of data from container
   *
   * @param address look-up address
   * @param count   number of slice
   *
   * @return pair of iterator (begin and end) slice of data from container
   */
  virtual constexpr slice_type get(const address_t& address,
                                   const count_t&   count) const = 0;

  /**
   * Set slice of data from container
   *
   * @param address     look-up address
   * @param container   container to add
   */
  virtual constexpr void set(const address_t&      address,
                             const container_type& buffer) = 0;

  /**
   * Set single data to container at specific address
   *
   * @param address     look-up address
   * @param value       value to add
   */
  virtual constexpr void set(const address_t& address, data_t value) = 0;

  /**
   * Reset container
   */
  virtual constexpr void reset() = 0;

  /**
   * Validate with count_t
   *
   * @param address look-up address
   * @param count   number of slice
   */
  inline virtual constexpr bool validate(const address_t& address,
                                         const count_t&   count = count_t{
                                             1}) const;

  /**
   * Validate size type
   *
   * @param address look-up address
   * @param count   number of slice
   */
  inline virtual constexpr bool validate_sz(const address_t& address,
                                            size_type        count = 1) const;

  /**
   * Get starting address
   *
   * @return starting address
   */
  inline constexpr const address_t& starting_address() const {
    return starting_address_;
  }

  /**
   * Get container
   *
   * @return container
   */
  inline constexpr container_type& container() { return container_; }

  /**
   * Get container (const)
   *
   * @return container (const)
   */
  inline constexpr const container_type& container() const {
    return container_;
  }

  /**
   * Get default value
   *
   * @return default value
   */
  inline constexpr const data_t& default_value() const {
    return default_value_;
  }

  /**
   * Get capacity
   *
   * @return capacity
   */
  inline virtual constexpr size_type capacity() const { return capacity_; }

  /**
   * Ostream operator
   *
   * @param os  ostream
   * @param obj base instance
   *
   * @return stream
   */
  template <typename ostream>
  inline friend ostream& operator<<(ostream& os, const base& obj) {
    return os << "(DataTable, starting_address=" << obj.starting_address()
              << ", capacity=" << obj.capacity()
              << ", default_value=" << obj.default_value();
  }

 protected:
  /**
   * Starting address of container
   */
  const address_t starting_address_;
  /**
   * Container
   */
  container_type container_;
  /**
   * Capacity
   */
  const size_type capacity_;
  /**
   * Default value
   */
  const data_t default_value_;
};

/**
 * @brief sequential block class
 *
 * @author   Ray Andrew
 * @ingroup  Modbus
 * @date     August 2020
 *
 * @tparam data_t      data type
 */
template <typename data_t, typename count_t>
class sequential : public base<std::vector, data_t, count_t> {
 public:
  /**
   * Data type
   */
  using typename base<std::vector, data_t, count_t>::data_type;

  /**
   * Container type
   */
  using typename base<std::vector, data_t, count_t>::container_type;

  /**
   * Iterator type
   */
  using typename base<std::vector, data_t, count_t>::iterator_type;

  /**
   * Slice type
   *
   * Containing begin and end that denotes slice of container
   */
  using typename base<std::vector, data_t, count_t>::slice_type;

  /**
   * Size type
   */
  using typename base<std::vector, data_t, count_t>::size_type;

  /**
   * Container max capacity
   */
  using base<std::vector, data_t, count_t>::max_capacity;

  /**
   * Initializer
   */
  struct initializer_t {
    /**
     * Starting address
     */
    address_t starting_address{0x00};
    /**
     * Capacity
     */
    size_type capacity = max_capacity;
    /**
     * Default value
     */
    data_type default_value = 0;
  };

  /**
   * Sequential block constructor
   *
   * @param starting_address starting address of container
   * @param capacity         max capacity of container
   * @param default_value    default value of container
   */
  inline explicit constexpr sequential(const address_t& starting_address,
                                       size_type        capacity = max_capacity,
                                       data_t default_value = 0) noexcept;

  /**
   * Sequential block constructor
   *
   * @param initializer sequential block initializer
   */
  inline explicit constexpr sequential(
      const initializer_t& initializer) noexcept;

  /**
   * Sequential block constructor
   *
   * @param starting_address starting address of container
   * @param container        container initializer
   */
  inline explicit constexpr sequential(
      const address_t&      starting_address,
      const container_type& container) noexcept;

  /**
   * Get slice of data from container
   *
   * @param address starting address
   * @param count   number of slice
   *
   * @return pair of iterator (begin and end) slice of data from container
   */

  inline virtual constexpr slice_type get(const address_t& address,
                                          const count_t& count) const override;

  /**
   * Set slice of data from container
   *
   * @param address     starting address
   * @param container   container to add
   */
  inline virtual constexpr void set(const address_t&      address,
                                    const container_type& container) override;

  /**
   * Set single data to container at specific address
   *
   * @param address     starting address
   * @param value       value to add
   */
  inline virtual constexpr void set(const address_t& address,
                                    data_t           value) override;

  /**
   * Reset container
   */
  inline virtual constexpr void reset() override;

  /**
   * Starting address getter
   */
  using base<std::vector, data_t, count_t>::starting_address;

  /**
   * Container getter
   */
  using base<std::vector, data_t, count_t>::container;

  /**
   * Capacity getter
   */
  using base<std::vector, data_t, count_t>::capacity;

  /**
   * Default value getter
   */
  using base<std::vector, data_t, count_t>::default_value;

  /**
   * Validation with count_t
   */
  using base<std::vector, data_t, count_t>::validate;

  /**
   * Validation with size_type
   */
  using base<std::vector, data_t, count_t>::validate_sz;

  /**
   * Get capacity
   *
   * @return capacity
   */
  inline virtual constexpr
      typename base<std::vector, data_t, count_t>::size_type
      capacity() const override {
    return capacity_;
  }

  /**
   * Ostream operator
   *
   * @param os  ostream
   * @param obj sequential instance
   *
   * @return stream
   */
  template <typename ostream>
  inline friend ostream& operator<<(ostream& os, const sequential& obj) {
    return os << static_cast<const base<std::vector, data_t, count_t>&>(obj)
              << ", type=sequential"
              << ")";
  }

 protected:
  /**
   * Container
   */
  using base<std::vector, data_t, count_t>::container_;
  /**
   * Capacity
   */
  using base<std::vector, data_t, count_t>::capacity_;
};

/**
 * Bit blocks
 */
using bits = sequential<bool, num_bits_t>;

/**
 * Register blocks
 */
using registers = sequential<std::uint16_t, num_regs_t>;
}

class table {
 public:
  /**
   * Table initializer
   */
  struct initializer_t {
    /**
     * Coils initializer
     */
    block::bits::initializer_t      coils;
    /**
     * Discrete inputs initializer
     */
    block::bits::initializer_t      discrete_inputs;
    /**
     * Holding registers initializer
     */
    block::registers::initializer_t holding_registers;
    /**
     * Input registers initializer
     */
    block::registers::initializer_t input_registers;
  };

  /**
   * Table constructor
   *
   * @param initializer initializer factory
   */
  explicit table(const initializer_t& initializer = {}) noexcept;

  /**
   * Get coils block
   *
   * @return coils block
   */
  inline block::bits& coils() { return coils_; }

  /**
   * Get coils block (const)
   *
   * @return coils block (const)
   */
  inline const block::bits& coils() const { return coils_; }

  /**
   * Get discrete inputs block
   *
   * @return discrete inputs block
   */
  inline block::bits& discrete_inputs() { return discrete_inputs_; }

  /**
   * Get discrete inputs (const)
   *
   * @return discrete inputs (const)
   */
  inline const block::bits& discrete_inputs() const { return discrete_inputs_; }

  /**
   * Get holding registers block
   *
   * @return holding registers block
   */
  inline block::registers& holding_registers() { return holding_registers_; }

  /**
   * Get holding registers (const)
   *
   * @return holding registers (const)
   */
  inline const block::registers& holding_registers() const {
    return holding_registers_;
  }

  /**
   * Get input registers block
   *
   * @return input registers block
   */
  inline block::registers& input_registers() { return input_registers_; }

  /**
   * Get input registers (const)
   *
   * @return input registers (const)
   */
  inline const block::registers& input_registers() const {
    return input_registers_;
  }

 private:
  /**
   * Coils
   */
  block::bits coils_;
  /**
   * Discrete inputs
   */
  block::bits discrete_inputs_;
  /**
   * Holding register
   */
  block::registers holding_registers_;
  /**
   * Input register
   */
  block::registers input_registers_;
};
}

#endif // LIB_MODBUS_MODBUS_DATA_TABLE_HPP_



