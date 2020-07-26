
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include <limits.h>
#include <pthread.h>

typedef struct StackNode
{
    int  size ;
    int  data;
    struct stackNode* next;
};

struct StackNode *root = NULL;
int exist=0;

typedef struct thread_args
{
    int  data;
    struct stackNode* root;
};

int data;
pthread_t thread[10];
pthread_mutex_t lock;
struct thread_args *args;

struct StackNode* newNode(int data)
{
    struct StackNode* stackNode = (struct StackNode*) malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->next = NULL;

    return stackNode;
}

int isEmpty(struct StackNode *root)
{
    return !root;
}


void* isEmpty_t(void *arg)
{
    pthread_mutex_lock(&lock);
    if (isEmpty(arg))
        printf("the stack is empty \n");
    else
        printf(("the stack is not empty \n"));
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

void push(struct StackNode** root, int data)
{
    struct StackNode* stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;

    printf("%d pushed to stack\n", data);
}

void* push_t(void *arg)
{
    pthread_mutex_lock(&lock);
    if (exist==1)
        push(arg, data);
    else
        printf("You have to create the stack, use create()\n");
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int pop(struct StackNode** root)
{
    if (isEmpty(root))
    {
        printf("the stack is empty\n");
        return INT_MIN;
    }
    struct StackNode* temp = *root;
    *root = (*root)->next;
    int popped = temp->data;
    free(temp);

    return popped;
}

void* pop_t(void *arg)
{
    pthread_mutex_lock(&lock);
    if (exist==1)
        if (isEmpty(root))
            printf(("the stack is empty, you cannot pop an element \n"));
        else
            printf("the element %d has been poped out of the stack \n", pop(arg));


    else
        printf("You have to create the stack, use create()\n");
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}


int peek(struct StackNode* root)
{
    if (isEmpty(root))
    {
        printf("the stack is empty\n");
        return INT_MIN;
    }
    return root->data;
}

void* peek_t(void *arg)
{
    pthread_mutex_lock(&lock);
    if (exist==1)
        if (isEmpty(root))
            printf(("the stack is empty, there is no upper element \n"));
        else
            printf("the uppper element in the list is :%d \n", peek(arg));
    else
        printf("You have to create the stack, use create()\n");
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

void display(struct StackNode* root)
{
    if (isEmpty(root))
    {
        printf("the stack is empty\n");
        return INT_MIN;
    }

    int i=0;
    while (!isEmpty(root))
    {
        printf("data_top %d : %d \n",-i, root->data);
        root=root->next;
        ++i;
    }
}

void* display_t(void *arg)
{
    pthread_mutex_lock(&lock);
    if (exist==1){
        printf("the stack is as follow :\n");
        display(arg);
    }
    else
        printf("You have to create the stack, use create()\n");
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int stack_size(struct StackNode* root)
{
    int size=0;
    while (!isEmpty(root))
    {
        root=root->next;
        ++size;
    }
    return size;
}

void* stack_size_t(void *arg)
{
    pthread_mutex_lock(&lock);
    if (exist==1){
        printf("the size of the stack is %d\n", stack_size(arg));
    }
    else
        printf("You have to create the stack, use create()\n");
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main()
{

    int error;
    char command[100];

    do {

        char argument[100];

        if (pthread_mutex_init(&lock, NULL) != 0)
        {
            printf("\n mutex init has failed\n");
            return 1;
        }

        memset(command, 0, sizeof(command));

        printf("Please select the command that you want to do\n");
        scanf(" %[^\n]s", command);


        memset(argument, 0, sizeof(argument));

        int i = 0;
        while ((command[i] != '(') && (command[i] != '\0'))
        {
            ++i;
        }

        int j = 0;

        if (command[i]=='\0')
            command[0]='A';
        else
            command[i] = '\0';

        ++i;

        while ((command[i] != ')') && (command[i] != '\0'))
        {
            argument[j] = command[i];
            command[i] = ' ';
            ++j;
            ++i;
        }
        if (command[i]=='\0')
            command[0]='A';
        else
            command[i] = '\0';

        argument[j] = '\0';


        data = atoi(argument);

        if (strcmp(command, "peek") == 0) {

            error=pthread_create(&thread[0], NULL, &peek_t, root);
            if (error!=0)
                printf("\nThread can't be created :[%s]", strerror(error));
            pthread_join(thread[0], NULL);

        } else if (strcmp(command, "push") == 0) {

            error=pthread_create(&thread[1], NULL, &push_t, &root);
            if (error!=0)
                printf("\nThread can't be created :[%s]", strerror(error));
            pthread_join(thread[1], NULL);

        } else if (strcmp(command, "pop") == 0) {

            error=pthread_create(&thread[2], NULL, &pop_t, &root);
            if (error!=0)
                printf("\nThread can't be created :[%s]", strerror(error));
            pthread_join(thread[2], NULL);

        } else if (strcmp(command, "empty") == 0) {

            error=pthread_create(&thread[4], NULL, &isEmpty_t, root);
            if (error!=0)
                printf("\nThread can't be created :[%s]", strerror(error));
            pthread_join(thread[4], NULL);


        } else if (strcmp(command, "display") == 0) {

            error=pthread_create(&thread[3], NULL, &display_t, root);
            if (error!=0)
                printf("\nThread can't be created :[%s]", strerror(error));
            pthread_join(thread[3], NULL);

        } else if (strcmp(command, "create") == 0) {
            struct StackNode *root = NULL;
            exist=1;
            printf("Stack created\n");

        } else if (strcmp(command, "stack_size") == 0) {

            error=pthread_create(&thread[5], NULL, &stack_size_t, root);
            if (error!=0)
                printf("\nThread can't be created :[%s]", strerror(error));
            pthread_join(thread[5], NULL);

        } else if (strcmp(command, "quit") == 0) {

        }else{
            printf("Please select a valid command (push(), pop(), peek(), stack_size(), empty(), display(), and create()), don't forget the parentheses ()\n");
        }

    }while (strcmp(command, "quit")!=0);
    return 0;
}


