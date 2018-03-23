
#include "stdafx.h"


BlockedQueue::BlockedQueue(int size, NTSTATUS &error)
{
  max_size = size;
  // initialize queue, semaphore and mutex
  m_semaphore = CreateSemaphore(NULL, 0, size, TEXT("BlockedQueue Semaphore"));
  if (NULL == m_semaphore)
  {
    printf("%s Error - Failed to create semaphore", __FUNCTION__);
    error = GetLastError();
    return;
  }

  m_mutex = CreateMutex(NULL,FALSE,TEXT("BlockedQueue WriteMutex"));
  if (NULL == m_mutex)
  {
    printf("%s Error - Failed to create mutex", __FUNCTION__);
    error = GetLastError();
    return;
  }
}

BlockedQueue::~BlockedQueue()
{
  if(NULL != m_semaphore)
    CloseHandle(m_semaphore);

  if(NULL != m_mutex)
    CloseHandle(m_mutex);

  while (!m_queue.empty())
  {
    WriteData *write_data = (WriteData*)m_queue.front();
    m_queue.pop();
    delete write_data->buffer;
    delete write_data;
  }
}

NTSTATUS BlockedQueue::Queue(WriteData* write_data)
{
  // Queue write data
  DWORD error;
  while (m_queue.size() == 10)
  {
    Sleep(100);
  }

  error = Lock();
  if (ERROR_SUCCESS != error)
  {
    return error;
  }

  m_queue.push(write_data);

  error = SingalSemaphore();
  if (ERROR_SUCCESS != error)
  {
    return error;
  }

  error = Unlock();
  if (ERROR_SUCCESS != error)
  {
    return error;
  }
  return error;
}

WriteData* BlockedQueue::Dequeue(NTSTATUS &error)
{
  error = WaitOnSemaphore();
  if (WAIT_OBJECT_0 != error)
  {
    return NULL;
  }

  error = Lock();
  if (ERROR_SUCCESS != error)
  {
     return NULL;
  }

  WriteData *write_data = (WriteData*)m_queue.front();

  m_queue.pop();

  error = Unlock();
  if (ERROR_SUCCESS != error)
  {
    return NULL;
  }

  return write_data;
}

NTSTATUS BlockedQueue::Lock()
{
  NTSTATUS error(ERROR_SUCCESS);
  DWORD result = WaitForSingleObject(m_mutex,INFINITE);

  if((WAIT_OBJECT_0 != result) && (WAIT_ABANDONED != result))
  {
    error = GetLastError(); 
    if(WAIT_FAILED == result)
    { 
      printf(" Lock WaitForSingleObject failed %d\n", error);
    }
    else
    {
      printf( "Lock WaitForSingleObject unexpected result %d\n", error);
    }
  }
  return error;
}

NTSTATUS BlockedQueue::Unlock()
{
  NTSTATUS error(ERROR_SUCCESS);
  if(!ReleaseMutex(m_mutex))
  {
    error = GetLastError();
    printf("Unlock ReleaseMutex failed %d\n", error);
  }

  return error;
}

NTSTATUS BlockedQueue::SingalSemaphore()
{
  DWORD error(ERROR_SUCCESS);

  if (!ReleaseSemaphore(m_semaphore, 1, NULL))
  {
    error = GetLastError();
    printf("SingalSemaphore ReleaseSemaphore failed %d\n", error);
  }
  
  return error;
}

NTSTATUS BlockedQueue::WaitOnSemaphore()
{
  DWORD error(ERROR_SUCCESS);
  DWORD result(WaitForSingleObject(m_semaphore,1000));
  
  if(WAIT_OBJECT_0 != result)
  {
    if(WAIT_TIMEOUT == result)
    {
      //timeout is allowed caller handles this case
      //no error setting needed
    }
    else 
    {
      error = GetLastError();
      if(WAIT_FAILED == result)
      {
        printf("WaitOnSemaphore WaitForSingleObject failed %d\n", error);
      }
      else
      {
        printf("WaitOnSemaphore WaitForSingleObject unexpected result %d\n", error);
      }
    }
  }

  return result;
}