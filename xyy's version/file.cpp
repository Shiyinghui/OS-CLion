//
// Created by 24757 on 2018/6/30.
//
#include "disk.h"
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>

void readFile(short id, char *s) { //�����Ϊid�Ŀ���ļ����ݶ����ַ�����s��
    Dir dir; //�½�һ��Ŀ¼�����dir
    readBlock(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    if(!dir.pri[user.id]) {
        printf("û��Ȩ��\n");
        return;
    }
    int k = 0; //��֪Ҫ�������ļ�������dir�е�itemNum��Ա��¼��Ӧ���ļ��ֽ������ñ���k��¼�Ѷ��ֽ���
    for(int i = 0; i < dir.blockNum; i++) { //�������ļ���ռ�����п�
        Block temp;
        readBlock(dir.member[i], &temp); //��ÿһ�鶼����һ����ʱ�Ŀ����temp��
        for(int j = 0; j < BlockCap && k < dir.byteNum; j++, k++) //���ֽڱ����鲢������д��s
            s[k] = temp.byte[j];
    }
    s[k] = 0;
}

void writeFile(short id, const char *s) { //���ַ�����s�е�����д����Ϊid�Ŀ���ļ��У�����ļ��ǿ���Ḳ��
    Dir dir; //�½�һ��Ŀ¼�����dir
    readBlock(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    int byteCnt = strlen(s); //�����Ҫд����ֽ���ΪitemCnt
    if(byteCnt>20*64)
    {
        printf("���ݱ�ը������\n");
        exit(0);
    }
    int blockCnt = ceil(1.0*byteCnt / BlockCap); //�����Ҫ�õ��Ŀ���Ϊ1.0*itemCnt / BlockCap����ȡ��
    if(blockCnt > dir.blockNum) { //�����Ҫ�Ŀ�ȵ�ǰĿ¼��Ŀ����Ӧ�����µĿ�
        while(dir.blockNum < blockCnt) dir.member[dir.blockNum++] = allocBlock();
    } else if(blockCnt < dir.blockNum) { //�����Ҫ�Ŀ�ȵ�ǰĿ¼��Ŀ�����Ӧ�ͷŶ�Ŀ�
        while(dir.blockNum > blockCnt) freeBlock(dir.member[--dir.blockNum]);
    }

    dir.byteNum = byteCnt; //�����ļ��ֽ���
    for(int i = 0, k = 0; i < blockCnt; i++) { //�������п飬kΪ��ǰ��д���ֽ���
        Block temp;
        for(int j = 0; j < BlockCap && k < byteCnt; j++, k++)
            temp.byte[j] = s[k];
        writeBlock(dir.member[i], &temp);
    }
    writeBlock(id, &dir);
}

void deleteFile(short id) { //ɾ�����Ϊid�Ŀ���ļ���superָ���ų��������ݵı���
    Dir dir; //�½�һ��Ŀ¼�����dir
    readBlock(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    if(!dir.pri[user.id]) {
        printf("û��Ȩ��\n");
        return;
    }
    for(int i = 0; i < dir.blockNum; i++) freeBlock(dir.member[i]); //����ͷ��ڴ�
    freeBlock(id); //�ͷ�Ŀ¼�����ڿ���ڴ�
}


