#include "personalinfo.h"
#include "../ui/ui_personalinfo.h"

PersonalInfo::PersonalInfo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PersonalInfo)
{
    ui->setupUi(this);
}

PersonalInfo::~PersonalInfo()
{
    delete ui;
}
