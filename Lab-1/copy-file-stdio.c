#include <stdio.h>

int main(int argc, char *argv[]){

    if (argc != 3){
        printf("copy-file-stdio: you need 2 parameters!\n");
        return 1;
    }

    FILE * fpo, * fpt;
    fpo = fopen(argv[1], "r");
    fpt = fopen(argv[2], "w");

    int c;
    while ((c = getc(fpo)) != EOF){
        putc(c, fpt);
    }
    return 0;
}