//cashtypes.h
#ifndef cashtypes_h_included
#define cashtypes_h_included

#include "ppincludebase.h"
#include "ppstring.h"
#include "commmsgbody.h"
#include "ppcontainer.h"

enum	eResultInfoType
{
	eResultInfoTypeUNKNOWN		= 0,
	eResultInfoTypeClickAndBuy	= 'A',
	eResultInfoTypeBonus		= 'B',
	eResultInfoTypeCard			= 'C',
	eResultInfoTypeDbt			= 'D',
	eResultInfoTypeECheck		= 'E',
	eResultInfoTypeFirePay		= 'F', // Obsolete - do not remove
	eResultInfoTypePayPal       = 'G',
	eResultInfoTypeDineromail   = 'H',
	eResultInfoTypeInstaDebit   = 'I',
	eResultInfoTypeYandexMoney  = 'J',
	eResultInfoTypeCentralCoin	= 'K', // Obsolete - do not remove
	eResultInfoTypeLoopBack		= 'L',
	eResultInfoTypeMaster		= 'M',
	eResultInfoTypeNeteller		= 'N',
	eResultInfoTypeMbookers		= 'O',
	eResultInfoTypeEPassporte	= 'P',
	eResultInfoTypeCheque		= 'Q',
	eResultInfoTypeBankDirect	= 'R',
	eResultInfoTypeSolo			= 'S',
	eResultInfoTypeCft			= 'T',
	eResultInfoTypeUnknown		= 'U',
	eResultInfoTypeEnvoy		= 'V',
	eResultInfoTypeWire			= 'W',
    eResultInfoTypePagosOnline  = 'X',
	eResultInfoTypeWebMoney		= 'Y',
	eResultInfoTypeQiwiWallet   = 'Z',
	eResultInfoTypeAstropay     = 'a',
	eResultInfoTypeInstantBank  = 'b',
	eResultInfoTypeHiPay        = 'c', // #26072
	eResultInfoTypeBoku         = 'd', // #26276
    eResultInfoTypeEpay         = 'e',
	eResultInfoTypeFullTilt		= 'f',
	eResultInfoTypeMgCashout	= 'g',
	eResultInfoTypeSightline    = 'h', // PYR-39651
    eResultInfoTypeUkash        = 'k',
	eResultInfoTypeMastercard   = 'm',
	eResultInfoTypeTodito       = 'o',
	eResultInfoTypePaySafe		= 'p',
	eResultInfoTypeRtbt			= 'r',
	eResultInfoTypeTicketPremium= 't',
	eResultInfoTypeUMWallet     = 'u', // Obsolete - do not remove for compatibility reasons
	eResultInfoTypeWalletOne    = 'w',
	eResultInfoTypeMoviPin      = 'x', // #23888 // TODO: sort them by value please
	eResultInfoTypeFrMigration  = '1',
	eResultInfoTypeClick2Pay	= '2', // Obsolete - do not remove
	eResultInfoTypeMoneta		= '3',
	eResultInfoType3V			= '4',
	eResultInfoTypeResort       = '5', // PYR-27726
	eResultInfoTypePsPayAstropay = '6',
	eResultInfoTypeElvSepa      = '7',
	eResultInfoTypePayLucky     = '8',
	eResultInfoTypeStarsWallet  = '9',
	eResultInfoTypeProfeeWallet = ':', // PYR-47450
	eResultInfoTypeProfeeCards  = ';', // PYR-47450
	eResultInfoTypeProfeeQiwi   = '<', // PYR-47450
	eResultInfoTypeProfeeYandex = '=', // PYR-47450
	eResultInfoTypeProfeeMobile = '>', // PYR-47450
	eResultInfoTypeCashfreeHosted = '?', // PYR-55411
	eResultInfoTypeInRtbt       = '@', // PYR-54168
	eResultInfoTypeMuchBetter   = 'i', // PYR-61756
	eResultInfoTypePayMoney     = 'n', // PYR-66127
	eResultInfoTypeVanillaDirect  = 'v', // PYR-73312
	eResultInfoTypeGamersCard   = 's', // PYR-60047
	eResultInfoTypeCupRtbt      = 'j', // PYR-69195
	eResultInfoTypeGenericCredit = 'q', // PYR-74280
	eResultInfoTypeGenericDebit  = 'z', // PYR-74280
	eResultInfoTypePaytm        = 'y', // PYR-55412
	eResultInfoTypeApplePay     = '0', // PYR-36843
	eResultInfoTypeGimo			= '!', // PYR-94807
	eResultInfoTypeSwish		= '"', // PYR-98052
	eResultInfoTypeInterac		= '#', // PYR-101793 PYR-110056
	eResultInfoTypeNow2PayMobile = 'l', // PYR-105915
	eResultInfoTypeNow2PayWeb	= '$', // PYR-105915

