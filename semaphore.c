#define _XOPEN_SOURCE 700

#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <unistd.h>
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <string.h>
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <signal.h>

#define RED	printf("\033[1;31m")
#define BLUE printf("\033[1;36m")
#define MAGENTA	printf("\033[1;35m")
#define YELLOW printf("\033[1;33m")
#define GREEN printf("\033[1;32m")
#define DEFAULT	printf("\033[0m")

#define TEST(real,expected) (real == expected ? printf("%d, Pass\n", expected) : printf("%d, Fail\n",expected))

#define SEMCOUNT (3)
#define MAX_IN (20)

void Parser(char *command, char **letter, int *num, int *undo);
static void Decrement(int num);
static void Increment(int num);
static void Undo();
static void SemInit(char argv[1]);
static void Count(int undo, char *letter, int num);
static void InterruptHandle(int signumber);

int count_g;
sem_t *sem_g = NULL;

int main(int argc, char **argv)
{
    struct sigaction signal;
    sigset_t mask;
    int undo = 0;
    int num = 0;
    char *command = NULL;
    char *letter = NULL;
    char *letter_tofree = NULL;
    char *command_tofree = NULL;

    letter = (char*)malloc(100);
    if (NULL == letter)
    {
        fprintf(stderr, "allocation failed\n");
        exit (1);
    }
    command = (char*)malloc(100);
    if (NULL == command)
    {
        fprintf(stderr, "allocation failed\n");
        exit (1);
    }

    signal.sa_handler = &InterruptHandle;
    sigfillset(&signal.sa_mask);

    letter_tofree = letter;
    command_tofree = command;

    if (sigaction(SIGINT, &signal, NULL) == -1)
    {
        printf("Failed to set SIGINT handler");
    }
    if (sigaction(SIGTERM, &signal, NULL) == -1)
    {
        printf("Failed to set SIGINT handler");
    }  
    if (sigaction(SIGHUP, &signal, NULL) == -1)
    {
        printf("Failed to set SIGINT handler");
    }

    /* initialize semaphores for shared processes */
    SemInit(argv[1]);

    while(1)
	{
        GREEN;
        printf("Please enter a letter:\n");
        DEFAULT;
        MAGENTA;
        printf("\n D-decrement\n U-increments\n X-exits");
        DEFAULT;
        GREEN;
        printf("\nAfter please enter a number and with/without undo\n");
        DEFAULT;

		fgets(command, MAX_IN, stdin);
        *strchr(command, '\n') = '\0';

        Parser(command, &letter, &num, &undo);

        YELLOW;
        printf("letter chosen is: %s, number is %d\n", letter, num);
        if(undo == 1)
        {
            printf("undo is chosen\n");
        }
        DEFAULT;

        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, NULL);

        switch (letter[0])
        {
            case 'D': Decrement(num);
                break;
            case 'U': Increment(num);
                break;    
            case 'X':
                    {
                        free(letter_tofree);
                        free(command_tofree);
                        Undo();
                    }
                break;
            RED;
            default: printf("please type 'D' 'U' 'X'\n");
            DEFAULT;
                break;
        }
        
        Count(undo, letter, num);

        sigemptyset(&mask);
        sigprocmask(SIG_SETMASK, &mask, NULL);
    }
    
    return 0;
}



static void SemInit(char argv[1])
{
    int sval = 0;

    if(argv != NULL)
    {
        sem_g = sem_open(argv, O_CREAT , 0666, SEMCOUNT);
        if(sem_g == SEM_FAILED)
        {
            perror("Failed to create semaphore\n");
        }
    }
    else
    {
        RED;
        printf("please enter a name for semaphore\n");
        DEFAULT;
    }
    
    atexit(&Undo);

    sem_getvalue(sem_g, &sval);
    BLUE;
    printf("value of semaphore is %d\n",sval);
    DEFAULT;
}



static void Count(int undo, char *letter, int num)
{
    if(undo == 1)
    {
        if(strcmp("D", letter) == 0)
        {
            count_g += num;
        }
        else if(strcmp("U", letter) == 0)
        {
            count_g -= num;
        }
    }
}



static void Undo()
{
    if(count_g > 0)
    {
        Increment(count_g);
    }
    else if(count_g < 0)
    {
        count_g = -count_g;
        Decrement(count_g);
    }

    count_g = 0;

    exit(1);
}




static void InterruptHandle(int signumber)
{
	(void)signumber;
	
    RED;
	printf("Hi from interrupt\n");
    DEFAULT;

	Undo();

}



void Parser(char *command, char **letter, int *num, int *undo)
{
    char *num_char = NULL;
    char *undo_char = NULL;

    
    *letter = strtok(command, " ");
    if(letter == NULL)
    {
        return;
    }

    num_char = strtok(NULL, " ");
    if(num_char == NULL)
    {
        return;
    }

    undo_char = strtok(NULL, " ");
    if((undo_char != NULL) && (strcmp(undo_char, "[undo]") == 0))
    {
        *undo = 1;
    }
    else
    {
        *undo = 0;
    }   
    
   *num = atoi(num_char);
}



static void Decrement(int num)
{
    size_t i = 0;

    for(i = 0; i < num; i++)
    {
        sem_wait(sem_g);
    }
}


static void Increment(int num)
{
    size_t i = 0;
    
    for(i = 0; i < num; i++)
    {
        sem_post(sem_g);
    }
}