#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_NAME "time_fifo"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    time_t current_time;
    struct tm *time_info;
    
    printf("Процесс 2 (PID: %d) ожидает данные...\n", getpid());
   
    sleep(10);
    
    fd = open(FIFO_NAME, O_RDONLY);
    
    read(fd, buffer, BUFFER_SIZE);
    close(fd);
    
    current_time = time(NULL);
    time_info = localtime(&current_time);
    char receiver_time_str[BUFFER_SIZE];
    strftime(receiver_time_str, sizeof(receiver_time_str), 
            "%Y-%m-%d %H:%M:%S", time_info);
    
    printf("Процесс 2 (PID: %d):\n", getpid());
    printf("  Получено: %s\n", buffer);
    printf("  Время получателя: %s\n", receiver_time_str);
    
    unlink(FIFO_NAME);
    
    return 0;
}