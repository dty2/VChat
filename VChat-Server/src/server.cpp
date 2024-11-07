#include "server.h"

namespace vchat {

Server::Server(std::string port)
  : address("127.0.0.1"),
    port(port),
    io_context(1),
    signals(io_context),
    acceptor(io_context)
{
  // signals
  signals.add(SIGINT);
  signals.add(SIGTERM);
  signals.add(SIGQUIT);

  boost::asio::ip::tcp::resolver resolver(io_context);
  boost::asio::ip::tcp::endpoint endpoint =
    *resolver.resolve(address, port).begin();
  acceptor.open(endpoint.protocol());
  acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor.bind(endpoint);
  acceptor.listen();
  do_await_stop();
  do_accept();
  LOG(INFO) << "Do accept on " << port << " port...";
}

void Server::run() {
  io_context.run();
}

void Server::do_accept() {
  acceptor.async_accept([&](boost::system::error_code ec, tcp::socket socket) {
    if (!ec) {
      LOG(INFO) << "A new client connection";
      manager.add(std::move(socket));
    } else {
      LOG(INFO) << "Connection refused";
      LOG(INFO) << ec.what();
    }
    do_accept();
  });
}

void Server::do_await_stop() {
  signals.async_wait([&](boost::system::error_code ec, int sign) {
    if (!ec) {
      LOG(INFO) << "Start close";
      LOG(INFO) << "Close acceptor";
      acceptor.close();
      LOG(INFO) << "Close connection manager";
      manager.stop_all();
      LOG(INFO) << "Close io_context";
      io_context.stop();
      LOG(INFO) << "Close finish, server exit";
    } else {
      LOG(INFO) << "Close Error";
      LOG(INFO) << ec.what();
    }
  });
}

} //namespace vchat
