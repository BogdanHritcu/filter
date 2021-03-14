/*
A21. (4 puncte) Scrieti un program "filtru" care se lanseaza sub forma:
     filtru f1 ... fn
 unde f1, ..., fn sunt fisiere executabile de pe disc si lanseaza procesul:
     f1 | ... | fn
*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Prea putine argumente!\n");
        return 1;
    }

    char* argv_n[2];
    argv_n[1] = NULL;

    if(argc == 2)
    {
        argv_n[0] = argv[1];
        execv(argv[1], argv_n);
        perror("execv");
        return 1;
    }   

    int d1[2];
    int d2[2];

    int nr_proc = argc - 1;
    int i;

    pipe(d1); //un tub temporar

    for(i = 0; i < nr_proc - 1; i++) //creeaza cate un proces pentru toate programele din argv, mai putin ultimul
    {
        pipe(d2);

        if(fork())
        {
            close(d1[0]);
            close(d1[1]);

            d1[0] = d2[0];
            d1[1] = d2[1];
        }
        else
        {
            if(i == 0) //daca este primul proces
            {
                //shimba stdout
                close(1);
                dup(d2[1]);
            }
            else
            {
                //schimba stdin
                close(0);
                dup(d1[0]);

                //schimba stdout
                close(1);
                dup(d2[1]);
            }

            close(d1[0]);
            close(d1[1]);
            close(d2[0]);
            close(d2[1]);

            argv_n[0] = argv[i + 1];
            execv(argv[i + 1], argv_n);
            perror("execv");
            return 1;
        }   
    }

    //creeaza ultimul proces prin substituirea procesului filtru cu el
    //schimba stdin
    close(0);
    dup(d1[0]);

    close(d1[0]);
    close(d1[1]);

    argv_n[0] = argv[i + 1];
    execv(argv[i + 1], argv_n);
    perror("execv");
    return 1;
}
