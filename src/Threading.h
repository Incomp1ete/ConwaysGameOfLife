#ifndef THREADING_H
#define THREADING_H

typedef struct Mutex Mutex;
int mutex_create(Mutex** mutex);
int mutex_lock(Mutex* mutex);
int mutex_unlock(Mutex* mutex);
int mutex_destroy(Mutex* mutex);

typedef struct Cond Cond;
int cond_create(Cond** cond);
int cond_signal(Cond* cond);
int cond_wait(Cond* cond, Mutex* mutex);
int cond_broadcast(Cond* cond);
int cond_destroy(Cond* cond);

typedef struct Thread Thread;
int thread_create(Thread** thread, void *(*func)(void *));
int thread_join(Thread* thread);

#endif