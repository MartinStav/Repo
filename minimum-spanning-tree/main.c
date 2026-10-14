#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int isFirst = 1;

struct Node
{
    int vertex;
    long long weight;
    struct Node *next;
};

struct Graph
{
    int numberOfVertices;
    struct Node **adjList;
};

struct Edge
{
    int parent;
    long long vertex;
};

int compareEdges(const void *a, const void *b)
{
    struct Edge *edgeA = (struct Edge *)a;
    struct Edge *edgeB = (struct Edge *)b;

    if (edgeA->parent != edgeB->parent)
        return edgeA->parent - edgeB->parent;
    else
        return edgeA->vertex - edgeB->vertex;
}

struct Graph *
createGraph(int numberOfVertices)
{
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    graph->numberOfVertices = numberOfVertices;
    graph->adjList = (struct Node **)calloc(numberOfVertices, sizeof(struct Node *));
    for (int i = 0; i < numberOfVertices; i++)
    {
        graph->adjList[i] = NULL;
    }
    return graph;
}

void failPrint(char *failCommand, int vertex_1, int vertex_2)
{
    if (!isFirst)
    {
        printf("\n");
    }
    isFirst = 0;
    if (vertex_1 == -1 || vertex_2 == -1)
    {
        printf("%s %d failed", failCommand, vertex_1);
        return;
    }
    printf("%s %d %d failed", failCommand, vertex_1, vertex_2);
}

void insert(struct Graph *graph, int vertex_1, int vertex_2, long long weight)
{
    struct Node *temp = graph->adjList[vertex_1];
    while (temp)
    {
        if (temp->vertex == vertex_2 || vertex_1 == vertex_2)
        {
            failPrint("insert", vertex_1, vertex_2);
            return;
        }
        temp = temp->next;
    }

    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->vertex = vertex_2;
    newNode->weight = weight;
    newNode->next = graph->adjList[vertex_1];
    graph->adjList[vertex_1] = newNode;

    newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->vertex = vertex_1;
    newNode->weight = weight;
    newNode->next = graph->adjList[vertex_2];
    graph->adjList[vertex_2] = newNode;
}

void update(struct Graph *graph, int vertex_1, int vertex_2, long long weight)
{
    struct Node *temp = graph->adjList[vertex_1];
    while (temp && temp->vertex != vertex_2)
        temp = temp->next;
    if (!temp || temp->weight + weight < 0)
    {
        failPrint("update", vertex_1, vertex_2);
        return;
    }
    temp->weight += weight;

    temp = graph->adjList[vertex_2];
    while (temp && temp->vertex != vertex_1)
        temp = temp->next;
    temp->weight += weight;
}

void delete(struct Graph *graph, int vertex_1, int vertex_2)
{
    struct Node *prev = NULL;
    struct Node *curr = graph->adjList[vertex_1];
    while (curr && curr->vertex != vertex_2)
    {
        prev = curr;
        curr = curr->next;
    }
    if (!curr)
    {
        failPrint("delete", vertex_1, vertex_2);
        return;
    }
    if (prev)
        prev->next = curr->next;
    else
        graph->adjList[vertex_1] = curr->next;
    free(curr);
    curr = graph->adjList[vertex_2];

    prev = NULL;
    while (curr && curr->vertex != vertex_1)
    {
        prev = curr;
        curr = curr->next;
    }
    if (prev)
        prev->next = curr->next;
    else
        graph->adjList[vertex_2] = curr->next;
    free(curr);
}

void search(struct Graph *graph, int vertex)
{
    if (vertex < 0 || vertex >= graph->numberOfVertices) {
        failPrint("search", vertex, -1);
        return;
    }
    int *visited = (int *)malloc(graph->numberOfVertices * sizeof(int));
    int *parent = (int *)malloc(graph->numberOfVertices * sizeof(int));
    long long *key = (long long *)malloc(graph->numberOfVertices * sizeof(long long));

    if (visited == NULL || parent == NULL || key == NULL)
    {
        printf("Memory  failed\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < graph->numberOfVertices; ++i)
    {
        visited[i] = 0;
        parent[i] = -1;
        key[i] = LLONG_MAX;
    }

    key[vertex] = 0;
    parent[vertex] = -1;

    for (int count = 0; count < graph->numberOfVertices - 1; count++)
    {
        long long minKey = LLONG_MAX;
        int minIndex = -1;

        for (int v = 0; v < graph->numberOfVertices; v++)
        {
            if (!visited[v] && key[v] < minKey)
            {
                minKey = key[v];
                minIndex = v;
            }
        }

        if (minIndex == -1)
            break;

        visited[minIndex] = 1;

        struct Node *temp = graph->adjList[minIndex];
        while (temp)
        {
            int adjVertex = temp->vertex;
            long long weight = temp->weight;
            if (!visited[adjVertex] && weight < key[adjVertex])
            {
                parent[adjVertex] = minIndex;
                key[adjVertex] = weight;
            }
            temp = temp->next;
        }
    }

    struct Edge *edges = (struct Edge *)malloc(graph->numberOfVertices * sizeof(struct Edge));
    if (edges == NULL)
    {
        printf("Memory  failed\n");
        exit(EXIT_FAILURE);
    }
    int edgeCount = 0;
    long long weight = 0;
    for (int i = 0; i < graph->numberOfVertices; ++i)
    {
        if (parent[i] != -1 && key[i] != LLONG_MAX)
        {
            weight += key[i];
            if (i < parent[i])
            {
                edges[edgeCount].parent = i;
                edges[edgeCount].vertex = parent[i];
            }
            else
            {
                edges[edgeCount].parent = parent[i];
                edges[edgeCount].vertex = i;
            }
            edgeCount++;
        }
    }
    if (edgeCount <= 0)
    {
        failPrint("search", vertex, -1);
        return;
    }
    if (!isFirst)
    {
        printf("\n");
    }
    isFirst = 0;
    qsort(edges, edgeCount, sizeof(struct Edge), compareEdges);

    printf("%lld: [", weight);
    for (int i = 0; i < edgeCount; ++i)
    {
        printf("(%d, %lld)", edges[i].parent, edges[i].vertex);
        if (i < edgeCount - 1)
            printf(", ");
    }
    printf("]");

    free(edges);
    free(visited);
    free(parent);
    free(key);
}

int main(void)
{
    char command, check;
    int vertex_1, vertex_2, weight, numberOfVertices, numberOfEdges;

    scanf("%d", &numberOfVertices);
    struct Graph *graph = createGraph(numberOfVertices);

    while (scanf(" %c", &command) != EOF)
    {
        switch (command)
        {
        case '(':
            scanf("%d, %d, %d)", &vertex_1, &vertex_2, &weight);
            insert(graph, vertex_1, vertex_2, weight);
            break;
        case 'i':
            scanf(" %d %d %d", &vertex_1, &vertex_2, &weight);
            if (vertex_1 >= 0 && vertex_1 < graph->numberOfVertices &&
                vertex_2 >= 0 && vertex_2 < graph->numberOfVertices)
            {
                insert(graph, vertex_1, vertex_2, weight);
            }
            else
            {
                failPrint("insert", vertex_1, vertex_2);
            }
            break;

        case 's':
            scanf(" %d", &vertex_1);
            search(graph, vertex_1);
            break;

        case 'd':
            scanf(" %d %d", &vertex_1, &vertex_2);
            delete (graph, vertex_1, vertex_2);
            break;

        case 'u':
            scanf(" %d %d %d", &vertex_1, &vertex_2, &weight);
            update(graph, vertex_1, vertex_2, weight);
            break;
        }
    }
    return 0;
}
