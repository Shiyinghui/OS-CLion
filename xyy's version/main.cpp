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
        "logout", "�ǳ���ǰ�û�",
        "ls", "��ʾ��ǰĿ¼�µ�ȫ������",
        "tree", "��ʾ�Ե�ǰĿ¼Ϊ�������νṹ",
        "back", "�����ϼ�Ŀ¼",
        "home", "���ظ�Ŀ¼",
        "format", "��ʽ������",
        "exit", "�˳�ϵͳ",
        "help", "����",
        "newfile �ļ���", "�������ļ�",
        "newfolder �ļ�����", "�������ļ���",
        "cd �ļ�����", "�����ļ���",
        "show �ļ���", "��ʾ�ļ�����",
        "del �ļ����ļ����� ", "ɾ����ǰĿ¼�µ��ļ����ļ���",
        "write �ļ���", "���ļ���д������",
        "property �ļ����ļ�����", "�鿴��ϸ����",
        "grant �ļ����ļ����� �û����", "�����û�Ȩ��",
        "copy �ļ����ļ�����", "���Ƶ�ǰĿ¼�µ��ļ����ļ���",
        "paste", "���Ѹ�������ճ������ǰĿ¼��",
        "rename �ļ����ļ����� �µ�����", "�������ļ����ļ���",
        "#"
};

Leader super; //������
Path p; //��ǰ·��
char cmd[1000]; //��ȡ����
vector<User> users; //��ǰ�û���
User user; //��ǰ�û�
Item plate; //���а�

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
    cout << "��ʽ���������ȫ���ļ����ļ���" << endl;
    cout << "��ȷ��Ҫ�����?(Y/N)" << endl;
    gets(cmd);          //��ȡ����
    Scanner sc(cmd);
    string str = sc.next();
    if(str == "y" || str == "Y") {
        FILE *file = fopen(DISK, "w");//DISK�ļ�������Ḳ��,û���ļ��ʹ����ļ�
        fclose(file);

        //��ʼ��������
        writeBlock(0, &user);

        //��ʼ�������鳤��
        int leader = BlockCnt - BlockPerGroup; //leader��ʼ��Ϊ���һ���鳤��ı��
                 // 1024 - 31
        for (; leader >= 3; leader -= BlockPerGroup){
            Leader l;
            l.blockNum = BlockPerGroup - 1;   // l.blockNum = 30
            l.nextLeader = leader + BlockPerGroup;
            for (int i = 1; i <= l.blockNum; i++) l.member[i] = leader + i;
            writeBlock(leader, &l); //����ʼ����ɵ��鳤������д����Ӧ�ڴ�
        }
        //ѭ��������leaderΪ��һ�鳤����-BlockPerGroup   leader = 33-31=2;

        //��ʼ��������
        Leader super;
        leader += BlockPerGroup;   // leader = 2+31=33;
        super.blockNum = leader - 3; //��ȥ��0��1��2���ʣ��Ŀ���  30
        super.nextLeader = leader; //������ĳ�ʼ��һ�鳤�鼴Ϊ��һ�鳤��
        for (int i = 0; i < super.blockNum; i++) super.member[i] = 3 + i;
        writeBlock(1, &super); //����ʼ����ɵĳ���������д���һ���ڴ�

        //��ʼ����Ŀ¼
        Dir root;          //�����ó�һ�飬2
        root.isFile = false;
        root.blockNum = 0;
        root.itemNum = 0;
        root.byteNum = 0;
        root.authorID = -1;
        for(int i = 0; i < U_MAX_NUM; i++) root.pri[i] = true;

        writeBlock(2, &root); //����ʼ����ɵĸ�Ŀ¼д��ڶ����ڴ�

        cout << "��ʽ���ɹ�" << endl;
    } else {
        cout << "��ʽ������ȡ��" << endl;
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
            printf("| %-10s | %-10s |\n", a[j].name, temp.isFile? "�ļ�": "�ļ���");
        }
    }
    cout << table << endl;
}

void del(string fname) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name))
        cout << "�������������ļ����ļ���" << endl;
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
            cout << "ɾ���ɹ�" << endl;
        }
    }
}

void newff(string str, string fname) {
    const char *name = fname.c_str();
    if ((int)fname.length() > NAME_LEN) cout << "�ļ����ļ�����̫��" << endl;
    else if(exist(p.top().blockID, name)) cout << "�Ѿ�������Ϊ" << fname << "���ļ����ļ�����" << endl;
    else {
        addItem(p.top().blockID, name, str == "newfile");
        cout << "�½��ɹ�" << endl;
    }
}

