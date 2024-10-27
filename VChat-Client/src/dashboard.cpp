#include "dashboard.h"
#include "../ui/ui_dashboard.h"
#include "ctos.pb.h"
#include "chat.h"
#include "store.h"
#include "error.h"
#include "net.h"

extern Error error;

Dashboard::Dashboard(QWidget *parent) : QWidget(parent), ui(new Ui::Dashboard) {
  ui->setupUi(this);
  // main page
  connect(ui->log, &QPushButton::clicked,
          this, [&] { ui->page->setCurrentIndex(3); });
  connect(ui->sign, &QPushButton::clicked,
          this, [&] { ui->page->setCurrentIndex(1); });
  connect(ui->about, &QPushButton::clicked,
          this, [&] { ui->page->setCurrentIndex(2); });
  connect(ui->exit, &QPushButton::clicked,
          this, [&] { this->close(); });

  // log page
  connect(ui->log_log, &QPushButton::clicked,
          this, [&] {
    this->log(ui->account->text().toInt(), ui->password_log->text().toInt());
  });
  connect(ui->cancel_log, &QPushButton::clicked,
          this, [&] { ui->page->setCurrentIndex(0); });

  // sign page
  connect(ui->sign_sign, &QPushButton::clicked,
          this, [&] {
    this->sign(ui->nickname->text().toStdString(), ui->password_log->text().toInt());
  });
  connect(ui->cancel_sign, &QPushButton::clicked, this,
          [&] { ui->page->setCurrentIndex(0); });

  // about page
  connect(ui->return_info, &QPushButton::clicked, this,
          [&] { ui->page->setCurrentIndex(0); });
}

void Dashboard::log(int id, int password) {
  vchat::PersionalInfo info;
  info.set_id(id);
  info.set_password(password);
  vchat::Request request;
  request.set_status(1);
  request.set_op(1);
  request.set_allocated_info(&info);
  vchat::Header header;
  header.set_version(1);
  header.set_packagetype(0);
  header.set_servicetype(0);
  header.set_size(request.ByteSizeLong());
  vchat::Package package;
  package.set_allocated_header(&header);
  package.set_allocated_request(&request);
  std::string ss;
  package.SerializeToString(&ss);
  Store::getinstance(id);
  net::Net::write(ss);
}

void Dashboard::sign(std::string nickname, int password) {
  vchat::PersionalInfo info;
  info.set_nickname(nickname);
  info.set_password(password);
  vchat::Request request;
  request.set_status(1);
  request.set_op(1);
  request.set_allocated_info(&info);
  vchat::Header header;
  header.set_version(1);
  header.set_packagetype(0);
  header.set_servicetype(1);
  header.set_size(request.ByteSizeLong());
  vchat::Package package;
  package.set_allocated_header(&header);
  package.set_allocated_request(&request);
  std::string ss;
  package.SerializeToString(&ss);
  net::Net::write(ss);
}

// handle log
void Dashboard::handle_response(int result, vchat::All all) {
  if(!result) {
    Store::store->loadInfo();
    Store::store->persionalinfo = all.info();
    for(auto &v : all.friends())
      Store::store->friends[v.id()] = v;
    for(auto &v : all.msgs())
      if(v.sender() == Store::store->persionalinfo.info.id)
        Store::store->msgs[v.receiver()];
      else Store::store->msgs[v.sender()];
    this->hide();
    Chat::start();
  } else if(result == 1){ // log again
    error.seterror("错误:重复登陆");
    error.show();
  } else if(result == 2){ // password wrong
    error.seterror("错误:密码错误");
    error.show();
  }
}

// sign
void Dashboard::handle_response(int result, int id) {
  if(!result) {
    ui->account->textEdited(QString(id));
    error.seterror("成功:注册成功,请务必记住您的账号！");
    error.show();
    Store::store->persionalinfo.info.id = id;
    ui->page->setCurrentIndex(3); 
  } else {
    error.seterror("错误:未知错误");
    error.show();
  }
}
