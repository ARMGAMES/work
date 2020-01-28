#include "commextedbmemail.h"



void Email::compose(CommMsgBody& body) const
{
	body.composeUINT32(status);
	if (status & eSource)
	{
		body.composeString(source.c_str());
	}
	if (status & eEmailId)
		body.composeUINT32(emailId);
	if (status & eReceived)
		received.compose(body);
	if (status & eSent)
		sent.compose(body);
	if (status & eFrom)
		from.compose(body);

	if (status & eReplyTo)
		replyTo.compose(body);

	if (status & eSubject)
		body.composeString(subject.c_str());

	if (status & eUserOwner)
		body.composeString(userOwner.c_str());

	if (status & eHasAttachments)
		body.composeUINT32(hasAttachments);
	if (status & eState)
		body.composeUINT32(state);
	if (status & eIncomingType)
		body.composeUINT32(incomingType);
	if (status & eTransId)
		body.composeUINT32(transId);
	if (status & eSpecial)
		body.composeUINT32(special);
	if (status & eFlag)
		body.composeUINT32(flag);
	if (status & eMailBox)
		body.composeString(mailBox.c_str());
	if (status & ePrivateEmailBox)
		body.composeString(privateEmailBox.c_str());

	if (status & eProcessingError)
		body.composeUINT32(processingError);

	int i;
	body.composeUINT32(addresses.size());
	for (i=0;i<addresses.size();i++)
	{
		addresses[i].compose(body);
	}
	body.composeUINT32(bodies.size());
	for (i=0;i<bodies.size();i++)
	{
		bodies[i].compose(body);
	}
	body.composeUINT32(attachments.size());
	for (i=0;i<attachments.size();i++)
	{
		attachments[i].compose(body);
	}
	body.composeUINT32(notes.size());
	for (i=0;i<notes.size();i++)
	{
		notes[i].compose(body);
	}
	body.composeUINT32(headers.size());
	for (i=0;i<headers.size();i++)
	{
		headers[i].compose(body);
	}
}

void Email::composeGenerated(CommMsgBody& body, const char * mailBox_,  const char * dataMailBox_, UINT32 emailId_, const char * userOwner_, vector<UINT32> & bodyIds)
{
	UINT32 status_ = status;
	status_ &= ~(eSource|eProcessingError);
	status_ |= (eEmailId|eIncomingType|eMailBox|ePrivateEmailBox|eState|eFlag|eUserOwner);
	body.composeUINT32(status_);
	body.composeUINT32(emailId_);
	if (status & eReceived)
		received.compose(body);
	if (status & eSent)
		sent.compose(body);
	if (status & eFrom)
		from.compose(body);

	if (status & eReplyTo)
		replyTo.compose(body);

	if (status & eSubject)
		body.composeString(subject.c_str());

	body.composeString(userOwner_);

	if (status & eHasAttachments)
		body.composeUINT32(hasAttachments);

	body.composeUINT32(edbm_StateNew);
	body.composeUINT32(edbm_emailReceived);

	if (status & eTransId)
		body.composeUINT32(transId);
	if (status & eSpecial)
		body.composeUINT32(special);

	body.composeUINT32(edbm_emailNew);

	body.composeString(mailBox_);
	body.composeString(dataMailBox_);

	int i;
	UINT32 addrCounter = 0;
	for (i=0;i<addresses.size();i++)
	{
		if (addresses[i].type != CommMail::addrBcc)
			addrCounter++;
	}

	body.composeUINT32(addrCounter);
	for (i=0;i<addresses.size();i++)
	{
		if (addresses[i].type != CommMail::addrBcc)
			addresses[i].compose(body);
	}
	body.composeUINT32(bodies.size());
	for (i=0;i<bodies.size();i++)
	{
		if (bodyIds.size() == bodies.size())
			bodies[i].composeGeneric(body,bodyIds[i]);
		else
			bodies[i].composeGeneric(body,i);
	}
	UINT32 counter = 0;
	for (i=0;i<attachments.size();i++)
		if (!(attachments[i].status & EmailAttachment::eSecureAttachment))
			counter++;

	body.composeUINT32(counter);
	for (i=0;i<attachments.size();i++)
	{
		if (!(attachments[i].status & EmailAttachment::eSecureAttachment))
			attachments[i].composeGenerated(body);
	}
	body.composeUINT32(0); //notes
	body.composeUINT32(0); //headers
}


