#ifndef _commwebsocket_h_included
#define _commwebsocket_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commhttp.h"
#include "_commcomm.h"
#include "commpasswdencrypt.h"
#include "commcgi.h"
#include "commssl.h"

#define MAX_DATA_LENGTH 512000
#define KEEPALIVE_PERIOD 30000
#define KEEPALIVE_TIMEOUT 20000




class CommCookieVector : public  vector<CommHttpHeader>
{
public:
	const char * find(const char * name) const
	{
		for (size_t i = 0; i < size(); i++)
		{
			if ((*this)[i].name.equals(name))
				return (*this)[i].value.c_str();
		}
		return 0;
	}
	void insert(const char * name,const char * value )
	{
		CommHttpHeader & p = *vector<CommHttpHeader>::insert(end(),CommHttpHeader());	
		p.name = name;
		p.value = value;
	}

	void insertMove(PString & name,PString & value )
	{
		CommHttpHeader & p = *vector<CommHttpHeader>::insert(end(),CommHttpHeader());	
		p.name.moveFrom(name);
		p.value.moveFrom(value);
	}
};



template< class _CompleteCommunicator,class _MsgHandler>
class _CommWebSocketHandler
{
private:
	_CompleteCommunicator * comm;
	_MsgHandler * msgHandler;
	
	enum {stateUnknown=0, stateInit=1, stateReceivingHeaders=2, stateSendingHeaders=3, stateWebSocket=4, stateClosingConnection=5, stateClosedConnection=6};  
	int  state;
	int rdState;
	int wrState;
	
	int sendingKeepAlive;  // 0 - none, 1 - to be sent, 2 - in progress, 3- sent, waiting for reply
	int sendingPong;       // 0 - none, 1 - to be sent, 2 - in progress
	int closingConnection; // 0 - none, 1 - to be sentclose 



	UINT32 keepAliveSent;
	UINT32 lastKeepAlive;

	UINT32 lastSent;
	UINT32 lastReceived;


	UINT32 _countSentBytes;
	UINT32 _countReceivedBytes;

	PString partialLine;
	
	BYTE buffer[2000];
	BYTE header[128];   // to fit all control frames
	bool finBit;
	int opcode;
	int mask;
	UINT64 dataLength;
	UINT32 dataPosition;
	BYTE pongData[128];   // to fit all control frames
	UINT32 pongLength;

	CommMsgSimpleQueue wrQueue;
	_CommMsg inWriting;
	UINT32   writingDataPosition;
	UINT32   dataLengthWr;
	_CommMsg inReading;
	BYTE wrHeader[128];  // to fit all control frames

	CommHttpServerRequest httpHeader;
	PString user;
	PString pwd;
	PString sid;
	PString address;
	int slot;
	int mgrIdx;

	void _clear()
	{
		rdState=0;
		wrState=0;
		state=stateUnknown;
		httpHeader.clear();
		partialLine.clear();
		address.clear();
		user.clear();
		pwd.clear();
		sid.clear();
		_countSentBytes=0;
		_countReceivedBytes=0;
		sendingKeepAlive=0;
		closingConnection=0;
		sendingPong=0;
		pongLength=0;
		wrQueue.clear();
		writingDataPosition=0;
		dataLengthWr=0;
		inWriting.body._dealloc();
		inReading.body._dealloc();
		lastSent=lastReceived=keepAliveSent=SYSTEMTICKS();
	}
	void _parseCookie(const char * line,CommCookieVector &cookie)
	{
		int code = 0;
		PString name;
		PString value;
		bool ok=true;
		while(ok)
		{
			switch(code)
			{
			case 0:
				if (*line == ' ' )
					line++;
				else
				{
					code = 1;
				}
				break;
			case 1:
				if (*line == '=')
				{
					code = 2;
					line++;
				}
				else
				{
					name.append(*line++);
				}
				break;
			case 2:
				if (!*line || *line == ';')
				{
					code = 0;
					PString decodedValue;
					CommCgi::urlDecode( decodedValue, value);
					cookie.insertMove(name,decodedValue);
					value.clear();
					if (!*line)
					{
						ok=false;
						break;
					}
					line++;
				}
				else
				{
					value.append(*line++);
				}
				break;
			}
		}
	}
	void _startHandshake();
    int  _processWrittenHeaders();
	bool _keepAlive();

