#include <http_server.hpp>
#include "suggestion.hpp"

int main(int argc, char* argv[]) {
  try
  {
    if(argc != 3) {
      std::cerr << "Usage: " << argv[0] << " <address> <port>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80\n";
      return EXIT_FAILURE;
    }

    auto const address = net::ip::make_address(argv[1]);
    unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));

    net::io_context ioc{1};

    suggestion sug = suggestion("/home/alesandr/CLionProjects/lab7-8/"
        "lab-07-http-server/suggestions.json");

    std::thread sug_thread([&sug]{
      sug.update_suggestion();
    });
    sug_thread.detach();

    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};
    http_server(acceptor, socket, sug);

    ioc.run();
  } catch(std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
