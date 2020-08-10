//
// Created by Maxi on 8/2/20.
//
#include <unistd.h>
#include "stdio.h"
#include "sys/fcntl.h"
#include "stdlib.h"
#include "string.h"

char *readALineFrom(int fromWhat);

void freeCharMalloc(char *pointer);

void child(char *argvs[]);

void commandToArgvs(char *command, char *argvs[]);

int countCharacters(const char *str, char character);

int main(int argc, char *argv[])
{
    // condition that decide the while dead loop
    int condition = 1;
    // store the input line.
    char *inputLine;
    // inside the dead loop.
    while (condition)
    {
        write(STDOUT_FILENO, ">", 1);
        // read a line from console.
        inputLine = readALineFrom(STDIN_FILENO);
        // if input line is equal to "exit" or nothing in it.
        if ((strcmp(inputLine, "exit\n") == 0) || (strcmp(inputLine, "\n") == 0))
        {
            // print Bye Bye.
            write(STDOUT_FILENO, "Bye Bye\n", 8);
            freeCharMalloc(inputLine); // free inputLine
            exit(0); // exit
        }

        /**
         * Preparing arguments
         */
        // store commands
        char *commands[256];
        for (int i = 0; i < 256; i++)
        {
            commands[i] = (char *) malloc(sizeof(char) * 256);
        }

        // get the first command and arguments from the input line.
        char *command = strtok(inputLine, ";");
        int j = 0;
        // get the rest of the command and arguments, until next command is not exist.
        while (command != NULL && j < 255)
        {
            // copy temp to commands[j]
            memcpy(commands[j], command, strlen(command) + 1);
            // new command value.
            command = strtok(NULL, ";");
            // next commands unit.
            j++;
        }
        // put the last item of commands to NULL.
        commands[j] = NULL;

        // index to loop each item in commands.
        int index = 0;
        while (commands[index] != NULL && index < 256)
        {
            // initial a 2D char array to store argument vectors.
            char *argvs[256];// store argument vectors.
            for (int i = 0; i < 256; i++)
            {
                argvs[i] = (char *) malloc(sizeof(char) * 256);
            }

            // transfer command to argument vectors.
            commandToArgvs(commands[index], argvs);

            /**
             * child process part
             */
            int pid; // store pid or child process.
            // when pid is 0, child process.
            if ((pid = fork()) == 0)
            {
                // child process
                child(argvs);
            }
            else // main process
            {
                int status; // Store exist status
                int childPID; // store exist child process's pid
                childPID = wait(&status); // wait for child process to end
                status = WEXITSTATUS(status); // transfer status to readable status
                // show a message.
                fprintf(stdout, "A child process end, pid: %d, return status: %d\n", childPID, status);
            }

            // free 2D array for next use.
            for (int i = 0; i < 256; i++)
            {
                freeCharMalloc(argvs[i]);
            }

            // get the next command and arguments.
            index++;
        }
    }
}

// read a line from somewhere
char *readALineFrom(int fromWhat)
{
    // temporary store a char
    char tmp;
    // indicate the index used in inputLine.
    int i = 0;
    // store the input:
    char *inputLine = (char *) malloc(64 * sizeof(char));
    // initial inputLine size
    int inputLineSize = 64;

    // check -1
    int n;

    // when there is next byte got read.
    while (read(fromWhat, &tmp, 1) > 0)
    {
        // if the index used in inputLine is bigger or equal to the inputLine size.
        if (i >= inputLineSize)
        {
            // double the inputLine size
            inputLineSize *= 2;
            // re-allocate the inputLine to double the size.
            inputLine = realloc(inputLine, inputLineSize * sizeof(char));
        }
        // add the char to the end of the inputLine.
        inputLine[i] = tmp;
        // if tmp equal to line end, break the loop.
        if (tmp == '\n')
        { break; }
        // increase i to index on the next using unit.
        i++;
    }

    // detect ctrl-D
    if ((n = getchar()) == -1)
    {
        // print Bye Bye.
        write(STDOUT_FILENO, "Bye Bye\n", 8);
        freeCharMalloc(inputLine); // free inputLine
        exit(0); // exit
    }

    return inputLine;
}

// read an entire file from somewhere, and return a string of it.
char *readAFileFrom(int fromWhat)
{
    // temporary store a char
    char tmp;
    // indicate the index used in inputFile.
    int i = 0;
    // store the input:
    char *inputFile = (char *) malloc(64 * sizeof(char));
    // initial inputFile size
    int inputFileSize = 64;

    // when there is next byte got read.
    while (read(fromWhat, &tmp, 1) > 0)
    {
        // if the index used in inputFile is bigger or equal to the inputFile size.
        if (i >= inputFileSize)
        {
            // double the inputFile size
            inputFileSize *= 2;
            // re-allocate the inputFile to double the size.
            inputFile = realloc(inputFile, inputFileSize * sizeof(char));
        }
        // add the char to the end of the inputFile.
        inputFile[i] = tmp;
        // increase i to index on the next using unit.
        i++;
    }
    return inputFile;
}

// free char dynamic array
void freeCharMalloc(char *pointer)
{
    // if the pointer is not NULL
    if (pointer != NULL)
    {
        // free that memory and set pointer to NULL
        free(pointer);
        pointer = NULL;
    }
}

// child process method
void child(char *argvs[])
{
    // execute the command with argument vectors, return < 0 indicate error.
    if (execvp(argvs[0], argvs) < 0)
    {
        write(STDERR_FILENO, "*** ERROR: execvp() ERROR.\n", 27);
    }
}

// transfer command to argument vectors.
void commandToArgvs(char *command, char *argvs[])
{
    // check if command is empty.
    if (*command == '\0')
    {
        return;
    }
    // replace white spaces with 0
    for (int i = 0; i < strlen(command); i++)
    {
        if (command[i] == ' ' || command[i] == '\t' || command[i] == '\n')
        {
            command[i] = ' ';
        }
    }
    // index for argument vectors.
    int j = 0;
    // temp store an argument vector.
    char *temp;
    // put an argument vector in temp.
    temp = strtok(command, " ");
    // while temp is not NULL and j < 256.
    while (temp != NULL && j < 255)
    {
        // copy temp to argvs[j]
        memcpy(argvs[j], temp, strlen(temp) + 1);
        // new temp value.
        temp = strtok(NULL, " ");
        // next argvs unit.
        j++;
    }
    // put the last item of argvs to NULL.
    argvs[j] = NULL;
}

// count the appearance of a character in a string.
int countCharacters(const char *str, char character)
{
    const char *string = str;
    int count = 0;
    int i = 0;
    while (string[i])
    {
        if (string[i] == character)
        {
            count++;
        }
        i++;
    }
    return count;
}