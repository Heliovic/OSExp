/**Thread communication (1b)*/

#define _GNU_SOURCE
#include "sched.h"
#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "semaphore.h"
#include "string.h"
#include "unistd.h"

int producer(int args);
int consumer(int args);
pthread_mutex_t mutex;  // 互斥锁
sem_t product;          // 产品信号量
sem_t warehouse;        // 缓冲区信号量

char buffer[8][4];
int bp = 0;

int main(int argc, char** argv)
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&product, 0, 0);
    sem_init(&warehouse, 0, 8); // 缓冲区大小为 8
    int clone_flag, arg, retval;
    char* stack;
    clone_flag = CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_FILES;
    int i;
    for (i = 0; i < 2; i++)
    {
        // arg = i;
        stack = (char*)malloc(4096);
        retval = clone((void*)producer, &(stack[4095]), clone_flag, i);
        stack = (char*)malloc(4096);
        retval = clone((void*)consumer, &(stack[4095]), clone_flag, i);
        sleep(1);
    }
    sleep(60);
    exit(1);
}

int producer(int args)
{
    int id = args;
    int i;
    for (i = 0; i < 10; i++)
    {
        sleep(i + 1);
        sem_wait(&warehouse);       // 若满，等待消费者取走物品
        pthread_mutex_lock(&mutex); // 互斥操作
        if (id == 0)
            strcpy(buffer[bp], "aaa\0");
        else
            strcpy(buffer[bp], "bbb\0");
        bp++;
        printf("producer %d produce %s in %d\n", id, buffer[bp - 1], bp - 1);
        pthread_mutex_unlock(&mutex);
        sem_post(&product);         // 唤醒消费者（若有）
    }
    printf("producer %d is over!\n", id);
}

int consumer(int args)
{
    int id = args;
    int i;
    for (i = 0; i < 10; i++)
    {
        sleep(10 - i);
        sem_wait(&product);       // 若满，等待消费者取走物品
        pthread_mutex_lock(&mutex); // 互斥操作
        bp--;
        printf("consumer %d get %s in %d\n", id, buffer[bp], bp);
        strcpy(buffer[bp], "zzz\0");
        pthread_mutex_unlock(&mutex);
        sem_post(&warehouse);         // 唤醒消费者（若有）
    }
    printf("consumer %d is over!\n", id);
}
