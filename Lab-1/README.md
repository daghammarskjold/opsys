Usage (fork):

I made it so if you want to execute a program in a child process, you have to write fork before that program in the skalman command-line;

fork ls
fork ls -al
fork sleep 3
fork echo Heigh-ho Heigh-ho It's home from work we go
fork exit

(fork exit is a bit pointless, if it is executed, a child process is started, but then immedietly exited with exit(EXIT_SUCCESS). The normal exit command works good for stopping a child process aswell as the original parent process)


Usage (<, >):

./outputfile > target.txt

./inputfile < target.txt

daghammarskjold
