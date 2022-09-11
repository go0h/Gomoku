
#include "GomokuGuiConnection.hpp"


using namespace boost::asio;

void GomokuGuiConnection::start() {
  _async_read();
}

void GomokuGuiConnection::_async_read() {
  _socket.async_receive(boost::asio::buffer(_data, 4096),
                        boost::bind(&GomokuGuiConnection::_handle_read, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void GomokuGuiConnection::_handle_read(const boost::system::error_code& error, std::size_t len) {

  if (!error) {
    _data[len] = '\0';

    #ifdef DEBUG
      std::cout << GREEN << "Recieve from client " << \
        &_socket << " (" << len << ") bytes: " << _data << RESET;
    #endif

    // std::string message = "hello from server\n";
    // boost::asio::async_write(_socket, boost::asio::buffer(message),
    //                         boost::bind(&GomokuGuiConnection::_handle_write, this,
    //                                     boost::asio::placeholders::error,
    //                                     boost::asio::placeholders::bytes_transferred));
  }

  else {
    #ifdef DEBUG
      std::cerr << RED << "Recieve error: " << error.message()  << RESET << std::endl;
    #endif

    if (error == boost::asio::error::eof) {
      _socket.close();
      return;
    }
  }

  _async_read();
}

void GomokuGuiConnection::_handle_write(const boost::system::error_code& error, std::size_t len) {
  if (error) {
    len *= 0;
  } else {
    #ifdef DEBUG
      std::cout << "Send " << len << " bytes to " << &_socket << std::endl;
    #endif
  }
}
