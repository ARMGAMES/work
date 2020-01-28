#ifndef edbm_h_included
#define edbm_h_included

enum DbmCropType
{
	typeNone,
	typeDeletedEmails,
	typeAttachments,
	typeAutoCashierEmails,
	typeAutoBouncedEmails,
	typeJunkEmails,
	typePrivateEmails,
	typeCommonEmails,
	typeConvertBodies,
	typeDeletePersonal,
	typeReinstate
};




#define edbm_ChangeOwner        1
#define edbm_ChangeState        2
#define edbm_ChangeFlag         3
#define edbm_DeleteEmail        4
#define edbm_AddNote	        5
#define edbm_DeleteAttachment   6

#define edbm_reqEmailsAll		0
#define edbm_reqAssingnedTo		1
#define edbm_reqStatus			2
#define edbm_reqEmailId			3
#define edbm_reqUserId			4
#define edbm_StatusAndAssigned	5
#define edbm_reqSetUser			6
#define edbm_reqSetTicketId 	7
#define edbm_reqTicketId		8
#define edbm_reqTime			9
#define edbm_reqIncomingType   10
#define edbm_reqEmailsIds	   11
#define edbm_reqEmailsAllByAddress 12

#define edbm_sortByTime         1
#define edbm_sortById           2
#define edbm_sortByTo           3
#define edbm_sortByFrom         4
#define edbm_sortBySubject      5
#define edbm_sortByUser         6
#define edbm_sortByOwner        7

#define edbm_reqSetOneEmail     0
#define edbm_reqSetAllEmail     1

//#define edbm_incomingType		1
//#define edbm_outgoingType		2
//#define edbm_draftType  		3

#define edbm_ReplyTo			3

#define edbm_StateNew           1
#define edbm_StateInProgress    2
#define edbm_StateRFU           3
#define edbm_StateClosed        4
#define edbm_StateSpam          5
#define edbm_StateDeleted       6
#define edbm_StateCMT           7
#define edbm_StateForSending    8
#define edbm_StateCorrupted     9
#define edbm_StateBounced      10
//#define edbm_StatePlayerCaseClosed 11

#define edbm_bitStateNew           1
#define edbm_bitStateInProgress    2
#define edbm_bitStateRFU           4
#define edbm_bitStateClosed        8
#define edbm_bitStateSpam         16
#define edbm_bitStateDeleted      32
#define edbm_bitStateCMT          64
#define edbm_bitStateForSending  128
#define edbm_bitStateCorrupted   256
#define edbm_bitStateBounced     512
//#define edbm_bitStatePlayerCaseClosed 1024
#define edbm_bitSearchAttachments  0x8000000
#define edbm_bitStateDefault edbm_bitStateNew|edbm_bitStateInProgress|edbm_bitStateRFU|edbm_bitStateClosed|edbm_bitStateCMT|edbm_bitStateForSending|edbm_bitStateCorrupted|edbm_bitStateBounced //|edbm_bitStatePlayerCaseClosed


#define edbm_emailReceived      1
#define edbm_emailSent          2
#define edbm_emailDraft         3
#define edbm_emailFrom          4
#define edbm_emailToOnly        6


#define edbm_emailUndefined			0
#define edbm_emailNew				1
#define edbm_emailRead				2
#define edbm_emailReplied			3
#define edbm_emailForwarded			4
#define edbm_emailSemiAutoReplied   5
#define edbm_emailAutoReplied		6


#define owner_haveToExist       1     // if not set record ignored
#define owner_allAccess         2	  // include in all selects (former AllSupport). Now not used.
//#define owner_includeIntoSelect 4   // not used
#define owner_automaticEmails     8	  // former 'onlyThis'. Set for owners like AutoCashiers. 
                                      // Excluded from search if 'No auto emails' checkbox checked.
#define owner_includeInView     0x10  // include in drop down box on the left bottom 
#define owner_canSignIn         0x20  // owner is valid CSR account which can log in
#define owner_assignByFromBox   0x40
#define owner_junkBox		    0x80
#define owner_assignByToAddress 0x100
#define owner_assignByFullAddress 0x200
#define owner_assignMask (owner_assignByFromBox | owner_assignByToAddress | owner_assignByFullAddress)
									  

