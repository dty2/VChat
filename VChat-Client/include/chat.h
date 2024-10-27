#ifndef CHAT_H
#define CHAT_H

#include <QMainWindow>
#include <boost/asio.hpp>
#include "ctos.pb.h"

using boost::asio::ip::tcp;

namespace Ui {
class Chat;
}

class Chat : public QMainWindow {
public:
  static void start();
  Chat& operator=(const Chat&) = delete;
  Chat(const Chat&) = delete;

private:
  static Chat* chat;
  Chat(QWidget *parent = nullptr);
  ~Chat();
  Ui::Chat *ui;

// public slots:
  // void handle_response(int result, int id, std::string nickname); // add
  
  // void handle_response(int result, vchat::PersionalInfo persionalinfo); // find

  Q_OBJECT
};

#endif // CHAT_H
