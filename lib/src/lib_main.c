/*
описание файла: реализация функций создания, удаления хеш-таблицы
и вычисления хеш-функции для строк.

Бабурин Дмитрий Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdlib.h>
#include <string.h>

// простая и надежная хеш-функция fnv-1a для строк
unsigned int hash_function(const char* key, size_t capacity) {
    unsigned int hash = 2166136261u;
    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= 16777619u;
    }
    return hash % capacity;
}

// создание новой хеш-таблицы
HashTable* hash_table_create(size_t initial_capacity) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) {
        return NULL;
    }
    // выделяем массив бакетов и сразу зануляем его с помощью calloc
    table->buckets = (Node**)calloc(initial_capacity, sizeof(Node*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    table->capacity = initial_capacity;
    table->size = 0;
    return table;
}

// полное удаление таблицы и очистка всей памяти
void hash_table_destroy(HashTable* table) {
    if (!table) {
        return;
    }
    // проходим по каждому бакету
    for (size_t i = 0; i < table->capacity; i++) {
        Node* current = table->buckets[i];
        // удаляем все узлы в связном списке (цепочке коллизий)
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp->key); // освобождаем скопированную строку
            free(temp);      // освобождаем сам узел
        }
    }
    free(table->buckets); // освобождаем массив бакетов
    free(table);          // освобождаем структуру таблицы
}