#define edbm_reqSignInState     0
#define edbm_signInState        1
#define edbm_signOutState       2

#define edbm_emailNoSpecial				 0
#define edbm_emailSpecialTransform		 1
#define edbm_emailSpecialTransformFailed                 2
#define edbm_emailSpecialDadSubcase						 4
#define edbm_emailSpecialFrenchAuthSupportSubcase		 5
#define edbm_emailSpecialItalianAuthSupportSubcase		 6
#define edbm_emailSpecialUKAuthSupportSubcase			 7



#define emailUserUnknownMoney    0
#define emailUserRealMoney       1
#define emailUserPlayMoney       2

#define emailUserVIP_SN			 4
#define emailUserVIP_SNE		 8
#define emailUserVIP			16
#define emailAddressVerified    32
#define emailUserClubFounder    64


#define edbm_privateRightToWrite    2
#define edbm_privateRightToRead     1
#define edbm_privateNoRights        0

#define NEW_ATTACHMENT_ORDINAL 1000
#define NEW_BODY_ORDINAL 1000


#define EMAIL_CONTENTTYPE_LEN	101
#define EMAIL_ATTACH_LEN		257
#define EMAIL_CHARSET_LEN		101
#define EMAIL_FOLDER_NAME_LENGTH 101

#define ATTDOC_ATTACH_LEN		257


#define MAILBOX_PASSWORD_LEN   257
#define MAILBOX_LEN			    21
#define EDBM_ADDR_LEN          257
#define EDBM_ADDRESSBOOK_LEN   2000

#define DBM_Q_ONE_EMAIL_BODIES 2521
#define DBM_A_ONE_EMAIL_BODIES 2522
#define DBM_Q_ONE_EMAIL_ATTACH 2523
#define DBM_A_ONE_EMAIL_ATTACH 2524

#define DBM_Q_CREATE_NEW_FOLDER          2525
#define DBM_A_CREATE_NEW_FOLDER          2526
#define DBM_Q_CREATE_NEW_DOCUMENT        2527
#define DBM_A_CREATE_NEW_DOCUMENT        2528
#define DBM_Q_UPDATE_DOCUMENT_BODY       2529
#define DBM_A_UPDATE_DOCUMENT_BODY       2530
#define DBM_Q_UPDATE_DOCUMENT_KEYWORDS   2531
#define DBM_A_UPDATE_DOCUMENT_KEYWORDS   2532
#define DBM_Q_UPDATE_DOCUMENT_NAME       2533
#define DBM_A_UPDATE_DOCUMENT_NAME       2534
#define DBM_Q_DELETE_DOCUMENT            2535
#define DBM_A_DELETE_DOCUMENT            2536
#define DBM_Q_DELETE_FOLDER_CONTENT      2537
#define DBM_A_DELETE_FOLDER_CONTENT      2538
#define DBM_Q_GET_DOCUMENT_BODY          2539
#define DBM_A_GET_DOCUMENT_BODY          2540
#define DBM_Q_GET_DOCUMENT               2541
#define DBM_A_GET_DOCUMENT               2542
#define DBM_Q_GET_FOLDER                 2543
#define DBM_A_GET_FOLDER                 2544
#define DBM_Q_GET_TREE                   2545
#define DBM_A_GET_TREE                   2546
#define DBM_Q_FIND_BY_KEYWORDS           2547
#define DBM_A_FIND_BY_KEYWORDS           2548
#define DBM_Q_MOVE_DOCUMENT              2549
#define DBM_A_MOVE_DOCUMENT              2550
#define DBM_Q_COPY_DOCUMENT              2551
#define DBM_A_COPY_DOCUMENT              2552
#define EDBM_Q_ADMIN_RIGHTS               2553
#define EDBM_A_ADMIN_RIGHTS               2554

