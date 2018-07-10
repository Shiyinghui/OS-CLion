//
// Created by 24757 on 2018/7/7.
//
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <conio.h>
#include "virtualDisk.h"
#include "dirPath.h"
#include "function.h"

using namespace std;
extern User user;
extern SFD_item tempSFD;
extern vector<User> users;
extern leadingBlk super;

// 获取时间函数
string getTime()
{
    time_t timeP;
    time (&timeP);
    char tmp[64];
    char res[16];
    int i = 0;
    strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S",localtime(&timeP));
    for(;i < 16;i++)
        res[i] = tmp[i];
    res[i] = '\0';
   // cout<<"当前时间:"<<res<<endl;
    return res;
}

//用户登陆函数
int login(){
    string name,pwd;
    cout << "用户名：";
    getline(cin,name);
    cout << "密码：";
    getline(cin,pwd);
    for(int i = 0; i< users.size(); i++) {
        if(!strcmp(name.c_str(), users[i].name) &&
           !strcmp(pwd.c_str(), users[i].password)) {
            cout << "登陆成功!" << endl;
            return users[i].id;
        }
    }
    cout << "用户名或密码输入有误" << endl;
    return -1;
}

//用户注册函数
int Register(){
    string name,pwd,email;
    bool ok = false;
    char input[14],temp[7];
    if((int)users.size() == MAX_USER_NUM)
        cout << "已注册用户数量达到系统上限，无法注册！" << endl;
    cout << "请输入用户名(24位以内)：";
    getline(cin,name);
    if((int)name.length() > NAME_USER_LEN)
        cout << "用户名过长，注册失败" <<endl;
    else {
        bool exist = false;
        for(int i = 0; i < users.size(); i++) {
            if(!strcmp(name.c_str(), users[i].name)) {
                exist = true;
                break;
            }
        }
        if(exist)
            cout << "系统中已存在相同的用户名" << endl;
        else {
            cout << "请输入密码(24位以内)：";
            getline(cin,pwd);
            if((int)pwd.length() > PWD_USER_LEN)
                cout << "密码过长，注册失败"<<endl;
            else {
                cout<<"请输入邮箱(14位以内)："<<endl;
                getline(cin,email);
                if((int)email.length() > MAIL_USER_LEN)
                    cout << "邮箱过长，注册失败"<<endl;
                else{
                    User user;
                    strcpy(user.name, name.c_str());
                    strcpy(user.password, pwd.c_str());
                    strcpy(user.email,input);
                    user.id = users.size();
                    users.push_back(user);
                    blkWrite(TOTAL_BLOCK+user.id, &user);
                    cout << "注册成功，已自动登录" << endl;
                    return user.id;
                }

            }
        }
    }
    return -1;
}

//初始化函数 init
void init(){

    FILE *file = fopen("virtualDisk.txt","w");
    fclose(file);
    blkWrite(0, &user);

    // leader=1000-31 = 969;
    // 最后一组组长块的编号 969，970~999(30块)
    short leader = TOTAL_BLOCK - GROUP_BLK_NUM;
    for (; leader >= 3; leader -= GROUP_BLK_NUM){
        leadingBlk l;
        l.blkNum = GROUP_BLK_NUM - 1;   // l.blkNum = 30
             //   cout<<"块数量:"<<l.blkNum<<endl;

        l.nextLeadingBlk = leader + GROUP_BLK_NUM; //第一次循环，下一个组长块 1000
             //   cout<<"下一块："<<l.nextLeadingBlk<<endl;
        for (short i = 0; i < l.blkNum; i++) {
            l.member[i] = leader + i + 1;
          //  cout<< l.member[i]<<" ";
        }
        blkWrite(leader, &l); //将初始化完成的组长块内容写入相应磁盘块
       // cout<<leader<<endl;
    }
    //结束循环：leader = 8-31=-23; 第一组组长块：8

    //初始化超级块
    leadingBlk super;
    leader += GROUP_BLK_NUM;   // leader = -23+31=8;
    super.blkNum = leader - 3; //除去第0、1、2块后还剩余的块数 5
    super.nextLeadingBlk = leader; // 下一组长块 8
    for (int i = 0; i < super.blkNum; i++) {
        super.member[i] = 3 + i; //(3,4,5,6,7)
       // cout<<endl;
       // cout<<super.member[i]<<" ";
    }
    blkWrite(1, &super); //把超级块写进磁盘块

    //初始化根目录
    BFD_dir base;          //单独拿出一块，2
    base.isFile = false;
    base.blkNum = 0;
    base.subDir = 0;
    base.byteNum = 0;
    base.ownerID = -1;
    for(int i = 0; i < MAX_USER_NUM; i++)
        base.access[i] = true;  //所有用户均可访问根目录
    blkWrite(2, &base); // 把根目录i节点写进磁盘块

}

