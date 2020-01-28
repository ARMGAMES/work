#ifndef _TOK822_H_INCLUDED_
#define _TOK822_H_INCLUDED_

/*++
* NAME
*	tok822 3h
* SUMMARY
*	RFC822 token structures
* SYNOPSIS
*	#include <tok822.h>
* DESCRIPTION
* .nf

*
* Internal address representation: a token tree.
*/
typedef struct TOK822 {
	int     type;			/* token value, see below */
	PString vstr;			/* token contents */
	struct TOK822 *prev;		/* peer */
	struct TOK822 *next;		/* peer */
	struct TOK822 *head;		/* group members */
	struct TOK822 *tail;		/* group members */
	struct TOK822 *owner;		/* group owner */
} TOK822;

/*
* Token values for multi-character objects. Single-character operators are
* represented by their own character value.
*/
#define TOK822_MINTOK	256
#define	TOK822_ATOM	256		/* non-special character sequence */
#define	TOK822_QSTRING	257		/* stuff between "", not nesting */
#define	TOK822_COMMENT	258		/* comment including (), may nest */
#define	TOK822_DOMLIT	259		/* stuff between [] not nesting */
#define	TOK822_ADDR	260		/* actually a token group */
#define TOK822_STARTGRP	261		/* start of named group */
#define	TOK822_COMMENT_TEXT 262		/* comment text */
#define TOK822_MAXTOK	262

typedef int (*TOK822_ACTION) (TOK822 *);

/*
* tok822_node.c
*/

#define TOK822_STR_NONE	(0)
#define TOK822_STR_WIPE	(1<<0)
#define TOK822_STR_TERM	(1<<1)
#define TOK822_STR_LINE	(1<<2)
#define TOK822_STR_ADDR (1<<3)
#define TOK822_STR_DEFL	(TOK822_STR_WIPE | TOK822_STR_TERM)
#define TOK822_STR_HEAD	(TOK822_STR_TERM | TOK822_STR_LINE)

/*
* tok822_find.c
*/

/*
* tok822_resolve.c
*/
//extern void tok822_resolve(TOK822 *, RESOLVE_REPLY *);

/* LICENSE
* .ad
* .fi
*	The Secure Mailer license must be distributed with this software.
* AUTHOR(S)
*	Wietse Venema
*	IBM T.J. Watson Research
*	P.O. Box 704
*	Yorktown Heights, NY 10598, USA
*--*/

#endif
