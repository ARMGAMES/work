//gateway.h
//cash processing gateway interface
#ifndef gateway_h_included
#define gateway_h_included

//see CashierServerObject::receiveGateway2CachInReply
#define REJECT_REASON_NONE		0
#define REJECT_REASON_OTHER		1
#define REJECT_REASON_GAMBLE	7995
#define PENDING_REASON_NOT_STARTED	1000
#define PENDING_REASON_OTHER		1001

#define PAYTRU_NSF_ERROR            55
#define PAYTRU_CSV_EXPIRED          56
#define X3D_SECURE_REQUIRED_ERROR	73 // see 6480 for used codes
#define X3D_SECURE2_REQUIRED		173 // PYR-80344, GW->Cashier, 3DSv2 processing
#define X3D_EXEMPTION_SOFT_DECLINE  273 // PYR-80344, GW->Cashier, 3DSv2 exemption

#define PPAL_REVERSED			0
#define PPAL_CHARGEBACK			1
#define PPAL_DISPUTE			2
#define PPAL_COMPLAINT			3
#define PPAL_CANCELLEDREVERSAL	4

#define PAGO_CIP_GENERATED   "G"
#define PAGO_CIP_PAID        "P"
#define PAGO_CIP_EXPIRED     "E"

#define VANILLARELOAD_ACCOUNT   1
#define VANILLARELOAD_DEPOSIT   2
#define VANILLARELOAD_REVERSAL  3

#define MAXIMUM_NOTIFICATION_LENGTH     (2048 - (256 - 218)) // IOS 8, 218 for IOS 7

#define PROFEE_CHARGEBACK          0
#define PROFEE_REVERSE_CHARGEBACK  1

enum eGcHostedVerifyFlags // MSG_GCHOSTED_VERIFY.FLAGS
{
	eGcHostedVerifyFlag_PayerInfoChecked, // PYR-33629 if set then paypal will NOT send MSG_CASHIER_CHECK_PAYPAL_EMAIL_AND_PAYERID intermedite back-request on MSG_GCHOSTED_VERIFY
};

#define MSG_GATEWAY_CASHIN			6000
//	4441s21ssssssssss
//		reqId transId amount cardType, cardnum cardexpYr cardexpMon custid fullName
//		street street2 city state country zip email
// 'V' = VISA, 'M' = MASTERCARD, 'F' = FIREPAY

#define MSG_GATEWAY_CASHIN_REPLY    6001
//(4)4414s2<R>
//(reqId) merchantTxn, txnNumber, status, amountreturned, errorInfo, numResults< Result >
//Result 114s - type, action, amount, reference

#define MSG_GATEWAY_CASHOUT			6002
/// 44s411[4ssssssss]
/// reqId, transId, custId, amount, paysystem, noCashins[ fakeCashinTransId fullName street street2 city state country zip email ]
//possible paysystems are: [P]aypal / che[Q]ue
// if noCashins != 0, ONLY then parse what is in [...]:

#define MSG_GATEWAY_CASHOUT_REPLY	6003
//(4)4414s2<R>
//(reqId) merchantTxn, txnNumber, status, amountreturned, errorInfo, numResults< Result >
//Result - to be defined later...

/////////////////// message from/to PayPal IPN receiver

///from IPN receiver to cashier
#define MSG_PAYPAL_IPN				6500 
//44s1144s1s12<ss>
//reqId, our_txn_id, invoiceStr, status, pending_reason, payment_gross,
//payment_fee, ext_txn_id, txn_type, payer_email, payment_type, numPairs<name, value>
//NB for details on status, pending_reason, txn_type, payment_type - please see IPN_* constants in "dbm.h"

///from cashier back to IPN receiver
#define MSG_PAYPAL_IPN_REPLY		6501 //46[s] reqId, err_code[errDescr]


//cashier to gateway2
#define MSG_GATEWAY2_CASHIN				6701
// 44s1s21ssssss4s1Tssss4bs4
// reqId amount_cents fullName ccType ccNumber ccExpiryYear ccExpiryMonth addr_1 addr_2 
// city state country zip transId email isRepeat started ipOnRequest currency issueNum startdate
// userIntId x3dSupport, userId, liscenseId
// ccType: 'V' = VISA, 'M' = MASTERCARD, 'F' = FIREPAY

//gateway2 to cashier
#define MSG_GATEWAY2_CASHIN_REPLY		6702
// 41s444
// reqId status xTransIdStr errorInfo transId rejectReason rejectExplanationCode// transId is redundant, just in case

#define MSG_GATEWAY2_CASHOUT			6703 
//444B
//reqId amount transId purchaseBody

#define MSG_GATEWAY2_CASHOUT_REPLY		6704
// 41s4
// reqId status xTransIdStr errorInfo transId // transId is redundant, just in case

#define MSG_NETELLER_CASHIN				6705
//44ss4 reqId ourTransId netAccount secureId transAmount
#define MSG_NETELLER_CASHIN_REPLY		6706
//44144s4s reqId ourTransId status returnedAmount fee errorInfo pendingOrRejectReason xTransStr

#define MSG_GFSL_OVERCREDIT				6707	
// 44s1ss21ssssss4s1sssss
// reqId amount_cents fullName ccType ccNumber ccCvv ccExpiryYear ccExpiryMonth addr_1 addr_2 
// city state country zip transId email repeat xtransStr
// ipAddress currency userId productId

#define MSG_GFSL_OVERCREDIT_REPLY		6708	//
// 41ss4s
// reqId status xTransIdStr errorInfo transId errCode // transId is redundant, just in case

// ...
#define MSG_GATEWAY3_CASHOUT			6709 // same as MSG_GATEWAY2_CASHOUT
#define MSG_GATEWAY3_CASHOUT_REPLY		6710 // same as MSG_GATEWAY2_CASHOUT_REPLY

// ...
#define MSG_GATEWAY3_OVERCREDIT			6711 //same as MSG_GATEWAY3_CASHOUT
#define MSG_GATEWAY3_OVERCREDIT_REPLY	6712 //same as MSG_GATEWAY3_CASHOUT_REPLY

#define MSG_ACH_CASHIN					6713 //
// 44s1s21ssssss4s1TsssssDss4
// (copy of MSG_GATEWAY2_CASHIN plus secNum, bDate, dLicense, userPhone)
// reqId amount_cents fullName ccType ccNumber ccExpiryYear ccExpiryMonth addr_1 addr_2 
// city state country zip transId email isRepeat started ipOnRequest currency issueNum startdate
// secNum bDate dLicense userPhone user_lt_fpp

#define MSG_ACH_CASHIN_REPLY			6714 // same as MSG_GATEWAY2_CASHIN_REPLY + returnAmt

#define MSG_ACH_CASHOUT					6715 // same as MSG_GATEWAY2_CASHOUT
#define MSG_ACH_CASHOUT_REPLY			6716 // same as MSG_GATEWAY2_CASHOUT_REPLY

#define MSG_ACH_OVERCREDIT				6717 // same as MSG_GFSL_OVERCREDIT
#define MSG_ACH_OVERCREDIT_REPLY		6718 // same as MSG_GFSL_OVERCREDIT_REPLY + returnAmt

#define MSG_ACH_NOTIFY                          6719
//44s1ss4414s requestId transId xTransStr transType('P'/'R') currency(USD/CAD) maskedAcctNum amtRequested amtPaid status('A'/'R') authCode authMsg
#define MSG_ACH_NOTIFY_REPLY                    6720
//42 requestId errCode

#define MSG_REQUEST_WIRE                        6721 //444sssssssssss reqId orderId amount currency ip fullname addr city zip state country email productId personalInfo
#define MSG_REQUEST_WIRE_REPLY                  6722 //44ssssssssss reqId orderId result erroInfo paymentReference accountHolder swift city bankAcct bankName country additionalRef 

#define URU_AGECHECK_UK                         6724 //4sssDsssssssssssDsssss reqid, title, fullname, gender, dateofbirth, address, city, zip, state, country, passportNumber1, passportNumber2, passportNumber3, passportNumber4, passportNumber5, passportNumber6, passportExprireDate, driverNumber1, driverNumber2, driverNumber3,driverNumber4, driverpostcode
#define URU_AGECHECK_UK_REPLY                   6725 

#define MSG_GCDEBIT_CASHIN                      6726 //4b4ssssssssssss reqId recover orderId amount currency ip fullname addr city zip state country email bankcode accountnumber accountname
#define MSG_GCDEBIT_CASHIN_REPLY                6727 // reqId status xTransIdStr errorInfo transId rejectReason rejectExplanationCode, mandateId, strErrCode