// 用户查看当前目录下自己有权限的文件(文件夹)
void access(){
    BFD_dir dir;
    vector<BFD_dir> dirV;
    vector<SFD_item> itemV;
    string desc;
    short blockID = -1;
    blockID = currentPath.top().blockID;
    blkRead(blockID, &dir);
    for(int i = 0,k = 0; i < dir.blkNum;i++){
        SFD_item temp[ITEM_OF_BLK];
        blkRead(dir.block[i], temp);
        for(int j = 0; j<ITEM_OF_BLK && k<dir.subDir;j++,k++){
            BFD_dir tempDir;
            blkRead(temp[j].blockID, &tempDir);
            if(tempDir.access[user.id]){
                dirV.push_back(tempDir);
                itemV.push_back(temp[j]);
            }
        }
    }
    if(dirV.size() == 0)
        cout<<"当前目录下您没有访问任何一个文件夹的权限"<<endl;
    else{
        printf("\t\t");
        cout << setw(14) << setiosflags(ios::left) << "items";
        cout << setw(8) << setiosflags(ios::left) << "type";
        cout << setw(10) << setiosflags(ios::left) << "bytes";
        cout << setw(14) << setiosflags(ios::left) << "owner";
        cout << setw(20) << setiosflags(ios::left) << "date" << endl;
        printf("\t\t");
        cout << "-------------------------------------------------------------"<< endl;
        for(int i = 0; i < dirV.size();i++){
            if(dirV[i].isFile) desc = "file";
            else desc = "folder";
            string tempT;
            for(int index = 0; index < 16;index++)
                tempT = tempT+ dirV[i].time[index];
            printf("\t\t");
            cout << setw(14) << setiosflags(ios::left) << itemV[i].name;
            cout << setw(8) << setiosflags(ios::left) << desc;
            cout << setw(10) << setiosflags(ios::left) << dirV[i].byteNum;
            cout << setw(14) << setiosflags(ios::left) << users[dirV[i].ownerID].name;
            cout << setw(20) << setiosflags(ios::left) << tempT<< endl;
        }
    }
}

// 1. 格式化函数format
void format(){
   string cmd;
   cout<<"警告：格式化将清空磁盘中的所有信息"<<endl;
   cout<<"您确定要格式化吗？(Y/N)"<<endl;
   getline(cin,cmd);
   if(cmd == "y" || cmd=="Y"){
       init();
       cout<<"已成功格式化磁盘"<<endl;
   }
   else cout<<"已取消格式化"<<endl;
}

//2. 进入系统函数enter
User enter() {
    string cmd,name,pwd;
    int ok = -1;
    for(;;) {
        printf("\t\t");
        cout<<"---------欢迎使用文件管理系统---------"<<endl;
        printf("\t\t\t");
        cout << "      1、登录" << endl;
        printf("\t\t\t");
        cout << "      2、注册" << endl;
        printf("\t\t\t");
        cout << "      3、退出" << endl;
        cout << "请输入1或2或3以选择相应操作：" << endl;
        getline(cin,cmd);
        if(cmd == "1") {
              ok = login();
              if(ok >= 0) return users[ok];
              else continue;
        } else if(cmd == "2") {
              ok = Register();
              if(ok >= 0) return users[ok];
              else continue;
        } else if(cmd == "3") {
            blkWrite(1,&super);
            exit(0);
        } else {
            cout << "输入有误，请输入1或2或3" << endl;
            continue;
        }
    }
}


//3. 新建文件或文件夹函数 newItem
void newItem(string str, string itemName) {
    const char *name = itemName.c_str();
    if ((int)itemName.length() > MAX_NAME_LEN)
        cout << "名字过长，文件(夹)名最大长度为14位" << endl;
    else if(exist(currentPath.top().blockID, name))
        cout << "当前目录已存在名为" <<itemName<< "的文件(夹)" << endl;
    else {
        addItem(currentPath.top().blockID, name, str == "newFile");
        cout << "新建成功" << endl;
    }

}

