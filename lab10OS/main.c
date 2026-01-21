#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define READERS_COUNT 10
#define ARRAY_SIZE 64
#define WRITE_ITERATIONS 20

char shared_array[ARRAY_SIZE];
pthread_rwlock_t rwlock;

void *writer_thread(void *arg)
{
    (void)arg;
    static int counter = 0;

    for (int i = 0; i < WRITE_ITERATIONS; i++) {
        pthread_rwlock_wrlock(&rwlock);

        counter++;
        snprintf(shared_array, ARRAY_SIZE, "Запись № %d", counter);

        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }

    return NULL;
}

void *reader_thread(void *arg)
{
    long id = (long)arg;

    while (1) {
        pthread_rwlock_rdlock(&rwlock);

        printf("Читатель TID=%ld | Данные: \"%s\"\n",
               id, shared_array);

        pthread_rwlock_unlock(&rwlock);
        usleep(300000);
    }

    return NULL;
}

int main(void)
{
    pthread_t readers[READERS_COUNT];
    pthread_t writer;

    pthread_rwlock_init(&rwlock, NULL);
    snprintf(shared_array, ARRAY_SIZE, "Начальное состояние");

    for (long i = 0; i < READERS_COUNT; i++) {
        pthread_create(&readers[i], NULL, reader_thread, (void *)i);
    }

    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_join(writer, NULL);

    pthread_rwlock_destroy(&rwlock);
    return 0;
}
