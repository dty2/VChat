#ifndef PERSONALINFO_H
#define PERSONALINFO_H

#include <QDialog>

namespace Ui {
class PersonalInfo;
}

class PersonalInfo : public QDialog
{
    Q_OBJECT

public:
    explicit PersonalInfo(QWidget *parent = nullptr);
    ~PersonalInfo();

private:
    Ui::PersonalInfo *ui;
};

#endif // PERSONALINFO_H
