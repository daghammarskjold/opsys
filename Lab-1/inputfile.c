#include <stdio.h>

int main(){
    char input[2000];

    fgets(input, sizeof input, stdin);

    printf("From inputfile: %s\n", input);
}