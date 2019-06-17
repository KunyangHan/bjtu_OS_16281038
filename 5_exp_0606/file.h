#pragma once
#include "io.h"
#include <map>

struct Descriptor {
    int file_len;
    int disk_num[3];
    int des_num;
    char is_free;

    void setVar(int, int[], int, char);
};

struct MenuItem {
    char file_name[12]; 
    int des_num;

    void setVar(char [], int);
};

class FileSys: public IO {
private:
    FileSys() {}
    static FileSys *m_pInstance;
public:
    static FileSys *GetFileSys() { return m_pInstance == nullptr ? new FileSys() : m_pInstance; }
    ~FileSys() {}

    void init();
    void create(char[]);
    void destroy(char []);
    int open(char []);
    void read(int, char *, int);
    void write(int, char *, int);
    void lseek(int, int);

    void directory();
private:
    map<int, int> pos_table;
    Descriptor descriptor[256];
    MenuItem menu_item[31];

    void descriptor2Disk();
    void disk2Descriptor();

    void initDescriptor();
    void initMenu();
    int getFreeDes();
    int getFreeMenu();

    void setDescriptor(int, int, int[], int, char);
};
