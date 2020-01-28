//cashtypes.cpp
#include "cashtypes.h"
#include "dbm.h"
#include "ppstring.h"
#include "commmsgbody.h"
#include "commonmacros.h"
#include "composecolls.h"
#include "misc.h"
#include "ppstrtok.h"


//////////////////////////////////////////////////////////////////////////
// NB: WHEN CHANGING THIS TABLE ALSO CHANGE CASHTYPESI18N.CPP
//////////////////////////////////////////////////////////////////////////

const PaySystemDescriptors::PaySystemData PaySystemDescriptors::table[] =
{
//	  paySystem  				cardType					descrCashin 				descrCashout			psBitNumber						encryptable	resultInfoType
//	--------------				--------------------		--------------------		--------------			----------------				-----------	--------------
	{ CHEQUE_NAME,				cardTypeCheque,				CHEQUE_NAME,				CHEQUE_NAME,			ePsBitNumberCheque,				false,		eResultInfoTypeUNKNOWN}, // or eResultInfoTypeCard?
	{ CREDIT_NAME,				cardTypeManualCredit,		CREDIT_NAME,				CREDIT_NAME,			ePsBitNumberUNDEFINED,			false,		eResultInfoTypeUNKNOWN},
	{ DEBIT_NAME,				cardTypeManualDebit,		DEBIT_NAME,					DEBIT_NAME,				ePsBitNumberUNDEFINED,			false,		eResultInfoTypeUNKNOWN},
	{ MASTERCARD_NAME,			cardTypeMasterCard,			MASTERCARD_NAME,			"MasterCard",			ePsBitNumberMaster,				true,		eResultInfoTypeMastercard}, // PYR-60047, sometimes eResultInfoTypeGamersCard
	{ VISA_NAME,				cardTypeVisa,				"Visa",						"Visa",					ePsBitNumberVisa,				true,		eResultInfoTypeCft},
	{ WUNION_NAME,				cardTypeWUnion,				WUNION_NAME,				WUNION_NAME,			ePsBitNumberWUnion,				false,		eResultInfoTypeUNKNOWN},
	{ DINERS_NAME,				cardTypeDiners,				DINERS_NAME,				"DinersClub",			ePsBitNumberDiners,				true,		eResultInfoTypeUNKNOWN },
	{ GLOBALWIRE_NAME,			cardTypeBankWire,			"Bank Wire Transfer",		"bank",					ePsBitNumberBankWire,			false,		eResultInfoTypeUNKNOWN},
	{ NETELLER_NAME,			cardTypeNeteller,			"NETELLER",					"NETELLER",				ePsBitNumberNeteller,			false,		eResultInfoTypeNeteller},
	{ CCOIN_NAME,				cardTypeCCoin,				CCOIN_NAME,					"CentralCoin",			ePsBitNumberCCoin,				false,		eResultInfoTypeUNKNOWN},
	{ FIREPAY_NAME,				cardTypeFirePay,			FIREPAY_NAME,				FIREPAY_NAME,			ePsBitNumberFirepay,			false,		eResultInfoTypeUNKNOWN },
	{ GLOBALDIRECT_NAME,		cardTypeBankDirect,			"ELV Direct Debit",			"bank",					ePsBitNumberBankDirect,			true,		eResultInfoTypeBankDirect},
	{ EPASSPORTE_NAME,			cardTypeEPassporte,			"ePassporte",				"ePassporte",			ePsBitNumberEPassporte,			false,		eResultInfoTypeUNKNOWN},
	{ SOLO_NAME,				cardTypeSolo,				"DEBIT CARD",				"Debit Card",			ePsBitNumberSolo,				true,		eResultInfoTypeSolo},
	{ ECHECK_NAME,				cardTypeECheck,				ECHECK_NAME,				ECHECK_NAME,			ePsBitNumberECheck,				true,		eResultInfoTypeECheck },
	{ RTBT_NAME,				cardTypeRtbt,				"Online Bank Transfer",		RTBT_NAME,				ePsBitNumberRTBT,				false,		eResultInfoTypeUNKNOWN},
	{ PAYSAFE_NAME,				cardTypePaySafe,			"PaySafeCard",				PAYSAFE_NAME,			ePsBitNumberPaySafe,			false,		eResultInfoTypeUNKNOWN},
	{ MONEYGRAM_NAME,			cardTypeMoneygram,			"MoneyGram",				"MoneyGram",			ePsBitNumberMoneygram,			false,		eResultInfoTypeUNKNOWN},
	{ MGCORRECT_NAME,			cardTypeMoneygram,			"MoneyGram",				"MoneyGram",			ePsBitNumberMoneygram,			false,		eResultInfoTypeUNKNOWN},
	{ MONEYORDER_NAME,			cardTypeMoneyOrder,			"Money Order",				"Money Order",			ePsBitNumberMoneyOrder,			false,		eResultInfoTypeUNKNOWN},
	{ MRMC_NAME,				cardTypeMRMC,				"Visa Debit",				"Visa Debit",			ePsBitNumberMRMC,				true,		eResultInfoTypeUNKNOWN},
	{ WIRE_NAME,				cardTypeWireCashout,		"Wire transfer",			"Wire Transfer",		ePsBitNumberWire,				false,		eResultInfoTypeWire},
	{ MBOOKERS_NAME,			cardTypeMBookers,			"Skrill",					"Skrill",				ePsBitNumberMBookers,			false,		eResultInfoTypeMbookers},
	{ TRANSFER_NAME,			cardTypeTransfer,			"Transfer",					"Transfer",				ePsBitNumberTransfer,			false,		eResultInfoTypeUNKNOWN},
	{ TRANSFER_OUT_NAME,		cardTypeTransferOut,		"Transfer out",				"Transfer out",			ePsBitNumberUNDEFINED,			false,		eResultInfoTypeUNKNOWN},
	{ USEMYBANK_NAME,			cardTypeUseMyBank,			"Use My Bank",				"Use My Bank",			ePsBitNumberUseMyBank,			false,		eResultInfoTypeUNKNOWN},
	{ DBT_NAME,					cardTypeDBT,				"Direct Bank Transfer", 	"Direct Bank Transfer",	ePsBitNumberDirbank,			false,		eResultInfoTypeDbt },
	{ BPAY_NAME,				cardTypeBPay,				"bPay",						"bPay",					ePsBitNumberBankWire,			false,		eResultInfoTypeUNKNOWN},
	{ CLICKANDBUY_NAME,			cardTypeClickNBuy,			"ClickAndBuy",				"ClickAndBuy",			ePsBitNumberClickAndBuy,		false,		eResultInfoTypeClickAndBuy},
	{ CITADEL_NAME,				cardTypeCitadel,			"Citadel",					"Citadel",				ePsBitNumberCitadel,			false,		eResultInfoTypeUNKNOWN},
	{ AFF_DEPOSIT_NAME,			cardTypeAffDeposit,			"Paid by affiliate",		"",						ePsBitNumberUNDEFINED,			false,		eResultInfoTypeUNKNOWN},
	{ WEBMONEY_NAME,			cardTypeWebMoney,			"WebMoney",					"WebMoney",				ePsBitNumberWebMoney,			false,		eResultInfoTypeWebMoney},
	{ ENVOY_RTBT_NAME,			cardTypeEnvoyRtbt,			"Online Bank Transfer",		"",						ePsBitNumberEnvoyRtbt,			false,		eResultInfoTypeUNKNOWN},
	{ MG_CASHOUT_NAME,			cardTypeMgCashout,			"",							"CashRefund",			ePsBitNumberMgCashout,			false,		eResultInfoTypeUNKNOWN},
	{ CLICK2PAY_NAME,			cardTypeClick2Pay,			"Click2Pay",				"Click2Pay",			ePsBitNumberClick2Pay,			false,		eResultInfoTypeUNKNOWN},
	{ ENVOY_LBT_NAME,			cardTypeEnvoyLbt,			"Local Bank Transfer",		"", 					ePsBitNumberEnvoyLbt,			false,		eResultInfoTypeUNKNOWN},
	{ MONETA_RBT_NAME,			cardTypeMonetaCashout,		"",							"Moneta",				ePsBitNumberMoneta,				false,		eResultInfoTypeMoneta},
	{ CARTE_BLEUE_NAME,			cardTypeCarteBleue,			"Debit Card",				"Debit Card",			ePsBitNumberCarteBleue,			true,		eResultInfoTypeSolo},
	{ BUGSY_CONVERSION_NAME,	cardTypeBugsyConversion,	"BugsysClub Conversion",	"", 					ePsBitNumberUNDEFINED,			false,		eResultInfoTypeUNKNOWN},
	{ ENTROPAY_NAME,			cardTypeEntropay,			"",							"", 					ePsBitNumberEntropay,			true,		eResultInfoTypeUNKNOWN},
	{ BOLETO_NAME,				cardTypeBoleto,				"Boleto",					"", 					ePsBitNumberBoleto,				false,		eResultInfoTypeUNKNOWN},
	{ UKASH_NAME,				cardTypeUkash,				"Ukash",					"Ukash",				ePsBitNumberUkash,				true,		eResultInfoTypeUkash},
	{ IT_CONVERSION_NAME,		cardTypeITConversion,		".IT Conversion",			".IT Conversion",		ePsBitNumberItConversion,		false,		eResultInfoTypeUNKNOWN},
	{ MC_TRANSFER_NAME,			cardTypeMCTransfer,			"Currency conversion",		"Currency conversion",	ePsBitNumberMcTransfer,			false,		eResultInfoTypeUNKNOWN},
	{ WIRECARD_WBT_NAME,		cardTypeWirecardWbt,		"Wirecard WBT",				"", 					ePsBitNumberWirecardWbt,		false,		eResultInfoTypeUNKNOWN},
	{ INSTADEBIT_NAME,			cardTypeInstaDebit,	    	"InstaDebit",				"InstaDebit",			ePsBitNumberInstaDebit,			false,		eResultInfoTypeInstaDebit},
	{ DANKORT_NAME,				cardTypeDankort,	    	"Dankort",					"Dankort",				ePsBitNumberDankort,			false,		eResultInfoTypeUNKNOWN },
	{ UMWALLET_NAME,			cardTypeUMWallet,	    	"UMWallet",					"UMWallet", 			ePsBitNumberUMWallet,			false,		eResultInfoTypeUNKNOWN },
	{ ASTROPAY_NAME,			cardTypeAstropay,	    	"Astropay",					"Astropay",				ePsBitNumberAstropay,			false,		eResultInfoTypeUNKNOWN },
	{ TICKETPREMIUM_NAME,		cardTypeTicketPremium,	 	"TicketPremium",			"TicketPremium",		ePsBitNumberTicketPrem,			false,		eResultInfoTypeUNKNOWN },
	{ TODITO_NAME,				cardTypeTodito,				"Todito",					"Todito",				ePsBitNumberTodito,		 		false,		eResultInfoTypeUNKNOWN },
	{ QIWI_NAME,				cardTypeQiwi,				"",							"",						ePsBitNumberQiwi,				true,		eResultInfoTypeUNKNOWN },
	{ EPAY_NAME,				cardTypeEPay,				"STARS CARD",				"",						ePsBitNumberEPay,				true,		eResultInfoTypeUNKNOWN },
	{ PAYPAL_NAME,				cardTypePayPal,				"PayPal",					"PayPal",				ePsBitNumberPayPal,				false,		eResultInfoTypePayPal},
	{ DINEROMAIL_NAME,			cardTypeDineromail,			"Dineromail",				"Dineromail",			ePsBitNumberDineromail,			false,		eResultInfoTypeUNKNOWN },
	{ FR_MIGRATION_NAME,		cardTypeFRMigration,		".FR Migration",			".FR Migration",		ePsBitNumberFrMigration,		false,		eResultInfoTypeUNKNOWN },
	{ ROBOKASSA_NAME,			cardTypeRobokassa,			"ROBOKASSA",				"",						ePsBitNumberRobokassa,			false,		eResultInfoTypeUNKNOWN },
	{ PSTARSPAY_NAME,			cardTypePStarsPay,			"PokerStarsPay",			"",						ePsBitNumberPStarsPay,  		false,		eResultInfoTypeUNKNOWN },
	{ PAGOSONLINE_NAME,			cardTypePagosOnline,		"PagosOnline",				"",						ePsBitNumberPagosOnline,		false,		eResultInfoTypeUNKNOWN },
	{ RICARICA_NAME,			cardTypeRicarica,			"Ricarica",					"",						ePsBitNumberRicarica,			false,		eResultInfoTypeUNKNOWN },
	{ GIROPAY_NAME,				cardTypeGiropay,			"GIROPAY",					"",						ePsBitNumberGiropay,			false,		eResultInfoTypeUNKNOWN },
	{ IDEAL_NAME,				cardTypeIdeal,				"IDEAL",					"",						ePsBitNumberIdeal,				false,		eResultInfoTypeUNKNOWN },
	{ EPS_NAME,					cardTypeEps,				"EPS",						"",						ePsBitNumberEps,				false,		eResultInfoTypeUNKNOWN },
	{ STARSCARD_NAME,			cardTypeStarscard,			"STARS CARD",				"STARS CARD",			ePsBitNumberStarscard,			true,		eResultInfoTypeUNKNOWN  },
	{ SOFORT_NAME,				cardTypeSofort,				"Sofortueberweisung",		"",						ePsBitNumberSofort,				false,		eResultInfoTypeUNKNOWN },
	{ BANKLINKSWED_NAME,		cardTypeBanklinkswed,       "Swedbank",					"",						ePsBitNumberBanklinkswed,		false,		eResultInfoTypeUNKNOWN },
	{ EUTELLER_NAME,			cardTypeEuteller,			"Euteller",					"",						ePsBitNumberEuteller,			false,		eResultInfoTypeUNKNOWN },
	{ BE_MIGRATION_NAME,		cardTypeBeMigration,		".BE Migration",			".BE Migration",		ePsBitNumberBeMigration,		false,		eResultInfoTypeUNKNOWN },
    { VODAFONE_NAME,			cardTypeVodafone,			"VODAFONE",	            	"",						ePsBitNumberVodafone,			false,		eResultInfoTypeUNKNOWN },
    { O2_NAME,				    cardTypeO2,				    "O2",		                "",						ePsBitNumberO2, 				false,		eResultInfoTypeUNKNOWN },
    { TMOBILE_NAME,				cardTypeTMobile,			"TMOBILE",          		"",						ePsBitNumberTMobile,			false,		eResultInfoTypeUNKNOWN },
	{ MBOOKOBT_NAME,            cardTypeMBookOBT,           "Skrill OBT",				"",                     ePsBitNumberMBookOBT,			false,		eResultInfoTypeUNKNOWN },
	{ ALFABANK_NAME,			cardTypeAlfabank,			"Mobile Visa card",			"",						ePsBitNumberAlfabank,			true,		eResultInfoTypeUNKNOWN  },
	{ QIWIWALLET_NAME,		    cardTypeQiwiWallet,	 	    "Qiwi Wallet",			    "Qiwi Wallet",	    	ePsBitNumberQiwiWallet,			false,		eResultInfoTypeQiwiWallet},
	{ TRUSTPAY_NAME,			cardTypeTrustPay,			"TrustPay",		            "",						ePsBitNumberTrustPay,			false,		eResultInfoTypeUNKNOWN },
	{ INSTANTBANK_NAME,		    cardTypeInstantBank,	 	"Instant Bank",			    "",	    	            ePsBitNumberInstantBank,		false,		eResultInfoTypeUNKNOWN },
	{ WALLETONE_NAME,			cardTypeWalletOne,			"Wallet One",		        "Wallet One",			ePsBitNumberWalletOne,			true,		eResultInfoTypeWalletOne }, // #20110
	{ NORDEA_NAME,				cardTypeNordea,				"NORDEA",					"",						ePsBitNumberNordea,				false,		eResultInfoTypeUNKNOWN },
	{ ES_MIGRATION_NAME,		cardTypeEsMigration,		"Conversion",			    "Conversion",			ePsBitNumberEsMigration,		false,		eResultInfoTypeUNKNOWN },
	{ NEMKONTO_NAME,		    cardTypeNemkontoCashout,	"",							"NemKonto",				ePsBitNumberNemconto,			false,		eResultInfoTypeDbt},
	{ PSTARSTOPUP_NAME,			cardTypePStarsTopUp,		"PStars TopUp",				"",						ePSBitNumberPStarsTopUp,		false,		eResultInfoTypeUNKNOWN },
	{ PSPREPAIDCARD_NAME,		cardTypePSPrepaidCard,		"PStars Prepaid Card",		"",						ePSBitNumberPSPrepaidCard,		false,		eResultInfoTypeUNKNOWN },
	{ RECHARGEPS_NAME,			cardTypeRechargePS,		    "Recharge PokerStars",		"",						ePSBitNumberRechargePS,			false,		eResultInfoTypeUNKNOWN },
	{ PSPREPAID_NAME,		    cardTypePSPrepaid,		    "PStars Prepaid",		    "",						ePSBitNumberPSPrepaid,			false,		eResultInfoTypeUNKNOWN },
	{ YANDEXMONEY_NAME,		    cardTypeYandexMoney,	 	"Yandex Money",			    "Yandex Money",	    	ePsBitNumberYandexMoney,		true,		eResultInfoTypeYandexMoney },
	{ SEVENELEVEN_NAME,		    cardType7Eleven,	 	    "7ELEVEN",			        "",	    	            ePsBitNumber7Eleven,			false,		eResultInfoTypeUNKNOWN },
	{ OXXO_NAME,		        cardTypeOxxo,    	 	    "OXXO",			            "",	    	            ePsBitNumberOxxo,				false,		eResultInfoTypeUNKNOWN },
	{ PROVINCIAPAGOS_NAME,		cardTypeProvinciaPagos,	 	"Provincia Pagos",			"",	    	            ePsBitNumberProvinciaPagos,		false,		eResultInfoTypeUNKNOWN },
	{ COBROEXPRESS_NAME,		cardTypeCobroExpress,	 	"Cobro Express",			"",	    	            ePsBitNumberCobroExpress,		false,		eResultInfoTypeUNKNOWN },
	{ RIPSAPAGOS_NAME,		    cardTypeRipsaPagos,	 	    "RIPSA PAGOS",			    "",	    	            ePsBitNumberRipsaPagos,			false,		eResultInfoTypeUNKNOWN },
	{ RECHARGEPOKERSTARS_NAME,  cardTypeRechargePokerStars, "RECHARGE POKERSTARS",      "",						ePsBitNumberRechargePokerstars,	false,		eResultInfoTypeUNKNOWN },
	{ PAYSYSTEM_FEE_NAME,       cardTypePaysystemFee,       "FEE",                      "FEE",	                ePsBitNumberPaysystemFee,		false,		eResultInfoTypeUNKNOWN },
	{ FULLTILT_NAME,			cardTypeFullTilt,			"Full Tilt Poker",			"Full Tilt Poker",		ePsBitNumberFullTilt,			false,		eResultInfoTypeUNKNOWN },
	{ FULLTILT_ASSET_MIGR_NAME,	cardTypeFullTiltAssetMigr,	"Full Tilt Poker Assets Credit","",					ePsBitNumberFullTiltAssetMigr,	false,		eResultInfoTypeUNKNOWN },
	{ MOVIPIN_NAME,				cardTypeMoviPin,			"MoviPIN",					"",				        ePsBitNumberMoviPin,			false,		eResultInfoTypeUNKNOWN },
	{ MOBILEPAYMENT_NAME,		cardTypeMobilePayment,		"Mobile Payment",			"",				        ePsBitNumberMobilePayment,		false,		eResultInfoTypeUNKNOWN },
	{ ENV_BALOTO_NAME,			cardTypeEnvBaloto,			"BALOTO",					"",				        ePsBitNumberEnvBaloto,			false,		eResultInfoTypeUNKNOWN },
	{ PAGOEFECTIVO_NAME,		cardTypePagoEfectivo,		"Pago Efectivo",			"",				        ePsBitNumberPagoEfectivo,		false,		eResultInfoTypeUNKNOWN },
	{ PSTARSPREPAID_NAME,		cardTypePStarsPrepaid,		"PStars Prepaid",			"",						ePsBitNumberPStarsPrepaid,		false,		eResultInfoTypeUNKNOWN },
	{ PSCASH_NAME,		        cardTypePSCash,     		"Stars Cash",			    "",						ePsBitNumberPSCash,				false,		eResultInfoTypeUNKNOWN },
	{ HIPAY_NAME,		        cardTypeHiPay,     		    "HiPay",			        "HiPay",				ePsBitNumberHiPay,				false,		eResultInfoTypeHiPay},
	{ BOKU_NAME,		        cardTypeBoku,     		    "BOKU",			            "",				        ePsBitNumberBoku,				false,		eResultInfoTypeUNKNOWN },
	{ UPSELL_AWARD_NAME,        cardTypeUpsellAward,        "AWARD",                    "",      	            ePsBitNumberUpsellAward,		false,		eResultInfoTypeUNKNOWN },
	{ BOKUTHREEUK_NAME,	        cardTypeBokuThreeUK,	    "BOKU THREE UK",            "",				        ePsBitNumberBokuThreeUK,		false,		eResultInfoTypeUNKNOWN },
	{ BOKUVODAFONE_NAME,		cardTypeBokuVodafone,     	"BOKU VODAFONE",            "",				        ePsBitNumberBokuVodafone,		false,		eResultInfoTypeUNKNOWN },
	{ BOKUO2_NAME,		        cardTypeBokuO2,     		"BOKU O2",		            "",				        ePsBitNumberBokuO2,				false,		eResultInfoTypeUNKNOWN },
	{ BOKUORANGE_NAME,		    cardTypeBokuOrange,     	"BOKU ORANGE",	            "",				        ePsBitNumberBokuOrange,			false,		eResultInfoTypeUNKNOWN },
	{ PAYSYSTEM_3V_NAME,        cardType3V,			        "StarsCard",				"StarsCard",	        ePsBitNumber3V,					false,		eResultInfoType3V},
	{ PAYNEARME_NAME,		    cardTypePayNearMe,	 	    "Pay Near Me",			    "",	    	            ePsBitNumberPayNearMe,			false,		eResultInfoTypeUNKNOWN},
	{ RESORT_NAME,				cardTypeResort,	 	        "RESORTS",					"RESORTS",	    	    ePsBitNumberResort,				false,		eResultInfoTypeResort},
	{ PAYSYSTEM_TAX_NAME,       cardTypeTax,				"TAX",                      "TAX",	                ePsBitNumberTax,				false,		eResultInfoTypeUNKNOWN },
	{ AGMO_NAME,				cardTypeAgmo,				"ComGate",					"",						ePsBitNumberAgmo,				false,		eResultInfoTypeUNKNOWN },
	{ SAFETYPAY_NAME,			cardTypeSafetyPay,			"SAFETYPAY",				"",						ePsBitNumberSafetyPay,			false,		eResultInfoTypeUNKNOWN },
	{ PSPAY_ASTROPAY_NAME,		cardTypePsPayAstropay,	    "Stars Transfer",			"Stars Transfer",		ePsBitNumberPsPayAstropay,		false,		eResultInfoTypePsPayAstropay},
	{ PAYSYSTEM_3VREFUND_NAME,  cardType3VRefund,			"StarsCard Refund",			"",          	        ePsBitNumber3VRefund,			false,		eResultInfoTypeUNKNOWN},
	{ ELV_SEPA_NAME,			cardTypeElvSepa,			"ELV Direct Debit",			"bank",					ePsBitNumberElvSepa,			true,		eResultInfoTypeBankDirect},
	{ PAYLUCKY_NAME,			cardTypePayLucky,			"PayLucky",					"PayLucky",				ePsBitNumberPayLucky,			false,		eResultInfoTypePayLucky}, // VISA - encrypted, PAYLUCKY token - not
	{ PSIOB_NAME,			    cardTypePSIOB,	    	    "PSIOB",				    "",			            ePsBitNumberPSIOB,				false,		eResultInfoTypeUNKNOWN },
	{ VANILLA_NAME,			    cardTypeVanilla,	    	"Vanilla",				    "",			            ePsBitNumberVanilla,			false,		eResultInfoTypeUNKNOWN },
	{ IDEBIT_NAME,				cardTypeIDebit,				"IDEBIT",					"",						ePsBitNumberIDebit,				false,		eResultInfoTypeUNKNOWN },
	{ SBERBANK_NAME,			cardTypeSberBank,			"SBERBANK",					"",						ePsBitNumberSberbank,			false,		eResultInfoTypeUNKNOWN },
	{ VIVAPAYMENTS_NAME,		cardTypeVivaPayments,		"VIVAPAYMENTS",				"",						ePsBitNumberVivaPayments,		false,		eResultInfoTypeUNKNOWN },
	{ STARSVOUCHER_NAME,		cardTypeStarsVouchers,	    "Stars Gift Card",		    "",						ePsBitNumberStarsVouchers,		false,		eResultInfoTypeUNKNOWN },
	{ SIGHTLINE_NAME,			cardTypeSightline,			"Play+ Prepaid Card",		"Play+ Prepaid Card",	ePsBitNumberSightline,			false,		eResultInfoTypeSightline}, // VISA - encrypted, SIGHTLINE token - not
	{ SIGHTLINE_NR_NAME,		cardTypeSightlineNR,		"Play+ Prepaid Card",		"Play+ Prepaid Card",	ePsBitNumberSightline,			false,		eResultInfoTypeSightline}, // same bit as
	{ FTPPAY_NAME,		        cardTypeFTPPay,	            "FTP Pay",			        "FTP Pay",	    	    ePsBitNumberFTPPay,				false,		eResultInfoTypePsPayAstropay},
	{ VANILLARELOAD_NAME,		cardTypeVanillaReload,	    "Vanilla Reload",	        "Vanilla Reload",		ePsBitNumberVanillaReload,		false,		eResultInfoTypeUNKNOWN },
	{ NEOSURF_NAME,		        cardTypeNeosurf,	        "Neosurf",	                "Neosurf",		        ePsBitNumberNeosurf,    		false,		eResultInfoTypeUNKNOWN },
	{ BOLETO_ASTROPAY_NAME,		cardTypeBoletoAstropay,	    "Boleto Bancario",	        "",		                ePsBitNumberBoletoAstropay,		false,		eResultInfoTypeUNKNOWN },
	{ PROFEE_CARDS_NAME,		cardTypeProfeeCards,	    "Profee Cards",	            "",		                ePsBitNumberProfeeCards,		false,		eResultInfoTypeProfeeCards},
	{ PROFEE_QIWI_NAME,		    cardTypeProfeeQiwi,	        "Profee Qiwi",	            "",		                ePsBitNumberProfeeQiwi,			false,		eResultInfoTypeProfeeQiwi},
	{ PROFEE_YANDEX_NAME,		cardTypeProfeeYandex,	    "Profee Yandex",	        "",		                ePsBitNumberProfeeYandex,		false,		eResultInfoTypeProfeeYandex},
	{ PROFEE_MOBILEPAYMENT_NAME,cardTypeProfeeMobilepayment,"Profee Mobilepayment",	    "",		                ePsBitNumberProfeeMobilepayment,false,		eResultInfoTypeProfeeMobile},
	{ PROFEE_WALLET_NAME,		cardTypeProfeeWallet,	    "Profee Wallet",	        "",		                ePsBitNumberProfeeWallet,		false,		eResultInfoTypeProfeeWallet},
	{ ECOPAYZ_NAME,		        cardTypeEcoPayZ,	        "ECOPAYZ",	                "ECOPAYZ",		        ePsBitNumberEcoPayZ,			false,		eResultInfoTypeCard},
	{ STARSWALLET_NAME,			cardTypeStarsWallet,		"StarsWallet",				"StarsWallet",			ePsBitNumberStarsWallet,		false,		eResultInfoTypeStarsWallet}, // PYR-37328
	{ IN_RTBT_NAME,				cardTypeInRtbt,				"NetBanking",				"NetBanking",			ePsBitNumberInRtbt,				false,		eResultInfoTypeInRtbt}, // PYR-35616
	{ IN_WALLET_NAME,			cardTypeInWallet,			"eWallet",					"eWallet",				ePsBitNumberInWallet,			false,		eResultInfoTypeUNKNOWN }, // PYR-35616
	{ CASHFREE_NAME,			cardTypeCashfree,			"CashFree",					"CashFree",				ePsBitNumberCashfree,			false,		eResultInfoTypeCashfreeHosted}, // PYR-55411
	{ MUCHBETTER_NAME,			cardTypeMuchBetter,			"MuchBetter",				"MuchBetter",			ePsBitNumberMuchBetter,			false,		eResultInfoTypeMuchBetter}, // PYR-61756
	{ PAYSYSTEM_AG_NAME,		cardTypeAG,					"",							"",						ePsBitNumberAG,					false,		eResultInfoTypeCard}, // PYR-52150
	{ PAYMONEY_YANDEX_NAME,		cardTypePayMoney,			"PayMoney",					"PayMoney",				ePsBitNumberPayMoney,			false,		eResultInfoTypePayMoney}, // PYR-66127
	{ VANILLA_DIRECT_NAME,		cardTypeVanillaDirect,		"VanillaDirect",			"",						ePsBitNumberVanillaDirect,		false,		eResultInfoTypeUNKNOWN }, // PYR-73312
	{ GAMERSCARD_NAME,			cardTypeGamersCard,			"GamersCard",				"GamersCard",			ePsBitNumberGamersCard,			false,		eResultInfoTypeUNKNOWN }, // PYR-60047 // MASTERCARD - encrypted
	{ CUP_RTBT_NAME,			cardTypeCupRtbt,			"",							"",						ePsBitNumberCupRtbt,			false,		eResultInfoTypeUNKNOWN }, // PYR-69195
	{ GENERIC_CREDITCARD_NAME,	cardTypeGenericCredit,		"Credit Card",				"Credit Card",			ePsBitNumberGenericCredit,		false,		eResultInfoTypeUNKNOWN }, // PYR-74280 // VISA/MASTERCARD - encrypted
	{ GENERIC_DEBITCARD_NAME,	cardTypeGenericDebit,		"Debit Card",				"Debit Card",			ePsBitNumberGenericDebit,		false,		eResultInfoTypeUNKNOWN }, // PYR-74280 // VISA/MASTERCARD - encrypted
	{ RUPAY_NAME,				cardTypeRupay,				"RuPay",					"RuPay",				ePsBitNumberRupay,				true,		eResultInfoTypeUNKNOWN}, // PYR-74280
	{ CASHFREE_NETBANKING_NAME,	cardTypeCashfreeNetbanking,	"NetBanking",				"",						ePsBitNumberCashfreeNetbanking,	false,		eResultInfoTypeUNKNOWN }, // PYR-70255
	{ CASHFREE_UPI_NAME,		cardTypeCashfreeUpi,		"UPI",						"",						ePsBitNumberCashfreeUpi,		false,		eResultInfoTypeUNKNOWN }, // PYR-70255
	{ CASHFREE_MOBILEWALLET_NAME, cardTypeCashfreeMobilewallet, "Mobile Wallet",		"",						ePsBitNumberCashfreeMobileWallet,false,		eResultInfoTypeUNKNOWN}, // PYR-70255
	{ PAYTM_NAME,				cardTypePaytm,				"Paytm",					"Paytm",				ePsBitNumberPaytm,				false,		eResultInfoTypePaytm}, // PYR-55412
	{ APPLE_PAY_NAME,			cardTypeApplePay,			"Apple Pay",				"Apple Pay",			ePsBitNumberApplePay,			false,		eResultInfoTypeApplePay}, // PYR-36843
	{ GIMO_TRANSFER_NAME,		cardTypeGimoTransfer,		"GIMO Transfer",			"GIMO Transfer",		ePsBitNumberGimoTransfer,		false,		eResultInfoTypeGimo}, // PYR-90603
	{ GIMO_PAYMENT_NAME,		cardTypeGimoPayment,		"GIMO Payment",				"GIMO Payment",			ePsBitNumberGimoPayment,		false,		eResultInfoTypeGimo}, // PYR-90603
	{ MIR_NAME,		            cardTypeMir,		        "MIR",				        "MIR",			        ePsBitNumberMir,				true,		eResultInfoTypeUNKNOWN},  // PYR-95023 encrypted like VISA/MASTERCARD
	{ SWISH_PAYSYSTEM_NAME,		cardTypeSwishPayment,		"SWISH Payment",			"SWISH Payment",		ePsBitNumberSwishPayment,		false,		eResultInfoTypeSwish}, // PYR-98052
	{ INTERAC_PAYSYSTEM_NAME,	cardTypeInterac,			"Interac",					"Interac Payment",		ePsBitNumberInterac,			false,		eResultInfoTypeUNKNOWN }, //PYR-101793 PYR-110056
	{ NOW2PAYMOBILE_NAME,	    cardTypeNow2PayMobile,		"NOW2PAYMOBILE",			"NOW2PAYMOBILE",		ePsBitNumberNow2PayMobile,		false,		eResultInfoTypeUNKNOWN }, // PYR-105915
	{ NOW2PAYWEB_NAME,	        cardTypeNow2PayWeb,		    "NOW2PAYWEB",			    "NOW2PAYWEB",		    ePsBitNumberNow2PayWeb,			false,		eResultInfoTypeUNKNOWN }, // PYR-105915
	//////////////////////////////////////////////////////////////////////////
	// 1) WHEN CHANGING THIS TABLE ALSO CHANGE CASHTYPESI18N.CPP
	// 2) When adding 'insert into PAYSYSTEMS2' also create 'insert into PS_Descriptors'
	//////////////////////////////////////////////////////////////////////////
};

