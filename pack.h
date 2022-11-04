#ifndef PACK_H
#define PACK_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum VERSION{
        STANDARD = 0,
};
enum TYPE{
    MSG = 0,
    LOGIN,
    GETONLINE,
    ACK,
    ERR,
};

class Pack{
public:
    Pack(VERSION _version,string _from,string _to,string _content,TYPE _type)
    :version(_version),from(_from),to(_to),content(_content),type(_type)
    {
        size = content.size();
    }

    Pack(string package);
    Pack(const Pack& pack);
    string Dump();
    bool Load(string package);
    void clear();
    int& get_size();
    VERSION& get_version();
    string& get_from();
    string& get_to();
    string& get_content();
    TYPE& get_type();
private:
    VERSION version;
    string from;
    string to;
    string content;
    int size;
    TYPE type;
};



#endif