	eResultInfoTypeNext_,     // = previous + 1
}; // {'%'} left
static_assert(eResultInfoTypeNext_ <= UCHAR_MAX, "eResultInfoType value out of range. Maximum allowed is 254");

enum eResultInfoAction
{
	eActionChequeCancelled	= 'N',
	eActionChequeIssued		= 'I',
	eActionCardCharged		= 'C',
	eActionCardRolledback	= 'R',
	eActionRedeem			= 'R',
	eActionPayPalRedeem		= 'P',
	eActionFinalRedeem		= 'F',
	eActionCftCompleted		= 'O',
	eActionRedeemVerified	= 'V'
};

enum EnumCardType
{
	cardTypeInstaDebit      = 'A',
	cardTypeMBookers		= 'B',
	cardTypeAstropay        = 'C',
	cardTypeMRMC			= 'D',
	cardTypeECheck			= 'E',
	cardTypeFirePay			= 'F', // Obsolete - do not remove
	cardTypeITConversion	= 'G',
	cardTypeTransferOut     = 'H',
	cardTypeDiners			= 'I',
	cardTypePayPal          = 'J',
	cardTypeCCoin			= 'K', // Obsolete - do not remove
	cardTypeLoopBack		= 'L',
	cardTypeMasterCard		= 'M',
	cardTypeNeteller		= 'N',
	cardTypeEntropay		= 'O',
	cardTypeEPassporte		= 'P',
	cardTypeCheque			= 'Q',
	cardTypeBankDirect		= 'R',
	cardTypeSolo			= 'S',
	cardTypeTransfer		= 'T',
	cardTypeWUnion			= 'U',
	cardTypeVisa			= 'V',
	cardTypeBankWire		= 'W',
	cardTypeFRMigration		= 'X',
	cardTypeWebMoney		= 'Y',	
    cardTypeRobokassa       = 'Z',
	cardTypeAffDeposit		= 'a',
	cardTypeBPay			= 'b',
	cardTypeManualCredit	= 'c',
	cardTypeManualDebit		= 'd',
	cardTypeEnvoyLbt		= 'e',
	cardTypeMCTransfer		= 'f',
	cardTypeMgCashout		= 'g',
	cardTypeTicketPremium   = 'h',
	cardTypeDBT				= 'i',
	cardTypeTodito          = 'j',
	cardTypeUkash           = 'k',
	cardTypeBoleto			= 'l',
	cardTypeMoneygram		= 'm',
	cardTypeClickNBuy		= 'n',
	cardTypeMoneyOrder		= 'o', // Obsolete - do not remove
	cardTypePaySafe			= 'p',
	cardTypeUMWallet        = 'q', // Obsolete - do not remove
	cardTypeRtbt			= 'r',
	cardTypeCarteBleue		= 's',
	cardTypeCitadel			= 't',
 	cardTypeUseMyBank		= 'u', // Obsolete - do not remove
	cardTypeEnvoyRtbt		= 'v',
	cardTypeWireCashout		= 'w',    
    cardTypePStarsPay       = 'x', // #16406
	cardTypeBugsyConversion = 'y',					
    cardTypePagosOnline     = 'z', // #17121
    cardTypeRicarica        = '0', // #17270
	cardTypeGiropay			= '1',
	cardTypeClick2Pay		= '2', // Obsolete - do not remove
	cardTypeMonetaCashout	= '3',	
	cardTypeWirecardWbt		= '4',    
	cardTypeDankort			= '5',   
	cardTypeQiwi			= '6',
    cardTypeEPay            = '7',    
	cardTypeDineromail		= '8',
	cardTypeIdeal			= '9',
	cardTypeEps				= ':',
	cardTypeStarscard		= '=',
	cardTypeSofort			= ';',
	cardTypeBanklinkswed	= '<',
	cardTypeEuteller		= '>',
	cardTypeBeMigration		= '~',
    cardTypeVodafone        = '?', // #18938
    cardTypeO2              = '@', // #18938
    cardTypeTMobile         = '!', // #18938
	cardTypeMBookOBT        = '#', // #19502
	cardTypeAlfabank        = '"', // #19485
	cardTypeQiwiWallet      = '$', // #19820
	cardTypeTrustPay		= '&', // #19818
	cardTypeInstantBank     = '%', // #20109
	cardTypeWalletOne		= '*', // #20110
	cardTypeNordea			= '\'',// #20295
	cardTypeEsMigration		= '+',
	cardTypeNemkontoCashout	= '(',
	cardTypePStarsTopUp     = ')', // #21516
	cardTypePSPrepaidCard   = ',', // #21531
	cardTypeRechargePS      = '-', // #21531
	cardTypePSPrepaid       = '.', // #21531
	cardTypeYandexMoney     = '/', // #22220
	cardType7Eleven         = '[', // #22585
	cardTypeOxxo            = '\\', // #22585
	cardTypeProvinciaPagos  = ']', // #22585
	cardTypeCobroExpress    = '^', // #22585
	cardTypeRipsaPagos      = '_', // #22585
	cardTypeRechargePokerStars = '|', //#22844
	cardTypePaysystemFee    = '{', // #22732
	cardTypeFullTilt		= '}',
	cardTypeFullTiltAssetMigr = 128, // #24070
	cardTypeMoviPin         = 129, // #23888
	cardTypeMobilePayment   = 130, // #24787
	cardTypeEnvBaloto		= 131, 
	cardTypePagoEfectivo    = 132, // #23692
	cardTypePStarsPrepaid   = 133, // #24836
	cardTypePSCash          = 134, // #25850
	cardTypeHiPay           = 135, // #26072
	cardTypeBoku            = 136, // #26276
	cardTypeUpsellAward     = 137, // PYR-24840
	cardTypeBokuThreeUK     = 138, // #26276
	cardTypeBokuVodafone    = 139, // #26276
	cardTypeBokuO2          = 140, // #26276
	cardTypeBokuOrange      = 141, // #26276
	cardType3V              = 142, // PYR-26837
	cardTypePayNearMe       = 143, // #27655
	cardTypeResort			= 144, // PYR-27726
	cardTypeTax				= 145, // PYR-28936
	cardTypeAgmo			= 146, // #28996
	cardTypeSafetyPay		= 147, // #29634
	cardTypePsPayAstropay   = 148, // #27624
	cardType3VRefund        = 149, // #26837
	cardTypeElvSepa         = 150, // PYR-30281
	cardTypePayLucky        = 151, // PYR-31414
	cardTypePSIOB           = 152, // PYR-28858
	cardTypeVanilla         = 153, // PYR-31264
	cardTypeIDebit          = 154, // PYR-36349
	cardTypeSberBank        = 155, // PYR-37082
	cardTypeVivaPayments    = 156, // PYR-37445
	cardTypePtMigration		= 157, // PYR-38555
	cardTypeStarsVouchers   = 158, // PYR-31435
	cardTypeSightline       = 159, // PYR-39651
	cardTypeSightlineNR     = 160, // PYR-39934
	cardTypeFTPPay          = 161, // PYR-41208
	cardTypeVanillaReload   = 162, // PYR-39936
	cardTypeNeosurf         = 163, // PYR-45220
	cardTypeBoletoAstropay  = 164, // PYR-35738
	cardTypeProfeeCards     = 165, // PYR-47450
	cardTypeProfeeQiwi      = 166, // PYR-47450
	cardTypeProfeeYandex    = 167, // PYR-47450
	cardTypeProfeeMobilepayment = 168, // PYR-47450
	cardTypeProfeeWallet    = 169, // PYR-47450
	cardTypeEcoPayZ         = 170, // PYR-48188
	cardTypeStarsWallet     = 171, // PYR-37328
	cardTypeInRtbt          = 172,
	cardTypeInWallet        = 173,
	cardTypeCashfree        = 174, // PYR-55411
	cardTypeMuchBetter      = 175, // PYR-61756
	cardTypeAG              = 176, // PYR-61756
	cardTypePayMoney        = 177, // PYR-66127
	cardTypeVanillaDirect   = 178, // PYR-73312
	cardTypeGamersCard      = 179, // PYR-60047
	cardTypeCupRtbt         = 180, // PYR-69195
	cardTypeGenericCredit   = 181, // PYR-74280
	cardTypeGenericDebit    = 182, // PYR-74280
	cardTypeRupay           = 183, // PYR-74280
	cardTypeCashfreeNetbanking   = 184, // PYR-70255
	cardTypeCashfreeUpi          = 185, // PYR-70255
	cardTypeCashfreeMobilewallet = 186, // PYR-70255
	cardTypePaytm           = 187, // PYR-55412
	cardTypeApplePay        = 188, // PYR-36843
	cardTypeGimoTransfer	= 189, // PYR-90603
	cardTypeGimoPayment		= 190, // PYR-90603
	cardTypeMir             = 191, // PYR-95023
	cardTypeSwishPayment	= 192, // PYR-98052
	cardTypeInterac			= 193, // PYR-101793 PYR-110056
	cardTypeNow2PayMobile   = 194, // PYR-105915
	cardTypeNow2PayWeb      = 195, // PYR-105915
	// Use non printable characters  

