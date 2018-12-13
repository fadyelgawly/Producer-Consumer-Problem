#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define BUFFER_SIZE 5

typedef int buffer_item;
int *buffer;
int items_in_buffer = 0;
int producer_buffer_location = 0;
int consumer_buffer_location = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_producer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_consumer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

int insert_item(buffer_item item) {
    // insert item into buffer
    pthread_mutex_lock(&mutex_producer);
    
    if (items_in_buffer == BUFFER_SIZE) {
        printf("Producer is waiting for the consumer to consume.\n");
        pthread_cond_wait(&cond_producer, &mutex_producer);
    }
    
    pthread_mutex_unlock(&mutex_producer);
    
    buffer[producer_buffer_location] = item;
    
    pthread_mutex_lock(&mutex);
    items_in_buffer++;
    pthread_mutex_unlock(&mutex);
    
    producer_buffer_location = producer_buffer_location + 1;
    producer_buffer_location = producer_buffer_location % BUFFER_SIZE;
    pthread_cond_signal(&cond_consumer);
    //return 0 if successful, otherwise
    return 0;
    //return -1 indicating an error condition
}
int remove_item(buffer_item *item) {
    // remove an object from buffer
    
    pthread_mutex_lock(&mutex_consumer);
    
    if (items_in_buffer == 0) {
        printf("Consumer is waiting for the producer to produce.\n");
        pthread_cond_wait(&cond_consumer, &mutex_consumer); //Wait for a place in buffer
    }
    pthread_mutex_unlock(&mutex_consumer);
    
    int i = buffer[consumer_buffer_location];
    buffer[consumer_buffer_location] = 0;    //Element produced
    
    *item = i;
    
    pthread_mutex_lock(&mutex);
    items_in_buffer--;           //Adjust elements number in buffer
    pthread_mutex_unlock(&mutex);
    
    consumer_buffer_location = consumer_buffer_location + 1;
    consumer_buffer_location = consumer_buffer_location % BUFFER_SIZE;
    
    pthread_cond_signal(&cond_producer);
    
    //return 0 if successful, otherwise
    return 0;
    //return -1 indicating an error condition */
}




void *producer(void *param) {
    buffer_item item;
    while (1) {
        srand(time(0));
        int sleepTime = ((rand() % 3) + 1);
        /* generate a random number */
        item = (rand() % 10) + 1;
        
        if (insert_item(item)){
            printf("report error condition");
        } else {
            printf("Producer produced %d. \n",item);
            printf("Producer will sleep for %d seconds\n", sleepTime);
            sleep(sleepTime);
            srand(time(0));
            sleepTime = ((rand() % 3) + 1);
        }
    }
}


void *consumer(void *param) {
    buffer_item item;
    while (1) {
        /* sleep for a random period of time */
        srand(time(0));
        int sleepTime = rand() % 3;
        if (remove_item(&item)){
            printf("report error condition");
        } else{
            printf("Consumer consumed %d. \n",item);
            printf("Consumer will sleep for %d seconds\n", sleepTime);
            sleep(sleepTime);
            srand(time(0));
            sleepTime = rand() % 3;
        }
    }
}



int main(int argc, char *argv[]) {
    
    buffer = malloc(BUFFER_SIZE * sizeof(int));
    for (int i = 0; i < BUFFER_SIZE; ++i) //empty the array
        buffer[i] = 0;
    
    
    pthread_t Produce, Consume ;
    
    if (pthread_create(&Produce, NULL, producer , NULL)) {
        printf("Create thread error\n");
        abort();
    }
    
    if (pthread_create(&Consume, NULL, consumer, NULL)) {
        printf("Create thread error\n");
        abort();
    }
    if (pthread_join(Produce, NULL)) {
        printf("Join thread error\n");
        
    }
    if (pthread_join(Consume, NULL)) {
        printf("Join thread error\n");
    }
    
    free(buffer);
    return 0;
}
