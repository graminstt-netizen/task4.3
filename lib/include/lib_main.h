/*
lib_main.h - заголовочный файл модуля lib_main.c. 

Бабурин Дмитрий Сергеевич
МК-101
*/


#ifndef LIB_MAIN_H
#define LIB_MAIN_H

#include <stddef.h>


// узел нашей хеш-таблицы (элемент связного списка)
typedef struct Node {
    char* key;            // само слово (строка, выделенная динамически)
    unsigned int count;   // сколько раз встретилось это слово
    struct Node* next;    // указатель на следующий узел при коллизии (метод цепочек)
} Node;

// структура хеш-таблицы
typedef struct HashTable {
    Node** buckets;       // динамический массив указателей на узлы (головы списков)
    size_t capacity;      // текущий размер массива бакетов
    size_t size;          // количество уникальных элементов (ключей) в таблице
} HashTable;

// создание новой хеш-таблицы с начальной емкостью
HashTable* hash_table_create(size_t initial_capacity);

// полное удаление хеш-таблицы и всей занятой памяти (включая ключи и узлы)
void hash_table_destroy(HashTable* table);

// добавление слова в таблицу, если слово уже есть: увеличиваем счетчик count на 1.
// возвращает указатель на узел (найденный или созданный)
Node* hash_table_insert(HashTable* table, const char* key);

// поиск узла в таблице по ключу, возвращает NULL, если ключ не найден.
Node* hash_table_find(HashTable* table, const char* key);

// удаление узла из таблицы по ключу, возвращает 1 в случае успеха, 0 если не найден.
int hash_table_remove(HashTable* table, const char* key);

// перестроение (рехеширование) таблицы на новую емкость (для сохранения быстродействия)
void hash_table_resize(HashTable* table, size_t new_capacity);

// вспомогательная хеш-функция (алгоритм FNV-1a — простой и эффективный для строк)
unsigned int hash_function(const char* key, size_t capacity);

// функция для чтения текстового файла, извлечения слов и заполнения таблицы
int process_text_file(HashTable* table, const char* filename);

#endif // LIB_MAIN_H