//4. 删除文件或文件夹
void del(string itemName) {
    const char *name = itemName.c_str();

   /* BFD_dir temp; /////////  输出调试
    blkRead(currentPath.top().blockID,&temp); //////////
    cout<<"当前目录所占字节数："<<temp.byteNum<<endl;//////////
*/
    if((int)itemName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name))
        cout << "当前目录下不存在这样的文件(夹)" << endl;
    else {
        SFD_item it = findAItem(currentPath.top().blockID, name);
        BFD_dir dir;
        blkRead(it.blockID, &dir);
        if(!dir.access[user.id]){
            cout<<"您没有权限删除此文件(夹)!"<<endl;
            return;
        }
        int freeBytes = dir.byteNum;
        //cout<<"freeBytes:"<<freeBytes<<endl;
        if(deleteItem(currentPath.top().blockID, name)) {
           // cout<<"Path:"<<currentPath.index<<endl;
            for(int i = currentPath.index-2; i >= 0; i--) {
                BFD_dir tmp;
                blkRead(currentPath.p[i].blockID, &tmp);
                //cout<<"当前目录"<<i<<"字节数："<<tmp.byteNum<<endl;
                tmp.byteNum -= freeBytes;
                //cout<<"删除后当前目录"<<i<<"字节数："<<tmp.byteNum<<endl;
                blkWrite(currentPath.p[i].blockID, &tmp);
            }
         /*   BFD_dir temp1;
            blkRead(currentPath.top().blockID,&temp1);*/
           // cout<<"删除后当前目录所占字节数："<<temp1.byteNum<<endl; /////////////
           /* cout<<"删除信息："<<endl;
            cout<<"   文件所在块："<<it.blockID<<"   释放字节："<<freeBytes<<endl;*/
            cout << "删除成功" << endl;
        }
    }
}

//5. 显示当前目录的内容
void ls() {

    int id = currentPath.top().blockID;  //
    BFD_dir dir;
    blkRead(id, &dir);
    if(dir.subDir == 0)
        cout<<"此目录下没有文件或文件夹！"<<endl;
    if(dir.subDir > 0) {
        printf("\t\t");
        cout << setw(14) << setiosflags(ios::left) << "items";
        cout << setw(8) << setiosflags(ios::left) << "type";
        cout << setw(10) << setiosflags(ios::left) << "bytes";
        cout << setw(14) << setiosflags(ios::left) << "owner";
        cout << setw(16) << setiosflags(ios::left) << "date" << endl;
        printf("\t\t");
        cout << "---------------------------------------------------------------"<< endl;

        for (int i = 0, k = 0; i < dir.blkNum; i++) {
            SFD_item a[ITEM_OF_BLK];    //
            blkRead(dir.block[i], a);
            for (int j = 0; j < ITEM_OF_BLK && k < dir.subDir; j++,k++) {
                BFD_dir temp;
                string desc = "",tempT;
                blkRead(a[j].blockID, &temp);
                if (temp.isFile) desc = "file";
                else desc = "folder";
                for(int index = 0; index < 16;index++)
                    tempT = tempT+ temp.time[index];
                printf("\t\t");
                cout << setw(14) << setiosflags(ios::left) << a[j].name;
                cout << setw(8) << setiosflags(ios::left) << desc;
                cout << setw(10) << setiosflags(ios::left) << temp.byteNum;
                cout << setw(14) << setiosflags(ios::left) << users[temp.ownerID].name;
                cout << setw(16) << setiosflags(ios::left) << tempT << endl;
            }
        }
    }
}

//6. 切换目录函数
void cd(string itemName) {
    const char *name = itemName.c_str();
    if ((int)itemName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)){
        cout << "当前目录下不存在此文件(夹)，不能切换" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if (dir.isFile){
        cout << "不能进入文件，只能进入文件夹" << endl;
        return;
    }
    if(!dir.access[user.id]) {
        cout << "您没有权限访问此文件夹" << endl;
        return;
    }
    currentPath.push(it);
}