void Email::parse(CommMsgParser & parser)
{
	parser.parseUINT32(status);
	if (status & eSource)
	{
		parser.parseStringP(source);
	}
	if (status & eEmailId)
		parser.parseUINT32(emailId);
	else
		emailId=0;
	if (status & eReceived)
		received.parse(parser);
	else
		received.setNull();
	if (status & eSent)
		sent.parse(parser);
	else
		sent.setNull();

	if (status & eFrom)
		from.parse(parser);

	if (status & eReplyTo)
		replyTo.parse(parser);

	if (status & eSubject)
		parser.parseStringP(subject);
	else
		subject="";

	if (status & eUserOwner)
		parser.parseStringP(userOwner);
	else
		userOwner="";

	if (status & eHasAttachments)
		parser.parseUINT32(hasAttachments);
	else
		hasAttachments=0;

	if (status & eState)
		parser.parseUINT32(state);
	else
		state=0;

	if (status & eIncomingType)
		parser.parseUINT32(incomingType);
	else
		incomingType=0;

	if (status & eTransId)
		parser.parseUINT32(transId);
	else
		transId=0;

	if (status & eSpecial)
		parser.parseUINT32(special);
	else
		special=0;

	if (status & eFlag)
		parser.parseUINT32(flag);
	else
		flag=0;

	if (status & eMailBox)
		parser.parseStringP(mailBox);
	else
		mailBox="";

	if (status & ePrivateEmailBox)
		parser.parseStringP(privateEmailBox);
	else
		privateEmailBox="";

	if (status & eProcessingError)
		parser.parseUINT32(processingError);
	else
		processingError=0;

	int i;
	UINT32 count;
	parser.parseUINT32(count);
	for (i=0;i<count;i++)
	{
		EmailAddress & addr = *addresses.insert(addresses.end(),EmailAddress());	
		addr.parse(parser);
	}
	parser.parseUINT32(count);
	for (i=0;i<count;i++)
	{
		EmailBody & bd = *bodies.insert(bodies.end(),EmailBody());	
		bd.parse(parser);
	}
	parser.parseUINT32(count);
	for (i=0;i<count;i++)
	{
		EmailAttachment & att = *attachments.insert(attachments.end(),EmailAttachment());	
		att.parse(parser);
	}
	parser.parseUINT32(count);
	for (i=0;i<count;i++)
	{
		EmailNote & nt = *notes.insert(notes.end(),EmailNote());	
		nt.parse(parser);
	}
	parser.parseUINT32(count);
	for (i=0;i<count;i++)
	{
		EmailHeader & hd = *headers.insert(headers.end(),EmailHeader());	
		hd.parse(parser);
	}
}

void Email::loadFromPOP3()
{
	PTRACE5("loadFromPOP3, status=%u", status); 
	PASSERT3(status & eSource);
	bodies.clear();// just in case
	attachments.clear();
	addresses.clear();
	notes.clear();
	headers.clear();
	if (email)
		delete email;
	email = new CommMail();
	try
	{
		email->load((const BYTE*)source.c_str(), source.length());
	}
	catch( PError& er )
	{
		delete email;
		email = new CommMail();
		email->addAddress(CommMail::addrTo,"Error");
		email->setFrom("Error","Error");
		email->setSubject("Error parsing email");
		email->addBody(er.why());
		email->setDate(plib_time(0));
		status |= eProcessingError;
		processingError = eParsingError;

	}

	received.currentLocalTime();
	status |= eReceived;
	time_t t = email->date();
	if (t)
		sent.decode(t);
	else
		sent.currentLocalTime();
	status |= eSent;

	from.address = email->from().addr;
	from.status = EmailAddress::eAddressSet;
	from.normalAddress = email->from().addrNormalized;
	from.status |= EmailAddress::eNormalAddressSet;
	from.type = edbm_ReplyTo;
	from.status |= EmailAddress::eTypeSet;

	status |= eFrom;
	if (*email->replyTo().addrNormalized && !from.normalAddress.equals(email->replyTo().addrNormalized.c_str()))
	{
		replyTo.address = email->replyTo().addr;
		replyTo.status = EmailAddress::eAddressSet;
		replyTo.normalAddress = email->replyTo().addrNormalized;
		replyTo.status |= EmailAddress::eNormalAddressSet;
		replyTo.type = edbm_ReplyTo;
		replyTo.status |= EmailAddress::eTypeSet;
		status |= eReplyTo;
	}

	subject = email->subj();
	status |= eSubject;

	int i;
	for (i=0;i<email->addresses().size();i++)
	{
		EmailAddress & addr = *addresses.insert(addresses.end(),EmailAddress());	
		addr.address = email->addresses()[i].addr;
		addr.status = EmailAddress::eAddressSet;
		addr.normalAddress = email->addresses()[i].addrNormalized;
		addr.status |= EmailAddress::eNormalAddressSet;
		addr.type = email->addresses()[i].addrType;
		addr.status |= EmailAddress::eTypeSet;
	}
	for (i=0;i<email->headers().size();i++)
	{
		EmailHeader & hd = *headers.insert(headers.end(),EmailHeader());	
		hd.name = email->headers()[i].name;
		hd.value = email->headers()[i].value;
	}
	for (i=0;i<email->bodies().size();i++)
	{
		EmailBody & bd = *bodies.insert(bodies.end(),EmailBody());	
		bd.contentType = email->bodies()[i].contentType;
		bd.status = EmailBody::eContentType;
		bd.charset = email->bodies()[i].charset;
		bd.status |= EmailBody::eCharset;
		bd.encoding = email->bodies()[i].encoding;
		if ( bd.encoding != 0 )
			bd.status |= EmailBody::eEncoding;
		bd.emailbody = email->bodies()[i].str.c_str();
		bd.status |= EmailBody::eBody;
		bd.ordinal = i;
		bd.status |= EmailBody::eOrdinal;
	}
	for (i=0;i<email->attachments().size();i++)
	{
		EmailAttachment & at = *attachments.insert(attachments.end(),EmailAttachment());	
		at.contentType = email->attachments()[i].contentType;
		at.status = EmailAttachment::eContentType;
		at.charset = email->attachments()[i].charset;
		at.status |= EmailAttachment::eCharset;
		at.name = email->attachments()[i].name;
		at.status |= EmailAttachment::eName;

		if (!email->attachments()[i].isBinary)
		{
			memcpy( at.emailbody.alloc( email->attachments()[i].str.length() ), email->attachments()[i].str.c_str(), email->attachments()[i].str.length() );
			at.textAttachment=true;
		}
		else
		{
			at.emailbody = email->attachments()[i].block;
			at.textAttachment=false;
		}
		at.status |= EmailAttachment::eBody;
		at.ordinal = i;
		at.status |= EmailAttachment::eOrdinal;
	}
	hasAttachments = email->attachments().size();
	status |= eHasAttachments;
	incomingType = edbm_emailReceived;
	status |= eIncomingType;
	special = edbm_emailNoSpecial;
	status |= eSpecial;
	flag = edbm_emailNew;
	status |= eFlag;
	state = edbm_StateNew;
	status |= eState;

	delete email;
	email=0;
}

