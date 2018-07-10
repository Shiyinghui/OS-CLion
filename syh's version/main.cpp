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
    cout<<"�����ʽ";
    for(int i=0;i<18;i++)
        cout<<" ";
    cout<<"��ʽ˵��"<<endl;
    printf("\t\t");
    for(int i=0;i<80;i++)
    cout<<"-";
    cout<<endl;
    string tip[100]={
    "help",                    "����",
    "format",                  "��ʽ������",
    "ls",                      "��ʾ��ǰĿ¼������",
    "logout",                  "�˳���ǰ�ʺ�",
    "exit",                    "�˳�ϵͳ",
    "back",                    "������һ��Ŀ¼",
    "root",                    "�ص���Ŀ¼��",
    "access",                  "��ʾ��ǰĿ¼���û���Ȩ�޷��ʵ��ļ����ļ���",
    "newFile fileName",        "�ڵ�ǰĿ¼���½�һ����ΪfileName���ļ�",
    "newFolder folderName",    "�ڵ�ǰĿ¼���½�һ����ΪfolderName���ļ���",
    "del itemName",            "ɾ����ǰĿ¼����ΪitemName���ļ����ļ���",
    "cd folderName",           "���뵱ǰĿ¼����ΪfolderName���ļ���",
    "read fileName",           "��ȡ��ǰĿ¼����ΪfileName���ļ�����",
    "write fileName",          "��ǰĿ¼����ΪfileName���ļ���д����",
    "rename itemName",         "��������ǰĿ¼����ΪitemName���ļ����ļ���",
    "details itemName",        "�鿴��ǰĿ¼����ΪitemName���ļ����ļ��е���ϸ��Ϣ",
    "copy fileName",           "���Ƶ�ǰĿ¼����ΪfileName���ļ�",
    "paste",                   "�����Ƶ��ļ�ճ������ǰĿ¼��",
    "grant itemName userName", "����ǰĿ¼����ΪitemName���ļ����ļ�����Ȩ��userName",
    "revoke itemName userName","�ջ�userName�Ե�ǰĿ¼����ΪitemName���ļ����ļ���",
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


// ȫ�ֱ���
 leadingBlk super;
 User user;
 SFD_item tempSFD;
 vector<User> users;
 Path currentPath; //��ǰ·��

int main() {
    FILE *file = fopen("virtualDisk.txt","r");
    char input[1000];
    if(file == NULL) {
        cout<<"���ļ�ϵͳ�Ĵ��̲����ڣ����½�һ�����̲���ʼ��"<<endl;
        init();
        cout<<"�ѳɹ���ʼ������"<<endl;
    }
    else{
        cout<<"��ȡ���̳ɹ��������ļ�ϵͳ"<<endl;
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
    //tempSFD�൱��һ����ʱ����������ʼ��ʱû���κ��ļ�������

    user = enter();   // ����ϵͳģ��
    blkWrite(0, &user); // user����Ϣд�������飬���

    cout << "��ʾ����ʹ��help����鿴����" << endl;
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
            currentPath.root(); // �ٻص���Ŀ¼��
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
             if(!str.hasNext()) cout << "��������ڣ������Сд�������ʽ" << endl;
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
                if (!str.hasNext()) cout << "���������������Ҫ����Ȩ�޵��û���" << endl;
                else grant(itemName, str.next());
            }
             else if(cmd == "revoke") {
                 string itemName = str.next();
                 if (!str.hasNext()) cout << "���������������Ҫ�ջ�Ȩ�޵��û���" << endl;
                 else revoke(itemName, str.next());
             }
             else if(cmd == "rename") {
                string oldName = str.next();
                if (!str.hasNext()) cout << "����������������µ��ļ�(��)��" << endl;
                else rename(oldName, str.next());
            }
        }
    }
    return 0;
  }

