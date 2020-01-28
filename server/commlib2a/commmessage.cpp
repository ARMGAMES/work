
#define _WIN32_WINNT  0x0400 
#include "commlib.h"
#include "commlzhl.h"


struct _cmsMessage
{
	CommMsgBody body;
	UINT32 msg;
	int number;
	bool wait;
	UINT32 repeat;
	PString parser;
	_cmsMessage()
	{
		number=0;
		msg=0;
		wait=false;
		repeat=0;
	}
};

struct _cmsConnection
{
	PString address;
	PString connType;
	PString destObject;
	bool pwd;
	bool sid;
	bool encr;
	int retry;
	list <_cmsMessage * > messages;
};




class ClientObject;
class ClientConn : public CommClientConnection
	{
	public:
	ClientObject* obj;
	list < _cmsMessage * > & messages;
	list < _cmsMessage * >::iterator itMessages;
	int retry;

	public:
	ClientConn( CommClientGuardFactory& guardFactory, list <_cmsMessage *> & messages_, int retry_ )
	: CommClientConnection( guardFactory ),
	messages(messages_)
		{
		retry = retry_;
		obj = 0;
		itMessages = messages.begin();
		setPriority(14);
		}
	void connected() override;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override;
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	};






class ClientObject : public CommConsoleClientObject
{
	public:
	static CommClientNullGuardFactory guardFactory;
	static CommClientPlainTextPasswordGuardFactory guardPwdNoSidFactory;
	static CommClientPlainTextPasswordSidGuardFactory guardPwdSidFactory;
	static CommClientEncryptedPwdGuardFactory guardEncrPwdSidFactory;

	private:
	CommClientConnectionPool cliPool;
	typedef PQueue< PQDeque< PString > > _Queue;
	_Queue q;//to wait on it only
	int numberOfConnections;

	public:

	ClientObject( CommConsoleClientInterface& inter, vector<_cmsConnection> & _cmsConnections )
	: CommConsoleClientObject( inter ),
	  cliPool( inter )
	{
		numberOfConnections=0;
		vector < _cmsConnection>::iterator it;
		for (it = _cmsConnections.begin();it != _cmsConnections.end();++it)
		{

			ClientConn * cl;
			if ((*it).pwd)
			{
				if((*it).encr)
				{
					cl = new ClientConn(guardEncrPwdSidFactory, (*it).messages,(*it).retry);
				}
				else
				{
					if ((*it).sid)
						cl = new ClientConn(guardPwdSidFactory, (*it).messages,(*it).retry);
					else
						cl = new ClientConn(guardPwdNoSidFactory, (*it).messages,(*it).retry);
				}
			}

			else
				cl = new ClientConn(guardFactory, (*it).messages,(*it).retry);
			cl->obj = this;
			cliPool.connect( *cl, (*it).address.c_str(), (*it).destObject.c_str(), (*it).connType.c_str() );
			numberOfConnections++;
		}
	}

	public:
	/* virtual */ void processMessage( _CommMsg& msg )
		{
			cliPool._processMessage( msg );
		}

		void exitObject( const char* err )
		{
			if (--numberOfConnections == 0) 
				q.push( PString( err ) );
		}

		void wait( PString& err )
		{
			int res = q.waitPop( err );
			if( res == _Queue::waitPopOk )
				return;
			err = "Queue::wait failed";
		}

}; 


