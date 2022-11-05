#ifndef LOGIN_H
#define LOGIN_H

#include <QMessageBox>
#include <QDialog>
#include <client.h>
using namespace std;

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    client* get_client();

private:
    Ui::login *ui;
    client* cli;
signals:
    void success();
    void cancel();
};

#endif // LOGIN_H
