#include "startServer.h"

int startServer(string program, char* arguments[])
{
    // PID type for storing the process's PID
    pid_t mypid;
    // Pipe for writing data to the child. This will become stdin in the child
    int wpipefd[2];
    // Pipe for reading data from the child. This will become stdout in the child
    int rpipefd[2];
    // Status for keeping track of errors
    int status;

    // Open the write pipe
    status = pipe(wpipefd);

    // If there was a failure opening the pipe
    if (status == -1)
    {
        perror("pipe");
        return 20;
    }

    // Open the read pipe
    status = pipe(rpipefd);

    // If there was a failure opening the pipe
    if (status == -1)
    {
        perror("pipe");
        return 20;
    }

    // Create 2D array from pipe arrays
    // Layout: row 0 - stdout pipe
    //         row 1 - stdin pipe
    //         col 0 - read end of pipe
    //         col 1 - write end of pipe
    int* serverPipe[] = { wpipefd, rpipefd };

    // Add 2D array to global array of server pipes
    serverPipes.push_back(serverPipe);

    // Fork the process and store the return value
    mypid = fork();

    // If fork returned a negative value
    if (mypid < 0)
    {
      // Write an error message to the screen
      writeLog("There was an error creating the fork. No child process was created. Exiting...");
      // Exit the program
      return 2;
    }

    // If fork returned a positive number
    else if (mypid > 0)
    {
      // This process is the parent
      // Close the write end of what will become stdout for the child
      close(serverPipe[0][1]);
      // Close the read end of what will become stdin for the child
      close(serverPipe[1][0]);
    }

    else
    {
        // This process is the child
        // Close the read end of what will become stdout
        close(serverPipe[0][0]);
        // Close the write end of what will become stdin
        close(serverPipe[1][1]);

        // Stdout and stdin should already be closed in main.cpp/initialize()
        // Duplicate the read pipe to stdin
        dup2(serverPipe[1][0], STDIN_FILENO);
        // Duplicate the write pipe to stdout
        dup2(serverPipe[0][1], STDOUT_FILENO);

        execv(program.c_str(), arguments);
    }

    return 0;
}
