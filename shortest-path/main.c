#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int isFirst = 1;

struct Node
{
    int vertex;
    int weight;
    struct Node *next;
};

struct Graph
{
    int numberOfVertices;
    struct Node **adjList;
};

struct Graph *
createGraph(int numberOfVertices)
{
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    graph->numberOfVertices = numberOfVertices;
    graph->adjList = (struct Node **)calloc(numberOfVertices, sizeof(struct Node *));
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
        printf("%s failed", failCommand);
        return;
    }
    printf("%s %d %d failed", failCommand, vertex_1, vertex_2);
}

void insert(struct Graph *graph, int vertex_1, int vertex_2, int weight)
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

void update(struct Graph *graph, int vertex_1, int vertex_2, int weight)
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

    prev = NULL;
    curr = graph->adjList[vertex_2];
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

void search(struct Graph *graph, int vertex_1, int vertex_2)
{
    int *dist = (int *)malloc(graph->numberOfVertices * sizeof(int));
    int *prev = (int *)malloc(graph->numberOfVertices * sizeof(int));
    int *visited = (int *)calloc(graph->numberOfVertices, sizeof(int));

    for (int i = 0; i < graph->numberOfVertices; i++)
    {
        dist[i] = INT_MAX;
        prev[i] = -1;
    }
    dist[vertex_1] = 0;

    while (1)
    {
        int min = INT_MAX;
        int minIndex = -1;
        for (int j = 0; j < graph->numberOfVertices; j++)
        {
            if (!visited[j] && dist[j] < min)
            {
                min = dist[j];
                minIndex = j;
            }
        }
        if (minIndex == -1 || minIndex == vertex_2)
            break;
        visited[minIndex] = 1;

        struct Node *temp = graph->adjList[minIndex];
        while (temp)
        {
            if (!visited[temp->vertex] && dist[minIndex] + temp->weight < dist[temp->vertex])
            {
                dist[temp->vertex] = dist[minIndex] + temp->weight;
                prev[temp->vertex] = minIndex;
            }
            temp = temp->next;
        }
    }

    if (dist[vertex_2] == INT_MAX)
    {
        failPrint("shortest", vertex_1, vertex_2);
        free(dist);
        free(prev);
        free(visited);
        return;
    }

    int *shortestPath = (int *)malloc(graph->numberOfVertices * sizeof(int));
    int pathLength = 0;
    int temp = vertex_2;
    while (temp != -1)
    {
        shortestPath[pathLength++] = temp;
        temp = prev[temp];
    }

    if (!isFirst)
    {
        printf("\n");
    }
    isFirst = 0;
    printf("%d: [", dist[vertex_2]);
    for (int i = pathLength - 1; i >= 0; i--)
    {
        printf("%d", shortestPath[i]);
        if (i > 0)
        {
            printf(", ");
        }
    }
    printf("]");

    free(shortestPath);
    free(dist);
    free(prev);
    free(visited);
}

int main(void)
{
    char command;
    int vertex_1, vertex_2, weight, numberOfVertices, numberOfEdges;

    scanf("%d %d", &numberOfVertices, &numberOfEdges);
    struct Graph *graph = createGraph(numberOfVertices);

    for (; numberOfEdges > 0; numberOfEdges--)
    {
        scanf(" (%d, %d, %d)", &vertex_1, &vertex_2, &weight);
        insert(graph, vertex_1, vertex_2, weight);
    }

    while (scanf(" %c", &command) != EOF)
    {
        switch (command)
        {
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
            scanf(" %d %d", &vertex_1, &vertex_2);
            if (vertex_1 < 0 || vertex_1 >= graph->numberOfVertices ||
                vertex_2 < 0 || vertex_2 >= graph->numberOfVertices)
            {
                failPrint("shortest", -1, -1);
                break;
            }
            search(graph, vertex_1, vertex_2);
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