#define MSG_GCDEBIT_CASHOUT						6728 // same as MSG_GATEWAY2_CASHOUT
#define MSG_GCDEBIT_CASHOUT_REPLY				6729 // same as MSG_GCDEBIT_CASHOUT_REPLY

#define MSG_GCDEBIT_VALIDATE					6730 //4ssssM reqId bankCode accNum holderName countryCode opaqueMsgBody
#define MSG_GCDEBIT_VALIDATE_REPLY				6731 //44sM reqId errCode32 errStr opaqueMsgBody

#define EPN_VALIDATE_EP_PURCHASE                6732 //444s4sssssss reqId transId EPAcctnum productcode amount total_amount(include S/H and Tax) EPUsername debit_trans_idx credit_trans_idx isAgeOver18 IPaddress ApprovalString
#define EPN_VALIDATE_EP_PURCHASE_REPLY          6733 //444sss reqId transId totalamount debit_trans_idx credit_trans_idx status

#define EPN_EP_CONFIRMED                        6734 //44ss   reqId transId status errorInfo
#define EPN_EP_CONFIRMED_REPLY                  6735 //44   reqId transId

#define MSG_GCHOSTED_CASHIN                     6736 // 44s1s21ssssss4s1Tssss4ss4
// reqId amount_cents fullName ccType ccNumber ccExpiryYear ccExpiryMonth addr_1 addr_2 
// city state country zip transId email isRepeat started ipOnRequest currency issueNum startdate
// paymentId, prefLanguage, userId, liscenseId
#define MSG_GCHOSTED_CASHIN_REPLY               6737 //414sssssss reqId, status, transId, errorInfo, formmethod, formaction, ref, mac, returnmac

#define MSG_GCHOSTED_VERIFY                     6738 //44s44sss4s  reqId, transId, country, licenseId, converted, procCurrency, xTransStr, cardNum, requestFlags, extPayerId
#define MSG_GCHOSTED_VERIFY_REPLY               6739 //414ssts reqId, status, transId, errorInfo, paymentReference, ResponseTime, errCode

#define MSG_VERIFY_AGE                          6740 //4ssssssssDssss  reqId,full_name,gender,address,city,state,zip,county,country,DOB,phone,SSN4,id,id_type,userId		
#define MSG_VERIFY_AGE_REPLY                    6741 //4ssss44s reqId, firstname,middle,lastname,xTransStr,matchCode,errCode,errDescr,userId

#define MSG_SET_MCAGESID                        6742 //4s  reqId,sid
#define MSG_SET_MCAGESID_REPLY                  6743 //44s  reqId, status(0-suucess, -1-fail) errStr

#define MSG_GET_MCAGESID                        6744 //4  reqId
#define MSG_GET_MCAGESID_REPLY                  6745 //4s  reqId, sid

#define MSG_MONEYGRAM_FILE_NOTIF                6746 //4  reqId

#define MSG_MONEYGRAM_TRANS_TOTAL               6747 //44 reqId, total

#define MSG_MONEYGRAM_TRANS                     6748 //4sss4s reqId xTransStr(Moneygram Ref#),issueTime,sender,amount,account#
#define MSG_MONEYGRAM_TRANS_REPLY               6749 //4 reqId

#define MSG_MONEYGRAM_TRANS_TOTAL_REPLY         6750 //4 reqId

#define MSG_MRMC_SIGNUP                         6751 //414s4tt44ss44ssssssssssDsss  reqId, recover, accountId, userId,userIntId, requested, completed, status, returnCode, errorInfo,cardNum,expYear,expMonth,userUrl,fullname,address,city,state,country,zip,question,answer,ssn,userDOB,phone,userIP,email
#define MSG_MRMC_SIGNUP_REPLY                   6752 //same as 6751

#define MSG_MRMC_NOTIFY                         6753
//44s1ss4414s requestId transId xTransStr transType('P'/'R') currency(USD/CAD) maskedAcctNum amtRequested amtPaid status('A'/'R') authCode authMsg
#define MSG_MRMC_NOTIFY_REPLY                   6754
//42 requestId errCode

#define MSG_MBOOKERS_CASHIN						6755 //same as MSG_GATEWAY2_CASHIN
#define MSG_MBOOKERS_CASHIN_REPLY				6756 //441s4s reqId, transId, status, sessionId errCode, errMsg

//cashier to MB gateway
#define MSG_MBOOKERS_REQUEST_NOTIFY             6757 //44s reqId, transId, currency
//MB gateway to cashier
#define MSG_MBOOKERS_NOTIFY                     6758 //4s44ss4s4ss[s] reqId, internalStatus, status, transId, user Email,xTransStr,mb_amount,mb_currency,orig_amount, orig_currency, errorInfo[, errCode]
//cashier to MB gateway
#define MSG_MBOOKERS_NOTIFY_REPLY               6759 //4 reqId

#define MSG_WUNION_GET_RECEIVER					6760 //444ssssss reqId, transId, userIntId, fullName, phone, city, state, country userAmount
#define MSG_WUNION_GET_RECEIVER_REPLY			6761 //4ss4sssssss reqId, status, errMsg, transId, transState, xTransStr, merchantCode,receiverCode, receiverName, receiverCity, receiverState, receiverCountry

//cashier to wu_gateway
#define MSG_MTCN_VALIDATE						6762 //4144sssss4sss reqId, recover, transId, userIntId, fullname, phone, city, state, country, amount, currency, mtcn, receiverName
#define MSG_MTCN_VALIDATE_REPLY					6763 //4ss44st reqId, status, errMsg, transId, amount, xTransStr
//wu_gateway to cashier
#define MSG_MTCN_NOTIFY							6764 //44s4sTss4s44sss reqId, transId, xTranstr, userIntId, mtcn, datetime, transState,transType, amount, currency, transferFee, EptTotalFee, comments, receiverName, senderName
#define MSG_MTCN_NOTIFY_REPLY					6765 //4 reqId

// MB gateway to cashier
#define MSG_MBOOKERS_REQUEST_NOTIFY_REPLY       6766 //44ss reqId transid status errMsg

#define MSG_USEMYBANK_NOTIFY                    6767 //44ss444s reqId,transId,status,confirmdata,amount,orignalAmount,converted,convrate
#define MSG_USEMYBANK_NOTIFY_REPLY              6768 //4 reqId

#define MSG_USEMYBANK_REQUEST_INFO              6769 //44 reqId,transId
#define MSG_USEMYBANK_REQUEST_INFO_REPLY        6770 //44sss444s reqId,transId,status,errStr,confirmdata,amount,orignalAmount,converted,convrate

#define MSG_EXCHANGERATE_UPDATE                 6771 //44<s44st> reqId, total, <currency,rateAsk,rateMid,errorInfo,time>
#define MSG_EXCHANGERATE_UPDATE_REPLY           6772 //4 errCode

#define MSG_U_EXCHANGERATE_UPDATE				 6773 //

#define MSG_DBTPAYOUT_GET_INFO                   6774 //4ss reqId, userId, country;
#define MSG_DBTPAYOUT_GET_INFO_REPLY             6775 //4sss44<sss>4<ss444s> reqId,userId,currency,errInfo,dbtAccountId,total of account fields,<fieldName, fieldDesc, fieldValue>,total of country specific needed fields,<fieldname,fieldDesc,minLen,maxlen,flags,fieldExtras>

#define MSG_DBTPAYOUT_BANK_VALIDATE              6776 //4sss4<ss>2M(ssssssssss) reqId, DbtBankValidateTrans as userId, country, currency, total, <filedname, fieldvalue> bankValidationCode M(userFields)
#define MSG_DBTPAYOUT_BANK_VALIDATE_REPLY        6777 //4sss  reqId,userId, status, errInfo

// 3D-secure notification SOLO/SWITCH from GFSL listener to cashier 
#define MSG_3DSECURE_NOTIFY                      6778
//44s1ss4414s requestId transId xTransStr transType('P'/'R') currency(GBP) maskedAcctNum amtRequested amtPaid status('A'/'R') authCode(990 only) authMsg
#define MSG_3DSECURE_NOTIFY_REPLY                6779

#define MSG_CLICKNBUY_NOTIFY                     6780 //44s4s reqId,transId,xTransId,retamount,currency
#define MSG_CLICKNBUY_NOTIFY_REPLY               6781 //4s status

#define MSG_CLICKNBUY_ACTION_NOTIFY              6782 //44<ss> reqId,total <name(in lower case), value>
#define MSG_CLICKNBUY_ACTION_NOTIFY_REPLY        6783 //4 reqId

