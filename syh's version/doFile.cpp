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
//���ļ������Ϊid�Ŀ�����ݶ���str��

    BFD_dir dir;
    blkRead(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    if(!dir.access[user.id]) {
        cout<<"��û��Ȩ�޶����ļ�����ȡʧ�ܣ�"<<endl;
        return;
    }
    int i=0, j=0, k = 0;   //whileѭ������
    while(i < dir.blkNum){ //�ļ���ռ�Ŀ�
        Block tempBlk;
        blkRead(dir.block[i],&tempBlk);   //ȡ��һ��
        while(j < BLOCK_SIZE && k < dir.byteNum){
            str[k] = tempBlk.byte[j];     //��ȡ�ֽڴ����str������
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
   /* cout<<"�ֽ�����"<<totalByte<<endl;
    cout<<"Ŀ¼��ռ������"<<dir.blkNum<<endl;*/
    if(totalByte>14*64) {
        //дһ���ļ����ļ����ݿ���ռ14���飬�ܹ�14*64�ֽ�
        cout<<"д������ռ���ڴ泬�����ޣ�д��ʧ�ܣ�"<<endl;
        exit(0);
    }
    int blockNeeded = ceil(1.0*totalByte /BLOCK_SIZE);
    //�õ�д�ļ���Ҫ�õ��Ŀ�����ceil��ȡ��
    // cout<<"��Ҫ�õ��Ŀ�����"<<blockNeeded<<endl;

    // dir.blkNum�ļ�ԭ����ռ�Ŀ���
    if(blockNeeded > dir.blkNum) {
        while(dir.blkNum <blockNeeded)
            dir.block[dir.blkNum++] = blkAlloc();
    } else if(blockNeeded < dir.blkNum) {
        // �µ������õ��Ŀ�����е��٣��ͷ�
        while(dir.blkNum > blockNeeded) blkFree(dir.block[--dir.blkNum]);
    }

    dir.byteNum = totalByte; // д����ֽ���
    int i=0,j=0,k=0;
    while(i < blockNeeded){
        Block tempBlk;
        while(j < BLOCK_SIZE && k < totalByte){
            tempBlk.byte[j] = s[k];
            //�ַ�����s�����ֽڸ�����ʱ����
            //д����̿���
            blkWrite(dir.block[i],&tempBlk);
            j++, k++;
        }
        i++;
    }
    blkWrite(id, &dir); //д��ԭ���Ĵ��̿���

}

void deleteFile(short id) {
    //�ļ�SFD���ڿ�Ϊid,ɾ�����ļ�
    BFD_dir dir;
    blkRead(id, &dir); //i�ڵ�����ݶ���dir��
    cout<<"�û����:"<<user.id<<endl;
    if(!dir.access[user.id]) {
        cout<<"��û��Ȩ��ɾ�����ļ���"<<endl;
        return;
    }
    for(int i = 0; i < dir.blkNum; i++) blkFree(dir.block[i]);
    //�ͷŴ��̿�
    blkFree(id); //�ͷ��ļ���i�ڵ�
}
