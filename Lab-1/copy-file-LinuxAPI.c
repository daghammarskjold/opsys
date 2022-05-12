#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    int fpc, fpt;
    fpc = open(argv[1], O_RDONLY);
    fpt = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0600);
    char c;
    while(read(fpc, &c, 1) == 1){
        write(fpt, &c, 1);
    }
    close(fpc);
    close(fpt);
    }