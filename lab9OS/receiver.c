#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

int shmid, semid;
char* shm;

void sem_op(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    semop(semid, &sb, 1);
}

void cleanup(int sig) {
    shmdt(shm);          
    _exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    shmid = shmget(SHM_KEY, 256, 0666);
    shm = shmat(shmid, NULL, 0);

    semid = semget(SEM_KEY, 1, 0666);

    while (1) {
        time_t now = time(NULL);

        sem_op(semid, -1);
        printf("Приёмник: pid=%d, time=%s",
               getpid(), ctime(&now));
        printf("Принято: %s\n", shm);
        sem_op(semid, 1);

        sleep(3);
    }
}
