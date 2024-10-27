#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <set>
#include <boost/asio.hpp>
#include <glog/logging.h>
#include "vchat.pb.h"

using boost::asio::ip::tcp;

namespace vchat {

// enpack
template <typename body>
std::string enpack(body target);
// depack
template <typename body>
body depackbody(std::string target);

class Service;
class Connection;
typedef std::shared_ptr<Connection> Connection_ptr;

class ConnectionManager {
public:
  ConnectionManager();
  void add(Connection_ptr cp);
  void stop(Connection_ptr cp);
  void stop_all();

private:
  std::set<Connection_ptr> connections;
  friend Connection;
};

class Connection : public std::enable_shared_from_this<Connection> {
public:
  Connection(tcp::socket, ConnectionManager*);
  ~Connection();
  void start();
  void stop();
  void write(int, std::string);

private:
  ConnectionManager* manager;
  tcp::socket socket;
  std::string head, body;
  vchat::Head headinfo;
  void readhead();
  void readbody(int packagetype, int servicetype, int bodysize);
};

} //namespace vchat

#endif //CONNECTION_H
