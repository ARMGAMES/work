/**
 * VipStore_atf_thin.h
 *
 * This file was auto-generated from VipStore_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin VipStore_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_VIPSTORE_ATF_THIN_H
#define ATF_PROTOCOL_DEFFILE_VIPSTORE_ATF_THIN_H

#include "VipStoreCommon.h"
#include "cashier.h"
#include "FppStoreOrder.h"


#include "ThinAtfUserDefStructs.h"

namespace VipStore
{
	class ProtocolFactory
	{
	public:
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	namespace cli
	{
		struct VipStoreAlternateClientAddress
		{
			// 4ssssssssss - addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode
			UINT32 addrId;
			PString userId;
			PString fullName;
			PString addr1;
			PString addr2;
			PString city;
			PString state;
			PString country;
			PString code;
			PString phone;
			PString fiscalCode;

			VipStoreAlternateClientAddress();
			void clear();
			bool equals(const VipStoreAlternateClientAddress&) const;
			const char *toTraceString(PString& _str) const;
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			VipStoreAlternateClientAddress(VipStoreAlternateClientAddress&&);
			VipStoreAlternateClientAddress& operator=(VipStoreAlternateClientAddress&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			VipStoreAlternateClientAddress(const VipStoreAlternateClientAddress&) = default;
			VipStoreAlternateClientAddress& operator=(const VipStoreAlternateClientAddress&) = default;
#endif
#endif
		};

		struct FppStoreOrder
		{
			// 44ssssst4<s>4ss4444ssssssssss4 - orderId,price,userId,itemDescr,status,trackingId,comment,when,numSpecs<specs>,itemId,shipper,trackingUrl,itemQuantity,cartId,fulfillmentId,addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode,flags
			UINT32 orderId;
			UINT32 price;
			PString userId;
			PString itemDescr;
			PString status;
			PString trackingId;
			PString comment;
			CommSrvTime when;
			ThinAtf::LAtfVector< PString, 4 > specs;
			UINT32 itemId;
			PString shipper;
			PString trackingUrl;
			UINT32 itemQuantity;
			UINT32 cartId;
			UINT32 fulfillmentId;
			VipStoreAlternateClientAddress altAddr;
			UINT32 flags;

			FppStoreOrder();
			void clear();
			bool equals(const FppStoreOrder&) const;
			const char *toTraceString(PString& _str) const;
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			FppStoreOrder(FppStoreOrder&&);
			FppStoreOrder& operator=(FppStoreOrder&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			FppStoreOrder(const FppStoreOrder&) = default;
			FppStoreOrder& operator=(const FppStoreOrder&) = default;
#endif
#endif
		};

		struct OrderHistoryDigestData
		{
			// 44 - itemId,numPurchased
			UINT32 itemId;
			UINT32 numPurchased;

			OrderHistoryDigestData();
			void clear();
			bool equals(const OrderHistoryDigestData&) const;
			const char *toTraceString(PString& _str) const;
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2
		{
			// s - notUsedStr
			PString notUsedStr;	// used to be userid

			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_ORDER_HISTORY2; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_ORDER_HISTORY2"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2&&);
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2& operator=(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2&) = default;
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2& operator=(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY
		{
			// 2[s|4<44ssssst4<s>4ss4444ssssssssss4>4<44ssssst4<s>4ss4444ssssssssss4>] - errCode[errStr|numOrders<orderId,price,userId,itemDescr,status,trackingId,comment,when,numSpecs<specs>,itemId,shipper,trackingUrl,itemQuantity,cartId,fulfillmentId,addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode,flags>,numPreMidgrationOrders<orderId,price,userId,itemDescr,status,trackingId,comment,when,numSpecs<specs>,itemId,shipper,trackingUrl,itemQuantity,cartId,fulfillmentId,addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode,flags>]
			INT16 errCode;

			PString errStr;

			ThinAtf::LAtfVector< FppStoreOrder, 4 > orders;
			ThinAtf::LAtfVector< FppStoreOrder, 4 > preMidgrationOrders;

			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY&&);
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY& operator=(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY&) = default;
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY& operator=(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST
		{
			// s - userid
			PString userid;

			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST&&);
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST& operator=(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST&) = default;
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST& operator=(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY
		{
			// 2[s|4<44>] - errCode[errStr|numData<itemId,numPurchased>]
			INT16 errCode;

			PString errStr;

			ThinAtf::LAtfVector< OrderHistoryDigestData, 4 > data;

			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY&&);
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY& operator=(Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY&) = default;
			Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY& operator=(const Protocol_MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM
		{
			// s4s44ssssssssss - notUsedStr,itemId,couponCode,itemQuantity,addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode
			PString notUsedStr;	// used to be userid
			UINT32 itemId;
			PString couponCode;	// client does not limit the length correctly (should be 20 max)
			UINT32 itemQuantity;
			VipStoreAlternateClientAddress alternateAddress;

			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_PREPURCHASE_ITEM; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_PREPURCHASE_ITEM"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM(Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM&&);
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM& operator=(Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM(const Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM&) = default;
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM& operator=(const Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY
		{
			// 2[s|444] - errCode[errStr|itemCost,couponDiscount,refund]
			INT16 errCode;

			PString errStr;

			UINT32 itemCost;
			UINT32 couponDiscount;
			UINT32 refund;	// not sure what this should be called?

			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY(Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY&&);
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY& operator=(Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY(const Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY&) = default;
			Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY& operator=(const Protocol_MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_PURCHASE_ITEM
		{
			// s44<s>ss44ssssssssss - parsedUserId,itemId,numSpecs<specs>,userPhone,couponCode,itemQuantity,addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode
			PString parsedUserId;
			UINT32 itemId;
			ThinAtf::LAtfVector< PString, 4 > specs;
			PString userPhone;
			PString couponCode;	// client does not limit the length correctly (should be 20 max)
			UINT32 itemQuantity;
			VipStoreAlternateClientAddress alternateAddress;

			Protocol_MSG_VIPSTORE_PURCHASE_ITEM();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_PURCHASE_ITEM&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_PURCHASE_ITEM; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_PURCHASE_ITEM"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM(Protocol_MSG_VIPSTORE_PURCHASE_ITEM&&);
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM& operator=(Protocol_MSG_VIPSTORE_PURCHASE_ITEM&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM(const Protocol_MSG_VIPSTORE_PURCHASE_ITEM&) = default;
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM& operator=(const Protocol_MSG_VIPSTORE_PURCHASE_ITEM&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY
		{
			// 2[s|84444b] - errCode[errStr|fppBalance,totalCost,discount,couponRemainder,giftCardSpecialType,isInstantBonus]
			INT16 errCode;

			PString errStr;

			UINT64 fppBalance;
			UINT32 totalCost;
			UINT32 discount;
			UINT32 couponRemainder;
			UINT32 giftCardSpecialType;
			bool isInstantBonus;

			Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_PURCHASE_ITEM_REPLY; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_PURCHASE_ITEM_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY(Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY&&);
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY& operator=(Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY(const Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY&) = default;
			Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY& operator=(const Protocol_MSG_VIPSTORE_PURCHASE_ITEM_REPLY&) = default;
#endif
#endif
		};

		struct InvItemStockCli2
		{
			// M(b4b4<s>) - M(available,stockCount,unlimitedStock,numSpecs<specs>)
			bool available;
			INT32 stockCount;
			bool unlimitedStock;
			ThinAtf::LAtfVector< PString, 4 > specs;

			InvItemStockCli2();
			void clear();
			bool equals(const InvItemStockCli2&) const;
			const char *toTraceString(PString& _str) const;
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			InvItemStockCli2(InvItemStockCli2&&);
			InvItemStockCli2& operator=(InvItemStockCli2&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			InvItemStockCli2(const InvItemStockCli2&) = default;
			InvItemStockCli2& operator=(const InvItemStockCli2&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2
		{
			// 44s - itemId,specialType,unusedCouponCode
			UINT32 itemId;
			UINT32 specialType;
			PString unusedCouponCode;	// this string is not used by server any more but current client still sends it

			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_INV_GET_ITEM_STOCK2; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_INV_GET_ITEM_STOCK2"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2(Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2&&);
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2& operator=(Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2(const Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2&) = default;
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2& operator=(const Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY
		{
			// 2[s|4<M(b4b4<s>)>] - errCode[errStr|numStocks<M(available,stockCount,unlimitedStock,numSpecs<specs>)>]
			INT16 errCode;

			PString errStr;

			ThinAtf::LAtfVector< InvItemStockCli2, 4 > stocks;

			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY(Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY&&);
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY& operator=(Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY(const Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY&) = default;
			Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY& operator=(const Protocol_MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER
		{
			// s4 - parsedUserId,orderId
			PString parsedUserId;
			UINT32 orderId;

			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_CANCEL_ORDER; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_CANCEL_ORDER"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER&&);
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER& operator=(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER&) = default;
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER& operator=(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER&) = default;
#endif
#endif
		};

		// this may not be correct
		struct Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY
		{
			// 2[s|4] - errCode[errStr|userIntId]
			INT16 errCode;

			PString errStr;

			UINT32 userIntId;

			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY&&);
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY& operator=(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY&) = default;
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY& operator=(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS
		{
			// s4<4> - parsedUserId,numOrderIds<orderIds>
			PString parsedUserId;
			ThinAtf::LAtfVector< UINT32, 4 > orderIds;

			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_CANCEL_ORDERS; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_CANCEL_ORDERS"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS&&);
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS& operator=(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS&) = default;
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS& operator=(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS&) = default;
#endif
#endif
		};

		// this may not be correct
		struct Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY
		{
			// 2[s|4] - errCode[errStr|userIntId]
			INT16 errCode;

			PString errStr;

			UINT32 userIntId;

			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY();
			void clear();
			bool equals(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY; }
			static const char *getMsgIdString() { return "MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY&&);
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY& operator=(Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY&) = default;
			Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY& operator=(const Protocol_MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES
		{

			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES();
			void clear();
			bool equals(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return VIPSTORE_GET_USER_ALT_ADDRESSES; }
			static const char *getMsgIdString() { return "VIPSTORE_GET_USER_ALT_ADDRESSES"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY
		{
			// 2[s|44<4ssssssssss>] - errCode[errStr|maxSuggestedAltAddrLimit,numAltAddresses<addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode>]
			INT16 errCode;

			PString errStr;

			UINT32 maxSuggestedAltAddrLimit;
			ThinAtf::LAtfVector< VipStoreAlternateClientAddress, 4 > altAddresses;

			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY();
			void clear();
			bool equals(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY; }
			static const char *getMsgIdString() { return "VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY(Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY&&);
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY& operator=(Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY&) = default;
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY& operator=(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2
		{
			// s - unusedStr
			PString unusedStr;	// this string is not used by server any more but current client still sends it

			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2();
			void clear();
			bool equals(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return VIPSTORE_GET_USER_ALT_ADDRESSES2; }
			static const char *getMsgIdString() { return "VIPSTORE_GET_USER_ALT_ADDRESSES2"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2(Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2&&);
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2& operator=(Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2&) = default;
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2& operator=(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2&) = default;
#endif
#endif
		};

		struct Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY
		{
			// 2[s|44<4ssssssssss>] - errCode[errStr|maxSuggestedAltAddrLimit,numAltAddresses<addrId,userId,fullName,addr1,addr2,city,state,country,code,phone,fiscalCode>]
			INT16 errCode;

			PString errStr;

			UINT32 maxSuggestedAltAddrLimit;
			ThinAtf::LAtfVector< VipStoreAlternateClientAddress, 4 > altAddresses;

			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY();
			void clear();
			bool equals(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY; }
			static const char *getMsgIdString() { return "VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY(Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY&&);
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY& operator=(Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY&) = default;
			Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY& operator=(const Protocol_VIPSTORE_GET_USER_ALT_ADDRESSES2_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS
		{
			// 4 - addrId
			UINT32 addrId;

			Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS();
			void clear();
			bool equals(const Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return VIPSTORE_OBSOLETE_USER_ALT_ADDRESS; }
			static const char *getMsgIdString() { return "VIPSTORE_OBSOLETE_USER_ALT_ADDRESS"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY
		{
			// 2[s] - errCode[errStr]
			INT16 errCode;

			PString errStr;

			Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY();
			void clear();
			bool equals(const Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY; }
			static const char *getMsgIdString() { return "VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY(Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY&&);
			Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY& operator=(Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY(const Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY&) = default;
			Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY& operator=(const Protocol_VIPSTORE_OBSOLETE_USER_ALT_ADDRESS_REPLY&) = default;
#endif
#endif
		};
	};
};

#endif