#define DBM_Q_UPDATE_EMAILS_OWNER		2555
#define DBM_A_UPDATE_EMAILS_OWNER		2556
#define DBM_Q_SEND_EMAIL				2557
#define DBM_A_SEND_EMAIL				2558
#define DBM_Q_UPDATE_EMAILS_STATE		2559
#define DBM_A_UPDATE_EMAILS_STATE		2560
#define DBM_Q_INSERT_EMAILS_NOTE		2561
#define DBM_A_INSERT_EMAILS_NOTE		2562
#define DBM_Q_DELETE_EMAIL				2563
#define DBM_A_DELETE_EMAIL				2564
#define DBM_Q_SAVE_DRAFT_EMAIL          2565
#define DBM_A_SAVE_DRAFT_EMAIL          2566
#define DBM_Q_ONE_EMAIL_SMTP            2567
#define DBM_A_ONE_EMAIL_SMTP            2668
#define DBM_A_ONE_EMAIL_SMTP_PRIVATE    2669



#define DBM_Q_UPDATE_EMAILS_STATE_ARRAY 2669
#define DBM_A_UPDATE_EMAILS_STATE_ARRAY 2670

#define DBM_Q_SIGN_IN					2671
#define DBM_A_SIGN_IN                   2672

#define DBM_Q_GET_ASSIGNED_EMAILS       2673
#define DBM_A_GET_ASSIGNED_EMAILS       2674

#define DBM_Q_UNASSIGN_EMAILADDRESS     2675 
#define DBM_A_UNASSIGN_EMAILADDRESS     2676 

#define DBM_Q_MARK_EMAILS_AS_ASSIGNED   2677
#define DBM_A_MARK_EMAILS_AS_ASSIGNED   2678

#define DBM_Q_ASSIGN_EMAILADDRESS       2679
#define DBM_A_ASSIGN_EMAILADDRESS       2680 

#define DBM_Q_WHO_SIGN_IN               2681
#define DBM_A_WHO_SIGN_IN               2682

#define DBM_Q_SAVE_PRIVATE_DRAFT_EMAIL  2683
#define DBM_A_SAVE_PRIVATE_DRAFT_EMAIL  2684

#define DBM_Q_PRIVATE_INCOMING_EMAILS   2685
#define DBM_A_PRIVATE_INCOMING_EMAILS   2686

#define DBM_Q_GET_PRIVATE_BOXES         2687
#define DBM_A_GET_PRIVATE_BOXES         2688

#define DBM_Q_VIEWOWNERS				2689
#define DBM_A_VIEWOWNERS		        2690

#define DBM_Q_REREAD_OWNERS				2691
#define DBM_A_REREAD_OWNERS		        2692

#define DBM_Q_REREAD_MAILBOXES			2693
#define DBM_A_REREAD_MAILBOXES	        2694

#define DBM_Q_GET_ADDRESSBOOK			2695
#define DBM_A_GET_ADDRESSBOOK	        2696

#define DBM_Q_INSERT_INTO_ADDRESSBOOK	2697
#define DBM_A_INSERT_INTO_ADDRESSBOOK   2698

#define DBM_Q_DELETE_FROM_ADDRESSBOOK	2699
#define DBM_A_DELETE_FROM_ADDRESSBOOK   2700

#define DBM_Q_GET_DEFAULTCONFIG			2701
#define DBM_A_GET_DEFAULTCONFIG			2702

#define DBM_Q_SET_DEFAULTCONFIG			2703
#define DBM_A_SET_DEFAULTCONFIG			2704

#define DBM_Q_UPDATE_ADDRESSBOOK	    2705
#define DBM_A_UPDATE_ADDRESSBOOK        2706

#define DBM_Q_UPDATE_EMAIL_FLAG  	    2707
#define DBM_A_UPDATE_EMAIL_FLAG         2708

#define DBM_Q_SET_AWAYMESSAGE    	    2709
#define DBM_A_SET_AWAYMESSAGE           2710

#define DBM_Q_GET_AUDIT                 2711
#define DBM_A_GET_AUDIT					2712

