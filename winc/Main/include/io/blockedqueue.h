
#ifndef BLOCKQUEUE_H_
#define BLOCKQUEUE_H_

#include "stdafx.h"
#include <queue>


enum IOTYPE
{
  IOTYPE_WRITE = 0,
  IOTYPE_COMPLETE,
};

typedef struct _write_request
{
  void* buffer;
  DWORD length;
  IOTYPE type;
}WriteData;

class BlockedQueue
{
public:
  BlockedQueue(int size, NTSTATUS &error);
  ~BlockedQueue();

  NTSTATUS Queue(WriteData* write_data);
  WriteData* Dequeue(NTSTATUS &error);

protected:
private:
  NTSTATUS Lock();
  NTSTATUS Unlock();

  NTSTATUS SingalSemaphore();
  NTSTATUS WaitOnSemaphore();

  HANDLE m_semaphore;
  HANDLE m_mutex;
  std::queue<WriteData*> m_queue;
  int max_size;
};

#endif