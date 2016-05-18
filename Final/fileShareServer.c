#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    

    if (argc != 2)
    {
        printf("Usage: ./client <ip address>\n");
        exit(EXIT_FAILURE);
    }

    printf("-> %s\n", argv[1]);

    return 0;
}