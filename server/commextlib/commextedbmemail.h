#ifndef commextedbmemail_h_included
#define commextedbmemail_h_included

#include <pplib.h>
#include <commlib.h>
#include <commtok822.h>
#include <dbm.h>
#include <edbm.h>

#include <pyrtime.h>
#include <i18n.localedef.h>

class Email
{
public:

	class EmailAddress
	{
		public:

		enum
		{
			eTypeSet=1,
			eFlagsSet=2,
			eAddressSet=4,
			eNormalAddressSet=8,
			eUserIdSet=16,
			eCountrySet=32,
			eFullNameSet=64,
			eStateSet = 128,
			eLocaleSet = 256,
			eLicenseIdSet = 512
		};

		UINT32  status;
		UINT32  type;
		UINT32  flags;
		PString address;
		PString normalAddress;
		PString userId;
		PString country;
		PString fullName;
		PString state;
		UINT32  emailLocale;
		UINT32  licenseId;
		UINT32  emailLocaleOriginal;
		UINT32  clientLocaleOriginal;
		bool    addressLocal;

		EmailAddress()
		{
			status=0;
			type=0;
			flags=0;
			emailLocale = LOCALE_DEFAULT;
			licenseId = 0;
			emailLocaleOriginal = LOCALE_INVALID;
			clientLocaleOriginal = LOCALE_INVALID;
			addressLocal = false;
		}
		void compose(CommMsgBody &body) const
		{
			body.composeUINT32(status);
			if (status & eTypeSet)
				body.composeUINT32(type);
			if (status & eFlagsSet)
				body.composeUINT32(flags);
			if (status & eAddressSet)
				body.composeString(address.c_str());
			if (status & eNormalAddressSet)
				body.composeString(normalAddress.c_str());
			if (status & eUserIdSet)
				body.composeString(userId.c_str());
			if (status & eCountrySet)
				body.composeString(country.c_str());
			if (status & eFullNameSet)
				body.composeString(fullName.c_str());
			if (status & eStateSet)
				body.composeString(state.c_str());
			if (status & eLocaleSet)
				body.composeUINT32(emailLocale);
			if (status & eLicenseIdSet)
				body.composeUINT32(licenseId);
		}

		void composeToPrivateSystem(CommMsgBody &body) const
		{
			if (status & eAddressSet)
				body.composeString(address.c_str());
			else
				body.composeString(normalAddress.c_str());
			body.composeString(normalAddress.c_str());
			body.composeUINT32(flags);
			body.composeString(userId.c_str());
			body.composeUINT32(type);
			body.composeString(country.c_str());
			body.composeString(state.c_str());
			body.composeUINT32(emailLocale);
			body.composeUINT32(licenseId);
		}
		void parse(CommMsgParser &parser)
		{
			parser.parseUINT32(status);
			if (status & eTypeSet)
				parser.parseUINT32(type);
			else
				type=0;
			if (status & eFlagsSet)
				parser.parseUINT32(flags);
			else
				flags=0;
			if (status & eAddressSet)
				parser.parseStringP(address);
			else
				address="";
			if (status & eNormalAddressSet)
				parser.parseStringP(normalAddress);
			else
				normalAddress="";
			if (status & eUserIdSet)
				parser.parseStringP(userId);
			else
				userId="";
			if (status & eCountrySet)
				parser.parseStringP(country);
			else
				country="";
			if (status & eFullNameSet)
				parser.parseStringP(fullName);
			else
				fullName="";
			if (status & eStateSet)
				parser.parseStringP(state);
			else
				state="";
			if (status & eLocaleSet)
				parser.parseUINT32(emailLocale);
			else
				emailLocale=0;
			if (status & eLicenseIdSet)
				parser.parseUINT32(licenseId);
			else
				licenseId=0;


		}
	};

