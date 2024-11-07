#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <set>
#include <functional>
#include <boost/asio.hpp>
#include <glog/logging.h>
#include "vchat.pb.h"


using boost::asio::ip::tcp;
using namespace vprotocol;

namespace vchat {

const int HeadSize = 16;

class Service;
class Connection;
typedef std::shared_ptr<Connection> Connection_ptr;

class ConnectionManager {
public:
  void add(tcp::socket socket);
  void stop(Connection_ptr cp);
  void stop_all();

private:
  std::set<Connection_ptr> connections;
  friend Connection;
};

class Connection : public std::enable_shared_from_this<Connection> {
public:
  Connection(tcp::socket, ConnectionManager*);
  void start();
  void stop();
  template<typename package>
  void write(Head head, package body, std::function<void()> callback = nullptr);
  static Head enpackHead(int packagetype, int servicetype, int size);

private:
  ConnectionManager* manager;
  tcp::socket socket;
  std::string head, body;
  void readhead();
  void readbody(int packagetype, int servicetype, int bodysize);

};

} //namespace vchat

#endif //CONNECTION_H
