#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <unordered_map>
#include <vector>
#include "login.h"
#include <QMainWindow>
#include <QListWidgetItem>
#include <QDateTime>
#include <QDebug>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    unordered_map<string,vector<string>> messagebox;
    Ui::MainWindow *ui;
    login* log;
    client* cli;
};
#endif // MAINWINDOW_H
