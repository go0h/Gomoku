#ifndef TCP_CONNECTION_HPP_
# define TCP_CONNECTION_HPP_

# include "utils.hpp"
# include "Gomoku.hpp"


class GomokuGuiConnection : public boost::enable_shared_from_this<GomokuGuiConnection>
{

 public:

  typedef boost::shared_ptr<GomokuGuiConnection> pointer;

  static pointer create(boost::asio::io_service& io_service) {
    return pointer(new GomokuGuiConnection(io_service));
  }

  boost::asio::ip::tcp::socket& socket() {  return _socket; }

  void start();

 private:

  GomokuGuiConnection(boost::asio::io_service& io_service)
    : _socket(io_service) {}


  void _async_read();
  void _handle_read(const boost::system::error_code& error, std::size_t len);

  void _async_write(std::string message);
  void _handle_write(const boost::system::error_code& error, std::size_t len);


  boost::asio::ip::tcp::socket _socket;
  char                         _data[4096];
  Gomoku                       _game;

};

#endif // TCP_CONNECTION_HPP_
