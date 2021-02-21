#include <algorithm>
#include <iostream>

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
long long ans = 0, res;
int a[20];

int main(int argc, char **argv) {
    int n;
    int rnd = 0;
    while (~scanf("%d", &n)) {
        for (int i = 0; i < n; i++) {
            scanf("%d", a + i);
        }
        for (int start = 0; start < n; start++)
            for (int end = start; end < n; end++) {
                res = 1;
                for (int k = start; k <= end; k++)
                    res *= a[k];
                ans = max(ans, res);
            }
        printf("%d: %lld.\n\n", ++rnd, ans);
    }
    return 0;
}
