#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#if defined(_MSC_VER)
# define EXPORT __declspec(dllexport)
#else  // _MSC_VER
# define EXPORT __attribute__ ((visibility ("default")))
#endif


EXPORT int Secret(char* str)
{
    int i;
    unsigned char XOR[] = {0x51,0x42,0x44,0x49,0x46,0x72,0x69,0x64,0x61};
    size_t len = strlen(str);

    printf("Input string is : %s\nEncrypted string is : \n", str);

    for (i = 0; i < len; i++) {
        printf("0x%x,", str[i]^XOR[i%sizeof(XOR)]);
    }
    printf("\n");
    fflush(stdout);
    return 0;
}

void Hello()
{
    Secret("Hello world !");
}

int main()
{
    Hello();
}