#define DBM_Q_DELETE_EMAIL_ATTACH		2713	
#define DBM_A_DELETE_EMAIL_ATTACH		2714


#define DBM_Q_GET_LISTOFDELETEDEMAILS	2715	
#define DBM_A_GET_LISTOFDELETEDEMAILS	2716


#define DBM_Q_GET_LISTOFATTACHMENTS		2717	
#define DBM_A_GET_LISTOFATTACHMENTS		2718

#define DBM_Q_GET_USER_INFO				2719	
#define DBM_A_GET_USER_INFO				2720

#define DBM_A_ADD_DOCUMENT_ATTACHMENT	2721	
#define DBM_Q_ADD_DOCUMENT_ATTACHMENT	2722

#define DBM_A_DELETE_DOCUMENT_ATTACHMENT	2723	
#define DBM_Q_DELETE_DOCUMENT_ATTACHMENT	2724

#define DBM_A_GET_DOCUMENT_ATTACHMENT	2725	
#define DBM_Q_GET_DOCUMENT_ATTACHMENT	2726

#define DBM_A_GET_ALL_DOCUMENT_ATTACHMENTS	2727	
#define DBM_Q_GET_ALL_DOCUMENT_ATTACHMENTS  2728

#define DBM_Q_USER_AUTH_AND_ADMIN_RIGHTS  2729
#define DBM_A_USER_AUTH_AND_ADMIN_RIGHTS  2730

#define DBM_Q_INSERT_EMAIL_FOLDER	2731
#define DBM_A_INSERT_EMAIL_FOLDER	2732

#define DBM_Q_DELETE_EMAIL_FOLDER	2733
#define DBM_A_DELETE_EMAIL_FOLDER	2734
	
#define DBM_Q_RENAME_EMAIL_FOLDER	2735
#define DBM_A_RENAME_EMAIL_FOLDER	2736

#define DBM_Q_MOVE_EMAIL_FOLDER		2737
#define DBM_A_MOVE_EMAIL_FOLDER		2738

#define DBM_Q_MOVE_EMAIL_INTO_FOLDER	2739
#define DBM_A_MOVE_EMAIL_INTO_FOLDER	2740

#define DBM_Q_GET_EMAIL_FOLDERS		2741
#define DBM_A_GET_EMAIL_FOLDERS		2742

#define DBM_Q_GET_DELETED_LIST_DAEMON	2743
#define DBM_A_GET_DELETED_LIST_DAEMON	2744

#define DBM_Q_GET_ATTACMENT_LIST_DAEMON	2745
#define DBM_A_GET_ATTACMENT_LIST_DAEMON	2746

#define DBM_Q_DELETE_EMAIL_DAEMON		2747
#define DBM_A_DELETE_EMAIL_DAEMON		2748

#define DBM_Q_DELETE_ATTACHMENT_DAEMON	2749
#define DBM_A_DELETE_ATTACHMENT_DAEMON	2750

#define EDBM_Q_GET_NEXT_CROPDATE	2751
#define EDBM_A_GET_NEXT_CROPDATE	2752

#define EDBM_Q_SAVE_LAST_CROPPED	2753
#define EDBM_A_SAVE_LAST_CROPPED	2754



#define DBM_Q_GET_ALL_OWNERS  2755
#define DBM_A_GET_ALL_OWNERS  2756


#define DBM_Q_GET_DEPARTMENTS  2757
#define DBM_A_GET_DEPARTMENTS  2758

#define DBM_Q_GET_POPMAILBOXES  2759
#define DBM_A_GET_POPMAILBOXES  2760

#define DBM_Q_GET_COMMON_ADDRESSBOOK  2761
#define DBM_A_GET_COMMON_ADDRESSBOOK  2762

#define DBM_Q_ADD_NEW_OWNER  2763
#define DBM_A_ADD_NEW_OWNER  2764

//#define DBM_Q_CHANGE_OWNER  2765
//#define DBM_A_CHANGE_OWNER  2766 

#define DBM_Q_DELETE_OWNER  2767
#define DBM_A_DELETE_OWNER  2768