// Obsolete after PYR-49205
// #define MSG_CITADEL_GET_BANKINFO                 6784 //4sss(B) reqId, country,currency,locale,contextBody
// #define MSG_CITADEL_GET_BANKINFO_REPLY           6785 //4(B)4s4<(CitadelBankInfoStruct)> reqId,contextBody,errNo,errMsg,total,<CitadelBankInfoStruct>
// #define MSG_CITADEL_SAVE_USER_CHOICE			 6786 //4(B)ssss4(CitadelBankInfoStruct) reqId contextBody userId fullName bonusCode requestedAmount CitadelBankInfoStruct
// #define MSG_CITADEL_SAVE_USER_CHOICE_REPLY		 6787 //4(B)4ss reqId contextBody errNo errMsg uniqueRef
// #define MSG_CITADEL_TRANS_COMPLETE				6788 //4s4sss4 reqId userId amt currency xtransStr bonusCode status
// #define MSG_CITADEL_TRANS_COMPLETE_REPLY		6789 //46[s|4] reqId errCode[errDescr|transId]
//42 requestId errCode

#define MSG_C2PAY_NOTIFY						6790 //44ss4ssss reqId purchaseTransId xTransStr account amount currency status reason returnDateStr
#define MSG_C2PAY_NOTIFY_REPLY					6791 //4 reqId

#define MSG_C2PAY_COLLECTION                    6792 //44ss4sssss reqId purchaseTransId xTransStr account amount currency transDate chargeDate repechageDate repechageChargeDate
#define MSG_C2PAY_COLLECTION_REPLY				6793 //4 reqId


#define MSG_VOUCHER_CASHIN                      6794 //same as GC_HOSTED_CASHIN
#define MSG_VOUCHER_CASHIN_REPLY                6795 //41ss4444s reqId status xtransStr errorInfo transId rejectreason rejectExplanation retAmount errcodeStr

#define MSG_DINEROMAIL_NOTIFY                   6796 //44 reqId, transId,amount,currency,xTransStr,status,errorInfo
#define MSG_DINEROMAIL_NOTIFY_REPLY             6797 //4 reqId

/*
const char RESULT_APPROVED =	'A';
const char RESULT_REJECTED =	'R';
const char RESULT_PENDING =		'P';
*/

//
// TMI server messages (admin to TMI)
//
#define MSG_TMI_SUBMIT_BATCH				6801
// 42<4s4ssssssssss>
// reqId numCheques <transId currency amount name streetAddr streetAddr2 city state zip country phone email shippingMethod>
// all fields are from CashTransaction unless noted otherwise
// shippingMethod = "mail"|"fedex" verbatim
// amount must be positve and converted to needed currency

#define MSG_TMI_SUBMIT_BATCH_REPLY			6802
// 42
// reqId status

#define MSG_TMI_GET_BATCHES					6803
// 4
// reqId

#define MSG_TMI_GET_BATCHES_REPLY 			6804
// 422<s4>
// reqId status numCheques <batchId transId>

#define MSG_TMI_UPDATE_BATCH 				6805
// 4s
// reqId batchId

#define MSG_TMI_UPDATE_BATCH_REPLY      	6806
// 422<s4>
// reqId status numCheques <batchId transId> (same as previous reply)

#define MSG_TMI_GET_LOST_CHEQUES			6807
// 4
// reqId

#define MSG_TMI_GET_LOST_CHEQUES_REPLY  	6808
// 422<4>
// reqId status numCheques <transId>

#define MSG_TMI_UPDATE_LOST_CHEQUES     	6809 
// 42<41[sss]>
// reqId numCheques <transId transStatus [chequeNum wayBill whenMailed]>
// compose [...] for status "A" only, don't compose for "R" or "E"

#define MSG_TMI_UPDATE_LOST_CHEQUES_REPLY 	6810
// 422<4>
// reqId status numCheques <transId> (same as previous reply)

#define MSG_TMI_GET_FINALIZED_CHEQUES 		6811
// 42<4>
// reqId numCheques <transId>

#define MSG_TMI_GET_FINALIZED_CHEQUES_REPLY 6812
// 422<41[ssD]>
// reqId status numCheques <transId transStatus [chequeNum wayBill whenMailed]>
// [...] is composed only for "A" status, not for "R" or "E"

//
// TMI gateway messages (server to gateway)
//
#define MSG_TMIGW_SUBMIT_BATCH				6851
// 42<4s4ssssssssss>
// reqId numCheques <transId currency amount name streetAddr streetAddr2 city state zip country phone email shippingMethod>
// all fields are from CashTransaction unless noted otherwise
// shippingMethod = "mail"|"fedex" verbatim
// amount must be positve and converted to needed currency

#define MSG_TMIGW_SUBMIT_BATCH_REPLY		6852
// 41s
// reqId status batchId

#define MSG_TMIGW_UPDATE_BATCH 				6853
// 4s
// reqId batchId

#define MSG_TMIGW_UPDATE_BATCH_REPLY      	6854
// 41s2<414s[ssD]>
// reqId status batchId numCheques <transId transStatus messageCode message [chequeNum wayBill whenMailed]>
// [...] is composed only for "A" status, not for "R" or "E"
// messageCode and message are always composed

#define MSG_AFFDEPOSIT_NOTIFY				6855 //4ss4s4444 reqId UserId AffiliateId DealId xTransStr AmountUSD TargetFpp ExpiryDays Flags 
#define MSG_AFFDEPOSIT_NOTIFY_REPLY			6856 //46[s|4] reqId errCode[errDescr|cashTransId]

#define MSG_WEBMONEY_TRANS_CHECK			6857 //444ss reqId, transId,amount,currency,customer WMID
#define MSG_WEBMONEY_TRANS_CHECK_REPLY		6858 //441 reqId, transId, status

#define MSG_WEBMONEY_TRANS_NOTIFY			6859 //444sssst reqId,transId,amount,currency,xTransStr,customer’s purse,customerWMID,transtime
#define MSG_WEBMONEY_TRANS_NOTIFY_REPLY		6860 //44 reqId, transId

#define MSG_ENVOY_TRANS_CHECK				6861 //444ss reqId, transId, amount currency xTransStr
#define MSG_ENVOY_TRANS_CHECK_REPLY			6862 //4ss reqId status ("A" or "R") errorInfo (empty for success)

#define MSG_ENVOY_TRANS_NOTIFY				6863 //44sss4ss reqId transId status(A or U) errorInfo (empty for success) xTransStr amount currency bankInfo
#define MSG_ENVOY_TRANS_NOTIFY_REPLY		6864 //4 reqId

#define AGECHECK_192						6865 //4ssssDsssssssssssDsssss reqid, userId, title, fullname, gender, dateofbirth, address, city, zip, state, country, passportNumber1, passportNumber2, passportNumber3, passportNumber4, passportNumber5, passportNumber6, passportExprireDate, driverNumber1, driverNumber2, driverNumber3,driverNumber4, driverpostcode   #12779
#define AGECHECK_192_REPLY					6866 //444<ss> reqId, uruVarificationStatus-(statusVerified|statusNamePick|statusAddressPick|statusNetworkFailure)), counter, <name><value>

#define MSG_GATEWAY_PAPER_CHECK            6867 //444ssssssssss14 reqId,transId,amount,currency,fullName,addr1,addr2,city,state,zip,country,phone,shipment,recover,chequedate
#define MSG_GATEWAY_PAPER_CHECK_REPLY      6868 //44ss4s reqId,transId,status,xTransStr,pendingReason,errStr

#define MSG_CNBUY_DONOT_COLLECT				6869 //4s444s reqId purchaseXTransStr purchaseId cancelId amount currency
#define MSG_CNBUY_DONOT_COLLECT_REPLY		6870 //44s reqId purchaseId infoStr

#define MSG_GET_LAST_RETURN_DATE			6871 //4 reqId
#define MSG_GET_LAST_RETURN_DATE_PEPLY		6872 //44[t|s] reqId errCode [errInfo|srvTime]

#define MSG_SET_LAST_RETURN_DATE			6873 //44t reqId, num, srvTime
#define MSG_SET_LAST_RETURN_DATE_PEPLY		6874 //44[s] reqId, errCode [errInfo]

#define MSG_ATM_REQUEST_RETURNS				6875 //4tt reqId, srvTimeFrom, srvTimeTo
#define MSG_ATM_REQUEST_RETURNS_REPLY		6876 //44[s] reqId errCode [errInfo]

#define MSG_WIRE_OVERCREDIT					6877 //44{wireAccountInfo}4 transid,amount,struct wireAccountInfo,transState
#define MSG_WIRE_OVERCREDIT_REPLY			6878 //41ss4s reqId,status,xTransStr,errInfo,transId,errCodeStr