void ClientConn::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	try
	{
		struct Condition
		{
			bool cond;
			bool inCondTrue;
			bool pass;
			Condition()
			{
				cond=false;
				inCondTrue=false;
				pass = false;
			}

		};
		struct Loop
		{
			bool inLoop;
			int  count;
			int  loopStart;
			Loop()
			{
				inLoop=false;
				loopStart=0;
				count=0;
			}
		};

		CommMsgParser* parser;
		CommMsgParser outParser(body);
		parser = &outParser;
		UINT32 reqId;
		parser->parseUINT32(reqId);
		list < _cmsMessage * >::iterator it;

		printf("Message N=%d (0x%x) received\n", msgId,msgId);
		for (it=messages.begin();it!=itMessages;++it)
		{
			if (reqId == (*it)->number)
			{
				CommMsgParser* inParser = 0;
				UINT32 count = 0;
				bool cond = false;

				_cmsMessage * ms = (*it);

				if (ms->repeat)
				{
					ms->repeat--;
					return;
				}
				vector<Condition> conditions;
				vector<Loop> loops;
				vector<CommMsgParser*> parsers;

				for (int i=0; i<ms->parser.length();i++)
				{
					if (conditions.size() && (conditions.back().pass == true || conditions.back().cond != conditions.back().inCondTrue) )
					{
						switch(ms->parser[i])
						{
						case '[':
							{
								conditions.push_back(Condition());
								conditions.back().inCondTrue = true;
								conditions.back().cond = cond;
								conditions.back().pass = true;
								break;
							}
						case ']':
							{
								if (conditions.size())
									conditions.pop_back();
								break;
							}
						case '|':
							{
								if (conditions.size())
								{
									conditions.back().inCondTrue = false;
								}
								break;
							}
						}
						continue;
					}

					if (loops.size() && loops.back().count == 0)
					{
						switch(ms->parser[i])
						{
						case '<':
							{
								loops.push_back(Loop());
								loops.back().loopStart = 0;
								loops.back().count = 0;
								break;
							}
						case '>':
							{
								if (loops.size())
								{
									loops.pop_back();
								}
								break;
							}
						}
						continue;
					}

					switch(ms->parser[i])
					{
					case '<':
						{
							loops.push_back(Loop());
							loops.back().loopStart = i;
							loops.back().count = count;
							break;
						}
					case '>':
						{
							if (loops.size())
							{
								if (loops.back().count > 0)
									--loops.back().count;
								if (loops.back().count > 0)
								{
									i = loops.back().loopStart;
								}
								else 
								{
									loops.pop_back();
								}
							}
							break;
						}
					case '[':
						{
							conditions.push_back(Condition());
							conditions.back().inCondTrue = true;
							conditions.back().cond = cond;
							break;
						}
					case ']':
						{
							if (conditions.size())
								conditions.pop_back();
							break;
						}
					case '|':
						{
							if (conditions.size())
							{
								conditions.back().inCondTrue = false;
							}
							break;
						}
					case '(':
						{
							if (!inParser)
								break;
							parsers.push_back(parser);
							parser = inParser;
							inParser = 0;
							printf("(\n");
							break;
						}
					case ')':
						{
							if (parsers.empty())
								break;
							delete parser;
							parser = parsers.back();
							parsers.pop_back();
							printf(")\n");
							break;
						}
					case 's':
					case 'S':
						{
							const char * string;
							parser->parseString(string);
							if (ms->parser[i+1] == '"') // save string to file
							{
								i += 2;
								const char* fileNameBegin = ms->parser + i;
								const char* fileNameEnd = strchr(fileNameBegin, '"');
								if (fileNameEnd != NULL)
								{
									i += (fileNameEnd - fileNameBegin) + 1;
									PString fileName(fileNameBegin, fileNameEnd);
									PLog("Saving '%s'", fileName.c_str());
									FILE* f = fopen(fileName, "wb");
									if (f)
									{
										fwrite(string, strlen(string), 1, f);
										fclose(f);
									}
								}
							}
							else
							{
								printf("s=%s\n",string);
							}
							cond = *string != 0;
							break;
						}
					case '8':
						{
							INT64 req;
							parser->parseINT64(req);
							PString bigInt;
							bigInt.appendInt64(req);
							printf("8=%s\n",bigInt.c_str());
							cond = req != 0;
							break;
						}
					case '4':
						{
							UINT32 req;
							parser->parseUINT32(req);
							printf("4=%d\n",req);
							cond = req != 0;
							count = req;
							break;
						}
					case '1':
						{
							BYTE req;
							parser->parseBYTE(req);
							printf("1=%c (%d)\n",(isprint(req)?req:' '),req);
							cond = req != 0;
							break;
						}
					case 'b':
					case 'B':
						{
							bool req;
							parser->parseBOOL(req);
							printf("b=%s\n", (req?"T":"F"));
							cond = req;
							break;
						}

					case '2':
						{
							INT16 req;
							parser->parseINT16(req);
							printf("2=%d\n",req);
							cond = req != 0;
							count = req;
							break;
						}
					case 'm':
					case 'M':
						{
							CommMsgBody* inBody = new CommMsgBody;
							parser->parseMsgBody(*inBody);
							inParser = new CommMsgParser(*inBody);
							printf("# MsgBody: parsed\n");
							break;
						}
					case 'v':
					case 'V':
						{

							const BYTE * ptr;
							size_t size;
							parser->_parseVarBlock(ptr,size);
							printf("# VarBlock: size=%d\n",size);
							if (ms->parser[i] == 'V')
							{
								printf("# VarBlock: data=");
								PString ret;
								size_t sz = size;
								const BYTE *p = ptr;
								if (ms->parser[i+1] == 'e') // encode
								{
									i++;
									CommSSLBase64::encode(ret, ptr,size);
									p=(const BYTE *)ret.c_str();
									sz=ret.length();
								}
								for (int j = 0; j < sz; ++j)
								{
									putchar(p[j]);
								}
								printf("\n");
							}
							else if (ms->parser[i+1] == '"') // save block to file
							{
								i += 2;
								const char* fileNameBegin = ms->parser + i;
								const char* fileNameEnd = strchr(fileNameBegin, '"');
								if (fileNameEnd != NULL)
								{
									i += (fileNameEnd - fileNameBegin) + 1;
									PString fileName(fileNameBegin, fileNameEnd);
									PLog("Saving '%s'", fileName.c_str());
									FILE* f = fopen(fileName, "wb");
									if (f)
									{
										fwrite(ptr, size, 1, f);
										fclose(f);
									}
								}
							}
							
							break;
						}
					case 't':
					case 'T':
						{
							CommSrvTime req;
							parser->parseSrvTime(req);
							printf("T=%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d\n",req._year,req._month,req._day,req._hour,req._minute,req._second);
							break;
						}

					case 'd':
					case 'D':
						{
							CommSrvDate req;
							parser->parseSrvDate(req);
							printf("D=%4.4d/%2.2d/%2.2d\n",req._year,req._month,req._day);
							break;
						}
					default:
						PASSERT(0);
				
					}
				}

				messages.erase(it);
				break;
			}
		}
		if (messages.begin() == messages.end())
			obj->exitObject("");

		if (itMessages == messages.begin())
		{
			while (itMessages != messages.end())
			{
				CommMsgBody body;
				body.composeUINT32((*itMessages)->number);
				body.merge((*itMessages)->body);
				post( (*itMessages)->msg,body);
				++itMessages;
				if (itMessages == messages.end() || (*itMessages)->wait)
					break;
			}
		}
	}
	catch(...)
	{
		obj->exitObject("");
	}
}

