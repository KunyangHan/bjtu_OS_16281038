#include "file.h"
#include <iostream>
#include <stdio.h>

void menu(FileSys *);
void getFileName(char *);
int main() {
    FileSys *fs = FileSys::GetFileSys();

    fs->init();

    menu(fs);

    return 0;
}


void menu(FileSys *fs) {
    bool exit = false;
    while (!exit) {
        int num;
        char name[12];
        char content[1536];
        cout << "1. 查看目录\t2. 打开文件" << endl;
        cout << "3. 创建文件\t4. 修改文件" << endl;
        cout << "5. 删除文件\t6. 保存至磁盘" << endl;
        cout << "7. 从磁盘恢复" << endl;
        cout << "其它. 退出" << endl;
        cin >> num;

        switch (num) {
        case 1:
            fs->directory();
            break;
        case 2:
            getFileName(name);
            fs->read(fs->open(name), content, 1536);
            cout << content << endl;
            break;
        case 3:
            getFileName(name);
            fs->create(name);
            break;
        case 4:
            int pos = 0;
            int len = 0;
            int index;
            getFileName(name);
            cout << "input where you want to start modify" << endl;
            cin >> pos;
            cout << "input the legth you wnat to modify" << endl;
            cin >> len;
            cout << "input modify text" << endl;
            scanf("%s", content);
            index = fs->open(name);
            fs->lseek(index, pos);
            fs->write(index, content, len);
            break;
        case 5:
            getFileName(name);
            fs->destroy(name);
            break;
        case 6:
            getFileName(name);
            fs->file2Disk(name);
            break;
        case 7:
            getFileName(name);
            fs->disk2File(name);
            break;
        default:
            exit = true;
            break;
        }
    }
}


void getFileName(char *n) {
    cout << "input a filename within 12 characters" << endl;
    cin >> n;
}