	void _processMsg(_CommMsg &msg)
	{
		if (msg.type==_COMM_MSGTYPE_CONNECT_GUARD_REQUEST)
		{
			UINT32 thisId, peerId, sessionId;
			CommMsgParser parser(msg.body);
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			if (!parser.parseEnded())
			{
				const char* user_;
				parser.parseString( user_ );
				if (user.equals(user_))
				{
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) _COMM_MSGTYPE_CONNECT_GUARD_REQUEST: User found: %s, password and sid appended",mgrIdx, slot,address.c_str(), user_);
					msg.body.composeString(pwd).composeString(sid);
				}
				else
				{
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) _COMM_MSGTYPE_CONNECT_GUARD_REQUEST: User %s not found",mgrIdx, slot,address.c_str(),user_);
					msg.body.composeString("").composeString("");
				}
			}
		}
	}

    void _processWriteClosingConnection()
	{
		PASSERT3(state == stateClosingConnection);
		size_t wr=0;
		do
		{
			switch (wrState)
			{
			case 0:
				{
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) _processWriteClosingConnection",mgrIdx, slot,address.c_str());
					PASSERT3(state == stateClosingConnection);
					closingConnection=2;
					wrState=1;
					wrHeader[0]=0x88;
					wrHeader[1]=dataLength; // dataLength and buffer from receiving contain recived data, just send it back
					if (dataLength)
					{
						memcpy(wrHeader+2,header+dataPosition,dataLength);
					}
					wr = comm->write(wrHeader,2+dataLength);
				}
				break;
			case 1:
				{
					UINT16 closeCode = 0;
					if (dataLength)
					{
						PASSERT3(dataLength>=2);
						closeCode =  CommMsgBody::readUINT16(header+dataPosition);
					}
					lastSent = SYSTEMTICKS();
					closingConnection=0;
					state = stateClosedConnection;
					wrState=0;
					wr=0;
					msgHandler->forceSlotFailure("Connection closed by peer",closeCode);
				}
				break;
			default:
				PASSERT3(0);
			}
		} while(wr > 0);
	}

    int _processWritePong()
	{
		PASSERT3(state == stateWebSocket);
		PASSERT3(sendingPong > 0);
		size_t wr=0;
		do
		{
			switch (wrState)
			{
			case 0:
				{
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) _processWritePong",mgrIdx, slot,address.c_str());
					sendingPong=2;
					wrState=1;
					wrHeader[0]=0x8a;
					wrHeader[1]=pongLength; // dataLength and buffer from receiving contain recived data, just send it back
					if (pongLength)
					{
						memcpy(wrHeader+2,pongData,pongLength);
					}
					wr = comm->write(wrHeader,2+pongLength);
				}
				break;
			case 1:
				{
					lastSent = SYSTEMTICKS();
					sendingPong = 0;
					pongLength = 0;
					wrState=0;
					return 1;
				}
			default:
				PASSERT3(0);
			}
		} while(wr > 0);
		return 0;
	}
    int _processWrittenKeepAlive()
	{
		PASSERT3(state == stateWebSocket);
		size_t wr=0;
		do
		{
			switch (wrState)
			{
			case 0:
				{
					wrState=1;
					sendingKeepAlive=2;
					wrHeader[0]=0x89;
					wrHeader[1]=0; 
					wr = comm->write(wrHeader,2);
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) sending keepalive",mgrIdx, slot,address.c_str());
				}
				break;
			case 1:
				{
					wrState = 0;
					lastSent = keepAliveSent=SYSTEMTICKS();
					sendingKeepAlive=3;
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) keepalive sent",mgrIdx, slot,address.c_str());
					wr=0;
					return 1;
				}
				break;
			default:
				PASSERT3(0);
			}
		} while(wr > 0);
		return 0;
	}




	int _processReadWebSocket()
	{
		PASSERT3(state == stateWebSocket && closingConnection == 0);
		size_t rd;
		int ret = 0;
		do
		{
			switch (rdState)
			{
			case 0:
				{
					finBit=false;
					opcode=0;
					rdState=1;
					mask=0;
					dataLength=0;
					dataPosition=0;
					rd = comm->read(header,2);
				}
				break;
			case 1:
				{
					lastReceived = SYSTEMTICKS();
					finBit = !!(header[0]&0x80);
					opcode = header[0]&0xf;
					mask = (header[1]&0x80);
					dataLength = header[1]&0x7f;
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) finBit=%s, opcode=%d, length=%d, mask=%d",mgrIdx, slot,address.c_str(), (finBit?"yes":"no"), opcode,(UINT32)dataLength,mask);
					if (dataLength > 0)
					{
						PASSERT3(mask);
					}
					else
					{
						PASSERT3(opcode != 2 && opcode != 0);
					}
					if (dataLength || mask)
					{
						size_t toRead=0;
						if (dataLength == 126)
							toRead=2;
						else if (dataLength == 127)
							toRead=8;
						if (mask)
							toRead+=4;
						dataPosition=toRead + 2;
						if (toRead)
						{
							rdState=2;
							rd = comm->read(header+2,toRead);
						}
						else
						{
							rdState=2;
							rd=1;
						}
					}
					else
					{
						rd=1;
						rdState=3;
					}
				}
				break;
			case 2:
				{
					lastReceived = SYSTEMTICKS();
					if (dataLength == 126)
					{
						dataLength = CommMsgBody::readUINT16(header+2);
						if (mask)
						{
							mask=4;
						}

					}
					else if (dataLength == 127)
					{
						dataLength =  CommMsgBody::readUINT64(header+2);
						if (mask)
						{
							mask=10;
						}
					}
					else 
					{
						if (mask)
							mask=2;
					}
					
					rdState=3;
					if(dataLength)
					{
						if (opcode == 2)
						{
							inReading.priority = _COMM_MSGPRIORITY_INITCONNECT;
							inReading.flags = 0;
							inReading.internal.mux = 0;
							inReading.internal.muxChannelId = 0;
							BYTE * ptr = inReading.body._append(dataLength);
							rd = comm->read(ptr,dataLength);
						}
						else if (opcode == 0)
						{
							// next fragment
							BYTE * ptr = inReading.body._append(dataLength);
							rd = comm->read(ptr,dataLength);
						}
						else
						{
							rd = comm->read(header+dataPosition,dataLength);
						}
					}
					else
					{
						rd=1;
					}
				}
				break;
			case 3:
				{
					lastReceived = SYSTEMTICKS();
					if (mask)
					{
						unsigned char *pMask = header + mask;
						unsigned char * pData = (opcode == 2 || opcode == 0) ? (inReading.body._writePtr() + inReading.body._size() - dataLength) : (header+dataPosition);
						for (UINT64 i=0;i<dataLength; i++)
						{
							pData[i]^=pMask[i%4];
						}
					}
					if (opcode == 2 || opcode == 0)
					{
						if (finBit)
						{
							inReading.type = *inReading.body._writePtr();
							inReading.body._shift(1);
							PTRACE3("CommWebSocket[ %d ][ %d ](%s) msg.type=%d, length=%d,",mgrIdx, slot,address.c_str(),inReading.type,(UINT32)dataLength);
							_processMsg(inReading);
							msgHandler->msg( inReading.internal.mux, inReading.internal.muxChannelId, inReading );
							inReading.body._dealloc();
						}
						rdState=0;
						rd=1;
					}
					else if (opcode == 8)
					{
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) opcode 8,",mgrIdx, slot,address.c_str());
						rd=0;
						rdState=0;
						closingConnection=1;
						ret=1;
						break;
					}
					else if (opcode == 9)
					{
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) opcode 9,",mgrIdx, slot,address.c_str());
						rd=1;
						rdState=0;
						if(sendingPong)
						{
							PTRACE3("CommWebSocket[ %d ][ %d ](%s) !!! previous ping was not replied, ignore",mgrIdx, slot,address.c_str());
						}
						else
						{
							ret=1;
							sendingPong=1;
							pongLength = dataLength;
							if (pongLength)
								memcpy(pongData,header+dataPosition,pongLength);
						}
						break;
					}
					else if (opcode == 10)
					{
					
						rd=1;
						rdState=0;
						
						if (sendingKeepAlive == 3)
						{
							lastKeepAlive = SYSTEMTICKS() - keepAliveSent;
							sendingKeepAlive = 0;
							PTRACE3("CommWebSocket[ %d ][ %d ](%s) Pong received %d",mgrIdx, slot,address.c_str(),lastKeepAlive);
						}
						else
						{
							PTRACE3("CommWebSocket[ %d ][ %d ](%s) _CommWebSocketHandler: unexpected PONG code received, sendingKeepAlive=%d",mgrIdx, slot,address.c_str(),sendingKeepAlive);
						}
						break;
					}
					else
					{
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) _CommWebSocketHandler: unexpected opcode '%d' received",mgrIdx, slot,address.c_str(), opcode);
						PASSERT3(0 && "Unknown opcode");
					}

				}
				break;
			default:
				PASSERT3(0);
			}
		} while(rd > 0);
		return ret;
	}


    int _processWrittenWebSocket()
	{
		PASSERT3(state == stateWebSocket);
		size_t wr=0;
		do
		{
			switch (wrState)
			{
			case 0:
				{
					if (wrQueue.size() == 0  && inWriting.body._size() == 0)
					{
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) wrQueue.size() = 0 && inWriting.body._size() == 0, should not happen, wr= %d",mgrIdx, slot,address.c_str(),(int)wr);
						return 0;
					}
					bool newMessage = true;
					if (inWriting.body._size() > 0)
					{
						// continue with previous message
						PASSERT3(writingDataPosition > 0);
						newMessage = false;
					}
					else
					{
						PASSERT3(writingDataPosition == 0);
						inWriting.moveFrom(wrQueue.peek());
						wrQueue.pop();
					}

					wrState=1;
					if (newMessage)
						dataLengthWr=inWriting.body._size() + 1;
					else
						dataLengthWr=inWriting.body._size() - writingDataPosition;
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) sending %s msg.type=%d, length of the rest=%d, block length=%d",mgrIdx, slot,address.c_str(), (newMessage?"new":"next block"),inWriting.type,dataLengthWr,(dataLengthWr>MAX_DATA_LENGTH ? MAX_DATA_LENGTH:dataLengthWr));
					bool lastBlock = true;
					if (dataLengthWr > MAX_DATA_LENGTH)
					{
						lastBlock = false;
						dataLengthWr = MAX_DATA_LENGTH;
					}
					int headerLength=2;
					BYTE *ptr=wrHeader+2;
					wrHeader[0]= (lastBlock ? 0x80 : 0x0) | (newMessage ? 0x02 : 0x0);
					
					if (dataLengthWr < 126)
					{
						wrHeader[1]=dataLengthWr;
					}
					else if (dataLengthWr <= 0xffff)
					{
						wrHeader[1]=126;
						CommMsgBody::writeUINT16( wrHeader + 2,  dataLengthWr);
						headerLength+=2;
						ptr+=2;
					}
					else
					{
						wrHeader[1]=127;
						CommMsgBody::writeUINT64( wrHeader + 2,  dataLengthWr);
						headerLength+=8;
						ptr+=8;
					}
					if (newMessage)
					{
						*ptr++=inWriting.type;
						if( inWriting.internal.flags & _CommMsg::Patch8 )
						{
							CommMsgBody::writeUINT32( ptr, inWriting.internal.patch0 );
							ptr += 4;
							CommMsgBody::writeUINT32( ptr, inWriting.internal.patch1 );
							ptr += 4;
						}
						else
						{
							memcpy( ptr, inWriting.body._readPtr(), 8 );
							ptr+=8;
						}
						if (dataLengthWr < 126)
						{
							wrState=2;
							memcpy(ptr,inWriting.body._readPtr() + 8, inWriting.body._size() - 8 );
							wr = comm->write(wrHeader,headerLength+dataLengthWr);
							writingDataPosition = inWriting.body._size();
							dataLengthWr = 0;
						}
						else
						{
							writingDataPosition = 8;
							dataLengthWr -= 9;
							wr = comm->write(wrHeader,headerLength+9);
						}
					}
					else
					{
						wr = comm->write(wrHeader,headerLength);
					}
				}
				break;
			case 1:
				{
					wrState=2;
					if (dataLengthWr > 0)
					{
						wr = comm->write(inWriting.body._readPtr() + writingDataPosition,dataLengthWr);
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) sent msg.type=%d, block length=%d, wr=%d,",mgrIdx, slot,address.c_str(),inWriting.type,dataLengthWr,(int)wr);
						writingDataPosition+=dataLengthWr;
						dataLengthWr = 0;
					}
					else
					{
						wr=1;
					}
				}
				break;
			case 2:
				{
					lastSent = SYSTEMTICKS();
					if (0 == (inWriting.body._size() - writingDataPosition))
					{
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) sent msg.type=%d, deallocated",mgrIdx, slot,address.c_str(),inWriting.type);
						inWriting.body._dealloc();
						writingDataPosition = 0;
					}
					else
					{
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) block sent msg.type=%d",mgrIdx, slot,address.c_str(),inWriting.type);
					}
					wrState=0;
					return 1;
