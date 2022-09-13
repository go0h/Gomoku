
#include <nlohmann/json.hpp>
#include "GomokuGuiConnection.hpp"
#include "gomoku_methods.hpp"


using json = nlohmann::json;
using namespace boost::asio;

unsigned GomokuGuiConnection::_client_counter = 0;

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
      std::cerr << GREEN << "Recieve " << len << " bytes from Client_" << _id << std::endl;
      std::cerr << BLUE << _data << RESET;
    #endif

    try {

      nlohmann::json json_data = json::parse(_data);

      std::string method = json_data["method"];
      nlohmann::json args = json_data["arguments"];

      if (method == "start_game") {
        _game = Gomoku(args["mode"], args["player_color"], args["difficult"], args["board_size"]);
      } else {

        GomokuMethod::pointer gm = ArgumentFactory::createArguments(method, args);

        std::string response = _game.exec_method(gm);

        if (response.length())
          _async_write(response);
      }
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
