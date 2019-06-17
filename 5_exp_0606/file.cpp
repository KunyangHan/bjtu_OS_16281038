#include "file.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Descriptor::setVar(int fl, int dn[], int n, char c) {
    file_len = fl;
    for (int i = 0; i < 3; i++) {
        disk_num[i] = dn[i];
    }
    des_num = n;
    is_free = c;
}

void MenuItem::setVar(char name[], int des) {
    strcpy(file_name, name);
    des_num = des;
}

void FileSys::create(char name[]) {
    int des, menu, disk;

    menu = getFreeMenu();
    des = getFreeDes();
    disk = getFree();

    descriptor[des].disk_num[0] = disk;
    menu_item[menu].setVar(name, des);
    descriptor[des].is_free = '1';

    setBitmap(disk, '1');
}

void FileSys::destroy(char name[]) {
    int menu = -1;
    for (int i = 0; i < 32; i++) {
        if (strcmp(menu_item[i].file_name, name) == 0) {
            menu = i;
            break;
        }
    }
    if (menu == -1) {
        printf("didnt find file %s��\n", name);
    }
    else {
        int des = menu_item[menu].des_num;

        memset(menu_item[menu].file_name, 0, sizeof(menu_item[menu].file_name));

        descriptor[des].is_free = '0';
        for (int i = 0; i < 3; i++) {
            if (descriptor[des].disk_num[i] != -1) {
                setBitmap(descriptor[des].disk_num[i], '0');
            }
        }
    }
}

int FileSys::open(char name[]) {
    int menu = -1;
    int result = -1;
    for (int i = 0; i < 32; i++) {
        if (strcmp(menu_item[i].file_name, name) == 0) {
            menu = i;
            break;
        }
    }
    if (menu == -1) {
        printf("didnt find file %s��\n", name);
    } 
    else {
        result = menu_item[menu].des_num;
    }

    return result;
}

void FileSys::read(int index, char * mem_area, int count) {
    char block[512];
    int ind = -1;
    int ptr = 0;
    int pos = pos_table[index];

    while (count > 0 && descriptor[index].disk_num[ind] != -1)    {
        ind += 1;
        int size = count > 512 ? 512 : count;
        count -= 512;

        readBlock(descriptor[index].disk_num[ind], block);
        memcpy(mem_area + ptr, block + pos, size);

        ptr += size;
        pos = 0;
    }
}

void FileSys::write(int index, char * mem_area, int count) {
    int pos = pos_table[index];
    int ind = pos / 512;
    int ptr = 0;
    char content[512];
    while (count > 0) {
        int size = count > (512 - pos) ? (512 - pos) : count;
        int block_num = descriptor[index].disk_num[ind];

        if (block_num == -1) {
            int disk = getFree();

            descriptor[index].disk_num[0] = disk;
            descriptor[index].is_free = '1';

            setBitmap(disk, '1');
        }
        readBlock(block_num, content);
        memcpy(content, (mem_area + ptr), size);
        writeBlock(block_num, content);

        pos = 0;
        ind += 1;
        ptr += size;
    }
}

void FileSys::lseek(int index, int p) {
    pos_table[index] = p;
}

void FileSys::directory() {
    int count = 0;

    printf("here files are:\n");
    for (int i = 0; i < 32; i++) {
        if (strlen(menu_item[i].file_name) != 0) {
            cout << "index\tfilename\t\tlength" << endl;
            cout << i + 1 << "\t" << menu_item[i].file_name << "\t";
            cout << descriptor[menu_item[i].des_num].file_len << endl;
            count++;
        }
    }
    cout << "totally " << count << " files found" << endl;
}

void FileSys::descriptor2Disk() {
    char temp_block[512];
    int disk_num = 2;
    int ind = 0;

    for (int i = 0; i < 256; i++, ind++) {
        char temp_descriptor[24];
        memcpy(temp_descriptor, &descriptor[i], sizeof(descriptor));
        memcpy(&temp_block[ind * 24], temp_descriptor, 24);

        if (i % 21 == 0) {
            ind = 0;

            writeBlock(chb2l(0, disk_num / B, disk_num % B), temp_block);
            disk_num++;
        }
    }
}

void FileSys::disk2Descriptor() {
    for (int i = 2; i < 15; i++) {
        char temp_block[512];

        readBlock(chb2l(0, i / B, i % B), temp_block);

        for (int j = 0; j < 21; j++) {
            if (((i - 2) * 21 + j) > 256)
                break;

            char temp_FileDescriptor[24];
            memcpy(temp_FileDescriptor, &temp_block[j * 24], 24);
            Descriptor *f = (Descriptor *)temp_FileDescriptor;

            setDescriptor((i - 2) * 21 + j, f->file_len, f->disk_num, f->des_num, f->is_free);
        }
    }
}

void FileSys::init() {
    initBitmap();
    initDescriptor();
    initDisk();
    initMenu();
}

void FileSys::initDescriptor() {
    for (int i = 0; i < 256; i++) {
        int  dn[3] = {i, -1, -1};
 
        setDescriptor(i, 0, dn, i, '0');
    }

    descriptor[0].is_free = '1';
    descriptor2Disk();
}

void FileSys::initMenu() {
    for (int i = 0; i < 32; i++) {
        menu_item[i].des_num = i;
    }
}

int FileSys::getFreeDes() {
    int result = -1;

    for (int i = 0; i < 256; i++) {
        if (descriptor[i].is_free == '0') {
            result = i;
            break;
        }
    }
    return result;
}

int FileSys::getFreeMenu() {
    int result = -1;

    for (int i = 0; i < 256; i++) {
        if (strlen(menu_item[i].file_name) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

void FileSys::setDescriptor(int i, int fl, int dn[], int n, char c) {
    descriptor[i].setVar(fl, dn, n, c);
}
