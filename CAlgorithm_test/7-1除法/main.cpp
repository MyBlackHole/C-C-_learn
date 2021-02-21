#include <set>
#include <cstdio>
#include <iostream>

using namespace std;

set<char> str;
int n;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void check(int a, int b) {
    char s1[6], s2[6];
    sprintf(s1, "%d", a);
    sprintf(s2, "%05d", b);
    str.insert(s1, s1 + 5);
    str.insert(s2, s2 + 5);
//	for (set<char>::iterator it = str.begin(); it != str.end(); it++)
//		cout << "----" << *it;
//	cout << endl;
    if (str.size() == 10)
        cout << a << " / " << b << " = " << n << endl;
}

int main(int argc, char **argv) {
    while (~scanf("%d", &n) && n >= 2 && n <= 79) {
        for (int i = 0; i <= 49383; i++) {
            int j = i * n;
            if (j <= 98765) {
                str.clear();
                check(j, i);
            } else
                break;
        }
    }
    return 0;
}
