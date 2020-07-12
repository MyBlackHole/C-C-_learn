#include <vector>
#include <iostream>

using namespace std;

vector<int> v;

int main() {
    int k;
    while (cin >> k) {
        v.clear();
        for (int y = k + 1; y <= 2 * k; ++y) {
            if ((k * y) % (y - k) == 0)
                v.push_back(y);
        }
        cout << v.size() << endl;
        for (int i = 0; i < v.size(); ++i) {
            int y = v[i], x = (k * y) / (y - k);
            printf("1/%d = 1/%d + 1/%d\n", k, x, y);
        }
    }
    return 0;
}
