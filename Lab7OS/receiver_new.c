#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
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

    int fd = shm_open(MEM_OBJECT, O_RDWR, 0666);
    if (fd < 0) {
        perror("receiver: cannot open shared memory");
        exit(1);
    }

    memchunk_t *mem = mmap(NULL, sizeof(memchunk_t),
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED, fd, 0);

    if (mem == MAP_FAILED) {
        perror("receiver mmap");
        exit(1);
    }

    printf("Receiver online (PID=%d)\n", getpid());

    while (1) {
        time_t t = time(NULL);
        struct tm *info = localtime(&t);

        char now_str[32];
        snprintf(now_str, sizeof(now_str),
                 "%02d:%02d:%02d",
                 info->tm_hour, info->tm_min, info->tm_sec);

        pthread_mutex_lock(&mem->lock);
        printf("[Receiver %d at %s] → %s\n",
               getpid(), now_str, mem->text);
        pthread_mutex_unlock(&mem->lock);

        usleep(800000); // 
    }

    return 0;
}
