#include "utils.hpp"
#include "exceptions.hpp"
#include "Server.hpp"


static void validate_arguments(int argc, char* argv[]) {

  const int number_of_args = 2;
  const char* digits = "0123456789";
  const int max_port_length = 5;

  throw_if_true<InvalidArgument>(argc != number_of_args);

  std::string port(argv[1]);
  throw_if_true<InvalidArgument>(port.find_first_not_of(digits) != std::string::npos);
  throw_if_true<InvalidArgument>(port.length() > max_port_length);
  throw_if_true<InvalidArgument>(atoi(port.c_str()) > USHRT_MAX);
}


int main(int argc, char* argv[]) {
  try {
    validate_arguments(argc, argv);

    const int port = atoi(argv[1]);
    Server server(port);
    server.run();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
