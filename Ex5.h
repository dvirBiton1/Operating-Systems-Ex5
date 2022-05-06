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
#include <string.h>

#include <pthread.h>

#define PORT "3490" // the pסrt users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

#define CLIENT_NUMBER 3 // how many clients can connect server parllel

#define MAXDATASIZE 1024 // max number of bytes we can get at once

//********************* stack ***************************
typedef struct myStack
{
    int top;
    char data[MAXDATASIZE];
}myStack, *pmyStack;

void push(char *str, pmyStack s){
    for (int i = 0 ; i < strlen(str); i++)
    {
        s->data[s->top + 1] = str[i];
        s->top++;
    }
    s->data[s->top + 1] = '\0';
    s->top++;
}
void pop(pmyStack s){
    s->top = s->top -1;
    while (s->data[s->top] != '\0')
    {
        s->top--;
    }
}
void printS(pmyStack s){
    for (int i = 0; i < 10; i++)
    {
        printf("S[%d] = %c \n", i , s->data[i]);
    }
    printf("top = %d\n", s->top);
}

void top(pmyStack s, int sockfd)
{
    char input[1024] = {0};
    strcat(input, "OUTPUT: ");
    int k = s->top - 1;
    while (s->data[k] != '\0')
    {
        k--;
    }
    // printf("%s", s->data[k]);
    
    for (int j = 8; s->data[k + 1] != '\0' ; j++)
    {
        input[j] = s->data[k+1];
        k++;
    }
    // for (int i = 0; i < 20; i++)
    // {
    //     printf("%c", input[i]);
    // }
    if (send(sockfd, input, strlen(input), 0) == -1)
    {
        perror("send");
    }
}

