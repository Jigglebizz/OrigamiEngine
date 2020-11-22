#pragma once

#include "Mutex.h"

// I think this might be totally unecessary. My thread implementation makes it pretty useless.
// I guess I got a queue implementation out of this...eh.

namespace ThreadPool
{
  //---------------------------------------------------------------------------------
  static constexpr uint32_t kTaskQueueSize    = 32;
  static constexpr uint32_t kMaxThreadCount   = 32;
  static constexpr uint16_t kInvalidThreadIdx = 0xffff;

  //---------------------------------------------------------------------------------
  typedef void(*TaskCallback)();

  //---------------------------------------------------------------------------------
  struct ThreadInfo
  {
    Mutex        m_Mutex;
    bool         m_HasTask;
    TaskCallback m_Task;
    uint16_t     m_Next;

    void Init();
    void Destroy();
  };

  //---------------------------------------------------------------------------------
  void     ENGINE_API Init( uint16_t num_threads );
  void     ENGINE_API Destroy();

  void     ENGINE_API EnqueueTask( TaskCallback function );

  uint16_t ENGINE_API GetNumThreads();
  uint16_t ENGINE_API GetNumFreeThreads();
  uint32_t ENGINE_API GetNumTasksWaiting();
  bool     ENGINE_API AreTasksRunning();
  void     ENGINE_API WaitForTasksToFinish();

  //---------------------------------------------------------------------------------
  uint32_t     s_TaskQueueReadHead;
  uint32_t     s_TaskQueueWriteHead;
  TaskCallback s_TaskQueue[ kTaskQueueSize ];

  uint16_t     s_ThreadCount;
  uint16_t     s_FreeThreadsHead;
  uint16_t     s_UsedThreadsHead;
  ThreadInfo   s_ThreadInfos[ kMaxThreadCount ];

  Mutex        s_PoolMutex;
}