void Email::composeForEmail( CommMsgBody& body )
{
	int i;
	body.composeString(from.normalAddress.c_str());
	for (i=0;i<addresses.size();i++)
	{
		PString addr;
		if (addresses[i].type == CommMail::addrTo)
			body.composeString(addresses[i].normalAddress.c_str());
		else if (addresses[i].type == CommMail::addrCc)
		{
			addr.append("cc:").append(addresses[i].normalAddress.c_str());
			body.composeString(addr);
		}
		else if (addresses[i].type == CommMail::addrBcc)
		{
			addr.append("bcc:").append(addresses[i].normalAddress.c_str());
			body.composeString(addr);
		}
	}
	body.composeString("");
	body.composeString(subject);
	size_t numAttachments = 0;
	if (bodies.size())
	{
		body.composeString(bodies[0].emailbody.c_str());
		numAttachments = bodies.size() - 1;
	}
	else
	{
		body.composeString("");
	}
	numAttachments += attachments.size();
	PASSERT3(numAttachments < 256);
	body.composeBYTE((BYTE)numAttachments);
	for (i=1;i<bodies.size();i++)
	{
		body.composeBYTE(0x00)
		    .composeString(bodies[i].contentType.c_str())
			.composeString("")
			.composeString(bodies[i].emailbody.c_str());

	}
	for (i=0;i<attachments.size();i++)
	{
		body.composeBYTE(attachments[i].textAttachment?0x01:0xFF)
		    .composeString(attachments[i].contentType.c_str())
			.composeString(attachments[i].name.c_str());
		if (attachments[i].textAttachment)
		{
			body.composeString((const char *)attachments[i].emailbody.ptr());
		}
		else
		{
			body._composeVarBlock(attachments[i].emailbody.ptr(),attachments[i].emailbody.size());
		}
	}
}