// PYR-112209
struct _PaySystemDataExt
{
private:
	PaySystemDescriptors::PaySystemData psData;
	// The following members not to be modified directly. Must only be updated by parse method for pointer consistency.
	PString _paySystem;		// psData.paySystem must always point to this member
	PString _descrCashin;	// psData.descrCashin must always point to this member
	PString _descrCashout;	// psData.descrCashout must always point to this member

	static const PString serializeSeparator; // PYR-117171

	void _init()
	{
		psData.paySystem = _paySystem.c_str();
		psData.descrCashin = _descrCashin.c_str();
		psData.descrCashout = _descrCashout.c_str();
	}

	_PaySystemDataExt& operator =(const _PaySystemDataExt& _psDataExt) // PYR-117171
	{
		if (this == &_psDataExt)
			return *this;

		psData = _psDataExt.psData;
		_paySystem = _psDataExt._paySystem;
		_descrCashin = _psDataExt._descrCashin;
		_descrCashout = _psDataExt._descrCashout;
		_init();
		return *this;
	}

public:
	_PaySystemDataExt()	{ _init(); }

	explicit _PaySystemDataExt(const PaySystemDescriptors::PaySystemData& _psData)
		: psData(_psData)
		, _paySystem(_psData.paySystem)
		, _descrCashin(_psData.descrCashin)
		, _descrCashout(_psData.descrCashout)
	{
		_init();
	}