/*
					if (state == stateClosingConnection)
					{
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) sending done msg.type=%d, go to closing",mgrIdx, slot,address.c_str(),inWriting.type);
						return _processWriteClosingConnection();
					}
					else
					{
						wr = wrQueue.size();
						PTRACE3("CommWebSocket[ %d ][ %d ](%s) sending done msg.type=%d, wr=%I64d",mgrIdx, slot,address.c_str(),inWriting.type,wr);
					}
*/
				}
				break;
			default:
				PASSERT3(0);
			}

		} while(wr > 0);
		return 0;
	}

    void _processReadHeaders(size_t rd);
	
public:
	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body);

	void construct(_CompleteCommunicator & comm_, _MsgHandler &handler_,int slot_, int mgrIdx_)
	{
		_clear();
		comm=&comm_;
		msgHandler = &handler_;
		slot = slot_;
		mgrIdx = mgrIdx_;
	}
	void init(const char * addr)
	{
		address =  addr;
		state=stateInit;
		UINT32 checkSum;
		if (comm->initConnection(checkSum))
		{
			_startHandshake();
		}
	}
	void deinit()
	{
		_clear();
	}

	bool isConnected() const
	{
		return state == stateWebSocket && closingConnection == 0;
	}

    void writeMsg(_CommMsg &msg)
	{
		wrQueue.push(msg);
		if (wrState == 0 && state == stateWebSocket && closingConnection == 0)
		{
			done(1);
		}
	}
	void setFirstRead();

	void done(int done_);
	void asyncWriteOk()
	{
		PASSERT3(state>=stateInit);
		if (state==stateInit)
		{
			UINT32 checkSum;
			if ( comm->initConnection(checkSum))
				_startHandshake();
			return;
		}
		_CommAsyncRet ret = comm->asyncWriteOk();
		_countSentBytes += ret.written;
		_countReceivedBytes += ret.read;
		done((ret.written?1:0) |(ret.read?2:0));
	}

	void asyncReadOk()
	{
		PASSERT3(state>=stateInit);
		if (state==stateInit)
		{
			UINT32 checkSum;
			if ( comm->initConnection(checkSum))
				_startHandshake();
			return;
		}
		 if (state==stateReceivingHeaders)
		 {
			_CommAsyncRet ret =  comm->parentAsyncReadOk();
			_countSentBytes += ret.written;
			_countReceivedBytes += ret.read;
			_processReadHeaders(ret.read);
		 }
		 else
		 {
			_CommAsyncRet ret = comm->asyncReadOk();
			_countSentBytes += ret.written;
			_countReceivedBytes += ret.read;
			done((ret.written?1:0) |(ret.read?2:0));
		 }
	}

};