	cardTypeNext_,          // = previous + 1
	cardTypeFirst           = cardTypeTMobile,    // '!'=33 min value for range check in ATF definition, please keep up-to-date
	cardTypeLast            = cardTypeNext_ - 1   // max value for range check in ATF definition
};
static_assert(cardTypeNext_ <= UCHAR_MAX, "EnumCardType value out of range. Maximum allowed is 254");

enum ePsBitNumber // zeroBased
{
	ePsBitNumberUNDEFINED	= -1,
	ePsBitNumberPayLucky    = ePsBitNumberUNDEFINED, // to be removed
	ePSBitNumberPSPrepaidCard = ePsBitNumberUNDEFINED, // formerly 77 bit 
	ePSBitNumberRechargePS  = ePsBitNumberUNDEFINED,   // 78 bit
	ePSBitNumberPSPrepaid   = ePsBitNumberUNDEFINED,   // 79 bit
	ePsBitNumber7Eleven     = ePsBitNumberUNDEFINED,   // 81 bit
	ePsBitNumberOxxo        = ePsBitNumberUNDEFINED,   // 82 bit
	ePsBitNumberProvinciaPagos = ePsBitNumberUNDEFINED,// 83 bit
	ePsBitNumberCobroExpress = ePsBitNumberUNDEFINED,  // 84 bit
	ePsBitNumberRipsaPagos  = ePsBitNumberUNDEFINED,   // 85 bit

