#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <time.h>
using namespace std;
/* ���������� �������� ��� ������� �� �++
    ��������� ������� �� ������� �� "���������"
   ����� ���� (����, ��������) ���� char[]
   ��������� �������: �������� ���� � �������
                      ������ �������� �� ��������� �����
                      ������� ���� �� �������� �����
*/

// ���������� ��� ��� ������������ ����
typedef unsigned char HASH_V; // ����� ������� int � ��������� const int TABLE_LEN=256
// ���������� ��� ��� ����� � �������� - ������ ��� ������
typedef char * KEY;
typedef char * VALUE;

typedef struct sEntry       //������� ������ ��� ���� (����, ��������) ���������� ��������� �������
{
        KEY k;     // �� ����� ����� ��������� ��� ������
        VALUE v;   // �������� ������ ��������
        sEntry *next; // ��������� �� ����� ��������� ���� � ��� �� ����� ��� NULL
} Entry;

typedef struct sBucket // ��� �������, ���� ����� ������ ������ (������ ����� � �� ����)
{                      // ��� � ���������� ����������� ���-�����
        Entry * head; // ����� ���� NULL ���� �����
} Bucket;

typedef struct sHashTable // ��� ��� �������
{
    Bucket * buckets; // ������ �� ������ - ������ ���� ������� eval_hash()
} HashTable;

const int TABLE_LEN=256; // ������� ������ ������� - ������ �� ������
const int KEY_LEN = 20; // ���������� ����� ������ ��� �����
const int VAL_LEN = 40; // � ��� ��������

// ���������� ����
HASH_V eval_hash(KEY k) // ����������� - ������ ������� �� ������� �� 10
{
    return ((HASH_V) atoi(k)%10);
}

// ���������� ���� (����, ��������) � �������
void ht_add( HashTable *table, Entry * new_pair ) // ����� ���������� ���� Entry � ���������� �������
{
        HASH_V hash = eval_hash(new_pair->k); // ��������� hash
        Bucket * bucket = &(table->buckets[hash]); // � �������� ������ �������
        Entry * list_item = bucket->head; // ������ ������ ��� ������� - ��� ������ ��� NULL

        if ( ! list_item ) // �� ������� - ����� ������ ������ ��� ������ �����
        {
                bucket->head = new Entry;
                bucket->head->k = new_pair->k; // ��� ���������� ������
                bucket->head->v = new_pair->v; // ��� ����
                bucket->head->next = NULL;
        }
        else // ���� ������ ������� � ����� ����� ��� ����� �����, �� ���������� �����
            do {  // ���������� ���� ������ � ���� �����
                if ( strcmp(list_item->k, new_pair->k)==0 ) { // ��� ���������� �����
                    list_item->v = new_pair->v;         // �� ������� �������� (����� �����)
                    break; // ������ ������ �� ����� - ������� �� do while
                }
                if ( !list_item->next ) { // ��������� ���� � ������ ��� - ��������� �� ��� ����� ����
                    Entry * tmp = new Entry;
                    tmp->k = new_pair->k; // ���������� ������ (������ �� ����������)
                    tmp->v = new_pair->v;  // ����� ����
                    tmp->next = NULL; // � ����� ���� ��� ��������� �� ��������� ����
                    list_item->next = tmp; // ����������� ����� ���� � ������
                    break; // ������ ������ �� ����� - ������� �� do while
                }
                list_item = list_item->next; // ������� � ��������� ���� � ����� �� �����
            } while (list_item); // ����� �������� �������� ������
}

VALUE ht_get( HashTable *table, KEY k_find) // ��������� �������� �� �����
{
        HASH_V hash = eval_hash(k_find); // ��������� hash
        Bucket * bucket = &(table->buckets[hash]); // � �������� ������ �������
        Entry * list_item = bucket->head; // ������ ������ ��� ������� - ��� ������ ��� NULL

        if ( ! list_item ) // ������ �� �������
        	return NULL;
        // ����� �������������� �������� �� ����������
	    do {  // ������� ���� ������ � ����� �����
		    if ( strcmp(list_item->k, k_find)==0 )
			    return list_item->v;

        list_item = list_item->next; // ������� � ���������� � ����� �� �����

        } while (list_item);
        return NULL; // ��� ������, � ���� ���!!!
}


int ht_delete( HashTable *table, KEY k_del) // ������� ���� � �������� ������
{
    HASH_V hash = eval_hash(k_del); // ��������� hash
    Bucket * bucket = &(table->buckets[hash]); // � �������� ������ �������
    Entry * list_item = bucket->head; // ��� ������� ������, ���� �� ��������
	Entry * prev = NULL; // ���������� ���� � ������ (�� ������ "�����" ������ ����� �������� �����)

    if ( ! list_item) // ������ �� �������
      	return 1; // ��������� ����� ������������ ��� ������� ������

    // ����� �������������� �������� �� ����������
	do {  // ������� ���� ������ � ����� �����
		if ( strcmp(list_item->k, k_del)==0 )  // ����� ����� ����
        {
		    if (prev)  // ��� �������� �� �������� ��� � �����
		            prev->next = list_item->next; // ���������� ������� � "�������" ������
		    else  // �� ������ ������� head
		            bucket->head = list_item->next; // ������ ��������� �� NULL
            delete list_item; // ������ ������
			return 0; // ��� ������
		}
		prev = list_item; // �������� �����������
        list_item = list_item->next; // ������� � ���������� � ����� �� �����

        } while (list_item);
        return 2; // ��� ������, � ���� ��� !!! ��������� ������� ��� �������, �������� ��� � �������
}

