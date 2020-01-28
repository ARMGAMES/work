#include "ppinclude.h"
#include "ppthread.h"
#include "pperror.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppipc.h"


#ifdef PWIN32



void setSecurityDescriptor(SECURITY_ATTRIBUTES &sa,SECURITY_DESCRIPTOR &sd)
{
	memset(&sa, 0, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE,(PACL)NULL, FALSE);
    sa.lpSecurityDescriptor = &sd;
}

#define SEC SECURITY_ATTRIBUTES sa; SECURITY_DESCRIPTOR sd; setSecurityDescriptor(sa,sd);



PNamedEvent::PNamedEvent()
{
	handle=NULL;
//	eventForWait = false;
}
PNamedEvent::~PNamedEvent()
{
	if( handle != NULL )
		::CloseHandle( handle );
}
void PNamedEvent::close()
{
	if( handle != NULL )
	{
		::CloseHandle( handle );
		handle=NULL;
	}
}

void PNamedEvent::create(bool eventForWait_, const char * name, bool manualReset)
{
	if (handle)
	{
		PString err("Unable to create event ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	SEC;
	handle = ::CreateEventA( &sa, ( manualReset ? TRUE : FALSE ), FALSE, name );
	if( handle == NULL)
	{
		PString err("Unable to create event ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), GetLastError() );
	}
	if( ::GetLastError()==ERROR_ALREADY_EXISTS )
	{
		close();
		PString err("Event already exists ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), ERROR_ALREADY_EXISTS );
	}
//	eventForWait = eventForWait_;
}

void PNamedEvent::open(bool eventForWait_, const char * name)
{
	if (handle)
	{
		PString err("Unable to open event ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	handle = ::OpenEventA(EVENT_ALL_ACCESS,FALSE,name);
	if (handle== NULL)
	{
		PString err("Unable to open event ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), GetLastError() );
	}
//	eventForWait = eventForWait_;
}

void PNamedEvent::set()
{
	PASSERT3(handle);
	BOOL Ok = ::SetEvent( handle );
	if( !Ok )
		throw PSystemError( "Unable to set event", GetLastError() );
}

SOCKET PNamedEvent::getSocket() const
{
	PASSERT3(handle);
	return (SOCKET) handle;
}
HANDLE PNamedEvent::getHandle() const
{
	PASSERT3(handle);
	return handle;
}

int PNamedEvent::wait(int timeOut)
{
	PASSERT3(handle);
	DWORD ret = ::WaitForSingleObject(handle,timeOut);
	if (ret == WAIT_FAILED)
		return waitFailed;
	if (ret == WAIT_TIMEOUT)
		return waitTimeout;
	return (int) ret;
}

PNamedMutex::PNamedMutex()
{
	handle=NULL;
}
PNamedMutex::~PNamedMutex()
{
	if( handle != NULL )
		::CloseHandle( handle );
}
void PNamedMutex::close()
{
	if( handle != NULL )
	{
		::CloseHandle( handle );
		handle=NULL;
	}
}

void PNamedMutex::create(const char * name)
{
	if (handle)
	{
		PString err("Unable to create mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	SEC
	handle = ::CreateMutexA(&sa,FALSE,name);
	if( handle == NULL)
	{
		PString err("Unable to create mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), GetLastError() );
	}
	if( ::GetLastError()==ERROR_ALREADY_EXISTS )
	{
		close();
		PString err("Mutex already exists ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), ERROR_ALREADY_EXISTS );
	}

}
void PNamedMutex::open(const char * name)
{
	if (handle)
	{
		PString err("Unable to open mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}

	handle = ::OpenMutexA(MUTEX_ALL_ACCESS,FALSE,name);
	if (handle == NULL)
	{
		PString err("Unable to open mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), GetLastError() );
	}
}

void PNamedMutex::set()
{
	PASSERT3(handle);
	::WaitForSingleObject(handle,INFINITE);
}

void PNamedMutex::release()
{
	PASSERT3(handle);
	::ReleaseMutex(handle);
}

PNamedSharedMemory::PNamedSharedMemory()
{
	handle=NULL;
	sharedAddress=0;
}

PNamedSharedMemory::~PNamedSharedMemory()
{
	if ( sharedAddress )
		UnmapViewOfFile(sharedAddress);
	if ( handle != NULL )
		::CloseHandle( handle );
}

void PNamedSharedMemory::close()
{
	if ( sharedAddress )
	{
		::UnmapViewOfFile(sharedAddress);
		sharedAddress = 0;
	}

	if( handle != NULL )
	{
		::CloseHandle( handle );
		handle=NULL;
	}
}

void * PNamedSharedMemory::create(int size, const char * name)
{
	if (handle)
	{
		PString err("Unable to create file mapping ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	if (sharedAddress)
	{
		PString err("Unable to create shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	SEC
	handle = ::CreateFileMappingA(INVALID_HANDLE_VALUE,&sa,PAGE_READWRITE,0,size,name);
	if( handle == NULL)
	{
		PString err("Unable to create file mapping ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), GetLastError() );
	}
	if( ::GetLastError()==ERROR_ALREADY_EXISTS )
	{
		close();
		PString err("File mapping already exists ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), ERROR_ALREADY_EXISTS );
	}
	sharedAddress= ::MapViewOfFile(handle,FILE_MAP_WRITE,0,0,0);
	if (sharedAddress==NULL)
	{
		int errCode = ::GetLastError(); 
		close();
		PString err("Unable to create shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errCode );
	}
	return sharedAddress;
}

void * PNamedSharedMemory::open(const char * name)
{
	if (handle)
	{
		PString err("Unable to open file mapping ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	if (sharedAddress)
	{
		PString err("Unable to open shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}

	handle = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS,FALSE,name);
	if( handle == NULL)
	{
		PString err("Unable to open file mapping ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), GetLastError() );
	}
	sharedAddress= ::MapViewOfFile(handle,FILE_MAP_WRITE,0,0,0);
	if (sharedAddress==NULL)
	{
		int errCode = ::GetLastError(); 
		close();
		PString err("Unable to open shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errCode );
	}
	return sharedAddress;
}

int PWaitForObjects::waitMultiple(int NumSlot, SOCKET * sockets, int timeOut)
{
	DWORD ret = ::WaitForMultipleObjects(NumSlot,(HANDLE * )sockets,FALSE,timeOut);
	if (ret == WAIT_FAILED)
		return waitFailed;
	if (ret == WAIT_TIMEOUT)
		return waitTimeout;
	return ret - WAIT_OBJECT_0;
}

/*static*/ int PWaitForObjects::waitSingle(SOCKET socket, int timeOut)
{
	DWORD ret = ::WaitForSingleObject((HANDLE) socket,timeOut);
	if (ret == WAIT_FAILED)
		return waitFailed;
	if (ret == WAIT_TIMEOUT)
		return waitTimeout;
	return 0;
}


#else  // PPOSIX


#include <sys/stat.h>
#if defined( P_MAC10X )
#undef SEM_FAILED
#define SEM_FAILED ((sem_t*)-1)
#endif

PNamedEvent::PNamedEvent()
{
	handle = -1;
	handlewrite = -1;
	eventForWait = false;
	created = false;
	shutdown = false;
}
PNamedEvent::~PNamedEvent()
{
	close();
}
void PNamedEvent::close()
{
	if(shutdown)
		return;
	shutdown=true;	
	if( handle != -1 )
	{
		if (eventForWait)
		{
			if (handlewrite != -1)
			{
				int s=0;
				int ret = write(handlewrite,&s,1);
				::close(handlewrite);
				handlewrite=-1;
			}
		}    
		::close( handle );
		handle=-1;
	}
	if ( created )
	{
		::unlink(_name.c_str());
		created=false;
		_name = "";
	}
}

void PNamedEvent::create(bool eventForWait_, const char * name, bool manualReset)
{
	if (handle != -1)
	{
		PString err("Unable to create event ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	::unlink(name);
	int ret = ::mkfifo(name,0666);
	if( ret==-1)
	{
		PString err("Unable to create event ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errno );
	}
	if (eventForWait_)
	{
		handle = ::open(name,O_RDONLY|O_NONBLOCK);
		handlewrite = ::open(name,O_WRONLY|O_NONBLOCK);
	}
	else
	{
		handle = ::open(name,O_RDWR|O_NONBLOCK);
	}
	if (handle == -1)
	{
		if (handlewrite != -1)
		{
			::close(handlewrite);
			handlewrite=-1;
		}
		int errCode = errno;
		::unlink(name);
		PString err("Unable to create event (open file) ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errCode );
	}
	int flags = ::fcntl( handle, F_GETFD, 0 );
	::fcntl( handle, F_SETFD, flags | FD_CLOEXEC );

	created=true;
	_name.assign(name);
	eventForWait = eventForWait_;

}
void PNamedEvent::open(bool eventForWait_, const char * name)
{
	if (handle != -1)
	{
		PString err("Unable to open event ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	if (eventForWait_)
	{
		handle = ::open(name,O_RDONLY|O_NONBLOCK);
		handlewrite = ::open(name,O_WRONLY|O_NONBLOCK);
	}
	else
	{
		handle = ::open(name,O_WRONLY|O_NONBLOCK);
	}
	if (handle == -1)
	{
		if (handlewrite != -1)
		{
			::close(handlewrite);
			handlewrite=-1;
		}

		PString err("Unable to open event (open file) ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errno );
	}
	int flags = ::fcntl( handle, F_GETFD, 0 );
	::fcntl( handle, F_SETFD, flags | FD_CLOEXEC );

	created=false;
	eventForWait = eventForWait_;
}

void PNamedEvent::set()
{
	PASSERT3(handle!=-1);
	if (eventForWait)
		return;
	int s=0;
retr_intr:
	int ret = write(handle,&s,1);
	if( ret<0 )
	{
		if (errno == EINTR)
			goto retr_intr;
		throw PSystemError( "Unable to set event", errno );
	}
}

SOCKET PNamedEvent::getSocket() const
{
	PASSERT3(handle!=-1);
	PASSERT3(eventForWait);
	return (SOCKET) handle;
}
int PNamedEvent::getHandle() const
{
	PASSERT3(handle!=-1);
	PASSERT3(eventForWait);
	return handle;
}

int PNamedEvent::wait(int timeOut) 
{
	PASSERT3(handle!=-1);
	PASSERT3(eventForWait);

	struct pollfd poll;
	poll.fd = handle;
	poll.events = POLLIN;
retr_intr:
	int res = ::poll( &poll, 1, timeOut );
	if (shutdown)
		return waitFailed;
	if( res < 0 )
	{
		if (errno == EINTR)
			goto retr_intr;
		return waitFailed;
	}
	else if( res == 0 )
		return waitTimeout;
	int r;
retr_intr_read:
	int ret = read(handle,&r,1);
	if( ret<0 )
	{
		if (errno == EINTR)
			goto retr_intr_read;
		throw PSystemError( "Unable to read event", errno );
	}
	char buf[100];
	while(read(handle,buf, sizeof(buf)) >0);
	return 0;
}

void PNamedEvent::reset() 
{
	PASSERT3(handle!=-1);
	PASSERT3(eventForWait);
	int r;
retr_intr_read:
	int ret = read(handle,&r,1);
	if( ret<0 )
	{
		if (errno == EINTR)
			goto retr_intr_read;
		throw PSystemError( "Unable to read event", errno );
	}
	char buf[100];
	while(read(handle,buf, sizeof(buf)) >0);
}


PNamedMutex::PNamedMutex()
{
	handle=SEM_FAILED;
	created=false;
}
void PNamedMutex::close()
{
	if( handle !=SEM_FAILED )
	{
		::sem_close(handle);
		handle = SEM_FAILED;
	}
	if (created)
	{
		::sem_unlink(_name.c_str());
		_name="";
	}
}

PNamedMutex::~PNamedMutex()
{
	close();
}

void PNamedMutex::create(const char * name)
{
	if (handle !=SEM_FAILED)
	{
		PString err("Unable to create mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	::sem_unlink(name);
	handle = ::sem_open(name,O_CREAT,0666,1);
	if( handle == SEM_FAILED)
	{
		PString err("Unable to create mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errno );
	}
	created = true;
	_name.assign(name);

}
void PNamedMutex::open(const char * name)
{
	if (handle != SEM_FAILED)
	{
		PString err("Unable to open mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}

	handle = ::sem_open(name,0);
	if (handle == SEM_FAILED)
	{
		PString err("Unable to open mutex ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errno );
	}
}

void PNamedMutex::set()
{
	PASSERT3(handle != SEM_FAILED);
retr_intr:
	int ret = ::sem_wait(handle);
	if (ret<0)
	{
		if (errno == EINTR)
			goto retr_intr;
	}
}
void PNamedMutex::release()
{
	PASSERT3(handle != SEM_FAILED);
	::sem_post(handle);
}


#ifndef P_ANDROID
PNamedSharedMemory::PNamedSharedMemory()
{
	handle=-1;
	sharedAddress=0;
	created=false;
	key = -1;
}
void PNamedSharedMemory::close()
{
	if ( sharedAddress )
	{
		::shmdt(sharedAddress);
		sharedAddress=0;
	}

	if ( handle != -1 )
	{
		if (created)
		{
			::shmctl( handle, IPC_RMID, 0);
		}
		handle=-1;
	}
	if (created)
	{
		::unlink(_name.c_str());
		_name="";
	}
}
PNamedSharedMemory::~PNamedSharedMemory()
{
	close();
}

void * PNamedSharedMemory::create(int size, const char * name)
{
	if (handle != -1)
	{
		PString err("Unable to create file mapping ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	if (sharedAddress)
	{
		PString err("Unable to create shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	::unlink(name);
	int tmp = ::open(name,O_RDWR|O_CREAT|O_EXCL,0666);
	if( tmp == -1)
	{
		PString err("Unable to create key_t file ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errno );
	}
	::close(tmp);

	key = ::ftok(name,1);
	if( key == -1)
	{
		int errCode=errno;
		::unlink(name);
		PString err("Unable to create key_t ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errCode );
	}

	handle = ::shmget(key,size,IPC_CREAT|IPC_EXCL | 0666);
	if( handle == -1)
	{
		if (errno == EEXIST)
		{
			handle = ::shmget(key,0,0666);
			if (handle == -1)
			{
				int errCode=errno;
				::unlink(name);
				PString err("Unable to create/open file mapping ");
				err.append(name?name:"");
				throw PSystemError( err.c_str(), errCode );
			}
			else 
			{
				PTRACE5("shmget opened, handle=%d", handle);
			}
			int code = ::shmctl( handle, IPC_RMID, 0);
			if (code == -1)
			{
				int errCode=errno;
				::unlink(name);
				PString err("Unable to delete file mapping ");
				err.append(name?name:"");
				throw PSystemError( err.c_str(), errCode );
			}
			else
			{
				PTRACE5("shmctl returned 0");
			}
			handle = ::shmget(key,size,IPC_CREAT|IPC_EXCL | 0666);
			if( handle == -1)
			{
				int errCode=errno;
				::unlink(name);
				PString err("Unable to create file mapping (2) ");
				err.append(name?name:"");
				throw PSystemError( err.c_str(), errCode );
			}
		}
		else
		{
			int errCode=errno;
			::unlink(name);
			PString err("Unable to create file mapping ");
			err.append(name?name:"");
			throw PSystemError( err.c_str(), errCode );
		}

	}


	created=true;
	_name.assign(name);

	sharedAddress= ::shmat(handle,0,0);
	if (sharedAddress==(void *)(-1))
	{
		sharedAddress = 0;
		int errCode = errno; 
		close();
		PString err("Unable to create shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errCode );
	}
	return sharedAddress;
}
void * PNamedSharedMemory::open(const char * name)
{
	if (handle != -1)
	{
		PString err("Unable to open file mapping ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	if (sharedAddress)
	{
		PString err("Unable to open shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), "Handle is opened");
	}
	key = ::ftok(name,1);
	if( key == -1)
	{
		PString err("Unable to open key_t ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errno );
	}
	handle = ::shmget(key,0,0666);
	if( handle == -1)
	{
		PString err("Unable to open file mapping ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errno );
	}
	sharedAddress= ::shmat(handle,0,0);
	if (sharedAddress==(void *)(-1))
	{
		sharedAddress = 0;
		int errCode = errno; 
		close();
		PString err("Unable to open shared memory ");
		err.append(name?name:"");
		throw PSystemError( err.c_str(), errCode );
	}
	return sharedAddress;
}
#endif

PWaitForObjects::PWaitForObjects(int maxNumSlots_)
{
	PASSERT3(maxNumSlots_>0);
	maxNumSlots = maxNumSlots_;
	polls = new pollfd[maxNumSlots];
}
PWaitForObjects::~PWaitForObjects()
{
	delete [] polls;
}

int PWaitForObjects::waitMultiple(int NumSlot, SOCKET * sockets, int timeOut)
{
	PASSERT3(NumSlot<=maxNumSlots);
	int i;
	for (i=0;i<NumSlot;i++)
	{
		polls[i].fd = sockets[i];
		polls[i].events = POLLIN;
		polls[i].revents = 0;
	}
retr_intr:
	int res = ::poll( polls, NumSlot, timeOut );
	if( res < 0 )
	{
		if (errno == EINTR)
			goto retr_intr;
		return waitFailed;
	}
	else if( res == 0 )
	{
		return waitTimeout;
	}
	for (i=0;i<NumSlot;i++)
	{
		if (polls[i].revents) 
		{
			return i;
		}
	}
	return waitFailed;
}

/*static*/ int PWaitForObjects::waitSingle(SOCKET socket, int timeOut)
{
	struct pollfd poll;
	int i;
	poll.fd = socket;
	poll.events = POLLIN;
	poll.revents = 0;
retr_intr:
	int res = ::poll( &poll, 1, timeOut );
	if( res < 0 )
	{
		if (errno == EINTR)
			goto retr_intr;
		return waitFailed;
	}
	else if( res == 0 )
	{
		return waitTimeout;
	}

	if (poll.revents) 
		return 0;
	else
		return waitFailed;
}


#endif

