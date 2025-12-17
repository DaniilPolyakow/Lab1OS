#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>

#define SHM_SIZE 256
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

void sem_op(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    semop(semid, &sb, 1);
}

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    char* shm = shmat(shmid, NULL, 0);

    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1);

    while (1) {
        time_t now = time(NULL);

        sem_op(semid, -1);
        snprintf(shm, SHM_SIZE,
                 "Отправитель: pid=%d, time=%s",
                 getpid(), ctime(&now));
        sem_op(semid, 1);

        sleep(3);
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
