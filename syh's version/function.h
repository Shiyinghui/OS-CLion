//
// Created by 24757 on 2018/7/5.
//

#ifndef OSV4_FUNCTION_H
#define OSV4_FUNCTION_H

#include "virtualDisk.h"
void format();
User enter();
int login();
int Register();
void ls();
void cd(string itemName);
void del(string itemName);
void details(string itemName);
void newItem(string str, string itemName);
void read(string fileName);
void write(string fileName);
void access();
void rename(string oldName, string newName);
void grant(string fName, string uName);
void revoke(string fName, string uName);
void copy(string fName);
void paste();
void init();

string getTime();
void blkRead(short, void *);
void blkWrite(short, void *);
int  blkAlloc();
void blkFree(short);
SFD_item findAItem(short, const char *);
int addItem(short, const char *, bool);
void deleteFolder(short);
bool deleteItem(short, const char *);
bool exist(short, const char *);
void readFile(short, char *);
void writeFile(short, const char *);
void deleteFile(short);


#endif //OSV4_FUNCTION_H
