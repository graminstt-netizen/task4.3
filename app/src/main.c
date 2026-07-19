/*
main.c - главная программа консольного приложения, обрабатывает аргументы командной строки и запускает парсинг.

Бабурин Дмитрий Сергеевич
МК-101
*/

#include <stdio.h>
#include <stdlib.h>
#include "lib_main.h"

int main(int argc, char* argv[]) {
    // проверка наличия аргумента с путем к файлу
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    printf("Hash Table Word Counter started\n");
    printf("Reading file: %s\n", argv[1]);

    // создание хеш-таблицы с начальным размером бакетов 1024
    HashTable* table = hash_table_create(1024);
    if (!table) {
        fprintf(stderr, "Error: Cannot create hash table\n");
        return 1;
    }

    // запуск чтения и обработки файла
    if (!process_text_file(table, argv[1])) {
        hash_table_destroy(table);
        return 1;
    }

    // подсчет общего количества слов с учетом их частоты
    unsigned long long total_words = 0;
    for (size_t i = 0; i < table->capacity; i++) {
        Node* curr = table->buckets[i];
        while (curr) {
            total_words += curr->count;
            curr = curr->next;
        }
    }

    // вывод собранной статистики в консоль
    printf("\n--- Statistics ---\n");
    printf("Total words: %llu\n", total_words);
    printf("Unique words: %zu\n", table->size);
    printf("Table capacity: %zu\n", table->capacity);

    // вывод первых 15 найденных уникальных слов для демонстрации
    printf("\n--- Sample of found words ---\n");
    size_t printed_count = 0;
    for (size_t i = 0; i < table->capacity && printed_count < 15; i++) {
        Node* curr = table->buckets[i];
        while (curr && printed_count < 15) {
            printf("%s: %u\n", curr->key, curr->count);
            printed_count++;
            curr = curr->next;
        }
    }

    // очистка ресурсов и завершение программы
    hash_table_destroy(table);
    printf("\nMemory successfully cleaned up. Program finished.\n");

    return 0;
}