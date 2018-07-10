#include "disk.h"
#include "scanner.h"
#include "path.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <conio.h>
#include <vector>
#include <string>
#include <sstream>

//0name 1super 2root 3-1024use
using namespace std;

char cmds[][100] = {
        "logout", "登出当前用户",
        "ls", "显示当前目录下的全部子项",
        "tree", "显示以当前目录为根的树形结构",
        "back", "返回上级目录",
        "home", "返回根目录",
        "format", "格式化磁盘",
        "exit", "退出系统",
        "help", "帮助",
        "newfile 文件名", "创建新文件",
        "newfolder 文件夹名", "创建新文件夹",
        "cd 文件夹名", "进入文件夹",
        "show 文件名", "显示文件内容",
        "del 文件或文件夹名 ", "删除当前目录下的文件或文件夹",
        "write 文件名", "往文件里写入内容",
        "property 文件或文件夹名", "查看详细属性",
        "grant 文件或文件夹名 用户编号", "授予用户权限",
        "copy 文件或文件夹名", "复制当前目录下的文件或文件夹",
        "paste", "将已复制内容粘贴到当前目录下",
        "rename 文件或文件夹名 新的名称", "重命名文件或文件夹",
        "#"
};

Leader super; //超级块
Path p; //当前路径
char cmd[1000]; //读取命令
vector<User> users; //当前用户组
User user; //当前用户
Item plate; //剪切板

void help() {
    char table[64];
    for (int i = 0; i < 64; i++)table[i] = '-';
    table[63] = 0;
    for (int i = 0; strcmp(cmds[i], "#"); i += 2){
        if (i == 0)printf("\t\t%s\n", table);
        printf("\t\t|%-30s|%-30s|\n", cmds[i], cmds[i + 1]);
        printf("\t\t%s\n", table);
    }
}

void format() {
    cout << "格式化将会清空全部文件及文件夹" << endl;
    cout << "您确定要清空吗?(Y/N)" << endl;
    gets(cmd);          //读取命令
    Scanner sc(cmd);
    string str = sc.next();
    if(str == "y" || str == "Y") {
        FILE *file = fopen(DISK, "w");//DISK文件存在则会覆盖,没有文件就创建文件
        fclose(file);

        //初始化引导块
        writeBlock(0, &user);

        //初始化所有组长块
        int leader = BlockCnt - BlockPerGroup; //leader初始化为最后一块组长块的编号
                 // 1024 - 31
        for (; leader >= 3; leader -= BlockPerGroup){
            Leader l;
            l.blockNum = BlockPerGroup - 1;   // l.blockNum = 30
            l.nextLeader = leader + BlockPerGroup;
            for (int i = 1; i <= l.blockNum; i++) l.member[i] = leader + i;
            writeBlock(leader, &l); //将初始化完成的组长块内容写入相应内存
        }
        //循环结束后leader为第一组长块编号-BlockPerGroup   leader = 33-31=2;

        //初始化超级块
        Leader super;
        leader += BlockPerGroup;   // leader = 2+31=33;
        super.blockNum = leader - 3; //除去第0、1、2块后还剩余的块数  30
        super.nextLeader = leader; //超级块的初始下一组长块即为第一组长块
        for (int i = 0; i < super.blockNum; i++) super.member[i] = 3 + i;
        writeBlock(1, &super); //将初始化完成的超级块内容写入第一块内存

        //初始化根目录
        Dir root;          //单独拿出一块，2
        root.isFile = false;
        root.blockNum = 0;
        root.itemNum = 0;
        root.byteNum = 0;
        root.authorID = -1;
        for(int i = 0; i < U_MAX_NUM; i++) root.pri[i] = true;

        writeBlock(2, &root); //将初始化完成的根目录写入第二块内存

        cout << "格式化成功" << endl;
    } else {
        cout << "格式化操作取消" << endl;
    }
}

void ls() {
    int id = p.top().blockID;  // id = 2
    cout<<id<<endl;
    Dir dir;
    readBlock(id, &dir);
    const int len = 28;
    char table[len];
    cout << p.path() << endl;
    for (int i = 0; i < len; i++) table[i] = '-';
    table[len - 1] = 0;
    cout << table << endl;

    for (int i = 0, k = 0; i < dir.blockNum; i++){
        Item a[ItemPerBlock];    // Item a[4]
        readBlock(dir.member[i], a);
        for (int j = 0; j < ItemPerBlock && k < dir.itemNum; j++, k++){
            Dir temp;
            readBlock(a[j].blockID, &temp);
            printf("| %-10s | %-10s |\n", a[j].name, temp.isFile? "文件": "文件夹");
        }
    }
    cout << table << endl;
}

