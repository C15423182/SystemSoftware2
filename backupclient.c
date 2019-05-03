/*
Cian Sincliar
C15423182
System Software Assignment 2
Multithreaded Server / Client program
Client Side
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//static vars
#define IP "127.0.0.1"
#define PORT 9999
#define TRUE 1
#define FALSE 0

//global vars
int SID;
char FILE_SOURCE[200];
char DIR_DEST[200];
char client_msg[500];
char server_msg[500];
struct sockaddr_in  server;

//functions
int SocketCreate();

//main
int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Failed\nFile must be ran: ./<program> <file name> <directory location>");
        exit(EXIT_FAILURE);
    }

    strcpy(FILE_SOURCE, argv[1]);
    strcpy(DIR_DEST, argv[2]);

    uid_t euid = geteuid();

    int socket_fd;


    if (strcmp(DIR_DEST,"Sales") == 0 || strcmp(DIR_DEST,"Promotions") == 0 || 
        strcmp(DIR_DEST,"Offers") == 0 || strcmp(DIR_DEST,"Root") == 0 || strcmp(DIR_DEST,"Sales") == 0)
    {
        printf("Location Accepted\n");
    }
    else
    {
        printf("Location Failed\n");
        exit(EXIT_FAILURE);
    }

    SID = SocketCreate();

    char EUID_STR[10];
    sprintf(EUID_STR, "%d", euid);
    printf("%s\n", EUID_STR );

    char *details = malloc(strlen(EUID_STR) + strlen(FILE_SOURCE) + strlen(DIR_DEST) + 1);
    strcat(details, EUID_STR);
    strcat(details, ",");
    strcat(details, FILE_SOURCE);
    strcat(details, ",");
    strcat(details, DIR_DEST);

    int msg_size = strlen(details);
    int sent = 0;

    while(sent != 1)
    {
        if (( send(SID, details, msg_size, 0) < 0 ) && (sent != 1))
        {
            puts("Error Sending File Details\n");
            exit(EXIT_FAILURE);
        }
        sent = 1;
    }

    sleep(1);

    /*send file to server*/

    char* file_name[200];
    char server_reply[1000];

    strcpy(file_name, FILE_SOURCE);
            
    char file_buffer[512]; 

    printf("Sending %s to the Server... ", file_name);

    FILE *file_open = fopen(file_name, "r");

    if (file_open == NULL)
    {
        printf("Cannot Find File\nExitting...");
        exit(EXIT_FAILURE);
    }

    bzero(file_buffer, 512); 

    int block_size, i=0; 
    printf("Transfering File bits....\n\n");
    while((block_size = fread(file_buffer, sizeof(char), 512, file_open)) > 0) 
    {
        printf("Data Sent %d = %d\n",i,block_size);

        if(send(SID, file_buffer, block_size, 0) < 0) 
        {
            printf("Transfer Failed\n");
            exit(EXIT_FAILURE);
        }

        bzero(file_buffer, 512);
        i++;
    }

    sleep(1);
    /*
    if(recv(SID,server_reply,strlen(server_reply),0 ) < 0)
    {
        printf("Error Receving message");
        //break;
    }
    printf("{S}: %s \n",server_reply );
    */
    //close socket
    close(SID);
    exit(EXIT_SUCCESS);;
}

//function to create client socket
int SocketCreate()
{
    SID = socket(AF_INET, SOCK_STREAM, 0);

    if (SID == -1)
    {
        puts("Error Client Socket Creation\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        puts("Client Socket Creation Complete\n");
    }

    //prepare socket setup
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(PORT);

    if (connect(SID, (struct socketaddr *)&server, sizeof(server)) < 0)
    {
        puts("Connection failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        puts("Connection made to server");
    }
    return SID;
}