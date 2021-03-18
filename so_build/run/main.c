#include "./skfapi.h"
#include <stdio.h>

int main() {
    ULONG ulRslt;
    char szDevName[256] = {0};
    ULONG ulNameLen = 256;
    ulRslt = SKF_EnumDev(1, szDevName, &ulNameLen);
    printf("%s", szDevName);
    return ulRslt;
}
