#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
#include<stdio.h>
typedef int buffer_item;
#define BUFFER_SIZE 10

buffer_item buffer[10];
int insertnum,removenum,all;
pthread_mutex_t mutex;
sem_t mo1,mo2;
sem_t empty;
sem_t full;
int cnt;
void *monitoring(void *para)
{
    while(1)
    {
        sem_wait(&mo2);
        printf("Acknowledge no : %d -> count==:%d\n",++all,cnt);
        sem_post(&mo1);
    }
}

int insert_item(buffer_item item)
{
    
    if(sem_wait(&empty)!=0)
        return -1;
    
    pthread_mutex_lock(&mutex);
    
    buffer[insertnum]=item;
    insertnum=(insertnum+1)%BUFFER_SIZE;
    
    printf("insert : %d\n",++cnt);
    
    sem_post(&mo2);
    sem_wait(&mo1);
    
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    
    
    
    return 0;
}
int remove_item(buffer_item *item)
{
    
    if(sem_wait(&full)!=0)
        return -1;
    
    pthread_mutex_lock(&mutex);
    
    
    *item=buffer[removenum];
    buffer[removenum]=0;
    removenum=(removenum+1)%BUFFER_SIZE;
    printf("remove : %d\n",--cnt);
    
    sem_post(&mo2);
    sem_wait(&mo1);
    
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    
    
}

void *producer(void *para)
{
    buffer_item item;
    while(1)
    {
        sleep(rand()%5);
        item=rand();
        if(insert_item(item))
            printf("report error condition");
        else
            printf("producer produced %d\n", item);
        
    }
}

void *consumer(void *para)
{
    buffer_item item;
    while(1)
    {
        sleep(rand()%3);
        if(remove_item(&item))
            printf("report error condition");
        else
            printf("consumer consumed %d\n", item);
    }
}

int main(int argc,char *argv[])
{
    int produce_num=atoi(argv[2]);
    int consume_num=atoi(argv[3]);
    int i=0;
    
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex,NULL);
    sem_init(&mo1, 0, 0);
    sem_init(&mo2, 0, 0);
    
    pthread_t producer_thread;
    pthread_t consumer_thread;
    pthread_t monitoring_thread;
    
    
    for(i=0;i<produce_num;i++)
        pthread_create(&producer_thread,NULL,producer,NULL);
    
    pthread_create(&monitoring_thread,NULL,monitoring,NULL);
    
    for(i=0;i<consume_num;i++)
        pthread_create(&consumer_thread,NULL,consumer,NULL);
    
    
    
    sleep(atoi(argv[1]));
    return 0;
}