//7. 写文件函数
void write(string fileName) {

    /*BFD_dir temp; /////////
    blkRead(currentPath.top().blockID,&temp); //////////
    cout<<"当前目录所占字节数："<<temp.byteNum<<endl;//////////*/

    char content[BLOCK_SIZE*MAX_ITEM_NUM];
    char s[BLOCK_SIZE*MAX_ITEM_NUM];
    const char *name = fileName.c_str();
    int i = 0, j = 0;
    if((int)fileName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)){
        cout << "当前目录下不存在此文件" << endl;
    } else {
        SFD_item it = findAItem(currentPath.top().blockID, name);
        BFD_dir dir;
        blkRead(it.blockID, &dir);
        if(!dir.isFile){
            cout << "这是文件夹，不能往文件夹中写内容" << endl;
            return;
        }
        if(!dir.access[user.id]) {
            cout << "您没有权限写此文件" << endl;
            return;
        }
        cout << "请输入要写入的内容(按#键结束)：" << endl;
        readFile(it.blockID, content);
        int len = strlen(content);
        for(i = 0;i < len;i++){
            putchar(content[i]);
            s[i] = content[i];
        }
        j = i;
        s[j] = getch();
        while(s[j]!='#') {
            if (s[j] == 0x08)
            {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                j--;
            }
           else if(s[j] == '\r') s[j] = '\n';
            else printf("%c",s[j++]);
            s[j] = getch();
        }
        s[j] = '\0';
        int beforeSize = dir.byteNum; // 文件原字节数
        //cout<<"文件原字节数："<<dir.byteNum<<endl;
      //  cout<<strlen(s)<<endl;
        writeFile(it.blockID, s);
        blkRead(it.blockID, &dir);
        int afterSize = dir.byteNum;
       // cout<<"文件现字节数："<<dir.byteNum<<endl; ////
        int diff = afterSize - beforeSize;
        for(int i = currentPath.index-1; i >= 0; i--) {
            BFD_dir tmp;
            blkRead(currentPath.p[i].blockID, &tmp);
            tmp.byteNum += diff;
            blkWrite(currentPath.p[i].blockID, &tmp);
        }
        cout<<"写入成功！"<<endl;

       /* BFD_dir temp2; /////////
        blkRead(currentPath.top().blockID,&temp2); //////////
        cout<<"当前目录所占字节数："<<temp2.byteNum<<endl;//////////*/

    }
}

//8. 读取文件内容函数
void read(string fileName) {
    const char *name = fileName.c_str();
    if ((int)fileName.length() > MAX_NAME_LEN|| !exist(currentPath.top().blockID, name)){
        cout << "当前目录下不存在此文件(夹)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if (!dir.isFile){
        cout << fileName << "不是文件，无法读取" << endl;
        return;
    }
    if(!dir.access[user.id]) {
        cout << "您没有权限读此文件" << endl;
        return;
    }
    char content[BLOCK_SIZE*30];
    readFile(it.blockID, content);
    if(strlen(content) == 0)
        cout<<"文件内容为空"<<endl;
    else{
        cout<<"文件内容为："<<endl;
        cout << content << endl;
    }
}

//9. 重命名函数
void rename(string oldName, string newName) {
    char name[MAX_NAME_LEN];
    strcpy(name, oldName.c_str());
    if((int)oldName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "当前目录下不存在此文件(夹)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id]) {
        cout << "您没有权限进行重命名操作" << endl;
        return;
    }
    if((int)newName.length() > MAX_NAME_LEN) {
        cout << "新名称过长，最大长度为" << MAX_NAME_LEN << endl;
        return;
    }
    if(exist(currentPath.top().blockID, newName.c_str())) {
        cout << "重命名失败，存在文件与新的名称同名！" << endl;
        return;
    }
    bool ok = 0;
    blkRead(currentPath.top().blockID, &dir);
    for(int i = 0, k = 0; i < dir.blkNum; i++) {
       SFD_item  temp[ITEM_OF_BLK];
        blkRead(dir.block[i], temp);
        for(int j = 0 ; j < ITEM_OF_BLK && k < dir.subDir; j++, k++) {
            if(!strcmp(name, temp[j].name)) {
                strcpy(temp[j].name, newName.c_str());
                ok = 1;
                break;
            }
        }
        if(ok) {
            blkWrite(dir.block[i], temp);
            break;
        }
    }
    cout << "重命名成功" << endl;
}

