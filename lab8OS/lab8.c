#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define READERS_COUNT 10
#define ARRAY_SIZE 64

char shared_array[ARRAY_SIZE];
int counter = 0;

pthread_mutex_t mutex;

void* writer_thread(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);

        counter++;
        snprintf(shared_array, ARRAY_SIZE, "Запись №%d", counter);

        pthread_mutex_unlock(&mutex);

        sleep(1);  
    }
    return NULL;
}

void* reader_thread(void* arg)
{
    long tid = (long)arg;

    while (1)
    {
        pthread_mutex_lock(&mutex);

        printf("Читающий поток TID=%ld | Данные: %s\n",
               tid, shared_array);

        pthread_mutex_unlock(&mutex);

        usleep(500000); 
    }
    return NULL;
}

int main()
{
    pthread_t writer;
    pthread_t readers[READERS_COUNT];

    pthread_mutex_init(&mutex, NULL);
    strcpy(shared_array, "Начальное состояние");

    pthread_create(&writer, NULL, writer_thread, NULL);

    for (long i = 0; i < READERS_COUNT; i++)
    {
        pthread_create(&readers[i], NULL, reader_thread, (void*)i);
    }

    pthread_join(writer, NULL);
    for (int i = 0; i < READERS_COUNT; i++)
    {
        pthread_join(readers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
 