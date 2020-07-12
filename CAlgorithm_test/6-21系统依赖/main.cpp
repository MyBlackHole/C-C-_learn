#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cstring>

#define maxn 80

using namespace std;

vector<string> names;
map<string, int> id;
vector<int> ojmount[maxn], ojrefer[maxn];
int status[maxn];
vector<int> installed;

bool needed(int u) {
    bool ret = false;
    for (int i = 0; i < ojrefer[u].size(); ++i) {
        if (status[ojrefer[u][i]])
            ret = true;
    }
    return ret;
}

void install(int u) {
    for (int i = 0; i < ojmount[u].size(); ++i)
        install(ojmount[u][i]);
    if (!status[u]) {
        status[u] = 2;
        cout << " " << names[u] << "????????" << endl;
        installed.push_back(u);
    }
}

void remove(int u) {
    status[u] = 0;
    installed.erase(remove(installed.begin(), installed.end(), u), installed.end());
    cout << names[u] << ": ???????!" << endl;
    for (int i = 0; i < ojmount[u].size(); ++i) {
        int v = ojmount[u][i];
        if (status[v] == 2 && !needed(v))
            remove(v);
    }
}

int ID(string s) {
    if (id.count(s))
        return id[s];
    names.push_back(s);
    id[s] = names.size() - 1;
}

int main() {
    freopen("in.txt", "r", stdin);
    freopen("out.txt", "w", stdout);
    memset(status, 0, sizeof(status));
    string in;
    while (getline(cin, in)) {
        if (in[0] == 'E')
            break;
        else {
            stringstream ss(in);
            string op;
            string name;
            ss >> op;
            ss >> name;
            int u = ID(name);
            if (op[0] == 'D') {
                while (ss >> name) {
                    int v = ID(name);
                    ojmount[u].push_back(v);
                    ojrefer[v].push_back(u);
                }
            } else if (op[0] == 'I') {
                if (status[u])
                    cout << "  " << name << "$$$$$$$" << endl;
                else {
                    install(u);
                    status[u] = 1;
                }
            } else if (op[0] == 'R') {
                if (status[u] == 0)
                    cout << " " << name << "$$$$$$$" << endl;
                else if (needed(u))
                    cout << " " << "$$$$$$" << endl;
                else remove(u);
            } else {
                cout << "$$$" << endl;
                for (int i = 0; i < installed.size(); ++i)
                    cout << i << ": " << names[installed[i]] << endl;
            }
        }

    }
}
