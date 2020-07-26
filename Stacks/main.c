// C program to demonstrate use of fork() and pipe() 
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include <limits.h>
#include <string.h>


typedef struct StackNode
{
    int  size ;
    int  data;
    struct stackNode* next;
};

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

void push(struct StackNode** root, int data)
{
    struct StackNode* stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;

    printf("%d pushed to stack\n", data);
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

int peek(struct StackNode* root)
{
    if (isEmpty(root))
    {
        printf("the stack is empty\n");
        return INT_MIN;
    }
    return root->data;
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
struct StackNode *root = NULL;
int exist=0;

int main()
{
    char quit[10];
    pid_t p;

    // We use two pipes
    // First pipe to send the commmand from parent to child
    // Second pipe to send the command back to parent from child

    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 


    if (pipe(fd1)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
    if (pipe(fd2)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }

    p = fork();

    do {
    if (p < 0)
    {
        fprintf(stderr, "fork Failed" );
        return 1;
    }
    // Parent process
    else if (p > 0)
    {
        char command[10];
        memset(command, 0, sizeof(command));

        printf("Please select the command that you want to do\n");
        scanf(" %[^\n]s", command);

        close(fd1[0]);  // Close reading end of first pipe

        // Write command and close writing end of first pipe.
        write(fd1[1], command, strlen(command) + 1);
        kill(getpid(), SIGSTOP);

        close(fd2[1]); // Close writing end of second pipe

        // Read command from child, print it and close
        read(fd2[0], quit, 100);
    }

        // child process
    else {
        char command[100];
        char argument[100];

        close(fd1[1]);  // Close writing end of first pipe

        int data;
        memset(argument, 0, sizeof(argument));

        // Read the command using first pipe
        read(fd1[0], command, 100);

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
            if (exist==1)
                if (isEmpty(root))
                    printf(("the stack is empty, there is no upper element\n"));
                else
                    printf("the uppper element in the list is :%d \n", peek(root));
            else
                printf("You have to create the stack, use create()\n");

        } else if (strcmp(command, "push") == 0) {
            if (exist==1)
                push(&root, data);
            else
                printf("You have to create the stack, use create()\n");
        } else if (strcmp(command, "pop") == 0) {
            if (exist==1)
                if (isEmpty(root))
                    printf(("the stack is empty, you cannot pop an element \n"));
                else
                    printf("the element %d has been poped out of the stack \n", pop(&root));
            else
                printf("You have to create the stack, use create()\n");

        } else if (strcmp(command, "empty") == 0) {
            if (isEmpty(root))
                printf("the stack is empty \n");
            else
                printf(("the stack is not empty \n"));

        } else if (strcmp(command, "display") == 0) {
            if (exist==1){
                printf("the stack is as follow :\n");
                display(root);
            }
            else
                printf("You have to create the stack, use create()\n");


        } else if (strcmp(command, "create") == 0) {
            struct StackNode *root = NULL;
            exist=1;
            printf("Stack created\n");

        } else if (strcmp(command, "stack_size") == 0) {
            if (exist==1){
                printf("the size of the stack is %d\n", stack_size(root));
            }
            else
                printf("You have to create the stack, use create()\n");

        } else if (strcmp(command, "quit") == 0) {
            // Close reading ends
            close(fd2[0]);

            // Write the command back to the parent and close writing end
            write(fd2[1], command, strlen(command)+1);
            kill(getppid(), SIGCONT);
            exit(EXIT_SUCCESS);
        } else{
            printf("Please select a valid command (push(), pop(), peek(), stack_size(), empty(), display(), and create()), don't forget the parentheses ()\n");
        }

        // Close reading ends
        close(fd2[0]);

        // Write the command back to the parent and close writing end
        write(fd2[1], command, strlen(command)+1);

        kill(getppid(), SIGCONT);

    }
    }while (strcmp(quit, "quit")!=0);
    return 0;
}


