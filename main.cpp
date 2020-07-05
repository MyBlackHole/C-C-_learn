#include <iostream>
#include "string"

int main() {
    char i[] = {'0'};
    char *s = i;
    std::cout << i << ',' << s[0] << std::endl;
    return 0;
}