	_PaySystemDataExt(const _PaySystemDataExt& _psDataExt) : _PaySystemDataExt(_psDataExt.psData) {}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseStringP(_paySystem)
			.parseStringP(_descrCashin)
			.parseStringP(_descrCashout)
			.parseBOOL(psData.encryptable);
		parseEnumAsUINT32(parser, psData.cardType);
		parseEnumAsUINT32(parser, psData.psBitNumber);
		parseEnumAsUINT32(parser, psData.resultInfoType);
		_init();
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeString(_paySystem)
			.composeString(_descrCashin)
			.composeString(_descrCashout)
			.composeBOOL(psData.encryptable);
		composeEnumAsUINT32(body, psData.cardType);
		composeEnumAsUINT32(body, psData.psBitNumber);
		composeEnumAsUINT32(body, psData.resultInfoType);
	}

	const PaySystemDescriptors::PaySystemData& getPsData()
	{
		_init();
		return psData;
	}

	const PString& getPaySystem() const { return _paySystem; }

	void serializeToString(PString& buf) const  // PYR-117171
	{
		buf = "";
		buf.append(_paySystem);
		buf.append(serializeSeparator).append(_descrCashin);
		buf.append(serializeSeparator).append(_descrCashout);
		buf.append(serializeSeparator).append(boolAsYN(psData.encryptable));
		buf.append(serializeSeparator).appendInt(psData.cardType);
		buf.append(serializeSeparator).appendInt(psData.psBitNumber);
		buf.append(serializeSeparator).appendInt(psData.resultInfoType);
		PLog("_PSExt::ser: %s", buf.c_str());
	}
	bool deserializeFromString(const PString& buf) // PYR-117171
	{
		PLog("_PSExt::deser: %s", buf.c_str());
		try
		{
			_PaySystemDataExt tempPsData;
			PStrTok strTok(buf, serializeSeparator);
			PStrTok::iterator it = strTok.first();
			if (!it)
				return false;
			tempPsData._paySystem = PString(it.begin(), it.end());
			++it;
			if (!it)
				return false;
			tempPsData._descrCashin = PString(it.begin(), it.end());
			++it;
			if (!it)
				return false;
			tempPsData._descrCashout = PString(it.begin(), it.end());
			++it;
			if (!it)
				return false;
			const PString encrYN(it.begin(), it.end());
			tempPsData.psData.encryptable = encrYN.equals("Y");
			++it;
			if (!it)
				return false;
			const PString cardTypeStr(it.begin(), it.end());
			tempPsData.psData.cardType = (EnumCardType)atoi(cardTypeStr);
			++it;
			if (!it)
				return false;
			const PString psBitNumberStr(it.begin(), it.end());
			tempPsData.psData.psBitNumber = (ePsBitNumber)atoi(psBitNumberStr);
			++it;
			if (!it)
				return false;
			const PString resultInfoTypeStr(it.begin(), it.end());
			tempPsData.psData.resultInfoType = (eResultInfoType)atoi(resultInfoTypeStr);
			++it;
			if (it) // Must be the end of buf
				return false;

			*this = tempPsData;
			PLog("_PSExt::deser OK, ps=%s", _paySystem.c_str());
			return true;
		}
		catch (const PError& ex) // To be extra safe with parsing
		{
			PLog("_PSExt::deser FAILED: why: %s", ex.why());
			return false;
		}
	}

};
const PString _PaySystemDataExt::serializeSeparator = "&&"; // PYR-117171

