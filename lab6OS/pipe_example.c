#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    time_t current_time;
    struct tm *time_info;
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) { // Родительский процесс
        close(pipefd[0]); // Закрываем чтение
        
        current_time = time(NULL);
        time_info = localtime(&current_time);
        
        char time_str[BUFFER_SIZE];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        
        char message[BUFFER_SIZE];
        snprintf(message, sizeof(message), 
                "Время родителя: %s, PID родителя: %d", 
                time_str, getpid());
        
        printf("Родительский процесс (PID: %d) отправляет: %s\n", 
               getpid(), message);
        
        write(pipefd[1], message, strlen(message) + 1);
        close(pipefd[1]); // Закрываем запись
        
        wait(NULL);
        
    } else { // Дочерний процесс
        close(pipefd[1]); // Закрываем запись
        
        sleep(5);
        
        read(pipefd[0], buffer, BUFFER_SIZE);
        close(pipefd[0]); // Закрываем чтение
        
        current_time = time(NULL);
        time_info = localtime(&current_time);
        char child_time_str[BUFFER_SIZE];
        strftime(child_time_str, sizeof(child_time_str), 
                "%Y-%m-%d %H:%M:%S", time_info);
        
        printf("Дочерний процесс (PID: %d):\n", getpid());
        printf("  Получено от родителя: %s\n", buffer);
        printf("  Время дочернего процесса: %s\n", child_time_str);
    }
    
    return 0;
}