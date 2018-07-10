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

// ��ȡʱ�亯��
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
   // cout<<"��ǰʱ��:"<<res<<endl;
    return res;
}

//�û���½����
int login(){
    string name,pwd;
    cout << "�û�����";
    getline(cin,name);
    cout << "���룺";
    getline(cin,pwd);
    for(int i = 0; i< users.size(); i++) {
        if(!strcmp(name.c_str(), users[i].name) &&
           !strcmp(pwd.c_str(), users[i].password)) {
            cout << "��½�ɹ�!" << endl;
            return users[i].id;
        }
    }
    cout << "�û�����������������" << endl;
    return -1;
}

//�û�ע�ắ��
int Register(){
    string name,pwd,email;
    bool ok = false;
    char input[14],temp[7];
    if((int)users.size() == MAX_USER_NUM)
        cout << "��ע���û������ﵽϵͳ���ޣ��޷�ע�ᣡ" << endl;
    cout << "�������û���(24λ����)��";
    getline(cin,name);
    if((int)name.length() > NAME_USER_LEN)
        cout << "�û���������ע��ʧ��" <<endl;
    else {
        bool exist = false;
        for(int i = 0; i < users.size(); i++) {
            if(!strcmp(name.c_str(), users[i].name)) {
                exist = true;
                break;
            }
        }
        if(exist)
            cout << "ϵͳ���Ѵ�����ͬ���û���" << endl;
        else {
            cout << "����������(24λ����)��";
            getline(cin,pwd);
            if((int)pwd.length() > PWD_USER_LEN)
                cout << "���������ע��ʧ��"<<endl;
            else {
                cout<<"����������(14λ����)��"<<endl;
                getline(cin,email);
                if((int)email.length() > MAIL_USER_LEN)
                    cout << "���������ע��ʧ��"<<endl;
                else{
                    User user;
                    strcpy(user.name, name.c_str());
                    strcpy(user.password, pwd.c_str());
                    strcpy(user.email,input);
                    user.id = users.size();
                    users.push_back(user);
                    blkWrite(TOTAL_BLOCK+user.id, &user);
                    cout << "ע��ɹ������Զ���¼" << endl;
                    return user.id;
                }

            }
        }
    }
    return -1;
}

//��ʼ������ init
void init(){

    FILE *file = fopen("virtualDisk.txt","w");
    fclose(file);
    blkWrite(0, &user);

    // leader=1000-31 = 969;
    // ���һ���鳤��ı�� 969��970~999(30��)
    short leader = TOTAL_BLOCK - GROUP_BLK_NUM;
    for (; leader >= 3; leader -= GROUP_BLK_NUM){
        leadingBlk l;
        l.blkNum = GROUP_BLK_NUM - 1;   // l.blkNum = 30
             //   cout<<"������:"<<l.blkNum<<endl;

        l.nextLeadingBlk = leader + GROUP_BLK_NUM; //��һ��ѭ������һ���鳤�� 1000
             //   cout<<"��һ�飺"<<l.nextLeadingBlk<<endl;
        for (short i = 0; i < l.blkNum; i++) {
            l.member[i] = leader + i + 1;
          //  cout<< l.member[i]<<" ";
        }
        blkWrite(leader, &l); //����ʼ����ɵ��鳤������д����Ӧ���̿�
       // cout<<leader<<endl;
    }
    //����ѭ����leader = 8-31=-23; ��һ���鳤�飺8

    //��ʼ��������
    leadingBlk super;
    leader += GROUP_BLK_NUM;   // leader = -23+31=8;
    super.blkNum = leader - 3; //��ȥ��0��1��2���ʣ��Ŀ��� 5
    super.nextLeadingBlk = leader; // ��һ�鳤�� 8
    for (int i = 0; i < super.blkNum; i++) {
        super.member[i] = 3 + i; //(3,4,5,6,7)
       // cout<<endl;
       // cout<<super.member[i]<<" ";
    }
    blkWrite(1, &super); //�ѳ�����д�����̿�

    //��ʼ����Ŀ¼
    BFD_dir base;          //�����ó�һ�飬2
    base.isFile = false;
    base.blkNum = 0;
    base.subDir = 0;
    base.byteNum = 0;
    base.ownerID = -1;
    for(int i = 0; i < MAX_USER_NUM; i++)
        base.access[i] = true;  //�����û����ɷ��ʸ�Ŀ¼
    blkWrite(2, &base); // �Ѹ�Ŀ¼i�ڵ�д�����̿�

}

