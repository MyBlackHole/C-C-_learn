#include <iostream>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void swap2(int &a, int &b) {
    int t = a;
    a = b;
    b = t;
}

int main(int argc, char **argv) {
    int a = 3, b = 4;
    std::swap(a, b);
    std::cout << a << " " << b << "\n";
    return 0;
}