#define MSG_PSIP_NOTIFY						6879 //4ss4s4444 reqId UserId AffiliateId DealId xTransStr AmountUSD TargetFpp ExpiryDays Flags 
#define MSG_PSIP_NOTIFY_REPLY				6880 //46[s|4] reqId errCode[errDescr|cashTransId]

#define MSG_PSIP_VALIDATE_USER				6883 //4ss4s4444 reqId UserId AffiliateId DealId xTransStr AmountUSD TargetFpp ExpiryDays Flags 
#define MSG_PSIP_VALIDATE_USER_REPLY 		6884 //46[s|4] reqId errCode[errDescr]

#define MSG_ENVLBT_BANKSINFO				6885 //4 reqId, total, EnvoyCountryBanks (see gcfr.h)
#define MSG_ENVLBT_BANKSINFO_REPLY			6886 //4 reqId

#define MSG_ENVLBT_SAVE_USER_CHOICE			6887 //4(B)ssss4(BankInfoStruct) reqId contextBody userId fullName bonusCode requestedAmount BankInfoStruct
#define MSG_ENVLBT_SAVE_USER_CHOICE_REPLY	6888 //4(B)4ss reqId contextBody errNo errMsg uniqueRef

#define MSG_ENVLBT_TRANS_COMPLETE			6889 //4s4sss4sss reqId userId amt currency xtransStr bonusCode status, operatorname, gateway, paysystem
#define MSG_ENVLBT_TRANS_COMPLETE_REPLY		6890 //46[s|4] reqId errCode[errDescr|transId]

#define MSG_CASHIER_INSTADEBIT_PURCHASE_NOTIFY           6891 // InstaDebit Wallet
#define MSG_CASHIER_INSTADEBIT_PURCHASE_NOTIFY_REPLY     6892 //
#define MSG_CASHIER_INSTADEBIT_PURCHASE_CHECK            6893 //
#define MSG_CASHIER_INSTADEBIT_PURCHASE_CHECK_REPLY      6894 //
#define MSG_CASHIER_INSTADEBIT_RETURN_NOTIFY             6895 //
#define MSG_CASHIER_INSTADEBIT_RETURN_NOTIFY_REPLY       6896 //

#define MSG_CASHIER_UMWALLET_PURCHASE_NOTIFY             6897 // Use My Wallet
#define MSG_CASHIER_UMWALLET_PURCHASE_NOTIFY_REPLY       6898 //
#define MSG_CASHIER_UMWALLET_PURCHASE_CHECK              6899 //
#define MSG_CASHIER_UMWALLET_PURCHASE_CHECK_REPLY        6900 //

#define MSG_CASHIER_UMWALLET_USER_ACCT_REGISTRATION        6901 // 4sss reqId, userId, fullName, email
#define MSG_CASHIER_UMWALLET_USER_ACCT_REGISTRATION_REPLY  6902 // 46[s|sss] reqId, errorcode, [errmsg | userAccount, userPin, activationLink]

#define MSG_CASHIER_TICKETPREMIUM_PRE_PURCHASE             6903 // 4444s reqId, clientConnId, transId, amount, currency
#define MSG_CASHIER_TICKETPREMIUM_PRE_PURCHASE_REPLY       6904 // 4444s6[s|sss] reqId, clientConnId, transId, amount, currency, errcode [errmsg | mid, keyid, tid, url]

#define MSG_CASHIER_TICKETPREMIUM_PURCHASE_NOTIFY          6905 // Ticket premium
#define MSG_CASHIER_TICKETPREMIUM_PURCHASE_NOTIFY_REPLY    6906 //

#define MSG_CASHIER_XPAY_PAYPAL_PURCHASE_NOTIFY            6907 // 444ssss reqId, transId, amount, status(A/R), currency, xTransStr, errCode
#define MSG_CASHIER_XPAY_PAYPAL_PURCHASE_NOTIFY_REPLY      6908 // 46[s] reqId, errCode, [errMsg]

#define MSG_CASHIER_UKASH_MIGRATION_OVERCREDIT			   6909
// same as MSG_GFSL_OVERCREDIT
// 44s1ss21ssssss4s1sssss
// *reqId *amount_cents fullName ccType ccNumber ccCvv ccExpiryYear ccExpiryMonth addr_1 addr_2 
// city state country zip *transId email repeat xtransStr
// ipAddress *currency userId productId
// note: mandatory filds are marked with *

#define MSG_CASHIER_UKASH_MIGRATION_OVERCREDIT_REPLY	   6910	//
// 41ss4ss
// reqId status xTransIdStr errorInfo transId errCode voucherNumber 

#define MSG_CASHIER_UKASH_MIGRATION_CASHIN                 6911
// same as GC_HOSTED_CASHIN
// 44s1ss21ssssss4s1tssss4s
// *reqId *amount fullname ccType *ccNumber ccCvv ccExpiryYear ccExpiryMonth addr_1 addr_2
// city state country zip *transId email repeat  timeStarted ipAddress *currency isueNumber startDate productId language
// note: mandatory filds are marked with *
// note: ccNumber value should contain voucher number

#define MSG_CASHIER_UKASH_MIGRATION_CASHIN_REPLY           6912
// 41ss4444s
// reqId status xtransStr errorInfo transId rejectreason rejectExplanation retAmount errcodeStr

// #14606
#define MSG_CASHIER_ROBOKASSA_PURCHASE_NOTIFY              6913 // 444ssss reqId, transId, amount, status(A/R), currency, xTransStr, errCode
#define MSG_CASHIER_ROBOKASSA_PURCHASE_NOTIFY_REPLY        6914 // 46[s] reqId, errCode, [errMsg]

#define MSG_CASHIER_ROBOKASSA_PURCHASE_CHECK               6915 // 44 reqId, transId
#define MSG_CASHIER_ROBOKASSA_PURCHASE_CHECK_REPLY         6916 // 446[s]4s reqId, transId, errCode[errMsg], amount, currency

#define MSG_VERIFY_AGE_FAIL                                6917 // 4s reqId, userId		
#define MSG_VERIFY_AGE_FAIL_REPLY                          6918 // 46[s] reqId, errcode[errstr]

// #17121
#define MSG_CASHIER_PAGOSONLINE_PURCHASE_NOTIFY            6919
#define MSG_CASHIER_PAGOSONLINE_PURCHASE_NOTIFY_REPLY      6920
#define MSG_CASHIER_PAGOSONLINE_PRE_PURCHASE               6921
#define MSG_CASHIER_PAGOSONLINE_PRE_PURCHASE_REPLY         6922

#define MSG_WORLDPAY_HOSTED_CASHIN                         6923 //444sssssssss1sss4 reqId transId, amount_cents currency fullName addr_1 addr_2 city state country zip email isRepeat paymentMethod prefLanguage, userId, liscenseId
#define MSG_WORLDPAY_HOSTED_CASHIN_REPLY                   6924 //414ssss reqId, status, transId, errorInfo, strErrCode, xTransStr, redirectUrl
#define MSG_WORLDPAY_TRANS_NOTIFY                          6925 //444sss reqId transId amount currency xTransStr status
#define MSG_WORLDPAY_TRANS_NOTIFY_REPLY                    6926 //4 reqId

#define MSG_SOFORT_TRANS_NOTIFY                            6927 //444ss reqId transId amount currency xTransStr
#define MSG_SOFORT_TRANS_NOTIFY_REPLY                      6928 //4 reqId

// #19820
#define MSG_CASHIER_QIWIWALLET_PRE_PURCHASE             6929 // 4444s4 reqId, clientConnId, transId, amount, currency, cardNum (phone), licenseId
#define MSG_CASHIER_QIWIWALLET_PRE_PURCHASE_REPLY       6930 // 4444s6[s|sss] reqId, clientConnId, errcode, [errmsg | url, numNameValue, <name, value>]
#define MSG_CASHIER_QIWIWALLET_PURCHASE_NOTIFY          6931 // QiwiWallet notify
#define MSG_CASHIER_QIWIWALLET_PURCHASE_NOTIFY_REPLY    6932 //

#define MSG_WALLETONE_TRANS_NOTIFY                      6933 //444sssssreqId transId amount currency status xTransStr wmiUserId
#define MSG_WALLETONE_TRANS_NOTIFY_REPLY                6934 //4ss reqId status errorInfo

// #20109
#define MSG_CASHIER_INSTANTBANK_CASHIN                  6935 //same as bonusCode + language + MSG_GATEWAY2_CASHIN
#define MSG_CASHIER_INSTANTBANK_CASHIN_REPLY            6936 //4s2[s|s] amount, userId, errCode [url | errMessage]

