//
// Created by 24757 on 2018/7/6.
//

#include "virtualDisk.h"
#include "function.h"
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include<iostream>
using namespace std;

extern User user;

void readFile(short id, char *str) {
//读文件，编号为id的块的内容读到str中

    BFD_dir dir;
    blkRead(id, &dir); //将编号为id的块内容读到dir中
    if(!dir.access[user.id]) {
        cout<<"您没有权限读此文件！读取失败！"<<endl;
        return;
    }
    int i=0, j=0, k = 0;   //while循环变量
    while(i < dir.blkNum){ //文件所占的块
        Block tempBlk;
        blkRead(dir.block[i],&tempBlk);   //取出一块
        while(j < BLOCK_SIZE && k < dir.byteNum){
            str[k] = tempBlk.byte[j];     //读取字节存放在str数组中
            k++;
            j++;
        }
        i++;
    }
    str[k] = '\0';
}

void writeFile(short id, const char *s) {
    BFD_dir dir;
    blkRead(id, &dir);
    int totalByte = strlen(s);
   /* cout<<"字节数："<<totalByte<<endl;
    cout<<"目录所占块数："<<dir.blkNum<<endl;*/
    if(totalByte>14*64) {
        //写一个文件，文件内容可以占14个块，总共14*64字节
        cout<<"写入内容占用内存超过上限！写入失败！"<<endl;
        exit(0);
    }
    int blockNeeded = ceil(1.0*totalByte /BLOCK_SIZE);
    //得到写文件需要用到的块数，ceil上取整
    // cout<<"需要用到的块数："<<blockNeeded<<endl;

    // dir.blkNum文件原来所占的块数
    if(blockNeeded > dir.blkNum) {
        while(dir.blkNum <blockNeeded)
            dir.block[dir.blkNum++] = blkAlloc();
    } else if(blockNeeded < dir.blkNum) {
        // 新的内容用到的块比已有的少，释放
        while(dir.blkNum > blockNeeded) blkFree(dir.block[--dir.blkNum]);
    }

    dir.byteNum = totalByte; // 写入的字节数
    int i=0,j=0,k=0;
    while(i < blockNeeded){
        Block tempBlk;
        while(j < BLOCK_SIZE && k < totalByte){
            tempBlk.byte[j] = s[k];
            //字符数组s的逐字节赋给临时变量
            //写入磁盘块中
            blkWrite(dir.block[i],&tempBlk);
            j++, k++;
        }
        i++;
    }
    blkWrite(id, &dir); //写回原来的磁盘块中

}

void deleteFile(short id) {
    //文件SFD所在块为id,删除此文件
    BFD_dir dir;
    blkRead(id, &dir); //i节点的内容读到dir中
    cout<<"用户编号:"<<user.id<<endl;
    if(!dir.access[user.id]) {
        cout<<"您没有权限删除此文件！"<<endl;
        return;
    }
    for(int i = 0; i < dir.blkNum; i++) blkFree(dir.block[i]);
    //释放磁盘块
    blkFree(id); //释放文件的i节点
}
