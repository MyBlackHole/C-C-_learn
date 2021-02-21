#include <iostream>
#include <set>
#include <stack>
#include <vector>
#include <map>
#include <algorithm>

#define ALL(x) x.begin(),x.end()
#define INS(x) inserter(x,x.begin())

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

typedef set<int> Set;
map<Set, int> IDcache;
vector<Set> Setcache;
stack<int> s;

int ID(Set x) {
    if (IDcache.count(x))
        return IDcache[x];
    Setcache.push_back(x);
    return IDcache[x] = Setcache.size() - 1;
}

int main(int argc, char **argv) {
    int n;
    cin >> n;
    for (int i = 0; i < n; i++) {
        string op;
        cin >> op;
        if (op[0] == 'P')
            s.push(ID(Set()));
        else if (op[0] == 'D') s.push(s.top());
        else {
            Set x1 = Setcache[s.top()];
            s.pop();
            Set x2 = Setcache[s.top()];
            s.pop();
            Set x;
            if (op[0] == 'U') set_union(ALL(x1), ALL(x2), INS(x));
            if (op[0] == 'I') set_intersection(ALL(x1), ALL(x2), INS(x));
            if (op[0] == 'A') {
                x = x2;
                x.insert(ID(x1));
            }
            s.push(ID(x));
        }
        cout << Setcache[s.top()].size() << endl;
    }
    return 0;
}