static list<_PaySystemDataExt> _paySystemDataTableExt; // PYR-112209

size_t PaySystemDescriptors::getTableSize()
{
	return SZARR( table ) + _paySystemDataTableExt.size(); // PYR-112209
}

inline const PaySystemDescriptors::PaySystemData* PaySystemDescriptors::getDescriptor( BYTE systemByte )
{
	// PYR-112209
	for( int i = 0; i < getPaySystemsSize(); i++ )
	{
		const PaySystemData* psData = getPaySystemAt(i);
		if( systemByte == psData->cardType )
			return psData;
	}
	return 0;
}

inline const PaySystemDescriptors::PaySystemData* PaySystemDescriptors::getDescriptor( const char* paySystem )
{
	// PYR-112209
	for( int i = 0; i < getPaySystemsSize(); i++ )
	{
		const PaySystemData* psData = getPaySystemAt(i);
		if( 0 == PString::compareIgnoreCase( psData->paySystem, paySystem ) )
			return psData;
	}
	return 0;
}

int PaySystemDescriptors::getDescriptorIndex( const char* paySystem )
{
	// PYR-112209
	for( int i = 0; i < getPaySystemsSize(); i++ )
	{
		if( 0 == PString::compareIgnoreCase( getPaySystemAt(i)->paySystem, paySystem ) )
			return i;
	}
	return -1;
}

