#include "Thread.h"
#include <stdlib.h>
#include <stdio.h>

int CreateMutex(Mutex* pMutex)
{
	pMutex->spinCount = MUTEX_DEFAULT_SPIN_COUNT;
	pMutex->mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutexattr_t attr;
	int result = pthread_mutexattr_init(&attr);
	result |= pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	result |= pthread_mutex_init(&pMutex->mutex, &attr);
	result |= pthread_mutexattr_destroy(&attr);
	
	return (result == 0) ? 0 : -1;
}

void DestroyMutex(Mutex* pMutex)
{
	pthread_mutex_destroy(&pMutex->mutex);
}

void LockMutex(Mutex* pMutex)
{
	//when trying to lock a mutex that is already locked, the thread will spin count times,
	//checking to see if the lock is released. If it isn't released before the loop finishes, then the thread
	//will go to sleep to wait for the lock to be released.
	uint32_t count = 0;
	while(count < pMutex->spinCount && pthread_mutex_trylock(&pMutex->mutex) != 0)
	{
		++count;
	}
	
	if(count == pMutex->spinCount)
	{
		int result = pthread_mutex_lock(&pMutex->mutex);
		assert(result == 0 && "LockMutex: Failed to lock the mutex"); //create your own assert
	}
}

void UnlockMutex(Mutex* pMutex)
{
	pthread_mutex_unlock(&pMutex->mutex);
}

int TryLockMutex(Mutex* pMutex)
{
	int result = pthread_mutex_trylock(&pMutex->mutex);
	return (result == 0) ? 0 : -1;
}

int CreateConditionVariable(ConditionVariable* pCv)
{
	pCv->conditionVar = PTHREAD_COND_INITIALIZER;
	int result = pthread_cond_init(&pCv->conditionVar, nullptr);
	return (result == 0) ? 0 : -1;
}

void DestroyConditionVariable(ConditionVariable* pCv)
{
	pthread_cond_destroy(&pCv->conditionVar);
}

void WaitConditionVariable(ConditionVariable* pCv, Mutex* pMutex)
{
	pthread_cond_wait(&pCv->conditionVar, &pMutex->mutex);
}

void SignalOneConditionVariable(ConditionVariable* pCv)
{
	pthread_cond_signal(&pCv->conditionVar);
}

void SignalAllConditionVariable(ConditionVariable* pCv)
{
	pthread_cond_broadcast(&pCv->conditionVar);
}

Thread gJoinThread;
bool gStoreId;
ThreadId gThreadToJoin;
Mutex gJoinThreadMutex;
ConditionVariable gJoinThreadCond;

//Function that joins threads
void ThreadJoin(void* ptr)
{
	while(true)
	{
		LockMutex(&gJoinThreadMutex);
		while(gStoreId == true)
		{
			WaitConditionVariable(&gJoinThreadCond, &gJoinThreadMutex);
		}
		
		printf("Joining thread\n");
		int joinError = pthread_join(gThreadToJoin, nullptr);
		if(joinError != 0)
		{
			perror("pthread_join");
		}
		printf("Thread joined\n");
		
		gStoreId = true;
		UnlockMutex(&gJoinThreadMutex);
	}
}

//Must call at the end of thread functions if InitThreadSystem() is called.
//Only call after InitThreadSystem();
void ExitThread()
{
	LockMutex(&gJoinThreadMutex);
	while(gStoreId == false)
	{
		WaitConditionVariable(&gJoinThreadCond, &gJoinThreadMutex);
	}
	
	gThreadToJoin = GetCurrentThreadId();
	gStoreId = false;
	SignalOneConditionVariable(&gJoinThreadCond);
	UnlockMutex(&gJoinThreadMutex);
}

//Call if you want to handle joining of threads without making the main thread wait.
void InitThreadSystem()
{
	gStoreId = true;
	int result = CreateMutex(&gJoinThreadMutex);
	if(result != 0)
	{
		printf("Error creating Join Thread Mutex. Exiting program!\n");
		exit(1);
	}
	
	result = CreateConditionVariable(&gJoinThreadCond);
	if(result != 0)
	{
		printf("Error creating Join Thread Condition Variable. Exiting program!\n");
		exit(1);
	}
	
	ThreadInfo info{};
	info.pFunc = ThreadJoin;
	result = CreateThread(&info, &gJoinThread);
	if(result != 0)
	{
		printf("Error creating Join Thread. Exiting program!\n");
		exit(1);
	}
	DeatchThread(&gJoinThread);
}

void ExitThreadSystem()
{
	DestroyMutex(&gJoinThreadMutex);
	DestroyConditionVariable(&gJoinThreadCond);
}

static void* ThreadFunction(void* data)
{
	ThreadInfo info = *(ThreadInfo*)data;
	free(data);
	
	info.pFunc(info.pData);
	
	return nullptr;
}

int CreateThread(ThreadInfo* pInfo, Thread* pThread)
{
	//Copy the contents of thread info so avoid accessing corrupted data if the variable is on the stack
	ThreadInfo* pCopy = (ThreadInfo* )calloc(1, sizeof(ThreadInfo));
	*pCopy = *pInfo;
  
	int pthread_error = pthread_create(&pThread->thread, nullptr, ThreadFunction, pCopy);
	
	if(pthread_error != 0)
		free(pCopy);
		
	return (pthread_error == 0) ? 0 : -1;
}

void JoinThread(Thread* pThread)
{
	pthread_join(pThread->thread, nullptr);
	pThread = nullptr;
}

void DeatchThread(Thread* pThread)
{
	pthread_detach(pThread->thread);
}

ThreadId GetCurrentThreadId()
{
	return pthread_self();
}