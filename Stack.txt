#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    char path[20];
    char file[20];

    struct node *parent;
    struct node *sibling[5];
} NODE;

//function to return the actual directory in the pathname
char * getDIR(char *pathname, int end)
{
    char *DIR = (char *) malloc(10*sizeof(char));
    DIR[0]='/';
    int k=0;
    int i=0;
    do
    {
        i=0;
        if (*pathname=='/')
        {
            ++pathname;
        }
        while ((*pathname!='/')&&(*pathname!='\0'))
        {
            DIR[i]=*pathname;
            ++i;
            ++pathname;
            k=1;
        }
        if (k==0)
            ++i;
    }while ((*pathname!='\0')&&(end));
    DIR[i]='\0';

    return DIR;
}

// create a new node
NODE *new_node(char *pathname, NODE* parent)
{
    NODE *node = (NODE *) malloc(sizeof(NODE));

    int i=0;
    char *DIR= getDIR(pathname, 0);

    while(*DIR!='\0')
    {
        node->path[i] = *DIR;
        ++DIR;
        ++i;
    }
    node->path[i]='\0';

    node->parent= parent;
    for (int i=0; i<5; ++i)
    {
        node->sibling[i]=NULL;
    }
    return node;
}

// create a new directory
NODE *mkdir(NODE *node, char *pathname, NODE* parent)
{
    if (node == NULL)
    {
        if (*pathname!='\0')
        {
            node = new_node(pathname, parent);
            char *DIR= getDIR(pathname, 0);

            if (*pathname=='/')
                pathname+=(strlen(DIR)+1);
            else
                pathname+=(strlen(DIR));

            node->sibling[0]= mkdir(node->sibling[0], pathname, node);
        }
    }
    else
    {
        char *DIR= getDIR(pathname, 0);
        if (*pathname!='\0')
        {
            int j=5;
            for (int i =0; i<5; ++i)
            {
                if ((node->sibling[i])!=NULL)
                {
                    if (strcmp (node->sibling[i]->path, DIR)==0)
                    {
                        if (*pathname=='/')
                            pathname+=(strlen(DIR)+1);
                        else
                            pathname+=(strlen(DIR));

                        node->sibling[i]= mkdir(node->sibling[i], pathname, node);
                        j=6;
                        break;
                    }
                }else
                {
                    j=i;
                }
            }
            if (j<5)
            {
                node->sibling[j]= mkdir(node->sibling[j], pathname, node);
            }else if (j==5)
            {
                printf("Sorry, there is no such path in directory and the folder is full to create a new one :(\n");
                return node;
            }
        }
    }
    return node;
}

//change the directory
NODE *cd(NODE *node, char *pathname)
{
    if ((node != NULL)&&(*pathname!='\0'))
    {
        char *DIR= getDIR(pathname, 0);

        if (*pathname=='/')
            pathname+=(strlen(DIR)+1);
        else
            pathname+=(strlen(DIR));

        int i=0;
        for (i =0; i<5; ++i)
        {
            if ((node->sibling[i])!=NULL)
            {
                if (strcmp (node->sibling[i]->path, DIR)==0)
                {
                    node= cd(node->sibling[i], pathname);
                    break;
                }
            }
        }
        if ((i==5)&&(pathname!='\0')&&(strcmp(DIR,"/" )!=0))
        {
            printf("Sorry, there is no such path in directory :(\n");
        }
    }
    return node;
}

//list the content of a directory and its siblings
void ls(NODE* node, int level)
{
    if (node!=NULL)
    {
        printf("Level %d: %s \n", level, node->path);
        for (int i=0; i<5; ++i)
        {
            ls(node->sibling[i], level-1);
        }
    }
}

//print the absolute pathname of the current directory
char* pwd(NODE* node)
{
    char *absolute_path = (char *) malloc(10*sizeof(char));
    char temp_path[10];

    if (node!=NULL)
    {
        int j=0;
        while (strcmp (node->path, "/")!=0)
        {
            int i=0;
            while(node->path[i]!='\0')
            {
                temp_path[j]= node->path[i];
                ++j;
                ++i;
            }
            temp_path[j]='/';
            ++j;
            node= node->parent;
        }
        temp_path[j]='\0';

        j=0;
        for(int i=0; i<(strlen(temp_path));  ++i)
        {
            absolute_path[j]= temp_path[strlen(temp_path)-(i+1)];
            ++j;
        }
        absolute_path[j]='\0';
   }
    return absolute_path;
}

