#include "connection.h"
#include "service.h"
#include "vchat.pb.h"

namespace vchat {

extern std::shared_ptr<Service> service;

void ConnectionManager::add(tcp::socket socket) {
  LOG(INFO) << "add new connection";
  Connection_ptr cp = std::make_shared<Connection>(std::move(socket));
  connections.insert(cp);
  cp->start();
}

void ConnectionManager::stop(Connection_ptr cp) {
  LOG(INFO) << "stop a connection";
  cp->stop();
  connections.erase(cp);
}

void ConnectionManager::stop_all() {
  LOG(INFO) << "stop all connection";
  for (auto &v : connections)
    v->stop();
  connections.clear();
}

Connection::Connection(tcp::socket socket, ConnectionManager *manager_)
  : socket(std::move(socket)), manager(manager_)
{}

void Connection::start() {
  LOG(INFO) << "Connection start work";
  readhead();
}

void Connection::stop() {
  socket.close();
}

void Connection::readhead() {
  async_read(socket, boost::asio::buffer(head, HeadSize),
    [&](boost::system::error_code ec, std::size_t bytes_transferred) {
      if (!ec) {
        LOG(INFO) << "connetion readhead successful";
        Head headinfo;
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
  if(packagetype == request) {
    switch (servicetype) {
      case ServiceType::sign : {
        RequestSign sign;
        sign.ParseFromString(body);
        service->sign(sign, shared_from_this());
        break;
      }
      case ServiceType::log : {
        RequestLog log;
        log.ParseFromString(body);
        service->log(log);
        break;
      }
      case ServiceType::find : {
        RequestFind find;
        find.ParseFromString(body);
        service->find(find, shared_from_this());
        break;
      }
      case ServiceType::change : {
        RequestChange change;
        change.ParseFromString(body);
        service->change(change);
        break;
      }
      case ServiceType::manage : {
        RequestManage manage;
        manage.ParseFromString(body);
        service->manage(manage);
        break;
      }
      case ServiceType::msg : {
        RequestMsg msg;
        msg.ParseFromString(body);
        service->msg(msg);
        break;
      }
      default : {
        LOG(INFO) << "get wrong servicetype";
      }
    }
  } else if(packagetype == heartbeat) {
  } else {
    LOG(INFO) << "get wrong packagetype";
  }
}

template <typename package>
void Connection::write(Head head, package body, std::function<void()> callback) {
  std::string target = head.SerializePartialAsString();
  target += body.SerializePartialAsString();
  async_write(
    socket, boost::asio::buffer(target, target.size()),
    [&](boost::system::error_code ec, std::size_t bytes_transferred) {
      if (!ec) {
        callback();
        LOG(INFO) << "Connetion write successful";
      } else {
        this->stop();
        manager->connections.erase(shared_from_this());
      }
    }
  );
}

Head Connection::enpackHead(int packagetype, int servicetype, int size) {
  int version = 1;
  Head head;
  head.set_version(version);
  head.set_packagetype(packagetype);
  head.set_servicetype(servicetype);
  head.set_bodysize(size);
  return head;
}

} //namespace vchat
