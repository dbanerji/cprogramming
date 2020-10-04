#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

//function declaration
int validate();
void *writetofile(void *params);

struct input
{
    char *data;
    int datalength;
};

int main()
{

    //stores size of the file
    int size;

    struct input params;
    //stores the data in the form of a dynamic array of characters
    char *data;

    int index = 0;

    //size of the file after exiting from the terminal using the '$' character
    int actual_size = 0;

    //function call that checks if the size is in valid range
    size = validate();

    //malloc is used for dynamic memory allocation a dynamic array
    data = malloc(size * sizeof(char));

    printf("Enter the data and press ($ and Enter) to exit:\n");

    /*Loop runs for the condition where both the length is less than that specified by the user
    and the character entered in stdin is not a $*/
    while (index < size && data[index - 1] != '$')
    {

        scanf("%c", &data[index]);
        index++;

        //keeps a count of the actual size of data written to the file
        actual_size++;
    }

    params.datalength = actual_size;
    params.data = data;

    pthread_t tid;
    pthread_create(&tid, NULL, writetofile, (void *)&params);
    pthread_join(tid, NULL);

    return 0;
}

/* 
This function validates range of integer datatype
@return: int - valid size
*/
int validate()
{

    //Flag is used to validate the input type
    int size, flag = 0;

    while (1)
    {
        printf("Enter the length of characters to write:\n");

        //stores size of the file from the stdin
        flag = scanf("%d", &size);

        //This is used to flush the input buffer instead of using the unpredictable nature of fflush(stdin)
        while ((getchar()) != '\n')
            ;

        //checks if size is not 0, or negative or greater than maximum limit
        if (size >= 1 && size <= INT_MAX && flag == 1)
        {
            return size;
        }

        else
        {
            printf("Input is out of range! Please try again.\n");
        }
    }
}

/* 
This function writes data to a file
@param1: data - Character Array that holds the data
@param2: size - Integer which holds length of the array
@return: void
*/
void *writetofile(void *args)
{

    char threadid[256];
    int thread = pthread_self();
    sprintf(threadid, "%d", thread);
    char filename[] = "newfile";
    strcat(filename, threadid);
    strcat(filename, ".txt");

    //file pointer
    FILE *fptr;

    //to open/create a file named "newfile.txt" in the write mode
    fopen_s(&fptr, filename, "w");

    //checks if the fopen function returns a null pointer
    if (fptr == NULL)
    {
        printf("Error opening the file..");
        exit(1);
    }

    /*the fwrite function is used to write a block of data to 
    location pointed by the file pointer in the file*/
    fwrite(((struct input *)args)->data, sizeof(char), (((struct input *)args)->datalength) - 1, fptr);

    printf("Data successfully written into the file");

    //to close the file and flush the buffers
    fclose(fptr);

    //deallocates the memory previously pointed to by the dynamic array
    free(((struct input *)args)->data);
}