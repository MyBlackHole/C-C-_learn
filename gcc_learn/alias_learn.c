#include <stdio.h>

int test1() 
{
  printf("%s\n",__FUNCTION__);
  return 0;
}
 
int test() __attribute__((alias("test1")));
 
int main()
{
  test();
  return 0;
}
