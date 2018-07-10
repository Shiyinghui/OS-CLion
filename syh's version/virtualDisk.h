//
// Created by 24757 on 2018/7/6.
//

#ifndef OSV4_DISK_H
#define OSV4_DISK_H

#include<iostream>
#include <vector>
#include "dir.h"
using namespace std;

const short TOTAL_BLOCK = 1000; //磁盘总块数
const short GROUP_BLK_NUM = 31; //一组31块
const short GROUP_BLK_MEM = 30; //每组块数
const short ITEM_OF_BLK = 4; //一块上的子项数
const short BLOCK_SIZE = 64; // 每块64字节

const short NAME_USER_LEN = 24; //最大用户名长度
const short PWD_USER_LEN = 24; //最大密码长度
const short MAIL_USER_LEN = 14; //邮箱长度

        // 一个组长块的数据结构
struct leadingBlk { //组长块
    short blkNum; //组内块数
    short nextLeadingBlk;  //下一个组长编号
    short member[GROUP_BLK_MEM]; //成员块编号
};

struct Block { // 一个磁盘块
    char byte[BLOCK_SIZE];
};

struct User {
    short id; //编号
    char name[NAME_USER_LEN]; //用户名
    char password[PWD_USER_LEN]; //密码
    char email[MAIL_USER_LEN]; //邮箱

};



#endif //OSV4_DISK_H