void ClientConn::connected()
{
	itMessages = messages.begin();
	if (itMessages == messages.end())
	{
		obj->exitObject("");
	}
	while (itMessages != messages.end())
	{
		_cmsMessage * sss = (*itMessages);
		if (sss->repeat)
		{
			for (int i=0;i<=sss->repeat;i++)
			{
				CommMsgBody body;
				body.composeUINT32(sss->number);
				body.merge(sss->body);
				post( sss->msg,body);
			}
			++itMessages;
		}
		else
		{
			CommMsgBody body;
			body.composeUINT32((*itMessages)->number);
			body.merge((*itMessages)->body);
			post( (*itMessages)->msg,body);
			++itMessages;
			if (itMessages == messages.end() || (*itMessages)->wait)
				break;
		}
	}
}

bool ClientConn::closedOrDisconnected( int errCode, const char* errMsg ) 
{
	PTRACE( "connection closed: %d '%s'", errCode, errMsg );
	if (retry)
	{
		retry--;
		if (retry == 0)
		{
			obj->exitObject("");
		}
		return retry == 0;
	}
	return false;
}



/* static */ CommClientNullGuardFactory ClientObject::guardFactory;
			 CommClientPlainTextPasswordGuardFactory ClientObject::guardPwdNoSidFactory;
			 CommClientPlainTextPasswordSidGuardFactory ClientObject::guardPwdSidFactory;
			 CommClientEncryptedPwdGuardFactory ClientObject::guardEncrPwdSidFactory;

