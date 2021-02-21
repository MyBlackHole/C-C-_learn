#include <iostream>

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

bool solve(int &W) {
    int W1, D1, W2, D2;
    bool b1 = true, b2 = true;
    cin >> W1 >> D1 >> W2 >> D2;
    if (!W1) b1 = solve(W1);
    if (!W2) b2 = solve(W2);
    W = W1 + W2;
    return b1 && b2 && (W1 * D1 == W2 * D2);
}

int main(int argc, char **argv) {
    int T, W;
    cin >> T;
    while (T--) {
        if (solve(W)) cout << "YES\n";
        else
            cout << "NO\n";
        if (T)
            cout << "\n";
    }
    return 0;
}
