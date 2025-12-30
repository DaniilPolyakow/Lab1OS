#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define READERS_COUNT 10
#define ARRAY_SIZE 64
#define ITERATIONS 10

char shared_array[ARRAY_SIZE];
int counter = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;
int data_ready = 0;

void* writer_thread(void* arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);

        counter++;
        snprintf(shared_array, ARRAY_SIZE,
                 "Запись номер: %d", counter);

        data_ready = 1;

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}

void* reader_thread(void* arg) {
    long tid = (long)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);

        while (!data_ready)
            pthread_cond_wait(&cond, &mutex);

        printf("Читатель TID=%ld | Данные: \"%s\"\n",
               tid, shared_array);

        data_ready = 0;

        pthread_mutex_unlock(&mutex);
        usleep(300000);
    }
    return NULL;
}

int main() {
    pthread_t readers[READERS_COUNT];
    pthread_t writer;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    strcpy(shared_array, "Пусто");

    pthread_create(&writer, NULL, writer_thread, NULL);

    for (long i = 0; i < READERS_COUNT; i++) {
        pthread_create(&readers[i], NULL,
                       reader_thread, (void*)i);
    }

    pthread_join(writer, NULL);

    for (int i = 0; i < READERS_COUNT; i++)
        pthread_join(readers[i], NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
