// runner.c

// Сборка: gcc -std=c11 -O0 -Wall -Wextra runner.c -o runner

// usage: ./runner <path-to-ema_sort> <instances> -- [args-for-ema_sort]

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/wait.h>



int main(int argc, char **argv){

    if(argc < 3){ fprintf(stderr,"Usage: %s <program> <instances> -- [args]\n", argv[0]); return 1; }

    char *prog = argv[1];

    int instances = atoi(argv[2]);

    int argstart = 3;

    if(argc > argstart && strcmp(argv[argstart], "--")==0) argstart++;



    for(int i=0;i<instances;i++){

        pid_t pid = fork();

        if(pid == -1){ perror("fork"); return 2; }

        if(pid == 0){

            // child

            // prepare argv for exec: prog, optional args..., NULL

            int newargc = argc - argstart + 1;

            char **nargv = malloc((newargc+1)*sizeof(char*));

            nargv[0] = prog;

            for(int j=argstart;j<argc;j++) nargv[j - argstart + 1] = argv[j];

            nargv[newargc] = NULL;

            execv(prog, nargv);

            perror("execv");

            _exit(127);

        }

        // parent continues to spawn next

    }



    // wait for all children

    int status;

    while(wait(&status) > 0) { /* collect all */ }

    return 0;

}

