
#include <nlohmann/json.hpp>
#include "GomokuGuiConnection.hpp"
#include "Arguments.hpp"


using json = nlohmann::json;
using namespace boost::asio;


void GomokuGuiConnection::start() {
  _async_read();
}


void GomokuGuiConnection::_async_read() {
  _socket.async_receive(buffer(_data, 4096),
                        boost::bind(&GomokuGuiConnection::_handle_read, this,
                                    placeholders::error,
                                    placeholders::bytes_transferred));
}


void GomokuGuiConnection::_handle_read(const boost::system::error_code& error, std::size_t len) {

  if (!error) {
    _data[len] = '\0';

    #ifdef DEBUG
      std::cout << GREEN << "Recieve from client " << \
        &_socket << " (" << len << ") bytes: " << _data << RESET;
    #endif

    auto json_data = json::parse(_data);

    std::string method = json_data["method"];
    auto arguments = json_data["arguments"];

    if (method == "start_game") {
      _game = Gomoku(arguments["mode"],
                     arguments["player_color"],
                     arguments["difficult"],
                     arguments["board_size"]);
    } else {

      Arguments::pointer args = ArgumentFactory::createArgPtr(json_data);

      std::string response = _game.exec_method(args);
      std::cout << "Responce: " << GREEN << response << RESET << std::endl;

      if (response.length())
        _async_write(response);
    }
  }

  else {
    #ifdef DEBUG
      std::cerr << RED << "Recieve error: " << error.message()  << RESET << std::endl;
    #endif

    if (error == error::eof) {
      _socket.close();
      return;
    }
  }

  _async_read();
}


void GomokuGuiConnection::_async_write(std::string message) {

  async_write(_socket, buffer(message),
              boost::bind(&GomokuGuiConnection::_handle_write, this,
                          placeholders::error,
                          placeholders::bytes_transferred));
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
