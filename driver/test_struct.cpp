#include <cstdlib>
#include <iostream>
#include <vector>

#include <libcore/core.hpp>
#include <libmodbus/modbus.hpp>

static void cout_bytes(const std::vector<char>& packet) {
  std::vector<char>::size_type index = 0;

  std::string s = "[Packet, ";

  for (unsigned byte : packet) {
    index++;
    if (index < packet.size()) {
      s += fmt::format("{:#04x} ", byte);
    } else {
      s += fmt::format("{:#04x}", byte);
    }
  }

  s += "]";

  LOG_DEBUG("{}", s);
}

int main() {
  USE_NAMESPACE

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  auto data = struc::pack("<HH", 0x1234, 0x01);

  cout_bytes(data);

  std::uint16_t h1, h2;
  struc::unpack("<HH", &data[0], h1, h2);
  std::cout << h1 << std::endl;
  std::cout << h2 << std::endl;
  return 0;
}