int PaySystemDescriptors::getDescriptorIndex( BYTE systemByte )
{
	// PYR-112209
	for( int i = 0; i < getPaySystemsSize(); i++ )
	{
		if( systemByte == getPaySystemAt(i)->cardType )
			return i;
	}
	return -1;
}

const char* PaySystemDescriptors::getCardType( BYTE systemByte )
{
	const PaySystemData* ds = getDescriptor( systemByte );
	return ds ? ds->paySystem : 0;
}

BYTE PaySystemDescriptors::getCardTypeByte( const char* paySystem )
{
	const PaySystemData* ds = getDescriptor( paySystem );
	return static_cast< BYTE >( ds ? ds->cardType : 0 );
}

const char* PaySystemDescriptors::paysystemToName( const char* paySystem )
{
	const PaySystemData* ds = getDescriptor( paySystem );
	return ds ? ds->descrCashin : "";
}

const char* PaySystemDescriptors::paysystemToNameForCashout( const char* paySystem )
{
	const PaySystemData* ds = getDescriptor(paySystem);
	return ds ? ds->descrCashout : "";
}

bool PaySystemDescriptors::isPaySystemDefined( const char* paySystem )
{
	const PaySystemData* ds = getDescriptor(paySystem);
	return ds ? true : false;
}