template< class _CompleteCommunicator,class _MsgHandler>
void _CommWebSocketHandler<_CompleteCommunicator,_MsgHandler>::_startHandshake()
{
		state=stateReceivingHeaders;
		size_t rd = comm->readIncomplete(buffer,sizeof(buffer));
		if (rd > 0)
			_processReadHeaders(rd);
}

template< class _CompleteCommunicator,class _MsgHandler>
void _CommWebSocketHandler<_CompleteCommunicator,_MsgHandler>::_processReadHeaders(size_t rd)
{

	PASSERT3(state == stateReceivingHeaders);
	const BYTE * p = buffer;
	if ( httpHeader.read(p,rd))
	{
		PString r;
		PString retKey;
		const char * cookieLine = httpHeader.getHeader("Cookie");
		if (cookieLine)
		{
			CommCookieVector cookie;
			_parseCookie(cookieLine,cookie);
			const char * _user = cookie.find("user" );
			if (_user)
			{
				const char * _encPwd = cookie.find("encpwd" );
				if (_encPwd)
				{
					bool ret = CommPasswdEncryptDecrypt::decrypt(_encPwd,pwd);
					if (ret)
					{
						user = _user;
						const char * sid_ = cookie.find("admsid" );
						if (sid_)
							sid = sid_;
						PLog( "CommWebSocket[ %d ][ %d ](%s) Encrypted password is found and used",mgrIdx, slot,address.c_str());
					}
					else
					{
						PLog( "CommWebSocket[ %d ][ %d ](%s) Encrypted password is found but couldn't be decrypted",mgrIdx, slot,address.c_str());
						pwd.clear();
						sid.clear();
					}
				}
				else
				{
					PLog( "CommWebSocket[ %d ][ %d ](%s) Encrypted password not found in cookies",mgrIdx, slot,address.c_str());
				}
			}
			else
			{
				PLog( "CommWebSocket[ %d ][ %d ](%s) User not found in cookies",mgrIdx, slot,address.c_str());
			}
		}
		else
		{
			PLog( "CommWebSocket[ %d ][ %d ](%s) Cookie not found",mgrIdx, slot,address.c_str());
		}
		const char * key = httpHeader.getHeader("Sec-WebSocket-Key");
		if (key)
		{
			PString ret;
			ret = key;
			ret.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
			CommSSLChksum sha1;
			sha1.addData((const BYTE *)ret.c_str(),ret.length());
			PBlock retSha1;
			sha1.chksum(retSha1);
			CommSSLBase64::encode(retKey,retSha1.ptr(),retSha1.size());
		}
		else
		{
			throw PInternalError( "Not web socket request");
		}
		CommHttpServerResponse hr;
		hr.setResponse(101,"Switching Protocols");
		hr.addHeader("Upgrade","websocket");
		hr.addHeader("Connection","Upgrade");
		hr.addHeader("Sec-WebSocket-Accept",retKey.c_str());
		hr.createResponse(r);
		state = stateSendingHeaders;
		size_t wr = comm->write((const BYTE *) r.c_str(),r.length());
		if (wr)
		{
			done(1);
		}
	}
	else
	{
		rd = comm->readIncomplete(buffer,sizeof(buffer));
		if (rd > 0)
			_processReadHeaders(rd);
	}
}


