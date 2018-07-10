//
// Created by 24757 on 2018/7/5.
//

#ifndef OSV4_DIRECTORY_H
#define OSV4_DIRECTORY_H
const int MAX_USER_NUM = 9;  //最大用户数量
const int MAX_ITEM_NUM = 14; //一个目录下最多有14个子项
const int MAX_NAME_LEN = 14; //文件或文件夹名称最长为14字节
const int MAX_TIME_LEN = 16; //文件(夹)的建立时间，16字节

//i节点
struct BFD_dir{  // 目录或文件  一个i节点64字节

    short blkNum; // 一个dir所占块数
    short subDir; // 包含子项数，目录项为文件时为0
    short byteNum; // 数据所占字节数
    bool  isFile; //目录是否为文件
    short ownerID; //创建者编号      2*4+1 = 9 bytes
    short block[MAX_ITEM_NUM]; //
    char time[MAX_TIME_LEN];
    bool  access[MAX_USER_NUM]; //用户权限
    //
};

//目录项
struct SFD_item{   // 一个目录项16字节
    short blockID;
    char name[MAX_NAME_LEN];

};
#endif //OSV4_DIRECTORY_H
