#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define HUNDRED_K (100000)
#define ULIMIT (30799) /*ulimit -u */
#define MAXTHREADS (1) /*ulimit -u */
#define BILLION (1000000000) /*ulimit -u */

#define TEST(real,expected) (real == expected ? printf("%d, Pass\n", expected) : printf("%d, Fail\n",expected))

#define RED	printf("\033[1;31m")
#define BLUE printf("\033[1;36m")
#define MAGENTA	printf("\033[1;35m")
#define YELLOW printf("\033[1;33m")
#define GREEN printf("\033[1;32m")
#define DEFAULT	printf("\033[0m")

static void EXS1();
static void EXS2();
static void EXS3();
static void EXS4();
static void EXS5part1();
static void EXS5part2();
static void EXS6();

void *threadFunction(void *arg);
void *threadFunction2(void *param);

int threads_arr_g[HUNDRED_K];

int main()
{
    /* EXS1(); */
   /*  EXS2(); */
   /*  EXS3(); */
   /*  EXS4(); */
   /*   EXS5part1(); */
    /* EXS5part2(); */
    EXS6();

    return 0;
}

typedef struct param
{
    size_t num;
    size_t start_index;
    size_t end_index;
}threads_t;


static void EXS1()
{
    size_t i = 0;
    int status = 0;
    pthread_t thread[HUNDRED_K];
    time_t start_time;
    time_t end_time;
    time_t total_time;
    
    start_time = time(NULL);

    for(i=0; i<HUNDRED_K; i++)
    {
        pthread_create(&thread[i], NULL, threadFunction, (void *)i);
       /*  pthread_join(thread, NULL); */
    }

    end_time = time(NULL);

    total_time = end_time - start_time;

    printf("the total time is %ld \n", total_time);

    sleep(10);

    for (i = 0; i < HUNDRED_K; i++)
    {
        status = 0;
        if(threads_arr_g[i] != i)
        {
            printf("array != index when i is %lu\n", i);
            status = 1;
            break;
        }
    }

    status == 0 ? printf("\033[1;32m \n array value = index \n\033[0m") :
     printf("\033[1;31m \n Each array value doesn't have an element equal to it's index \n\033[0m");
}


void *threadFunction(void *arg)
{
    threads_arr_g[(size_t)arg] = (int)arg;
    return arg;
}



static void EXS2()
{
    size_t i = 0;
    int status = 0;
    pthread_t thread[HUNDRED_K];
    time_t start_time;
    time_t end_time;
    time_t total_time;
    int ret_val = 0;
    
    start_time = time(NULL);

    for(i=0; i<HUNDRED_K; i++)
    {
        ret_val = pthread_create(&thread[i], NULL, threadFunction, (void *)i);
        if (0 != ret_val)
        {
            RED;
            printf("failed to create at index %lu, ret_val is %d\n", i, ret_val);
            DEFAULT;
            while(ret_val != 0)
            {
                pthread_create(&thread[i], NULL, threadFunction, (void *)i);
            }
        }
       /*  pthread_join(thread, NULL); */
    }

    end_time = time(NULL);

    total_time = end_time - start_time;

    printf("the total time is %ld \n", total_time);

    for (i = 0; i < HUNDRED_K; i++)
    {
        status = 0;
        if(threads_arr_g[i] != i)
        {
            printf("array != index when i is %lu\n", i);
            status = 1;
            break;
        }
    }

    status == 0 ? printf("\033[1;32m \n array value = index \n\033[0m") :
     printf("\033[1;31m \n Each array value doesn't have an element equal to it's index \n\033[0m");

}




static void EXS3()
{
    size_t i = 0;
    int status = 0;
    pthread_t thread[HUNDRED_K];
    time_t start_time;
    time_t end_time;
    time_t total_time;
    int ret_val = 0;
    
    start_time = time(NULL);

    for(i=0; i<HUNDRED_K; i++)
    {
        pthread_create(&thread[i], NULL, threadFunction, (void *)i);
        ret_val = pthread_detach(thread[i]);
        if (0 != ret_val)
        {
            RED;
            printf("failed to detach at index %lu, ret_val is %d\n", i, ret_val);
            DEFAULT;
            while(ret_val != 0)
            {
                pthread_create(&thread[i], NULL, threadFunction, (void *)i);
            }
        }
    }

    end_time = time(NULL);

    total_time = end_time - start_time;

    printf("the total time is %ld \n", total_time);

    for (i = 0; i < HUNDRED_K; i++)
    {
        status = 0;
        if(threads_arr_g[i] != i)
        {
            printf("array != index when i is %lu\n", i);
            status = 1;
            break;
        }
    }

    status == 0 ? printf("\033[1;32m \n array value = index \n\033[0m") : printf("\033[1;31m \n Each array value doesn't have an element equal to it's index \n\033[0m");

}



