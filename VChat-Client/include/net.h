#ifndef NET_H
#define NET_H

#include <boost/asio.hpp>
#include <qobject.h>
#include "ctos.pb.h"

const int HEAD_SIZE = 16;
const int BODY_SIZE = 50000;

using boost::asio::ip::tcp;

namespace net {

class Net : public QObject{
private:
  static tcp::socket socket;
  static boost::asio::io_context io_context;
  const std::string address = "127.0.0.1";
  const std::string port = "7080";
  tcp::resolver resolver;
  tcp::resolver::results_type endpoint;
  std::string head, body;
  void readhead();
  void recv_notify(int servicetype, int size);
  void recv_response(int servicetype, int size);

public:
  Net();
  ~Net();
  void connect();
  static void start();
  static void write(std::string ss);
  void close();

signals:
  void handle_response(int result, vchat::All all); // log
  void handle_response(int result, int id); // sign
  void handle_response(int result, vchat::PersionalInfo persionalinfo); // find
  void handle_response(int result, int id, std::string nickname); // add
  void handle_notify(); // add

};

extern std::condition_variable cv;
extern std::mutex mtx;
extern bool connection_finsih;
extern bool connection_result;

} // namespace net

#endif // NET_H
