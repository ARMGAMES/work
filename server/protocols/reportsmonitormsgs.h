#ifndef reportsmonitormsgs_h_included
#define reportsmonitormsgs_h_included

#define MSG_STATUS_REPORT							1 // 4s4s2[s] - reqId,componentId,reportId,dataDescr,errCode,[errDescr]
#define MSG_STATUS_REPORT_REPLY						2 // 42[s] - reqId,errCode,[errDescr]

// Admin messages should be sent through AuthRedirector
#define MSG_ADMIN_STATUS_REPORT						3 // 4ss4s2[s] - reqId,adminComment,componentId,reportId,dataDescr,errCode,[errDescr]
#define MSG_ADMIN_STATUS_REPORT_REPLY				4 // 42[s] - reqId,errCode,[errDescr]

#define MSG_ADMIN_ADD_REPORT						5 // 4sss4s - reqId,adminComment,reportName,schedule,daysAgo,configuration
#define MSG_ADMIN_ADD_REPORT_REPLY					6 // 42[s|4] - reqId,errCode,[errDescr|reportId]

#define MSG_ADMIN_EDIT_REPORT						7 // 4s4ss4s - reqId,adminComment,reportId,reportName,schedule,daysAgo,configuration
#define MSG_ADMIN_EDIT_REPORT_REPLY					8 // 42[s] - reqId,errCode,[errDescr]

#define MSG_ADMIN_CHANGE_REPORT_STATUS				9 // 4s4b - reqId,adminComment,reportId,isActive
#define MSG_ADMIN_CHANGE_REPORT_STATUS_REPLY		10 // 42[s] - reqId,errCode,[errDescr]

#define REGULATORY_REPORT_NAME_LEN	(100+1)
#define ADMIN_COMMENT_LEN			(100+1)
#define REPORT_SCHEDULE_LEN			(500+1)
#define CONFIGURATION_LEN			(1000+1)
#define DESCRIPTION_LEN				(500+1)

enum eReportStatus
{
	eReportStatus_None						= 0,
	eReportStatus_Scheduled					= 1,	// report is not done yet and its deadline is in future
	eReportStatus_ReportDone				= 2,	// report done on time
	eReportStatus_ReportDoneLate			= 3,	// report done late
	eReportStatus_ComponentLate				= 10,	// report is still not late but one of its components is late
	eReportStatus_ReportLate				= 11,	// report is not done yet and its deadline is in the past
	eReportStatus_ComponentPotentiallyLate	= 12,	// report is still not late but one of its components is potentially late
	eReportStatus_ReportPotentiallyLate		= 13,	// report is not done yet and is potentially late
	eReportStatus_ComponentFailed			= 100,	// report is not done yet and one of its components failed
	eReportStatus_ReportFailed				= 101	// final report component failed
};

#endif // reportsmonitormsgs_h_included
