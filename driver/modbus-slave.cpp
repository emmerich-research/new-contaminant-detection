#include <cstdlib>
#include <type_traits>

#include <libcore/core.hpp>
#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

const unsigned int SLAVE_ID = 0x01;

const std::uint16_t BITS_ADDRESS = 0x00;
const std::uint16_t REGISTER_ADDRESS = 0x00;

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
                LOG_INFO("Length={}, NB={}", response.length,
                         response.num_of_bytes);
                // LOG_INFO("Length={}, NB={}, Buffer8 Data={}",
                // response.length,
                //          response.num_of_bytes, data);
              } else if constexpr (std::is_same_v<
                                       T, networking::Modbus::Buffer16>) {
                LOG_INFO("Length={}, NB={}", response.length,
                         response.num_of_bytes);
                // LOG_INFO("Length={}, NB={}, Buffer16 Data={}",
                // response.length,
                //          response.num_of_bytes, data);
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

  {
    client->write_bit(BITS_ADDRESS, true);
    const auto&& read_bits_response =
        client->read_bits(BITS_ADDRESS, 1, buffer_8);

    if (networking::Modbus::error(read_bits_response)) {
      LOG_ERROR("FAILED");
      return -1;
    }

    massert(buffer_8[0] == 1, "check");
  }

  {
    client->write_register(REGISTER_ADDRESS, 12);

    const auto&& read_registers_response =
        client->read_registers(REGISTER_ADDRESS, 0x1, buffer_16);

    if (networking::Modbus::error(read_registers_response)) {
      LOG_ERROR("FAILED");
      return -1;
    }

    massert(buffer_16[REGISTER_ADDRESS] == 12, "check");
  }

  {
    const std::uint8_t buff[] = {1, 1, 1, 1, 1};
    client->write_bits(BITS_ADDRESS + 0x10, 5, buff);

    const auto&& read_bits_mult_response =
        client->read_bits(BITS_ADDRESS + 0x10, 5, buffer_8);

    if (networking::Modbus::error(read_bits_mult_response)) {
      LOG_ERROR("FAILED");
      return -1;
    }

    massert(buffer_8[3] == 1, "check");
    massert(buffer_8[4] == 1, "check");
  }

  {
    const std::uint16_t buff[] = {1, 2, 3, 4, 5};
    client->write_registers(REGISTER_ADDRESS + 0x10, 5, buff);

    const auto&& read_register_mult_response =
        client->read_registers(REGISTER_ADDRESS + 0x10, 5, buffer_16);

    if (networking::Modbus::error(read_register_mult_response)) {
      LOG_ERROR("FAILED");
      return -1;
    }

    massert(buffer_16[0] == 1, "check");
    massert(buffer_16[4] == 5, "check");
  }

  // {
  //   const auto&& read_input_bits_response =
  //       client->read_input_bits(BITS_ADDRESS, 5, buffer_8);

  //   if (networking::Modbus::error(read_input_bits_response)) {
  //     LOG_ERROR("FAILED");
  //     return -1;
  //   }

  //   massert(buffer_8[0] == 0, "check");
  //   massert(buffer_8[4] == 1, "check");
  // }

  // {
  // client->read_input_registers(REGISTER_ADDRESS, 5, buffer_16);

  // const auto&& read_register_mult_response =
  //     client->read_input_registers(REGISTER_ADDRESS, 5, buffer_16);

  // if (networking::Modbus::error(read_register_mult_response)) {
  //   LOG_ERROR("FAILED");
  //   return -1;
  // }

  // massert(buffer_16[0] == 5, "check");
  // massert(buffer_16[4] == 1, "check");
  // }

  ec = client->close();

  if (ec) {
    LOG_ERROR("Something wrong while closing the connection");
    return -1;
  }

  LOG_INFO("Closing connection succeed...");

  return 0;
}