// �û��鿴��ǰĿ¼���Լ���Ȩ�޵��ļ�(�ļ���)
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
        cout<<"��ǰĿ¼����û�з����κ�һ���ļ��е�Ȩ��"<<endl;
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

// 1. ��ʽ������format
void format(){
   string cmd;
   cout<<"���棺��ʽ������մ����е�������Ϣ"<<endl;
   cout<<"��ȷ��Ҫ��ʽ����(Y/N)"<<endl;
   getline(cin,cmd);
   if(cmd == "y" || cmd=="Y"){
       init();
       cout<<"�ѳɹ���ʽ������"<<endl;
   }
   else cout<<"��ȡ����ʽ��"<<endl;
}

//2. ����ϵͳ����enter
User enter() {
    string cmd,name,pwd;
    int ok = -1;
    for(;;) {
        printf("\t\t");
        cout<<"---------��ӭʹ���ļ�����ϵͳ---------"<<endl;
        printf("\t\t\t");
        cout << "      1����¼" << endl;
        printf("\t\t\t");
        cout << "      2��ע��" << endl;
        printf("\t\t\t");
        cout << "      3���˳�" << endl;
        cout << "������1��2��3��ѡ����Ӧ������" << endl;
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
            cout << "��������������1��2��3" << endl;
            continue;
        }
    }
}


//3. �½��ļ����ļ��к��� newItem
void newItem(string str, string itemName) {
    const char *name = itemName.c_str();
    if ((int)itemName.length() > MAX_NAME_LEN)
        cout << "���ֹ������ļ�(��)����󳤶�Ϊ14λ" << endl;
    else if(exist(currentPath.top().blockID, name))
        cout << "��ǰĿ¼�Ѵ�����Ϊ" <<itemName<< "���ļ�(��)" << endl;
    else {
        addItem(currentPath.top().blockID, name, str == "newFile");
        cout << "�½��ɹ�" << endl;
    }

}

//4. ɾ���ļ����ļ���
void del(string itemName) {
    const char *name = itemName.c_str();

   /* BFD_dir temp; /////////  �������
    blkRead(currentPath.top().blockID,&temp); //////////
    cout<<"��ǰĿ¼��ռ�ֽ�����"<<temp.byteNum<<endl;//////////
*/
    if((int)itemName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name))
        cout << "��ǰĿ¼�²������������ļ�(��)" << endl;
    else {
        SFD_item it = findAItem(currentPath.top().blockID, name);
        BFD_dir dir;
        blkRead(it.blockID, &dir);
        if(!dir.access[user.id]){
            cout<<"��û��Ȩ��ɾ�����ļ�(��)!"<<endl;
            return;
        }
        int freeBytes = dir.byteNum;
        //cout<<"freeBytes:"<<freeBytes<<endl;
        if(deleteItem(currentPath.top().blockID, name)) {
           // cout<<"Path:"<<currentPath.index<<endl;
            for(int i = currentPath.index-2; i >= 0; i--) {
                BFD_dir tmp;
                blkRead(currentPath.p[i].blockID, &tmp);
                //cout<<"��ǰĿ¼"<<i<<"�ֽ�����"<<tmp.byteNum<<endl;
                tmp.byteNum -= freeBytes;
                //cout<<"ɾ����ǰĿ¼"<<i<<"�ֽ�����"<<tmp.byteNum<<endl;
                blkWrite(currentPath.p[i].blockID, &tmp);
            }
         /*   BFD_dir temp1;
            blkRead(currentPath.top().blockID,&temp1);*/
           // cout<<"ɾ����ǰĿ¼��ռ�ֽ�����"<<temp1.byteNum<<endl; /////////////
           /* cout<<"ɾ����Ϣ��"<<endl;
            cout<<"   �ļ����ڿ飺"<<it.blockID<<"   �ͷ��ֽڣ�"<<freeBytes<<endl;*/
            cout << "ɾ���ɹ�" << endl;
        }
    }
}

