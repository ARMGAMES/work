#ifndef aux_hh_h_included
#define aux_hh_h_included

#ifndef CRM_REPLY_DELTA
#define CRM_REPLY_DELTA	1000
#endif

#define AUX_HH_MAX_HANDS_ALLOWED       500u

// PYR-61146
// Aux -> CrmServer
#define CRM_Q_AUX_HAND_HISTORY_REQUEST				2000 // 444sbssM(handHistoryRequest)M(metadataBody)44s4 - reqId, userIntId, licenseId, metadata, requestedByRamCli, password, userId, M(handHistoryRequest), M(metadataBody) locale, brand, handHistory, reportStatus
#define CRM_A_AUX_HAND_HISTORY_REQUEST				(CRM_Q_AUX_HAND_HISTORY_REQUEST + CRM_REPLY_DELTA) // 46[s] - reqId, errCode[errDescr]

namespace aux
{
	enum CriteriaHandSearchType
	{
		AUX_HH_BY_NUM_OF_HANDS = 1,
		AUX_HH_BY_TIME = 2,
		AUX_HH_BY_HANDID = 3
	};
};

#endif