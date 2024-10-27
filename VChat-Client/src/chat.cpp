#include "chat.h"
#include "../ui/ui_chat.h"

Chat* Chat::chat = nullptr;

void Chat::start() {
  chat = new Chat;
}

Chat::Chat(QWidget *parent) : QMainWindow(parent), ui(new Ui::Chat) {
  ui->setupUi(this);
  connect(ui->personal_info_button, &QPushButton::clicked, [=]() { });
  connect(ui->msg_button, &QPushButton::clicked, [=]() {
    ui->list->setCurrentIndex(1);
    ui->right_widget->setCurrentIndex(0);
  });
  connect(ui->personal_chat_button, &QPushButton::clicked, [=]() {
    ui->list->setCurrentIndex(0);
    ui->right_widget->setCurrentIndex(1);
  });
  connect(ui->option_button, &QPushButton::clicked, [=]() {});
  connect(ui->exit_button, &QPushButton::clicked, [=]() { this->close(); });
}

Chat::~Chat() { delete ui; }
