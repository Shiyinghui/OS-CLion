//
// Created by 24757 on 2018/7/6.
//
#include "virtualDisk.h"
#include <cstdio>
#include<iostream>
using namespace std;

extern leadingBlk super;

void blkRead(short id, void *block) {
    //���̿���Ϊid, �������ݶ����ַ�����block��
    FILE *file = fopen("virtualDisk.txt", "rb");
    fseek(file, id*BLOCK_SIZE, 0);
    fread(block, BLOCK_SIZE, 1, file);
    fclose(file);
}

void blkWrite(short id, void *block) {
    //�ַ�����block,���̿���Ϊid, ����д��������
    FILE *file = fopen("virtualDisk.txt", "rb+");
    // rb+ ��д��һ���������ļ���ֻ�����д����

    fseek(file, id*BLOCK_SIZE, 0);
    fwrite(block, BLOCK_SIZE, 1, file); //ָ�����ݿ��ָ�룬ÿ�����ݵĴ�С�����ݸ������ļ�ָ��
    fclose(file);
}

int blkAlloc() {
    //����һ�����̿�
    if(super.blkNum == 0) { //ջ��û�п��п�
        int res = super.nextLeadingBlk; // �ҵ����������һ���鳤��
        if(res == TOTAL_BLOCK) return -1;
        //�������������̵����һ�飬������ʧ��
        blkRead(res, &super);
        //������ָ����һ���鳤��
        return res; //�����鳤���
    }

    super.blkNum--;
  /*  cout<<"��������ʣ�������"<<super.blkNum<<endl;
    cout<<"����Ŀ�ţ�"<<super.member[super.blkNum]<<endl;*/
    return super.member[super.blkNum]; //ֱ�ӷ���
}

void blkFree(short id) {
    // ���̿���Ϊid �ͷ�
    if(super.blkNum== GROUP_BLK_NUM - 1) {
        //����30����п�

        blkWrite(id, &super); //�����������д��id����
        super.blkNum = 0;
        // ��������п���Ϊ0
        super.nextLeadingBlk = id;
    } else {
        super.member[super.blkNum++] = id;
    }
}


