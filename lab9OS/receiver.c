#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

void sem_op(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    semop(semid, &sb, 1);
}

int main() {
    int shmid = shmget(SHM_KEY, 256, 0666);
    char* shm = shmat(shmid, NULL, 0);

    int semid = semget(SEM_KEY, 1, 0666);

    while (1) {
        time_t now = time(NULL);

        sem_op(semid, -1);
        printf("Приёмник: pid=%d, time=%s",
               getpid(), ctime(&now));
        printf("Принято: %s\n", shm);
        sem_op(semid, 1);

        sleep(3);
    }

    shmdt(shm);
    return 0;
}
