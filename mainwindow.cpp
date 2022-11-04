#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    log = new login(this);
    log->show();
    /* 监听login */
    connect(log,&login::success,this,[=](){
        log->hide();
        cli = log->get_client();
        this->show();
    });
    connect(log,&login::cancel,this,[=](){
       exit(0);
    });
    /* 监听发送与取消 */
    connect(ui->send,&QPushButton::clicked,this,[=](){
        string msg = ui->content->document()->toPlainText().toStdString();
        cli->message(cli->get_cur_to(),msg,MSG);
    });
    connect(ui->close,&QPushButton::clicked,this,[=](){
        exit(0);
    });
    /* 监听工具栏 */
    connect(ui->toolBar,&QToolBar::actionTriggered,this,[=](QAction* action){
        if(action->text() == "login"){

        }else if(action->text() == "list"){
            cli->get_onlines();
            for(auto i:cli->get_onlines()){

            }
        }else if(action->text() == "quit"){

        }else{}
    });
}

MainWindow::~MainWindow()
{
    delete log;
    delete ui;
}

