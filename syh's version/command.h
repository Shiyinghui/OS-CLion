//
// Created by 24757 on 2018/7/6.
//

#ifndef OSV4_COMMAND_H
#define OSV4_COMMAND_H

#include <cstring>
#include <string>

using std::string;

struct Command {
    int pos;
    char cmd[800];

    Command(char str[]) {
        strcpy(cmd, str);
        pos = 0;
        while(cmd[pos] == ' ') pos++; //ÂËµô¿Õ¸ñ
    }

    bool hasNext() {
        return cmd[pos] != '\0';
    }

    string next() {
        string res;
        while(cmd[pos] && cmd[pos] != ' ') res += cmd[pos++];
        while(cmd[pos] == ' ') pos++;
        return res;
    }
};


#endif //OSV4_COMMAND_H