void Email::composeForEmailWithCharset( CommMsgBody& body )
{
	int i;
	body.composeString(from.normalAddress.c_str());
	for (i=0;i<addresses.size();i++)
	{
		PString addr;
		if (addresses[i].type == CommMail::addrTo)
			body.composeString(addresses[i].normalAddress.c_str());
		else if (addresses[i].type == CommMail::addrCc)
		{
			addr.append("cc:").append(addresses[i].normalAddress.c_str());
			body.composeString(addr);
		}
		else if (addresses[i].type == CommMail::addrBcc)
		{
			addr.append("bcc:").append(addresses[i].normalAddress.c_str());
			body.composeString(addr);
		}
	}
	body.composeString("");
	body.composeString(subject);
	size_t numAttachments = 0;
	if (bodies.size())
	{
		body.composeString(bodies[0].charset.c_str())
			.composeString(bodies[0].emailbody.c_str());
		numAttachments = bodies.size() - 1;
	}
	else
	{
		body.composeString("")
			.composeString("");
	}
	numAttachments += attachments.size();
	PASSERT3(numAttachments < 256);
	body.composeBYTE((BYTE)numAttachments);
	for (i=1;i<bodies.size();i++)
	{
		body.composeBYTE(0x00)
		    .composeString(bodies[i].contentType.c_str())
			.composeString("")
			.composeString(bodies[i].charset.c_str())
			.composeString(bodies[i].emailbody.c_str());

	}
	for (i=0;i<attachments.size();i++)
	{
		body.composeBYTE(attachments[i].textAttachment?0x01:0xFF)
		    .composeString(attachments[i].contentType.c_str())
			.composeString(attachments[i].name.c_str())
			.composeString(attachments[i].charset.c_str());
		if (attachments[i].textAttachment)
		{
			body.composeString((const char *)attachments[i].emailbody.ptr());
		}
		else
		{
			body._composeVarBlock(attachments[i].emailbody.ptr(),attachments[i].emailbody.size());
		}
	}
}


void Email::composeForEmailWithCharsetAndEncoding( CommMsgBody& body )
{
	int i;
	body.composeString(from.normalAddress.c_str());
	for (i=0;i<addresses.size();i++)
	{
		PString addr;
		if (addresses[i].type == CommMail::addrTo)
			body.composeString(addresses[i].normalAddress.c_str());
		else if (addresses[i].type == CommMail::addrCc)
		{
			addr.append("cc:").append(addresses[i].normalAddress.c_str());
			body.composeString(addr);
		}
		else if (addresses[i].type == CommMail::addrBcc)
		{
			addr.append("bcc:").append(addresses[i].normalAddress.c_str());
			body.composeString(addr);
		}
	}
	body.composeString("");
	body.composeString(subject);
	size_t numAttachments = 0;
	if (bodies.size())
	{
		body.composeString(bodies[0].charset.c_str())
			.composeINT32(bodies[0].encoding)
			.composeString(bodies[0].emailbody.c_str());
		numAttachments = bodies.size() - 1;
	}
	else
	{
		body.composeString("")
			.composeUINT32(0)
			.composeString("");
	}
	numAttachments += attachments.size();
	PASSERT3(numAttachments < 256);
	body.composeBYTE((BYTE)numAttachments);
	for (i=1;i<bodies.size();i++)
	{
		body.composeBYTE(0x00)
		    .composeString(bodies[i].contentType.c_str())
			.composeString("")
			.composeString(bodies[i].charset.c_str())
			.composeINT32(bodies[1].encoding)
			.composeString(bodies[i].emailbody.c_str());

	}
	for (i=0;i<attachments.size();i++)
	{
		body.composeBYTE(attachments[i].textAttachment?0x01:0xFF)
		    .composeString(attachments[i].contentType.c_str())
			.composeString(attachments[i].name.c_str())
			.composeString(attachments[i].charset.c_str());
		if (attachments[i].textAttachment)
		{
			body.composeString((const char *)attachments[i].emailbody.ptr());
		}
		else
		{
			body._composeVarBlock(attachments[i].emailbody.ptr(),attachments[i].emailbody.size());
		}
	}
}


void Email::addAddressFromSMTPFormat( int type, const char* addr )
{
	vector <PString> ad,ad_n;
	CommAddressNormalize(ad,ad_n,addr);
	for (int i=0;i<ad.size();i++)
	{
		EmailAddress & addr_ = *addresses.insert(addresses.end(),EmailAddress());
		addr_.address =  ad[i];
		addr_.status = EmailAddress::eAddressSet;
		addr_.normalAddress =ad_n[i];
		addr_.status |= EmailAddress::eNormalAddressSet;
		addr_.type = type;
		addr_.status |= EmailAddress::eTypeSet;
	}
}