#define DBM_Q_ADD_USER_INTO_DEPARTMENT  2769
#define DBM_A_ADD_USER_INTO_DEPARTMENT  2770

#define DBM_Q_DELETE_USER_FROM_DEPARTMENT  2771
#define DBM_A_DELETE_USER_FROM_DEPARTMENT  2772

#define DBM_Q_ADD_POPMAILBOX  2773
#define DBM_A_ADD_POPMAILBOX  2774

#define DBM_Q_DELETE_POPMAILBOX  2775
#define DBM_A_DELETE_POPMAILBOX  2776

#define DBM_Q_INSERT_INTO_COMMON_ADDRESSBOOK  2777
#define DBM_A_INSERT_INTO_COMMON_ADDRESSBOOK  2778

#define DBM_Q_DELETE_FROM_COMMON_ADDRESSBOOK  2779
#define DBM_A_DELETE_FROM_COMMON_ADDRESSBOOK  2780

#define DBM_Q_MOVE_IN_COMMON_ADDRESSBOOK  2781
#define DBM_A_MOVE_IN_COMMON_ADDRESSBOOK  2782


#define DBM_Q_GET_DATA_FOR_NEW_MAILBOX	 2783
#define DBM_A_GET_DATA_FOR_NEW_MAILBOX	 2784

#define DBM_Q_CREATE_NEW_EMAILADDRESS_IN_PRIVATE	 2785
#define DBM_A_CREATE_NEW_EMAILADDRESS_IN_PRIVATE	 2786

#define DBM_Q_CREATE_NEW_EMAILADDRESS_IN_COMMON		 2787
#define DBM_A_CREATE_NEW_EMAILADDRESS_IN_COMMON		 2788

#define DBM_Q_ROLLBACK_NEW_EMAILADDRESS_IN_COMMON	 2789
#define DBM_A_ROLLBACK_NEW_EMAILADDRESS_IN_COMMON	 2790

#define DBM_Q_ROLLBACK_NEW_EMAILADDRESS_IN_PRIVATE	 2791
#define DBM_A_ROLLBACK_NEW_EMAILADDRESS_IN_PRIVATE	 2792


#define DBM_Q_EDIT_DEPARTMENTS_IN_PRIVATE			 2793
#define DBM_A_EDIT_DEPARTMENTS_IN_PRIVATE			 2794

#define DBM_Q_EDIT_DEPARTMENTS						 2795
#define DBM_A_EDIT_DEPARTMENTS						 2796


#define DBM_Q_EDIT_ADDRESSBOOK_IN_PRIVATE			 2797
#define DBM_A_EDIT_ADDRESSBOOK_IN_PRIVATE			 2798

#define DBM_Q_EDIT_ADDRESSBOOK						 2799
#define DBM_A_EDIT_ADDRESSBOOK						 2800

#define DBM_Q_REOPEN_DATABASE						 0x56AB 
#define DBM_A_REOPEN_DATABASE						 0x56AC 

#define DBM_Q_REREAD_INIFILE						 2801 
#define DBM_A_REREAD_INIFILE						 2802 


#define DBM_Q_GET_ADMINS_FOR_MAILBOX				 2803 
#define DBM_A_GET_ADMINS_FOR_MAILBOX				 2804 

#define DBM_Q_GET_MAILBOXES_FOR_ADMIN				 2805 
#define DBM_A_GET_MAILBOXES_FOR_ADMIN				 2806 


#define DBM_Q_EDIT_ADMINRIGHTS						 2807 
#define DBM_A_EDIT_ADMINRIGHTS						 2808 


#define DBM_Q_GET_USER_INFO_BY_EMAILADDRESS			 2809
#define DBM_A_GET_USER_INFO_BY_EMAILADDRESS			 2810 


#define DBM_Q_CHECK_ALREADY_REPLIED_LANGUAGE_EMAIL	 2811
#define DBM_A_CHECK_ALREADY_REPLIED_LANGUAGE_EMAIL	 2812 

