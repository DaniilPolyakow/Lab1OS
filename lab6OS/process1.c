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
    
    mkfifo(FIFO_NAME, 0666);
    
    current_time = time(NULL);
    time_info = localtime(&current_time);
    
    char time_str[BUFFER_SIZE];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
    
    snprintf(buffer, sizeof(buffer), 
            "Время отправителя: %s, PID отправителя: %d", 
            time_str, getpid());
    
    printf("Процесс 1 (PID: %d) отправляет: %s\n", getpid(), buffer);
    
    fd = open(FIFO_NAME, O_WRONLY);
    
    write(fd, buffer, strlen(buffer) + 1);
    close(fd);
    
    return 0;
}