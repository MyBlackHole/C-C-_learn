#include <cstring>
#include <iostream>

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
const int maxn = 10000 + 10;
int sum[maxn];

void build(int p) {
    int v;
    cin >> v;
    if (v == -1) return;
    sum[p] += v;
    build(p - 1);
    build(p + 1);
}

bool init() {
    int v;
    cin >> v;
    if (v == -1) return false;
    memset(sum, 0, sizeof(sum));
    int pos = maxn / 2;
    sum[pos] = v;
    build(pos - 1);
    build(pos + 1);
}

int main(int argc, char **argv) {
    int kase = 0;
    while (init()) {
        int p = 0;
        while (sum[p] != 0) p++;
        cout << "Case" << ++kase << ":\n" << sum[p++];
        while (sum[p] != 0)
            cout << " " << sum[p++];
        cout << "\n\n";
    }
    return 0;
}
