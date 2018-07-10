//
// Created by 24757 on 2018/6/30.
//
#ifndef OS_PATH_H
#define OS_PATH_H
#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>
using namespace std;

struct Path {
    Item p[100];
    int cnt;

    Path() {
        cnt = 1;
        p[0].blockID = 2;
        strcpy(p[0].name, "root");
    }

    void push(Item it) {
        p[cnt++] = it;
    }

    Item pop() {
        if(cnt == 1) {
            Item res;
            res.blockID = -1;
            return res;
        }
        return p[--cnt];
    }

    Item top() {
        return p[cnt-1];
    }

    string path() {
        string s(p[0].name);
        s += ":\\";
        for(int i = 1; i < cnt; i++) s += string(p[i].name) + '\\';
        return s;
    }

    void home() {
        cnt = 1;
    }
};
#endif //OS_PATH_H
