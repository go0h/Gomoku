#include "gomoku.hpp"
#include "Server.hpp"



int main(void) {

  Server server = Server(8080);

  server.run();
  return 0;
}
