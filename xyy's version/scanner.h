//
// Created by 24757 on 2018/6/30.
//
#ifndef OS_SCANNER_H
#define OS_SCANNER_H
#include <cstring>
#include <string>

using std::string;

struct Scanner {
    int pos;
    char s[1000];

    Scanner(char str[]) {
        strcpy(s, str);
        pos = 0;
        while(s[pos] == ' ') pos++; //滤掉空格
    }

    bool hasNext() {
        return s[pos] != '\0';
    }

    string next() {
        string res;
        while(s[pos] && s[pos] != ' ') res += s[pos++];
        while(s[pos] == ' ') pos++;
        return res;
    }
};
#endif //OS_SCANNER_H
