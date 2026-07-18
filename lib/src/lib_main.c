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

// вспомогательная функция копирования строк, чтобы избежать предупреждений компилятора о strdup
static char* string_duplicate(const char* s) {
    size_t len = strlen(s) + 1;
    char* d = (char*)malloc(len);
    if (d) {
        memcpy(d, s, len);
    }
    return d;
}

// поиск узла по строковому ключу
Node* hash_table_find(HashTable* table, const char* key) {
    if (!table || !key) {
        return NULL;
    }
    // определяем индекс бакета
    unsigned int index = hash_function(key, table->capacity);
    Node* current = table->buckets[index];
    
    // идем по списку цепочки, пока не найдем или не дойдем до конца
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current; // нашли!
        }
        current = current->next;
    }
    return NULL; // не нашли
}

// вставка ключа в таблицу
Node* hash_table_insert(HashTable* table, const char* key) {
    if (!table || !key) {
        return NULL;
    }

    // 1. проверяем, есть ли уже такое слово
    Node* existing = hash_table_find(table, key);
    if (existing) {
        existing->count++; // увеличиваем счетчик вхождений
        return existing;
    }

    // 2. если слова нет, нужно создать новый узел
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        return NULL;
    }
    new_node->key = string_duplicate(key);
    if (!new_node->key) {
        free(new_node);
        return NULL;
    }
    new_node->count = 1;

    // 3. вставляем узел в начало цепочки соответствующего бакета (метод цепочек)
    unsigned int index = hash_function(key, table->capacity);
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->size++;

    // 4. если таблица заполнена более чем на 75%, запускаем перестроение (рехеширование)
    if ((double)table->size / table->capacity >= 0.75) {
        hash_table_resize(table, table->capacity * 2);
    }

    return new_node;
}

// заглушка для функции перестроения (ее логику напишем на следующем шаге)
void hash_table_resize(HashTable* table, size_t new_capacity) {
    (void)table;
    (void)new_capacity;
}