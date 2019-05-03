/*
Cian Sincliar
C15423182
System Software Assignment 2
Multithreaded Server / Client program
Function for uploading files to  server
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

//defs
#define TRUE 1
#define FALSE 0

//vars
char DIR_DEST[200];
char CMD[300];

//functions
int recieve_file (int CID, char file_ptr[]);
int file_exist (int CID, char filename[]);


//main function
void uploadclientfile (int CID, int ClientUID, char fp[], char dir_loc[])
{
	int transfer_check;
	int cmd_check;

    if (strcmp(dir_loc,"Root") == 0 )
    {
    	strcpy(DIR_DEST,"/var/www/html/intranet/Root/");
    	printf("Directory Destination: %s\n",DIR_DEST);	
    }
    else if (strcmp(dir_loc,"Sales") == 0 )
    {
    	strcpy(DIR_DEST,"/var/www/html/intranet/Sales/");
    	printf("Directory Destination: %s\n",DIR_DEST);	
    }
    else if (strcmp(dir_loc,"Offers") == 0 )
    {
    	strcpy(DIR_DEST,"/var/www/html/intranet/Offers/");
    	printf("Directory Destination: %s\n",DIR_DEST);	
    }    
    else if (strcmp(dir_loc,"Marketing") == 0 )
    {
    	strcpy(DIR_DEST,"/var/www/html/intranet/Marketing/");
    	printf("Directory Destination: %s\n",DIR_DEST);	
    }    
    else if (strcmp(dir_loc,"Promotions") == 0 )
    {
    	strcpy(DIR_DEST,"/var/www/html/intranet/Promotions/");
    	printf("Directory Destination: %s\n",DIR_DEST);	
    }
    else
    {
    	printf("DIRECTORY NOT FOUND!\nEXITING");
    	exit(EXIT_FAILURE);
    }

    // transfer from master file to tmp 
    transfer_check = recieve_file(CID, fp);

    if (transfer_check == TRUE)
    {
    	printf("File Transfer Complete\n");
    }
    else
    {
    	printf("File Transfer Failed\n");
    }

    /*
    uid_t s_euid = geteuid();
    printf("euid %d\n" , s_euid);
    printf("id: %d\n", ClientUID);
    */

    if (chown ("/tmp/server_temp.txt", ClientUID, ClientUID) == -1)
    { 
    	printf("Chown Failed\n"); 
    } 
    else
    { 
    	printf("Chown Success\n"); 
    } 

	//printf("%s\n", CMD);

	cmd_check = system(CMD);

	if (cmd_check == -1)
	{
		printf("System Command Failed\n");
	}
	sleep(1);
	
	// file check
	//transfer_check = file_exist(CID, "server_temp.txt");

	if (transfer_check == TRUE)
	{
		printf("Transfer Success, File Exists\n");
	}
	else
	{
		printf("Transfer Failed, File Doesnt Exist\n");
	}
	
}


int recieve_file (int CID, char file_ptr[])
{
	int check;

    /*Receive File from Client */

    char file_buffer[512]; // Receiver buffer

    char* file_tmp = "/tmp/server_temp.txt";
           
    FILE *file_open = fopen(file_tmp, "w");

    if(file_open == NULL)
    {
    	printf("File %s Cannot be opened file on server.\n", file_open);
    	check = FALSE;
    }
    else 
    {
    	printf("\nTemp File Opened\n");
        bzero(file_buffer, 512); 
        int block_size = 0;
        int i=0;

        while((block_size = recv(CID, file_buffer, 512, 0)) > 0) 
        {
            printf("Data Received %d = %d\n",i ,block_size);
            int write_sz = fwrite(file_buffer, sizeof(char), block_size, file_open);
            bzero(file_buffer, 512);
            i++;
        }
        check = TRUE;
    }

    fclose(file_open); 

    strcpy(CMD, "cp ");
    strcat(CMD, file_tmp);
    strcat(CMD, " ");
    strcat(CMD, DIR_DEST);

    return check;
}

int file_exist (int CID, char filename[])
{
  	int check; 
  	char msg[100];
	char filecheck_loc[200];
	strcpy(filecheck_loc, DIR_DEST);
	strcat(filecheck_loc, filename);

	FILE *file;

	if ((file = fopen(filecheck_loc, "r")))
	{
		fclose(file);
		check = TRUE; 
		strcpy(msg,"{S} File Exists at Destination\n");
	 	//write(CID,msg,strlen(msg));
	}
	else
	{
		check = FALSE;
		strcpy(msg,"{S} File Doesnt Exists at Destination");
	 	//write(CID,msg,strlen(msg));
	}
	return check;
}