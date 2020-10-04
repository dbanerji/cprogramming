#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/in.h>



//function declaration
void *requesthandler(void *args);

typedef struct
{
    int sock;
    struct sockaddr address;
    int addr_len;
} connection_t; 


int main(int argc, char * argv[]){

    int sock = -1;
    struct sockaddr_in address;
    int port;
    connection_t * connection;
    pthread_t thread_id;

	   /* check for command line arguments */
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        return -1;
    }

    /* obtain port number */
    if (sscanf(argv[1], "%d", &port) <= 0)
    {
        fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
        return -2;
    }

    /* create socket */
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock <= 0)
    {
        fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
        return -3;
    }

    /* bind socket to port */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
        return -4;
    }   

    /* listen on port */
    if (listen(sock, 4) < 0)
    {
        fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
        return -5;
    }

        printf("%s: ready and listening\n", argv[0]);

        while (1)
    {
        /* accept incoming connections */
        connection = (connection_t *)malloc(sizeof(connection_t));
        connection->sock = accept(sock, &connection->address, &connection->addr_len);
        if (connection->sock <= 0)
        {
            free(connection);
        }
        else
        {
            /* start a new thread but do not wait for it */
            pthread_create(&thread_id, NULL, requesthandler, (void *)connection);
            pthread_detach(thread_id);
        }
    }

     return 0;
}



void *requesthandler(void *args){
    
    char *data;
    int size;
    
    connection_t * conn;
    long address=0;

    if(!args) pthread_exit(0);

    conn = (connection_t *) args;
    
    printf("Enter the length of characters to write:\n");
    read(conn -> sock, &size, sizeof(int));

    if(size > 0 && size < INT_MAX){
        
        address = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
        data =  (char *)malloc((size+1)*sizeof(char));
        data[size] = 0;
        
        printf("Enter the data\n");
        read(conn->sock, data, size);
        
        //file pointer
        FILE *fptr;  
        
        //to open/create a file named "newfile.txt" in the write mode
        fptr=fopen("newfile.txt", "w");

        //checks if the fopen function returns a null pointer   
        if(fptr == NULL){                     
        printf("Error opening the file..");
        exit(1);
        }
        
        fwrite(data, sizeof(char), (size-1), fptr);  

        printf("Data successfully written into the file");
        
        fclose(fptr);
        free(data);


    }
    close(conn->sock);
    free(conn);
    pthread_exit(0);


}




