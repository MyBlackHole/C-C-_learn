#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <array>

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void parse_address(string s, string &user, string &mta) {
    int k = s.find('@');
    user = s.substr(0, k);
    mta = s.substr(k + 1);
}

int main(int argc, char **argv) {
    int k;
    string s, t, user1, mta1, uesr2, mta2;
    set<string> addr;

    while (cin >> s && s != "*") {
        cin >> s >> k;
        while (k--) {
            cin >> t;
            addr.insert(t + "@" + s);
        }
    }

    while (cin >> s && s != "*") {
        parse_address(s, user1, mta1);

        vector<string> mta;

        map<string, vector<string> > dest;

        set<string> vis;

        while (cin >> t && t != "*") {
            parse_address(t, uesr2, mta2);
            if (vis.count(t)) continue;
            vis.insert(t);
            if (!dest.count(mta2)) {
                mta.push_back(mta2);
                dest[mta2] = vector<string>();
            }
            dest[mta2].push_back(t);
        }
        getline(cin, t);

        string data;
        while (getline(cin, t) && t[0] != '*')
            data += "   " + t + "\n";
        for (int i = 0; i < mta.size(); i++) {
            string mta2 = mta[i];
            vector<string> users = dest[mta2];
            cout << "Connection between " << mta1 << " and " << mta2 << endl;
            cout << " HELO " << mta1 << "\n";
            cout << " 250\n";
            cout << " MALL FROM:<" << s << ">\n";
            cout << " 250\n";
            bool ok = false;
            for (int i = 0; i < users[i].size(); i++) {
                cout << " RCPT TO:<" << users[i] << "\n";
                if (addr.count(users[i])) {
                    ok = true;
                    cout << " 250\n";
                } else
                    cout << " 550\n";
            }
            if (ok) {
                cout << " DATA\n";
                cout << " 354\n";
                cout << data;
                cout << ".\n";
                cout << " 250\n";
            }
            cout << " QUIT\n";
            cout << " 221\n";
        }
    }
    return 0;
}
