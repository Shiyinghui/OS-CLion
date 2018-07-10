//
// Created by 24757 on 2018/7/6.
//

#ifndef OSV4_DISK_H
#define OSV4_DISK_H

#include<iostream>
#include <vector>
#include "dir.h"
using namespace std;

const short TOTAL_BLOCK = 1000; //�����ܿ���
const short GROUP_BLK_NUM = 31; //һ��31��
const short GROUP_BLK_MEM = 30; //ÿ�����
const short ITEM_OF_BLK = 4; //һ���ϵ�������
const short BLOCK_SIZE = 64; // ÿ��64�ֽ�

const short NAME_USER_LEN = 24; //����û�������
const short PWD_USER_LEN = 24; //������볤��
const short MAIL_USER_LEN = 14; //���䳤��

        // һ���鳤������ݽṹ
struct leadingBlk { //�鳤��
    short blkNum; //���ڿ���
    short nextLeadingBlk;  //��һ���鳤���
    short member[GROUP_BLK_MEM]; //��Ա����
};

struct Block { // һ�����̿�
    char byte[BLOCK_SIZE];
};

struct User {
    short id; //���
    char name[NAME_USER_LEN]; //�û���
    char password[PWD_USER_LEN]; //����
    char email[MAIL_USER_LEN]; //����

};



#endif //OSV4_DISK_H
