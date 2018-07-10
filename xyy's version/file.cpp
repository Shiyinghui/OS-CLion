//
// Created by 24757 on 2018/6/30.
//
#include "disk.h"
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>

void readFile(short id, char *s) { //将编号为id的块的文件内容读到字符数组s中
    Dir dir; //新建一个目录项变量dir
    readBlock(id, &dir); //将编号为id的块内容读到dir中
    if(!dir.pri[user.id]) {
        printf("没有权限\n");
        return;
    }
    int k = 0; //已知要读的是文件内容则dir中的itemNum成员记录的应是文件字节数，用变量k记录已读字节数
    for(int i = 0; i < dir.blockNum; i++) { //遍历该文件所占的所有块
        Block temp;
        readBlock(dir.member[i], &temp); //将每一块都读到一个临时的块变量temp中
        for(int j = 0; j < BlockCap && k < dir.byteNum; j++, k++) //逐字节遍历块并将内容写入s
            s[k] = temp.byte[j];
    }
    s[k] = 0;
}

void writeFile(short id, const char *s) { //将字符数组s中的内容写入编号为id的块的文件中，如果文件非空则会覆盖
    Dir dir; //新建一个目录项变量dir
    readBlock(id, &dir); //将编号为id的块内容读到dir中
    int byteCnt = strlen(s); //计算出要写入的字节数为itemCnt
    if(byteCnt>20*64)
    {
        printf("数据爆炸！！！\n");
        exit(0);
    }
    int blockCnt = ceil(1.0*byteCnt / BlockCap); //计算出要用到的块数为1.0*itemCnt / BlockCap的上取整
    if(blockCnt > dir.blockNum) { //如果需要的块比当前目录项的块多则应申请新的块
        while(dir.blockNum < blockCnt) dir.member[dir.blockNum++] = allocBlock();
    } else if(blockCnt < dir.blockNum) { //如果需要的块比当前目录项的块少则应释放多的块
        while(dir.blockNum > blockCnt) freeBlock(dir.member[--dir.blockNum]);
    }

    dir.byteNum = byteCnt; //更新文件字节数
    for(int i = 0, k = 0; i < blockCnt; i++) { //遍历所有块，k为当前已写入字节数
        Block temp;
        for(int j = 0; j < BlockCap && k < byteCnt; j++, k++)
            temp.byte[j] = s[k];
        writeBlock(dir.member[i], &temp);
    }
    writeBlock(id, &dir);
}

void deleteFile(short id) { //删除编号为id的块的文件，super指向存放超级块内容的变量
    Dir dir; //新建一个目录项变量dir
    readBlock(id, &dir); //将编号为id的块内容读到dir中
    if(!dir.pri[user.id]) {
        printf("没有权限\n");
        return;
    }
    for(int i = 0; i < dir.blockNum; i++) freeBlock(dir.member[i]); //逐块释放内存
    freeBlock(id); //释放目录项所在块的内存
}