	ePsBitNumberVisa		=  0, // Mask.bit0
	ePsBitNumberMaster		=  1,
	ePsBitNumberFirepay		=  2,
	ePsBitNumberCCoin		=  3, 
	ePsBitNumberDiners		=  4,
	ePsBitNumberSolo		=  5,
	ePsBitNumberECheck		=  6,
	ePsBitNumberWUnion		=  7,
	ePsBitNumberNeteller	=  8,
	ePsBitNumberBankWire	=  9,
	ePsBitNumberBankDirect	= 10,
	ePsBitNumberEPassporte	= 11,
	ePsBitNumberRTBT		= 12,
	ePsBitNumberPaySafe		= 13,
	ePsBitNumberMoneygram	= 14,
	ePsBitNumberMoneyOrder	= 15,
	ePsBitNumberMRMC		= 16,
	ePsBitNumberMBookers	= 17,
	ePsBitNumberTransfer	= 18,
	ePsBitNumberUseMyBank	= 19,
	ePsBitNumberClickAndBuy = 20,
	ePsBitNumberCitadel		= 21,
	ePsBitNumberStrategy	= 22,
	ePsBitNumberWebMoney	= 23,
	ePsBitNumberEnvoyRtbt	= 24,
	ePsBitNumberMgCashout	= 25,
	ePsBitNumberClick2Pay	= 26,
	ePsBitNumberEnvoyLbt	= 27,
	ePsBitNumberCarteBleue	= 28,
	ePsBitNumberEntropay	= 29,
	ePsBitNumberUkash       = 30,
// 31 bit skipped
	ePsBitNumberCheque		= 32, // Mask2.bit0
	ePsBitNumberWire		= 33,
	ePsBitNumberMoneta		= 34,
	ePsBitNumberDirbank		= 35,
	ePsBitNumberItConversion= 36,
	ePsBitNumberBoleto		= 37,
	ePsBitNumberMcTransfer	= 38,
	ePsBitNumberWirecardWbt	= 39,
	ePsBitNumberInstaDebit  = 40,
	ePsBitNumberDankort		= 41,
	ePsBitNumberUMWallet    = 42, // Mask2.bit10
	ePsBitNumberAstropay    = 43,
	ePsBitNumberTicketPrem	= 44,
	ePsBitNumberTodito      = 45,
	ePsBitNumberQiwi		= 46,
	ePsBitNumberEPay		= 47,
	ePsBitNumberPayPal      = 48,
	ePsBitNumberDineromail  = 49,
	ePsBitNumberFrMigration	= 50,
	ePsBitNumberRobokassa   = 51,
	ePsBitNumberPStarsPay   = 52, // Mask2.bit20
    ePsBitNumberPagosOnline = 53,
    ePsBitNumberRicarica    = 54,
	ePsBitNumberGiropay		= 55,
	ePsBitNumberIdeal		= 56,
	ePsBitNumberEps			= 57,
	ePsBitNumberStarscard	= 58,
	ePsBitNumberSofort		= 59,
	ePsBitNumberBanklinkswed= 60,
	ePsBitNumberEuteller	= 61,
	ePsBitNumberBeMigration	= 62, // Mask2.bit30
// 63 bit skipped
    ePsBitNumberVodafone    = 64, // Mask3.bit0
    ePsBitNumberO2          = 65,
    ePsBitNumberTMobile     = 66,
	ePsBitNumberMBookOBT    = 67,
	ePsBitNumberAlfabank    = 68,
	ePsBitNumberQiwiWallet  = 69,
	ePsBitNumberTrustPay	= 70,
	ePsBitNumberInstantBank = 71,
	ePsBitNumberWalletOne   = 72,
	ePsBitNumberNordea		= 73,
	ePsBitNumberEsMigration	= 74, // Mask3.bit10
	ePsBitNumberNemconto	= 75,
	ePSBitNumberPStarsTopUp = 76,
	ePsBitNumberProfeeYandex = 77,
	ePsBitNumberProfeeMobilepayment = 78,
	ePsBitNumberProfeeWallet= 79,
	ePsBitNumberYandexMoney = 80,
	ePsBitNumberStarsWallet = 81,
	ePsBitNumberInRtbt      = 82,
	ePsBitNumberInWallet    = 83,
	ePsBitNumberCashfree    = 84, // Mask3.bit20 PYR-55411
// 85 bit can be used
	ePsBitNumberRechargePokerstars = 86,
	ePsBitNumberPaysystemFee = 87,
	ePsBitNumberFullTilt	= 88,
	ePsBitNumberFullTiltAssetMigr = 89,
	ePsBitNumberMoviPin     = 90,
	ePsBitNumberMobilePayment = 91,
	ePsBitNumberEnvBaloto   = 92,
	ePsBitNumberPagoEfectivo = 93,
	ePsBitNumberPStarsPrepaid = 94, // Mask3.bit30
	ePsBitNumberPSCash      = 95,
	ePsBitNumberHiPay       = 96,
	ePsBitNumberBoku        = 97,
	ePsBitNumberUpsellAward = 98,
	ePsBitNumberBokuThreeUK = 99,
	ePsBitNumberBokuVodafone = 100,
	ePsBitNumberBokuO2      = 101,
	ePsBitNumberBokuOrange  = 102,
	ePsBitNumber3V          = 103,
	ePsBitNumberPayNearMe   = 104, // Mask3.bit40
	ePsBitNumberResort		= 105,
	ePsBitNumberTax			= 106,
	ePsBitNumberAgmo		= 107,
	ePsBitNumberSafetyPay	= 108,
	ePsBitNumberPsPayAstropay = 109,
	ePsBitNumber3VRefund      = 110,
	ePsBitNumberElvSepa     = 111,
	ePsBitNumberEcoPayZ     = 112, // PYR-55315 re-used ePsBitNumberPayLucky    
	ePsBitNumberPSIOB       = 113,
	ePsBitNumberVanilla     = 114, // Mask3.bit50
	ePsBitNumberIDebit      = 115,
	ePsBitNumberSberbank    = 116,
	ePsBitNumberVivaPayments = 117,
	ePsBitNumberPtMigration	= 118,
	ePsBitNumberStarsVouchers = 119,
	ePsBitNumberSightline   = 120, // same bit for SIGHTLINE and SIGHTLINENR
	ePsBitNumberFTPPay      = 121,
	ePsBitNumberVanillaReload = 122,
	ePsBitNumberNeosurf     = 123,
	ePsBitNumberBoletoAstropay = 124, // Mask3.bit60
	ePsBitNumberProfeeCards = 125,
	ePsBitNumberProfeeQiwi  = 126,
// 127 bit skipped
	ePsBitNumberMuchBetter  = 128, // Mask4.bit0
	ePsBitNumberAG          = 129,
	ePsBitNumberPayMoney    = 130,
	ePsBitNumberVanillaDirect = 131,
	ePsBitNumberGamersCard  = 132,
	ePsBitNumberCupRtbt     = 133, // PYR-69195
	ePsBitNumberGenericCredit = 134, // PYR-74280
	ePsBitNumberGenericDebit  = 135, // PYR-74280
	ePsBitNumberRupay       = 136, // PYR-74280
	ePsBitNumberCashfreeNetbanking	= 137, // PYR-70255
	ePsBitNumberCashfreeUpi			= 138, // Mask4.bit10   PYR-70255
	ePsBitNumberCashfreeMobileWallet= 139, // PYR-70255
	ePsBitNumberPaytm       = 140, // PYR-55412
	ePsBitNumberApplePay    = 141, // PYR-36843
	ePsBitNumberGimoTransfer		= 142,
	ePsBitNumberGimoPayment		= 143,
	ePsBitNumberMir         = 144, // PYR-95023
	ePsBitNumberSwishPayment	= 145, // PYR-98052
	ePsBitNumberInterac			= 146, // PYR-101793 PYR-110056
	ePsBitNumberNow2PayMobile   = 147, // PYR-105915
	ePsBitNumberNow2PayWeb      = 148, // PYR-105915

