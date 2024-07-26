#include <stdio.h>
#include <stdlib.h>

typedef struct Person
{
    int id;
    char data[38];
} Person;

typedef struct Node
{
    Person data;
    struct Node *left;
    struct Node *right;
    int height;
} Node;

Node *newNode(Person data)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

int height(Node *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

int max(int left, int right)
{
    return (left > right) ? left : right;
}

int getBalance(Node *node)
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

Node *rightRotate(Node *y)
{
    Node *x = y->left;
    Node *temp = x->right;

    x->right = y;
    y->left = temp;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

Node *leftRotate(Node *x)
{
    Node *y = x->right;
    Node *temp = y->left;

    y->left = x;
    x->right = temp;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

Node *insert(Node *root, Person data)
{
    if (root == NULL)
        return newNode(data);

    if (data.id < root->data.id)
        root->left = insert(root->left, data);
    else if (data.id > root->data.id)
        root->right = insert(root->right, data);
    else
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && data.id < root->left->data.id)
        return rightRotate(root);

    if (balance < -1 && data.id > root->right->data.id)
        return leftRotate(root);

    if (balance > 1 && data.id > root->left->data.id)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && data.id < root->right->data.id)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

Node *search(Node *root, int id)
{
    if (root == NULL || root->data.id == id)
        return root;

    if (root->data.id > id)
        return search(root->left, id);

    return search(root->right, id);
}

Node *delete(Node *root, int id)
{
    if (root == NULL)
        return root;

    if (id < root->data.id)
        root->left = delete (root->left, id);
    else if (id > root->data.id)
        root->right = delete (root->right, id);

    else
    {
        if (root->left == NULL)
        {
            Node *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            Node *temp = root->left;
            free(root);
            return temp;
        }
        else
        {
            Node *temp = root->right;
            while (temp->left != NULL)
            {
                temp = temp->left;
            }
            root->data = temp->data;
            root->right = delete (root->right, temp->data.id);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

int main(void)
{
    Node *root = NULL;
    char command, data[38];
    int id, toId, isFirst = 1;

    while (scanf("%c", &command) != EOF)
    {
        switch (command)
        {
        case 'i':
            scanf("%d", &id);
            scanf(" %[^\n]", data);
            Person NewPerson;
            NewPerson.id = id;
            for (int i = 0; i < 38; i++)
            {
                NewPerson.data[i] = data[i];
            }
            root = insert(root, NewPerson);
            break;
        case 's':
            scanf("%d", &id);
            if (scanf("%d", &toId) == 1)
            {
                if (id > toId)
                {
                    int temp = id;
                    id = toId;
                    toId = temp;
                }
                for (int i = id; i <= toId; i++)
                {
                    Node *result = search(root, i);
                    if (result != NULL)
                    {
                        if (isFirst == 0)
                        {
                            printf("\n");
                        }
                        isFirst = 0;
                        printf("%d %s", result->data.id, result->data.data);
                    }
                }
            }
            else
            {
                Node *result = search(root, id);
                if (result != NULL)
                {
                    if (isFirst == 0)
                    {
                        printf("\n");
                    }
                    isFirst = 0;
                    printf("%d %s", result->data.id, result->data.data);
                }
            }
            break;
        case 'd':
            scanf("%d", &id);
            root = delete (root, id);
            break;
        }
    }

    return 0;
}
