#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEMNUMBER 5

int main(int argc, char *argv[])
{
    key_t kluczSemafor, kluczMemory;
    int semID;
    int shmID;
    char buffor[3];
    int *widelec;
    if( (kluczSemafor = ftok(".",'A')) == -1 )
    {
        printf("Blad ftok(main)\n");
        exit(2);
    }
    semID = semget(kluczSemafor, SEMNUMBER, IPC_CREAT | IPC_EXCL | 0666);
    for (int i = 0; i < SEMNUMBER; ++i)
    {
        semctl(semID,i,SETVAL,1);
    }

    kluczMemory = ftok(".",'B');
    shmID = shmget(kluczMemory,5* sizeof(int), IPC_CREAT|IPC_EXCL|0666);
    if(shmID == -1)
    {
        printf("Blad shmget(main\n");
        exit(1);
    }
    fflush(stdout);
    widelec = (int*)shmat(shmID,NULL,0);
    for (short i = 0; i < SEMNUMBER; ++i)
    {
        widelec[i] = 0;
    }
    for (short i = 0; i < SEMNUMBER; ++i)
    {
        switch(fork())
        {
            case -1:
                perror("Blad fork(main)\n");
                exit(2);
            case 0:
                sprintf(buffor,"%d",i);
                execl("./filozof", "filozof", buffor, NULL);
            default:
                break;
        }
    }
    for (int i = 0; i < SEMNUMBER; ++i)
    {
        wait(NULL);
    }
    semctl(semID, SEMNUMBER, IPC_RMID,NULL);
    shmctl(shmID,IPC_RMID, NULL);
    printf("KONIEC main\n");
    return 0;
}