template< class _CompleteCommunicator,class _MsgHandler>
int _CommWebSocketHandler<_CompleteCommunicator,_MsgHandler>::_processWrittenHeaders()
{
	PASSERT3(state == stateSendingHeaders);
	state=stateWebSocket;
	return 0;

}

template< class _CompleteCommunicator,class _MsgHandler>
void _CommWebSocketHandler<_CompleteCommunicator,_MsgHandler>::setFirstRead()
{
	PASSERT3(rdState==0);
	_processReadWebSocket();
}

template< class _CompleteCommunicator,class _MsgHandler>
void _CommWebSocketHandler<_CompleteCommunicator,_MsgHandler>::done(int done_)
{
	PASSERT3(state>stateInit);
	while (done_ != 0)
	{
		if (done_ &1)
		{
			done_ &= ~0x01;
			//written
			if (state == stateReceivingHeaders)
			{
				PASSERT3(0);
			}
			else if (state == stateSendingHeaders)
			{
				done_ |= _processWrittenHeaders();
			}
			else if (state == stateWebSocket)
			{
				if (wrState == 0)
				{
					// ready to send new packet
					if (closingConnection == 1)
					{
						state = stateClosingConnection;
						_processWriteClosingConnection();
					}
					else if (sendingPong == 1)
						done_ |= _processWritePong();
					else if (sendingKeepAlive == 1)
						done_ |= _processWrittenKeepAlive();
					else 
					{
						if (inWriting.body._size() > 0 || wrQueue.size() > 0)
							 done_ |= _processWrittenWebSocket();
					}
				}
				else
				{
					if(sendingPong == 2)
						done_ |= _processWritePong();
					else if (sendingKeepAlive == 2)
						done_ |= _processWrittenKeepAlive();
					else 
					{
						done_ |= _processWrittenWebSocket();
					}
				}
			}
			else if  (state == stateClosingConnection)
			{
				_processWriteClosingConnection();
			}
			else if (state == stateClosedConnection)
			{
				break; // do nothing
			}
			else
			{
				PASSERT3(0);
			}
		}
		else
		{
			done_ &= ~0x02;
			//read
			if (state == stateReceivingHeaders)
			{
				// !!! should not happen
				PASSERT3(0);
			}
			else if (state == stateSendingHeaders)
			{
				PASSERT3(0);
			}
			else if (state == stateWebSocket)
			{
				if(closingConnection == 0)
					done_ |= (_processReadWebSocket() && wrState == 0)?1:0;
			}
			else if (state == stateClosingConnection || state == stateClosedConnection)
			{
				break; // do nothing
			}
			else
			{
				PASSERT3(0);
			}
		}
	}
}

