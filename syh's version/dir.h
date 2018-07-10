//
// Created by 24757 on 2018/7/5.
//

#ifndef OSV4_DIRECTORY_H
#define OSV4_DIRECTORY_H
const int MAX_USER_NUM = 9;  //����û�����
const int MAX_ITEM_NUM = 14; //һ��Ŀ¼�������14������
const int MAX_NAME_LEN = 14; //�ļ����ļ��������Ϊ14�ֽ�
const int MAX_TIME_LEN = 16; //�ļ�(��)�Ľ���ʱ�䣬16�ֽ�

//i�ڵ�
struct BFD_dir{  // Ŀ¼���ļ�  һ��i�ڵ�64�ֽ�

    short blkNum; // һ��dir��ռ����
    short subDir; // ������������Ŀ¼��Ϊ�ļ�ʱΪ0
    short byteNum; // ������ռ�ֽ���
    bool  isFile; //Ŀ¼�Ƿ�Ϊ�ļ�
    short ownerID; //�����߱��      2*4+1 = 9 bytes
    short block[MAX_ITEM_NUM]; //
    char time[MAX_TIME_LEN];
    bool  access[MAX_USER_NUM]; //�û�Ȩ��
    //
};

//Ŀ¼��
struct SFD_item{   // һ��Ŀ¼��16�ֽ�
    short blockID;
    char name[MAX_NAME_LEN];

};
#endif //OSV4_DIRECTORY_H