void cd(string fname) {
    const char *name = fname.c_str();
    if ((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)){
        cout << "�������������ļ�" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "û��Ȩ��" << endl;
        return;
    }
    if (dir.isFile){
        cout << "���ܽ����ļ�,�������ļ���" << endl;
        return;
    }
    p.push(it);
}

void show(string fname) {
    const char *name = fname.c_str();
    if ((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)){
        cout << "�������������ļ�" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "û��Ȩ��" << endl;
        return;
    }
    if (!dir.isFile){
        cout << fname << "�����ļ�" << endl;
        return;
    }
    char content[BlockCap*30];
    readFile(it.blockID, content);
    cout << content << endl;
}

string replace(string str, string old, string news) { //��str�г��ֵ�old�Ӵ�ȫ���滻��news�Ӵ��������滻��Ĵ�
    string res;
    int strl = str.length(), oldl = old.length();
    for (int i = 0; i < strl;) {
        int j;
        for (j = 0; j < oldl && i + j < strl && str[i+j] == old[j]; j++);
        if (j == oldl) { //�ҵ�һ��old�Ӵ�
            res += news;
            i += j;
        } else{
            res += str[i];
            i++;
        }
    }
    return res;
}

void tree(short id, const char *name, bool isFile, string prefix, string &treeString) { //��ʾĿ¼�����νṹ
    treeString += prefix + name + "\n";
    if (isFile) return; //��ǰ���ļ�����ݹ����
    string temp = replace(prefix, "��", "��");
    temp = replace(temp, "��", "  ");
    temp = replace(temp, "��", "  ");
    Dir dir;
    readBlock(id, &dir);
    for (int i = 0, k = 0; i < dir.blockNum; i++){
        Item a[ItemPerBlock]; readBlock(dir.member[i], a);
        for (int j = 0; j < ItemPerBlock && k < dir.itemNum; j++, k++) {
            Dir dir;
            readBlock(a[j].blockID, &dir);
            if (k == dir.itemNum-1) tree(a[j].blockID, a[j].name, dir.isFile, temp + "����", treeString);
            else tree(a[j].blockID, a[j].name, dir.isFile ,temp + "�ǩ�", treeString);
        }
    }
}

void write(string fname) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)){
        cout << "�������������ļ�" << endl;
    } else {
        Item it = getItem(p.top().blockID, name);
        Dir dir;
        readBlock(it.blockID, &dir);
        if(!dir.pri[user.id]) {
            cout << "û��Ȩ��" << endl;
            return;
        }
        if(!dir.isFile){
            cout << "�������ļ�����д����" << endl;
            return;
        }
        cout << "����������(��esc������)��" << endl;
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
        cout << "��������Ӧ��ѡ�������в���" << endl;
        cout << "1����¼" << endl;
        cout << "2��ע��" << endl;
        cout << "3���˳�" << endl;
        gets(cmd); Scanner sc(cmd);
        string str = sc.next(), name, pwd;
        if(str == "1") {
            cout << "�û�����";
            gets(cmd); sc = Scanner(cmd);
            name = sc.next();
            cout << "���룺";
            gets(cmd); sc = Scanner(cmd);
            pwd = sc.next();
            for(int i = 0, sz = users.size(); i < sz; i++) {
                if(!strcmp(name.c_str(), users[i].name) &&
                   !strcmp(pwd.c_str(), users[i].password)) {
                    cout << "��½�ɹ�" << endl;
                    return users[i];
                }
            }
            cout << "�û������������" << endl;
        } else if(str == "2") {
            if((int)users.size() == U_MAX_NUM) {
                cout << "�û������������޷�ע�����û�" << endl;
                continue;
            }
            cout << "�û�����";
            gets(cmd); sc = Scanner(cmd);
            name = sc.next();
            if((int)name.length() > U_NAME_LEN) cout << "�û�����������󳤶�Ϊ" << U_NAME_LEN << endl;
            else {
                bool ok = 1;
                for(int i = 0, sz = users.size(); i < sz; i++) {
                    if(!strcmp(name.c_str(), users[i].name)) {
                        ok = 0;
                        break;
                    }
                }
                if(!ok) cout << "���û����Ѵ���" << endl;
                else {
                    cout << "���룺";
                    gets(cmd); sc = Scanner(cmd);
                    pwd = sc.next();
                    if((int)pwd.length() > U_PWD_LEN) cout << "�����������󳤶�Ϊ" << U_PWD_LEN << endl;
                    else {
                        cout << "ȷ�����룺";
                        gets(cmd); sc = Scanner(cmd);
                        str = sc.next();
                        if(str == pwd) {
                            User user;
                            strcpy(user.name, name.c_str());
                            strcpy(user.password, pwd.c_str());
                            user.id = users.size();
                            users.push_back(user);
                            writeBlock(BlockCnt+user.id, &user);
                            cout << "ע��ɹ������Զ���¼" << endl;
                            return user;
                        } else {
                            cout << "ȷ�����벻һ��" << endl;
                        }
                    }
                }
            }
        } else if(str == "3") {
            exit(0);
        } else {
            cout << "�����ʽ����" << endl;
        }
    }
}

