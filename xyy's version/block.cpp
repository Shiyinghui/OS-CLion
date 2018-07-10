//
// Created by 24757 on 2018/6/30.
//
#include "disk.h"
#include <cstdio>

void readBlock(short id, void *block) { //��һ����̣�idΪ�ڴ���ţ�blockָ���ȡ���ݴ�ŵı���
    FILE *file = fopen(DISK, "rb");
    fseek(file, id*BlockCap, 0);
    fread(block, BlockCap, 1, file);
    fclose(file);
}

void writeBlock(short id, void *block) { //дһ����̣�idΪ�ڴ���ţ�blockָ�򱻶�ȡ���ݴ�ŵı���
    FILE *file = fopen(DISK, "rb+");     //rb+�����д����
    fseek(file, id*BlockCap, 0);
    fwrite(block, BlockCap, 1, file);   //ָ�����ݿ��ָ�룬ÿ�����ݵĴ�С�����ݸ������ļ�ָ��
    fclose(file);
}

int allocBlock() { //����һ���ڴ�
    if(super.blockNum == 0) { //�����ǰ���������ڵ���û�п��п�
        int res = super.nextLeader; //ָ����һ��
        if(res == BlockCnt) return -1; //���ָ���β˵��û�п��п飬����ʧ�ܷ���-1
        readBlock(res, &super); //����˵����һ����ã�ʹ������ָ����һ��
        return res; //����ԭ��һ���鳤����
    }
    return super.member[--super.blockNum]; //�����ǰ���п��п���ֱ�ӷ���
}

void freeBlock(short id) { //�ͷű��Ϊid�Ŀ��ڴ�
    if(super.blockNum == BlockPerGroup - 1) { //�����ǰ���������ڵ�����п���������
        writeBlock(id, &super); //�����Ϊid�Ŀ���Ϊ��ǰ����鳤��
        super.blockNum = 0;  //����������Ŀ��п������
        super.nextLeader = id; //ʹ������ָ����
    } else {
        super.member[super.blockNum++] = id; //����ֱ�ӽ����Ϊid�Ŀ���뵱ǰ�Ŀ���
    }
}


