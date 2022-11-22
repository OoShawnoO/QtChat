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
    connect(ui->send,&QPushButton::clicked,this,&MainWindow::send);
    connect(ui->close,&QPushButton::clicked,this,[=](){exit(0);});
    /* 监听工具栏 */
    connect(ui->toolBar,&QToolBar::actionTriggered,this,&MainWindow::toolbars);
}

MainWindow::~MainWindow()
{
    delete log;
    delete ui;
}

void MainWindow::toolbars(QAction* action){
    /* 监听login */
    if(action->text() == "login"){
        if(!log){log = new login(this);}
        log->show();
        connect(log,&login::success,this,&MainWindow::loginSuccess);
        connect(log,&login::cancel,this,[=](){log->hide();});
    }
    /* 监听getonline list */
    else if(action->text() == "list"){
        if(!cli){
            QMessageBox::critical(this,"Error","Not Login.");
        }else{
            onlines();
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
}

void MainWindow::loginSuccess(){
    log->hide();
    cli = log->get_client();
    QTimer* timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
       timer->start(1000);
       onlines();
    });
    timer->start(50);
    connect(cli,&client::msg,this,[=](){
        string msg = cli->get_queue().begin()->get_content();
        cli->get_queue().pop_front();
        changeDialog(ui->online->findItems(cli->get_pack().get_to().c_str(),Qt::MatchExactly)[0]);
        qDebug() << ui->online->findItems(cli->get_pack().get_to().c_str(),Qt::MatchExactly)[0]->text();
        QListWidgetItem* item = new QListWidgetItem(msg.c_str());
        ui->message->addItem(item);
        messagebox[cli->get_cur_to()].push_back(msg);
    });
    cli->getonline();
    setWindowTitle(cli->get_name().c_str());
    this->show();
}

void MainWindow::onlines(){
    if(cli->getonline()){
        ui->online->clear();
        for(auto i:cli->get_onlines()){
            if(i == ""){continue;}
            auto listitem = new QListWidgetItem(i.c_str());
            ui->online->addItem(listitem);
        }
        connect(ui->online,&QListWidget::itemClicked,this,&MainWindow::changeDialog);
    }
    else{
        QMessageBox::critical(this,"Error","Get Online List failed.");
    }
}

void MainWindow::changeDialog(QListWidgetItem* item){
    if(item->text().toStdString() != this->cli->get_name()){
        cli->get_cur_to() = item->text().toStdString();
        ui->label->setText(item->text());
        ui->message->clear();
        for(auto i : messagebox[cli->get_cur_to()]){
            auto item = new QListWidgetItem(i.c_str());
            ui->message->addItem(item);
        }
    }
}

void MainWindow::send(){
    if(cli && cli->get_cur_to()!=""){
        string msg = ui->content->document()->toPlainText().toStdString();
        QDateTime current_date_time =QDateTime::currentDateTime();
//        QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
        QString current_date =current_date_time.toString("[hh:mm]");
        msg = current_date.toStdString() + " " + cli->get_name() + ":" + msg;
        cli->message(cli->get_cur_to(),msg,MSG);
        QListWidgetItem* item = new QListWidgetItem(msg.c_str());
        ui->message->addItem(item);
        messagebox[cli->get_cur_to()].push_back(msg);
    }
    else if(cli){
        QMessageBox::critical(this,"Error","No User Select");
    }
    else{
        QMessageBox::critical(this,"Error","Not Login");
    }
}
