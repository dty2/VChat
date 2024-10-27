#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "ctos.pb.h"
#include <boost/asio.hpp>
#include <QWidget>

using boost::asio::ip::tcp;

namespace Ui {
class Dashboard;
}

class Chat;

class Dashboard : public QWidget {
  Q_OBJECT

public:
  explicit Dashboard(QWidget *parent = nullptr);
  ~Dashboard();

private:
  Ui::Dashboard *ui;
  void log(int id, int password);
  void sign(std::string nickname, int password);

public slots:
  void handle_response(int result, vchat::All all); // log
  void handle_response(int result, int id); // sign

};

#endif // DASHBOARD_H
