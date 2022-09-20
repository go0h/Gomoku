
#include <nlohmann/json.hpp>
#include "GomokuGuiConnection.hpp"
#include "gomoku_methods.hpp"


using json = nlohmann::json;
using namespace boost::asio;

unsigned GomokuGuiConnection::_client_counter = 0;

GomokuGuiConnection::GomokuGuiConnection(boost::asio::io_service& io_service) :
  _id(_client_counter++),
  _socket(io_service),
  _game(Gomoku())
{
  memset(_data, 0, DATA_LENGTH);
}

void GomokuGuiConnection::start() {
  _async_read();
}


void GomokuGuiConnection::_async_read() {
  _socket.async_receive(buffer(_data, DATA_LENGTH),
                        boost::bind(&GomokuGuiConnection::_handle_read, this,
                                    placeholders::error,
                                    placeholders::bytes_transferred));
}


void GomokuGuiConnection::_handle_read(const boost::system::error_code& error, std::size_t len) {

  if (!error) {

    _data[len] = '\0';

    #ifdef DEBUG
      std::cerr << GREEN << "Recieve " << len << " bytes from Client_" << _id << std::endl;
      std::cerr << BLUE << _data << RESET;
    #endif

    try {

      nlohmann::json json_data = json::parse(_data);

      GomokuMethod::pointer gm = ArgumentFactory::createArguments(
        json_data["method"], json_data["arguments"]
      );

      std::string response = _game.process(gm);

      if (response.length())
        _async_write(response);

    } catch (const std::exception& e) {
      std::cerr << RED << "Get exception: " << e.what() << RESET << std::endl;
    }
  } else {

    #ifdef DEBUG
      std::cerr << RED << "Recieve error from Client_" << _id << ": " \
        << error.message()  << RESET << std::endl;
    #endif

    if (error == error::eof) {
      _close_connection();
      return;
    }
  }

  _async_read();
}


void GomokuGuiConnection::_async_write(std::string message) {

  #ifdef DEBUG
    std::cerr << YELLOW << "Send to Client_" << _id << ": " << message << RESET << std::endl;
  #endif

  async_write(_socket, buffer(message),
              boost::bind(&GomokuGuiConnection::_handle_write, this,
                          placeholders::error,
                          placeholders::bytes_transferred));
}


void GomokuGuiConnection::_handle_write(const boost::system::error_code& error, std::size_t len) {
  if (error) {
    std::cerr << RED << "Error send " << len << " bytes to Client_" << _id \
      << ": " << error.message()  << RESET << std::endl;
  } else {
    #ifdef DEBUG
      std::cerr << GREEN << "Send " << len << " bytes to Client_" << _id << RESET << std::endl;
    #endif
  }
}

void GomokuGuiConnection::_close_connection() {

  #ifdef DEBUG
    std::cerr << MAGENTA << "Close connection for Client_" << _id << RESET << std::endl;
  #endif

  _socket.close();
}
