#include "Threading.h"
#include <stdlib.h>
#include <pthread.h>

struct Mutex
{
    pthread_mutex_t mutex;
};

int mutex_create(Mutex** mutex){
    *mutex = malloc(sizeof(Mutex));
    if(*mutex == NULL){
        return -1;
    }

    int code = pthread_mutex_init(&((*mutex)->mutex), NULL);
    if(code != 0){
        free(*mutex);
    }
    
    return code;
}

int mutex_lock(Mutex* mutex){
    return pthread_mutex_lock(&(mutex->mutex));
}

int mutex_unlock(Mutex* mutex){
    return pthread_mutex_unlock(&(mutex->mutex));
}

int mutex_destroy(Mutex* mutex){
    int code = pthread_mutex_destroy(&(mutex->mutex));
    if(code == 0){
        free(mutex);
    }

    return code;
}

struct Cond
{
    pthread_cond_t cond;
};


int cond_create(Cond** cond){
    *cond = malloc(sizeof(Cond));
    if(*cond == NULL){
        return -1;
    }

    int code = pthread_cond_init((&(*cond)->cond), NULL);
    if(code != 0){
        free(*cond);
    }

    return code;
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
    int code = pthread_cond_destroy(&(cond->cond));
    if(code == 0){
        free(cond);
    }

    return code;
}

struct Thread
{
    pthread_t thread;
};

int thread_create(Thread** thread, void *(*func)(void *)){
    *thread = malloc(sizeof(Thread));
    if(*thread == NULL){
        return -1;
    }
    
    int code = pthread_create(&((*thread)->thread), NULL, func, NULL);
    if(code != 0){
        free(*thread);
    }

    return code;
}

int thread_join(Thread* thread){
    int code = pthread_join(thread->thread, NULL);
    if(code == 0){
        free(thread);
    }
    
    return code;
}