#define DBM_Q_CHECK_ALREADY_REPLIED_AUTOREPLY_EMAIL	 2813
#define DBM_A_CHECK_ALREADY_REPLIED_AUTOREPLY_EMAIL	 2814 

#define DBM_Q_COMPOSITE								 3000
#define DBM_A_COMPOSITE								 3001 

#define DBM_Q_ADD_ALREADY_REPLIED_AUTOREPLY_EMAIL	 2815
#define DBM_A_ADD_ALREADY_REPLIED_AUTOREPLY_EMAIL	 2816 

#define DBM_Q_REQUEST_NEXT_EMAILID					 2817
#define DBM_A_REQUEST_NEXT_EMAILID					 2818 

#define DBM_Q_GET_INIT_DATA							 2819
#define DBM_A_GET_INIT_DATA							 2820 

#define DBM_Q_EMAIL_STORE							 2821
#define DBM_A_EMAIL_STORE							 2822 

#define DBM_Q_ASSIGN_EMAIL_TO_OWNER					 2823
#define DBM_A_ASSIGN_EMAIL_TO_OWNER					 2824 

#define DBM_Q_GET_USER_INFO_BY_USERID				 2825
#define DBM_A_GET_USER_INFO_BY_USERID				 2826

#define DBM_Q_CHECK_TRANSACTION_ID					 2827
#define DBM_A_CHECK_TRANSACTION_ID					 2828

#define DBM_Q_GET_ONE_EMAIL_FOR_SENDING				 2829
#define DBM_A_GET_ONE_EMAIL_FOR_SENDING				 2830

#define DBM_Q_UPDATE_EMAILS_STATE_PROXY				 2831
#define DBM_A_UPDATE_EMAILS_STATE_PROXY				 2832

#define DBM_Q_UPDATE_NOTES_EMAILID					 2833
#define DBM_A_UPDATE_NOTES_EMAILID					 2834

#define DBM_Q_ADD_ADMINRIGHTS						 2835
#define DBM_A_ADD_ADMINRIGHTS					     2836

#define DBM_Q_DELETE_ADMINRIGHTS					 2837
#define DBM_A_DELETE_ADMINRIGHTS				     2838


#define DBM_Q_REMOVE_FROM_OWNER_LIST				 2839
#define DBM_A_REMOVE_FROM_OWNER_LIST			     2840

#define DBM_Q_UNASSIGNED_EMAIL_ADDRESS				 2841
#define DBM_A_UNASSIGNED_EMAIL_ADDRESS			     2842

#define DBM_Q_ASSIGN_OTHER_EMAILS_TO_OWNER			 2843
#define DBM_A_ASSIGN_OTHER_EMAILS_TO_OWNER		     2844

#define DBM_Q_GET_DATA_FOR_OWNER_UPDATE				 2845
#define DBM_A_GET_DATA_FOR_OWNER_UPDATE			     2846

#define DBM_Q_ADD_AUDIT								 2847
#define DBM_A_ADD_AUDIT								 2848

#define DBM_Q_REASSIGN_EMAILADDRESS_IN_OWNER_LIST	 2849
#define DBM_A_REASSIGN_EMAILADDRESS_IN_OWNER_LIST    2850

#define DBM_Q_SIGNIN_REQUEST						 2851
#define DBM_A_SIGNIN_REQUEST						 2852

#define DBM_Q_SIGNOUT_REQUEST						 2853
#define DBM_A_SIGNOUT_REQUEST						 2854

#define DBM_Q_UPDATE_EMAIL_OWNER					 2855
#define DBM_A_UPDATE_EMAIL_OWNER					 2856

#define DBM_Q_UPDATE_EMAILS_STATE_ADMIN				 2857
#define DBM_A_UPDATE_EMAILS_STATE_ADMIN				 2858

#define DBM_Q_GET_STAT								 2859
#define DBM_A_GET_STAT								 2860

#define DBM_Q_DAD_SUBCASE							 2861
#define DBM_A_DAD_SUBCASE							 2862

