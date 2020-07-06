#include <libcore/core.hpp>
#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[]) {
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
  client->connect_timeout(networking::modbus::time::seconds(2));
  client->request_timeout(networking::modbus::time::seconds(2));
  client->response_timeout(networking::modbus::time::seconds(2));

  client->callback(
      [](const networking::ModbusResponse& response) {
        LOG_INFO("{}", response.function);
      },
      [](const networking::ModbusError& error) {
        LOG_INFO("{}", error.error);
      });

  ec = client->connect();

  if (ec) {
    LOG_ERROR("Failed to connect to server, exiting...");
    return -1;
  }

  std::uint8_t buffer[20];
  client->read_bits(12, 5, buffer);
  ec = client->close();

  if (ec) {
    LOG_ERROR("Something wrong while closing the connection");
    return -1;
  }

  LOG_INFO("Closing connection succeed...");

  return 0;
}