void property(string fname) {
    const char *name = fname.c_str();
    Item it = getItem(p.top().blockID, name);
    if(it.blockID == -1) {
        cout << "�ļ����ļ��в�����" << endl;
        return;
    }
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "û��Ȩ��" << endl;
        return;
    }
    cout << "���ͣ�" << (dir.isFile? "�ļ�": "�ļ���") << endl;
    cout << "��ռ������" << dir.blockNum << endl;
    cout << "��ռ�ֽڴ�С��" << dir.byteNum << endl;
    cout << "�����û���ţ�" << dir.authorID << endl;
}

void grant(string fname, string id) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)) {
        cout << "�������������ļ�" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "û��Ȩ��" << endl;
        return;
    }
    stringstream ss(id);
    int userid; ss >> userid;
    if(userid < 0 || userid >= (int)users.size()) {
        cout << "�����ڵ��û�" << endl;
        return;
    }
    dir.pri[userid] = true;
    writeBlock(it.blockID, &dir);
    cout << "����Ȩ�޳ɹ�" << endl;
}

void rename(string oldname, string newname) {
    char name[NAME_LEN];
    strcpy(name, oldname.c_str());
    if((int)oldname.length() > NAME_LEN || !exist(p.top().blockID, name)) {
        cout << "�������������ļ�" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "û��Ȩ��" << endl;
        return;
    }
    if((int)newname.length() > NAME_LEN) {
        cout << "�����ƹ�������󳤶�Ϊ" << NAME_LEN << endl;
        return;
    }
    if(exist(p.top().blockID, newname.c_str())) {
        cout << "�������Ѵ���" << endl;
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
    cout << "�������ɹ�" << endl;
}

void cop(string fname) {
    const char *name = fname.c_str();
    if((int)fname.length() > NAME_LEN || !exist(p.top().blockID, name)) {
        cout << "�������������ļ�" << endl;
        return;
    }
    Item it = getItem(p.top().blockID, name);
    Dir dir;
    readBlock(it.blockID, &dir);
    if(!dir.pri[user.id]) {
        cout << "û��Ȩ��" << endl;
        return;
    }
    plate = it;
    cout << "���Ƴɹ�" << endl;
}

void paste() {
    if(plate.blockID == -1) {
        cout << "û�п�ճ�����ļ����ļ���" << endl;
        return;
    }
    if(exist(p.top().blockID, plate.name)) {
        cout << "����ͬ���ļ����ļ���" << endl;
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
    cout << "ճ���ɹ�" << endl;
}

int main() {
    FILE *file = fopen(DISK, "r");
    //printf("%d******%d\n",0,'\0');
    users.clear();
    if(file == NULL) {
        cout << "���̲�����,��Ҫ�½����̲���ʽ��" << endl;
        format();
    } else {
        cout << "��ȡ���̳ɹ�" << endl;
        for(int i = 0; i < U_MAX_NUM; i++) {    //����û�����
            fseek(file, (BlockCnt+i)*BlockCap, 0); //��ʼƫ��1024*64���ֽ�
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

    cout << "����һ��Ҫע��,�˳�ʱҪʹ��exitָ���˳�, ��Ҫ�رտ���̨" << endl;
    cout << "��Ϊ������Ҫ��exit()��һЩ�ƺ���" << endl;
    cout << "��ʹ��help����鿴����" << endl;
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
            if(!sc.hasNext()) cout << "�����ʽ����, ȱ���ļ����ļ�����" << endl;
            else if(str == "del") del(sc.next());
            else if(str == "newfile" || str == "newfolder") newff(str, sc.next());
            else if(str == "cd") cd(sc.next());
            else if(str == "show") show(sc.next());
            else if(str == "write") write(sc.next());
            else if(str == "copy") cop(sc.next());
            else if(str == "property") {
                if (!sc.hasNext()) cout << "�����ʽ����" << endl;
                else property(sc.next());
            } else if(str == "grant") {
                str = sc.next();
                if (!sc.hasNext()) cout << "�����ʽ����" << endl;
                else grant(str, sc.next());
            } else if(str == "rename") {
                str = sc.next();
                if (!sc.hasNext()) cout << "�����ʽ����" << endl;
                else rename(str, sc.next());
            }
        }
    }
    return 0;
}