//10. 授予权限函数
void grant(string fName, string userName) {
    const char *name = fName.c_str();
    const char *uName = userName.c_str();
    bool ok = false;
    int index = -1;
    if((int)fName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "当前目录下不存在此文件(夹)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id]) {
        cout << "您没有权限进行授权操作" << endl;
        return;
    }
    if(!strcmp(uName,user.name)){
        cout<<"您是文件(夹)的创建者，无须授权给自己！"<<endl;
        return;
    }
    for(int i = 0;i<users.size();i++){
        if(!strcmp(users[i].name,uName)){
            ok = true;
            index = i;
        }
    }
    if(!ok) {
        cout<<"输入的用户不存在！"<<endl;
        return;
    }
    dir.access[index] = true;
    blkWrite(it.blockID, &dir);
    cout << "成功将文件的权限授予此用户" << endl;
}

// 11. 收回权限函数
void revoke(string fName, string userName) {
    const char *name = fName.c_str();
    const char *uName = userName.c_str();
    bool ok = false;
    int index = -1;
    if((int)fName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "当前目录下不存在此文件(夹)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id] || user.id != dir.ownerID) {
        cout << "您没有权限进行收回权限操作" << endl;
        return;
    }
    for(int i = 0;i<users.size();i++){
        if(!strcmp(users[i].name,uName)){
            ok = true;
            index = i;
        }
    }
    if(!ok) {
        cout<<"输入的用户不存在！"<<endl;
        return;
    }
    if(users[index].id == dir.ownerID){
        cout<<"您是文件(夹)的创建者，不能收回自己的权限！"<<endl;
        return;
    }
    else if(!dir.access[index]){
        cout<<"此用户没有访问文件(夹)的权限，无须收回！"<<endl;
        return;
    }
    else{
        dir.access[index] = false;
        blkWrite(it.blockID, &dir);
        cout << "成功收回权限" << endl;
    }

}

//12. 复制函数
void copy(string fName) {
    const char *name = fName.c_str();
    if((int)fName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "当前目录下不存在此文件" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.isFile){
        cout<<"注意：不能粘贴文件夹！"<<endl;
        return;
    }
    if(!dir.access[user.id]) {
        cout << "您没有权限" << endl;
        return;
    }
    tempSFD = it;
    cout << "复制成功" << endl;
}

//13. 粘贴函数
void paste() {
    if(tempSFD.blockID == -1) {
        cout << "没有可粘贴的文件" << endl;
        return;
    }
    if(exist(currentPath.top().blockID, tempSFD.name)) {
        cout << "当前目录下存在同名文件(夹)" << endl;
        return;
    }
    BFD_dir dir;
    blkRead(tempSFD.blockID, &dir);  //把要粘贴的文件所在的BFD读到dir中
    addItem(currentPath.top().blockID, tempSFD.name, dir.isFile); // 在要粘贴的目录下新建一个文件
    SFD_item newItem = findAItem(currentPath.top().blockID, tempSFD.name); // 取出此粘贴的文件

    char text[BLOCK_SIZE*MAX_ITEM_NUM];
    readFile(tempSFD.blockID, text);  // 读出原文件中的内容
    writeFile(newItem.blockID,text);  // 写入新的文件中
    int newBytes = dir.byteNum;

    for(int i = currentPath.index-1; i >= 0; i--) {
        BFD_dir temp;
        blkRead(currentPath.p[i].blockID, &temp);
        temp.byteNum += newBytes;  //目录的字节变化
        blkWrite(currentPath.p[i].blockID, &temp);
    }
    cout << "粘贴成功" << endl;
}

//14. 查看文件详细信息函数
void details(string fname) {
    const char *name = fname.c_str();
    SFD_item it = findAItem(currentPath.top().blockID, name);
    if(it.blockID == -1) {
        cout << "当前目录下不存在此文件(夹)" << endl;
        return;
    }
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id]) {
        cout << "您没有权限查看文件的详细信息" << endl;
        return;
    }
    cout << "类型：" << (dir.isFile? "文件": "文件夹") <<"  ";
    cout <<"包含子项数："<<dir.subDir<<"  ";
    cout << "所占块数：" << dir.blkNum <<"  ";
    cout << "所占字节大小：" << dir.byteNum <<"  ";
    cout << "所有者：" << users[dir.ownerID].name << endl;
    cout<<"拥有权限的用户：";
    for(int i = 0; i < users.size(); i++){
        if(dir.access[users[i].id])
            cout<<users[i].name<<" ";
    }
    cout<<endl;
}

