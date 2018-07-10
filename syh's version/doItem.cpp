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
    // ����ѭ������һ��Ŀ¼����������
    for(int i = 0, k = 0; i < dir.blkNum; i++) {
        SFD_item temp[ITEM_OF_BLK];   // temp[4]
        blkRead(dir.block[i], temp);
        for(int j = 0 ; j < ITEM_OF_BLK && k < dir.subDir; j++, k++)
            if(!strcmp(name, temp[j].name)) return temp[j];   // strcmp ��ȷ���0
    }
    SFD_item res;
    res.blockID = -1; //û�ҵ��� ���-1��ʾ������
    return res;
}

// һ��Ŀ¼���Ƿ����һ������
bool exist(short id, const char *name) {
    SFD_item it = findAItem(id, name);
    return it.blockID != -1;
}

// ��һ��Ŀ¼��
int addItem(short id, const char *name, bool isFile) {
    BFD_dir dir;
    string curTime;
    blkRead(id, &dir);
    if (exist(id, name)) {
        printf("���ļ������Ѿ�����ͬ���ļ����ļ���\n");
        return -1; //���ʧ��
    }
    int pos = dir.subDir % ITEM_OF_BLK;
    //cout<<pos<<" "<<dir.subDir<<endl;

    if (pos == 0)dir.block[dir.blkNum++] = blkAlloc();

    //��ȡ���һ�鲢����������������һ��
    SFD_item temp[ITEM_OF_BLK];
    blkRead(dir.block[dir.blkNum-1], temp);
    strcpy(temp[pos].name, name);
    temp[pos].blockID = blkAlloc();
    //cout<<"Ŀ¼�����뵽�Ŀ飺"<<temp[pos].blockID<<endl;

    blkWrite(dir.block[dir.blkNum-1], temp);

    BFD_dir tempDir;
    curTime = getTime();
    tempDir.blkNum = tempDir.subDir = tempDir.byteNum = 0;
    tempDir.ownerID = user.id; tempDir.isFile = isFile;
    strcpy(tempDir.time, curTime.c_str());
   // cout<<"ʱ�䣺"<<tempDir.time<<endl;
    memset(tempDir.access, 0, sizeof(tempDir.access)); //��ʼ��
    tempDir.access[user.id] = true;
  //  cout<<"�����û��ı��:"<<user.id<<endl;
    blkWrite(temp[pos].blockID, &tempDir);
    dir.subDir++;
    blkWrite(id, &dir);

    return temp[pos].blockID;

}

void deleteFolder(short id) {
    BFD_dir dir;
    blkRead(id, &dir);
    if(!dir.access[user.id]) {
        cout<<"��û��Ȩ��ɾ�����ļ���"<<endl;
        return;
    }
    for (int i = 0, k = 0; i < dir.blkNum; i++) {
        SFD_item temp[ITEM_OF_BLK];
        blkRead(dir.block[i], temp);  // dir.member[i]�����temp��
        for (int j = 0; j < ITEM_OF_BLK && k < dir.subDir; j++, k++){
            BFD_dir tempDir;
            blkRead(temp[j].blockID, &tempDir);
            if (tempDir.isFile) deleteFile(temp[j].blockID); //ɾ���ļ�
            else deleteFolder(temp[j].blockID);
        }
      //  cout<<"ɾ��Ŀ¼�ͷŵĿ飺"<<dir.block[i]<<endl;
        blkFree(dir.block[i]);
    }
    blkFree(id);
}


bool deleteItem(short id, const char *name) { //ɾ�����Ϊid�Ŀ��Ŀ¼����Ϊname������

    BFD_dir dir;
    blkRead(id, &dir);

    //cout<<"Ҫɾ���ļ����ڵ�Ŀ¼���ǣ�"<<id<<endl;
    for (int i = 0, k = 0; i < dir.blkNum; i++) {

     //   cout<<"Ŀ¼�а���������"<<dir.blkNum<<endl;
        SFD_item temp[ITEM_OF_BLK];   // temp[4]
        blkRead(dir.block[i], temp);
        for (int j = 0; j < ITEM_OF_BLK && k < dir.subDir; j++, k++){
            if (!strcmp(name, temp[j].name)) {
                BFD_dir tempDir;
                blkRead(temp[j].blockID, &tempDir);

          //      cout<<"�ļ���ţ�"<<temp[j].blockID<<endl;

                dir.byteNum -= tempDir.byteNum;
                if (tempDir.isFile) deleteFile(temp[j].blockID);
                else deleteFolder(temp[j].blockID);

                if(k < dir.subDir-1) {  // ɾ���Ĳ������һ������
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
