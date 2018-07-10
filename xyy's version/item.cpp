//
// Created by 24757 on 2018/6/30.
//
#include "disk.h"
#include <cstdio>
#include <cstring>

Item getItem(short id, const char *name) { //�ӱ��Ϊid�Ŀ��Ŀ¼�л�ȡһ����Ϊname������
    Dir dir; //�½�һ��Ŀ¼�����dir
    readBlock(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    for(int i = 0, k = 0; i < dir.blockNum; i++) { //������
        Item temp[ItemPerBlock];
        readBlock(dir.member[i], temp);
        for(int j = 0 ; j < ItemPerBlock && k < dir.itemNum; j++, k++) //����Ƚ�����
            if(!strcmp(name, temp[j].name)) return temp[j];
    }
    Item res; res.blockID = -1; //û�ҵ�����һ�����Ϊ-1�������ʾ������
    return res;
}

bool exist(short id, const char *name) { //�ж����Ϊid�Ŀ��Ŀ¼���Ƿ����һ����Ϊname������
    Item it = getItem(id, name); //����getItem
    return it.blockID != -1; //������Ϊ-1˵��������
}

int addItem(short id, const char *name, bool isFile) { //�ڱ��Ϊid�Ŀ��Ŀ¼�����һ����Ϊname�����isFile��ʾ���Ƿ�Ϊ�ļ�
    Dir dir; //�½�һ��Ŀ¼�����dir
    readBlock(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    if (exist(id, name)) {
        printf("���ļ������Ѿ�����ͬ���ļ����ļ���\n");
        return -1; //���ʧ��
    }
    int pos = dir.itemNum % ItemPerBlock; //pos��ʾ��������Ϊ��ǰ��ĵڼ�������
    if (pos == 0) dir.member[dir.blockNum++] = allocBlock(); //posΪ0ʱ��Ҫ�����µĿ�

    //��ȡ���һ�鲢����������������һ��
    Item temp[ItemPerBlock];
    readBlock(dir.member[dir.blockNum-1], temp);
    strcpy(temp[pos].name, name);
    temp[pos].blockID = allocBlock(); //Ϊ�µ��������Ŀ¼���ڴ�
    writeBlock(dir.member[dir.blockNum-1], temp);

    Dir it;
    it.blockNum = it.itemNum = it.byteNum = 0;
    it.authorID = user.id; it.isFile = isFile;
    memset(it.pri, 0, sizeof(it.pri)); it.pri[user.id] = true;
    //��ʼ���������Ŀ¼������

    writeBlock(temp[pos].blockID, &it);

    dir.itemNum++;
    writeBlock(id, &dir);

    return temp[pos].blockID; //�������һ����

}

void deleteFolder(short id) { //ɾ�����Ϊid�Ŀ��Ŀ¼
    Dir dir; //�½�һ��Ŀ¼�����dir
    readBlock(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    if(!dir.pri[user.id]) {
        printf("û��Ȩ��\n");
        return;
    }
    for (int i = 0, k = 0; i < dir.blockNum; i++) { //������
        Item temp[ItemPerBlock];
        readBlock(dir.member[i], temp);
        for (int j = 0; j < ItemPerBlock && k < dir.itemNum; j++, k++){
            Dir it;
            readBlock(temp[j].blockID, &it);
            if (it.isFile) deleteFile(temp[j].blockID); //ɾ���ļ�
            else deleteFolder(temp[j].blockID); //�ݹ�ɾ��Ŀ¼
        }
        freeBlock(dir.member[i]); //����ͷ�
    }
    freeBlock(id); //�ͷŵ�id��
}

bool deleteItem(short id, const char *name) { //ɾ�����Ϊid�Ŀ��Ŀ¼����Ϊname������
    Dir dir; //�½�һ��Ŀ¼�����dir
    readBlock(id, &dir); //�����Ϊid�Ŀ����ݶ���dir��
    for (int i = 0, k = 0; i < dir.blockNum; i++) { //������
        Item temp[ItemPerBlock];
        readBlock(dir.member[i], temp);
        for (int j = 0; j < ItemPerBlock && k < dir.itemNum; j++, k++){
            if (!strcmp(name, temp[j].name)) { //�ҵ���
                Dir it;
                readBlock(temp[j].blockID, &it);
                dir.byteNum -= it.byteNum;
                if (it.isFile) deleteFile(temp[j].blockID); //ɾ���ļ�
                else deleteFolder(temp[j].blockID); //ɾ��Ŀ¼

                //ɾ��֮�󽫿ճ���һ�����������һ����ѡ������һ����յ�λ����
                if(k < dir.itemNum-1) {
                    Item last[ItemPerBlock];
                    readBlock(dir.member[dir.blockNum-1], last);
                    temp[j] = last[(dir.itemNum-1)%ItemPerBlock];
                    writeBlock(dir.member[i], temp);
                }
                if(dir.itemNum % ItemPerBlock == 1) freeBlock(dir.member[--dir.blockNum]); //������һ��ֻ��һ���������һ
                dir.itemNum--;
                writeBlock(id, &dir);
                return true; //ɾ���ɹ�
            }
        }
    }
    return false; //ɾ��ʧ��
}

