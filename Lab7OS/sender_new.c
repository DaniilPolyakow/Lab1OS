#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define MEM_OBJECT "/shared_time_block"

typedef struct {
    pthread_mutex_t lock;
    char text[160];
} memchunk_t;

int main() {
    int lock_file = open("/tmp/time_sender_guard", O_CREAT | O_RDWR, 0644);
    if (lock_file < 0) {
        perror("cannot create guard file");
        exit(1);
    }

    if (flock(lock_file, LOCK_EX | LOCK_NB) != 0) {
        printf("Процесс-передатчик уже работает. Повторный запуск невозможен.\n");
        exit(0);
    }

    int fd = shm_open(MEM_OBJECT, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open failed");
        exit(1);
    }

    if (ftruncate(fd, sizeof(memchunk_t)) < 0) {
        perror("ftruncate");
        exit(1);
    }

    memchunk_t *mem = mmap(NULL, sizeof(memchunk_t),
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED, fd, 0);

    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&mem->lock, &mattr);

    printf("Sender started (PID: %d)\n", getpid());

    while (1) {
        time_t now = time(NULL);
        struct tm *ts = localtime(&now);

        char buffer[160];
        snprintf(buffer, sizeof(buffer),
                 "SRC %d :: %02d-%02d-%02d %02d:%02d:%02d",
                 getpid(),
                 ts->tm_mday, ts->tm_mon + 1, ts->tm_year + 1900,
                 ts->tm_hour, ts->tm_min, ts->tm_sec);

        pthread_mutex_lock(&mem->lock);
        strcpy(mem->text, buffer);
        pthread_mutex_unlock(&mem->lock);

        sleep(1);
    }

    return 0;
}
