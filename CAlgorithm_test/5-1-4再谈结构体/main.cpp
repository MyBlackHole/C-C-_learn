#include <iostream>

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

struct Point {
    int x, y;

    Point(int x = 0, int y = 0) : x(x), y(y) {
    }
};

Point operator+(const Point &A, const Point &B) {
    return Point(A.x + B.x, A.y + B.y);
}

ostream &operator<<(ostream &out, const Point &P) {
    out << "(" << P.x << "," << P.y << ")";
}

int main(int argc, char **argv) {
    Point a, b(1, 2);
    a.x = 3;
    cout << a + b << "\n";
    return 0;
}