// #22220
#define MSG_CASHIER_YANDEXMONEY_PRE_PURCHASE            6937 // 4444s414 reqId, clientConnId, transId, amount, currency, userIntId, cardTypeByte, licenseId
#define MSG_CASHIER_YANDEXMONEY_PRE_PURCHASE_REPLY      6938 // 4444s6[s|sss] reqId, clientConnId, errcode, [errmsg | url, numNameValue, <name, value>]
#define MSG_CASHIER_YANDEXMONEY_PURCHASE_NOTIFY         6939 // YandexMoney notify
#define MSG_CASHIER_YANDEXMONEY_PURCHASE_NOTIFY_REPLY   6940 // 42s reqId, errcode, errStr
#define MSG_CASHIER_YANDEXMONEY_CHECKORDER              6941 // 444 reqId, transId, amount
#define MSG_CASHIER_YANDEXMONEY_CHECKORDER_REPLY        6942 // 42[s] reqId, errcode, [errstr]

// #22585
#define MSG_CASHIER_MBOOKERSCASH_APPROVE_PURCHASE       6943 // 44sss reqID, procAmount, procCurrency, userId, xTransStr
#define MSG_CASHIER_MBOOKERSCASH_APPROVE_PURCHASE_REPLY 6944 // 42[s|4] reqID, errCode [errDesc | transID]

// #25751
#define MSG_CASHIER_PAGOEFECTIVO_NOTIFY                 6945 // 4s44ss reqid, status, transid, procamount, proccurrency, xtransstr
#define MSG_CASHIER_PAGOEFECTIVO_NOTIFY_REPLY           6946 // 4 reqid. 

#define MSG_CASHIER_PAYPALDIRECT_REVERSAL_NOTIFY		6947 // 444s reqId, transid, reversal type (0,1,2,3,4), xtransstr
#define MSG_CASHIER_PAYPALDIRECT_REVERSAL_NOTIFY_REPLY	6948 // 46[s] reqId, errCode, [errMsg]

// #25884
#define MSG_CASHIER_CHECK_PAYPAL_EMAIL_AND_PAYERID       6949 // 44ss reqId, transId, email, payerID
#define MSG_CASHIER_CHECK_PAYPAL_EMAIL_AND_PAYERID_REPLY 6950 // 46[s] reqId, errCode, [errMsg]

// #25751
#define MSG_CASHIER_HIPAY_NOTIFY                         6951 // 4s44sss reqid, status, transid, procamount, proccurrency, xtransstr, emailid
#define MSG_CASHIER_HIPAY_NOTIFY_REPLY                   6952 // 4 reqid. 

// #26267
#define MSG_CASHIER_BOKU_NOTIFY                          6953 // 4s44ssss reqid, status, transid, procamount, proccurrency, xtransstr, errCode, errMsg
#define MSG_CASHIER_BOKU_NOTIFY_REPLY                    6954 // 4 reqid. 

// #26837
#define MSG_CASHIER_GW_3V_PREREGISTER                       6955 // 4ss4sDssssss444sD reqid, firstname, lastname, titleid, email, dob, phone, userWebId, address1, address2, city, zip, countrycode, vipstatus, KYC level, KYC doc type, KYC doc Id, issue date 
#define MSG_CASHIER_GW_3V_PREREGISTER_REPLY                 6956 // 46[s] reqid, errCode, [errMsg|signupUrl]

// #26837
// #35692
#define MSG_CASHIER_GW_3V_STATUS_CHANGE                     6957 // 4s444sDssDsssss reqid, UserIdFor3V, vipstatus, KYC level, KYC doc type, KYC doc id, issue date, firstName, lastName, birthDate, addr1, addr2, city, zipCode, country
#define MSG_CASHIER_GW_3V_STATUS_CHANGE_REPLY               6958 // 46[s] reqid, errCode, [errMsg]

#define MSG_IOS_VERIFY_RECEIPT                              6959  // 44s reqid, siteId, receipt string(The base64 encoded receipt data)
#define MSG_IOS_VERIFY_RECEIPT_REPLY                        6960  // 44s reqId, status(0-success), receipt(Json representation of the receipt data)

#define MSG_FREEMIUM_CASHIN									6961 //444ss reqId, userIntId, storeType, receiptData, signature
#define MSG_FREEMIUM_CASHIN_REPLY							6962 //46[s|1ss4] reqId errCode [errDescr | verifResult(eFreemiumVerification), SKU/productId, xTranStr, FREEMIUMRECEIPT.RECEIPTID]

#define MSG_CASHIER_ASTROPAYDIRECT_CASHIN                6963 // 44s44ssD4ssssss reqId,clientId,userId,userIntId,bankId,cpf,bonusCode,DOB,amount,fullname,email,addr,zip,city,
                                                              // state
#define MSG_CASHIER_ASTROPAYDIRECT_CASHIN_REPLY          6964 // 446[s|ss4 T] reqId,clientId,errCode[errDescr|redirect,userId,amount T]
                                                              // T = 4sssssssss = bankId,bankCode,bankName,bankCurrency,branchName,nicAlias,bankCountry,nicValue,
                                                              // userCashoutBankAccountNumber,userCashoutBankAccountType

#define MSG_CASHIER_ASTROPAYDIRECT_PURCHASE_NOTIFY       6965 // 4444sssss reqId,asTransId,userIntId,amount,currency,status,authCode,xTransStr,bonusCode
#define MSG_CASHIER_ASTROPAYDIRECT_PURCHASE_NOTIFY_REPLY 6966 // 46[s] reqId, errCode, errStr

#define MSG_3V_TRANSACTION_NOTIFY                        6967  //4sss4 reqId, psWebId, xTransStr, currency, amount
#define MSG_3V_TRANSACTION_NOTIFY_REPLY                  6968  //46[s|4] reqId, err, errInfo|transId

#define MSG_CASHIER_ASTROPAYDIRECT_GET_BANKINFO          6969 // 444sB reqId, connId, userIntId, country, bSkipBankList
#define MSG_CASHIER_ASTROPAYDIRECT_GET_BANKINFO_REPLY    6970 // 446[s|64<T>] reqId, connId, errCode [ errDescr | index, total, <T>]
                                                              // T = 4sssssssss = bankId,bankCode,bankName,bankCurrency,branchName,nicAlias,bankCountry,nicValue,
                                                              // userCashoutBankAccountNumber,userCashoutBankAccountType

#define MSG_3V_TRANSACTION_REFUND                        6971  //4sss4 reqId, userId, xTransStr, currency, amount
#define MSG_3V_TRANSACTION_REFUND_REPLY                  6972  //46[s|4] reqId, err, errInfo|transId

#define MSG_3V_REGISTER_NOTIFY                           6973  //4ss reqId, psWebId, status
#define MSG_3V_REGISTER_NOTIFY_REPLY                     6974  //46[s] reqId, err

#define MSG_CASHIER_ASTROPAYDIRECT_CHARGEBACK_NOTIFY       6975 // 4444ss reqId, psTransId, userIntId, recvAmount, recvCurrency, reason
#define MSG_CASHIER_ASTROPAYDIRECT_CHARGEBACK_NOTIFY_REPLY 6976 // 46[s] reqId, errCode [errDescr]

#define MSG_CASHIER_ASTROPAYDIRECT_REVCHARGEBACK_NOTIFY       6977 // 4444ss reqId, psTransId, userIntId, recvAmount, recvCurrency, reason
#define MSG_CASHIER_ASTROPAYDIRECT_REVCHARGEBACK_NOTIFY_REPLY 6978 // 46[s] reqId, errCode [errDescr]

#define MSG_3V_TRANSACTION_QUERY                         6979  //4ss reqId, userWebId, xTransStr
#define MSG_3V_TRANSACTION_QUERY_REPLY                   6980  //4ss4 reqId, status, errInfo, transId

#define MSG_CASHIER_ASTROPAYDIRECT_SAVE_BANKINFO         6981 // 44s4T reqId,connId,userId,userIntId,T
                                                              // T = 4sssssssss = bankId,bankCode,bankName,bankCurrency,branchName,nicAlias,bankCountry,nicValue,
                                                              // userCashoutBankAccountNumber,userCashoutBankAccountType
#define MSG_CASHIER_ASTROPAYDIRECT_SAVE_BANKINFO_REPLY   6982 // 446[s] reqId,connId,errCode,[errDescr]

#define MSG_CASHIER_ASTROPAYDIRECT_CASHOUT_NOTIFY        6983 // 4444sss reqId, psTransId, userIntId, recvAmount, recvCurrency, status, xtransstr
#define MSG_CASHIER_ASTROPAYDIRECT_CASHOUT_NOTIFY_REPLY  6984 // 46[s] reqId, errCode [errDescr]