//remove the directory
void rmdir(NODE* node)
{
    if (node!=NULL)
    {
        free(node);
        for (int i=0; i<5; ++i)
        {
            rmdir(node->sibling[i]);
            node->sibling[i]=NULL;
        }
        if (node->parent != NULL)
        {
            for (int i=0; i<5; ++i)
            {
                if (node->parent->sibling[i] == node)
                node->parent->sibling[i]= NULL;
            }
        }
        node=NULL;
    }
}

//create a file node
void create(NODE* node, char* file_name)
{
    if (node!=NULL)
    {
        int i=0;
        while(*file_name!='\0')
        {
            node->file[i]= *file_name;
            ++file_name;
            ++i;
        }
        node->file[i]='\0';
    }
}

//remove a file node
void rm(NODE* node)
{
    if (node!=NULL)
    {
        memset (node->file, 0, sizeof (node->file));
    }
}

int main()
{
    char rootpath[10]="/";
    NODE *root = new_node(rootpath, NULL);
    NODE *CWDir = root;

    while (1)
    {
        char command[100];
        char pathname[100];

        memset (command, 0, sizeof (command));
        memset (pathname, 0, sizeof (pathname));

        printf("Please select the command that you want to do\n");
        scanf(" %[^\n]s", command);

        int i=0;
        pathname[0]='\0';
        while((command[i]!=' ')&&(command[i]!='\0'))
        {
            ++i;
        }
        int j=0;
        command[i]='\0';
        ++i;
        while (command[i]!='\0')
        {
            pathname[j]=command[i];
            command[i]=' ';
            ++j;
            ++i;
        }
        pathname[j]='\0';

        //printf("dernier repertoire %s\n", getDIR(pathname, 1));
        if (pathname[0]=='/')
        {
            CWDir=root;
        }

        if (strcmp(command,"mkdir" )==0)
        {
            CWDir=mkdir(CWDir, pathname, NULL);
            CWDir=cd(CWDir, pathname);

            printf("Current directory: %s\n", CWDir->path);
            printf("The file system tree is like this:\n");
            ls(root,0);

        }else if (strcmp(command,"cd" )==0)
        {
            if (pathname[0]=='\0')
            {
                CWDir=root;
            }else
                CWDir=cd(CWDir, pathname);

            printf("Current directory: %s\n", CWDir->path);
            printf("The file system tree is like this:\n");
            ls(root,0);

        }else if (strcmp(command,"ls" )==0)
        {
            if (pathname[0]!='\0')
            {
                CWDir=cd(CWDir, pathname);
                if (strcmp(CWDir->path,getDIR(pathname, 1))==0)
                {
                    printf("The file system tree is like this:\n");
                    ls(CWDir, 0);
                }
            }else
            {
                printf("The file system tree is like this:\n");
                ls(CWDir, 0);
            }

        }else if (strcmp(command,"rmdir" )==0)
        {
            CWDir=cd(root, pathname);
            if (strcmp(CWDir->path,getDIR(pathname, 1))==0)
            {
                rmdir(CWDir);
                printf("Directory %s and its sibings removed from directory\n",getDIR(pathname, 1));
                printf("The file system tree is like this:\n");
                ls(root, 0);
            }

        }else if (strcmp(command,"pwd" )==0)
        {
            printf("The absolute pathnme of CWD is: %s \n", pwd(CWDir));

        }else if (strcmp(command,"create" )==0)
        {
            CWDir= cd(root, pathname);
            if (strcmp(CWDir->path,getDIR(pathname, 1))==0)
            {
                create(CWDir, "file node");
                printf("Current directory: %s\n", CWDir->path);
                printf("File name: %s\n", CWDir->file);
            }

        }else if (strcmp(command,"rm" )==0)
        {
            CWDir= cd(root, pathname);
            if (strcmp(CWDir->path,getDIR(pathname, 1))==0)
            {
                rm(CWDir);
                printf("Current directory: %s\n", CWDir->path);
                printf("File name: %s\n", CWDir->file);
            }
        }else if (strcmp(command,"quit" )==0)
        {
            rmdir(root);
            break;
        }else
        {
            printf("Please select a valid command (mkdir pathname, rmdir pathname, cd pathname, ls pathname, pwd , create pathname or rm pathname),\ndon't forget the space between the command and the pathname :)  \n");
        }
    }
}

