#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define READERS_COUNT 10
#define ARRAY_SIZE 64
#define ITERATIONS 10

char shared_array[ARRAY_SIZE];
pthread_rwlock_t rwlock;
int counter = 0;

void* writer_thread(void* arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_rwlock_wrlock(&rwlock);

        counter++;
        snprintf(shared_array, ARRAY_SIZE, "Запись номер: %d", counter);

        pthread_rwlock_unlock(&rwlock);

        sleep(1);
    }
    return NULL;
}

void* reader_thread(void* arg) {
    long tid = (long)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_rwlock_rdlock(&rwlock);

        printf("Читатель TID=%ld | Данные: \"%s\"\n",
               tid, shared_array);

        pthread_rwlock_unlock(&rwlock);

        usleep(300000);
    }
    return NULL;
}

int main() {
    pthread_t readers[READERS_COUNT];
    pthread_t writer;

    pthread_rwlock_init(&rwlock, NULL);
    strcpy(shared_array, "Пусто");

    pthread_create(&writer, NULL, writer_thread, NULL);

    for (long i = 0; i < READERS_COUNT; i++) {
        pthread_create(&readers[i], NULL, reader_thread, (void*)i);
    }

    pthread_join(writer, NULL);

    for (int i = 0; i < READERS_COUNT; i++) {
        pthread_join(readers[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);

    return 0;
}