void Email::parseMailCharsetWithEncoding( CommMsgParser& parser )
{
	PTRACE5("parseMailCharsetWithEncoding, status=%u", status); 
	bodies.clear();// just in case
	attachments.clear();
	addresses.clear();
	notes.clear();
	headers.clear();
	if (email)
		delete email;
	email=0;

	const char* _from;
	parser.parseString( _from );
	CommAddressNormalize(from.address,from.normalAddress,_from);
	from.status = EmailAddress::eAddressSet;
	from.status |= EmailAddress::eNormalAddressSet;
	from.type = edbm_ReplyTo;
	from.status |= EmailAddress::eTypeSet;
	status |= eFrom;
	for(;;)
	{
		const char* s;
		parser.parseString( s );
		if( *s == 0 )
			break;

		if( strncmp( s, "cc:", 3 /*strlen("cc:")*/ ) == 0 )
		{
			addAddressFromSMTPFormat(CommMail::addrCc,s+3);
		}
		else if( strncmp( s, "bcc:", 4 /*strlen("cc:")*/ ) == 0 )
		{
			addAddressFromSMTPFormat(CommMail::addrBcc,s+4);
		}
		else 
		{
			addAddressFromSMTPFormat(CommMail::addrTo,s);
		}
	}

	const char* _subj;
	parser.parseString( _subj );
	subject = _subj;
	status |= eSubject;

	const char * _body;
	const char * _charset;
	UINT32 _encoding;
	parser.parseString( _charset );
	parser.parseUINT32( _encoding );
	parser.parseString( _body );
	if( *_body )
	{
		EmailBody & bd = *bodies.insert(bodies.end(),EmailBody());	
		bd.contentType = "text/plain";
		bd.status = EmailBody::eContentType;
		bd.charset = *_charset?_charset:"us-ascii";
		bd.status |= EmailBody::eCharset;
		bd.encoding = _encoding;
		if ( bd.encoding != 0)
			bd.status |= EmailBody::eEncoding;
		bd.emailbody = _body;
		bd.status |= EmailBody::eBody;
		bd.ordinal = 0;
		bd.status |= EmailBody::eOrdinal;
	}

	BYTE _nAttach;
	parser.parseBYTE( _nAttach );
	for( int i=0; i < _nAttach ; ++i )
	{
		BYTE _attachmentType;
		parser.parseBYTE( _attachmentType );

		switch( _attachmentType )
		{
			case 0x00:
				{
				const char * _mimeType;
				parser.parseString( _mimeType );
				const char * _name;
				parser.parseString( _name );

				const char * _charset;
				const char * _att;
				UINT32 _encoding;
				parser.parseString( _charset );
				parser.parseUINT32( _encoding );
				parser.parseString( _att );

				EmailBody & bd = *bodies.insert(bodies.end(),EmailBody());	
				bd.contentType = _mimeType;
				bd.status = EmailBody::eContentType;
				bd.charset = *_charset?_charset:"us-ascii";
				bd.status |= EmailBody::eCharset;
				bd.encoding = _encoding;
				if (bd.encoding != 0 )
					bd.status |= EmailBody::eEncoding;
				bd.emailbody = _att;
				bd.status |= EmailBody::eBody;
				bd.ordinal = bodies.size() - 1;
				bd.status |= EmailBody::eOrdinal;
				}
				break;
			case 0x01:
				{
				const char * _mimeType;
				parser.parseString( _mimeType );
				const char * _name;
				parser.parseString( _name );

				const char * _charset;
				const char * _att;
				parser.parseString( _charset );
				parser.parseString( _att );
				EmailAttachment & at = *attachments.insert(attachments.end(),EmailAttachment());	
				at.contentType = _mimeType;
				at.status = EmailAttachment::eContentType;
				at.charset = *_charset?_charset:"us-ascii";
				at.status |= EmailAttachment::eCharset;
				if (*_name)
				{
					at.name = _name;
					at.status |= EmailAttachment::eName;
				}
				memcpy( at.emailbody.alloc( strlen(_att) ), _att, strlen(_att));
				at.textAttachment=true;
				at.status |= EmailAttachment::eBody;
				at.ordinal = attachments.size()-1;
				at.status |= EmailAttachment::eOrdinal;
				}
				break;
			case 0xFF:
				{
				const char * _mimeType;
				parser.parseString( _mimeType );
				const char * _name;
				parser.parseString( _name );

				const char * _charset;
				const BYTE* p;
				size_t sz;
				parser.parseString( _charset );
				parser._parseVarBlock( p, sz );
				EmailAttachment & at = *attachments.insert(attachments.end(),EmailAttachment());	
				at.contentType = _mimeType;
				at.status = EmailAttachment::eContentType;
				at.charset = *_charset?_charset:"us-ascii";
				at.status |= EmailAttachment::eCharset;
				if (*_name)
				{
					at.name = _name;
					at.status |= EmailAttachment::eName;
				}
				memcpy( at.emailbody.alloc( sz ), p,sz);
				at.textAttachment=false;
				at.status |= EmailAttachment::eBody;
				at.ordinal = attachments.size()-1;
				at.status |= EmailAttachment::eOrdinal;
				}
				break;

			default:
				PASSERT( 0 );
				break;
		}
	}
	hasAttachments = attachments.size();
	status |= eHasAttachments;
	incomingType = edbm_emailSent;
	status |= eIncomingType;
	special = edbm_emailNoSpecial;
	status |= eSpecial;
	flag = edbm_emailNew;
	status |= eFlag;
	state = edbm_StateForSending;
	status |= eState;

	const char * _userId="";
	const char * _mailBox="";
	autoEmail = 0;
	if (!parser.parseEnded())
	{
		parser.parseString(_userId);
		if (!parser.parseEnded())
		{
			parser.parseString(_mailBox);
			if (!parser.parseEnded())
			{
				parser.parseBYTE(autoEmail);
			}
		}
	}
	if (*_mailBox)
	{
		privateEmailBox = _mailBox;
		status |= ePrivateEmailBox;
	}
	if (*_userId)
	{
		userOwner = _userId;
		status |= eUserOwner;
	}
}