void readMessageFields( char * buffer, CommMsgBody & body,FILE *f)
{
		if (!strnicmp(buffer,"s=",2))
		{
			char * p;
			p = strchr(buffer,'=');
			char * pp = p+1;
			while(*pp)
			{
				if (*pp == '\\' && *(pp+1) == 'n')
				{
					*pp = '\xd';
					*(pp+1) = '\xa';
					pp+=2;
				}
				else
				{
					pp++;
				}
			}
			body.composeString(p+1);
		}
		else if (!strnicmp(buffer,"e=",2))
		{
			char * p;
			p = strchr(buffer,'=');
			char * pp = p+1;
			PBlock msgBlock;
			CommSSLBase64::decode( msgBlock, pp);
			body._composeVarBlock( msgBlock.ptr(), msgBlock.size() );
		}

		else if (!strnicmp(buffer,"sfile=",6))
		{
			char * p;
			p = strchr(buffer,'=');
			char * pp = p+1;
			PString content;
			int handle=open(pp,_O_BINARY|_O_RDONLY);
			if (handle != -1)
			{
				char buff[4000];
				memset(buff,0,sizeof(buff));
				while(read(handle,buff,sizeof(buff)-1)>0)
				{
					content.append(buff);
					memset(buff,0,sizeof(buff));
				}
				close(handle);
				body.composeString(content);
			}
		}
		else if (!strnicmp(buffer,"vfile=",6))
		{
			PIOFile rsaFile; 
			char * filename = strchr(buffer,'=') + 1;
			rsaFile.openFile(filename,PLIB_FILE_READONLY );
			PBlock content(rsaFile.getSize());
			rsaFile.readFile(content.ptr(), content.size());
			body._composeVarBlock(content.ptr(),content.size());
		}

		else if (!strnicmp(buffer,"8=",2))
		{
			const char * p;

			p = strchr(buffer,'=');
			if (!strnicmp(p+1,"0x",2))
			{
				UINT64 num;
				num=p_atohex64(p+3);
				body.composeUINT64(num);
			}
			else
			{
				INT64 num = p_atoi64(p+1);
				body.composeINT64(num);
			}
	
		}
		else if (!strnicmp(buffer,"4=",2))
		{
			const char * p;
			char *p1;

			p = strchr(buffer,'=');
			if (!strnicmp(p+1,"0x",2))
			{
				unsigned int num;
				num=strtoul(p+3,&p1,16);
				body.composeUINT32(num);
			}
			else
			{
				int num = atoi(p+1);
				body.composeINT32(num);
			}
	

		}
		else if (!strnicmp(buffer,"1=",2))
		{
			const char * p;
			p = strchr(buffer,'=');
			BYTE b;
			if (!strnicmp(p+1,"0x",2))
			{
				char *p1;
				b=(BYTE) strtol(p+3,&p1,16);
			}
			else
			{
				b = *(BYTE *)(p+1);
			}
			body.composeBYTE(b);
		}
		else if (!strnicmp(buffer,"2=",2))
		{
			const char * p;
			char *p1;
			int num;
			p = strchr(buffer,'=');
			if (!strnicmp(p+1,"0x",2))
			{
				num=strtol(p+3,&p1,16);
			}
			else
			{
				num = atoi(p+1);
			}
			body.composeUINT16(num);
		}
		else if (!strnicmp(buffer,"b=",2))
		{
			const char * p;
			p = strchr(buffer,'=');
			bool b;
			if (*(p+1) == 'T' || *(p+1) == 't')
				b = true;
			else if (*(p+1) == 'F' || *(p+1) == 'f')
				b = false;
			else
				b = atoi(p+1) != 0;
			body.composeBOOL(b);
		}

		else if (!strnicmp(buffer,"t=",2))
		{
			const char * p;
			CommSrvTime t;
			p = strchr(buffer,'=');
			int Year,Month,Day,hour,minute,sec;
			sscanf(p+1,"%d/%d/%d %d:%d:%d",&Year,&Month,&Day,&hour,&minute,&sec);
			t._year = Year;
			t._month = Month;
			t._day = Day;
			t._hour = hour;
			t._minute = minute;
			t._second = sec;
			body.composeSrvTime(t);
		}
		else if (!strnicmp(buffer,"d=",2))
		{
			const char * p;
			CommSrvDate t;
			p = strchr(buffer,'=');
			int Year,Month,Day;
			sscanf(p+1,"%d/%d/%d",&Year,&Month,&Day);
			t._year = Year;
			t._month = Month;
			t._day = Day;
			body.composeSrvDate(t);
		}
		else if (!strnicmp(buffer,"m=",2))
		{
			CommMsgBody newBody;
			for (;;)
			{
				char buf [4000];
				if (f)
				{
					if (!fgets(buf,sizeof(buf),f))
						break;
				}
				else
				{
					if (!gets(buf))
						break;
				}
				char * p_newl = strpbrk(buf,"\n\a");
				if (p_newl)
					*p_newl=0;
				if (!strnicmp(buf,"end",3))
					break;
				readMessageFields(buf,newBody,f);
			}
			body.composeMsgBody(newBody);
		}
}