#define MSG_CASHIER_ELV_SEPA_MANDATE_SIGNED	             6985 // 4s4sDs - reqId, userId, userIntId, signedWhere, signedWhen, country
#define MSG_CASHIER_ELV_SEPA_MANDATE_SIGNED_REPLY        6986 // 46[s] reqId, errCode [errDescr]

#define MSG_CASHIER_GET_CAPSCAN_ADDRESS                  6987 // 4sss - reqId, freeFormatAddress, zipCode, CountryCode
#define MSG_CASHIER_GET_CAPSCAN_ADDRESS_REPLY            6988 // 46[s|4<T>] reqId, errCode, [errDescr | total, <T>] T = sssss = address,city,state,zip,country

#define MSG_PAYLUCKY_GET_INFO                            6989 // 44sss - reqId, userIntId, acctToken, firstName, lastName
#define MSG_PAYLUCKY_GET_INFO_REPLY                      6990 // 46[s|4]  - reqId, errCode, [errorInfo|balance]

#define MSG_PAYLUCKY_CASHIN				                 6991 // for both PayLucky and SightLine
#define MSG_PAYLUCKY_CASHIN_REPLY			             6992 // 

#define MSG_CASHIER_PSIOB_PURCHASE_NOTIFY                6993 // PSIOB PYR-28858 444ss4ss reqid,transid,amount,currency,status,userintid,xtranstr,errCode
#define MSG_CASHIER_PSIOB_PURCHASE_NOTIFY_REPLY          6994 // 4 reqId

#define MSG_CASHIER_PSIOB_PRE_PURCHASE                   6995 // T ECheckTrans
#define MSG_CASHIER_PSIOB_PRE_PURCHASE_REPLY             6996 // 444s6s4<ss> reqId,transid,amount,currency,errCode,redirectUrl,total,<name,value>

#define MSG_CASHIER_PSIOB_CHARGEBACK_NOTIFY              6997 // 4444ssss reqId, psTransId, userIntId, recvAmount, recvCurrency, reversalCode,origXTransStr,XTransStr
#define MSG_CASHIER_PSIOB_CHARGEBACK_NOTIFY_REPLY        6998 // 46 reqId,errCode

#define MSG_CASHIER_VANILLA_INCOMM_GET_AMTCURRENCY       6999 // 44ss reqid, connid, pin, balanceCurrency - PYR-31264
#define MSG_CASHIER_VANILLA_INCOMM_GET_AMTCURRENCY_REPLY 7000 // 446[s|4sss] reqId, connid, errCode, [errStr | procAmount, procCurrency, balanceCurrency, pin] - PYR-31264

#define MSG_TS_SEND_PUSH_NOTIFICATION                    7001  // 444tssss4sb – reqId, device type(iOS, Android), siteId, expiry, token, imageId, soundId, pnmsg, badgeNum, extraInfo, ifSilent	
#define MSG_TS_SEND_PUSH_NOTIFICATION_REPLY              7002  //46[s] - reqId, errCode, [errDescr]

// PYR-36700
#define MSG_ADYEN_HOSTED_CASHIN                          7003 //444sssssssss1ssss4ss488
// reqId,transId,amount,currency,fullName,addr1,addr2,city,state,country,zip,email,isRepeat,paymentMethod,prefLanguage,skinCode,userId,liscenseId,firstName,lastName,userIntId
// flags, flags2
#define MSG_ADYEN_HOSTED_CASHIN_REPLY                    7004 //44s reqId,transId,redirectUrl
#define MSG_ADYEN_HOSTED_TRANS_NOTIFY                    7005 //444sss reqId,transId,amount,currency,xTransStr,status
#define MSG_ADYEN_HOSTED_TRANS_NOTIFY_REPLY              7006 //46 reqId,errCode

// PYR-37230 (CrmServer->SignalGW)
#define MSG_REPORT_SIGNAL								 7007 //4s - reqI, urlParams
#define MSG_REPORT_SIGNAL_REPLY							 7008 //46[s] - reqI, errCode[,errDescr]

// PYR-23517 3D verify initiated by callback server
#define MSG_3DSECURE_VERIFY                              7009
// 44s1s21ssssss4s1Tssss4bs4
// reqId amount_cents fullName ccType ccNumber ccExpiryYear ccExpiryMonth addr_1 addr_2 
// city state country zip transId email isRepeat started ipOnRequest currency issueNum startdate
// userIntId x3dSupport, userId, liscenseId
// ccType: 'V' = VISA, 'M' = MASTERCARD, 'F' = FIREPAY

//gateway2 to cashier
#define MSG_3DSECURE_VERIFY_REPLY		                 7010
// 41s444
// reqId status xTransIdStr errorInfo transId rejectReason rejectExplanationCode// transId is redundant, just in case

#define MSG_BOLDCHAT_REPORT      		                 7011 // 4<8888sTs> total, <chatId, chatMessageId, cannedMsgId, personId, name, created, text>
#define MSG_BOLDCHAT_REPORT_REPLY   		             7012 //46 reqId,errCode
// 41s444
// reqId status xTransIdStr errorInfo transId rejectReason re

#define MSG_CASHIER_SOFORT_PRE_PURCHASE                  7013  // 44ss transId, amount, currency, country
#define MSG_CASHIER_SOFORT_PRE_PURCHASE_REPLY            7014 // 444s6s4<ss> reqId,transid,amount,currency,errCode,redirectUrl,total,<name,value>

#define MSG_SIGHTLINE_NR_SIGNUP                          7015 // 44s - reqId, SightlineRegisterPrepaidAccount
#define MSG_SIGHTLINE_NR_SIGNUP_REPLY                    7016 // 46ss  - reqId, errCode, token|errStr, cardNumber

#define MSG_CAMSGW_VOID_TRANSACTION                      7017 // 44s // reqId, transId, xTransStr
#define MSG_CAMSGW_VOID_TRANSACTION_REPLY                7018 // 46s4 // reqId, errCodeNumber, errInfoStr/newXTransStr, transId

#define MSG_ENVLBT_TRANS_CHECK			                 7019 //4s4sss4sss reqId userId amt currency xtransStr bonusCode status, operatorname, gateway, paysystem PYR-41830
#define MSG_ENVLBT_TRANS_CHECK_REPLY		             7020 //46[s|4] reqId errCode[errDescr|transId]

#define MSG_VANILLARELOAD_SIGNUP                         7021 // 44s - reqId, struct VanillaSignup
#define MSG_VANILLARELOAD_SIGNUP_REPLY                   7022 // 46ss  - reqId, errCode, errStr|customerAID, barcodeImg

#define MSG_VANILLARELOAD_NOTIFY                         7023 // 44s4b - reqId, userIntId, customerAID, amount, isDeposit
#define MSG_VANILLARELOAD_NOTIFY_REPLY                   7024 // 46ss - reqId, errCode

#define MSG_VANILLARELOAD_GET_CARDINFO                   7025 // 44s - reqId, userIntId
#define MSG_VANILLARELOAD_GET_CARDINFO_REPLY             7026 // 46ss  - reqId, errCode, errStr|barcodeImg

#define MSG_VANILLARELOAD_SAVE_SIGNUIP                   7027 // 4s144sT - reqId, struct SignupAudit
#define MSG_VANILLARELOAD_SAVE_SIGNUIP_REPLY             7028 // 46ss  - reqId, errCode, errStr

#define MSG_VANILLARELOAD_GET_CUSTOMERAID                7029 // 44 - reqId, userIntId
#define MSG_VANILLARELOAD_GET_CUSTOMERAID_REPLY          7030 // 46ss  - reqId, errCode, customerAID|errStr

// PYR-47450
#define MSG_PROFEE_PURCHASE_NOTIFY                       7031 // 444sssssss - reqid,transid,amount,cur,status,xtransstr,errcode,account,cardmasked,message
#define MSG_PROFEE_PURCHASE_NOTIFY_REPLY                 7032 // 46s - reqid,errcode,errstr
#define MSG_PROFEE_CHECK_CARDNUM                         7033 // 444sssssss - reqid,transid,amount,cur,status,xtransstr,errcode,account,cardmasked,message
#define MSG_PROFEE_CHECK_CARDNUM_REPLY                   7034 // 46s - reqid,errcode,errstr

// PYR-48188
#define MSG_ECOPAYZ_PURCHASE_NOTIFY                      7035 // 4444ssssss - reqid,transid,userintid,amount,cur,status,xransstr,errcode,statusdescription,svscustomeraccount
#define MSG_ECOPAYZ_PURCHASE_NOTIFY_REPLY                7036 // 46s - reqid,errcode,errstr


