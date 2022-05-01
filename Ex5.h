#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>

#include <pthread.h>

#define PORT "3490" // the pסrt users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

#define CLIENT_NUMBER 3 // how many clients can connect server parllel

#define MAXDATASIZE 1024 // max number of bytes we can get at once
// ********************* malloc, free and calloc*********************************
void * mymalloc ( size_t size )
{
    int *plen;
    int len = size + sizeof( size ); // Add sizeof( size ) for holding length.
    printf("hehere1\n");
    plen = mmap( 0, len, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0 );
    printf("hehere2\n");
    *plen = len;                     // First 4 bytes contain length.
    return (void*)(&plen[1]);        // Memory that is after length variable.
}

void myfree ( void * ptr )
{
    int *plen = (int*)ptr;
    int len;

    plen--;                          // Reach top of memory
    len = *plen;                     // Read length

    munmap( (void*)plen, len );
}
// ********************** tbb lock ********************************

// int flagp = 1;
// // pthread_mutex_t mutex;
// mutex myMutex1;
// mutex myMutex2;
// mutex myMutex3;
//********************* stack ***************************
typedef struct StackNode
{
    char data[MAXDATASIZE];
    struct StackNode *next;
} StackNode, *pStackNode;

pStackNode myNewNode(char *data)
{
    printf("im here1\n");
    pStackNode newN = (pStackNode)(mmap(0, sizeof(StackNode), PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0 ));
    printf("hehere4\n");
    bzero(newN->data, MAXDATASIZE);
    printf("hehere4\n");
    strcpy(newN->data, data);
    newN->next = NULL;
    printf("im here\n");
    return newN;
}

void push(char *data, pStackNode *root)
{
    // pthread_mutex_lock(&mutex);
    // myMutex1.lock();
    // if (flagp)
    // {
    //     flagp = 0;
    //     sleep(5);
    // }
    pStackNode newN = myNewNode(data);
    newN->next = *root;
    *root = newN;
    printf("\'");
    for (int i = 0; i < strlen(data); i++)
    {
        printf("%c", data[i]);
    }
    printf("\'");
    printf(" push to the stack\n");
    // pthread_mutex_unlock(&mutex);
    // myMutex1.unlock();
}
void pop(pStackNode *root)
{
    // myMutex2.lock();
    pStackNode temp = *root;
    *root = (*root)->next;
    printf("\'");
    for (int i = 0; i < strlen(temp->data); i++)
    {
        printf("%c", temp->data[i]);
    }
    printf("\' pop from the stack\n");
    myfree(temp);
    // myMutex2.unlock();
}
void top(pStackNode *root, int sockfd)
{
    // myMutex3.lock();
    char input[1024] = {0};
    strcat(input, "OUTPUT: ");
    for (int i = 0, j = 8; i < strlen((*root)->data); j++, i++)
    {
        input[j] = (*root)->data[i];
    }
    if (send(sockfd, input, strlen(input), 0) == -1)
    {
        perror("send");
    }
    // myMutex3.unlock();
}
//****************Bonus Queue*******************************
//for the bonus:
// void enqueue(char *data, pStackNode *root)
// {
//     // pthread_mutex_lock(&mutex);
//     myMutex1.lock();
//     if (flagp)
//     {
//         flagp = 0;
//         sleep(5);
//     }
//     pStackNode newN = myNewNode(data);
//     if (!*root)
//     {
//         *root = newN;
//         newN->next = NULL;
//         printf("\'");
//         for (int i = 0; i < strlen(data); i++)
//         {
//             printf("%c", data[i]);
//         }
//         printf("\'");
//         printf(" enqueue to the stack\n");
//         return;
//     }
//     pStackNode temp = *root;
//     while (temp->next)
//     {
//         temp = temp->next;
//     }
//     temp->next = newN;
//     newN->next = NULL;
//     printf("\'");
//     for (int i = 0; i < strlen(data); i++)
//     {
//         printf("%c", data[i]);
//     }
//     printf("\'");
//     printf(" enqueue to the stack\n");
//     // pthread_mutex_unlock(&mutex);
//     myMutex1.unlock();
// }
// void dequeue(pStackNode *root, int sockfd)
// {
//     myMutex2.lock();
//     pStackNode temp = *root;
//     *root = (*root)->next;
//     printf("\'");
//     for (int i = 0; i < strlen(temp->data); i++)
//     {
//         printf("%c", temp->data[i]);
//     }
//     printf("\' dequeuq from the stack\n");
//     char input[1024] = {0};
//     strcat(input, "OUTPUT: ");
//     for (int i = 0, j = 8; i < strlen(temp->data); j++, i++)
//     {
//         input[j] = temp->data[i];
//     }
//     if (send(sockfd, input, strlen(input), 0) == -1)
//     {
//         perror("send");
//     }
//     myfree(temp);
//     myMutex2.unlock();
// }
//until here
