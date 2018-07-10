//
// Created by 24757 on 2018/7/6.
//
#include "virtualDisk.h"
#include <cstdio>
#include<iostream>
using namespace std;

extern leadingBlk super;

void blkRead(short id, void *block) {
    //磁盘块编号为id, 将其内容读到字符数组block中
    FILE *file = fopen("virtualDisk.txt", "rb");
    fseek(file, id*BLOCK_SIZE, 0);
    fread(block, BLOCK_SIZE, 1, file);
    fclose(file);
}

void blkWrite(short id, void *block) {
    //字符数组block,磁盘块编号为id, 内容写到磁盘里
    FILE *file = fopen("virtualDisk.txt", "rb+");
    // rb+ 读写打开一个二进制文件，只允许读写数据

    fseek(file, id*BLOCK_SIZE, 0);
    fwrite(block, BLOCK_SIZE, 1, file); //指向数据块的指针，每个数据的大小，数据个数，文件指针
    fclose(file);
}

int blkAlloc() {
    //申请一个磁盘块
    if(super.blkNum == 0) { //栈中没有空闲块
        int res = super.nextLeadingBlk; // 找到超级块的下一个组长块
        if(res == TOTAL_BLOCK) return -1;
        //如果已是虚拟磁盘的最后一块，则申请失败
        blkRead(res, &super);
        //超级块指向下一个组长块
        return res; //返回组长块号
    }

    super.blkNum--;
  /*  cout<<"超级块中剩余块数："<<super.blkNum<<endl;
    cout<<"申请的块号："<<super.member[super.blkNum]<<endl;*/
    return super.member[super.blkNum]; //直接分配
}

void blkFree(short id) {
    // 磁盘块编号为id 释放
    if(super.blkNum== GROUP_BLK_NUM - 1) {
        //已有30块空闲块

        blkWrite(id, &super); //超级块的内容写到id块中
        super.blkNum = 0;
        // 超级块空闲块数为0
        super.nextLeadingBlk = id;
    } else {
        super.member[super.blkNum++] = id;
    }
}


