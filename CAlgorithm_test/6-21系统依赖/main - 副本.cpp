#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

#define maxn 10005
using namespace std;

vector<string> names;
map<string, int> id;
vector<int> before[maxn], after[maxn];
vector<int> installed;
int status[maxn];

int ID(string s) {
    if (id.count(s)) return id[s];
    names.push_back(s);
    return id[s] = names.size() - 1;
}

void install(int u) {
    for (int i = 0; i < before[u].size(); ++i)
        install(before[u][i]);
    if (!status[u]) {
        status[u] = 2;
        cout << "   Installing " << names[u] << endl;
        installed.push_back(u);
    }
}

bool needed(int u) {
    bool ret = false;
    for (int i = 0; i < after[u].size(); ++i) {
        if (status[after[u][i]]) ret = true;
    }
    return ret;
}

void remove(int u) {
    status[u] = 0;
    installed.erase(remove(installed.begin(), installed.end(), u), installed.end());
    cout << "   Removing " << names[u] << endl;
    for (int i = 0; i < before[u].size(); ++i) {
        int v = before[u][i];
        if (status[v] == 2 && !needed(v))
            remove(v);
    }
}

int main() {
    string in;
    while (getline(cin, in)) {
        cout << in << endl;
        if (in[0] == 'E') break;
        stringstream ss(in);
        string op;
        string name;
        ss >> op;
        if (op[0] == 'L') {
            for (int i = 0; i < installed.size(); ++i)
                cout << "   " << names[installed[i]] << endl;
        } else {
            ss >> name;
            int u = ID(name);
            if (op[0] == 'D') {
                while (ss >> name) {
                    int v = ID(name);
                    before[u].push_back(v);
                    after[v].push_back(u);
                }
            } else if (op[0] == 'I') {
                if (status[u]) cout << "   " << name << " is already installed.\n";
                else {
                    install(u);
                    status[u] = 1;
                }
            } else if (op[0] == 'R') {
                if (status[u] == 0) cout << "   " << name << " is not installed.\n";
                else if (needed(u)) cout << "   " << name << " is still needed.\n";
                else remove(u);
            }
        }
    }
    return 0;
}