void del(string fname) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name))
        cout << "不存在这样的文件或文件夹" << endl;
    else {
        Item it = getItem(p.top().blockID, name);
        Dir dir;
        readBlock(it.blockID, &dir);
        int bytechange = dir.byteNum;
        if(deleteItem(p.top().blockID, name)) {
            for(int i = p.cnt-2; i >= 0; i--) {
                Dir tmp;
                readBlock(p.p[i].blockID, &tmp);
                tmp.byteNum -= bytechange;
                writeBlock(p.p[i].blockID, &tmp);
            }
            cout << "删除成功" << endl;
        }
    }
}

void newff(string str, string fname) {
    const char *name = fname.c_str();
    if ((int)fname.length() > NAME_LEN) cout << "文件或文件夹名太长" << endl;
    else if(exist(p.top().blockID, name)) cout << "已经存在名为" << fname << "的文件或文件夹了" << endl;
    else {
        addItem(p.top().blockID, name, str == "newfile");
        cout << "新建成功" << endl;
    }
}

void cd(string fname) {
    const char *name = fname.c_str();
    if ((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)){
        cout << "不存在这样的文件" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "没有权限" << endl;
        return;
    }
    if (dir.isFile){
        cout << "不能进入文件,请输入文件夹" << endl;
        return;
    }
    p.push(it);
}

void show(string fname) {
    const char *name = fname.c_str();
    if ((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)){
        cout << "不存在这样的文件" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "没有权限" << endl;
        return;
    }
    if (!dir.isFile){
        cout << fname << "不是文件" << endl;
        return;
    }
    char content[BlockCap*30];
    readFile(it.blockID, content);
    cout << content << endl;
}

string replace(string str, string old, string news) { //将str中出现的old子串全部替换成news子串并返回替换后的串
    string res;
    int strl = str.length(), oldl = old.length();
    for (int i = 0; i < strl;) {
        int j;
        for (j = 0; j < oldl && i + j < strl && str[i+j] == old[j]; j++);
        if (j == oldl) { //找到一个old子串
            res += news;
            i += j;
        } else{
            res += str[i];
            i++;
        }
    }
    return res;
}

void tree(short id, const char *name, bool isFile, string prefix, string &treeString) { //显示目录的树形结构
    treeString += prefix + name + "\n";
    if (isFile) return; //当前是文件，则递归结束
    string temp = replace(prefix, "┣", "┃");
    temp = replace(temp, "━", "  ");
    temp = replace(temp, "┗", "  ");
    Dir dir;
    readBlock(id, &dir);
    for (int i = 0, k = 0; i < dir.blockNum; i++){
        Item a[ItemPerBlock]; readBlock(dir.member[i], a);
        for (int j = 0; j < ItemPerBlock && k < dir.itemNum; j++, k++) {
            Dir dir;
            readBlock(a[j].blockID, &dir);
            if (k == dir.itemNum-1) tree(a[j].blockID, a[j].name, dir.isFile, temp + "┗━", treeString);
            else tree(a[j].blockID, a[j].name, dir.isFile ,temp + "┣━", treeString);
        }
    }
}

void write(string fname) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)){
        cout << "不存在这样的文件" << endl;
    } else {
        Item it = getItem(p.top().blockID, name);
        Dir dir;
        readBlock(it.blockID, &dir);
        if(!dir.pri[user.id]) {
            cout << "没有权限" << endl;
            return;
        }
        if(!dir.isFile){
            cout << "不能往文件夹中写内容" << endl;
            return;
        }
        cout << "请输入内容(按esc键结束)：" << endl;
        char s[BlockCap*20];//20
        int cnt = 0;
        while((s[cnt] = getch()) != 27) {
            if(s[cnt] == '\b') {
                printf("\b \b");
                cnt--;
                continue;
            }
            if(s[cnt] == '\r') s[cnt] = '\n';
            printf("%c", s[cnt++]);
        }
        s[cnt] = '\0';

        int bytechange = dir.byteNum;

        writeFile(it.blockID, s);
        readBlock(it.blockID, &dir);
        bytechange -= dir.byteNum;
        for(int i = p.cnt-1; i >= 0; i--) {
            Dir tmp;
            readBlock(p.p[i].blockID, &tmp);
            tmp.byteNum -= bytechange;
            writeBlock(p.p[i].blockID, &tmp);
        }
    }
}