static void EXS4()
{
    size_t i = 0;
    pthread_t thread[ULIMIT];
    time_t start_time;
    time_t end_time;
    time_t total_time;
    int ret_val = 0;
    threads_t *param = NULL;
    size_t sum_of_div = 0;
    
    start_time = time(NULL);

    param = (threads_t *)malloc(sizeof(threads_t)*ULIMIT);

    for(i = 0; i < ULIMIT; i++)
    {
        (param + i)->start_index = 1 +i*100000;
        (param + i)->end_index = (i + 1)*100000;
        (param + i)->num = (size_t)ULIMIT*100000;
        ret_val = pthread_create(&thread[i], NULL, threadFunction2, &(param[i]));
        if (0 != ret_val)
        {
            RED;
            printf("failed to detach at index %lu, ret_val is %d\n", i, ret_val);
            DEFAULT;
        }
    }

    for(i = 0; i < ULIMIT; i++)
    {
        void *retval;

        pthread_join(thread[i], &retval);
        sum_of_div += (size_t)retval;
    } 
    printf("sum_of_div %lu\n", sum_of_div);



    end_time = time(NULL);

    total_time = end_time - start_time;

    printf("the total time is %lu \n", total_time);
 
    free(param);
}


void *threadFunction2(void *param)
{
    size_t sum_of_divisiors = 0;
    size_t i = 0;

    for(i = (((threads_t *)param)->start_index); i <= ((threads_t*)param)->end_index; i++)
    {
        if( 0 == ((threads_t*)param)->num % i)
        {
            sum_of_divisiors += i;
        } 
    }
   return (void *)sum_of_divisiors;
}




static void EXS5part1()
{
    size_t i = 0;
    size_t sum_of_divisiors = 0;
    

    for(i = 1; i <= (size_t)ULIMIT*100000; i++)
    {
        if( 0 == (size_t)ULIMIT*100000 % i)
        {
            sum_of_divisiors += i;
        } 
    }

    printf("sum_of_div %lu\n", sum_of_divisiors);
}


static void EXS5part2()
{
    size_t i = 0;
    pthread_t thread[MAXTHREADS];
    time_t start_time;
    time_t end_time;
    time_t total_time;
    size_t ret_val = 0;
    threads_t *param = NULL;
    size_t sum_of_div = 0;
    
    start_time = time(NULL);


    param = (threads_t *)malloc(sizeof(threads_t)*MAXTHREADS);

    for(i = 0; i < MAXTHREADS; i++)
    {
        (param + i)->start_index = 1 +i*BILLION/MAXTHREADS;
        (param + i)->end_index = (i + 1)*BILLION/MAXTHREADS;
        (param + i)->num = BILLION;
        ret_val = pthread_create(&thread[i], NULL, threadFunction2, &(param[i]));
        if (0 != ret_val)
        {
            RED;
            printf("failed to detach at index %lu, ret_val is %lu\n", i, ret_val);
            DEFAULT;
        }
    }

    for(i = 0; i < MAXTHREADS; i++)
    {
        void *retval;

        pthread_join(thread[i], &retval);
        sum_of_div += (size_t)retval;
    } 
    printf("sum_of_div %lu\n", sum_of_div);



    end_time = time(NULL);

    total_time = end_time - start_time;

    printf("the total time is %ld \n", total_time);
 
    free(param);
}







static void EXS6()
{
    size_t i = 0;
    size_t sum_of_divisiors = 0;

    #pragma omp parallel for

    for(i = 1; i <= (size_t)ULIMIT*100000; i++)
    {
        if( 0 == (size_t)ULIMIT*100000 % i)
        {
            sum_of_divisiors += i;
        } 
    }

    printf("sum_of_div %lu\n", sum_of_divisiors);

}