	ePsBitNumberHWM, // used for array size and sanity checks
	ePsBitNumberLast = ePsBitNumberHWM - 1
};
static_assert(ePsBitNumberHWM < ((32+32+64) + 64 - 1), "Paysystem bit value out of range"); // no more space in DEPOSITMASK4

/* obsoleted after PYR-45523
//paysystem codes (see DatabaseManager::isEligibleForBonus())
#define UNKNOWN_CODE		(-1)
#define ANY_CODE			0
#define NETELLER_CODE		1
#define VISA_CODE			2
#define MASTERCARD_CODE		3
#define FIREPAY_CODE		4
#define CCOIN_CODE			5
#define DINERS_CODE			6
#define SOLO_CODE			7
#define ECHECK_CODE			8
#define WU_CODE				9
#define WIRE_CODE			10
#define GCDEBIT_CODE		11
#define EPASSPORTE_CODE		12
#define RTBT_CODE			13
#define PAYSAFE_CODE		14
#define MONEYGRAM_CODE		15
#define MONEYORDER_CODE		16
#define MRMC_CODE			17
#define MBOOKERS_CODE		18
#define USEMYBANK_CODE		19
#define CNBUY_CODE			20
#define CITADEL_CODE		21
#define STRATEGY_CODE		22
#define WEBMONEY_CODE		23
#define ENVOY_CODE			24
#define CLICK2PAY_CODE		25
#define ENVOY_LBT_CODE		26
#define CARTEBLEUE_CODE		27
#define UKASH_CODE          28
#define BOLETO_CODE			29
#define IT_CONVERSION_CODE	30
#define WIRECARD_CODE		31
#define INSTADEBIT_CODE     32
#define DANKORT_CODE		33
#define UMWALLET_CODE		34
#define ASTROPAY_CODE       35
#define TICKETPREMIUM_CODE  36
#define ENTROPAY_CODE		37 //#13209 Entropay is VISA, but we want to give PS-specific bonus
#define TODITO_CODE			38
#define QIWI_CODE			39 //#14114 Qiwi is similar to Entropay: it is VISA, but we want to give PS-specific bonus
#define EPAY_CODE           40
#define PAYPAL_CODE         41
#define DINEROMAIL_CODE     42
#define FR_MIGRATION_CODE	43
#define ROBOKASSA_CODE      44
#define PSTARSPAY_CODE      45
#define PAGOSONLINE_CODE    46
#define RICARICA_CODE       47
#define GIROPAY_CODE		48
#define IDEAL_CODE			49
#define EPS_CODE			50
#define STARSCARD_CODE		51
#define SOFORT_CODE			52
#define BANKLINKSWED_CODE	53
#define EUTELLER_CODE		54
#define BE_MIGRATION_CODE	55
#define VODAFONE_CODE       56
#define O2_CODE             57
#define TMOBILE_CODE        58 
#define MBOOKOBT_CODE       59
#define ALFABANK_CODE       60
#define QIWIWALLET_CODE     61
#define TRUSTPAY_CODE		62
#define INSTANTBANK_CODE    63
#define WALLETONE_CODE	    64
#define NORDEA_CODE			65
#define ES_MIGRATION_CODE	66
#define NEMKONTO_CODE		67
#define PSTARSTOPUP_CODE    68
#define PSPREPAIDCARD_CODE  69
#define RECHARGEPS_CODE     70
#define PSPREPAID_CODE      71
#define YANDEXMONEY_CODE    72
#define SEVENELEVEN_CODE    73
#define OXXO_CODE           74
#define PROVINCIAPAGOS_CODE 75
#define COBROEXPRESS_CODE   76
#define RIPSAPAGOS_CODE     77
#define RECHARGEPOKERSTARS_CODE 78
#define FULLTILT_CODE		79
#define FT_ASSET_MIGR_CODE	80
#define MOVIPIN_CODE        81
#define MOBILEPAYMENT_CODE  82
#define ENV_BALOTO_CODE		83
#define PAGOEFECTIVO_CODE   84
#define PSTARSPREPAID_CODE  85
#define PSCASH_CODE         86
#define HIPAY_CODE          87
#define BOKU_CODE           88
#define BOKUTHREEUK_CODE    89
#define BOKUVODAFONE_CODE   90
#define BOKUO2_CODE         91
#define BOKUORANGE_CODE     92
#define PAYSYSTEM_3V_CODE   93
#define PAYNEARME_CODE      94
#define RESORT_CODE			95
#define AGMO_CODE			96
#define SAFETYPAY_CODE      97
#define PSPAY_ASTROPAY_CODE 98
#define ELV_SEPA_CODE       99
#define PAYLUCKY_CODE      100
#define PSIOB_CODE         101
#define VANILLA_CODE       102
#define IDEBIT_CODE        103
#define SBERBANK_CODE      104
#define VIVAPAYMENTS_CODE  105
#define PT_MIGRATION_CODE  106
#define STARSVOUCHERS_CODE 107
#define SIGHTLINE_CODE     108
#define SIGHTLINE_NR_CODE  109
#define FTPPAY_CODE        110
#define VANILLARELOAD_CODE 111
#define NEOSURF_CODE       112
#define BOLETO_ASTROPAY_CODE 113
#define PROFEECARDS_CODE   114
#define PROFEEQIWI_CODE    115
#define PROFEEYANDEX_CODE  116
#define PROFEEMOBILEPAYMENT_CODE 117
#define PROFEEWALLET_CODE  118
*/

