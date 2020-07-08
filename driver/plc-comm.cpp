#include <cstdlib>
#include <type_traits>

#include <libcore/core.hpp>
#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

const std::uint8_t SLAVE_ID = 0x01;

const std::uint16_t YEAR_ADDRESS = 30001;
// const std::uint16_t MONTH_ADDRESS = 30002;
// const std::uint16_t DAY_ADDRESS = 30003;
// const std::uint16_t HOUR_ADDRESS = 30004;
// const std::uint16_t MINUTE_ADDRESS = 30005;
const std::uint16_t SECOND_ADDRESS = 30006;

int main(int argc, const char* argv[]) {
  USE_NAMESPACE

  if (initialize_core()) {
    return 1;
  }

  if (argc != 3) {
    std::cerr << "Usage: plc-comm <hostname> <port>\n";
    return 1;
  }

  networking::Modbus::ErrorCode ec;
  // networking::Modbus::Buffer8   buffer_8;
  networking::Modbus::Buffer16 buffer_16;

  std::shared_ptr<networking::Modbus> client =
      networking::modbus::TCP::create(argv[1], argv[2]);
  client->slave_id(SLAVE_ID);
  client->connect_timeout(networking::modbus::time::seconds(10));
  client->request_timeout(networking::modbus::time::seconds(10));
  client->response_timeout(networking::modbus::time::seconds(10));

  {
    ec = client->connect();

    if (ec) {
      LOG_ERROR("Failed to connect to server, exiting...");
      return -1;
    }
  }

  {
    // read date and time
    const auto&& read_date = client->read_input_registers(
        YEAR_ADDRESS, SECOND_ADDRESS - YEAR_ADDRESS, buffer_16);

    if (networking::Modbus::error(read_date)) {
      LOG_ERROR("FAILED");
      return -1;
    }

    LOG_INFO("DateTime: {}/{}/{} @ {}:{}:{}", buffer_16[2], buffer_16[1],
             buffer_16[0], buffer_16[3], buffer_16[4], buffer_16[5]);
  }

  client->close();

  return 0;
}
