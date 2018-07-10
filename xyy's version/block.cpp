//
// Created by 24757 on 2018/6/30.
//
#include "disk.h"
#include <cstdio>

void readBlock(short id, void *block) { //读一块磁盘，id为内存块编号，block指向读取内容存放的变量
    FILE *file = fopen(DISK, "rb");
    fseek(file, id*BlockCap, 0);
    fread(block, BlockCap, 1, file);
    fclose(file);
}

void writeBlock(short id, void *block) { //写一块磁盘，id为内存块编号，block指向被读取内容存放的变量
    FILE *file = fopen(DISK, "rb+");     //rb+允许读写数据
    fseek(file, id*BlockCap, 0);
    fwrite(block, BlockCap, 1, file);   //指向数据块的指针，每个数据的大小，数据个数，文件指针
    fclose(file);
}

int allocBlock() { //申请一块内存
    if(super.blockNum == 0) { //如果当前超级块所在的组没有空闲块
        int res = super.nextLeader; //指向下一组
        if(res == BlockCnt) return -1; //如果指向块尾说明没有空闲块，申请失败返回-1
        readBlock(res, &super); //否则说明下一组可用，使超级块指向下一组
        return res; //返回原下一组组长块编号
    }
    return super.member[--super.blockNum]; //如果当前组有空闲块则直接分配
}

void freeBlock(short id) { //释放编号为id的块内存
    if(super.blockNum == BlockPerGroup - 1) { //如果当前超级块所在的组空闲块数量满了
        writeBlock(id, &super); //将编号为id的块设为当前组的组长块
        super.blockNum = 0;  //并将超级块的空闲块数清空
        super.nextLeader = id; //使超级块指向它
    } else {
        super.member[super.blockNum++] = id; //否则直接将编号为id的块加入当前的块组
    }
}


