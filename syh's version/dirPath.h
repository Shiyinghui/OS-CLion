//
// Created by 24757 on 2018/7/6.
//

#ifndef OSV4_PATH_H
#define OSV4_PATH_H

#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>
using namespace std;

struct Path {
   SFD_item  p[100];
    int index;

    Path() {
        index = 1;
        p[0].blockID = 2;
        strcpy(p[0].name, "OS");
    }

    void push(SFD_item it) {
        p[index++] = it;
    }

    SFD_item pop() {
        if(index == 1) {
            SFD_item res;
            res.blockID = -1;
            return res;
        }
        return p[--index];
    }

    SFD_item top() {
        return p[index-1];
    }

    string path() {
        string s(p[0].name);
        s += ":\\";
        for(int i = 1; i < index; i++) s += string(p[i].name) + '\\';
        return s;
    }

    void root() {
        index = 1;
    }
};
extern Path currentPath;
#endif //OSV4_PATH_H