// PYR-42193
enum UATagOperation
{
	UA_addTag = 0,
	UA_removeTag = 1,
	UA_setTag = 2,
};

enum UAServerEvent
{
	UA_ReportSelfExclusion = 0,
	UA_ReportEmLocaleChange = 1,
	UA_ReportBoostCountDown = 2, // PYR-50292
	UA_ReportSportLiveAlert = 3, // PYR-60282
	UA_ReportCountryChange = 4, // PYR-94187
};

#define MSG_URBANAIRSHIP_REPORT_EVENT					 7037 // 44s4s4<s> - reqId, op, webId, event, tagValue, numOfAppKes <appKey>
#define MSG_URBANAIRSHIP_REPORT_EVENT_REPLY				 7038 // 46[s] - reqId, errCode [,errDesc]

#define MSG_VIDEOADS_REWARD_NOTIFY                       7039 // 4s4sTs - reqId, userWebId, reward, eventId, when, provider
#define MSG_VIDEOADS_REWARD_NOTIFY_REPLY                 7040 // 46ss  - reqId, errCode, errStr

#define MSG_PAYPAL_GET_PERSONAL_INFO                     7041 // 44ss - reqId, licenseId, country, token
#define MSG_PAYPAL_GET_PERSONAL_INFO_REPLY               7042 // 46[s|<T>]  - reqId, errCode, errStr|personal Info struct

#define MSG_IMPORT_PAYPAL_FILE                     7043 //44Ts4<ss>4 reqId,recNum,fileTime,fileName,recordNumber,<name,value>,invoiceID
#define MSG_IMPORT_PAYPAL_FILE_REPLY               7044 //464[s|B[4MMM]] reqId,errCode,transId,[errStr|isChargeback[reason,achNotifyBody,cashierNotifBody,InsertChargebackBody]]

#define MSG_INSERT_PAYPAL_CHARGEBACK               7045 // 444ssssss reqId,invoiceid,grossamnt,grosscurrency,transactionid,transEventCode,caseType,TRRfilename,DDRfilename 
#define MSG_INSERT_PAYPAL_CHARGEBACK_REPLY         7046 // 464s reqId,errCode,invoiceId,errStr

#define MSG_ECOPAYZ_CHECK_CARDNUM                  7047 //
#define MSG_ECOPAYZ_CHECK_CARDNUM_REPLY            7048 // 46s - reqid,errcode,errstr

// PYR-37328
#define MSG_CASHIER_GW_STARWALLET_REGISTER         7049 // 4.. - reqId, struct StarswalletRegisterParams
#define MSG_CASHIER_GW_STARWALLET_REGISTER_REPLY   7050 // 46[s|4s] reqid, errCode, [errMsg|status, loadingNumber]

// PYR-47450
#define MSG_PROFEE_CASHOUT_NOTIFY                  7051 // TODO
#define MSG_PROFEE_CASHOUT_NOTIFY_REPLY            7052 // 46[s] reqId,errCode,[errMsg]

#define MSG_GW_SET_XTRANSSTR                       7053 // 44s   reqId, transId, xTransStr
#define MSG_GW_SET_XTRANSSTR_REPLY                 7054 // 46[s] reqId,errCode,[errMsg]

// PYR-37328
#define MSG_CASHIER_GW_STARWALLET_RESENDPWD        7055 // 4s4s reqId, StarsWalletResendOtpRequest as swCardNum, licenseId, country
#define MSG_CASHIER_GW_STARWALLET_RESENDPWD_REPLY  7056 // 46[s|] reqId, errCode, [errMsg|]

// PYR-37328
#define MSG_CASHIER_GW_STARWALLET_EDIT             7057 // 4ss4s reqId, StarsWalletChangeMobileRequest as swCardNum, newMobile, licenseId, country
#define MSG_CASHIER_GW_STARWALLET_EDIT_REPLY       7058 // 46[s|] reqId, errCode, [errMsg|]

// PYR-37328
#define MSG_CASHIER_GW_STARWALLET_GET_STATUS             7059 // 4s4 reqId, StarsWalletGetStatusRequest as mobile, userintId
#define MSG_CASHIER_GW_STARWALLET_GET_STATUS_REPLY       7060 // 46[s|4s] reqId, errCode, [errMsg|status, loadingNumber]

// PYR-49972
enum StrongViewEvent
{
	eStrongViewEvent_AccountClosed = 1,
	eStrongViewEvent_EmailBounced = 2,
	eStrongViewEvent_UsedSelfexclusion = 3,
};

// PYR-49972
#define MSG_STRONGVIEW_REPORT_EVENT					 7061 // 44bs reqId,event,value,email
#define MSG_STRONGVIEW_REPORT_EVENT_REPLY			 7062 // 46[s|] reqId, errCode, [errMsg|]

// PYR-35615
#define MSG_TECHPROCESS_PURCHASE_NOTIFY              7063 // 444ssss - reqid,transid,amount,cur,status,xtransstr,errcode
#define MSG_TECHPROCESS_PURCHASE_NOTIFY_REPLY        7064 // 46[s] reqId,errCode,[errStr]

// PYR-50292
#define MSG_URBANAIRSHIP_REPORT_EVENT2				 7065 // 4s4s4<s> - reqId, webId, event, value, numOfAppKes <appKey>
#define MSG_URBANAIRSHIP_REPORT_EVENT2_REPLY		 7066 // 46[s] - reqId, errCode [,errDesc]

// PYR-53344
#define MSG_TECHPROCESS_DECRYPT_WEB_PARAMS			 7067 // 444<ss> -  reqId, TechproDecryptParams as transId, paramSize <key, value>
#define MSG_TECHPROCESS_DECRYPT_WEB_PARAMS_REPLY	 7068 // 46[s|] - reqId, errCode [errDesc|TechproDecryptParams]

// PYR-55411
#define MSG_CASHFREE_HOSTED_CASHIN					 7069 // 444sssssssss1ssss4ss488 as reqId, CashfreeHostedPurchaseTrans
// reqId,transId,amount,currency,fullName,addr1,addr2,city,state,country,zip,email,isRepeat,paymentMethod,prefLanguage,customerPhone,userId,liscenseId,firstName,lastName,userIntId
// flags, flags2
#define MSG_CASHFREE_HOSTED_CASHIN_REPLY			 7070 // 4sss reqId, status, url, reason
#define MSG_CASHFREE_HOSTED_TRANS_NOTIFY			 7071 // 444sss  reqId, CashFreeHostedData as transId, procAmount, referenceId, txStatus, txMsg
#define MSG_CASHFREE_HOSTED_TRANS_NOTIFY_REPLY		 7072

#define MSG_CASHIER_PROFEE_REVERSAL_NOTIFY           7073 // 444s reqId, transid, reversal type (0,1), xtransstr
#define MSG_CASHIER_PROFEE_REVERSAL_NOTIFY_REPLY     7074 // 46[s] reqId, errCode, [errMsg]

// PYR-55212 Cashier -> Cashfree gw
#define MSG_CASHFREE_INVALIDATE_DBT					 7075 // 4s44 reqId, userId, accountId (DBTACCOUNTS.ACCOUNTID), userIntId
#define MSG_CASHFREE_INVALIDATE_DBT_REPLY			 7076 // 6[s|] errCode [errDescr|]

// PYR-51402
#define MSG_VITESSE_OVERCREDIT_TRANS_NOTIFY          7077 // 44sss reqId, transId, status, xTransStr, errDescr
#define MSG_VITESSE_OVERCREDIT_TRANS_NOTIFY_REPLY    7078 // 46[s] reqId, errCode, [errDescr]

// PYR-61756
#define MSG_MUCHBETTER_TRANS_NOTIFY                  7079 // 444sss reqId, MuchbetterTransNotif as (transId, procAmount, procCurrency, status, xTransStr)
#define MSG_MUCHBETTER_TRANS_NOTIFY_REPLY            7080 // 46[s] reqId, errCode, [errDescr]

// PYR-60821
#define MSG_BOLDCHAT_ASSIGNMENT_REPORT               7081 // 44<m> reqId, total, <BoldChatAssignment>
#define MSG_BOLDCHAT_ASSIGNMENT_REPORT_REPLY         7082 // 46[s] reqId, errCode, [errDescr]

// PYR-60821
#define MSG_BOLDCHAT_DEPARTMENT_REPORT               7083 // 44<m> reqId, total, <BoldChatDepartment>
#define MSG_BOLDCHAT_DEPARTMENT_REPORT_REPLY         7084 // 46[s] reqId, errCode, [errDescr]