bool PaySystemDescriptors::isPaySystemBitDefined( const char* paySystem )
{
	const PaySystemData* ds = getDescriptor(paySystem);
	return (ds && (ds->psBitNumber >= 0));
}

const char* PaySystemDescriptors::cardTypeToName( BYTE systemByte )
{
	const PaySystemData* ds = getDescriptor( systemByte );
	return ds ? ds->descrCashin : "";
}

const char* PaySystemDescriptors::cardTypeToCashoutName( BYTE systemByte )
{
	const PaySystemData* ds = getDescriptor( systemByte );
	return ds ? ds->descrCashout : "";
}

bool PaySystemDescriptors::isEncryptable( const char* paySystem )
{
	const PaySystemData* ds = getDescriptor(paySystem);
	return ds ? ds->encryptable : false;	
}

const PaySystemDescriptors::PaySystemData * PaySystemDescriptors::getPaySystemAt(int i)
{
	if (i>=0 && i < SZARR( table ))
		return  &table[i];
	// PYR-112209
	const int iExt = i - SZARR(table);
	if (iExt >= 0 && iExt < _paySystemDataTableExt.size())
	{
		auto it = _paySystemDataTableExt.begin();
		advance(it, iExt);
		return &(it->getPsData());
	}
	return 0;
}

//#13208 + #14114 + #17735 + #19485
bool PaySystemDescriptors::isVisaSubtypeBit(ePsBitNumber bitNumber) // #vasilyt consider to remove
{
	return (
		bitNumber == ePsBitNumberEntropay || 
		bitNumber == ePsBitNumberQiwi || 
		bitNumber == ePsBitNumberStarscard ||
//		bitNumber == ePsBitNumberPayLucky ||
		bitNumber == ePsBitNumberAlfabank );
}
bool PaySystemDescriptors::isVisaSubtype(const char* paysystem) // #vasilyt consider to remove
{ 
	return (
		PString::equals(paysystem, ENTROPAY_NAME) || 
		PString::equals(paysystem, QIWI_NAME) || 
		PString::equals(paysystem, STARSCARD_NAME) ||
//		PString::equals(paysystem, PAYLUCKY_NAME) ||
		PString::equals(paysystem, ALFABANK_NAME) );
}