#define DBM_Q_UPDATE_DADEMAILS_STATE				 2863
#define DBM_A_UPDATE_DADEMAILS_STATE				 2864

#define DBM_Q_UPDATE_EMAILS_OWNER_AND_AUTOREPLY		 2865
#define DBM_A_UPDATE_EMAILS_OWNER_AND_AUTOREPLY		 2866

#define DBM_Q_GET_ONE_EMAIL_GENERIC					 2867
#define DBM_A_GET_ONE_EMAIL_GENERIC					 2868

#define DBM_Q_SET_CHANGEEMAILADDRESS_ENABLED		 2869
#define DBM_A_SET_CHANGEEMAILADDRESS_ENABLED		 2870

#define DBM_Q_SET_FROMEMAILADDRESS					 2871
#define DBM_A_SET_FROMEMAILADDRESS					 2872

#define DBM_Q_GET_FROMEMAILADDRESS					 2873
#define DBM_A_GET_FROMEMAILADDRESS					 2874

#define DBM_Q_CDMS_MAILLIST_BY_ID					 2875
#define DBM_A_CDMS_MAILLIST_BY_ID					 2876

#define DBM_Q_GET_ONE_EMAIL_CDMS					 2879
#define DBM_A_GET_ONE_EMAIL_CDMS					 2880

#define DBM_Q_UNHAPPYREPLY							 2881
#define DBM_A_UNHAPPYREPLY							 2882

#define DBM_Q_CDMS_MAILLIST							 2883
#define DBM_A_CDMS_MAILLIST							 2884

#define DBM_Q_GET_ONE_EMAIL_CDMS_STATUS_UPDATE		 2885
#define DBM_A_GET_ONE_EMAIL_CDMS_STATUS_UPDATE		 2886


#define DBM_Q_UPDATE_OWNER							 2887
#define DBM_A_UPDATE_OWNER							 2888

#define DBM_Q_CREATE_NEW_COMMONEMAILADDRESS_IN_PRIVATE		2889
#define DBM_A_CREATE_NEW_COMMONEMAILADDRESS_IN_PRIVATE		2890

#define DBM_Q_CREATE_NEW_COMMONEMAILADDRESS_IN_COMMON		2891
#define DBM_A_CREATE_NEW_COMMONEMAILADDRESS_IN_COMMON		2892

#define DBM_Q_ROLLBACK_NEW_COMMONEMAILADDRESS_IN_PRIVATE	2893
#define DBM_A_ROLLBACK_NEW_COMMONEMAILADDRESS_IN_PRIVATE	2894

#define DBM_Q_ROLLBACK_NEW_COMMONEMAILADDRESS_IN_COMMON		2895
#define DBM_A_ROLLBACK_NEW_COMMONEMAILADDRESS_IN_COMMON		2896

#define DBM_Q_GET_DATA_FOR_NEW_COMMON_MAILBOX				2897
#define DBM_A_GET_DATA_FOR_NEW_COMMON_MAILBOX				2898

#define DBM_Q_GET_HIERARCHY_POPMAILBOXES					2899
#define DBM_A_GET_HIERARCHY_POPMAILBOXES					2900

#define DBM_Q_GET_HIERARCHY_ADMINS_FOR_MAILBOX				2901
#define DBM_A_GET_HIERARCHY_ADMINS_FOR_MAILBOX				2902

#define DBM_Q_GET_HIERARCHY									2903
#define DBM_A_GET_HIERARCHY									2904

#define DBM_Q_EDIT_HIERARCHY_ADMINRIGHTS					2905
#define DBM_A_EDIT_HIERARCHY_ADMINRIGHTS					2906

#define DBM_Q_GET_SHARED_POPMAILBOXES						2907
#define DBM_A_GET_SHARED_POPMAILBOXES						2908

#define DBM_Q_GET_ADMINS_FOR_SHARED_MAILBOX					2909
#define DBM_A_GET_ADMINS_FOR_SHARED_MAILBOX					2910


#define DBM_Q_EDIT_ADMINRIGHTS_FOR_SHARED					2911
#define DBM_A_EDIT_ADMINRIGHTS_FOR_SHARED					2912

