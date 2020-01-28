#ifndef comextjson_h_included
#define	comextjson_h_included

#include "commext.h"
#include "ppcontainer.h"
#include "ppi18n_utf8.h"

class CommExtJson
{

public:
	PString name;
	PString s;
	int     i;
	bool    b;
	vector<CommExtJson*> v;
	int type; //1-string 2-int 3- bool 4-map 5-vector 6- null;
	CommExtJson()
	{
		i=0;
		b=false;
		type=0;
	}
	~CommExtJson()
	{
		for (int i=0; i< v.size(); i++)
		{
			delete v[i];
		}
		v.clear();
	}

	static void getString(PString &ret, const char *&p)
	{
		PASSERT3(*p=='"');
		p++;
		while(1)
		{
			PASSERT3(*p && "End of string");
			if (*p == '"')
			{
				p++;
				return;
			}
			if (*p == '\\')
			{
				p++;
				PASSERT3(*p && "End of string");
				switch(*p)
				{
				case '"': ret.append('"');p++;break;
				case '/': ret.append('/');p++;break;
				case '\\': ret.append('\\');p++;break;
				case 'b': ret.append('\b');p++;break;
				case 'f': ret.append('\f');p++;break;
				case 'n': ret.append('\n');p++;break;
				case 'r': ret.append('\r');p++;break;
				case 't': ret.append('\t');p++;break;
				case 'u':
					{
						p++;
						PUNICHAR code=0;
						for (int i=0;i<4;i++,p++)
						{
							code *=16;
							if (*p >='0' && *p<='9')
								code += *p - '0';
							else if (*p >='A' && *p<='F')
								code += *p - 'A' + 10;
							else if (*p >='a' && *p<='f')
								code += *p - 'a' + 10;
							else
								PASSERT3(0 && "Incorrect \\u value");
						}
						PUtf8String::staticAppend( ret, code );
					}
					break;
				default:
					PASSERT3(0 && "Invalid symbol");
				}
			}
			else
			{
				ret.append(*p);
				p++;
			}
		}
	}
	static const char * putString(PString &ret, const char *p)
	{
		ret.append('"');
		while(1)
		{
			PUNICHAR_RET s = PUtf8String::_nextChar(p);
			if (s<=0)
			{
				ret.append('"');
				return ret.c_str();
			}
			if (s == 0x10)
			{
				; // ignore
			}
			else if (s == '"')
			{
				ret.append("\\\"");
			}
			else if (s =='/')
			{
				ret.append("\\/");
			}
			else if (s =='\f')
			{
				ret.append("\\f");
			}
			else if (s =='\n')
			{
				ret.append("\\n");
			}
			else if (s =='\r')
			{
				ret.append("\\r");
			}
			else if (s =='\t')
			{
				ret.append("\\t");
			}
			else if (s>255)
			{
				s&=0xffff;
				ret.append("\\u");
				for (int i=12;i>=0;i-=4)
				{
					static char t[]="0123456789abcdef";
					ret.append(t[ (s>>i)&0xf ]);
				}

			}
			else
				ret.append((char)(s & 0xff));
		}
	}

	static CommExtJson * getNode(CommExtJson * start, int *path, int len)
	{
		while(len)
		{
			PASSERT3(start->v.size() > *path);
			start=start->v[*path];
			path++;
			len--;
		}
		return start;
	}
	static CommExtJson * findNode(CommExtJson * start, const char * toFind, int *path, int &len)
	{
		if (start->v.size())
		{
			len++;
			for (int i=0;i<start->v.size();i++)
			{
				*path=i;
				if (start->v[i]->name.equals(toFind))
				{
					return start->v[i];
				}
				else
				{
					CommExtJson *r;
					if(r = findNode(start->v[i],toFind,path+1,len))
						return r;
				}
			}
			len--;
		}
		return 0;
	}

	void cleanSpace(const char *&p)
	{
		while(*p)
		{
			if (*p == ' ' || *p== '\t' || *p == '\r' || *p == '\n')
				p++;
			else
				break;
		}
	}
	void load(const char *&p)
	{
		cleanSpace(p);
		if (*p == '\"')
		{
			type=1;
			getString(s,p);
		}
		else if (*p>='0' && *p <='9')
		{
			type=2;
			i=atoi(p);
			while(*p>='0' && *p <='9')
				p++;
			return;
		}
		else if (*p=='t' || *p == 'f')
		{
			type=3;
			if (!strncmp(p,"true",4))
			{
				b=true;
				p+=4;
			}
			else if (!strncmp(p,"false",5))
			{
				b=false;
				p+=5;
			}
			else
				PASSERT3(0 && "Illigal bool value");

			return;
		}
		else if (!strncmp(p,"null",4))
		{
			type=6;
			p+=4;
			return;
		}

		else if (*p == '{')
		{
			type = 4;
			p++;
			while(*p)
			{
				cleanSpace(p);
				if (*p == '\"')
				{
					CommExtJson *json = new CommExtJson();
					getString(json->name,p);
					v.push_back(json);
					cleanSpace(p);
					PASSERT3(*p==':');
					p++;
					json->load(p);
					cleanSpace(p);
				}
				if (*p == '}')
				{
					p++;
					return;
				}
				else if (*p ==',')
					p++;
				else
					PASSERT3(0 && "Invalid structure");
			}
		}
		else if (*p == '[')
		{
			type = 5;
			p++;
			while(*p)
			{
				cleanSpace(p);
				if (*p == ']')
				{
					p++;
					return;
				}
				else if (*p ==',')
					p++;
				else
				{
					CommExtJson *json = new CommExtJson();
					v.push_back(json);
					json->load(p);
					cleanSpace(p);
				}
			}
		}
	
	}
};




#endif