void assignCheckSlash(PString &ret, const char * start, const char * end = 0)
{
	while(*start && start != end)
	{
		if (*start == '/')
		{
			ret.append('/');
			if (start[1] == '/')
			{
				start+=2;
			}
			else
			{
				start++;
			}
		}
		else
		{
			ret.append(*start);
			start++;
		}
	}
}


void readIniFile(vector<_cmsConnection> & connections, const char * fileName) 
{

	_cmsConnection * currC = 0;
	_cmsMessage    * currM = 0;
	int currMNumber=1;
	FILE *f=0;
	if (fileName)
	{
		f = fopen(fileName,"r");
		if (!f)
			throw PError("error open file");
	}

	while(1)
	{
		char buffer [4000];
		if (f)
		{
			if (!fgets(buffer,sizeof(buffer),f))
				break;
		}
		else
		{
			if (!gets(buffer))
				break;
		}
		char * p_newl = strpbrk(buffer,"\n\a");
		if (p_newl)
			*p_newl=0;

		if (!strnicmp(buffer,"connection=",11) ||
		    !strnicmp(buffer,"aconnection=",12) ||
			!strnicmp(buffer,"asconnection=",13) ||
			!strnicmp(buffer,"aseconnection=",14))
		{
			// ip:1.1.1.1:1234/objectname/objecttype
			bool pwd=false;
			bool sid = false;
			bool encr= false;
			if (!strnicmp(buffer,"aconnection=",12))
			{
				char buffer_pwd[100];
				char buffer_usr[100];
				printf("User:");
				gets(buffer_usr);
				printf("Password:");
				int i=0;
				int ch;
				for (i=0;i<100 && (ch=getch()) && ch != 13;i++)
					buffer_pwd[i] = ch;
				buffer_pwd[i]=0;
				CommMsgBody body;
				body.composeString("127.0.0.1");
				ClientObject::guardPwdNoSidFactory.setAuth( buffer_usr, buffer_pwd, &body);
				pwd=true;
			}
			else if (!strnicmp(buffer,"asconnection=",13))
			{
				char buffer_pwd[100];
				char buffer_usr[100];
				printf("User:");
				gets(buffer_usr);
				printf("Password:");
				int i=0;
				int ch;
				for (i=0;i<100 && (ch=getch()) && ch != 13;i++)
					buffer_pwd[i] = ch;
				buffer_pwd[i]=0;

				char buffer_sid[100];
				printf("\nSID:");
				gets(buffer_sid);

				CommMsgBody body;
				body.composeString("127.0.0.1");
				ClientObject::guardPwdSidFactory.setAuth( buffer_usr,buffer_pwd,buffer_sid, &body);
				pwd=true;
				sid = true;
			}
			else if (!strnicmp(buffer,"aseconnection=",14))
			{
				char buffer_pwd[100];
				char buffer_usr[100];
				printf("User:");
				gets(buffer_usr);
				printf("Password:");
				int i=0;
				int ch;
				for (i=0;i<100 && (ch=getch()) && ch != 13;i++)
					buffer_pwd[i] = ch;
				buffer_pwd[i]=0;

				char buffer_sid[100];
				printf("\nSID:");
				gets(buffer_sid);

				CommMsgBody body;
				body.composeString("127.0.0.1");
				ClientObject::guardEncrPwdSidFactory.setAuth( buffer_usr,buffer_pwd,buffer_sid, &body);
				pwd=true;
				sid = true;
				encr=true;
			}

			currM=0;
			currMNumber=1;
			PString address;
			PString objectname;
			PString objecttype;
			const char * p, *p1, *p2;
			p = strchr(buffer,'=');
			if (!p) 
				throw PError("Connection - illegal format");
			p++;
			p1=p;
			while(1)
			{
				p1 = strchr(p1,'/');
				if (p1[1] == '/')
					p1+=2;
				else
					break;
			}

			if (!p1) 
				throw PError("Connection - illegal format");
			p1++;
			p2=p1;
			while(1)
			{
				p2 = strchr(p2,'/');
				if (p2[1] == '/')
					p2+=2;
				else
					break;
			}
			if (!p2) 
				throw PError("Connection - illegal format");
			p2++;
			assignCheckSlash(address,p,p1-1);
			assignCheckSlash(objectname,p1,p2-1);
			if (*p2)
				assignCheckSlash(objecttype,p2);
			currC = &*connections.insert(connections.end(),_cmsConnection());
			currC->address = address;
			currC->destObject = objectname;
			currC->connType = objecttype;
			currC->pwd=pwd;
			currC->sid=sid;
			currC->encr=encr;
		}
		else if (!strnicmp(buffer,"message=",8))
		{
			if (currC == 0)
				throw PError("Message without connection");
			const char * p, *p1;
			p = strchr(buffer,'=');
			if (!p) 
				throw PError("Message - illegal format");
			p++;
			int mess = atoi(p);
			bool wait = false;
			if ((p1 = strchr(p,'/')) && !strnicmp(p1+1,"wait",4))
				wait=true;
			currM = new _cmsMessage();
			currC->messages.push_back(currM);
			currM->msg=mess;
			currM->wait = wait;
			currM->number = currMNumber++;
		}
		else if (!strnicmp(buffer,"repeat=",7))
		{
			if (currM == 0)
				throw PError("Field without message");
			const char * p;
			p = strchr(buffer,'=');
			if (!p) 
				throw PError("repeat - illegal format");
			p++;
			currM->repeat = atoi(p);
		}
		else if (!strnicmp(buffer,"retry=",6))
		{
			if (currC == 0)
				throw PError("retry without connection");
			const char * p;
			p = strchr(buffer,'=');
			if (!p) 
				throw PError("retry - illegal format");
			p++;
			currC->retry = atoi(p);
		}
		else if (!strnicmp(buffer,"parser=",7))
		{
			if (currM == 0)
				throw PError("Field without message");
			const char * p;
			CommSrvTime t;
			p = strchr(buffer,'=');
			currM->parser.assign(p+1);
		}
		else if (!strnicmp(buffer,"s=",2) ||
				 !strnicmp(buffer,"8=",2) ||
				 !strnicmp(buffer,"e=",2) ||
				 !strnicmp(buffer,"4=",2) ||
				 !strnicmp(buffer,"1=",2) ||
				 !strnicmp(buffer,"b=",2) ||
				 !strnicmp(buffer,"2=",2) ||
				 !strnicmp(buffer,"t=",2) ||
				 !strnicmp(buffer,"d=",2) ||
				 !strnicmp(buffer,"sfile=",6) ||
				 !strnicmp(buffer,"vfile=",6) ||
				 !strnicmp(buffer,"m=",2))
		{
			if (currM == 0)
				throw PError("Field without message");
			readMessageFields(buffer,currM->body,f);
		}
	}
	if (f)
	{
		fclose(f);
	}
} 


