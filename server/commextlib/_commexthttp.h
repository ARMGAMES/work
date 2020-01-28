#ifndef _commexthttp_h_included
#define _commexthttp_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commhttp.h"
#include "_commcomm.h"
#include "_commextthreadmanager.h"


template< class _CompleteCommunicatorAndHandler, class HttpHeader>
class _CommExtHttpClientHandler
{
private:
	_CompleteCommunicatorAndHandler * comm;
	
	bool sendInProgress;
	bool readInProgress;
	enum {stateReceivingUnknown=0, stateReceivingInit=1, stateReceivingHeaders=2, stateReceivingContentLength=3, stateReceivingChunkSize=4, stateReceivingChunkData=5, stateReceivingCloseConnection=6};  
	int  receivingState;
	bool connectionInit;

	UINT32 _countSentBytes;
	UINT32 _countReceivedBytes;
	size_t dataToRead;
	PBlock   sending;
	PBlock   receiveing;
	PString partialLine;
	UINT32 checkSumm;
	
	BYTE buffer[2000];
	HttpHeader header;
	bool clientType;
	void _clear()
	{
		clientType= !!strcmp(typeid(HttpHeader).name(),"class CommHttpServerRequest");
		dataToRead=0;
		sendInProgress=false;
		readInProgress=false;
		receivingState=stateReceivingInit;
		connectionInit=false;
		sending.clear();
		receiveing.clear();
		header.clear();
		partialLine.clear();
	}
    void _processWritten()
	{
		PASSERT3(sendInProgress == true);
		sendInProgress=false;
		sending.clear();
		comm->sentDone();
	}

	void _readChunked( const BYTE* p, size_t sz)
	{
		while(1)
		{
			PASSERT3(receivingState == stateReceivingChunkSize);
			const BYTE * pStart = p;
			bool lineOk = sz > 0 && CommLine::read( partialLine, p, sz );
			if (lineOk)
			{
				if (partialLine.length() == 0)
				{
					sz -= p - pStart;
					continue;
				}
				size_t chunkLength = strtoul(partialLine,0,16);
				partialLine="";
				if (chunkLength == 0)
				{
					_stopRead();
					return;
				}
				BYTE * ptr = receiveing.append(chunkLength);
				receivingState = stateReceivingChunkData;
				sz -= p - pStart;
				if (sz >= chunkLength)
				{
					memcpy(ptr,p,chunkLength);
					p+=chunkLength;
					sz-=chunkLength;
					receivingState = stateReceivingChunkSize;
					continue;
				}
				else
				{
					if (sz > 0)
					{
						memcpy(ptr,p,sz);
						ptr+=sz;
						p+=sz;
					}
					dataToRead = chunkLength - sz;
					size_t rd = comm->read(ptr,dataToRead);
					if (rd)
					{
						PASSERT3(dataToRead == rd);
						dataToRead -= rd;
						receivingState = stateReceivingChunkSize;
						sz = 0;
						continue;
					}
					return;
				}
			}
			else
			{
				dataToRead=0;
				sz = comm->readIncomplete(buffer,sizeof(buffer));
				if (sz > 0)
				{
					p=buffer;
					continue;
				}
				return;
			}
		}
	}