bool PaySystemDescriptors::isMastercardSubtypeBit(ePsBitNumber bitNumber)
{
	return bitNumber == ePsBitNumberGamersCard;
}
bool PaySystemDescriptors::isMcSubtype(const char* paysystem)
{
	return PString::equals(paysystem, GAMERSCARD_NAME);
}

bool PaySystemDescriptors::needsSubtypeConversion(const char* cliPaysystem, PString& srvPaysystem /*out*/) // converts 'clientPaysytem' to 'serverPaysystem'
{
	if (isVisaSubtype(cliPaysystem))
	{
		srvPaysystem = VISA_NAME;
		return true;
	}
	if (isMcSubtype(cliPaysystem))
	{
		srvPaysystem = MASTERCARD_NAME;
		return true;
	}
	return false;
}

bool PaySystemDescriptors::doPaysystemAndSubtypeMatch(const char* paysystem, const char* productId)
{
	bool isGenericCard = (PString::equals(productId, GENERIC_CREDITCARD_NAME) || PString::equals(productId, GENERIC_DEBITCARD_NAME)) &&
		(PString::equals(paysystem, VISA_NAME) || PString::equals(paysystem, MASTERCARD_NAME) || PString::equals(paysystem, SOLO_NAME) || PString::equals(paysystem, RUPAY_NAME));

	return isGenericCard || (PString::equals(paysystem, VISA_NAME) && PaySystemDescriptors::isVisaSubtype(productId)) ||
		(PString::equals(paysystem, MASTERCARD_NAME) && isMcSubtype(productId));
}


bool PaySystemDescriptors::isAnyCreditCard(const char* paysystem) // moved here from cashierobject.cpp rev 1.2321
{
	return paysystem && *paysystem && (
		PString::equals(paysystem, VISA_NAME       ) ||
		PString::equals(paysystem, MIR_NAME       ) ||		
		PString::equals(paysystem, PAYLUCKY_NAME   ) ||
		PString::equals(paysystem, SIGHTLINE_NAME  ) ||
		isVisaSubtype  (paysystem) ||
		isMcSubtype(paysystem)     ||
		PString::equals(paysystem, MASTERCARD_NAME ) ||
		PString::equals(paysystem, DINERS_NAME     ) ||
		PString::equals(paysystem, SOLO_NAME       ) ||
		PString::equals(paysystem, RUPAY_NAME      ) ||
		PString::equals(paysystem, CARTE_BLEUE_NAME) );
}

/*
bool PaySystemDescriptors::isPhoneBased(const char* paysystem) // PYR-24538
{
	return (
		PString::equals(paysystem, VODAFONE_NAME) || 
		PString::equals(paysystem, O2_NAME      ) || 
		PString::equals(paysystem, TMOBILE_NAME ) ||
		PString::equals(paysystem, MOBILEPAYMENT_NAME)); // PYR-25409
}
*/

bool PaySystemDescriptors::isExternalPaysystem(const char* paysystem)
{
	return (paysystem && *paysystem && isExternalPaysystemBit(getPsBitNumber(paysystem)));
}

// REAL ps only; excludes: transfer, Conversion, FEE, Migration, Tax, Upsell, etc
bool PaySystemDescriptors::isExternalPaysystemBit(ePsBitNumber bitNumber)
{
	if (bitNumber < 0 || bitNumber > ePsBitNumberLast)
		return false;

	switch (bitNumber)
	{
		// tranfer/conversion
	case ePsBitNumberTransfer:
	case ePsBitNumberMcTransfer:
		// migration			
	case ePsBitNumberItConversion:
	case ePsBitNumberFrMigration:
	case ePsBitNumberBeMigration:
	case ePsBitNumberEsMigration:
	case ePsBitNumberPtMigration:
		// others
	case ePsBitNumberPaysystemFee:
	case ePsBitNumberTax:
	case ePsBitNumberUpsellAward:

	case ePsBitNumberFullTilt:
	case ePsBitNumberFullTiltAssetMigr:

		return false;

	default:
		return true;
	}
}

bool PaySystemDescriptors::isDepositable(const char* paysystem)
{
	return (paysystem && *paysystem && isDepositableBit(getPsBitNumber(paysystem)));
}

bool PaySystemDescriptors::isDepositableBit(ePsBitNumber bitNumber)
{
	if (bitNumber < 0 || bitNumber > ePsBitNumberLast)
		return false;

	if (!isExternalPaysystemBit(bitNumber))
		return false;

	switch (bitNumber)
	{
		// obsolete or cashout only
	case ePsBitNumberFirepay:
	case ePsBitNumberWUnion:
	case ePsBitNumberEPassporte:
	case ePsBitNumberMoneygram:
	case ePsBitNumberMgCashout:
	case ePsBitNumberMoneyOrder:
	case ePsBitNumberMRMC:
	case ePsBitNumberCitadel:
	case ePsBitNumberBankDirect:
		// cashout only
	case ePsBitNumberCheque:
	case ePsBitNumberWire:
//	case ePsBitNumberMoneta:
	case ePsBitNumberDirbank:		

		return false;

	default:
		return true;
	}
}

const char* PaySystemDescriptors::paySystemMaskToName(const PaysystemMask& paysystemMask) // only one bit is analysed
{
	ePsBitNumber bitNumber = paysystemMask.getOneRaisedBitNumber();
	if (bitNumber < 0)
		return "";
	// PYR-112209
	for( int i = 0; i < getPaySystemsSize(); i++ )
	{
		const PaySystemData* psData = getPaySystemAt(i);
		if( bitNumber == psData->psBitNumber )
			return psData->paySystem;
	}
	return "";
}

void PaySystemDescriptors::paySystemMaskToAllNames(const PaysystemMask& pMask, bool isForbiddenMask, vector<PString>& paysystems) // PYR-45523
{
	for (int i = 0; i < PaySystemDescriptors::getPaySystemsSize(); ++i)
	{
		const PaySystemDescriptors::PaySystemData* curr = PaySystemDescriptors::getPaySystemAt(i);
		if (!curr) // to pacify lint, should never happen
			continue;
		bool bitFound = pMask.isBitSet(curr->psBitNumber);
		if ((bitFound && !isForbiddenMask) || (!bitFound && isForbiddenMask) ) //-V728
		{
			paysystems.push_back(curr->paySystem);
		}
	} 
}

eResultInfoType PaySystemDescriptors::paysystemToResultInfoType(const char* paySystem)
{
	const PaySystemData* ds = getDescriptor(paySystem);
	return ds ? ds->resultInfoType : eResultInfoTypeUNKNOWN;
}

ePsBitNumber PaySystemDescriptors::getPsBitNumber( const char* paySystem )
{
	const PaySystemData* ds = getDescriptor(paySystem);
	return ds ? ds->psBitNumber : ePsBitNumberUNDEFINED;
}

