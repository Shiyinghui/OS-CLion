#include <iostream>
#include <cstdio>
#include "virtualDisk.h"
#include "dirPath.h"
#include "function.h"
#include "command.h"
#include "dir.h"
using namespace std;

void help(){

    printf("\t\t");
    cout<<"命令格式";
    for(int i=0;i<18;i++)
        cout<<" ";
    cout<<"格式说明"<<endl;
    printf("\t\t");
    for(int i=0;i<80;i++)
    cout<<"-";
    cout<<endl;
    string tip[100]={
    "help",                    "帮助",
    "format",                  "格式化磁盘",
    "ls",                      "显示当前目录的内容",
    "logout",                  "退出当前帐号",
    "exit",                    "退出系统",
    "back",                    "返回上一级目录",
    "root",                    "回到根目录下",
    "access",                  "显示当前目录下用户有权限访问的文件或文件夹",
    "newFile fileName",        "在当前目录下新建一个名为fileName的文件",
    "newFolder folderName",    "在当前目录下新建一个名为folderName的文件夹",
    "del itemName",            "删除当前目录下名为itemName的文件或文件夹",
    "cd folderName",           "进入当前目录下名为folderName的文件夹",
    "read fileName",           "读取当前目录下名为fileName的文件内容",
    "write fileName",          "向当前目录下名为fileName的文件中写内容",
    "rename itemName",         "重命名当前目录下名为itemName的文件或文件夹",
    "details itemName",        "查看当前目录下名为itemName的文件或文件夹的详细信息",
    "copy fileName",           "复制当前目录下名为fileName的文件",
    "paste",                   "将复制的文件粘贴到当前目录下",
    "grant itemName userName", "将当前目录下名为itemName的文件或文件夹授权给userName",
    "revoke itemName userName","收回userName对当前目录下名为itemName的文件或文件夹",
    "END"
    };
    for(int i = 0;tip[i]!="END";i+=2){
        printf("\t\t");
        cout<<tip[i];
        for(int j=0;j<25-tip[i].length();j++)
            cout<<" ";
        cout<<tip[i+1]<<endl;
    }
    printf("\t\t");
    for(int i=0;i<80;i++)
        cout<<"-";
    cout<<endl;
}


// 全局变量
 leadingBlk super;
 User user;
 SFD_item tempSFD;
 vector<User> users;
 Path currentPath; //当前路径

int main() {
    FILE *file = fopen("virtualDisk.txt","r");
    char input[1000];
    if(file == NULL) {
        cout<<"此文件系统的磁盘不存在，将新建一个磁盘并初始化"<<endl;
        init();
        cout<<"已成功初始化磁盘"<<endl;
    }
    else{
        cout<<"读取磁盘成功，进入文件系统"<<endl;
        users.clear();
        for(int i=0;i<MAX_USER_NUM;i++){
            fseek(file,BLOCK_SIZE*(TOTAL_BLOCK+i),0);
            User tempUser;
            if(!fread(&tempUser,BLOCK_SIZE,1,file))break;
            users.push_back(tempUser);
            }
            fclose(file);
    }

    blkRead(1, &super);
    currentPath.root();
    tempSFD.blockID = -1;
    //tempSFD相当于一个临时变量区，初始化时没有任何文件被复制

    user = enter();   // 进入系统模块
    blkWrite(0, &user); // user的信息写到引导块，存放

    cout << "提示：可使用help命令查看帮助" << endl;
    while(cout<<currentPath.path()<<">",gets(input)){
        Command str(input);
        string cmd = str.next();
        if(cmd =="") { cout<<endl;continue;}
        if(cmd == "help") help();
        else if(cmd == "ls") ls();
        else if(cmd == "root") currentPath.root();
        else if(cmd == "access") access();
        else if(cmd == "back") currentPath.pop();
        else if(cmd == "paste") paste();
        else if(cmd == "logout") {
            user = enter();
            blkWrite(0, &user);
            currentPath.root(); // 再回到根目录下
        } else if(cmd == "format") {
            format();
            for(int i = 0, sz = users.size(); i < sz; i++) blkWrite(TOTAL_BLOCK+i, &users[i]);
            blkRead(1, &super);
            currentPath.root();
            tempSFD.blockID = -1;
        }  else if(cmd == "exit") {
            blkWrite(1, &super);
            break;
        } else {
             if(!str.hasNext()) cout << "此命令不存在，请检查大小写或命令格式" << endl;
             else if(cmd == "del") del(str.next());
             else if(cmd == "newFile" || cmd == "newFolder") newItem(cmd, str.next());
             else if(cmd == "cd") cd(str.next());
             else if(cmd == "read") read(str.next());
             else if(cmd == "write") write(str.next());
             else if(cmd == "details") details(str.next());
             else if(cmd == "copy") copy(str.next());
             //else if(cmd == "details") property(str.next());
             else if(cmd == "grant") {
               string itemName = str.next();
                if (!str.hasNext()) cout << "命令不完整，需输入要授予权限的用户名" << endl;
                else grant(itemName, str.next());
            }
             else if(cmd == "revoke") {
                 string itemName = str.next();
                 if (!str.hasNext()) cout << "命令不完整，需输入要收回权限的用户名" << endl;
                 else revoke(itemName, str.next());
             }
             else if(cmd == "rename") {
                string oldName = str.next();
                if (!str.hasNext()) cout << "命令不完整，需输入新的文件(夹)名" << endl;
                else rename(oldName, str.next());
            }
        }
    }
    return 0;
  }

