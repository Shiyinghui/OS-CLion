//
// Created by 24757 on 2018/6/30.
//
#ifndef OS_DISK_H
#define OS_DISK_H
const int GRUOP_NUM = 30; //һ�����
const int DIR_NUM = 20; //һ��Ŀ¼��Ŀ���
const int NAME_LEN = 14; //�ļ����ļ���������󳤶�

const int U_MAX_NUM = 15; //����û�����
const int U_NAME_LEN = 31; //����û�������
const int U_PWD_LEN = 31; //������볤��

const char DISK[] = "disk.txt";
const int BlockCnt = 1024; //�ڴ����
const int BlockCap = 64; //ÿ���ڴ��С
const int BlockPerGroup = 31; //ÿ���ڴ����
const int ItemPerBlock = 4; //ÿ���ϵ�������

struct User {
    char name[U_NAME_LEN]; //�û���
    char password[U_PWD_LEN]; //����
    short id; //���
};

struct Leader { //�鳤��
    short blockNum; //���ڿ���
    short nextLeader;  //��һ���鳤���
    short member[GRUOP_NUM]; //GRUOP_NUM����Ա����
};

//BFD
struct Dir { //Ŀ¼���ļ�����
    short blockNum; //��ռ����
    short itemNum; //������������ֻ�е�Ŀ¼��Ϊ�ļ���ʱ��Ч
    short byteNum; //������ռ�ֽ���
    short authorID; //�����û����
    short member[DIR_NUM]; //DIR_NUM����ռ����
    bool isFile; //�Ƿ�Ϊ�ļ�
    bool pri[U_MAX_NUM]; //pri[i]Ϊtrue��ʾ���Ϊi���û�ӵ�з���Ŀ¼���Ȩ�ޣ���֮��û��
};

//SFD
struct Item { //Ŀ¼��
    char name[NAME_LEN]; //Ŀ¼������
    short blockID; //Ŀ¼�����ڿ���
};

struct Block { //��
    char byte[BlockCap];
};

extern User user;
extern Leader super;
extern void readBlock(short, void *);
extern void writeBlock(short, void *);
extern int allocBlock();
extern void freeBlock(short);
extern Item getItem(short, const char *);
extern int addItem(short, const char *, bool);
extern void deleteFolder(short);
extern bool deleteItem(short, const char *);
extern bool exist(short, const char *);
extern void readFile(short, char *);
extern void writeFile(short, const char *);
extern void deleteFile(short);
#endif //OS_DISK_H
