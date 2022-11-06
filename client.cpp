#include "client.h"
#include <QDebug>
const char* address = "127.0.0.1";
const short port = 9999;
using namespace std;

void split(const string& str,char splitchar,vector<string>& res){
    string strs = str + splitchar;
    int pos = strs.find(splitchar);
    while(pos != string::npos){
        string temp = strs.substr(0,pos);
        res.push_back(temp);
        strs = strs.substr(pos+1);
        pos = strs.find(splitchar);
    }
}

client::client(string _name,string _password,QObject* parent):
QObject(parent),name(_name),password(_password),pack(Pack(STANDARD,name,"Server",password,LOGIN))
{
    logined = false;
    t = new std::thread(READ,ref(*this));
    t->detach();
    fd = -1;
    cur_to = "";
    bzero(buffer,sizeof(buffer));
}
client::~client(){
    if(t){
        delete t;
    }
    close(fd);
}
bool client::connect(){
    fd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET,address,&serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(port);
    int ret = ::connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(ret==-1){
        perror("connect :");
        return false;
    }
    connected = true;
    return true;
}
bool client::login(){
    pack.get_from() = name;
    pack.get_to() = "Server";
    pack.get_type() = LOGIN;
    pack.get_content() = password;
    pack.get_size() = password.size();
    int ret = write(fd,pack.Dump().c_str(),pack.Dump().size());
    if(ret == -1){
        perror("login write");
        return false;
    }
    sleep(1);
    if(logined != true) return false;
    return true;
}

bool client::getonline(){
    if(logined == false) return false;
    pack.get_from() = name;
    pack.get_to() = "Server";
    pack.get_type() = GETONLINE;
    pack.get_content().clear();
    pack.get_size() = 0;
    int ret = write(fd,pack.Dump().c_str(),pack.Dump().size());
    if(ret == -1){
        perror("getonline write");
        return false;
    }
    return true;
}

bool client::message(string to,string content,TYPE type){
    if(logined == false) return false;
    done = false;
    pack.get_from() = name;
    pack.get_to() = to;
    pack.get_type() = type;
    pack.get_content() = content;
    pack.get_size() = content.size();
    int writeindex = 0;
    string x = pack.Dump();
    while(writeindex < x.size()){
        int ret = write(fd,x.substr(writeindex).c_str(),x.substr(writeindex).size());
        if(ret == -1){
            perror("message write");
            exit(-1);
        }
        writeindex += ret;
    }

    if(type != ACK){
        while(done != true && pack.get_type() != ERR){
        }
        if(pack.get_content() == "ok" && pack.get_type() == ACK){
           return true;
        }else{
            return false;
        }
    }
    return true;
}

bool client::readmsg(){
    string readCache;
    while(true){
        bzero(buffer,sizeof(buffer));
        int ret = read(fd,buffer,sizeof(buffer));
        if(ret == -1){
            perror("message read");
            return false;
        }else if(ret == 0){
            close(fd);
            exit(-1);
        }
        readCache += buffer;
        if(readCache.find("{") != string::npos && readCache.find("}")!=string::npos) break;
    }
    pack.Load(readCache);
    return true;
}

void client::READ(client& clt){
    while(1){
        if(clt.connected != true) continue;
        if(clt.readmsg()){
            TYPE types = clt.get_pack().get_type();
            if(types == MSG){
                clt.queue.push_back(clt.get_pack());
                clt.notice();
                Pack reply(clt.get_pack());
                reply.get_to() = reply.get_from();
                reply.get_content() = "ok";
                reply.get_size() = sizeof(reply.get_content());
                clt.message(reply.get_to(),reply.get_content(),ACK);
            }
            else if(types == ACK){
                clt.get_done() = true;
            }
            else if(types == GETONLINE){
                clt.onlines.clear();
                split(clt.get_pack().get_content(),'\\',clt.onlines);
            }else if(types == ERR){
                clt.err_content = clt.get_pack().get_content();
                clt.err = true;
            }else if(types == LOGIN){
                clt.logined = true;
            }
        }
    }
}

string& client::get_name(){
    return name;
}

string& client::get_password(){
    return password;
}

Pack& client::get_pack(){
    return pack;
}

bool& client::get_done(){
    return done;
}

bool& client::get_logined(){
    return logined;
}

bool &client::get_err()
{
    return err;
}

string &client::get_err_content()
{
    return err_content;
}

vector<string>& client::get_onlines(){
    return onlines;
}

string& client::get_cur_to(){
    return cur_to;
}

deque<Pack>& client::get_queue(){
    return queue;
}

void client::notice(){
    emit msg();
}