	class EmailBody
	{
	public:
		enum
		{
			eOrdinal = 1,
			eContentType = 2,
			eCharset = 4,
			eBody = 8,
			eEncoding = 16
		};
		bool binaryBody;
		UINT32  status;
		UINT32  ordinal;
		PString contentType;
		PString charset;
		UINT32 encoding;
		PString emailbody;
		PBlock  binaryEmailBody;
		EmailBody()
		{
			status=0;
			ordinal=0;
			binaryBody = false;
			encoding = 0;
		}
		void compose(CommMsgBody & body) const
		{
			body.composeUINT32(status);
			if (status & eOrdinal)
				body.composeUINT32(ordinal);
			if (status & eContentType)
				body.composeString(contentType.c_str());
			if (status & eCharset)
				body.composeString(charset.c_str());
			if (status & eEncoding)
				body.composeUINT32(encoding);
			if (status & eBody)
			{
				body.composeBOOL(binaryBody);
				if (binaryBody)
					body._composeVarBlock(binaryEmailBody.ptr(),binaryEmailBody.size());
				else
					body.composeString(emailbody.c_str());
			}
		}
		void composeGeneric(CommMsgBody & body, UINT32 bodyId) const
		{
			UINT32 status_ = status;
			status_|=eOrdinal;
			if (bodyId >= NEW_BODY_ORDINAL)
			{
				if ((status & eOrdinal) && (ordinal == bodyId))
				{
					body.composeUINT32(eOrdinal);
					body.composeUINT32(bodyId);
					return;
				}
			}
			body.composeUINT32(status_);
			body.composeUINT32(bodyId);
			if (status & eContentType)
				body.composeString(contentType.c_str());
			if (status & eCharset)
				body.composeString(charset.c_str());
			if (status & eEncoding)
				body.composeUINT32(encoding);
			if (status & eBody)
			{
				body.composeBOOL(binaryBody);
				if (binaryBody)
					body._composeVarBlock(binaryEmailBody.ptr(),binaryEmailBody.size());
				else
					body.composeString(emailbody.c_str());
			}
		}

		void composeToPrivateSystem(CommMsgBody & body) const
		{
			body.composeUINT32(ordinal);
			body.composeString(contentType.c_str());
			body.composeString(charset.c_str());
			body.composeUINT32(encoding);
			body.composeBOOL(binaryBody);
			if (binaryBody)
				body._composeVarBlock(binaryEmailBody.ptr(),binaryEmailBody.size());
			else
				body.composeString(emailbody.c_str());
		}

		void parse(CommMsgParser &parser)
		{
			parser.parseUINT32(status);
			if (status & eOrdinal)
				parser.parseUINT32(ordinal);
			else
				ordinal=0;
			if (status & eContentType)
				parser.parseStringP(contentType);
			else
				contentType="";
			if (status & eCharset)
				parser.parseStringP(charset);
			else
				charset="";
			if (status & eEncoding)
			{
				parser.parseUINT32(encoding);
				if ( encoding == 0 )
					status &= ~eEncoding;
			}
			else
				encoding=0;

			if (status & eBody)
			{
				parser.parseBOOL(binaryBody);
				if (binaryBody)
				{
					const BYTE* p;
					size_t sz;
					parser._parseVarBlock( p, sz );
					memcpy( binaryEmailBody.alloc( sz ), p, sz );
				}
				else
					parser.parseStringP(emailbody);
			}
			else
				emailbody="";
		}
	};
	class EmailAttachment
	{
	public:
		enum
		{
			eOrdinal = 1,
			eContentType = 2,
			eCharset = 4,
			eName = 8,
			eBody = 16,
			eSize = 32,
			eSecureAttachment = 64
		};
		UINT32  status;
		UINT32  ordinal;
		PString contentType;
		PString charset;
		PString name;
		UINT32 size;
		bool    textAttachment;
		PBlock  emailbody;
		UINT32  tempStatus;
		EmailAttachment()
		{
			status=0;
			ordinal=0;
			size = 0;
			textAttachment=false;
			tempStatus = 0;
		}
		void compose(CommMsgBody & body) const
		{
			body.composeUINT32(status);
			if (status & eOrdinal)
				body.composeUINT32(ordinal);
			if (status & eContentType)
				body.composeString(contentType.c_str());
			if (status & eCharset)
				body.composeString(charset.c_str());
			if (status & eName)
				body.composeString(name.c_str());
			if (status & eSize)
				body.composeUINT32(size);

			if (status & eBody)
			{
				body.composeBOOL(textAttachment);
				body._composeVarBlock(emailbody.ptr(),emailbody.size());
			}
		}
		void composeGenerated(CommMsgBody & body) const
		{
			PASSERT3(status & eOrdinal);
			if (ordinal >= NEW_ATTACHMENT_ORDINAL)
			{
				body.composeUINT32(eOrdinal);
				body.composeUINT32(ordinal);
			}
			else
			{
				compose(body);
			}
		}

