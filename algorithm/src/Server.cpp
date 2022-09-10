#include <Server.hpp>

using namespace boost::asio;


void handle_accept(socket_ptr sock, const boost::system::error_code& err)
{
    if (err)
      return;
    try {
      while (true) {
        char data[512];
        size_t len = sock->receive(buffer(data));
        data[len] = '\0';
        std::cout << "receive from " << sock << " " << data;
        if (len > 0)
          sock->write_some(buffer("ok\n", 3));
      }
    } catch (boost::exception& e) {
      std::cerr << "ERROR" << std::endl;
    }
}

Server::Server(const int port) :
  _endpoint(ip::tcp::endpoint(ip::tcp::v4(), port)),
  _acceptor(ip::tcp::acceptor(_service, _endpoint)),
  _socket(socket_ptr(new ip::tcp::socket(_service))) {
}


Server::~Server() {
  _socket->close();
}

void Server::run() {
  _acceptor.async_accept(*_socket, boost::bind(handle_accept, _socket, _1));
  _service.run();
}
