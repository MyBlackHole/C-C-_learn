#include <iostream>
#include <string>


bool StringContain(std::string &a, std::string &b) {
    for (int i = 0; i < b.length(); ++i) {
        int j;
        for (j = 0; (j < a.length()) && (a[j] != b[i]); ++j);
        if (j >= a.length()) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    std::string a = "b";
    std::string b = "abc";
    if (StringContain(a, b)) {
        printf("是");
    } else {
        printf("否");
    }
    return 0;
}