		void composeToPrivateSystem(CommMsgBody & body) const
		{
			body.composeUINT32(ordinal);
			body.composeString(contentType.c_str());
			body.composeString(charset.c_str());
			body.composeString(name.c_str());
			if (status & eSize)
				body.composeUINT32(size);
			else if (status & eBody)
				body.composeUINT32(emailbody.size());
			else
				body.composeUINT32(0);
		}

		void parse(CommMsgParser &parser)
		{
			parser.parseUINT32(status);
			if (status & eOrdinal)
				parser.parseUINT32(ordinal);
			else
				ordinal=0;
			if (status & eContentType)
				parser.parseStringP(contentType);
			else
				contentType="";
			if (status & eCharset)
				parser.parseStringP(charset);
			else
				charset="";
			if (status & eName)
				parser.parseStringP(name);
			else
				name="";
			if (status & eSize)
				parser.parseUINT32(size);
			else
				size=0;

			if (status & eBody)
			{
				parser.parseBOOL(textAttachment);
				const BYTE* p;
				size_t sz;
				parser._parseVarBlock( p, sz );
				memcpy( emailbody.alloc( sz ), p, sz );
			}
			else
			{
				textAttachment = false;
				emailbody.clear();
			}
		}
	};


	class EmailNote
	{
	public:
		enum
		{
			eWhen = 1,
			eUserId = 2,
			eNote = 4
		};
		UINT32 status;
		SrvTime when;
		PString userId;
		PString note;
		UINT32 tempStatus;
		EmailNote()
		{
			status=0;
			tempStatus=0;
		}
		void compose(CommMsgBody & body) const
		{
			body.composeUINT32(status);
			if (status & eWhen)
				when.compose(body);
			if (status & eUserId)
				body.composeString(userId.c_str());
			if (status & eNote)
				body.composeString(note.c_str());
		}
		void parse(CommMsgParser &parser)
		{
			parser.parseUINT32(status);
			if (status & eWhen)
				when.parse(parser);
			else
				when.setNull();
			if (status & eUserId)
				parser.parseStringP(userId);
			else
				userId="";
			if (status & eNote)
				parser.parseStringP(note);
			else
				note="";
		}
		void composeToPrivateSystem(CommMsgBody & body) const
		{
			body.composeString(note.c_str());
			when.compose(body);
			body.composeString(userId.c_str());
		}

	};
	class EmailHeader
	{
	public:
		PString name;
		PString value;
		void compose(CommMsgBody & body) const
		{
			body.composeString(name.c_str());
			body.composeString(value.c_str());
		}
		void parse(CommMsgParser & parser)
		{
			parser.parseStringP(name);
			parser.parseStringP(value);
		}
	};
	class EmailAttachmentReference
	{
	public:
		UINT32 emailId;
		UINT32 ordinal;
		EmailAttachmentReference()
		{
			emailId = 0;
			ordinal = 0;
		}
		void parse(CommMsgParser & parser) 
		{
			parser.parseUINT32(emailId)
				  .parseUINT32(ordinal);
		}
	};
	class EmailLabel
	{
	public:
		UINT32 labelId;
		EmailLabel()
		{
			labelId = 0;
		}
		EmailLabel(UINT32 labelId_)
		{
			labelId = labelId_;
		}