void Email::parseMailCharset( CommMsgParser& parser )
{
	PTRACE5("parseMailCharset, status=%u", status); 
	bodies.clear();// just in case
	attachments.clear();
	addresses.clear();
	notes.clear();
	headers.clear();
	if (email)
		delete email;
	email=0;

	const char* _from;
	parser.parseString( _from );
	CommAddressNormalize(from.address,from.normalAddress,_from);
	from.status = EmailAddress::eAddressSet;
	from.status |= EmailAddress::eNormalAddressSet;
	from.type = edbm_ReplyTo;
	from.status |= EmailAddress::eTypeSet;
	status |= eFrom;
	for(;;)
	{
		const char* s;
		parser.parseString( s );
		if( *s == 0 )
			break;

		if( strncmp( s, "cc:", 3 /*strlen("cc:")*/ ) == 0 )
		{
			addAddressFromSMTPFormat(CommMail::addrCc,s+3);
		}
		else if( strncmp( s, "bcc:", 4 /*strlen("cc:")*/ ) == 0 )
		{
			addAddressFromSMTPFormat(CommMail::addrBcc,s+4);
		}
		else 
		{
			addAddressFromSMTPFormat(CommMail::addrTo,s);
		}
	}

	const char* _subj;
	parser.parseString( _subj );
	subject = _subj;
	status |= eSubject;

	const char * _body;
	const char * _charset;

	parser.parseString( _charset );
	parser.parseString( _body );
	if( *_body )
	{
		EmailBody & bd = *bodies.insert(bodies.end(),EmailBody());	
		bd.contentType = "text/plain";
		bd.status = EmailBody::eContentType;
		bd.charset = *_charset?_charset:"us-ascii";
		bd.status |= EmailBody::eCharset;
		bd.emailbody = _body;
		bd.status |= EmailBody::eBody;
		bd.ordinal = 0;
		bd.status |= EmailBody::eOrdinal;
	}

	BYTE _nAttach;
	parser.parseBYTE( _nAttach );
	for( int i=0; i < _nAttach ; ++i )
	{
		BYTE _attachmentType;
		parser.parseBYTE( _attachmentType );

		switch( _attachmentType )
		{
			case 0x00:
				{
				const char * _mimeType;
				parser.parseString( _mimeType );
				const char * _name;
				parser.parseString( _name );

				const char * _charset;
				const char * _att;
				parser.parseString( _charset );
				parser.parseString( _att );

				EmailBody & bd = *bodies.insert(bodies.end(),EmailBody());	
				bd.contentType = _mimeType;
				bd.status = EmailBody::eContentType;
				bd.charset = *_charset?_charset:"us-ascii";
				bd.status |= EmailBody::eCharset;
				bd.emailbody = _att;
				bd.status |= EmailBody::eBody;
				bd.ordinal = bodies.size() - 1;
				bd.status |= EmailBody::eOrdinal;
				}
				break;
			case 0x01:
				{
				const char * _mimeType;
				parser.parseString( _mimeType );
				const char * _name;
				parser.parseString( _name );

				const char * _charset;
				const char * _att;
				parser.parseString( _charset );
				parser.parseString( _att );
				EmailAttachment & at = *attachments.insert(attachments.end(),EmailAttachment());	
				at.contentType = _mimeType;
				at.status = EmailAttachment::eContentType;
				at.charset = *_charset?_charset:"us-ascii";
				at.status |= EmailAttachment::eCharset;
				if (*_name)
				{
					at.name = _name;
					at.status |= EmailAttachment::eName;
				}
				memcpy( at.emailbody.alloc( strlen(_att) ), _att, strlen(_att));
				at.textAttachment=true;
				at.status |= EmailAttachment::eBody;
				at.ordinal = attachments.size()-1;
				at.status |= EmailAttachment::eOrdinal;
				}
				break;
			case 0xFF:
				{
				const char * _mimeType;
				parser.parseString( _mimeType );
				const char * _name;
				parser.parseString( _name );

				const char * _charset;
				const BYTE* p;
				size_t sz;
				parser.parseString( _charset );
				parser._parseVarBlock( p, sz );
				EmailAttachment & at = *attachments.insert(attachments.end(),EmailAttachment());	
				at.contentType = _mimeType;
				at.status = EmailAttachment::eContentType;
				at.charset = *_charset?_charset:"us-ascii";
				at.status |= EmailAttachment::eCharset;
				if (*_name)
				{
					at.name = _name;
					at.status |= EmailAttachment::eName;
				}
				memcpy( at.emailbody.alloc( sz ), p,sz);
				at.textAttachment=false;
				at.status |= EmailAttachment::eBody;
				at.ordinal = attachments.size()-1;
				at.status |= EmailAttachment::eOrdinal;
				}
				break;

			default:
				PASSERT( 0 );
				break;
		}
	}
	hasAttachments = attachments.size();
	status |= eHasAttachments;
	incomingType = edbm_emailSent;
	status |= eIncomingType;
	special = edbm_emailNoSpecial;
	status |= eSpecial;
	flag = edbm_emailNew;
	status |= eFlag;
	state = edbm_StateForSending;
	status |= eState;

	const char * _userId="";
	const char * _mailBox="";
	autoEmail = 0;
	if (!parser.parseEnded())
	{
		parser.parseString(_userId);
		if (!parser.parseEnded())
		{
			parser.parseString(_mailBox);
			if (!parser.parseEnded())
			{
				parser.parseBYTE(autoEmail);
			}
		}
	}
	if (*_mailBox)
	{
		privateEmailBox = _mailBox;
		status |= ePrivateEmailBox;
	}
	if (*_userId)
	{
		userOwner = _userId;
		status |= eUserOwner;
	}
}


