/**
 * dbm_atf.h
 *
 * This file was auto-generated from dbm_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: dbm_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_DBM_ATF_H
#define ATF_PROTOCOL_DEFFILE_DBM_ATF_H

#include "dbm.h"
#include "brandimgs.h"


#include "UserDefStructs.h"

namespace dbm
{
	class ProtocolFactory
	{
	public:
		static Atf::MessageProtocol* createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId);
		static bool validateProtocolMessageFromJSONString(const char *_nameSpace, UINT32 _msgId, const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	namespace admin
	{
		// This message MUST be prepend with MessageInfoStruct
		struct Protocol_DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG : public Atf::MessageProtocol
		{
			// 444s - userIntId,reasons,updateType,comment
			UINT32 userIntId;
			UINT32 reasons;
			INT32 updateType;	// type*SetFraudulentAccountFlag
			PString comment;	// In DB field name is 'USERCOMMENTS.COMMENT'

			Protocol_DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG();
			void clear() override;
			bool equals(const Protocol_DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG; }
			const char *getMsgIdString() const override { return "DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_DBM_A_SET_FRAUDULENT_ACCOUNT_FLAG : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errDesc]
			INT16 errCode;

			PString errDesc;

			Protocol_DBM_A_SET_FRAUDULENT_ACCOUNT_FLAG();
			void clear() override;
			bool equals(const Protocol_DBM_A_SET_FRAUDULENT_ACCOUNT_FLAG&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_SET_FRAUDULENT_ACCOUNT_FLAG; }
			const char *getMsgIdString() const override { return "DBM_A_SET_FRAUDULENT_ACCOUNT_FLAG"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		// This message MUST be prepend with MessageInfoStruct
		struct Protocol_DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG : public Atf::MessageProtocol
		{
			// 44s - userIntId,updateType,comment
			UINT32 userIntId;
			INT32 updateType;	// type*ClearFraudulentAccountFlag
			PString comment;	// In DB field name is 'USERCOMMENTS.COMMENT'

			Protocol_DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG();
			void clear() override;
			bool equals(const Protocol_DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG; }
			const char *getMsgIdString() const override { return "DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_DBM_A_CLEAR_FRAUDULENT_ACCOUNT_FLAG : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errDesc]
			INT16 errCode;

			PString errDesc;

			Protocol_DBM_A_CLEAR_FRAUDULENT_ACCOUNT_FLAG();
			void clear() override;
			bool equals(const Protocol_DBM_A_CLEAR_FRAUDULENT_ACCOUNT_FLAG&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_CLEAR_FRAUDULENT_ACCOUNT_FLAG; }
			const char *getMsgIdString() const override { return "DBM_A_CLEAR_FRAUDULENT_ACCOUNT_FLAG"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};
	};

	namespace ArchiveManager
	{
		struct RelatedUsers
		{
			// M(s4ts4sts4) - M(groupId,severity,userWhen,userComment,otherIntId,otherUserId,otherWhen,otherComment,otherAdminIntId)
			PString groupId;
			UINT32 severity;
			CommSrvTime userWhen;
			PString userComment;
			UINT32 otherIntId;
			PString otherUserId;
			CommSrvTime otherWhen;
			PString otherComment;
			UINT32 otherAdminIntId;

			RelatedUsers();
			void clear();
			bool equals(const RelatedUsers&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_DBM_Q_ARCH_GET_RELATEDUSERS : public Atf::MessageProtocol	// archivemanager -> OLAP
		{
			// s - userId
			PString userId;

			Protocol_DBM_Q_ARCH_GET_RELATEDUSERS();
			void clear() override;
			bool equals(const Protocol_DBM_Q_ARCH_GET_RELATEDUSERS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_ARCH_GET_RELATEDUSERS; }
			const char *getMsgIdString() const override { return "DBM_Q_ARCH_GET_RELATEDUSERS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_DBM_A_ARCH_GET_RELATEDUSERS : public Atf::MessageProtocol
		{
			//! 2 <FIXME_COND_IF> 4<M(s4ts4sts4)> <FIXME_COND_ELSE> s <FIXME_COND_ENDIF>  - errCode <FIXME_COND_IF> ,numRelatedUsers<M(groupId,severity,userWhen,userComment,otherIntId,otherUserId,otherWhen,otherComment,otherAdminIntId)> <FIXME_COND_ELSE> ,errString <FIXME_COND_ENDIF> 
			INT16 errCode;

			Atf::LAtfVector< RelatedUsers, 4 > relatedUsers;

			PString errString;

			Protocol_DBM_A_ARCH_GET_RELATEDUSERS();
			void clear() override;
			bool equals(const Protocol_DBM_A_ARCH_GET_RELATEDUSERS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_ARCH_GET_RELATEDUSERS; }
			const char *getMsgIdString() const override { return "DBM_A_ARCH_GET_RELATEDUSERS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_DBM_Q_UNARCHIVE_INSERT_RELATEDUSERS : public Atf::MessageProtocol	// archivemanager -> dbm
		{
			// 4s4<M(s4ts4sts4)> - userIntId,userId,numRelatedUsers<M(groupId,severity,userWhen,userComment,otherIntId,otherUserId,otherWhen,otherComment,otherAdminIntId)>
			UINT32 userIntId;
			PString userId;
			Atf::LAtfVector< RelatedUsers, 4 > relatedUsers;

			Protocol_DBM_Q_UNARCHIVE_INSERT_RELATEDUSERS();
			void clear() override;
			bool equals(const Protocol_DBM_Q_UNARCHIVE_INSERT_RELATEDUSERS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_UNARCHIVE_INSERT_RELATEDUSERS; }
			const char *getMsgIdString() const override { return "DBM_Q_UNARCHIVE_INSERT_RELATEDUSERS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_DBM_A_UNARCHIVE_INSERT_RELATEDUSERS : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errString]
			INT16 errCode;

			PString errString;

			Protocol_DBM_A_UNARCHIVE_INSERT_RELATEDUSERS();
			void clear() override;
			bool equals(const Protocol_DBM_A_UNARCHIVE_INSERT_RELATEDUSERS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_UNARCHIVE_INSERT_RELATEDUSERS; }
			const char *getMsgIdString() const override { return "DBM_A_UNARCHIVE_INSERT_RELATEDUSERS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};
	};

	struct Protocol_DBM_Q_USER_INFO : public Atf::MessageProtocol
	{
		// sss - userId,countryByIp,stateByIp
		PString userId;
		PString countryByIp;
		PString stateByIp;

		Protocol_DBM_Q_USER_INFO();
		void clear() override;
		bool equals(const Protocol_DBM_Q_USER_INFO&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return DBM_Q_USER_INFO; }
		const char *getMsgIdString() const override { return "DBM_Q_USER_INFO"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct P_UserProperty
	{
		// 44st - propType,propInt,propStr,propWhen
		INT32 propType;
		INT32 propInt;
		PString propStr;
		SrvTime propWhen;

		P_UserProperty();
		void clear();
		bool equals(const P_UserProperty&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
	};

	struct Protocol_DBM_A_USER_INFO : public Atf::MessageProtocol
	{
		//! 2[s|ssssssssv4444444sd884ttsss4<44st>424284ssbts4881d18 <FIXME>BrandProt::BrandImageList</FIXME> 44] - errcode[errstr|fullName,email,addr_1,addr_2,city,state,country,zipCode,image,privileges,correctedUserFlags,effectivePrivileges,imageIdUser,bonus,fppToEarn,availAmount,phone,birthDate,privileges2,effectivePrivileges2,daysExclActual,srvExclFrom,srvExclUntil,firstName,lastName,fiscalCode,numUserProps<propType,propInt,propStr,propWhen>,vipCounter1,vipCounter1Cents,vipCounter2,vipCounter2Cents,correctedUserFlags2,imageId,sex,mobile,addressStateLocked,registered,promoName,blockingCode,privileges3,effectivePrivileges3,missedMonths,vipStatusUntil,missableMonthsYearly,correctedUserFlags3, <FIXME>brandImages</FIXME> ,hallOfFameLevel,licenseId]
		INT16 errcode;

		PString errstr;

		PString fullName;
		PString email;
		PString addr_1;
		PString addr_2;
		PString city;
		PString state;
		PString country;
		PString zipCode;
		PBlock image;	// deprecated and not filled out it appears
		UINT32 privileges;
		UINT32 correctedUserFlags;
		UINT32 effectivePrivileges;
		UINT32 imageIdUser;	// duplicated field with later imageId?
		UINT32 bonus;
		UINT32 fppToEarn;
		UINT32 availAmount;
		PString phone;
		SrvDate birthDate;
		UINT64 privileges2;
		UINT64 effectivePrivileges2;
		UINT32 daysExclActual;
		SrvTime srvExclFrom;
		SrvTime srvExclUntil;
		PString firstName;
		PString lastName;
		PString fiscalCode;
		Atf::LAtfVector< P_UserProperty, 4 > userProps;
		UINT32 vipCounter1;
		UINT16 vipCounter1Cents;
		UINT32 vipCounter2;
		UINT16 vipCounter2Cents;
		UINT64 correctedUserFlags2;
		UINT32 imageId;
		PString sex;
		PString mobile;
		bool addressStateLocked;
		SrvTime registered;
		PString promoName;
		UINT32 blockingCode;
		UINT64 privileges3;
		UINT64 effectivePrivileges3;
		INT8 missedMonths;
		SrvDate vipStatusUntil;
		INT8 missableMonthsYearly;
		UINT64 correctedUserFlags3;
		BrandProt::BrandImageList brandImages;
		UINT32 hallOfFameLevel;
		UINT32 licenseId;

		Protocol_DBM_A_USER_INFO();
		void clear() override;
		bool equals(const Protocol_DBM_A_USER_INFO&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return DBM_A_USER_INFO; }
		const char *getMsgIdString() const override { return "DBM_A_USER_INFO"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};
};

#endif