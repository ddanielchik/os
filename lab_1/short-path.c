//
// Created by Медведева Даниэла on 10.10.2025.
// Поиск кратчайших путей (нагрузка на CPU)


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define INF 999999

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <количество_вершин>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("errorchik, количество вершин должно быть >0 \n");
        return 1;
    }

    // Выделяем память под матрицу графа
    int **graph = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        graph[i] = malloc(n * sizeof(int));

    srand(time(NULL));

    // Генерируем случайные веса (0 = нет ребра)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j)
                graph[i][j] = 0;
            else
                graph[i][j] = (rand() % 10) + 1; // вес от 1 до 10
        }
    }

    // Алгоритм Дейкстры
    int *dist = malloc(n * sizeof(int));
    int *visited = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++)
        dist[i] = INF;

    dist[0] = 0;

    clock_t start = clock();

    for (int count = 0; count < n - 1; count++) {
        // ищем вершину с минимальной дистанцией
        int min = INF, u = -1;
        for (int i = 0; i < n; i++)
            if (!visited[i] && dist[i] <= min)
                min = dist[i], u = i;

        visited[u] = 1;

        // обновляем расстояния до соседей
        for (int v = 0; v < n; v++)
            if (!visited[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v])
                dist[v] = dist[u] + graph[u][v];
    }

    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Вычисление кратчайших путей завершено.\n");
    printf("Количество вершин: %d\n", n);
    printf("Время выполнения: %.3f сек\n", time_spent);

    // Освобождаем память
    for (int i = 0; i < n; i++)
        free(graph[i]);
    free(graph);
    free(dist);
    free(visited);

    return 0;
}
// Компиляция: gcc short-path.c -o short-path
// Запуск: ./short-path 500
// (500 — количество вершин)
// time ./short-path 500
