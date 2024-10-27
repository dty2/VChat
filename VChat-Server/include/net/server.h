#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <glog/logging.h>
#include "connection.h"

namespace vchat {

using boost::asio::ip::tcp;

class Server {
public:
  Server(ConnectionManager &manager, std::string port);
  void run();

private:
  std::string address;
  std::string port;
  void do_accept();
  void do_await_stop();
  boost::asio::io_context io_context;
  boost::asio::signal_set signals;
  tcp::acceptor acceptor;
  ConnectionManager &manager;

};

} //namespace vchat

#endif // !SERVER_H
