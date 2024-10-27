#include <mutex>
#include <condition_variable>
#include "net.h"
#include "error.h"

extern Error error;

namespace net {

boost::asio::io_context Net::io_context;
tcp::socket Net::socket(io_context);
std::condition_variable cv;
std::mutex mtx;
bool connection_finsih = false;
bool connection_result = true;

Net::Net() : resolver(io_context) {
  endpoint = resolver.resolve(address, port);
  this->connect();
}

void Net::connect() {
  boost::asio::async_connect(socket, endpoint,
    [&](boost::system::error_code ec, tcp::endpoint) {
      std::lock_guard<std::mutex> lock(net::mtx);
      connection_finsih = true;
      if (!ec) {
        connection_result = true;
        this->readhead();
      } else {
        connection_result = false;
        this->close();
      }
      cv.notify_one();
    }
  );
}

void Net::start() {
  io_context.run();
}

void Net::write(std::string ss) {
  async_write(socket, boost::asio::buffer(ss, ss.size()),
    [&](boost::system::error_code ec, std::size_t bytes_transferred) {
      if (!ec) {
      } else {
        error.seterror("错误:连接服务器错误");
        error.show();
      }
    }
  );
}

Net::~Net() {}

void Net::close() {
  this->io_context.stop();
}

void Net::readhead() {
  async_read(socket, boost::asio::buffer(head, HEAD_SIZE),
    [&](boost::system::error_code ec, std::size_t bytes_transferred) {
      if (!ec) {
        vchat::Header header;
        header.ParsePartialFromString(head);
        if(header.packagetype() == 4)
          recv_response(header.servicetype(), header.size());
        if(header.packagetype() == 3)
          recv_notify(header.servicetype(), header.size());
      } else {
        this->close();
        error.seterror("错误:连接服务器错误");
        error.show();
      }
    }
  );
}

void Net::recv_notify(int servicetype, int size) {
  async_read(socket, boost::asio::buffer(body, size),
    [servicetype, this](boost::system::error_code ec, std::size_t bytes_transferred) {
      if(!ec) {
        vchat::Notify notify;
        notify.ParsePartialFromString(body);
        handle_notify();
        readhead();
      } else {
        this->close();
        error.seterror("错误:连接服务器错误");
        error.show();
      }
    }
  );
}

void Net::recv_response(int servicetype, int size) {
  async_read(socket, boost::asio::buffer(body, size),
    [servicetype, this](boost::system::error_code ec, std::size_t bytes_transferred) {
      if(!ec) {
        vchat::Response response;
        response.ParsePartialFromString(body);
        switch (servicetype) {
          case 0:
            emit handle_response(response.result(), response.all());
            break;
          case 1:
            emit handle_response(response.result(), response.persionalinfo().id());
             break;
          case 2:
            emit handle_response(response.result(), response.persionalinfo());
            break;
          case 3:
            emit handle_response(response.result(), response.persionalinfo().id(), response.persionalinfo().nickname());
            break;
        }
        readhead();
      } else {
        this->close();
      }
    }
  );
}

} // namespace net
