#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <cstdint>
#include <assert.h>

typedef unsigned long ThreadId;
#define MUTEX_DEFAULT_SPIN_COUNT 1500

struct Mutex
{
	pthread_mutex_t mutex;
	uint32_t spinCount;
};

//returns -1 if error, 0 otherwise
int CreateMutex(Mutex* pMutex);

void DestroyMutex(Mutex* pMutex);
void LockMutex(Mutex* pMutex);
void UnlockMutex(Mutex* pMutex);

//returns -1 if error, 0 otherwise
int TryLockMutex(Mutex* pMutex);

struct ConditionVariable
{
	pthread_cond_t conditionVar;
};

//returns -1 if error, 0 otherwise
int CreateConditionVariable(ConditionVariable* pCv);

void DestroyConditionVariable(ConditionVariable* pCv);
void WaitConditionVariable(ConditionVariable* pCv, Mutex* pMutex);
void SignalOneConditionVariable(ConditionVariable* pCv);
void SignalAllConditionVariable(ConditionVariable* pCv);

typedef void (*ThreadFunc)(void*);

struct ThreadInfo
{
	ThreadFunc pFunc;
	void* pData;
};

struct Thread
{
	pthread_t thread;
};

void InitThreadSystem();
void ExitThreadSystem();
void ExitThread();

//returns -1 if error, 0 otherwise
int CreateThread(ThreadInfo* pInfo, Thread* pThread);

void JoinThread(Thread* pThread);
void DeatchThread(Thread* pThread);
ThreadId GetCurrentThreadId();

#endif