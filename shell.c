#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ 0;
#define WRITE 1;

#define FALSE 0;
#define TRUE 1;

typedef int Bool;

const char * PIPE_DELIMETER = "|";
const char * SPACE_DELIMETER = " ";

struct Pipe
{
    int fd[2];
};

typedef struct Pipe Pipe;

/*
*   Split up string by a delimeter and store each substring inthe output arr
*   @param command the command that is going to split
*   @param delimeter the delimeter to scan for
*   @param commands the arr with the split substrings
*   @return size the number of command substrings (not the NULL)
*/
size_t split(const char* command, const char* delimeter, char* commands[16])
{
    size_t size = 0;
    char *tokens;

    char *command_copy = strdup(command);

    tokens = strtok(command_copy, delimeter);

    while (tokens != NULL)
    {
        commands[size++] = strdup(tokens);
        tokens = strtok(NULL, delimeter);
    }

    commands[size] = NULL;
    return size;
}


/*Execute command runs single command. sets up pipes for in/out 
*/
void execute(const char *command, int redirect_input_pipe[2], int redirect_output_pipe[2], Pipe* pipes)
{
    char *commands[16];
    size_t number_of_commands = split(command, SPACE_DELIMETER, commands);   

    if (fork() == 0) //if it's a child
    {
    execvp(commands[0], commands);
    perror("Exec failed. We should not be here.");
    exit(EXIT_FAILURE);
    } else { //otherwise it's a parent

    }
}

int main()
{
    char input[256];

    char command[256] = "ls -l|wc|wc";

    char *commands[16];

    struct Pipe* pipes;

    printf(">> ");

    fgets(input, 256, stdin);

    while (strcmp(input, "QUIT\n")){
        size_t number_of_commands = split(command, PIPE_DELIMETER, commands);

        for(size_t i=0; i < number_of_commands; ++i)
        {
            printf(">%s<\n", commands[i]);

        }

        pipes = malloc(number_of_commands -1 * sizeof(Pipe));

        for (size_t i = 0; i < number_of_commands-1; ++i)
        {
            if (pipe(pipes[i].fd) < 0)
            {
                perror("Pipe init failed.");
                exit(EXIT_FAILURE);
            }
        }

        if (number_of_commands == 1) {
            execute(commands[0], NULL, NULL, pipes);

        } else {
            for(size_t i=0; i<number_of_commands; ++i)
            {
                if (i == 0){
                    execute(commands[i], NULL, pipes[i].fd, pipes);
                } else if (i == number_of_commands-1) {
                    execute(commands[i], pipes[i-1].fd, NULL, pipes);
                } else {
                    execute(commands[i], pipes[i-1].fd, pipes[i].fd, pipes);
                }
            }
        }

        printf(">> ");

        fgets(input, 256, stdin);

        printf("You said: %s", input);
    }

}