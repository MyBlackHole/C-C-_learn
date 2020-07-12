#include<cstdio>
#include<cstring>
#include<algorithm>

using namespace std;
const int maxn = 200;
const int maxt = 50;
const int DQ = 999999;
const double eps = 1e-8;

struct player {
    char name[maxt];
    double money;
    int place, all_2, all_4, dq[5];
    bool is_pro, is_T;
} players[maxn];

int rev_len(int k) {
    int sum = 0;
    while (k != 0) {
        k /= 10;
        sum++;
    }
    return sum;
}

bool cmp1(const player &a, const player &b) {
    if (a.all_2 != b.all_2)return a.all_2 < b.all_2;
    return strcmp(a.name, b.name) < 0;
}

bool cmp2(const player &a, const player &b) {
    if (a.all_4 != b.all_4)return a.all_4 < b.all_4;
    return strcmp(a.name, b.name) < 0;
}

bool cmp3(const player &a, const player &b) {
    int suma = 0, sumb = 0;
    for (int i = 0; i < 4; ++i)
        if (a.dq[i] == DQ) {
            suma = i;
            break;
        }
    for (int i = 0; i < 4; ++i)
        if (b.dq[i] == DQ) {
            sumb = i;
            break;
        }
    if (suma != sumb)return suma > sumb;
    if (a.all_4 != b.all_4)return a.all_4 < b.all_4;
    return strcmp(a.name, b.name) < 0;
}

int main() {
    //freopen("out.txt","w",stdout);
    int T, n;
    double prot[maxn], money_all;

    scanf("%d", &T);
    {
        while (T--) {
            memset(players, 0, sizeof(players));
            memset(prot, 0, sizeof(prot));
            scanf("%lf", &money_all);
            for (int i = 0; i < 70; ++i)scanf("%lf", &prot[i]);
            scanf("%d", &n);
            getchar();
            for (int i = 0; i < n; ++i) {
                fgets(players[i].name, 20, stdin);
                if (!strchr(players[i].name, '*'))players[i].is_pro = true;
                for (int j = 0; j < 4; ++j) {
                    int flag = 0;
                    if (!scanf("%d", &players[i].dq[j])) {
                        players[i].dq[j] = DQ;
                        flag = 1;
                    }
                    if (j < 2)players[i].all_2 += players[i].dq[j];
                    players[i].all_4 += players[i].dq[j];
                    if (flag)break;
                }
                char ch[45];
                fgets(ch, 40, stdin);
            }
            sort(players, players + n, cmp1);
            int pos = 0, pos2 = 0;
            while (pos < min(n, 70) && players[pos].all_2 < DQ)++pos;
            while (pos < n && players[pos].all_2 == players[pos - 1].all_2 && players[pos].all_2 < DQ)++pos;
            sort(players, players + pos, cmp2);
            while (pos2 < n && pos2 <= pos && players[pos2].all_4 < DQ)++pos2;
            while (pos2 < n && players[pos2].all_4 == players[pos2 - 1].all_4 && players[pos2].all_4 < DQ)++pos2;
            if (pos != pos2)sort(players + min(pos, pos2), players + max(pos, pos2), cmp3);
            int rak = 1, cur = 0, pos3, cont3 = 0;
            while (cur < min(pos, pos2)) {
                int sum = 0;
                double sum_mon = 0;
                for (pos3 = cur; players[pos3].all_4 == players[cur].all_4; ++pos3) {
                    if (players[pos3].is_pro) {
                        sum++;
                        sum_mon += prot[cont3++];
                    }
                }
                if (sum)sum_mon /= sum;
                else sum_mon /= 100;
                for (int i = cur; i < pos3; ++i) {
                    players[i].place = rak;
                    if (players[i].is_pro && sum)players[i].money = sum_mon * money_all / 100.0 + eps;
                    if (players[i].is_pro && sum > 1 && cont3 - sum < 70)players[i].is_T = true;
                    if (cont3 - sum >= 70)players[i].is_pro = false;
                }
                int numb = pos3 - cur;
                rak += numb;
                cur += numb;
            }
            printf("Player Name          Place     RD1  RD2  RD3  RD4  TOTAL     Money Won\n");
            printf("-----------------------------------------------------------------------\n");
            for (int i = 0; i < pos; ++i) {
                printf("%-21s", players[i].name);
                int N = 10;
                if (players[i].all_4 < DQ) {
                    printf("%d", players[i].place);
                    N -= rev_len(players[i].place);
                }
                if (players[i].is_T) {
                    printf("T");
                    N--;
                }
                for (int i = 0; i < N; ++i)printf(" ");
                N = 4;
                for (int j = 0; j < 4; ++j) {
                    if (players[i].dq[j] != DQ)
                        printf("%-5d", players[i].dq[j]);
                    else {
                        N -= j;
                        break;
                    }
                }
                if (N == 4)N = 0;
                for (int i = 0; i < N; ++i)
                    printf("     ");
                if (N) {
                    printf("DQ\n");
                    continue;
                }
                if (players[i].is_pro) {
                    printf("%-10d", players[i].all_4);
                    printf("$%9.2lf", players[i].money);
                } else printf("%d", players[i].all_4);
                printf("\n");
            }
            if (T)printf("\n");
        }
    }
    return 0;
}
