#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

int check_if_octal(const char *mode_str) {
    for (int idx = 0; mode_str[idx] != '\0'; idx++) {
        if (mode_str[idx] < '0' || mode_str[idx] > '7') {
            return 0;
        }
    }
    return 1;
}

mode_t convert_octal_mode(const char *mode_str) {
    char *end;
    long result = strtol(mode_str, &end, 8);
    
    if (*end != '\0' || result < 0 || result > 07777) {
        fprintf(stderr, "Некорректный режим доступа: '%s'\n", mode_str);
        return (mode_t)-1;
    }
    
    return (mode_t)result;
}

int process_symbolic_part(const char *part, mode_t current, mode_t *updated) {
    const char *p = part;
    mode_t target_mask = 0;
    char action = 0;
    mode_t perm_bits = 0;
    int has_target = 0;

    while (*p && (*p == 'u' || *p == 'g' || *p == 'o' || *p == 'a')) {
        has_target = 1;
        switch (*p) {
            case 'u': target_mask |= S_IRWXU; break;
            case 'g': target_mask |= S_IRWXG; break;
            case 'o': target_mask |= S_IRWXO; break;
            case 'a': target_mask |= S_IRWXU | S_IRWXG | S_IRWXO; break;
        }
        p++;
    }
    
    if (!has_target) {
        target_mask = S_IRWXU | S_IRWXG | S_IRWXO;
    }
    
    if (*p == '+' || *p == '-' || *p == '=') {
        action = *p;
        p++;
    } else {
        fprintf(stderr, "Неправильный синтаксис в '%s'\n", part);
        return -1;
    }
    
    while (*p && (*p == 'r' || *p == 'w' || *p == 'x')) {
        switch (*p) {
            case 'r': perm_bits |= S_IRUSR | S_IRGRP | S_IROTH; break;
            case 'w': perm_bits |= S_IWUSR | S_IWGRP | S_IWOTH; break;
            case 'x': perm_bits |= S_IXUSR | S_IXGRP | S_IXOTH; break;
        }
        p++;
    }
    
    if (*p != '\0') {
        fprintf(stderr, "Неправильный синтаксис в '%s'\n", part);
        return -1;
    }
    
    switch (action) {
        case '+':
            *updated = current | (perm_bits & target_mask);
            break;
        case '-':
            *updated = current & ~(perm_bits & target_mask);
            break;
        case '=':
            *updated = (current & ~target_mask) | (perm_bits & target_mask);
            break;
    }
    
    return 0;
}

int interpret_symbolic_mode(const char *mode_str, mode_t current, mode_t *result) {
    char buffer[256];
    strncpy(buffer, mode_str, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    *result = current;
    
    char *segment = strtok(buffer, ",");
    while (segment != NULL) {
        if (process_symbolic_part(segment, *result, result) == -1) {
            return -1;
        }
        segment = strtok(NULL, ",");
    }
    
    return 0;
}

int modify_file_permissions(const char *filepath, const char *mode_str) {
    struct stat file_info;
    mode_t target_mode;
    
    if (stat(filepath, &file_info) == -1) {
        perror("Ошибка получения информации о файле");
        return -1;
    }
    
    if (check_if_octal(mode_str)) {
        target_mode = convert_octal_mode(mode_str);
        if (target_mode == (mode_t)-1) {
            return -1;
        }
    } else {
        if (interpret_symbolic_mode(mode_str, file_info.st_mode, &target_mode) == -1) {
            return -1;
        }
    }
    
    if (chmod(filepath, target_mode) == -1) {
        perror("Ошибка изменения прав");
        return -1;
    }
    
    printf("Права для '%s' обновлены\n", filepath);
    return 0;
}

int main(int arg_count, char *arg_values[]) {
    if (arg_count != 3) {
        fprintf(stderr, "Формат: %s <режим> <файл>\n", arg_values[0]);
        fprintf(stderr, "Варианты использования:\n");
        fprintf(stderr, "  %s +x файл\n", arg_values[0]);
        fprintf(stderr, "  %s u-w файл\n", arg_values[0]);
        fprintf(stderr, "  %s go+rw файл\n", arg_values[0]);
        fprintf(stderr, "  %s ug+rw файл\n", arg_values[0]);
        fprintf(stderr, "  %s a+rwx файл\n", arg_values[0]);
        fprintf(stderr, "  %s u=rwx,g=rx,o= файл\n", arg_values[0]);
        fprintf(stderr, "  %s 755 файл\n", arg_values[0]);
        return EXIT_FAILURE;
    }
    
    const char *mode_spec = arg_values[1];
    const char *target_file = arg_values[2];
    
    if (modify_file_permissions(target_file, mode_spec) == -1) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}