#define DBM_Q_CHECK_ALREADY_REPLIED_CHAT_EMAIL				2913
#define DBM_A_CHECK_ALREADY_REPLIED_CHAT_EMAIL				2914

#define DBM_Q_ONE_EMAIL_BODIES_NEW						    2915
#define DBM_A_ONE_EMAIL_BODIES_NEW							2916

#define DBM_Q_REQUEST_NEXT_ATTACHMENTID						2917
#define DBM_A_REQUEST_NEXT_ATTACHMENTID						2918 

#define DBM_Q_UPDATE_EMAILS_OWNER_EX						2919
#define DBM_A_UPDATE_EMAILS_OWNER_EX						2920

#define DBM_Q_REQUEST_NEXT_BODYID							2921
#define DBM_A_REQUEST_NEXT_BODYID							2922 

#define DBM_Q_SEND_COMMON_EMAIL								2923
#define DBM_A_SEND_COMMON_EMAIL								2924 

#define DBM_Q_STORE_DRAFT_EMAIL								2925
#define DBM_A_STORE_DRAFT_EMAIL								2926 

#define DBM_Q_DELETE_GROUP_FROM_COMMON_ADDRESSBOOK			2927
#define DBM_A_DELETE_GROUP_FROM_COMMON_ADDRESSBOOK			2928

#define DBM_Q_CTS_SUBCASE									2929
#define DBM_A_CTS_SUBCASE									2930

#define DBM_Q_UPDATE_CTSEMAILS_STATE					    2931
#define DBM_A_UPDATE_CTSEMAILS_STATE				        2932

#define DBM_Q_GET_LIST_OLD_EMAILS_DAEMON					2933
#define DBM_A_GET_LIST_OLD_EMAILS_DAEMON					2934

#define DBM_Q_MOVE_OLD_EMAILS_DAEMON						2935
#define DBM_A_MOVE_OLD_EMAILS_DAEMON						2936

#define DBM_Q_GET_ONE_EMAIL_CESAR_CDMS						2937	// PYR-25511
#define DBM_A_GET_ONE_EMAIL_CESAR_CDMS						2938

#define DBM_Q_GET_COUNTRY_LIMITS							2937
#define DBM_A_GET_COUNTRY_LIMITS							2938

// PYR-28638
#define DBM_Q_GET_ATTACHMENTS_CESAR_CDMS					2939	//444 - reqId, CaseId, DocId, typeId
#define DBM_A_GET_ATTACHMENTS_CESAR_CDMS					2940	//46[s|ss4<sPs>] - reqId, errCode [errString | userId emailAddr numAttachs<contenType,PBlock,name>]

#define DBM_Q_GET_POPMAILBOXES_WITH_ADMIN  2941
#define DBM_A_GET_POPMAILBOXES_WITH_ADMIN  2942

#define DBM_Q_REINSTATE_EMAILS_DAEMON						2943
#define DBM_A_REINSTATE_EMAILS_DAEMON						2944

#define DBM_Q_GET_LIST_REINSTATE_EMAILS_DAEMON				2945
#define DBM_A_GET_LIST_REINSTATE_EMAILS_DAEMON				2946


//   From EDBM to DAD system

#define DAD_Q_SUBCASE_CLOSED						 4000
#define DAD_A_SUBCASE_CLOSED						 4001
#define CTS_Q_SUBCASE_CLOSED						 DAD_Q_SUBCASE_CLOSED
#define CTS_A_SUBCASE_CLOSED						 DAD_A_SUBCASE_CLOSED


#define DAD_Q_NEW_EMAIL								 4002
#define DAD_A_NEW_EMAIL								 4003
#define CTS_Q_NEW_EMAIL								 DAD_Q_NEW_EMAIL
#define CTS_A_NEW_EMAIL								 DAD_A_NEW_EMAIL

#define CESAR_Q_NEW_EMAIL							 5000
#define CESAR_A_NEW_EMAIL							 5001


#endif