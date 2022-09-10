
#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <iostream>
# include <sstream>
# include <string>
# include <boost/asio.hpp>
# include <boost/thread.hpp>
# include <boost/bind.hpp>

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;


class Server {

public:

  Server(const int port);
  ~Server();

  void run();

private:

  Server(const Server&);
  Server& operator=(const Server&);

  boost::asio::io_service           _service;
  boost::asio::ip::tcp::endpoint   _endpoint;
  boost::asio::ip::tcp::acceptor   _acceptor;
  socket_ptr                       _socket;

};

#endif // SERVER_HPP
