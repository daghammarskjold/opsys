// skalman.c -- a bad stupid Unix shell
// Thomas Padron-McCarthy (thomas.padron-mccarthy@oru.se)
// Thu Apr 25 11:42:55 CEST 2019

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 1000
#define MAX_WORD_LENGTH 100
#define MAX_WORDS 100

// Splits "Hi,   Ho! Silver!!!   " into "Hi,", "Ho!" and "Silver!!!",
// Returns the number of words in the result (3 in the example above).
int split_line(char *line, char words[][MAX_WORD_LENGTH + 1]) {
    int linepos = 0, wordnr = 0;
    while (line[linepos] != '\0' && isspace(line[linepos]))
        ++linepos;
    while (line[linepos] != '\0') {
        if (wordnr >= MAX_WORDS) {
            printf("skalman: Warning - too many words on line. Max is %d.\n", MAX_WORDS);
            break;
        }
        int wordpos = 0;
        while(line[linepos] != '\0' && !isspace(line[linepos])) {
            if (wordpos >= MAX_WORD_LENGTH) {
                printf("skalman: Warning - word too long. Max length is %d.\n", MAX_WORD_LENGTH);
                break;
            }
            words[wordnr][wordpos++] = line[linepos++];
        }
        words[wordnr][wordpos] = '\0';
        ++wordnr;
        while (line[linepos] != '\0' && isspace(line[linepos]))
            ++linepos;
    }
    words[wordnr][0] = '\0';
    return wordnr;
} // split_line

// Executes a command that has been split into an array of words
void handle_command(char words[][MAX_WORD_LENGTH + 1], int nr_words) {
    if (strcmp(words[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(words[0], "chdir") == 0 || strcmp(words[0], "cd") == 0) {
        if (nr_words != 2)
            printf("skalman: chdir takes exactly one argument\n");
        else {
            if (chdir(words[1]) != 0)
                printf("skalman: Couldn't chdir to '%s'. Error %d = %s\n", words[1], errno, strerror(errno));
        }
    }
    else if (strcmp(words[0], "fork") == 0){

        // Start a new process, start the given command, wait for it to finish!
        // Perhaps useful hint: Does the array "words" have the right structure for execv et al.?
        // A shell should give useful feedback, so don't forget error checking!

        // For extra points: Handle I/O redirect with "<" and ">"

        if (nr_words < 2) //has to be more than two arguments
            printf("skalman: fork takes more than one arguments\n");
        else{
            
           char *temp[nr_words];
            //create the temp array depending on the amout of arguments
            for(int i = 0; i < nr_words; ++i){
                if (i == (nr_words -1)){
                    temp[i] = NULL;
                }
                else{
                    temp[i] = words[i+1];
                }
            } 

            printf("skalman: forking...\n");
            int childpid = fork();
            printf("skalman: forked\n");
            if (childpid == 0){
                execv(words[1], temp); //execute command to to start program
                printf("skalman: execv failed!\n");
            }
            else{
                int waitstatus;
                printf("skalman: parent waiting...\n");
                waitpid(childpid, &waitstatus, 0); //wait for child
                printf("skalman: parent done!\n");
            }
        }
    }
    else if(strcmp(words[1], ">") == 0){
        if (nr_words != 3)
            printf("skalman: you need 3 arguments!\n");
        else{

            int fp = open(words[2], O_CREAT | O_WRONLY | O_TRUNC, 0600); //open the destination file
            if (fp == -1){
                printf("skalman: file couldn't open (output)\n");
                perror("open()");
            }

            char *temp[] = {words[0], NULL};
            int childpid = fork();
            if (childpid == 0){
                dup2(fp, 1);
                execv(words[0], temp);
                printf("skalman: error execv2\n"); 
            }
            else{
                int waitstatus;
                printf("skalman: parent waiting...\n");
                waitpid(childpid, &waitstatus, 0);
                close(fp);
                printf("skalman: parent done!\n");
            }
        }
    }
    else if(strcmp(words[1], "<") == 0){
        if (nr_words != 3)
            printf("skalman: you need 3 words!\n");
        else{
            char *temp[] = {words[0], NULL};
            int childpid = fork();
            if (childpid == 0){
                if (freopen(words[2], "r", stdin) != NULL){
                    execv(words[0], temp);
                    printf("skalman: error execv2\n"); 
                }
                else{
                    printf("could not reassign standard input\n");
                    perror("freopen()");
                }
                
            }
            else{
                int waitstatus;
                printf("skalman: parent waiting...\n");
                waitpid(childpid, &waitstatus, 0);
                printf("skalman: parent done!\n");
            }
        }
    }
    else {
        printf("too far\n");
    }
} // handle_command

int main(void) {
    printf("Skalman shell starting. Exit with exit or EOF.\n");
    char command_line[MAX_LINE_LENGTH + 1 + 1]; // +2 for '\n' and '\0'
    char command_words[MAX_WORDS + 1][MAX_WORD_LENGTH + 1]; // +1 for NULL and +1 for '\0'
    while (printf("skalman> "), fflush(stdout), fgets(command_line, sizeof command_line, stdin) != NULL) {
        if (strlen(command_line) == MAX_LINE_LENGTH + 1 && command_line[MAX_LINE_LENGTH] != '\n' && !feof(stdin))
            printf("skalman: Warning - line too long. Max length is %d.\n", MAX_LINE_LENGTH);
        int nr_words = split_line(command_line, command_words);
        if (nr_words == 0)
            printf("skalman: Empty command line.\n");
        else {

            // Some output to simplify debugging
            printf("skalman: Command has %d words: [ ", nr_words);
            for (int i = 0; i < nr_words; ++i) {
                printf("'%s'", command_words[i]);
                if (i < nr_words - 1)
                    printf(", ");
            }
            printf(" ]\n");

            handle_command(command_words, nr_words);
        }
    }
    return EXIT_SUCCESS;
} // main