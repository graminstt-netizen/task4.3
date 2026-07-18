/*
описание файла: реализация функций создания, удаления хеш-таблицы
и вычисления хеш-функции для строк.

Бабурин Дмитрий Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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

// полноценное перестроение (рехеширование) таблицы при увеличении ее емкости
void hash_table_resize(HashTable* table, size_t new_capacity) {
    if (!table || new_capacity == 0) {
        return;
    }

    // 1. выделяем новый массив бакетов большего размера
    Node** new_buckets = (Node**)calloc(new_capacity, sizeof(Node*));
    if (!new_buckets) {
        return; // если память не выделилась, продолжаем работать со старой таблицей
    }

    // 2. переносим ВСЕ существующие узлы в новый массив БЕЗ перевыделения памяти
    for (size_t i = 0; i < table->capacity; i++) {
        Node* current = table->buckets[i];
        while (current) {
            Node* next_node = current->next; // запоминаем следующий узел в старой цепочке

            // вычисляем новый индекс по новой емкости
            unsigned int new_index = hash_function(current->key, new_capacity);

            // вставляем узел в начало цепочки нового бакета
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next_node; // переходим к следующему узлу из старой цепочки
        }
    }

    // 3. удаляем старый массив бакетов и обновляем структуру
    free(table->buckets);
    table->buckets = new_buckets;
    table->capacity = new_capacity;
}

// удаление узла из таблицы по ключу
int hash_table_remove(HashTable* table, const char* key) {
    if (!table || !key) {
        return 0;
    }

    unsigned int index = hash_function(key, table->capacity);
    Node* current = table->buckets[index];
    Node* prev = NULL;

    // ищем узел в цепочке коллизий
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // если удаляем первый элемент в цепочке
            if (prev == NULL) {
                table->buckets[index] = current->next;
            } else {
                // если удаляем из середины или конца цепочки
                prev->next = current->next;
            }
            
            // освобождаем память удаляемого узла
            free(current->key);
            free(current);
            table->size--;
            return 1; // успешно удалено
        }
        prev = current;
        current = current->next;
    }

    return 0; // элемент не найден
}

// чтение файла, разбиение текста на слова и добавление их в таблицу
int process_text_file(HashTable* table, const char* filename) {
    if (!table || !filename) {
        return 0;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return 0;
    }

    char buffer[256];
    size_t length = 0;
    int c;

    while ((c = fgetc(file)) != EOF) {
        if (isalnum(c)) {
            // собираем слово по символам и приводим к нижнему регистру
            if (length < sizeof(buffer) - 1) {
                buffer[length++] = (char)tolower(c);
            }
        } else {
            // разделитель найден, записываем готовое слово в таблицу
            if (length > 0) {
                buffer[length] = '\0';
                hash_table_insert(table, buffer);
                length = 0;
            }
        }
    }

    // обрабатываем слово на конце файла, если разделителя не было
    if (length > 0) {
        buffer[length] = '\0';
        hash_table_insert(table, buffer);
    }

    fclose(file);
    return 1;
}