User login() {
    for(;;) {
        cout << "请输入相应的选项来进行操作" << endl;
        cout << "1、登录" << endl;
        cout << "2、注册" << endl;
        cout << "3、退出" << endl;
        gets(cmd); Scanner sc(cmd);
        string str = sc.next(), name, pwd;
        if(str == "1") {
            cout << "用户名：";
            gets(cmd); sc = Scanner(cmd);
            name = sc.next();
            cout << "密码：";
            gets(cmd); sc = Scanner(cmd);
            pwd = sc.next();
            for(int i = 0, sz = users.size(); i < sz; i++) {
                if(!strcmp(name.c_str(), users[i].name) &&
                   !strcmp(pwd.c_str(), users[i].password)) {
                    cout << "登陆成功" << endl;
                    return users[i];
                }
            }
            cout << "用户名或密码错误" << endl;
        } else if(str == "2") {
            if((int)users.size() == U_MAX_NUM) {
                cout << "用户数量已满，无法注册新用户" << endl;
                continue;
            }
            cout << "用户名：";
            gets(cmd); sc = Scanner(cmd);
            name = sc.next();
            if((int)name.length() > U_NAME_LEN) cout << "用户名过长，最大长度为" << U_NAME_LEN << endl;
            else {
                bool ok = 1;
                for(int i = 0, sz = users.size(); i < sz; i++) {
                    if(!strcmp(name.c_str(), users[i].name)) {
                        ok = 0;
                        break;
                    }
                }
                if(!ok) cout << "该用户名已存在" << endl;
                else {
                    cout << "密码：";
                    gets(cmd); sc = Scanner(cmd);
                    pwd = sc.next();
                    if((int)pwd.length() > U_PWD_LEN) cout << "密码过长，最大长度为" << U_PWD_LEN << endl;
                    else {
                        cout << "确认密码：";
                        gets(cmd); sc = Scanner(cmd);
                        str = sc.next();
                        if(str == pwd) {
                            User user;
                            strcpy(user.name, name.c_str());
                            strcpy(user.password, pwd.c_str());
                            user.id = users.size();
                            users.push_back(user);
                            writeBlock(BlockCnt+user.id, &user);
                            cout << "注册成功，已自动登录" << endl;
                            return user;
                        } else {
                            cout << "确认密码不一致" << endl;
                        }
                    }
                }
            }
        } else if(str == "3") {
            exit(0);
        } else {
            cout << "命令格式错误" << endl;
        }
    }
}

void property(string fname) {
    const char *name = fname.c_str();
    Item it = getItem(p.top().blockID, name);
    if(it.blockID == -1) {
        cout << "文件或文件夹不存在" << endl;
        return;
    }
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "没有权限" << endl;
        return;
    }
    cout << "类型：" << (dir.isFile? "文件": "文件夹") << endl;
    cout << "所占块数：" << dir.blockNum << endl;
    cout << "所占字节大小：" << dir.byteNum << endl;
    cout << "创建用户编号：" << dir.authorID << endl;
}

void grant(string fname, string id) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)) {
        cout << "不存在这样的文件" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "没有权限" << endl;
        return;
    }
    stringstream ss(id);
    int userid; ss >> userid;
    if(userid < 0 || userid >= (int)users.size()) {
        cout << "不存在的用户" << endl;
        return;
    }
    dir.pri[userid] = true;
    writeBlock(it.blockID, &dir);
    cout << "授予权限成功" << endl;
}

void rename(string oldname, string newname) {
    char name[NAME_LEN];
    strcpy(name, oldname.c_str());
    if((int)oldname.length() > NAME_LEN || !exist(p.top().blockID, name)) {
        cout << "不存在这样的文件" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "没有权限" << endl;
        return;
    }
    if((int)newname.length() > NAME_LEN) {
        cout << "新名称过长，最大长度为" << NAME_LEN << endl;
        return;
    }
    if(exist(p.top().blockID, newname.c_str())) {
        cout << "新名称已存在" << endl;
        return;
    }
    bool ok = 0;
    readBlock(p.top().blockID, &dir);
    for(int i = 0, k = 0; i < dir.blockNum; i++) {
        Item temp[ItemPerBlock];
        readBlock(dir.member[i], temp);
        for(int j = 0 ; j < ItemPerBlock && k < dir.itemNum; j++, k++) {
            if(!strcmp(name, temp[j].name)) {
                strcpy(temp[j].name, newname.c_str());
                ok = 1;
                break;
            }
        }
        if(ok) {
            writeBlock(dir.member[i], temp);
            break;
        }
    }
    cout << "重命名成功" << endl;
}

