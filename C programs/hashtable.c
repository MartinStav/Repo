#include <stdio.h>
#include <stdlib.h>

int isFirst = 1;

typedef struct Node
{
    char data[50];
    double balance;
    struct Node *next;
} Node;

typedef struct HashTable
{
    Node **table;
    int size;
} HashTable;

HashTable *createHashTable(int size)
{
    HashTable *hashTable = (HashTable *)malloc(sizeof(HashTable));
    hashTable->size = size;
    hashTable->table = (Node **)calloc(size, sizeof(Node *));
    return hashTable;
}

int hash(char *key, int size)
{
    unsigned int hash = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        hash = 31 * hash + key[i];
    }
    return hash % size;
}

Node *search(HashTable *hashTable, char *data)
{
    int index = hash(data, hashTable->size);
    Node *current = hashTable->table[index];
    while (current != NULL)
    {
        for (int i = 0; i < 50; i++)
        {
            if (current->data[i] != data[i])
            {
                break;
            }
            if (current->data[i] == '\0')
            {
                return current;
            }
        }
        current = current->next;
    }
    return NULL;
}

void insert(HashTable *hashTable, char *data, double balance)
{
    Node *result = search(hashTable, data);
    if (result != NULL)
    {
        if (!isFirst)
        {
            printf("\n");
        }
        isFirst = 0;
        printf("insert failed");
        return;
    }
    int index = hash(data, hashTable->size);
    Node *newNode = (Node *)malloc(sizeof(Node));
    for (int i = 0; i < 50; i++)
    {
        newNode->data[i] = data[i];
    }
    newNode->balance = balance;
    newNode->next = hashTable->table[index];
    hashTable->table[index] = newNode;
}

void delete(HashTable *hashTable, char *data)
{
    int index = hash(data, hashTable->size);
    Node *current = hashTable->table[index];
    Node *prev = NULL;

    while (current != NULL)
    {
        for (int i = 0; i < 50; i++)
        {
            if (current->data[i] != data[i])
            {
                break;
            }
            if (current->data[i] == '\0')
            {
                if (prev == NULL)
                {
                    hashTable->table[index] = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                free(current);
                return;
            }
        }
        prev = current;
        current = current->next;
    }
    if (!isFirst)
    {
        printf("\n");
    }
    isFirst = 0;
    printf("delete failed");
}

double charToDouble(char *str)
{
    double result = 0, decimalResult = 0;
    int i = 0, negative = 1, decimal = 0, decimalPlaces = 0;

    if (str[0] == '-')
    {
        i++;
        negative = -1;
    }

    while (str[i] != '\0')
    {
        if (str[i] == ',')
        {
            decimal = 1;
            i++;
            continue;
        }

        if (decimal == 0)
        {
            result = result * 10 + (str[i] - '0');
        }
        else
        {
            decimalResult = decimalResult * 10 + (str[i] - '0');
            decimalPlaces++;
        }

        i++;
    }

    while (decimalPlaces > 0)
    {
        decimalResult /= 10;
        decimalPlaces--;
    }

    return (result + decimalResult) * negative;
}

void update(HashTable *hashTable, char *data, double newBalance)
{
    Node *node = search(hashTable, data);
    if (node != NULL)
    {
        if (node->balance + newBalance < 0)
        {
            if (!isFirst)
            {
                printf("\n");
            }
            isFirst = 0;
            printf("update failed");
            return;
        }
        node->balance += newBalance;
        return;
    }
    if (!isFirst)
    {
        printf("\n");
    }
    isFirst = 0;
    printf("update failed");
}

int main(void)
{
    HashTable *hashTable = createHashTable(10000);
    char command, data[50], balanceStr[20];
    double balance;
    int j = 0;

    while (scanf("%c ", &command) != EOF)
    {
        switch (command)
        {
        case 'i':
            for (int i = 0; i < 20; i++)
            {
                balanceStr[i] = '\0';
            }
            scanf("%[^\n]", data);
            for (int i = 0, j = 0, k = 0; data[j] != '\0'; j++)
            {
                if (data[j] == ' ')
                    i++;
                if (i >= 3)
                {
                    if (data[j] == ' ')
                    {
                        data[j] = '\0';
                        continue;
                    }
                    balanceStr[k] = data[j];
                    data[j] = '\0';
                    k++;
                }
            }
            balance = charToDouble(balanceStr);
            insert(hashTable, data, balance);
            break;

        case 's':
            scanf("%[^\n]", data);
            Node *result = search(hashTable, data);
            if (result != NULL)
            {
                if (!isFirst)
                {
                    printf("\n");
                }
                isFirst = 0;
                for (int i = 0; i < 20; i++)
                {
                    balanceStr[i] = '\0';
                }
                snprintf(balanceStr, 20, "%.2lf", result->balance);
                for (int i = 0; balanceStr[i] != '\0'; i++)
                {
                    if (balanceStr[i] == '.')
                    {
                        balanceStr[i] = ',';
                        break;
                    }
                }
                printf("%s", balanceStr);
                break;
            }
            if (!isFirst)
            {
                printf("\n");
            }
            isFirst = 0;
            printf("search failed");
            break;

        case 'd':
            scanf("%[^\n]", data);
            delete (hashTable, data);
            break;

        case 'u':
            for (int i = 0; i < 20; i++)
            {
                balanceStr[i] = '\0';
            }
            scanf("%[^\n]", data);
            for (int i = 0, j = 0, k = 0; data[j] != '\0'; j++)
            {
                if (data[j] == ' ')
                    i++;
                if (i >= 3)
                {
                    if (data[j] == ' ')
                    {
                        data[j] = '\0';
                        continue;
                    }
                    balanceStr[k] = data[j];
                    data[j] = '\0';
                    k++;
                }
            }
            balance = charToDouble(balanceStr);
            update(hashTable, data, balance);
            break;
        }
    }

    return 0;
}