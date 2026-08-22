#define _POSIX_C_SOURCE 200809L

#include "Threading.h"
#include <stdlib.h>
#include <pthread.h>

struct Mutex
{
    pthread_mutex_t mutex;
};

Mutex* mutex_create(void){
    Mutex* mutex = malloc(sizeof(Mutex));
    if(mutex == NULL){
        return NULL;
    }

    mutex->mutex = PTHREAD_MUTEX_INITIALIZER;
    return mutex;
}

int mutex_lock(Mutex* mutex){
    return pthread_mutex_lock(&(mutex->mutex));
}

int mutex_unlock(Mutex* mutex){
    return pthread_mutex_unlock(&(mutex->mutex));
}

int mutex_destroy(Mutex* mutex){
    return pthread_mutex_destroy(&(mutex->mutex));
}

struct Cond
{
    pthread_cond_t cond;
};


Cond* cond_create(void){
    Cond* cond = malloc(sizeof(Cond));
    if(cond == NULL){
        return NULL;
    }

    cond->cond = PTHREAD_COND_INITIALIZER;
    return cond;
}

int cond_signal(Cond* cond){
    return pthread_cond_signal(&(cond->cond));
}

int cond_wait(Cond* cond, Mutex* mutex){
    return pthread_cond_wait(&(cond->cond), &(mutex->mutex));
}

int cond_broadcast(Cond* cond){
    return pthread_cond_broadcast(&(cond->cond));
}

int cond_destroy(Cond* cond){
    return pthread_cond_destroy(&(cond->cond));
}

struct Thread
{
    pthread_t thread;
};

int thread_create(Thread** thread, void *(*func)(void *)){
    *thread = malloc(sizeof(Thread));
    if(thread == NULL){
        return -1;
    }

    return pthread_create(&((*thread)->thread), NULL, func, NULL);
}

int thread_join(Thread* thread){
    return pthread_join(thread->thread, NULL);
}