template< class _CompleteCommunicator,class _MsgHandler>
bool _CommWebSocketHandler<_CompleteCommunicator,_MsgHandler>::_keepAlive()
{

	if (state != stateWebSocket || closingConnection != 0)
		return true;
	if (sendingKeepAlive == 0)
	{
		if  (SYSTEMTICKS() -  keepAliveSent > KEEPALIVE_PERIOD)
		{
			PTRACE3("CommWebSocket[ %d ][ %d ](%s) set to send keepalive",mgrIdx, slot,address.c_str());
			sendingKeepAlive=1;
			if (wrState == 0)
			{
				done(1);
			}
		}
	}
	else if (sendingKeepAlive == 1 || sendingKeepAlive == 2)
	{
		if  (SYSTEMTICKS() -  keepAliveSent > (KEEPALIVE_PERIOD + KEEPALIVE_TIMEOUT))
		{
			// could not send keepalive for 20 seconds 
			PTRACE3("CommWebSocket[ %d ][ %d ](%s) could not send keepalive for %d seconds, last sent=%d, last received=%d",mgrIdx, slot,address.c_str(),KEEPALIVE_TIMEOUT/1000,SYSTEMTICKS() -  lastSent,SYSTEMTICKS() -  lastReceived);
			return false;
		}
	}
	else if (sendingKeepAlive == 3 || sendingKeepAlive == 4)
	{
		lastKeepAlive = SYSTEMTICKS() - keepAliveSent;
		if (lastKeepAlive > KEEPALIVE_TIMEOUT)
		{
			if (SYSTEMTICKS() -  lastReceived > KEEPALIVE_TIMEOUT/2)
			{
				PTRACE3("CommWebSocket[ %d ][ %d ](%s) lastKeepAlive=%d > %d, last sent=%d, last received=%d",mgrIdx, slot,address.c_str(),lastKeepAlive, KEEPALIVE_TIMEOUT, SYSTEMTICKS() -  lastSent,SYSTEMTICKS() -  lastReceived);
				return false;
			}
			else
			{
				if ( sendingKeepAlive == 3)
				{
					PTRACE3("CommWebSocket[ %d ][ %d ](%s) lastKeepAlive=%d > %d, but we are still receiving from peer, last sent=%d, last received=%d",mgrIdx, slot,address.c_str(),lastKeepAlive,KEEPALIVE_TIMEOUT, SYSTEMTICKS() -  lastSent,SYSTEMTICKS() -  lastReceived);
					sendingKeepAlive = 4;
				}
				return true;
			}
		}
	}
	return true;
}


template< class _CompleteCommunicator,class _MsgHandler>
bool _CommWebSocketHandler<_CompleteCommunicator,_MsgHandler>::serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
{

	if ( id == _SERVICE_REQUEST_KEEP_ALIVE)
	{
		return _keepAlive();
	}
	return false;
}



#endif