struct PaysystemMask // #17229
{
	UINT32 mask;  // USERS.DEPOSITMASK
	UINT32 mask2; // USERS.DEPOSITMASK2
	UINT64 mask3; // USERS.DEPOSITMASK3
	UINT64 mask4; // USERS.DEPOSITMASK4


	PaysystemMask() : mask(0), mask2(0), mask3(0), mask4(0) {}
	bool isZero() const
	{
		return (mask == 0 && mask2 == 0 && mask3 == 0 && mask4 == 0);
	}
	void setZero()
	{
		mask = mask2 = 0;
		mask3 = mask4 = 0;
	}
	bool equals(const PaysystemMask& another) const
	{
		return (mask == another.mask && mask2 == another.mask2 && mask3 == another.mask3 && mask4 == another.mask4);
	}
	void setMasks(UINT32 mask_, UINT32 mask2_, UINT64 mask3_, UINT64 mask4_)
	{
		mask  = mask_;
		mask2 = mask2_;
		mask3 = mask3_;
		mask4 = mask4_;
	}

	bool isBitSet(int bitNumber) const // ZeroBased
	{
		if (bitNumber < 0)
			return false;
		if (bitNumber < 32)
			return (mask & (1 << bitNumber)) != 0;
		if (bitNumber < 64)
			return (mask2 & (1 << (bitNumber-32))) != 0;
		if (bitNumber < 128)
			return (mask3 & (ONE64 << (bitNumber-64))) != 0;
		if (bitNumber < (128+64))
			return (mask4 & (ONE64 << (bitNumber-128))) != 0;
		return false;
	}
	void setBit  (int bitNumber) // ZeroBased
	{
		if (bitNumber < 0)
			return;

		if (bitNumber < 32)
			mask |= (1 << bitNumber);
		else if (bitNumber < 64)
			mask2 |= (1 << (bitNumber-32));
		else if (bitNumber < 128)
			mask3 |= (ONE64 << (bitNumber-64));
		else if (bitNumber < (128+64))
			mask4 |= (ONE64 << (bitNumber-128));
	}
	void clearBit(int bitNumber)
	{
		if (bitNumber < 0)
			return;

		if (bitNumber < 32)
			mask  &= ~(1U << bitNumber);
		else if (bitNumber < 64)
			mask2 &= ~(1U << (bitNumber-32));
		else if (bitNumber < 128)
			mask3 &= ~(ONE64 << (bitNumber-64));
		else if (bitNumber < (128+64))
			mask4 &= ~(ONE64 << (bitNumber-128));
	}

