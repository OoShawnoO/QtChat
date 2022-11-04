#include "login.h"
#include "ui_login.h"
#include <QDebug>
#include <QMessageBox>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    cli = new client("","");

    connect(ui->loginin,&QPushButton::clicked,this,[=](){
       qDebug()<<ui->username->text();
       qDebug()<<ui->password->text();
       cli->get_name() = ui->username->text().toStdString();
       cli->get_password() = ui->password->text().toStdString();
       if(!cli->connect()){
            QMessageBox::critical(this,"Error","Connect Failed");
       }else{
           if(!cli->login()){
                QMessageBox::critical(this,"Error",cli->get_err_content().c_str());
           }else{
                emit success();
           }
       }
    });
    connect(ui->cancel,&QPushButton::clicked,this,[=](){
        emit cancel();
    });
}

login::~login()
{
    delete cli;
    delete ui;
}

client* login::get_client(){
    return cli;
}