		void parse(CommMsgParser & parser) 
		{
			parser.parseUINT32(labelId);
		}
		void compose(CommMsgBody & body) const
		{
			body.composeUINT32(labelId);
		}

	};


	enum
	{
		eEmailId = 1,
		eReceived = 1<<1,
		eSent = 1<<2,
		eFrom =1<<4,
		eReplyTo = 1<<5,
		eSubject = 1<<6,
		eUserOwner = 1<<7,
		eHasAttachments = 1<<8,
		eState=1<<9,
		eIncomingType = 1<<10,
		eTransId = 1<<11,
		eSpecial = 1<<12,
		eFlag = 1<<13,
		eSource = 1<<14,
		eMailBox = 1<<15,
		eProcessingError = 1<<16,
		ePrivateEmailBox = 1<<17
	};
	enum
	{
		eParsingError =1
	};


	UINT32 refCounter;
	UINT32 status;

	PString source;
	CommMail * email;

	UINT32 emailId;
	SrvTime received;
	SrvTime sent;
	EmailAddress from;
	EmailAddress replyTo;
	PString subject;
	PString userOwner;
	PString mailBox;
	PString privateEmailBox;
	UINT32  hasAttachments;
	UINT32  state;
	UINT32  incomingType;
	UINT32  transId;
	UINT32  special;
	UINT32  flag;
	UINT32  processingError;
	BYTE autoEmail;
	vector <EmailBody> bodies; 
	vector <EmailAttachment> attachments;
	vector <EmailAddress> addresses;
	vector <EmailNote> notes;
	vector <EmailHeader> headers;
	vector <EmailAttachmentReference> attReferences;
	vector <EmailLabel> labels;

	Email()
	{
		refCounter = 0;
		status=0;
		email=0;
		emailId = 0;
		hasAttachments = 0;
		state = 0;
		incomingType = 0;
		transId = 0;
		special = 0;
		flag = 0;
		processingError = 0;
		autoEmail=0;

	}
	~Email()
	{
		if (email)
			delete email;
	}

	const EmailAddress & replyAddress() const
	{
		if (status & eReplyTo)
			return replyTo;
		else
			return from;
	}
	EmailAddress * findTo() 
	{
		for (int i=0;i<addresses.size();i++)
		{
			if (addresses[i].type == CommMail::addrTo)
				return &addresses[i];
		}
		return 0;
	}

	void addAddressFromSMTPFormat( int type, const char* addr );

	// compose-parse between proxy-database. Generic format to keep all data
	void compose(CommMsgBody& body) const;
	void composeGenerated(CommMsgBody& body, const char * mailBox_,  const char * dataMailBox_, UINT32 emailId, const char * userOwner_, vector<UINT32> & bodyIds);
	void parse(CommMsgParser & parser);

	// parse from source
	void loadFromPOP3();

	// compose in DBM for private front end. Used for list of emails with and without bodies
	void composeHeadersToPrivateSystem(CommMsgBody& body); 
	void composeToPrivateSystem(CommMsgBody& body);		   


	// In this format email is sent from CGI or Private Front End to server side
	void parseFromFrontEnd(CommMsgParser & parser, bool saveAsDraft);
	
	// In this format email is sent from  server to email object (SMTP)  for sending
	void composeForSMTP( CommMsgBody& body, bool encryptedBody );
	
	// Parse emails from email object for sending.
	// Obsolet format, shouldn't be used any more, at least in private system
	void parseMailCharsetWithEncoding( CommMsgParser& parser );
	void parseMailCharset( CommMsgParser& parser );

	// corresponding compose functions for previous format. Obsolete.
	void composeForEmail( CommMsgBody& body );
	void composeForEmailWithCharset( CommMsgBody& body );
	void composeForEmailWithCharsetAndEncoding( CommMsgBody& body );

};



#endif