#include "modbus.hpp"

#include "modbus-data-table.hpp"

namespace modbus {
table::table() noexcept
    : coils_{address_t{0x00}},
      discrete_inputs_{address_t{0x00}},
      holding_registers_{address_t{0x00}},
      input_registers_{address_t{0x00}} {}

table::table(const table::initializer_t& initializer) noexcept
    : coils_{initializer.coils},
      discrete_inputs_{initializer.discrete_inputs},
      holding_registers_{initializer.holding_registers},
      input_registers_{initializer.input_registers} {}
}
