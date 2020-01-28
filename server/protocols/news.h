/********************************************************************
*																	*
*				=========================							*
*				=	PYR Software LTD	=							*
*				=========================							*
*																	*
*																	*
*	Created:	10/01/2006											*
*	File Name: 	C:\WORK\PROTOCOLS\news.h							*
*	File path:	C:\WORK\PROTOCOLS									*
*	Author:		Benny Kritzer										*
*																	*
*	Description: Defines the protocol of the News Server			*
*********************************************************************/

/*****************************************************************

 1 - BYTE 
 2 - UINT16
 3 - nonDefined
 4 - UINT32
 5 - INT8
 6 - INT16
 7 - INT32

******************************************************************/

#ifndef news_h_included
#define news_h_included


//Error Codes
#define NEWS_OK						0x0000
#define NEWS_GENERAL_ERROR			0x0001


enum eNewsPriority
{
	eNewsPriorityLow		= 10,
	eNewsPriorityMedium		= 20,
	eNewsPriorityHigh		= 30,
	eNewsPrioritySuper		= 40,
};

#endif //news_h_included
