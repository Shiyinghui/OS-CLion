//
// Created by 24757 on 2018/6/30.
//
#ifndef OS_DISK_H
#define OS_DISK_H
const int GRUOP_NUM = 30; //一组块数
const int DIR_NUM = 20; //一个目录项的块数
const int NAME_LEN = 14; //文件或文件夹名的最大长度

const int U_MAX_NUM = 15; //最大用户个数
const int U_NAME_LEN = 31; //最大用户名长度
const int U_PWD_LEN = 31; //最大密码长度

const char DISK[] = "disk.txt";
const int BlockCnt = 1024; //内存块数
const int BlockCap = 64; //每块内存大小
const int BlockPerGroup = 31; //每组内存块数
const int ItemPerBlock = 4; //每块上的子项数

struct User {
    char name[U_NAME_LEN]; //用户名
    char password[U_PWD_LEN]; //密码
    short id; //编号
};

struct Leader { //组长块
    short blockNum; //组内块数
    short nextLeader;  //下一个组长编号
    short member[GRUOP_NUM]; //GRUOP_NUM个成员块编号
};

//BFD
struct Dir { //目录、文件共用
    short blockNum; //所占块数
    short itemNum; //包含子项数，只有当目录项为文件夹时有效
    short byteNum; //数据所占字节数
    short authorID; //创建用户编号
    short member[DIR_NUM]; //DIR_NUM个所占块编号
    bool isFile; //是否为文件
    bool pri[U_MAX_NUM]; //pri[i]为true表示编号为i的用户拥有访问目录项的权限，反之则没有
};

//SFD
struct Item { //目录项
    char name[NAME_LEN]; //目录项名称
    short blockID; //目录项所在块编号
};

struct Block { //块
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
