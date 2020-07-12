#include <algorithm>
#include <string>
#include <iostream>

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

const int maxcol = 60;
const int maxn = 100 + 5;
string filenames[maxn];

void print(const string &s, int lan, char extra) {
    cout << s;
    for (int i = 0; i < lan - s.length(); i++)
        cout << extra;
}

int main(int argc, char **argv) {
    int n;
    while (cin >> n) {
        int M = 0;
        for (int i = 0; i < n; i++) {
            cin >> filenames[i];
            M = max(M, (int) filenames[i].length());
        }
        int cols = (maxcol - M) / (M + 2) + 1, rows = (n - 1) / cols + 1;
        print("", 60, '-');
        cout << "\n";
        sort(filenames, filenames + n);
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int idx = c * rows + r;
                if (idx < n)
                    print(filenames[idx], c == cols - 1 ? M : M + 2, ' ');
            }
            cout << "\n";
        }
    }
    return 0;
}
