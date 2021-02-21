#include "stdio.h"
#include "string.h"
#include "assert.h"

#define sum(a, b) ({int z = a + b; z+1;})
#define str(a, b) a.b
#define str1(a, b) a->b

typedef struct s1{
    int a;
    int b;
}s2;

unsigned int func_hash(const char *str)
{
    unsigned int hash = 0;
    unsigned int seed = 131;

    int i = 0;
    while (*str) {
        printf("%d %c\n", i++,*str);
        hash = hash * seed + *str++;
    }
    printf("%d %d\n", i++,*str);

    return hash & (0x7FFFFFFF);
}

void test_0()
{
    int key = func_hash("sss");
    printf("'sss'%ld字节大小\n", sizeof("sss"));
    printf("%d\n", '\0');
    printf("%d", key);
    return;
}

void test_1()
{
    char str1[3];
    char str2[3];
    int ret = 0;

    strcpy(str1, "a");
    strcpy(str2, "A");
    printf("%s %s\n", str1, str2);

    ret = strcmp(str1, str2);
    printf("%d\n", ret);
    printf("%d\n", 'a'-'b');
    int ret1 = strcmp("a","A");
    printf("%d\n", ret1);
}

void test_2()
{
    printf("%d", sum(1, 2));
}

void test_3()
{
    int a = 1;
    typeof(a)  b = 2;
    printf("%d", b);
}

void test4()
{
    s2 a = {1, 2};
    typeof(s2*) b = &a;
    printf("%d\n", str(a, b));
    printf("%d\n", str1(b, b));
}

void test5()
{
    int a[5] = {1, 2, 3, 4};
    int *b = a;
    int **c = &b;
    printf("%d", *c[0]);
}

int main(int argc, char *argv[])
{
    // printf("%d", (1100 & 0x7FFFFFFF) % 5);
    int i = 3;
    assert( i>4 );
    return 0;
}
