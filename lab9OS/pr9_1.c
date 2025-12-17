#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define BUF_SIZE 128

char shared_buf[BUF_SIZE];
sem_t sem;

void* writer(void* arg) {
    int counter = 0;

    while (1) {
        sem_wait(&sem);
        snprintf(shared_buf, BUF_SIZE, "Запись № %d", counter++);
        sem_post(&sem);
        sleep(1);
    }
    return NULL;
}

void* reader(void* arg) {
    pthread_t tid = pthread_self();

    while (1) {
        sem_wait(&sem);
        printf("[Reader tid=%lu] buffer = \"%s\"\n",
               (unsigned long)tid, shared_buf);
        sem_post(&sem);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t w, r;

    sem_init(&sem, 0, 1);
    strcpy(shared_buf, "Пусто");

    pthread_create(&w, NULL, writer, NULL);
    pthread_create(&r, NULL, reader, NULL);

    pthread_join(w, NULL);
    pthread_join(r, NULL);

    sem_destroy(&sem);
    return 0;
}
