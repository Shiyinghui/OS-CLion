//
// Created by 24757 on 2018/7/6.
//
#include "virtualDisk.h"
#include "function.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

extern User user;
SFD_item findAItem(short id, const char *name) {

    BFD_dir dir;
    blkRead(id, &dir);
    // 两个循环遍历一个目录的所有子项
    for(int i = 0, k = 0; i < dir.blkNum; i++) {
        SFD_item temp[ITEM_OF_BLK];   // temp[4]
        blkRead(dir.block[i], temp);
        for(int j = 0 ; j < ITEM_OF_BLK && k < dir.subDir; j++, k++)
            if(!strcmp(name, temp[j].name)) return temp[j];   // strcmp 相等返回0
    }
    SFD_item res;
    res.blockID = -1; //没找到， 块号-1表示不存在
    return res;
}

// 一个目录中是否存在一个子项
bool exist(short id, const char *name) {
    SFD_item it = findAItem(id, name);
    return it.blockID != -1;
}

// 在一个目录中
int addItem(short id, const char *name, bool isFile) {
    BFD_dir dir;
    string curTime;
    blkRead(id, &dir);
    if (exist(id, name)) {
        printf("此文件夹中已经包含同名文件或文件夹\n");
        return -1; //添加失败
    }
    int pos = dir.subDir % ITEM_OF_BLK;
    //cout<<pos<<" "<<dir.subDir<<endl;

    if (pos == 0)dir.block[dir.blkNum++] = blkAlloc();

    //读取最后一块并将新添子项加入最后一块
    SFD_item temp[ITEM_OF_BLK];
    blkRead(dir.block[dir.blkNum-1], temp);
    strcpy(temp[pos].name, name);
    temp[pos].blockID = blkAlloc();
    //cout<<"目录项申请到的块："<<temp[pos].blockID<<endl;

    blkWrite(dir.block[dir.blkNum-1], temp);

    BFD_dir tempDir;
    curTime = getTime();
    tempDir.blkNum = tempDir.subDir = tempDir.byteNum = 0;
    tempDir.ownerID = user.id; tempDir.isFile = isFile;
    strcpy(tempDir.time, curTime.c_str());
   // cout<<"时间："<<tempDir.time<<endl;
    memset(tempDir.access, 0, sizeof(tempDir.access)); //初始化
    tempDir.access[user.id] = true;
  //  cout<<"创建用户的编号:"<<user.id<<endl;
    blkWrite(temp[pos].blockID, &tempDir);
    dir.subDir++;
    blkWrite(id, &dir);

    return temp[pos].blockID;

}

void deleteFolder(short id) {
    BFD_dir dir;
    blkRead(id, &dir);
    if(!dir.access[user.id]) {
        cout<<"您没有权限删除此文件夹"<<endl;
        return;
    }
    for (int i = 0, k = 0; i < dir.blkNum; i++) {
        SFD_item temp[ITEM_OF_BLK];
        blkRead(dir.block[i], temp);  // dir.member[i]块读到temp中
        for (int j = 0; j < ITEM_OF_BLK && k < dir.subDir; j++, k++){
            BFD_dir tempDir;
            blkRead(temp[j].blockID, &tempDir);
            if (tempDir.isFile) deleteFile(temp[j].blockID); //删除文件
            else deleteFolder(temp[j].blockID);
        }
      //  cout<<"删除目录释放的块："<<dir.block[i]<<endl;
        blkFree(dir.block[i]);
    }
    blkFree(id);
}


bool deleteItem(short id, const char *name) { //删除编号为id的块的目录下名为name的子项

    BFD_dir dir;
    blkRead(id, &dir);

    //cout<<"要删除文件所在的目录块是："<<id<<endl;
    for (int i = 0, k = 0; i < dir.blkNum; i++) {

     //   cout<<"目录中包含块数："<<dir.blkNum<<endl;
        SFD_item temp[ITEM_OF_BLK];   // temp[4]
        blkRead(dir.block[i], temp);
        for (int j = 0; j < ITEM_OF_BLK && k < dir.subDir; j++, k++){
            if (!strcmp(name, temp[j].name)) {
                BFD_dir tempDir;
                blkRead(temp[j].blockID, &tempDir);

          //      cout<<"文件块号："<<temp[j].blockID<<endl;

                dir.byteNum -= tempDir.byteNum;
                if (tempDir.isFile) deleteFile(temp[j].blockID);
                else deleteFolder(temp[j].blockID);

                if(k < dir.subDir-1) {  // 删除的不是最后一个子项
                    SFD_item last[ITEM_OF_BLK];
                    blkRead(dir.block[dir.blkNum-1], last);
                    temp[j] = last[(dir.subDir-1)%ITEM_OF_BLK];
                    blkWrite(dir.block[i], temp);
                }
                if(dir.subDir % ITEM_OF_BLK == 1) blkFree(dir.block[--dir.blkNum]);
                dir.subDir--;
                blkWrite(id, &dir);
                return true;
            }
        }
    }
    return false;
}