//5. ��ʾ��ǰĿ¼������
void ls() {

    int id = currentPath.top().blockID;  //
    BFD_dir dir;
    blkRead(id, &dir);
    if(dir.subDir == 0)
        cout<<"��Ŀ¼��û���ļ����ļ��У�"<<endl;
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

//6. �л�Ŀ¼����
void cd(string itemName) {
    const char *name = itemName.c_str();
    if ((int)itemName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)){
        cout << "��ǰĿ¼�²����ڴ��ļ�(��)�������л�" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if (dir.isFile){
        cout << "���ܽ����ļ���ֻ�ܽ����ļ���" << endl;
        return;
    }
    if(!dir.access[user.id]) {
        cout << "��û��Ȩ�޷��ʴ��ļ���" << endl;
        return;
    }
    currentPath.push(it);
}

//7. д�ļ�����
void write(string fileName) {

    /*BFD_dir temp; /////////
    blkRead(currentPath.top().blockID,&temp); //////////
    cout<<"��ǰĿ¼��ռ�ֽ�����"<<temp.byteNum<<endl;//////////*/

    char content[BLOCK_SIZE*MAX_ITEM_NUM];
    char s[BLOCK_SIZE*MAX_ITEM_NUM];
    const char *name = fileName.c_str();
    int i = 0, j = 0;
    if((int)fileName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)){
        cout << "��ǰĿ¼�²����ڴ��ļ�" << endl;
    } else {
        SFD_item it = findAItem(currentPath.top().blockID, name);
        BFD_dir dir;
        blkRead(it.blockID, &dir);
        if(!dir.isFile){
            cout << "�����ļ��У��������ļ�����д����" << endl;
            return;
        }
        if(!dir.access[user.id]) {
            cout << "��û��Ȩ��д���ļ�" << endl;
            return;
        }
        cout << "������Ҫд�������(��#������)��" << endl;
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
        int beforeSize = dir.byteNum; // �ļ�ԭ�ֽ���
        //cout<<"�ļ�ԭ�ֽ�����"<<dir.byteNum<<endl;
      //  cout<<strlen(s)<<endl;
        writeFile(it.blockID, s);
        blkRead(it.blockID, &dir);
        int afterSize = dir.byteNum;
       // cout<<"�ļ����ֽ�����"<<dir.byteNum<<endl; ////
        int diff = afterSize - beforeSize;
        for(int i = currentPath.index-1; i >= 0; i--) {
            BFD_dir tmp;
            blkRead(currentPath.p[i].blockID, &tmp);
            tmp.byteNum += diff;
            blkWrite(currentPath.p[i].blockID, &tmp);
        }
        cout<<"д��ɹ���"<<endl;

       /* BFD_dir temp2; /////////
        blkRead(currentPath.top().blockID,&temp2); //////////
        cout<<"��ǰĿ¼��ռ�ֽ�����"<<temp2.byteNum<<endl;//////////*/

    }
}

//8. ��ȡ�ļ����ݺ���
void read(string fileName) {
    const char *name = fileName.c_str();
    if ((int)fileName.length() > MAX_NAME_LEN|| !exist(currentPath.top().blockID, name)){
        cout << "��ǰĿ¼�²����ڴ��ļ�(��)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if (!dir.isFile){
        cout << fileName << "�����ļ����޷���ȡ" << endl;
        return;
    }
    if(!dir.access[user.id]) {
        cout << "��û��Ȩ�޶����ļ�" << endl;
        return;
    }
    char content[BLOCK_SIZE*30];
    readFile(it.blockID, content);
    if(strlen(content) == 0)
        cout<<"�ļ�����Ϊ��"<<endl;
    else{
        cout<<"�ļ�����Ϊ��"<<endl;
        cout << content << endl;
    }
}

//9. ����������
void rename(string oldName, string newName) {
    char name[MAX_NAME_LEN];
    strcpy(name, oldName.c_str());
    if((int)oldName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "��ǰĿ¼�²����ڴ��ļ�(��)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id]) {
        cout << "��û��Ȩ�޽�������������" << endl;
        return;
    }
    if((int)newName.length() > MAX_NAME_LEN) {
        cout << "�����ƹ�������󳤶�Ϊ" << MAX_NAME_LEN << endl;
        return;
    }
    if(exist(currentPath.top().blockID, newName.c_str())) {
        cout << "������ʧ�ܣ������ļ����µ�����ͬ����" << endl;
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
    cout << "�������ɹ�" << endl;
}

//10. ����Ȩ�޺���
void grant(string fName, string userName) {
    const char *name = fName.c_str();
    const char *uName = userName.c_str();
    bool ok = false;
    int index = -1;
    if((int)fName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "��ǰĿ¼�²����ڴ��ļ�(��)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id]) {
        cout << "��û��Ȩ�޽�����Ȩ����" << endl;
        return;
    }
    if(!strcmp(uName,user.name)){
        cout<<"�����ļ�(��)�Ĵ����ߣ�������Ȩ���Լ���"<<endl;
        return;
    }
    for(int i = 0;i<users.size();i++){
        if(!strcmp(users[i].name,uName)){
            ok = true;
            index = i;
        }
    }
    if(!ok) {
        cout<<"������û������ڣ�"<<endl;
        return;
    }
    dir.access[index] = true;
    blkWrite(it.blockID, &dir);
    cout << "�ɹ����ļ���Ȩ��������û�" << endl;
}

// 11. �ջ�Ȩ�޺���
void revoke(string fName, string userName) {
    const char *name = fName.c_str();
    const char *uName = userName.c_str();
    bool ok = false;
    int index = -1;
    if((int)fName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "��ǰĿ¼�²����ڴ��ļ�(��)" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id] || user.id != dir.ownerID) {
        cout << "��û��Ȩ�޽����ջ�Ȩ�޲���" << endl;
        return;
    }
    for(int i = 0;i<users.size();i++){
        if(!strcmp(users[i].name,uName)){
            ok = true;
            index = i;
        }
    }
    if(!ok) {
        cout<<"������û������ڣ�"<<endl;
        return;
    }
    if(users[index].id == dir.ownerID){
        cout<<"�����ļ�(��)�Ĵ����ߣ������ջ��Լ���Ȩ�ޣ�"<<endl;
        return;
    }
    else if(!dir.access[index]){
        cout<<"���û�û�з����ļ�(��)��Ȩ�ޣ������ջأ�"<<endl;
        return;
    }
    else{
        dir.access[index] = false;
        blkWrite(it.blockID, &dir);
        cout << "�ɹ��ջ�Ȩ��" << endl;
    }

}

