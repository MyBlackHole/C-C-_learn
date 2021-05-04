#include <stdio.h>

#define weak_alias(name, aliasname) _weak_alias (name, aliasname)
#define _weak_alias(name,aliasname) extern __typeof (name) aliasname __attribute__ ((weak,alias(#name)));

void __fun(void)
{
        printf("this is ___fun\n");
}

weak_alias(__fun,fun1);

int main(int argc,char * argv[])
{
   __fun();
   fun1();
   return 0;
}