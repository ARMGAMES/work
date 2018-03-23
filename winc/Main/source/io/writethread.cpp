
#include "stdafx.h"

DWORD WINAPI WriteThreadFuncWrapper( LPVOID lpvParam )
{
  //Status_Assert(lpvParam != NULL);
  WriteThread *write_thread = (WriteThread *)lpvParam;
 // write_thread->WriteThreadFunc();
  return 0;
}

void WriteThread::WriteThreadFunc()
{
  NTSTATUS m_status(ERROR_SUCCESS);
  while (1)
  {
    if (m_writethreadfinished)
      break;

    WriteData* writedata = m_blockqueue.Dequeue(m_status);

    if (m_status != WAIT_OBJECT_0)
    {
      if (m_status == WAIT_TIMEOUT)
      {
        continue;
      }
      else
      {
        printf ("Failed on semaphore %s\n", __FUNCTION__);
        break;
      }
    }

    // process write data
    printf ("[WRITE FILE] --- %s\n", writedata->buffer);

    // delete buffer 
    delete writedata->buffer;
    delete writedata;

  }

}

NTSTATUS WriteThread::AddWriteData(WriteData *writedata)
{
  WriteData *write_data_ = new WriteData();
  write_data_->buffer = malloc(writedata->length);
  write_data_->length = writedata->length;
  memcpy(write_data_->buffer, writedata->buffer, writedata->length);
  write_data_->type = writedata->type;

  return m_blockqueue.Queue(write_data_);
}

void WriteThread::TestWriteData()
{
  NTSTATUS m_status(ERROR_SUCCESS);

  WriteData *writedata = m_blockqueue.Dequeue(m_status);

  printf ("[WRITE FILE] --- %s\n", writedata->buffer);

  delete writedata->buffer;
  delete writedata;

}

WriteThread::WriteThread(NTSTATUS &error):m_blockqueue(10, error)
{
  m_status = ERROR_SUCCESS;
  m_writethreadfinished = false;

  m_writethread = CreateThread(NULL, 0, WriteThreadFuncWrapper, (void*)this, 0, NULL);
  if(m_writethread == NULL)
  {
    error = GetLastError();
    printf("Failed to create write thread. %s", __FUNCTION__);
  }

}

WriteThread::~WriteThread()
{
  DWORD error(ERROR_SUCCESS);

  if(m_writethread == NULL)
    return;
  if(m_writethread == INVALID_HANDLE_VALUE)
    return;

  m_writethreadfinished = true;
  error = WaitForSingleObject(m_writethread, 1000*5);
  if (error != WAIT_OBJECT_0)
  {
    printf("Failed to close thread. %s", __FUNCTION__);
  }

  error = CloseHandle(m_writethread);
}