//12. ���ƺ���
void copy(string fName) {
    const char *name = fName.c_str();
    if((int)fName.length() > MAX_NAME_LEN || !exist(currentPath.top().blockID, name)) {
        cout << "��ǰĿ¼�²����ڴ��ļ�" << endl;
        return;
    }
    SFD_item it = findAItem(currentPath.top().blockID, name);
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.isFile){
        cout<<"ע�⣺����ճ���ļ��У�"<<endl;
        return;
    }
    if(!dir.access[user.id]) {
        cout << "��û��Ȩ��" << endl;
        return;
    }
    tempSFD = it;
    cout << "���Ƴɹ�" << endl;
}

//13. ճ������
void paste() {
    if(tempSFD.blockID == -1) {
        cout << "û�п�ճ�����ļ�" << endl;
        return;
    }
    if(exist(currentPath.top().blockID, tempSFD.name)) {
        cout << "��ǰĿ¼�´���ͬ���ļ�(��)" << endl;
        return;
    }
    BFD_dir dir;
    blkRead(tempSFD.blockID, &dir);  //��Ҫճ�����ļ����ڵ�BFD����dir��
    addItem(currentPath.top().blockID, tempSFD.name, dir.isFile); // ��Ҫճ����Ŀ¼���½�һ���ļ�
    SFD_item newItem = findAItem(currentPath.top().blockID, tempSFD.name); // ȡ����ճ�����ļ�

    char text[BLOCK_SIZE*MAX_ITEM_NUM];
    readFile(tempSFD.blockID, text);  // ����ԭ�ļ��е�����
    writeFile(newItem.blockID,text);  // д���µ��ļ���
    int newBytes = dir.byteNum;

    for(int i = currentPath.index-1; i >= 0; i--) {
        BFD_dir temp;
        blkRead(currentPath.p[i].blockID, &temp);
        temp.byteNum += newBytes;  //Ŀ¼���ֽڱ仯
        blkWrite(currentPath.p[i].blockID, &temp);
    }
    cout << "ճ���ɹ�" << endl;
}

//14. �鿴�ļ���ϸ��Ϣ����
void details(string fname) {
    const char *name = fname.c_str();
    SFD_item it = findAItem(currentPath.top().blockID, name);
    if(it.blockID == -1) {
        cout << "��ǰĿ¼�²����ڴ��ļ�(��)" << endl;
        return;
    }
    BFD_dir dir;
    blkRead(it.blockID, &dir);
    if(!dir.access[user.id]) {
        cout << "��û��Ȩ�޲鿴�ļ�����ϸ��Ϣ" << endl;
        return;
    }
    cout << "���ͣ�" << (dir.isFile? "�ļ�": "�ļ���") <<"  ";
    cout <<"������������"<<dir.subDir<<"  ";
    cout << "��ռ������" << dir.blkNum <<"  ";
    cout << "��ռ�ֽڴ�С��" << dir.byteNum <<"  ";
    cout << "�����ߣ�" << users[dir.ownerID].name << endl;
    cout<<"ӵ��Ȩ�޵��û���";
    for(int i = 0; i < users.size(); i++){
        if(dir.access[users[i].id])
            cout<<users[i].name<<" ";
    }
    cout<<endl;
}

