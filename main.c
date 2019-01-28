#include <windows.h>
#include <stdio.h>
#include <stdint.h>

void foo(void)
{
    char *tmp = "This is the first thing we have actually printed.\n";
    
    OutputDebugStringA("I print to the Debug Console only!\n");
    
    printf("%s\n", tmp);
}

int main(){
    printf("UINT8_MAX: %d\n", UINT8_MAX);

    unsigned char c = 0;
    for (; c <= UINT8_MAX; ++c) {
        printf("%d : %c\t", c, c);
        if (c % 8 ==0) printf("\n");
        if (c == UINT8_MAX) break;
    }

    foo();

    getch();
    return 0;
}