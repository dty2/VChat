#include <QApplication>
#include <QHotkey>
#include <qobject.h>
#include <thread>
#include "ctos.pb.h"
#include "error.h"
#include "dashboard.h"
#include "net.h"

void init_signals_slot(net::Net &net, Dashboard &dashboard);
Error error;

int main(int argc, char *argv[]) {
  // net
  net::Net net;
  QApplication app(argc, argv);
  std::thread p([&] {
    net::Net::start();
  });
  // gui
  Dashboard dashboard;
  init_signals_slot(net, dashboard);
  std::unique_lock<std::mutex> lock(net::mtx);
  net::cv.wait(lock, []{ return net::connection_finsih; });
  if(net::connection_result) {
    dashboard.show();
  } else {
    error.seterror("错误:服务器连接失败");
    error.show();
  }
  lock.unlock();
  app.exec();
  p.join();
  return 0;
}

void init_signals_slot(net::Net &net, Dashboard &dashboard) {
  void (net::Net:: *signals_log)(int, vchat::All) = &net::Net::handle_response;
  void (net::Net:: *signals_sign)(int, int) = &net::Net::handle_response;
  void (net::Net:: *signals_find)(int, vchat::PersionalInfo) = &net::Net::handle_response;
  void (net::Net:: *signals_add)(int, int, std::string) = &net::Net::handle_response;
  void (Dashboard:: *slot_log)(int, vchat::All) = &Dashboard::handle_response;
  void (Dashboard:: *slot_sign)(int, int) = &Dashboard::handle_response;
  QObject::connect(&net, signals_log, &dashboard, &slot_log);
  QObject::connect(&net, signals_sign, &dashboard, &slot_sign);
}

  // QHotkey hotkey(QKeySequence("Alt+E"), true, &app);
  // QObject::connect(&hotkey, &QHotkey::activated, qApp, [&] {
  //   if(dashboard.isVisible()) dashboard.hide();
  //   else dashboard.show();
  // });
