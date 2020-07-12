#include <iostream>
#include <string>
#include <sstream>
#include <set>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

using namespace std;

set<string> dict;

int main(int argc, char **argv) {
    string s, buf;
    while (cin >> s) {
        for (int i = 0; i < s.length(); i++)
            if (isalpha(s[i]))
                s[i] = tolower(s[i]);
            else
                s[i] = ' ';
        stringstream ss(s);
        while (ss >> buf)
            dict.insert(buf);
    }
    for (set<string>::iterator it = dict.begin(); it != dict.end(); ++it)
        cout << *it << "\n";
    return 0;
}
