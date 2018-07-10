//
// Created by 24757 on 2018/6/30.
//
#include "disk.h"
#include <cstdio>
#include <cstring>

Item getItem(short id, const char *name) { //从编号为id的块的目录中获取一个名为name的子项
    Dir dir; //新建一个目录项变量dir
    readBlock(id, &dir); //将编号为id的块内容读到dir中
    for(int i = 0, k = 0; i < dir.blockNum; i++) { //逐块遍历
        Item temp[ItemPerBlock];
        readBlock(dir.member[i], temp);
        for(int j = 0 ; j < ItemPerBlock && k < dir.itemNum; j++, k++) //逐项比较名称
            if(!strcmp(name, temp[j].name)) return temp[j];
    }
    Item res; res.blockID = -1; //没找到返回一个块号为-1的子项表示不存在
    return res;
}

bool exist(short id, const char *name) { //判读编号为id的块的目录中是否存在一个名为name的子项
    Item it = getItem(id, name); //尝试getItem
    return it.blockID != -1; //如果块号为-1说明不存在
}

int addItem(short id, const char *name, bool isFile) { //在编号为id的块的目录下添加一个名为name的子项，isFile表示其是否为文件
    Dir dir; //新建一个目录项变量dir
    readBlock(id, &dir); //将编号为id的块内容读到dir中
    if (exist(id, name)) {
        printf("此文件夹中已经包含同名文件或文件夹\n");
        return -1; //添加失败
    }
    int pos = dir.itemNum % ItemPerBlock; //pos表示新添子项为当前块的第几个子项
    if (pos == 0) dir.member[dir.blockNum++] = allocBlock(); //pos为0时需要申请新的块

    //读取最后一块并将新添子项加入最后一块
    Item temp[ItemPerBlock];
    readBlock(dir.member[dir.blockNum-1], temp);
    strcpy(temp[pos].name, name);
    temp[pos].blockID = allocBlock(); //为新的子项分配目录项内存
    writeBlock(dir.member[dir.blockNum-1], temp);

    Dir it;
    it.blockNum = it.itemNum = it.byteNum = 0;
    it.authorID = user.id; it.isFile = isFile;
    memset(it.pri, 0, sizeof(it.pri)); it.pri[user.id] = true;
    //初始化新子项的目录项内容

    writeBlock(temp[pos].blockID, &it);

    dir.itemNum++;
    writeBlock(id, &dir);

    return temp[pos].blockID; //返回最后一块编号

}

void deleteFolder(short id) { //删除编号为id的块的目录
    Dir dir; //新建一个目录项变量dir
    readBlock(id, &dir); //将编号为id的块内容读到dir中
    if(!dir.pri[user.id]) {
        printf("没有权限\n");
        return;
    }
    for (int i = 0, k = 0; i < dir.blockNum; i++) { //逐块遍历
        Item temp[ItemPerBlock];
        readBlock(dir.member[i], temp);
        for (int j = 0; j < ItemPerBlock && k < dir.itemNum; j++, k++){
            Dir it;
            readBlock(temp[j].blockID, &it);
            if (it.isFile) deleteFile(temp[j].blockID); //删除文件
            else deleteFolder(temp[j].blockID); //递归删除目录
        }
        freeBlock(dir.member[i]); //逐块释放
    }
    freeBlock(id); //释放第id块
}

bool deleteItem(short id, const char *name) { //删除编号为id的块的目录下名为name的子项
    Dir dir; //新建一个目录项变量dir
    readBlock(id, &dir); //将编号为id的块内容读到dir中
    for (int i = 0, k = 0; i < dir.blockNum; i++) { //逐块遍历
        Item temp[ItemPerBlock];
        readBlock(dir.member[i], temp);
        for (int j = 0; j < ItemPerBlock && k < dir.itemNum; j++, k++){
            if (!strcmp(name, temp[j].name)) { //找到了
                Dir it;
                readBlock(temp[j].blockID, &it);
                dir.byteNum -= it.byteNum;
                if (it.isFile) deleteFile(temp[j].blockID); //删除文件
                else deleteFolder(temp[j].blockID); //删除目录

                //删完之后将空出来一项，如果不是最后一项则选择把最后一项补到空的位置上
                if(k < dir.itemNum-1) {
                    Item last[ItemPerBlock];
                    readBlock(dir.member[dir.blockNum-1], last);
                    temp[j] = last[(dir.itemNum-1)%ItemPerBlock];
                    writeBlock(dir.member[i], temp);
                }
                if(dir.itemNum % ItemPerBlock == 1) freeBlock(dir.member[--dir.blockNum]); //如果最后一块只有一项则块数减一
                dir.itemNum--;
                writeBlock(id, &dir);
                return true; //删除成功
            }
        }
    }
    return false; //删除失败
}

