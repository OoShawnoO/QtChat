#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    log = nullptr;
    cli = nullptr;
    /* 监听发送与取消 */
    connect(ui->send,&QPushButton::clicked,this,[=](){
        if(cli && cli->get_cur_to()!=""){
            string msg = ui->content->document()->toPlainText().toStdString();
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
            msg = current_date.toStdString() + "    " + cli->get_name() + ":" + msg;
            cli->message(cli->get_cur_to(),msg,MSG);
            QListWidgetItem* item = new QListWidgetItem(msg.c_str());
            ui->message->addItem(item);
            messagebox[cli->get_cur_to()].push_back(msg);
        }
        else{
            QMessageBox::critical(this,"Error","Not Login or Not Select Message To.");
        }
    });
    connect(ui->close,&QPushButton::clicked,this,[=](){
        exit(0);
    });
    /* 监听工具栏 */
    connect(ui->toolBar,&QToolBar::actionTriggered,this,[=](QAction* action){
        /* 监听login */
        if(action->text() == "login"){
            if(!log){
                log = new login(this);
            }
            log->show();
            connect(log,&login::success,this,[=](){
                log->hide();
                cli = log->get_client();
                connect(cli,&client::msg,this,[=](){
                    string msg = cli->get_queue().begin()->get_content();
                    cli->get_queue().pop_front();
                    QListWidgetItem* item = new QListWidgetItem(msg.c_str());
                    ui->message->addItem(item);
                    messagebox[cli->get_cur_to()].push_back(msg);
                });
                cli->getonline();
                this->show();
            });
            connect(log,&login::cancel,this,[=](){
               log->hide();
            });
        }
        /* 监听getonline list */
        else if(action->text() == "list"){
            if(!cli){
                QMessageBox::critical(this,"Error","Not Login.");
            }else{
                if(cli->getonline()){
                    ui->online->clear();
                    for(auto i:cli->get_onlines()){
                        if(i == ""){continue;}
                        auto listitem = new QListWidgetItem(i.c_str(),ui->online);
                    }
                    connect(ui->online,&QListWidget::itemClicked,this,[=](QListWidgetItem *item){
                        cli->get_cur_to() = item->text().toStdString();
                    });
                }
                else{
                    QMessageBox::critical(this,"Error","Get Online List failed.");
                }
            }
        }

        /* 监听quit login*/
        else if(action->text() == "quit"){
            if(!cli){
                QMessageBox::critical(this,"Error","Not Login.");
            }else{
                cli = nullptr;
                log->disconnect();
                delete log;
                log = nullptr;
                QMessageBox::critical(this,"Notice","Login Quited.");
            }
        }

        else{

        }
    });

}

MainWindow::~MainWindow()
{
    delete log;
    delete ui;
}

