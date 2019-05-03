/*
Cian Sincliar
C15423182
System Software Assignment 2
Multithreaded Server / Client program
Server
*/
//imports
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h> //for threading , link with lpthread
#include "uploadclientfile.h"

//defines vars
#define PORT 9999
#define TRUE 1
#define FALSE 0
#define MAXCLIENTS 5
#define ROOT 0

// global vars
int connectionSize;
int socket_des;
struct sockaddr_in  server;
struct sockaddr_in client;
pthread_mutex_t lock_x;

//functions
void AcceptMulClients();
void *ThreadClient(void *ptr);
void uploadclientfile (int CID, int ClientUID, char fp[], char dir_loc[]);
int ChangeServerEUID(int ID);

//main
int main (int argc, char *argv[])
{
    //server vars
    int c_socket, readSize;
    // s = socket_des, cs = c_socket, connectionSize = connSize, readSize = readsize

    char msg[500];

    //create socket
    socket_des = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_des == -1)
    {
        puts("Socket not created\n");
        exit(1);
    }
    else
    {
        puts("Socket Created\n");
    }

    //prepare socket setup
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //bind
    if (bind(socket_des, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind Failed\n");
        exit(1);
    }
    else
    {
        printf("Bind Successful on port: %d\n", PORT);
    }

    //listen
    listen(socket_des, MAXCLIENTS);

    //accept connections
    puts("Waiting for connections.....\n");

    while(TRUE)
    {
        AcceptMulClients();
    }

    puts("Server Closing...\n");
    close(socket_des);
    close(c_socket);
    exit(0);
}

//function to accept multiple clients and set up sockets
void AcceptMulClients()
{
    int client_socket;

    listen(socket_des, MAXCLIENTS);
    connectionSize = sizeof(struct  sockaddr_in);

    client_socket = accept(socket_des, (struct socketaddr *)&client, (socklen_t*)&connectionSize);


    if (client_socket < 0)
    {
        perror("Cant make connection\n");
        exit(EXIT_FAILURE);
    }
    
    pthread_t client_thread;
    printf("\nConnection from Client %d accepted\n", client_socket);
    int *c = malloc(sizeof(*c));
    *c = client_socket;
    int threadReturn = pthread_create(&client_thread, NULL, ThreadClient, (void*) c);

    if(threadReturn)
    {
        printf("Failed to create client thread: %dn", threadReturn);
    }
}

//thread per client functionality 
void *ThreadClient(void *client_socket)
{
    int readSize;
    int i = 0;
    char *temp[30];
    char msg[500];
    memset(msg, 0, 500);
    int clientID, details_check;
    char *details[10];

    int CID = *((int*) client_socket);

    int thread_ID = pthread_self();


    readSize = recv( CID, msg, 2000, 0);

    // Splitting string and conversion
    int init_size = strlen(msg);
    char delim[] = ",";

    char *ptr = strtok(msg, delim);

    while(ptr != NULL)
    {
        details[i++] = ptr;
        ptr = strtok(NULL, delim);
    }
    printf("File %s\n", details[1]);
    printf("Dir %s\n", details[2]);
    //convert string to int 
    clientID = atoi (details[0]);

    int check = ChangeServerEUID(clientID);

    if (check == FALSE)
    {
        printf("! Server did not change EUID !\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server ID changed Successfully\n");
    }

    // 0 - Socket, 1 - Client ID, 2 - File Source, 3 - Directory Destination 
    /*
    printf("%d\n", CID );
    printf("%d\n", clientID );
    printf("%s\n", details[1] );
    printf("%s\n", details[2] );
    */

    uploadclientfile(CID, clientID, details[1], details[2]);

    check = ChangeServerEUID(0);
    if (check == FALSE)
    {
        printf("! Server didn't reset EID !\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server ID Reset\n");
    }

    pthread_mutex_unlock(&lock_x);

    pthread_exit(NULL);
}

int ChangeServerEUID(int CID)
{
    pthread_mutex_lock(&lock_x);

    uid_t s_euid = geteuid();
    printf("Server ID: %d\n", s_euid);
    printf("Client ID: %d\n", CID);

    if (seteuid(CID) < 0)
    {
        printf("Cannot Change EUID\n");
        return FALSE;
    }
    else
    {
        uid_t tmp = geteuid();
        printf("Server Temp ID: %d\n", tmp );
        return TRUE;
    }
    sleep(1);
}
