#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <string>
#include <unistd.h>

#define SEMNUMBER 5
#define LOOPCOUNT 5

int P(int, int);
int V(int, int);

int main(int argc, char *argv[])
{
    key_t kluczSemafor, kluczMemory;
    int semID;
    int shmID;
    int noFilozofa;
    int *widelec;
    if( (kluczSemafor = ftok(".",'A')) == -1 )
    {
        printf("Blad ftok A(filozof.cpp)\n");
        exit(2);
    }
    if( (kluczMemory = ftok(".",'B')) == -1 )
    {
        printf("Blad ftok B(filozof.cpp)\n");
        exit(2);
    }
    if( (semID = semget(kluczSemafor, SEMNUMBER, IPC_CREAT | 0666)) == -1)
    {
        printf("Blad semget(filozof.cpp)\n");
        exit(2);
    }
    if( (shmID = shmget(kluczMemory,SEMNUMBER* sizeof(int), IPC_CREAT|0666)) == -1 )
    {
        printf("Blad shmget(filozof.cpp)\n");
        exit(1);
    }
    fflush(stdout);
    widelec = (int*)shmat(shmID,NULL,0);
    noFilozofa = atoi(argv[1]);
    int nf2 = noFilozofa + 1;

    for (int i = 0; i < LOOPCOUNT; ++i)
    {
        printf("Filozof %d mysli...\n", nf2);
	
        if(noFilozofa == 0)
        {
            P(semID,noFilozofa);                //czekanie na lewy widelec
            P(semID,(noFilozofa+1)%SEMNUMBER);  //czekanie na prawy widelec
        }
        else
        {
            P(semID,(noFilozofa+1)%SEMNUMBER);  //czekanie na prawy widelec
            P(semID,noFilozofa);                //czekanie na lewy widelec
        }
        widelec[(noFilozofa+1)%SEMNUMBER] = nf2;
        widelec[noFilozofa] = nf2;
        printf("Filozof %d je\n", nf2);
        
        printf("\nTablica widelcow\n");
        for(int j=0; j<SEMNUMBER; j++)
            printf("%d ",widelec[j]);
        printf("\n\n");
	sleep(1); 
        widelec[(noFilozofa+1)%SEMNUMBER] = 0;
        widelec[noFilozofa] = 0;
        V(semID,noFilozofa);
        V(semID,(noFilozofa+1)%SEMNUMBER);

    }

}

int P(int semID, int number)
{
    struct sembuf operacje;
    operacje.sem_num = number;
    operacje.sem_op = -1;
    operacje.sem_flg = SEM_UNDO;
    if(semop(semID,&operacje, 1) == -1)
    {
        perror("Blad semop(P)\n");
        return -1;
    }
    return 1;
}

int V(int semID, int number)
{
    struct sembuf operacje;
    operacje.sem_num = number;
    operacje.sem_op = 1;
    operacje.sem_flg = SEM_UNDO;
    if(semop(semID,&operacje, 1) == -1)
    {
        perror("Blad semop(V)\n");
        return -1;
    }
    return 1;
}
