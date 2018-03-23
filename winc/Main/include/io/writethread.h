
#ifndef WRITE_THREAD_H_
#define WRITE_THREAD_H_

#include "stdafx.h"

class WriteThread
{
public:
  WriteThread(NTSTATUS &error);
  ~WriteThread();

  NTSTATUS AddWriteData(WriteData *writedata);
  void TestWriteData();
  void WriteThreadFunc();
  void CancelWriteThread(){m_writethreadfinished = true;}

protected:

private:
  BlockedQueue m_blockqueue;
  HANDLE m_writethread;
  bool   m_writethreadfinished;
  NTSTATUS  m_status;

};

#endif