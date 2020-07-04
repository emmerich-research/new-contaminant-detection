#include <libcore/core.hpp>
#include <libnetworking/networking.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[]) {
  USE_NAMESPACE

  if (initialize_core()) {
    return 1;
  }

  try {
    if (argc != 3) {
      std::cerr << "Usage: modbus-slave <hostname> <port>\n";
      return 1;
    }

    std::shared_ptr<networking::Modbus> client =
        networking::modbus::TCP::create(argv[1], argv[2]);
    client->connect();
    client->close();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
