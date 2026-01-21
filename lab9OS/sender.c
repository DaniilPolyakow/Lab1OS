#include <signal.h>

int shmid, semid;
char* shm;

void cleanup(int sig) {
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    _exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    shm = shmat(shmid, NULL, 0);

    semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
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
}
