//
// Created by BlackHole on 2020/7/24.
//

#include <stdio.h>
#include <z3.h>
#include <string.h>


//爱丽丝和鲍勃一起玩游戏，他们轮流行动。爱丽丝先手开局。
//
//最初，黑板上有一个数字 N 。在每个玩家的回合，玩家需要执行以下操作：
//
//选出任一 x，满足 0 < x < N 且 N % x == 0 。
//用 N - x 替换黑板上的数字 N 。
//如果玩家无法执行这些操作，就会输掉游戏。
//
//只有在爱丽丝在游戏中取得胜利时才返回 True，否则返回 false。假设两个玩家都以最佳状态参与游戏。

bool divisorGame(int N) {
    bool b[N];
    memset(b, false, sizeof(b));
    if (N <= 1) {
        return false;
    }
    b[1] = true;
    for (int i = 3; i <= N; i++) {
        for (int j = 1; j <= (i / 2); j++) {
            if ((i % j) == 0 && b[i - j - 1] == false) {
                b[i - 1] = true;
                break;
            }
        }
    }
    return b[N - 1];
}

int main() {
//    printf("%d", 7 % 2);
    bool status = divisorGame(60);
    printf("%d", status);
    return 0;
}

