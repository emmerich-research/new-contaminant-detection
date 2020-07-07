#include <cstdlib>
#include <type_traits>

#include <libcore/core.hpp>
#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

const unsigned int SLAVE_ID = 0x01;

const std::uint16_t UT_BITS_ADDRESS = 0x130;
// const std::uint16_t UT_BITS_NB = 0x25;
// const std::uint8_t  UT_BITS_TAB[] = {0xCD, 0x6B, 0xB2, 0x0E, 0x1B};

// const std::uint16_t UT_INPUT_BITS_ADDRESS = 0x1C4;
// const std::uint16_t UT_INPUT_BITS_NB = 0x16;
// const std::uint8_t  UT_INPUT_BITS_TAB[] = {0xAC, 0xDB, 0x35};

// const std::uint16_t UT_REGISTERS_ADDRESS = 0x160;
// const std::uint16_t UT_REGISTERS_NB = 0x3;
// const std::uint16_t UT_REGISTERS_NB_MAX = 0x20;
// const std::uint16_t UT_REGISTERS_TAB[] = {0x022B, 0x0001, 0x0064};

int main(int argc, const char* argv[]) {
  USE_NAMESPACE

  if (initialize_core()) {
    return 1;
  }

  if (argc != 3) {
    std::cerr << "Usage: modbus-slave <hostname> <port>\n";
    return 1;
  }

  networking::Modbus::ErrorCode ec;

  std::shared_ptr<networking::Modbus> client =
      networking::modbus::TCP::create(argv[1], argv[2]);
  client->slave_id(SLAVE_ID);
  client->connect_timeout(networking::modbus::time::seconds(10));
  client->request_timeout(networking::modbus::time::seconds(10));
  client->response_timeout(networking::modbus::time::seconds(10));

  client->callback(
      [](const networking::ModbusResponse& response) {
        std::visit(
            [&](auto&& data) {
              using T = std::decay_t<decltype(data)>;
              if constexpr (std::is_same_v<T, networking::Modbus::Buffer8>) {
                LOG_INFO("Length={}, NB={}, Buffer8 Data={}", response.length,
                         response.num_of_bytes, data);
              } else if constexpr (std::is_same_v<
                                       T, networking::Modbus::Buffer16>) {
                LOG_INFO("Length={}, NB={}, Buffer16 Data={}", response.length,
                         response.num_of_bytes, data);
              } else if constexpr (std::is_same_v<T, std::monostate>) {
                // noop
                // LOG_INFO("Test");
              }
            },
            response.data);

        // std::visit(util::overloaded{[](networking::Modbus::Buffer8&& data) {
        //                               LOG_INFO("Buffer8 Data {}", data);
        //                             },
        //                             [](networking::Modbus::Buffer16&& data) {
        //                               LOG_INFO("Buffer16 Data {}", data);
        //                             },
        //                             [](std::monostate&& data) {

        //                             }},
        //            response.data);
      },
      [](const networking::ModbusError& error) {
        LOG_INFO("{}", error.error);
      });

  ec = client->connect();

  if (ec) {
    LOG_ERROR("Failed to connect to server, exiting...");
    return -1;
  }

  networking::Modbus::Buffer8  buffer_8;
  networking::Modbus::Buffer16 buffer_16;

  client->write_bit(UT_BITS_ADDRESS, true);

  // if (networking::Modbus::error(response)) {
  //   LOG_INFO("");
  // }

  client->read_bits(UT_BITS_ADDRESS, 1, buffer_8);

  client->read_input_bits(0x1C4, 0x16, buffer_8);

  client->read_registers(0x160, 0x1, buffer_16);
  client->read_registers(0x160, 0x3, buffer_16);

  client->read_input_registers(0x108, 0x1, buffer_16);

  ec = client->close();

  if (ec) {
    LOG_ERROR("Something wrong while closing the connection");
    return -1;
  }

  LOG_INFO("Closing connection succeed...");

  return 0;
}
