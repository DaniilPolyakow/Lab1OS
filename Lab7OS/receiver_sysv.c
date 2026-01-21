#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define SHM_KEY 0x1234

typedef struct {
    pid_t sender_pid;
    char text[128];
} shm_data_t;

int main() {
    int shmid = shmget(SHM_KEY, sizeof(shm_data_t), 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    shm_data_t *data = shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Receiver started (PID=%d)\n", getpid());

    while (1) {
        time_t now = time(NULL);
        struct tm *tm = localtime(&now);

        printf("[Receiver %d | %02d:%02d:%02d] "
               "Sender PID=%d, time=%s\n",
               getpid(),
               tm->tm_hour, tm->tm_min, tm->tm_sec,
               data->sender_pid,
               data->text);

        sleep(1);
    }

    return 0;
}
