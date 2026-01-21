#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/file.h>
#include <fcntl.h>
#include <string.h>

#define SHM_KEY 0x1234

typedef struct {
    pid_t sender_pid;
    char text[128];
} shm_data_t;

int main() {
    int lock_fd = open("/tmp/sysv_sender_lock", O_CREAT | O_RDWR, 0644);
    if (lock_fd < 0) {
        perror("lock file");
        exit(1);
    }

    if (flock(lock_fd, LOCK_EX | LOCK_NB) != 0) {
        printf("Передатчик уже запущен\n");
        exit(0);
    }

    int shmid = shmget(SHM_KEY, sizeof(shm_data_t),
                       IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    shm_data_t *data = shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Sender (PID=%d) started\n", getpid());

    while (1) {
        time_t now = time(NULL);
        struct tm *tm = localtime(&now);

        data->sender_pid = getpid();
        snprintf(data->text, sizeof(data->text),
                 "%02d:%02d:%02d",
                 tm->tm_hour, tm->tm_min, tm->tm_sec);

        sleep(1);
    }

    return 0;
}