// ����� ������� �� �����
void print( HashTable * table )
{
    std::cout << "Hashtable:" << "\n";
    for(int i=0; i<TABLE_LEN;i++)
    {
        Bucket * bucket = &(table->buckets[i]); // ����� ����� ��������� �������
        Entry * entr = bucket->head;  // ����� ����� ������ ���� � ������� (����� ���� NULL)
        if ( !bucket->head) // ������ �������� ������� �������
             continue; // ������� � ������ ����� for
        std::cout << "hashcode: " << i <<", hashset: (";
        while( entr )  {    // ���� �� �������� ������ ��������
                std::cout << " (" << string(entr->k) <<", " << string(entr->v) << ") "; //������� �������� �� �����
                entr = entr->next;     //����� ������ �� ����� ���������� ��������
        }
        std::cout<<")\n"; // ��������� ������ � ������� ������
    }
}

int main()
{
    srand(time(NULL)); //������������� ��������� ��������� �����
    HashTable * table = new HashTable;
    table->buckets = new Bucket [TABLE_LEN]; // ������� ������ ��� �������

    Bucket * tmp_ptr = table->buckets; // ����� ����� ������ �������
    for(int i=0; i<TABLE_LEN;i++) // ������� ��� �������
           (tmp_ptr++)->head = NULL; // ���� � ������� �����

    // �������� �������
    std::cout << "============= TEST RANDOM INSERT ===============\n";
    for (char j=0; j<100;j++) {  // 100 ��������� �������
        Entry * entr = new Entry; // ������ ��������� ������ ������ ��� !!! -
                       // ��� ��������� �� ����������, � ������������ �� �����
        entr->k = new char[KEY_LEN]; // ����� ����� �������� ���������� ������
        entr->v = new char[VAL_LEN]; // � �� ��������� ������� ��� �������
        entr->next = NULL;

        sprintf(entr->k,"%d",rand()%100); // ��� ����������� random ����� � ������ � �������� �� ������ entr->k
        sprintf(entr->v,"value %d",j+1); // �������� ��������� � �������� �� ������ entr->v
                             // !!! �� ��������� ����� VAL_LEN !!!
        ht_add(table, entr); // ������� � ������� ��� ����� ����
    }
    print(table);  // ������� �� �����
    std::cout << "============= TEST APPEND 15 and 55 ===============\n";
    Entry * entr = new Entry;
    entr->k = new char[KEY_LEN]; // ����� ����� �������� ������
    entr->v = new char[VAL_LEN]; // � �� ��������� ��� ������� �����
    entr->next = NULL;
    sprintf(entr->k,"%d",15); // ��� ����������� ����� � ������ � ���������� �� ������ entr->k
    sprintf(entr->v,"INSERTED %d",15); // � �� ������ entr->v
    ht_add(table, entr); // ������� ������� �������� 15
    Entry * entr2 = new Entry; // ������ �������� ���������� � entr - ��� �������� ������ �������� ������
    entr2->k = new char[KEY_LEN];
    entr2->v = new char[VAL_LEN];
    entr2->next = NULL;
    sprintf(entr2->k,"%d",55);
    sprintf(entr2->v,"INSERTED %d",55);
    ht_add(table, entr2); // ������� �������� 55
    print(table); // ������� ���������� ������ �� �����

    // �������� ������� ht_get
    std::cout << "============= TEST GET 15===============\n";
    std::cout << "value for 15 is \"" << ht_get(table, "15") << "\"\n";
    std::cout << "============= TEST GET 9===============\n";
    std::cout << "value for 9 is \"" << ht_get(table, "9") << "\"\n";
    std::cout << "============= TEST GET 55===============\n";
    std::cout << "value for 55 is \"" << ht_get(table, "55") << "\"\n";
    std::cout << "============= TEST DELETE 60 ===============\n";
// ������ ������� 60
    ht_delete(table, "60");
    // ������� ���������� ������ �� ����� - �� ������ ���� (60, value 60)
    print(table);  // ������� �� ����� ����� ��������
    // �������� ������� ht_get ����� ��������
    std::cout << "============= TEST GET 15 ANOTHER ===============\n";
    std::cout << "value for 15 is \"" << ht_get(table, "15") << "\"\n";
    std::cout << "============= TEST GET 60 ===============\n";
    if ( ht_get(table, "60") )
         std::cout << "value for 60 is \"" << ht_get(table, "60") << "\"\n";
    else
         std::cout << "value for 60 not found\n";

  // ������ ��� �������, ����� ���� ���������� ��������� �� �����������
  std::cout << "enter any integer number...";
  int z;
  std::cin >> z;
}