	bool hasCommonBits(const PaysystemMask& testMask) const // tests if some bits in testMask are also set in this
	{
		return	(mask  & testMask.mask ) != 0 ||
				(mask2 & testMask.mask2) != 0 ||
				(mask3 & testMask.mask3) != 0 ||
				(mask4 & testMask.mask4) != 0;
	}
	bool isSupersetOf (const PaysystemMask& testMask) const // tests if all set bits in testMask are also set in this
	{
		return	(mask  | testMask.mask ) == mask  &&
				(mask2 | testMask.mask2) == mask2 &&
				(mask3 | testMask.mask3) == mask3 &&
				(mask4 | testMask.mask4) == mask4 ;
	}

	void setBits(const PaysystemMask& another)
	{
		mask  |= another.mask;
		mask2 |= another.mask2;
		mask3 |= another.mask3;
		mask4 |= another.mask4;
	}
	void clearBits(const PaysystemMask& another)
	{
		mask  &= ~another.mask;
		mask2 &= ~another.mask2;
		mask3 &= ~another.mask3;
		mask4 &= ~another.mask4;
	}
	void andBits(const PaysystemMask& another)
	{
		mask  &= another.mask;
		mask2 &= another.mask2;
		mask3 &= another.mask3;
		mask4 &= another.mask4;
	}

	bool isPsBitSet(const char* paysystem) const;
	void setPsBit  (const char* paysystem);
	void clearPsBit(const char* paysystem);

	ePsBitNumber getOneRaisedBitNumber() const;

