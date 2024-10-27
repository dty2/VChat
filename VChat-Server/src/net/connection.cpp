#include "connection.h"
#include "boost/asio/read.hpp"
#include "vchat.pb.h"

namespace vchat {

template <typename body>
std::string enpack(vchat::Head, body target) {
}

template <typename body>
body depackbody(std::string target) {
}

ConnectionManager::ConnectionManager()
{}

void ConnectionManager::add(Connection_ptr cp) {
  LOG(INFO) << "add new connection";
  connections.insert(cp);
  cp->start();
}

void ConnectionManager::stop(Connection_ptr cp) {
  LOG(INFO) << "stop a connection";
  connections.erase(cp);
  cp->stop();
}

void ConnectionManager::stop_all() {
  LOG(INFO) << "stop all connection";
  for (auto &v : connections)
    v->stop();
  connections.clear();
}

Connection::Connection(tcp::socket socket_, ConnectionManager *manager_)
  : socket(std::move(socket_)), manager(manager_)
{ }

Connection::~Connection() {
}

void Connection::start() {
  LOG(INFO) << "Connection start work";
  readhead();
}

void Connection::stop() {
  socket.close();
}

void Connection::readhead() {
  async_read_until(
    socket, head, "\n",
    [&](boost::system::error_code ec, std::size_t bytes_transferred) {
      if (!ec) {
        LOG(INFO) << "connetion readhead successful";
        vchat::Head headinfo;
        headinfo.ParseFromString(head);
        if(headinfo.version() == 1) {
          readbody(headinfo.packagetype(), headinfo.servicetype(), headinfo.bodysize());
        } else {
          LOG(INFO) << "protocol version fault";
        }
      } else {
        this->stop();
        manager->connections.erase(shared_from_this());
      }
    }
  );
}

void Connection::readbody(int packagetype, int servicetype, int bodysize) {
  read(socket, boost::asio::buffer(body, bodysize));
}

void Connection::write(int status, std::string target) {
  async_write(
    socket, boost::asio::buffer(target, target.size()),
    [&](boost::system::error_code ec, std::size_t bytes_transferred) {
      if (!ec) {
        LOG(INFO) << "Connetion write successful";
      } else {
        this->stop();
        manager->connections.erase(shared_from_this());
      }
    }
  );
}

} //namespace vchat
