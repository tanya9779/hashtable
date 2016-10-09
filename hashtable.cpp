#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <time.h>
using namespace std;
/* Реализация ОТКРЫТОЙ хэш таблицы на с++
    Структуры названы по образцу на "Википедии"
   Будут пары (ключ, значение) типа char[]
   Реализуем функции: добавить пару в таблицу
                      выдать значение по заданному ключу
                      удалить пару по заданому ключу
*/

// отпределим тип для вычисленного хэша
typedef unsigned char HASH_V; // можно сделать int и поправить const int TABLE_LEN=256
// отпределим тип для КЛЮЧА и ЗНАЧЕНИЯ - сейчас это строки
typedef char * KEY;
typedef char * VALUE;

typedef struct sEntry       //связный список пар типа (ключ, значение) являющийся элементом таблицы
{
        KEY k;     // по ключу будем вычислять хэш фунцию
        VALUE v;   // значение просто хранится
        sEntry *next; // Указатели на адрес следующей пары с тем же хэшем или NULL
} Entry;

typedef struct sBucket // это корзина, куда будем класть список (списка может и не быть)
{                      // пар с одинаковым вычисленным хэш-кодом
        Entry * head; // может быть NULL если пусто
} Bucket;

typedef struct sHashTable // это хэш таблица
{
    Bucket * buckets; // массив из корзин - индекс дает функция eval_hash()
} HashTable;

const int TABLE_LEN=256; // зададим размер массива - доступ по номеру
const int KEY_LEN = 20; // допустимая длина строки для ключа
const int VAL_LEN = 40; // и для значения

// вычисление хэша
HASH_V eval_hash(KEY k) // ПРИМИТИВНАЯ - просто остаток от деления на 10
{
    return ((HASH_V) atoi(k)%10);
}

// добавление пары (ключ, значение) в таблицу
void ht_add( HashTable *table, Entry * new_pair ) // важно передавать сюда Entry с выделенной памятью
{
        HASH_V hash = eval_hash(new_pair->k); // вычисляем hash
        Bucket * bucket = &(table->buckets[hash]); // и выбираем нужную корзину
        Entry * list_item = bucket->head; // каждая строка хэш таблицы - это список или NULL

        if ( ! list_item ) // не найдено - нужно просто занять это пустое место
        {
                bucket->head = new Entry;
                bucket->head->k = new_pair->k; // это присвоение адреса
                bucket->head->v = new_pair->v; // это тоже
                bucket->head->next = NULL;
        }
        else // если найден элемент с таким хэшем для этого ключа, то сравниваем ключи
            do {  // перебираем весь список с этим хэшем
                if ( strcmp(list_item->k, new_pair->k)==0 ) { // при совпадении строк
                    list_item->v = new_pair->v;         // мы обновим значение (через адрес)
                    break; // больше искать не нужно - выходим из do while
                }
                if ( !list_item->next ) { // последняя пара в списке пар - добавляем за ней новую пару
                    Entry * tmp = new Entry;
                    tmp->k = new_pair->k; // присвоение адреса (строка не копируется)
                    tmp->v = new_pair->v;  // здесь тоже
                    tmp->next = NULL; // у новой пары нет указателя на следующую пару
                    list_item->next = tmp; // привязываем новую пару в список
                    break; // больше искать не нужно - выходим из do while
                }
                list_item = list_item->next; // переход к следующей паре с таким же хэшем
            } while (list_item); // конец перебора связного списка
}

VALUE ht_get( HashTable *table, KEY k_find) // получение значения по ключу
{
        HASH_V hash = eval_hash(k_find); // вычисляем hash
        Bucket * bucket = &(table->buckets[hash]); // и выбираем нужную корзину
        Entry * list_item = bucket->head; // каждая строка хэш таблицы - это список или NULL

        if ( ! list_item ) // ничего не найдено
        	return NULL;
        // нужна дополнительная проверка на совпадение
	    do {  // обойдем весь список с таким хэшем
		    if ( strcmp(list_item->k, k_find)==0 )
			    return list_item->v;

        list_item = list_item->next; // переход к следующему с таким же хэшем

        } while (list_item);
        return NULL; // хэш совпал, а ключ нет!!!
}


int ht_delete( HashTable *table, KEY k_del) // удаляет пару с заданным ключем
{
    HASH_V hash = eval_hash(k_del); // вычисляем hash
    Bucket * bucket = &(table->buckets[hash]); // и выбираем нужную корзину
    Entry * list_item = bucket->head; // это элемент удалим, если он найдется
	Entry * prev = NULL; // предыдущая пара в списке (мы должны "сшить" список после удаления звена)

    if ( ! list_item) // ничего не найдено
      	return 1; // результат можно использовать для анализа ошибок

    // нужна дополнительная проверка на совпадение
	do {  // обойдем весь список с таким хэшем
		if ( strcmp(list_item->k, k_del)==0 )  // нашли такой ключ
        {
		    if (prev)  // это удаление из середины или с конца
		            prev->next = list_item->next; // выкалываем элемент и "сшиваем" список
		    else  // мы должны удалить head
		            bucket->head = list_item->next; // теперь указывает на NULL
            delete list_item; // уборка мусора
			return 0; // все хорошо
		}
		prev = list_item; // запомним предыдущего
        list_item = list_item->next; // переход к следующему с таким же хэшем

        } while (list_item);
        return 2; // хэш совпал, а ключ нет !!! попросили удалить тот элемент, которого нет в таблице
}

