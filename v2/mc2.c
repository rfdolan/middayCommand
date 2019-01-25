#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>

/*
    linkedList to hold the user's commands
*/
typedef struct node {
    char* command;
    char** argv;
    struct node* next;
    int isBckg;
} node;    

/*
    adds a new node to the end of the linkedList
*/
node *addNode( node* head, char* argv[], int isBckg ) {
    node* current = head;
    
    // If we currently have no nodes in the list, create a new head
    if( current == NULL )
    {
        node* newHead = malloc( sizeof(node) );
        newHead->command = argv[0];
        newHead->argv = argv;
        newHead->next = NULL;
        newHead->isBckg = isBckg;
        return newHead;
    }
    
    // Get to the end of the linkedList
    while( current->next != NULL )
    {
        current = current->next;
    }
    
    // Allocate space for a new node and assign its fields
    node* newNode = malloc( sizeof(node) );
    newNode->command = argv[0];
    newNode->argv = argv;
    newNode->next = NULL;
    newNode->isBckg = isBckg;
    current->next = newNode;
    return head;
} 
   
/*
    Function to free the space taken up by the linkedList of user added commands
*/ 
void freeUserCommands( node* userCommands ) {
    node *theNextOne;
    
    // Free each command
    for( node *current = userCommands; current != NULL; current = theNextOne )
    {
        free( current->argv[0] );
        free( current->argv );
        theNextOne = current->next;
        free( current );
    }        
}

/*
    Prints the message which displays all available commands.
*/
void printWelcomeMessage( node* userCommands ) {
    int userCommandNum = 2;
    // Print for each command
    printf( "\nG'day, Commander! What command would you like to run?\n");
    printf("\t0. whoami\t: Prints out the result of the whoami command\n");
    printf("\t1. last\t\t: Prints out the result of the last command\n");
    printf("\t2. ls\t\t: Prints out the result of a listing on a user specified path\n");
    
    // Print out each user added command
    while( userCommands != NULL) 
    {
        userCommandNum++;
        printf("\t%i. ", userCommandNum);
        for(int i = 0; userCommands->argv[i]; i++)
        {
            printf("%s ", userCommands->argv[i]); 
        }
        printf("\t: User added command\n");
        userCommands = userCommands->next;
    }
    printf("\ta. add command : Adds a new command to the menu.\n");
    printf("\tc. change directory : Changes process working directory\n");
    printf("\te. exit : Leave Mid-Day Commander\n");
    printf("\tp. pwd : Prints working directory\n");
    printf("\tr. running processes : Print list of running processes\n");
    printf("Option?:");
}

/*
    Prints out the statistics for the running program
*/
void printStats(struct rusage usage, int totalTime ) {

    // Print out all relevant stats
    printf("\n---Statistics---\nElapsed time: %d milliseconds\n", totalTime);
    printf("Page Faults: %ld\nPage Faults (reclaimed): %ld\n", usage.ru_majflt, usage.ru_minflt);
}

