//
// Created by BlackHole on 2020/8/9.
//

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define SEG_COUNT 4
int segments[SEG_COUNT];
char **ans;
int ans_len;

void dfs(char *s, int segId, int segStart) {
    // 如果找到了 4 段 IP 地址并且遍历完了字符串，那么就是一种答案
    int len_s = strlen(s);
    if (segId == SEG_COUNT) {
        if (segStart == len_s) {
            char *ipAddr = (char *) malloc(sizeof(char) * (len_s + 4));
            int add = 0;
            for (int i = 0; i < SEG_COUNT; ++i) {
                int number = segments[i];
                if (number >= 100) {
                    ipAddr[add++] = number / 100 + '0';
                }
                if (number >= 10) {
                    ipAddr[add++] = number % 100 / 10 + '0';
                }
                ipAddr[add++] = number % 10 + '0';
                if (i != SEG_COUNT - 1) {
                    ipAddr[add++] = '.';
                }
            }
            ipAddr[add] = 0;
            ans = realloc(ans, sizeof(char *) * (ans_len + 1));
            ans[ans_len++] = ipAddr;
        }
        return;
    }

    // 如果还没有找到 4 段 IP 地址就已经遍历完了字符串，那么提前回溯
    if (segStart == len_s) {
        return;
    }

    // 由于不能有前导零，如果当前数字为 0，那么这一段 IP 地址只能为 0
    if (s[segStart] == '0') {
        segments[segId] = 0;
        dfs(s, segId + 1, segStart + 1);
    }

    // 一般情况，枚举每一种可能性并递归
    int addr = 0;
    for (int segEnd = segStart; segEnd < len_s; ++segEnd) {
        addr = addr * 10 + (s[segEnd] - '0');
        if (addr > 0 && addr <= 0xFF) {
            segments[segId] = addr;
            dfs(s, segId + 1, segEnd + 1);
        } else {
            break;
        }
    }
}

char **restoreIpAddresses(char *s, int *returnSize) {
    ans = (char **) malloc(0);
    ans_len = 0;
    dfs(s, 0, 0);
    (*returnSize) = ans_len;
    return ans;
}


int main() {
    char str[] = {"25525511135"};
    int *returnSize = malloc(sizeof(int *));
    restoreIpAddresses(str, returnSize);
    for (int i = 0; i < *returnSize; i++) {
        printf("%s\n", ans[i]);
    }
}