void cop(string fname) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)) {
        cout << "不存在这样的文件" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "没有权限" << endl;
        return;
    }
    plate = it;
    cout << "复制成功" << endl;
}

void paste() {
    if(plate.blockID == -1) {
        cout << "没有可粘贴的文件或文件夹" << endl;
        return;
    }
    if(exist(p.top().blockID, plate.name)) {
        cout << "存在同名文件或文件夹" << endl;
        return;
    }
    Dir dir;
    readBlock(plate.blockID, &dir);
    addItem(p.top().blockID, plate.name, dir.isFile);
    Item newit = getItem(p.top().blockID, plate.name);

    char content[BlockCap*20];
    readFile(plate.blockID, content);
    writeFile(newit.blockID, content);

    int bytechange = dir.byteNum;

    for(int i = p.cnt-1; i >= 0; i--) {
        Dir tmp;
        readBlock(p.p[i].blockID, &tmp);
        tmp.byteNum += bytechange;
        if(i < p.cnt-1) tmp.itemNum++;
        writeBlock(p.p[i].blockID, &tmp);
    }
    cout << "粘贴成功" << endl;
}

int main() {
    FILE *file = fopen(DISK, "r");
    //printf("%d******%d\n",0,'\0');
    users.clear();
    if(file == NULL) {
        cout << "磁盘不存在,将要新建磁盘并格式化" << endl;
        format();
    } else {
        cout << "读取磁盘成功" << endl;
        for(int i = 0; i < U_MAX_NUM; i++) {    //最大用户个数
            fseek(file, (BlockCnt+i)*BlockCap, 0); //开始偏移1024*64个字节
            User tmp;
            if(!fread(&tmp, BlockCap, 1, file)) break;
            users.push_back(tmp);
        }
        fclose(file);
    }
    readBlock(1, &super);
    p.home();
    plate.blockID = -1;

    user = login();
    writeBlock(0, &user);

    cout << "请您一定要注意,退出时要使用exit指令退出, 不要关闭控制台" << endl;
    cout << "因为程序需要用exit()做一些善后工作" << endl;
    cout << "请使用help命令查看帮助" << endl;
    while(cout << p.path() << ">", gets(cmd)) {
        Scanner sc(cmd);
        string str = sc.next();
        if(str == "help") help();
        else if(str == "ls") ls();
        else if(str == "home") p.home();
        else if(str == "back") p.pop();
        else if(str == "paste") paste();
        else if(str == "logout") {
            user = login();
            writeBlock(0, &user);
            p.home();
        } else if(str == "format") {
            format();
            for(int i = 0, sz = users.size(); i < sz; i++) writeBlock(BlockCnt+i, &users[i]);
            readBlock(1, &super);
            p.home();
            plate.blockID = -1;
        } else if(str == "tree") {
            Item it = p.top();
            string tr = "";
            cout << "---------------------------" << endl;
            tree(it.blockID, it.name, 0, "", tr);
            cout << tr << endl;
            cout << "---------------------------" << endl;
        } else if(str == "exit") {
            writeBlock(1, &super);
            break;
        } else {
            if(!sc.hasNext()) cout << "命令格式错误, 缺少文件或文件夹名" << endl;
            else if(str == "del") del(sc.next());
            else if(str == "newfile" || str == "newfolder") newff(str, sc.next());
            else if(str == "cd") cd(sc.next());
            else if(str == "show") show(sc.next());
            else if(str == "write") write(sc.next());
            else if(str == "copy") cop(sc.next());
            else if(str == "property") {
                if (!sc.hasNext()) cout << "命令格式错误" << endl;
                else property(sc.next());
            } else if(str == "grant") {
                str = sc.next();
                if (!sc.hasNext()) cout << "命令格式错误" << endl;
                else grant(str, sc.next());
            } else if(str == "rename") {
                str = sc.next();
                if (!sc.hasNext()) cout << "命令格式错误" << endl;
                else rename(str, sc.next());
            }
        }
    }
    return 0;
}