	void parse(CommMsgParser& parser)
	{
		CommMsgBody maskBody;
		parser.parseMsgBody(maskBody);
		CommMsgParser maskParser(maskBody);
		maskParser
			.parseUINT32(mask)
			.parseUINT32(mask2)
			.parseUINT64(mask3)
			.parseUINT64(mask4);
	}
	void compose(CommMsgBody& body) const
	{
		CommMsgBody maskBody;
		maskBody
			.composeUINT32(mask)
			.composeUINT32(mask2)
			.composeUINT64(mask3)
			.composeUINT64(mask4);
		body.composeMsgBody(maskBody);
	}
	const char* masksToHexString(PString& out) const
	{
		out.assign("").appendHexInt64(mask4).append('_'). appendHexInt64(mask3).append('_').appendHexInt(mask2).append('_').appendHexInt(mask);
		return out.c_str();
	}

	const char* toHexString(PString& out) const; // no "0x" prefix
	void fromHexString(const char* str); // would skip "0x" prefix if exists

	static const char decStrSeparator = '_';
	const char* toDecString(PString& out) const;
	void fromDecString(const char* str);

	// temporary for transitional period; they default mask3=0; make those private after everything's settled
	UINT64 getUInt64() const
	{
		return (UINT64(mask2) << 32) | UINT64(mask);
	}
};


class PaySystemDescriptors
{
public:
	struct PaySystemData
	{
		const char*		paySystem;
		EnumCardType	cardType;
		const char*		descrCashin;
		const char*		descrCashout;
		ePsBitNumber	psBitNumber; // #17229
		bool			encryptable;
		eResultInfoType	resultInfoType; // PYR-113333
		// 1) When adding a new char* member to this struct there is a need to add a
		//    private PString member to _PaySystemDataExt for storing the underlying string data.
		// 2) Also for every new member there is a need to add handling in:
		//    _PaySystemDataExt::parse(), _PaySystemDataExt::compose()
		//    _PaySystemDataExt::serializeToString(), _PaySystemDataExt::deserializeFromString()
	};

private: // PYR-112209
	static const PaySystemData table[];
	static inline const PaySystemData* getDescriptor( const char* paySystem );
	static inline const PaySystemData* getDescriptor( BYTE systemByte );

protected:
	static int getDescriptorIndex(const char* paysystem);
	static int getDescriptorIndex(BYTE systemByte);

public:
	static size_t getTableSize();
	static const char* getCardType( BYTE systemByte );
	static BYTE getCardTypeByte( const char* paySystem );
	static const char* paysystemToName( const char* paySystem );
	static const char* paysystemToNameForCashout(const char* paySystem );
	static bool isPaySystemDefined( const char* paySystem );
	static bool isPaySystemBitDefined( const char* paySystem );
	static const char* cardTypeToName( BYTE systemByte );
	static const char* cardTypeToCashoutName( BYTE systemByte );
	static bool isEncryptable( const char* paySystem );
	static int getPaySystemsSize() { return getTableSize(); } // PYR-112209
	static const PaySystemData * getPaySystemAt(int n);
	static const char* paySystemMaskToName(const PaysystemMask& paysystemMask); // only one bit is analysed
	static void paySystemMaskToAllNames(const PaysystemMask& pMask, bool isForbiddenMask, vector<PString>& paysystems); // PYR-45523
	static eResultInfoType paysystemToResultInfoType(const char* paySystem); // PYR-113333
	static ePsBitNumber getPsBitNumber( const char* paySystem );
	static bool isVisaSubtype(const char* paysystem);
	static bool isMcSubtype(const char* paysystem);
	static bool needsSubtypeConversion(const char* cliPaysystem, PString& srvPaysystem /*out*/); // converts 'clientPaysytem' to 'serverPaysystem'
	static bool doPaysystemAndSubtypeMatch(const char* paysystem, const char* productId);
	static bool isVisaSubtypeBit(ePsBitNumber bitNumber);
	static bool isMastercardSubtypeBit(ePsBitNumber bitNumber);
	static bool isAnyCreditCard(const char* paysystem);
//	static bool isPhoneBased(const char* paysystem);
	static bool isExternalPaysystemBit(ePsBitNumber bitNumber); // REAL ps only; excludes: transfer, Conversion, FEE, Migration, Tax, Upsell, etc
	static bool isExternalPaysystem(const char* paysystem);
	static bool isDepositableBit(ePsBitNumber bitNumber);
	static bool isDepositable(const char* paysystem);

	static void processPaySystemsUpdate(CommMsgParser& parser); // PYR-112209
	static void composePaySystemData(CommMsgBody& body); // PYR-112209
	static void serializeExtPaysystems(PStringMap<PString>& dest); // PYR-117171, [PS name]->serialized _PaySystemDataExt
	static void deserializeExtPaysystems(const vector<PString/*serialized _PaySystemDataExt*/>& source); // PYR-117171
};

#endif // cashtypes_h_included