// PYR-60821
#define MSG_BOLDCHAT_OPERATORS_REPORT                7085 // 44<m> reqId, total, <BoldChatOperator>
#define MSG_BOLDCHAT_OPERATORS_REPORT_REPLY          7086 // 46[s] reqId, errCode, [errDescr]

// PYR-60821
#define MSG_BOLDCHAT_SETUPFOLDER_REPORT              7087 // 44<m> reqId, total, <BoldChatSetupFolder>
#define MSG_BOLDCHAT_SETUPFOLDER_REPORT_REPLY        7088 // 46[s] reqId, errCode, [errDescr]

// PYR-60821
#define MSG_BOLDCHAT_SETUPITEM_REPORT                7089 // 44<m> reqId, total, <BoldChatSetupItem>
#define MSG_BOLDCHAT_SETUPITEM_REPORT_REPLY          7090 // 46[s] reqId, errCode, [errDescr]

// PYR-69310 erfeed GW -> cashier (notification)
#define MSG_ROMANIA_CONVRATE_UPDATE                  7091 //4dd4<s8> reqId, published, cubicDate, vectorSize, <currency,rate> // rate - x100M
#define MSG_ROMANIA_CONVRATE_UPDATE_REPLY            7092 //46[s] reqId, errCode, [errDescr]

// PYR-73312
#define MSG_VANILLADIRECT_GET_BARCODE                7093 // 44 reqId, userIntId
#define MSG_VANILLADIRECT_GET_BARCODE_REPLY          7094 // 4.. - reqId, struct VanillaDirectGetBarcode

#define MSG_VANILADIRRECT_NOTIFY                     7095 // 4.. - reqId, struct VanillaDirectNotify
#define MSG_VANILADIRRECT_NOTIFY_REPLY               7096 // 46[s] reqId, errCode, [errMsg]

// PYR-63165
#define MSG_BE_GAMESNG_GET_REQUESTID				 7097 // 4s4 - reqId, userId, errCode
#define MSG_BE_GAMESNG_GET_REQUESTID_REPLY           7098 // 44  - reqId, requestId

#define MSG_BE_GAMESNG_INSERT_AUIDT				     7099 // 4s4 - reqId, struct BeGamesNgAudit
#define MSG_BE_GAMESNG_INSERT_AUDIT_REPLY            7100 // 46[s] reqId, errCode, [errMsg]

// PYR-69195

#define MSG_AWEPAY_NOTIFY                            7101 // 4.. - reqId, struct AwepayNotify
#define MSG_AWEPAY_NOTIFY_REPLY                      7102 // 46[s] reqId, errCode, [errMsg]

// PYR-51865
#define MSG_CASHIER_GW_STARSWALLET_SEND_MTAN         7103 // 44sss reqId, licenseId, country, loadingnum, mTAN
#define MSG_CASHIER_GW_STARSWALLET_SEND_MTAN_REPLY   7104 // 46[s] reqId, errCode, [errMsg]

// PYR-55412
#define MSG_PAYTM_NOTIFY                            7105 // 4.. - reqId, struct PaytmNotify
#define MSG_PAYTM_NOTIFY_REPLY                      7106 // 46[s] reqId, errCode, [errMsg]

// PYR-36843
#define MSG_APPLEPAY_GET_MERCHANTID                 7107 // 4 - reqId
#define MSG_APPLEPAY_GET_MERCHANTID_REPLY           7108 // 46[s|s] reqId, errCode, [errMsg|merchantId]

// PYR-36843
#define MSG_APPLEPAY_MERCHANT_VALIDATION            7109 // 4ss4s - reqId, initiative, validationURL, licenseId, country
#define MSG_APPLEPAY_MERCHANT_VALIDATION_REPLY      7110 // 46[s|s] reqId, errCode, [errMsg|merchantSession]

// PYR-36843
#define MSG_APPLEPAY_DECRYPT_PAYMENT_TOKEN          7111 // 4ss - reqId, encrPaymentToken
#define MSG_APPLEPAY_DECRYPT_PAYMENT_TOKEN_REPLY    7112 // 46[s|sds4sss] reqId, errCode, [errMsg|DPAN, expirationDate, currency, amount, onlinePaymentCryptogram, eciIndicator, cardDisplayName]

// PYR-80344
#define MSG_WORLDPAY_GENERATE_JWT                   7113 // 4s4s44 - reqId, userId, licenseId, country
#define MSG_WORLDPAY_GENERATE_JWT_REPLY             7114 // 42[s|ss] - reqId, errCode, [errMsg|DDCUrl,jwt]

// PYR-80344
#define MSG_WORLDPAY_3DS_2ND_STAGE                  7115 // 444sss1 - reqId, transId, licenseId, country, cookie, dfReferenceId(sessionId), recover
#define MSG_WORLDPAY_3DS_2ND_STAGE_REPLY            7116 // 414sss - reqId, status, transId, xTransStr, errorInfo, errorCodeStr

// PYR-80351 FDMS GW -> Cashier
// obsolete. The notifications are sent by Client as 'MSG_CASHIER_FDMS_NOTIFY_3DFORM_COMPLETED'
#define MSG_FDMS_NOTIFY_3DFORM_COMPLETED            7117 // 44 - reqId, transId
#define MSG_FDMS_NOTIFY_3DFORM_COMPLETED_REPLY      7118 // 4[s|4] - reqId, errCode, [errMsg|transId]

// PYR-80351 Cashier -> FDMS GW
#define MSG_FDMS_3DFORM_COMPLETED                   7119 // 4.. - same as MSG_GATEWAY2_CASHIN
#define MSG_FDMS_3DFORM_COMPLETED_REPLY             7120 // format is exactly the same as 'MSG_GATEWAY2_CASHIN_REPLY'

// PYR-98052 SwishGw -> Cashier
#define MSG_SWISH_DEPOSIT_NOTIFY                    7121 // 44s4ssss - reqId,uint32_transId,status(A/R),int32_amount,currency,xtransStr,errorCode,errorMessage
#define MSG_SWISH_DEPOSIT_NOTIFY_REPLY              7122 // 46[s] - reqId,int16_errCode,[errMsg]

// PYR-101793 Gigadat -> Cashier
#define MSG_GIGADAT_DEPOSIT_NOTIFY                  7123 // 444sss - reqId, transId, amount, currency, processor status, cpiType
#define MSG_GIGADAT_DEPOSIT_NOTIFY_REPLY            7124 // 46[s] - reqId, errCode,[errMsg]

// PYR-105915 NOW2PAY -> Cashier
#define MSG_NOW2PAY_DEPOSIT_NOTIFY                  7125 // 4.. - reqId, struct Now2PayNotify
#define MSG_NOW2PAY_DEPOSIT_NOTIFY_REPLY            7126 // 46[s] reqId, errCode, [errMsg]

// PYR-116520 Cashier -> Plaid gw
#define MSG_CASHIER_PLAID_GET_ENV                   7127 // 4 reqId
#define MSG_CASHIER_PLAID_GET_ENV_REPLY             7128 // 464sss reqId, int16_errCode=0, uint32_enumEnvironment, clientId, secretKey, publicKey. (uint32_enumEnvironment: 0-PROD, 1-SANDBOX, 2-DEV)

#define MSG_CASHIER_PLAID_EXCHANGE_TOKEN            7129 // 44s reqId, userIntId, public_token
#define MSG_CASHIER_PLAID_EXCHANGE_TOKEN_REPLY      7130 // 46[s|4s] reqId, int16_errCode, [errMsg | userIntId, access_token]

#define MSG_CASHIER_PLAID_GET_AUTH                  7131 // 44ss reqId, userIntId, access_token, bank_account_id
#define MSG_CASHIER_PLAID_GET_AUTH_REPLY            7132 // 46[s|4sssss] reqId, int16_errCode, [errMsg | userIntId, bank_account, routing_number, account_type(saving/checking), bank_name, currency

#define MSG_CASHIER_PLAID_GET_BALANCE               7133 // 44ss reqId, userIntId, access_token, bank_account_id
#define MSG_CASHIER_PLAID_GET_BALANCE_REPLY         7134 // 46[s|44s] reqId, int16_errCode [errMsg | userIntId, uint32_balance_in_cents, currency]

// PYR-121189 PAYSAFE -> Cashier
#define MSG_CASHIER_CHECK_PAYSAFE_CUSTOMER_ID       7135 // 44s reqId, transId, customerID
#define MSG_CASHIER_CHECK_PAYSAFE_CUSTOMER_ID_REPLY 7136 // 46[s] reqId, errCode, [errMsg]

#endif //gateway_h_included