// вывод таблицы на экран
void print( HashTable * table )
{
    std::cout << "Hashtable:" << "\n";
    for(int i=0; i<TABLE_LEN;i++)
    {
        Bucket * bucket = &(table->buckets[i]); // берем адрес очередной корзины
        Entry * entr = bucket->head;  // берем адрес первой пары в корзине (может быть NULL)
        if ( !bucket->head) // только непустые корзины выводим
             continue; // переход к началу цикла for
        std::cout << "hashcode: " << i <<", hashset: (";
        while( entr )  {    // Пока не встретим пустое значение
                std::cout << " (" << string(entr->k) <<", " << string(entr->v) << ") "; //Выводим значение на экран
                entr = entr->next;     //Смена адреса на адрес следующего элемента
        }
        std::cout<<")\n"; // закрываем скобку и перенос строки
    }
}

int main()
{
    srand(time(NULL)); //перезапускаем генератор случайных чисел
    HashTable * table = new HashTable;
    table->buckets = new Bucket [TABLE_LEN]; // выделим память под корзины

    Bucket * tmp_ptr = table->buckets; // берем адрес первой корзины
    for(int i=0; i<TABLE_LEN;i++) // обходим все корзины
           (tmp_ptr++)->head = NULL; // пока в таблице пусто

    // наполним таблицу
    std::cout << "============= TEST RANDOM INSERT ===============\n";
    for (char j=0; j<100;j++) {  // 100 элементов добавим
        Entry * entr = new Entry; // делаем выделение памяти каждый раз !!! -
                       // эта структура не копируется, а используется ее адрес
        entr->k = new char[KEY_LEN]; // ОЧЕНЬ важно выделить достаточно памяти
        entr->v = new char[VAL_LEN]; // и НЕ превышать никогда эти размеры
        entr->next = NULL;

        sprintf(entr->k,"%d",rand()%100); // так преобразуем random число в строку и копируем по адресу entr->k
        sprintf(entr->v,"value %d",j+1); // значение формируем и копируем по адресу entr->v
                             // !!! не превышать длину VAL_LEN !!!
        ht_add(table, entr); // добавим в таблицу эту новую пару
    }
    print(table);  // выведем на экран
    std::cout << "============= TEST APPEND 15 and 55 ===============\n";
    Entry * entr = new Entry;
    entr->k = new char[KEY_LEN]; // ОЧЕНЬ важно выделить память
    entr->v = new char[VAL_LEN]; // и НЕ превышать эти размеры строк
    entr->next = NULL;
    sprintf(entr->k,"%d",15); // так преобразуем число в строку и отправляем по адрему entr->k
    sprintf(entr->v,"INSERTED %d",15); // и по адресу entr->v
    ht_add(table, entr); // добавим пробное значение 15
    Entry * entr2 = new Entry; // нельзя повторно обращаться к entr - так заместим старые значения новыми
    entr2->k = new char[KEY_LEN];
    entr2->v = new char[VAL_LEN];
    entr2->next = NULL;
    sprintf(entr2->k,"%d",55);
    sprintf(entr2->v,"INSERTED %d",55);
    ht_add(table, entr2); // добавим значение 55
    print(table); // выведем измененный массив на экран

    // проверка функции ht_get
    std::cout << "============= TEST GET 15===============\n";
    std::cout << "value for 15 is \"" << ht_get(table, "15") << "\"\n";
    std::cout << "============= TEST GET 9===============\n";
    std::cout << "value for 9 is \"" << ht_get(table, "9") << "\"\n";
    std::cout << "============= TEST GET 55===============\n";
    std::cout << "value for 55 is \"" << ht_get(table, "55") << "\"\n";
    std::cout << "============= TEST DELETE 60 ===============\n";
// удалим элемент 60
    ht_delete(table, "60");
    // выведем измененный массив на экран - не должно быть (60, value 60)
    print(table);  // выведем на экран после удаления
    // проверка функции ht_get после удаления
    std::cout << "============= TEST GET 15 ANOTHER ===============\n";
    std::cout << "value for 15 is \"" << ht_get(table, "15") << "\"\n";
    std::cout << "============= TEST GET 60 ===============\n";
    if ( ht_get(table, "60") )
         std::cout << "value for 60 is \"" << ht_get(table, "60") << "\"\n";
    else
         std::cout << "value for 60 not found\n";

  // только для отладки, чтобы окно запущенной программы не закрывалось
  std::cout << "enter any integer number...";
  int z;
  std::cin >> z;
}