void Email::composeHeadersToPrivateSystem(CommMsgBody& body)
{
	if (status & eEmailId)
		body.composeUINT32(emailId);
	else
		PASSERT3(0);
	body.composeString(privateEmailBox.c_str());
	body.composeString(subject.c_str());
	body.composeUINT32(state);
	body.composeUINT32(flag);	
	body.composeUINT32(hasAttachments);
	body.composeUINT32(incomingType);
	sent.compose(body);
	received.compose(body);
	body.composeString(userOwner.c_str());
	int i;
	body.composeUINT32(addresses.size() + (status&eFrom ? 1:0));
	if (status&eFrom)
	{
		from.composeToPrivateSystem(body);
	}
	for (i=0;i<addresses.size();i++)
	{
		addresses[i].composeToPrivateSystem(body);
	}
	body.composeUINT32(labels.size());
	for (i=0;i<labels.size();i++)
	{
		labels[i].compose(body);
	}

}

void Email::composeToPrivateSystem(CommMsgBody& body)
{
	int i;
	composeHeadersToPrivateSystem(body);

	body.composeUINT32(bodies.size());

	for (i=0;i<bodies.size();i++)
	{
		bodies[i].composeToPrivateSystem(body);
	}
	body.composeUINT32(notes.size());
	for (i=0;i<notes.size();i++)
	{
		notes[i].composeToPrivateSystem(body);
	}

	body.composeUINT32(attachments.size());
	for (i=0;i<attachments.size();i++)
	{
		attachments[i].composeToPrivateSystem(body);
	}

}


