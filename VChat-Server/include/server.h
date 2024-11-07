#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <glog/logging.h>
#include "connection.h"
#include "singleton_crtp.h"

namespace vchat {

using boost::asio::ip::tcp;

class Server : public SingletonCRTP<Server>{
public:
  void run();

private:
  std::string address, port;
  boost::asio::io_context io_context;
  boost::asio::signal_set signals;
  tcp::acceptor acceptor;
  ConnectionManager manager;
  
  Server(std::string port);
  void do_accept();
  void do_await_stop();

};

} //namespace vchat

#endif // !SERVER_H
