// pannshell.c
// Компиляция: gcc main.c -o pannshell
// Запуск: ./pannshell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // для fork, execvp
#include <sys/wait.h>   // для waitpid
#include <time.h>       // для измерения времени

#define MAX_INPUT 1024
#define MAX_ARGS 64

// проверяем операторы(штучки) и запуск команд
int run_with_operators(char *input) {

    //штучка ";"
    char *p = strstr(input, ";");
    if (p) {
        *p = '\0';
        char *cmd1 = input;
        char *cmd2 = p + 1;
        system(cmd1);
        system(cmd2);
        return 0;
    }

    // штучка "&&"
    p = strstr(input, "&&");
    if (p) {
        *p = '\0';
        char *cmd1 = input;
        char *cmd2 = p + 2;
        int res = system(cmd1);
        if (res == 0)
            system(cmd2);
        return 0;
    }

    // штучка "||"
    p = strstr(input, "||");
    if (p) {
        *p = '\0';
        char *cmd1 = input;
        char *cmd2 = p + 2;
        int res = system(cmd1);
        if (res != 0)
            system(cmd2);
        return 0;
    }

    // штучка "&"
    p = strstr(input, "&");
    if (p) {
        *p = '\0';
        char *cmd1 = input;
        pid_t pid = fork();
        if (pid == 0) {
            setsid();
            system(cmd1);
            exit(0);
        } else if (pid > 0) {
            printf("Фоновый процесс PID %d запущен\n", pid);
        } else {
            perror("Ошибка fork");
        }
        return 0;
    }

    return 1; // если операторов нет — вернуть 1, чтобы выполнить как обычную команду
}


int main() {
    char input[MAX_INPUT];  // строка ввода
    char *args[MAX_ARGS];   // массив аргументов
    char *token;
    pid_t pid;
    int status;

    printf("Selam, ашкым\n");
    printf("Копируй 'exit' если хощ уйти отседа\n");

    while (1) {
        printf("pannshell> ");
        fflush(stdout);

        if (!fgets(input, MAX_INPUT, stdin))
            break; // EOF (Ctrl+D)

        input[strcspn(input, "\n")] = 0; // убираем '\n'

        if (strcmp(input, "exit") == 0)
            break; // выход из программы

        if (run_with_operators(input) == 0) {
            continue;
        }

        // Разбиваем строку на слова (разделитель — пробел)
        int i = 0;
        token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (args[0] == NULL)
            continue; // пустая строка


        // Засекаем время
        clock_t start = clock();

        pid = fork(); // создаём новый процесс

        if (pid == 0) {
            // Дочерний процесс — выполняем команду
            execvp(args[0], args);
            perror("Ошибка при выполнении команды");
            exit(1);
        } else if (pid > 0) {
            // Родитель ждёт завершения
            waitpid(pid, &status, 0);

            clock_t end = clock();
            double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
            printf("Время выполнения: %.3f сек.\n", elapsed);
        } else {
            perror("fork не удался");
        }
    }

    printf("Выход из shell.\n");
    return 0;
}



/* тестим эту штукенцию:
    тестик 1:
 echo first ;  вторая second
    тестик 2:
echo ok && echo done
    или
 false && echo это не работает

    тестик 3:
 echo false || echo это быль ощибка
    или
echo good || а это не выполнится
тест 4
sleep 50
*/