void PaySystemDescriptors::processPaySystemsUpdate(CommMsgParser& parser)
{
	PStringSet staticPaySystemNames;
	for (const PaySystemDescriptors::PaySystemData& ps : table)
		staticPaySystemNames.insert(ps.paySystem);

	vector<_PaySystemDataExt> v;
	parseVectorOfParsableMessageBodies(v, parser);
	_paySystemDataTableExt.clear();
	PString logStr;
	for (const _PaySystemDataExt& psDataExt : v)
	{
		if (staticPaySystemNames.find(psDataExt.getPaySystem()) == staticPaySystemNames.end())
		{
			_paySystemDataTableExt.emplace_back(psDataExt);
			if (logStr.length() != 0)
				logStr.append(",");
			logStr.append(psDataExt.getPaySystem());
		}
	}
	PLog("processPaySystemsUpdate totalrecvd=%zu, static=%zu, dyn=%zu, dynps='%s'", v.size(), SZARR(table), _paySystemDataTableExt.size(), logStr.c_str());
}

void PaySystemDescriptors::composePaySystemData(CommMsgBody& body)
{
	vector<_PaySystemDataExt> v;
	for (int i = 0; i < getPaySystemsSize(); i++)
	{
		v.emplace_back(*getPaySystemAt(i));
	}
	PLog("composePaySystemData total=%zu, static=%zu, dyn=%zu", v.size(), SZARR(table), _paySystemDataTableExt.size());
	composeVectorOfMessageBodiesOfComposableValues(v, body);
}

void PaySystemDescriptors::serializeExtPaysystems(PStringMap<PString>& dest)
{
	PLog("PSDescrs::ser, size=%zu", _paySystemDataTableExt.size());
	dest.clear();
	for (const _PaySystemDataExt& psExt: _paySystemDataTableExt)
	{
		PString serializedPs;
		psExt.serializeToString(serializedPs);
		dest.insert(psExt.getPaySystem(), serializedPs);
	}
}

void PaySystemDescriptors::deserializeExtPaysystems(const vector<PString>& source)
{
	PLog("PSDescrs::deser, size=%zu", source.size());

	PStringSet staticPaySystemNames;
	for (const PaySystemDescriptors::PaySystemData& ps : table)
		staticPaySystemNames.insert(ps.paySystem);

	_paySystemDataTableExt.clear();
	PString logStr;
	for (const PString& serializedPs: source)
	{
		_PaySystemDataExt psDataExt;
		psDataExt.deserializeFromString(serializedPs);

		if (staticPaySystemNames.find(psDataExt.getPaySystem()) == staticPaySystemNames.end())
		{
			_paySystemDataTableExt.emplace_back(psDataExt);
			if (logStr.length() != 0)
				logStr.append(",");
			logStr.append(psDataExt.getPaySystem());
		}
	}
	PLog("deserializeExtPaysystems totalrecvd=%zu, static=%zu, dyn=%zu, dynps='%s'", source.size(), SZARR(table), _paySystemDataTableExt.size(), logStr.c_str());
}

bool PaysystemMask::isPsBitSet(const char* paysystem) const
{
	return isBitSet(PaySystemDescriptors::getPsBitNumber(paysystem));
}
void PaysystemMask::setPsBit(const char* paysystem)
{
	setBit(PaySystemDescriptors::getPsBitNumber(paysystem));
}
void PaysystemMask::clearPsBit(const char* paysystem)
{
	clearBit(PaySystemDescriptors::getPsBitNumber(paysystem));
}

const char* PaysystemMask::toHexString(PString& out) const
{
	out = "";
	char buf[32];
	const char* format;
	if (mask4)
	{
		p_hex64toa(mask4, buf);
		out.append(buf);
	}
	if (mask3 || out.length())
	{
		format = out.length() ? "%016llX" : "%llX";
		sprintf(buf, format, mask3);
		out.append(buf);
	}
	if (mask2 || out.length())
	{
		format = out.length() ? "%08X" : "%X";
		sprintf(buf, format, mask2);
		out.append(buf);
	}
	if (mask || out.length())
	{
		format = out.length() ? "%08X" : "%X";
		sprintf(buf, format, mask);
		out.append(buf);
	}
	if (out.length() == 0)
	{
		out = "0";
	}
	return out.c_str();
}

void PaysystemMask::fromHexString(const char* str)
{
	setZero();
	if (!str || !*str)
		return;
	if (PString::startsWith(str, "0x") || PString::startsWith(str, "0X")) // ignore the 0x prefix if exists
		str += 2;

	// find the end of the hex string
	const char *pEnd;
	for (pEnd = str; *pEnd; ++pEnd)
	{
		if (!isxdigit(*pEnd))
			break;
	}

	PString hexStr(str, pEnd - str);
	str = hexStr;
	if (!*str)
		return;

	int len = hexStr.length();
	if (len > 32)
	{
		mask4 = p_atohex64(PString(str, len-32));
		str += (len-32);
		len = 32;
	}
	if (len > 16)
	{
		mask3 = p_atohex64(PString(str, len-16));
		str += (len-16);
	}
	UINT64 low64 = p_atohex64(str);
	mask2 = UINT32(low64 >> 32);
	mask  = UINT32(low64);
}

const char* PaysystemMask::toDecString(PString& out) const
{
	out = "";
	char buf[32];
	if (mask4)
	{
		p_u64toa(mask4, buf);
		out.append(buf);
		out.append(decStrSeparator);
	}
	if (mask3 || out.length())
	{
		p_u64toa(mask3, buf);
		out.append(buf);
		out.append(decStrSeparator);
	}
	UINT64 low64 = getUInt64();
	if (low64 || out.length())
	{
		p_u64toa(low64, buf);
		out.append(buf);
	}
	if (out.length() == 0)
	{
		out = "0";
	}
	return out.c_str();
}

void PaysystemMask::fromDecString(const char* str)
{
	setZero();
	if (!str || !*str)
		return;

	// find the end of the dec string
	const char *pEnd;
	for (pEnd = str; *pEnd; ++pEnd)
	{
		if (!isdigit(*pEnd) && *pEnd != decStrSeparator)
			break;
	}

	PString decStr(str, pEnd - str);
	str = decStr;
	if (!*str)
		return;

	const char* pSeparator = strchr(str, decStrSeparator);
	if (pSeparator)
	{
		const char* pNextSeparator = strchr(pSeparator + 1, decStrSeparator);
		if (pNextSeparator)
		{
			mask4 = p_atou64(PString(str, pSeparator-str));
			str = pSeparator + 1;
			pSeparator = pNextSeparator;
		}
	}

	if (pSeparator)
	{
		mask3 = p_atou64(PString(str, pSeparator-str));
		str = pSeparator + 1;
	}
	UINT64 low64 = p_atou64(str);
	mask2 = UINT32(low64 >> 32);
	mask  = UINT32(low64);
}

ePsBitNumber PaysystemMask::getOneRaisedBitNumber() const
{
	if (mask)
	{
		for (int i = 0; i < 32; ++i)
		{
			if ((mask & (1U << i)) != 0)
				return (ePsBitNumber)(i);
		}
	}
	if (mask2)
	{
		for (int i = 0; i < 32; ++i)
		{
			if ((mask2 & (1U << i)) != 0)
				return (ePsBitNumber)(i + 32);
		}
	}
	if (mask3)
	{
		for (int i = 0; i < 64; ++i)
		{
			if ((mask3 & (ONE64 << i)) != 0)
				return (ePsBitNumber)(i + 64);
		}
	}
	if (mask4)
	{
		for (int i = 0; i < 64; ++i)
		{
			if ((mask4 & (ONE64 << i)) != 0)
				return (ePsBitNumber)(i + 128);
		}
	}
	return ePsBitNumberUNDEFINED;
}