    void _processRead(size_t rd)
	{
		PASSERT3(readInProgress == true);
		if (receivingState == stateReceivingHeaders)
		{
			const BYTE * p = buffer;
			if ( header.read(p,rd))
			{
				receivingState = stateReceivingUnknown;
				size_t dataInBuffer = rd - (p-buffer);
				const char* cl = header.getHeader( "Content-Length" );
				if( cl )
				{
					receivingState=stateReceivingContentLength;
					size_t sizeOfData = atoi(cl);
					if (sizeOfData)
					{
						receiveing.alloc(sizeOfData);
						if (dataInBuffer > sizeOfData)
							dataInBuffer = sizeOfData;
						if (dataInBuffer)
						{
							memcpy(receiveing.ptr(),p,dataInBuffer);
						}
						if (sizeOfData > dataInBuffer)
						{
							dataToRead = sizeOfData - dataInBuffer;
							rd = comm->read(receiveing.ptr()+dataInBuffer,dataToRead);
							if (rd)
							{
								PASSERT3(dataToRead == rd);
								dataToRead -= rd;
								_stopRead();
							}
						}
						else
							_stopRead();
					}
					else
					{
						_stopRead();
					}
				}
				else
				{
					const char* cl = header.getHeader( "Transfer-Encoding" );
				    if ( cl && !strcmp(cl,"chunked"))
					{
						receivingState = stateReceivingChunkSize;
						_readChunked(p,dataInBuffer);
					}
					else
					{
						if (clientType)
						{
							receivingState = stateReceivingCloseConnection;
							if (dataInBuffer)
							{
								receiveing.alloc(dataInBuffer);
								memcpy(receiveing.ptr(),p,dataInBuffer);
							}
							dataToRead=0;
							size_t rdNext = comm->readIncomplete(buffer,sizeof(buffer));
							if (rdNext > 0)
								_processRead(rdNext);
						}
						else
						{
							if (dataInBuffer)
							{
								receiveing.alloc(dataInBuffer);
								memcpy(receiveing.ptr(),p,dataInBuffer);
							}
							dataToRead=0;
							_stopRead();
						}
					}
				}
			}
			else
			{
				size_t rd_ = comm->readIncomplete(buffer,sizeof(buffer));
				if (rd_ > 0)
					_processRead(rd_);
			}
		}
		else if (receivingState==stateReceivingContentLength)
		{
			PASSERT3(dataToRead == rd);
			dataToRead -= rd;
			_stopRead();
		}
		else if (receivingState==stateReceivingChunkSize)
		{
			PASSERT3(dataToRead == 0);
			_readChunked(buffer,rd);
		}
		else if (receivingState==stateReceivingChunkData)
		{
			PASSERT3(dataToRead == rd);
			dataToRead -= rd;
			receivingState = stateReceivingChunkSize;
			_readChunked(0,0);
		}
		else if (receivingState==stateReceivingCloseConnection)
		{
			PASSERT3(dataToRead == 0);
			BYTE *ptr = receiveing.append(rd);
			memcpy(ptr,buffer,rd);
			size_t rdNext = comm->readIncomplete(buffer,sizeof(buffer));
			if (rdNext > 0)
				_processRead(rdNext);
		}

	}
	void _stopRead()
	{
		PASSERT3(dataToRead == 0);
		readInProgress=false;
		CommMsgBody results;
		header.compose(results);
		results.composePBlock(receiveing);
		comm->readDone(results);
		readStart();
	}
	
public:

	void construct(_CompleteCommunicatorAndHandler & comm_)
	{
		_clear();
		comm=&comm_;
	}
	void init()
	{
		_clear();
		if (comm->initConnection(checkSumm))
		{
			connectionInit=true;
		}
	}
	void deinit()
	{
		_clear();
	}

	bool isActive() const
	{
		return connectionInit;
	}

    void writeMsg(_CommMsg &msg)
	{
		PASSERT3(sendInProgress == false);
		sendInProgress=true;
		CommMsgParser parser(msg.body);
		parser.parsePBlock(sending);
		size_t wr = comm->write((const BYTE *) sending.ptr(),sending.size());
		if (wr)
		{
			_processWritten();
		}
	}
	void asyncWriteOk()
	{
		if(connectionInit == false)
		{
			if (comm->initConnection(checkSumm))
			{
				connectionInit=true;

			}
			return;
		}
		PASSERT3(sendInProgress == true);
		_CommAsyncRet ret = comm->asyncWriteOk();
		_countSentBytes += ret.written;
		_countReceivedBytes += ret.read;
		if( ret.read )
			_processRead(ret.read);
		if( ret.written )
			_processWritten();

	}

	void asyncReadOk()
	{
		if(connectionInit == false)
		{
			if (comm->initConnection(checkSumm))
			{
				connectionInit=true;

			}
			return;
		}
		PASSERT3(readInProgress == true);
		_CommAsyncRet ret = ( receivingState == stateReceivingHeaders || receivingState == stateReceivingCloseConnection || receivingState == stateReceivingChunkSize) ? comm->parentAsyncReadOk() : comm->asyncReadOk();
		_countSentBytes += ret.written;
		_countReceivedBytes += ret.read;
		if( ret.read )
			_processRead(ret.read);
		if( ret.written )
			_processWritten();
	}
	void readStart()
	{
		PASSERT3(readInProgress == false);
		PASSERT3( comm->connId);
		PASSERT3( connectionInit );
		dataToRead = 0;
		readInProgress=true;
		receivingState=stateReceivingHeaders;
		receiveing.clear();
		header.clear();
		size_t rd = comm->readIncomplete(buffer,sizeof(buffer));
		if (rd > 0)
			_processRead(rd);
	}

	void failed(int errCode)
	{
		if (receivingState==stateReceivingCloseConnection && errCode == 0)
		{
			CommMsgBody results;
			header.compose(results);
			results.composePBlock(receiveing);
			comm->readDone(results);
		}
	}
};


#endif