int main(int npar, char * param[])
{


	PLogInit( new PLogFile( "commesage.log", PLogFile::alsoToStdout | PLogFile::printThreadId | PLogFile::perDate /*PLogFile::perDateLimited*/ ) );
	PLog( "Starting... %d",time(0) );
	PSocket::startup();


	try 
	{
		{
			CommRoutingTable theRoutingTable;
			vector <_cmsConnection> connections;
			readIniFile(connections, npar>1?param[1]:0);
			CommRule rule;

			rule.mask = "ip:*:*";
			if (npar > 2)
				rule.conns.push_back( param[2] );
			else
				rule.conns.push_back( "pyrtcp0://$0:$1" );
			theRoutingTable.addRule( rule );


			theRoutingTable.setOption( "maxfailures", "10" );
			CommClientThreadManager simple( theRoutingTable );
			CommConsoleClientThreadManager console( theRoutingTable );
			CommConsoleClientInterface& inter = console.createInterface();
			ClientObject obj( inter, connections);
			console.registerObject( obj );
            simple.setOption("compressbits","0");
			simple.setOption("tcpconnect","5");

			theRoutingTable.start();
			PString err;
			obj.wait(err);
			theRoutingTable.shutdown();
			bool Ok = theRoutingTable.waitUntilTerminated( 5 );
			if( Ok )
				PLog( "main: Threads terminated" );
			else
				PLog( "main: Threads not terminated - exiting anyway" );
		}
		PSocket::cleanup();

	}
	catch( PError& err )
	{
		PLog( "Error in main: '%s'\n", err.why() );
	}
	return 0;
}

