#include "error.h"
#include "../ui/ui_error.h"

Error::Error(QWidget *parent) : QDialog(parent), ui(new Ui::Error) {
  ui->setupUi(this);
  connect(ui->button, &QPushButton::clicked, this,
          [=] { this->close(); });
}

void Error::seterror(std::string error) {
  ui->label->setText(QString(error.c_str()));
}

Error::~Error() { delete ui; }
