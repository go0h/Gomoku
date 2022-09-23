#include "Server.hpp"
#include "GomokuGuiConnection.hpp"

using namespace boost::asio;


Server::Server(const int port) :
  _endpoint(ip::tcp::endpoint(ip::tcp::v4(), port)),
  _acceptor(ip::tcp::acceptor(_service, _endpoint)),
  _socket(socket_ptr(new ip::tcp::socket(_service))) {
}


Server::~Server() {
  _socket->close();
}

void Server::run() {

  GomokuGuiConnection::pointer new_gui_conn = GomokuGuiConnection::create(_service);

  _acceptor.async_accept(new_gui_conn->socket(),
                         boost::bind(&Server::handle_accept, this, new_gui_conn,
                                     boost::asio::placeholders::error));
  _service.run();
}

void Server::handle_accept(GomokuGuiConnection::pointer new_connection,
                           const boost::system::error_code& error) {
  if (!error) {
    new_connection->start();
  }
  run();
}

void Server::stop() {
  _service.post(boost::bind(&Server::handle_stop, this));
}

void Server::handle_stop() {
  _acceptor.close();
  _socket->close();
}