void Email::parseFromFrontEnd(CommMsgParser & parser, bool saveAsDraft)
{
	bodies.clear();// just in case
	attachments.clear();
	addresses.clear();
	notes.clear();
	headers.clear();

	status = 0;
	received.currentLocalTime();
	status |= eReceived;
	sent.currentLocalTime();
	status |= eSent;
	incomingType = saveAsDraft ? edbm_emailDraft : edbm_emailSent;
	status |= eIncomingType;
	special = edbm_emailNoSpecial;
	status |= eSpecial;
	flag = edbm_emailNew;
	status |= eFlag;
	state = saveAsDraft ? edbm_StateInProgress:edbm_StateForSending;
	status |= eState;

	parser.parseUINT32(emailId);
	if (emailId )
		status |= eEmailId;
	parser.parseStringP(privateEmailBox);
	status |= ePrivateEmailBox;
	parser.parseStringP(subject);
	status |= eSubject;

	const char * fromAddress;
	parser.parseString(fromAddress);


	CommAddressNormalize(from.address,from.normalAddress,fromAddress);
	from.status = EmailAddress::eAddressSet;
	from.status |= EmailAddress::eNormalAddressSet;
	from.type = edbm_ReplyTo;
	from.status |= EmailAddress::eTypeSet;
	status |= eFrom;

	UINT32 counter;
	parser.parseUINT32(counter);

	int i;
	for (i=0;i<counter;i++)
	{
		const char * toAddress;
		parser.parseString(toAddress);
		UINT32 addrType;
		parser.parseUINT32(addrType);
		EmailAddress & addr = *addresses.insert(addresses.end(),EmailAddress());	
		CommAddressNormalize(addr.address,addr.normalAddress,toAddress);
		addr.status = EmailAddress::eAddressSet;
		addr.status |= EmailAddress::eNormalAddressSet;
		addr.type = addrType;
		addr.status |= EmailAddress::eTypeSet;
	}
	parser.parseUINT32(counter);
	for (i=0;i<counter;i++)
	{
		EmailBody & bd = *bodies.insert(bodies.end(),EmailBody());	
		parser.parseStringP(bd.contentType);
		bd.status = EmailBody::eContentType;
		parser.parseStringP(bd.charset);
		bd.status |= EmailBody::eCharset;
		parser.parseUINT32(bd.encoding);
		if ( bd.encoding != 0 )
			bd.status |= EmailBody::eEncoding;
		bd.binaryBody = false;
		PBlock b;
		parser.parsePBlock(b);
		bd.emailbody.assignFromBlock( b.ptr(),b.size());
		bd.status |= EmailBody::eBody;
		bd.ordinal = i;
		bd.status |= EmailBody::eOrdinal;
	}

	parser.parseUINT32(counter);
	for (i=0;i<counter;i++)
	{
		EmailAttachment & at = *attachments.insert(attachments.end(),EmailAttachment());	

		parser.parseStringP(at.contentType);
		at.status = EmailAttachment::eContentType;
		parser.parseStringP(at.charset);
		at.status |= EmailAttachment::eCharset;
		parser.parseStringP(at.name);
		at.status |= EmailAttachment::eName;
		at.textAttachment=false;
		parser.parsePBlock(at.emailbody);
		at.status |= EmailAttachment::eBody;
		at.ordinal = i;
		at.status |= EmailAttachment::eOrdinal;
	}
	parser.parseUINT32(counter);
	for (i=0;i<counter;i++)
	{
		EmailAttachmentReference & at = *attReferences.insert(attReferences.end(),EmailAttachmentReference());	
		at.parse(parser);
	}
	hasAttachments = attachments.size() + attReferences.size();
	if (hasAttachments)
		status |= eHasAttachments;
}

void Email::composeForSMTP( CommMsgBody& body, bool encryptedBody )
{
	int i;
	body.composeUINT32(emailId);
	body.composeString(from.address.c_str());
	body.composeString(from.normalAddress.c_str());

	body.composeUINT32(addresses.size());
	for (i=0;i<addresses.size();i++)
	{
		body.composeUINT32(addresses[i].type)
			.composeString(addresses[i].address.c_str())
			.composeString(addresses[i].normalAddress.c_str())
			.composeBOOL(addresses[i].addressLocal);
	}
	body.composeString(subject);


	if (!encryptedBody)
	{
		UINT32 counter = 0;
		for (i=0;i<bodies.size();i++)
		{
			if (!bodies[i].contentType.equalsIgnoreCase("ps_special/encrypted"))
				counter++;
		}

		body.composeUINT32(counter);
		for (i=0;i<bodies.size();i++)
		{
			if (!bodies[i].contentType.equalsIgnoreCase("ps_special/encrypted"))
			{
				body.composeString(bodies[i].charset.c_str())
					.composeString(bodies[i].contentType.c_str())
					.composeINT32(bodies[i].encoding);
				if (bodies[i].binaryBody)
				{
					int len = 0;
					const char * p= (const char * )  bodies[i].binaryEmailBody.ptr();
					while (len <  bodies[i].binaryEmailBody.size())
					{
						if (!*p++)
							break;
						len++;
					}
					PString s((const char*) bodies[i].binaryEmailBody.ptr(),len);
					body.composeString(s);
				}
				else
				{
					body.composeString(bodies[i].emailbody);
				}
			}
		}
	}
	else
	{
		body.composeUINT32(0);
	}
	body.composeUINT32(attachments.size());
	for (i=0;i<attachments.size();i++)
	{
		body.composeString(attachments[i].charset.c_str())
			.composeString(attachments[i].contentType.c_str())
			.composeString(attachments[i].name.c_str());
		body._composeVarBlock(attachments[i].emailbody.ptr(),attachments[i].emailbody.size());
	}
}
