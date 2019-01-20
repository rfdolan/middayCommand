#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>

/*
    Prints the message which displays all available commands.
*/
void printWelcomeMessage() {
    // Print for each command
    printf( "\nG'day, Commander! What command would you like to run?\n");
    printf("\t0. whoami\t: Prints out the result of the whoami command\n");
    printf("\t1. last\t\t: Prints out the result of the last command\n");
    printf("\t2. ls\t\t: Prints out the result of a listing on a user specified path\n");
    printf("Option?:");
}

/*
    Prints out the statistics for the running program
*/
void printStats(struct rusage usage ) {
    // Calculate the total time it took to execute the process
    int systemMili = ((usage.ru_stime.tv_usec) / 1000) + ((usage.ru_stime.tv_sec) * 1000); 
    int userMili = ((usage.ru_utime.tv_usec) / 1000) + ((usage.ru_utime.tv_sec) * 1000); 
    int totalTime = (systemMili) + (userMili);

    // Print out all relevant stats
    printf("\n---Statistics---\nElapsed time: %d milliseconds\n", totalTime);
    printf("Page Faults: %ld\nPage Faults (reclaimed): %ld\n", usage.ru_majflt, usage.ru_minflt);
}

/*
    Creates a child process and executes the inputted command in it
*/
void child( char* command, char* argv[] ) {
    // Fork
    int rc = fork();

    // If we encountered some kind of erroe, return an error message and exit
    if( rc < 0 )
    {
        printf("We forked up");
        exit(1);
    }
    
    // If fork returned 0, we are the child, so use execvp to run the process
    if( rc == 0 ) //we are child
    {
        execvp( command, argv ); 
    }

    // We must be the parent, so wait for the child to finish
    else
    {
        wait(NULL);
    }
}

/*
    Function which calls child for the "whoami" process
*/
void sayWho() {
    // Set argv to the appropriate value
    char* argv[] = {"whoami", NULL};

    // Initialize the struct to keep track of the stats
    struct rusage usage;

    // Create a child process
    child( "whoami", argv);

    // Update usage and print out the stats
    getrusage( RUSAGE_SELF, &usage );
    printStats( usage );
}

/*
    Function which calls child for the "last" process
*/
void sayLast() {
    // Set argv to the appropriate value
    char* argv[] = {"last", NULL};

    // Initialize the struct to keep track of the stats
    struct rusage usage;

    // Create a child process
    child( "last", argv );

    // Update usage and print out the stats
    getrusage( RUSAGE_SELF, &usage );
    printStats( usage );
}

/*
    Function which calls child for the "ls" process
*/
void list() {
    // Allocate strings for the arguments we want to accept
    char *args = malloc(128 * sizeof(char));
    char *path = malloc(128 * sizeof(char));

    // Initialize argv
    char* argv[] = {"ls", NULL, NULL, NULL};

    // Initialize the struct to keep tracl of the stats
    struct rusage usage;

    // Get the arguments and path
    printf("Arguments?: ");
    fgets(args, 128, stdin);
    printf("Path?: ");
    fgets(path, 128, stdin);

    // Remove the \n character from the end of path and put path into argv
    path[(strlen(path))-1] = '\0';
    argv[1] = path;

    // If the user inputted arguments, remove the \n from the end and put args into argv 
    if(args[0] != '\n')
    {
        args[(strlen(args))-1] = '\0';
        argv[2] = args;
    }

    // Create a child process
    child( "ls", argv );

    // Update usage and print out the stats
    getrusage( RUSAGE_SELF, &usage );
    printStats( usage );

    // free the memory taken up by path and args
    free(args);
    free(path);
}

/*
    Function that takes the operation character and calls the correct function
*/
void operate( char command ) {
    switch(command) 
    {
        case '0' :
            printf("\n-- Who Am I? --\n");
            sayWho();
            break;
        case '1' :
            printf("\n-- Last Logins --\n");
            sayLast();
            break;
        case '2' :
            printf("\n-- Directory Listing --\n");
            list();
            break;
        case 'e' :
            printf("Logging you out, Commander.\n");
            exitShell();
            break;
        
        // If the command is not recognized, return an error message
        default:
        printf("\nNot sure what that means commander.\n");
    }
}




int main( int argc, char** argv) {
    printf("\n===== Mid-Day Commander, v0 =====");
    
    // Loop forever
    while(1)
    {
        // Print out the list of commands the user can input
        printWelcomeMessage();

        // Get the user's input
        char command_buff[128];
        char command;
        fgets(command_buff, 128, stdin);
        sscanf(command_buff, "%c", &command);

        // Perform the operation the user entered
        operate( command );
    }	
    return 0;
}

