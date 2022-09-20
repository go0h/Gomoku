#ifndef TCP_CONNECTION_HPP_
# define TCP_CONNECTION_HPP_

# define DATA_LENGTH 4096

# include <boost/asio.hpp>
# include <boost/bind.hpp>
# include "utils.hpp"
# include "Gomoku.hpp"


class GomokuGuiConnection : public std::enable_shared_from_this<GomokuGuiConnection>
{

 public:

  using pointer = std::shared_ptr<GomokuGuiConnection>;

  static pointer create(boost::asio::io_service& io_service) {
    return pointer(new GomokuGuiConnection(io_service));
  }

  ~GomokuGuiConnection() {};

  boost::asio::ip::tcp::socket& socket() {  return _socket; }

  void start();

 private:

  static unsigned _client_counter;

  GomokuGuiConnection(boost::asio::io_service& io_service);

  void _async_read();
  void _handle_read(const boost::system::error_code& error, std::size_t len);

  void _async_write(std::string message);
  void _handle_write(const boost::system::error_code& error, std::size_t len);

  void _close_connection();

  unsigned                     _id;
  boost::asio::ip::tcp::socket _socket;
  Gomoku                       _game;
  char                         _data[DATA_LENGTH];

};

#endif // TCP_CONNECTION_HPP_
