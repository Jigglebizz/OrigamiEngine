#include "Origami/pch.h"
#include "Origami/Concurrency/ThreadPool.h"

#include "Origami/Concurrency/Thread.h"

//---------------------------------------------------------------------------------
void ThreadPool::ThreadInfo::Init()
{
  m_Mutex.Init();
  m_HasTask = false;
  m_Task    = nullptr;
  m_Next    = 0xffff;
}

//---------------------------------------------------------------------------------
void ThreadPool::ThreadInfo::Destroy()
{
  m_Mutex.Destroy();
}

//---------------------------------------------------------------------------------
void ThreadPool::Init( uint16_t num_threads )
{
  ASSERT_MSG(num_threads < kMaxThreadCount, "Requested more threads than supported" );

  s_PoolMutex.Init();

  s_ThreadCount = num_threads;
  for ( uint16_t i_thread = 0; i_thread < num_threads; ++i_thread  )
  {
    s_ThreadInfos[ i_thread ].Init();
    if ( i_thread != num_threads - 1 )
    {
      s_ThreadInfos[ i_thread ].m_Next = i_thread + 1;
    }
  }

  MemZero( s_TaskQueue, sizeof( s_TaskQueue) );
  s_TaskQueueReadHead  = 0;
  s_TaskQueueWriteHead = 0;

  s_FreeThreadsHead = 0;
  s_UsedThreadsHead = kInvalidThreadIdx;
}

//---------------------------------------------------------------------------------
void ThreadPool::Destroy()
{
  WaitForTasksToFinish();

  for ( uint16_t i_thread = 0; i_thread < s_ThreadCount; ++i_thread )
  {
    s_ThreadInfos[ i_thread ].Destroy();
  }

  s_PoolMutex.Destroy();
}

//---------------------------------------------------------------------------------
void ThreadPool::EnqueueTask( TaskCallback task )
{
  ScopedLock lock( &s_PoolMutex );
  if ( s_FreeThreadsHead != kInvalidThreadIdx  )
  {
    uint16_t    thread_idx = s_FreeThreadsHead;
    ThreadInfo* thread = &s_ThreadInfos[ thread_idx ];
    {
      ScopedLock thread_lock( &thread->m_Mutex );
      thread->m_Task    = task;
      thread->m_HasTask = true;
      s_FreeThreadsHead = thread->m_Next;
      thread->m_Next    = s_UsedThreadsHead;
    }
    s_UsedThreadsHead = thread_idx;
  }
  else
  {
   TaskCallback* q_task = &s_TaskQueue[ s_TaskQueueWriteHead++ ];
   if ( s_TaskQueueWriteHead == kTaskQueueSize )
   {
     s_TaskQueueWriteHead = 0;
   }

   ASSERT_MSG( s_TaskQueueWriteHead != s_TaskQueueReadHead, "Task queue is full " );
   
   *q_task = task;
  }
}

//---------------------------------------------------------------------------------
uint16_t ThreadPool::GetNumThreads()
{
  return s_ThreadCount;
}

//---------------------------------------------------------------------------------
uint16_t ThreadPool::GetNumFreeThreads()
{
  if ( s_FreeThreadsHead == kInvalidThreadIdx )
  {
    return 0;
  }

  uint16_t free_count = 0;
  uint16_t thread_idx = s_FreeThreadsHead;
  while ( thread_idx != kInvalidThreadIdx )
  {
    thread_idx = s_ThreadInfos[ thread_idx ].m_Next;
    free_count++;
  }

  return free_count;
}

//---------------------------------------------------------------------------------
uint32_t ThreadPool::GetNumTasksWaiting()
{
  if ( s_TaskQueueReadHead >= s_TaskQueueWriteHead )
  {
    return s_TaskQueueReadHead - s_TaskQueueWriteHead;
  }

  return kTaskQueueSize - ( s_TaskQueueWriteHead - s_TaskQueueReadHead );
}

//---------------------------------------------------------------------------------
bool ThreadPool::AreTasksRunning()
{
  return s_UsedThreadsHead != kInvalidThreadIdx;
}

//---------------------------------------------------------------------------------
void ThreadPool::WaitForTasksToFinish()
{
  while ( s_UsedThreadsHead != kInvalidThreadIdx )
  {
    Sleep(1);
  }
}

//---------------------------------------------------------------------------------
void ThreadPoolFunction( Thread* thread )
{
  thread->
}