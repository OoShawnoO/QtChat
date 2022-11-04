#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include "pack.h"
#include <memory.h>
#include <thread>

class client{
public:
    client(string _name,string _password);
    ~client();
    bool connect();
    bool login();
    bool getonline();
    bool message(string to,string content,TYPE type);
    bool readmsg();
    string& get_name();
    string& get_password();
    Pack& get_pack();
    bool& get_done();
    bool& get_logined();
    bool& get_err();
    string& get_err_content();
    vector<string>& get_onlines();
    string& get_cur_to();
    static void READ(client&);
private:
    int fd;
    string name;
    string password;
    Pack pack;
    char buffer[1024];
    bool done;
    bool logined;
    vector<string> onlines;
    bool err;
    string err_content;
    bool connected;
    thread* t;
    string cur_to;
};



#endif
