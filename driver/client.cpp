#include <cstdlib>
#include <cstring>
#include <iostream>

#include <boost/asio.hpp>

#include <libcore/core.hpp>
#include <libmodbus/modbus.hpp>
#include <libutil/util.hpp>

using boost::asio::ip::tcp;

static void cout_bytes(const modbus::internal::packet_t& packet) {
  modbus::internal::packet_t::size_type index = 0;

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

int main(int argc, char* argv[]) {
  USE_NAMESPACE
  try {
    if (argc != 3) {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }


    if (initialize_core()) {
      std::cerr << "cannot initialize config, state, and logger!" << std::endl;
      return ATM_ERR;
    }

    boost::asio::io_context io_context;

    tcp::socket   s(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

    std::cout << "Enter message: ";
    modbus::request::read_coils req_read_coils{modbus::address_t{0x00},
                                               modbus::num_bits_t{1}};

    req_read_coils.initialize({0x1234, 0x01});

    auto request = req_read_coils.encode();
    cout_bytes(request);

    size_t request_length = request.size();
    boost::asio::write(s, boost::asio::buffer(request, request_length));

    modbus::internal::packet_t response;
    size_t                     reply_length =
        boost::asio::read(s, boost::asio::buffer(response, request_length));

    cout_bytes(response);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