/*
    Creates a child process and executes the inputted command in it
    Returns the total time to execute the child process
*/
int child( char* command, char* argv[] ) {
    // Fork
    int rc = fork();

    // If we encountered some kind of error, return an error message and exit
    if( rc < 0 )
    {
        printf("Error while forkint.\n");
        exit(1);
    }
    
    // If fork returned 0, we are the child, so use execvp to run the process
    if( rc == 0 ) //we are child
    {
        execvp( command, argv ); 
        return 0;
    }

    // We must be the parent, so wait for the child to finish
    else
    {
        // declare start and end timeval
        struct timeval startTime;
        struct timeval endTime;

        // Define the starting time and the wait
        gettimeofday(&startTime, NULL);
        wait(NULL);

        // Define the ending time
        gettimeofday(&endTime, NULL);

        // Calculate the time difference in milliseconds, then return
        int startingTime = ((startTime.tv_usec) / 1000) + ((startTime.tv_sec) * 1000); 
        int endingTime = ((endTime.tv_usec) / 1000) + ((endTime.tv_sec) * 1000); 
        int totalTime = endingTime - startingTime;
        return totalTime;
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

    // Create a child process and record total time
    int totalTime = child( "whoami", argv);

    // Update usage and print out the stats
    getrusage( RUSAGE_SELF, &usage );
    printStats( usage , totalTime);
}

/*
    Function which calls child for the "last" process
*/
void sayLast() {

    // Set argv to the appropriate value
    char* argv[] = {"last", NULL};

    // Initialize the struct to keep track of the stats
    struct rusage usage;

    // Create a child process and record total time
    int totalTime = child( "last", argv );

    // Update usage and print out the stats
    getrusage( RUSAGE_SELF, &usage );
    printStats( usage, totalTime );
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

    // Create a child process and record total time
    int totalTime = child( "ls", argv );

    // Update usage and print out the stats
    getrusage( RUSAGE_SELF, &usage );
    printStats( usage, totalTime );

    // free the memory taken up by path and args
    free(args);
    free(path);
}

/*
    Function that adds a command to the optiont.
*/
node *addCommand( node* userCommands ) {
    
    // Allocate space for the command name and arguments
    char *string = malloc(128 * sizeof(char));
    char **argv = malloc(32 * sizeof(char*)); 

    // Initialize variable to keep track of how many arguments we have
    int argPosition = 0;

    // Get the command and strip off the newline
    printf("Command to add?: ");
    fgets(string, 128, stdin);
    string[(strlen(string))-1] = '\0';

    // Enter the command as the first thing in argv
    char *command = strtok( string, " ");
    argv[argPosition] = command;

    // Looop through the rest of the arguments and add them to argv
    char *argument = strtok( NULL, " ");
    while( argument != '\0' )
    {
        argPosition++;
        argv[argPosition] = argument;
        argument = strtok( NULL, " ");
    }

    // If this is a background operation, set the flag
    if(!strcmp(argv[argPosition], "&"))
    {
        userCommands = addNode(userCommands, argv, 1);
    }
    else
    {
        // Add the command to the linkedList of commands
        userCommands = addNode(userCommands, argv, 0);
    }
    return userCommands;
}

/*
    Function that changes the working directory
*/
void changeDir() {

    // Allocate space for inputted string
    char *newD = malloc( 128 * sizeof(char) );
    printf("New Directory?: ");

    // Get the user's input and strip the newline
    fgets(newD, 128, stdin);
    newD[(strlen(newD))-1] = '\0';

    // Change the directory
    chdir( newD );

    // Free the memory taken by newD
    free( newD );    
    
}

/*
    Function that prints the working directory
*/
void printDir() {

    // Allocate space to store the name of our directory
    char *currentD = malloc( 128 * sizeof(char) );
    printf("Directory: ");

    // Store the name of the current directory in currentD and then print it
    getcwd( currentD, 128 );
    printf("%s\n", currentD );

    // Free the memory used by currentD
    free( currentD );
}

/*
    Function that processes integer input
*/
void operateInt( int command, node *userCommands ) {
    int commandMatch = 0;
    int idNum = 2;
    struct rusage usage;
    int totalTime;

    // Do the appropriate command
    switch(command)
    {
        case 0:
            printf("\n-- Who Am I? --\n");
            sayWho();
            break;
    
        case 1 :
            printf("\n-- Last Logins --\n");
            sayLast();
            break;
        case 2 :
            printf("\n-- Directory Listing --\n");
            list();
            break;

        default:
            // Loop through our user added commands and execute the proper one if it exists   
            for( node *current = userCommands; current != NULL; current = current->next )
            {
                idNum++;

                // If this command's number matches the one the user asked for
                if(command == idNum)
                {

                    // Perform the function, while also gathering and returning data
                    totalTime = child( current->command, current->argv );
                    getrusage( RUSAGE_SELF, &usage );
                    printStats( usage, totalTime );
                    // Set the command matched flag
                    commandMatch = 1;   
                }
            }

            // If the user entered an invalid number, return this error message
            if( !commandMatch )
            {
                printf("\n\nNot sure what that means commander.\n");
            }
    }
}

/*
    Function that takes the operation character and calls the correct function
*/
node *operateChar( char command, node *userCommands ) {
    
    // Do the appropriate command
    switch(command) 
    {
        case 'a' :
            printf("\n-- Add a command --\n");
            userCommands = addCommand( userCommands );
            break;
        case 'c' :
            printf("\n-- Change Directory --\n");
            changeDir();
            break;
        case 'e' :
            printf("\nLogging you out, Commander.\n");
            freeUserCommands( userCommands );        
            exit(0);
            break;
        case 'p' :
            printf("\n-- Current Directory --\n");
            printDir();
            break;
        
        // If the user enterd an invalid character, return an error message
        default:
            printf("\n\nNot sure what that means commander.\n");
    }
    
    return userCommands;
}

/*
    main function of the program
*/
int main( int argc, char** argv) {
    printf("===== Mid-Day Commander, v0 =====");

    // String to hold the user input.
    char commandBuff[128];

    // Integer and character to get what the user typed.
    int commandInt;
    char commandChar;

    // LinkedList to store the commands added by the user.
    node *userCommands = NULL;   

    // LinkedList containing the currently running processes
//    node *runningProcesses = NULL;

    // Loop forever (more or less)
    while(1)
    {

        // Print out the list of commands the user can input.
        printWelcomeMessage( userCommands );

        // If we have not reached the end of a file, then operate.
        if(fgets(commandBuff, 128, stdin) )
        {
        
            // sscanf returns the number of things that it read correctly.
            if( sscanf(commandBuff, "%d", &commandInt) )
            {
                // We were given an int, so parse accordingly.
                operateInt( commandInt, userCommands );
            }
            else
            {
                // We were given a character (or other non-integer), so parse accordingly.
                sscanf( commandBuff, "%c", &commandChar );
                userCommands = operateChar( commandChar, userCommands );
            }
        }

        // If we have reached the end of a file, print the exit message and exit.
        else
        {
            printf("\nLogging you out, Commander.\n");
            freeUserCommands( userCommands );        
            exit(0);
        }
    }	

    return 0;
}

