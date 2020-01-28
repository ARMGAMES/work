/*++
* NAME
*	tok822_parse 3
* SUMMARY
*	RFC 822 address parser
* SYNOPSIS
*	#include <tok822.h>
*
*	TOK822 *tok822_scan(str, tailp)
*	const char *str;
*	TOK822	**tailp;
*
*	TOK822	*tok822_parse(str)
*	const char *str;
*
*	TOK822	*tok822_scan_addr(str)
*	const char *str;
*
*	PString	*tok822_externalize(buffer, tree, flags)
*	PString	*buffer;
*	TOK822	*tree;
*	int	flags;
*
*	PString	*tok822_internalize(buffer, tree, flags)
*	PString	*buffer;
*	TOK822	*tree;
*	int	flags;
* DESCRIPTION
*	This module converts address lists between string form and parse
*	tree formats. The string form can appear in two different ways:
*	external (or quoted) form, as used in message headers, and internal
*	(unquoted) form, as used internally by the mail software.
*	Although RFC 822 expects 7-bit data, these routines pay no
*	special attention to 8-bit characters.
*
*	tok822_scan() converts the external-form string in \fIstr\fR
*	to a linear token list. The \fItailp\fR argument is a null pointer
*	or receives the pointer value of the last result list element.
*
*	tok822_parse() converts the external-form address list in
*	\fIstr\fR to the corresponding token tree. The parser is permissive
*	and will not throw away information that it does not understand.
*	The parser adds missing commas between addresses.
*
*	tok822_scan_addr() converts the external-form string in
*	\fIstr\fR to an address token tree. This is just string to
*	token list conversion; no parsing is done. This routine is
*	suitable for data should contain just one address and no
*	other information.
*
*	tok822_externalize() converts a token list to external form.
*	Where appropriate, characters and strings are quoted and white
*	space is inserted. The \fIflags\fR argument is the binary OR of
*	zero or more of the following:
* .IP TOK822_STR_WIPE
*	Initially, truncate the result to zero length.
* .IP TOK822_STR_TERM
*	Append a null terminator to the result when done.
* .IP TOK822_STR_LINE
*	Append a line break after each comma token, instead of appending
*	whitespace.  It is up to the caller to concatenate short lines to
*	produce longer ones.
* .PP
*	The macro TOK_822_NONE expresses that none of the above features
*	should be activated.
*
*	The macro TOK822_STR_DEFL combines the TOK822_STR_WIPE and
*	TOK822_STR_TERM flags. This is useful for most token to string
*	conversions.
*
*	The macro TOK822_STR_HEAD combines the TOK822_STR_TERM and
*	TOK822_STR_LINE flags. This is useful for the special case of
*	token to mail header conversion.
*
*	tok822_internalize() converts a token list to string form,
*	without quoting. White space is inserted where appropriate.
*	The \fIflags\fR argument is as with tok822_externalize().
* STANDARDS
* .ad
* .fi
*	RFC 822 (ARPA Internet Text Messages). In addition to this standard
*	this module implements additional operators such as % and !. These
*	are needed because the real world is not all RFC 822. Also, the ':'
*	operator is allowed to appear inside addresses, to accommodate DECnet.
*	In addition, 8-bit data is not given special treatment.
* LICENSE
* .ad
* .fi
*	The Secure Mailer license must be distributed with this software.
* AUTHOR(S)
*	Wietse Venema
*	IBM T.J. Watson Research
*	P.O. Box 704
*	Yorktown Heights, NY 10598, USA
*--*/

/* System library. */

#include "ppinclude.h"
#include "ppstring.h"
#include "commtok822.h"

#include "tok822.h"


static TOK822 *tok822_alloc(int, const char *);
static TOK822 *tok822_free(TOK822 *);
static TOK822 *tok822_append(TOK822 *, TOK822 *);
static TOK822 *tok822_prepend(TOK822 *, TOK822 *);
static TOK822 *tok822_cut_before(TOK822 *);
static TOK822 *tok822_cut_after(TOK822 *);
static TOK822 *tok822_unlink(TOK822 *);
static TOK822 *tok822_sub_append(TOK822 *, TOK822 *);
static TOK822 *tok822_sub_prepend(TOK822 *, TOK822 *);
static TOK822 *tok822_sub_keep_before(TOK822 *, TOK822 *);
static TOK822 *tok822_sub_keep_after(TOK822 *, TOK822 *);
static TOK822 *tok822_free_tree(TOK822 *);
static int tok822_apply(TOK822 *, int, TOK822_ACTION);
static TOK822 **tok822_grep(TOK822 *, int);
static TOK822 *tok822_scan(const char *, TOK822 **);
static TOK822 *tok822_scan_addr(const char *);
static TOK822 *tok822_parse(const char *);
static TOK822 *tok822_normalize(PString &, PString &, TOK822 *, int);
static TOK822 *tok822_find_type(TOK822 *, int);
static TOK822 *tok822_rfind_type(TOK822 *, int);
static TOK822 *tok822_rewrite(TOK822 *, const char *);


/*
* I suppose this is my favorite macro. Used heavily for tokenizing.
*/

#define ISSPACE(a) isspace(a)
#define ISCNTRL(a) iscntrl(a)

#define COLLECT(t,s,c,cond) { \
	while ((c = *(unsigned char *) s) != 0) { \
	if (c == '\\') { \
	if ((c = *(unsigned char *)++s) == 0) \
	break; \
	} else if (!(cond)) { \
	break; \
	} \
	t->vstr.append(ISSPACE(c) ? ' ' : (char)c); \
	s++; \
	} \
}

#define COLLECT_SKIP_LAST(t,s,c,cond) { COLLECT(t,s,c,cond); if (*s) s++; }

/*
* Not quite as complex. The parser depends heavily on it.
*/
#define SKIP(tp, cond) { \
	while (tp->type && (cond)) \
	tp = tp->prev; \
}

#define MOVE_COMMENT_AND_CONTINUE(tp, right) { \
	TOK822 *prev = tok822_unlink(tp); \
	right = tok822_prepend(right, tp); \
	tp = prev; \
	continue; \
}

#define SKIP_MOVE_COMMENT(tp, cond, right) { \
	while (tp->type && (cond)) { \
	if (tp->type == TOK822_COMMENT) \
	MOVE_COMMENT_AND_CONTINUE(tp, right); \
	tp = tp->prev; \
	} \
}

/*
* Single-character operators. We include the % and ! operators because not
* all the world is RFC822. XXX Make this operator list configurable when we
* have a real rewriting language.
*/
static const char tok822_opchar[] = "|\"(),.:;<>@[]%!";

static void tok822_quote_atom(TOK822 *);
static const char *tok822_comment(TOK822 *, const char *);
static TOK822 *tok822_group(int, TOK822 *, TOK822 *, int);
static void tok822_copy_quoted(PString &, const char *, char *);
static int tok822_append_space(TOK822 *);

#define DO_WORD		(1<<0)		/* finding a word is ok here */
#define DO_GROUP	(1<<1)		/* doing an address group */

#define ADD_COMMA	','		/* resynchronize */
#define NO_MISSING_COMMA 0

TOK822 *tok822_alloc(int type, const char *strval)
{
	TOK822 *tp;

#define CONTAINER_TOKEN(x) \
	((x) == TOK822_ADDR || (x) == TOK822_COMMENT || (x) == TOK822_STARTGRP)

	tp = new TOK822();
	tp->type = type;
	tp->next = tp->prev = tp->head = tp->tail = tp->owner = 0;
	tp->vstr = (type < TOK822_MINTOK || CONTAINER_TOKEN(type) ? 0 :
		strval == 0 ? "" :
		strval);
	return (tp);
}

/* tok822_free - destroy token */

static TOK822 *tok822_free(TOK822 *tp)
{
	delete tp;
	return (0);
}

static TOK822 *tok822_append(TOK822 *t1, TOK822 *t2)
{
	TOK822 *next = t1->next;

	t1->next = t2;
	t2->prev = t1;

	t2->owner = t1->owner;
	while (t2->next)
		(t2 = t2->next)->owner = t1->owner;

	t2->next = next;
	if (next)
		next->prev = t2;
	return (t2);
}

/* tok822_prepend - insert token list, return end of inserted list */

static TOK822 *tok822_prepend(TOK822 *t1, TOK822 *t2)
{
	TOK822 *prev = t1->prev;

	if (prev)
		prev->next = t2;
	t2->prev = prev;

	t2->owner = t1->owner;
	while (t2->next)
		(t2 = t2->next)->owner = t1->owner;

	t2->next = t1;
	t1->prev = t2;
	return (t2);
}

/* tok822_cut_before - split list before token, return predecessor token */

static TOK822 *tok822_cut_before(TOK822 *tp)
{
	TOK822 *prev = tp->prev;

	if (prev) {
		prev->next = 0;
		tp->prev = 0;
	}
	return (prev);
}

/* tok822_cut_after - split list after token, return successor token */

static TOK822 *tok822_cut_after(TOK822 *tp)
{
	TOK822 *next = tp->next;

	if (next) {
		next->prev = 0;
		tp->next = 0;
	}
	return (next);
}

/* tok822_unlink - take token away from list, return predecessor token */

static TOK822 *tok822_unlink(TOK822 *tp)
{
	TOK822 *prev = tp->prev;
	TOK822 *next = tp->next;

	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	tp->prev = tp->next = 0;
	return (prev);
}

/* tok822_sub_append - append sublist, return end of appended list */

static TOK822 *tok822_sub_append(TOK822 *t1, TOK822 *t2)
{
	if (t1->head) {
		return (t1->tail = tok822_append(t1->tail, t2));
	} else {
		t1->head = t2;
		while (t2->next)
			(t2 = t2->next)->owner = t1;
		return (t1->tail = t2);
	}
}

/* tok822_sub_prepend - prepend sublist, return end of prepended list */

static TOK822 *tok822_sub_prepend(TOK822 *t1, TOK822 *t2)
{
	TOK822 *tp;

	if (t1->head) {
		tp = tok822_prepend(t1->head, t2);
		t1->head = t2;
		return (tp);
	} else {
		t1->head = t2;
		while (t2->next)
			(t2 = t2->next)->owner = t1;
		return (t1->tail = t2);
	}
}

/* tok822_sub_keep_before - cut sublist, return tail of disconnected list */

static TOK822 *tok822_sub_keep_before(TOK822 *t1, TOK822 *t2)
{
	TOK822 *tail = t1->tail;

	if ((t1->tail = tok822_cut_before(t2)) == 0)
		t1->head = 0;
	return (tail);
}

/* tok822_sub_keep_after - cut sublist, return head of disconnected list */

static TOK822 *tok822_sub_keep_after(TOK822 *t1, TOK822 *t2)
{
	TOK822 *head = t1->head;

	if ((t1->head = tok822_cut_after(t2)) == 0)
		t1->tail = 0;
	return (head);
}

/* tok822_free_tree - destroy token tree */

static TOK822 *tok822_free_tree(TOK822 *tp)
{
	if (tp) {
		if (tp->next)
			tok822_free_tree(tp->next);
		if (tp->head)
			tok822_free_tree(tp->head);
		tok822_free(tp);
	}
	return (0);
}

/* tok822_apply - apply action to specified tokens */

static int tok822_apply(TOK822 *tree, int type, TOK822_ACTION action)
{
	TOK822 *tp;
	int     result = 0;

	for (tp = tree; tp; tp = tp->next) {
		if (type == 0 || tp->type == type)
			if ((result = action(tp)) != 0)
				break;
	}
	return (result);
}

/* tok822_grep - list matching tokens */

static TOK822 **tok822_grep(TOK822 *tree, int type)
{
	TOK822 **list_;
	TOK822 *tp;
	int     count;

	for (count = 0, tp = tree; tp; tp = tp->next)
		if (type == 0 || tp->type == type)
			count++;

	list_ = (TOK822 **) malloc(sizeof(*list_) * (count + 1));
    PASSERT3(list_);

	for (count = 0, tp = tree; tp; tp = tp->next)
		if (type == 0 || tp->type == type)
			list_[count++] = tp;

	list_[count] = 0;
	return (list_);
}



static TOK822 * tok822_normalize(PString &vp, PString & vpn, TOK822 *tree, int flags)
{
	TOK822 *tp;

	if (flags & TOK822_STR_WIPE)
	{
		vp.assign("");
		vpn.assign("");
	}

	for (tp = tree; tp; tp = tp->next) {
		switch (tp->type) {
	case '<':
	case '>':
		vp.append((char)tp->type);
		break;
	case ',':
	case ';':
		tp=tp->next;
		return tp;

	case TOK822_ADDR:
		tok822_normalize(vp, vpn, tp->head, TOK822_STR_NONE|TOK822_STR_ADDR);
		break;
	case TOK822_ATOM:
		if (flags & TOK822_STR_ADDR)
			vpn.append(tp->vstr);
		vp.append(tp->vstr);
		break;
	case TOK822_COMMENT:
		//	    vp.append('(');
		//	    tok822_externalize(vp, tp->head, TOK822_STR_NONE);
		//	    vp.append(')');
		break;
	case TOK822_COMMENT_TEXT:
		//	    tok822_copy_quoted(vp, tp->vstr.c_str(), "()\\");
		break;
	case TOK822_QSTRING:
		if (flags & TOK822_STR_ADDR)
		{
			vpn.append('"');
			tok822_copy_quoted(vpn, tp->vstr.c_str(), "\"\\\r\n");
			vpn.append('"');
		}
		vp.append('"');
		tok822_copy_quoted(vp, tp->vstr.c_str(), "\"\\\r\n");
		vp.append('"');

		break;
	case TOK822_DOMLIT:
		vp.append('[');
		tok822_copy_quoted(vp, tp->vstr.c_str(), "\\\r\n");
		vp.append(']');
		break;
	case TOK822_STARTGRP:
		//	    vp.append(':');
		break;

	default:
		if (flags & TOK822_STR_ADDR)
		{
			vpn.append((char)tp->type);
		}
		vp.append((char)tp->type);
		break;
		}
		if (tok822_append_space(tp))
			vp.append(' ');

	}
	return (tp);
}





/* tok822_copy_quoted - copy a string while quoting */

static void tok822_copy_quoted(PString &vp, const char *str, char *quote_set)
{
	int     ch;

	while ((ch = *(unsigned char *) str++) != 0) {
		if (strchr(quote_set, ch))
			vp.append('\\');
		vp.append((char)ch);
	}
}

/* tok822_append_space - see if space is needed after this token */

static int tok822_append_space(TOK822 *tp)
{
	TOK822 *next;

	if (tp == 0 || (next = tp->next) == 0 || tp->owner != 0)
		return (0);
	if (tp->type == ',' || tp->type == TOK822_STARTGRP || next->type == '<')
		return (1);

#define NON_OPERATOR(x) \
	(x->type == TOK822_ATOM || x->type == TOK822_QSTRING \
	|| x->type == TOK822_COMMENT || x->type == TOK822_DOMLIT \
	|| x->type == TOK822_ADDR)

	return (NON_OPERATOR(tp) && NON_OPERATOR(next));
}

/* tok822_scan - tokenize string */

static TOK822 *tok822_scan(const char *str, TOK822 **tailp)
{
	TOK822 *head = 0;
	TOK822 *tail = 0;
	TOK822 *tp;
	int     ch;

	while ((ch = *(unsigned char *) str++) != 0) {
		if (ISSPACE(ch))
			continue;
		if (ch == '(') {
			tp = tok822_alloc(TOK822_COMMENT, (char *) 0);
			str = tok822_comment(tp, str);
		} else if (ch == '[') {
			tp = tok822_alloc(TOK822_DOMLIT, (char *) 0);
			COLLECT_SKIP_LAST(tp, str, ch, ch != ']');
		} else if (ch == '"') {
			tp = tok822_alloc(TOK822_QSTRING, (char *) 0);
			COLLECT_SKIP_LAST(tp, str, ch, ch != '"');
		} else if (ch != '\\' && strchr(tok822_opchar, ch)) {
			tp = tok822_alloc(ch, (char *) 0);
		} else {
			tp = tok822_alloc(TOK822_ATOM, (char *) 0);
			str -= 1;				/* \ may be first */
			COLLECT(tp, str, ch, !ISSPACE(ch) && !strchr(tok822_opchar, ch));
			tok822_quote_atom(tp);
		}
		if (head == 0) {
			head = tail = tp;
			while (tail->next)
				tail = tail->next;
		} else {
			tail = tok822_append(tail, tp);
		}
	}
	if (tailp)
		*tailp = tail;
	return (head);
}

/* tok822_parse - translate external string to token tree */

static TOK822 *tok822_parse(const char *str)
{
	TOK822 *head;
	TOK822 *tail;
	TOK822 *right;
	TOK822 *first_token;
	TOK822 *last_token;
	TOK822 *tp;
	int     state;

	/*
	* First, tokenize the string, from left to right. We are not allowed to
	* throw away any information that we do not understand. With a flat
	* token list that contains all tokens, we can always convert back to
	* string form.
	*/
	if ((first_token = tok822_scan(str, &last_token)) == 0)
		return (0);

	/*
	* For convenience, sandwich the token list between two sentinel tokens.
	*/
#define GLUE(left,rite) { left->next = rite; rite->prev = left; }

	head = tok822_alloc(0, (char *) 0);
	GLUE(head, first_token);
	tail = tok822_alloc(0, (char *) 0);
	GLUE(last_token, tail);

	/*
	* Next step is to transform the token list into a parse tree. This is
	* done most conveniently from right to left. If there is something that
	* we do not understand, just leave it alone, don't throw it away. The
	* address information that we're looking for sits in-between the current
	* node (tp) and the one called right. Add missing commas on the fly.
	*/
	state = DO_WORD;
	right = tail;
	tp = tail->prev;
	while (tp->type) {
		if (tp->type == TOK822_COMMENT) {	/* move comment to the side */
			MOVE_COMMENT_AND_CONTINUE(tp, right);
		} else if (tp->type == ';') {		/* rh side of named group */
			right = tok822_group(TOK822_ADDR, tp, right, ADD_COMMA);
			state = DO_GROUP | DO_WORD;
		} else if (tp->type == ':' && (state & DO_GROUP) != 0) {
			tp->type = TOK822_STARTGRP;
			(void) tok822_group(TOK822_ADDR, tp, right, NO_MISSING_COMMA);
			SKIP(tp, tp->type != ',');
			right = tp;
			continue;
		} else if (tp->type == '>') {		/* rh side of <route> */
			right = tok822_group(TOK822_ADDR, tp, right, ADD_COMMA);
			SKIP_MOVE_COMMENT(tp, tp->type != '<', right);
			(void) tok822_group(TOK822_ADDR, tp, right, NO_MISSING_COMMA);
			SKIP(tp, tp->type > 0xff || strchr(">;,:", tp->type) == 0);
			right = tp;
			state |= DO_WORD;
			continue;
		} else if (tp->type == TOK822_ATOM || tp->type == TOK822_QSTRING
			|| tp->type == TOK822_DOMLIT) {
				if ((state & DO_WORD) == 0)
					right = tok822_group(TOK822_ADDR, tp, right, ADD_COMMA)->next;
				state &= ~DO_WORD;
		} else if (tp->type == ',') {
			right = tok822_group(TOK822_ADDR, tp, right, NO_MISSING_COMMA);
			state |= DO_WORD;
		} else {
			state |= DO_WORD;
		}
		tp = tp->prev;
	}
	(void) tok822_group(TOK822_ADDR, tp, right, NO_MISSING_COMMA);

	/*
	* Discard the sentinel tokens on the left and right extremes. Properly
	* terminate the resulting list.
	*/
	tp = (head->next != tail ? head->next : 0);
	tok822_cut_before(head->next);
	tok822_free(head);
	tok822_cut_before(tail);
	tok822_free(tail);
	return (tp);
}

/* tok822_quote_atom - see if an atom needs quoting when externalized */

static void tok822_quote_atom(TOK822 *tp)
{
	const char   *cp;
	int     ch;

	/*
	* RFC 822 expects 7-bit data. Rather than quoting every 8-bit character
	* (and still passing it on as 8-bit data) we leave 8-bit data alone.
	*/
	for (cp = tp->vstr.c_str(); (ch = *(unsigned char *) cp) != 0; cp++) {
		if ( /* !ISASCII(ch) || */ ISSPACE(ch)
			|| ISCNTRL(ch) || strchr(tok822_opchar, ch)) {
				tp->type = TOK822_QSTRING;
				break;
		}
	}
}

/* tok822_comment - tokenize comment */

static const char *tok822_comment(TOK822 *tp, const char *str)
{
	TOK822 *tc = 0;
	int     ch;

#define COMMENT_TEXT_TOKEN(t) ((t) && (t)->type == TOK822_COMMENT_TEXT)

#define APPEND_NEW_TOKEN(tp, type, strval) \
	tok822_sub_append(tp, tok822_alloc(type, strval))

	while ((ch = *(unsigned char *) str) != 0) {
		str++;
		if (ch == '(') {			/* comments can nest! */
			//	    if (COMMENT_TEXT_TOKEN(tc))
			//		PString_TERMINATE(tc->vstr);
			tc = APPEND_NEW_TOKEN(tp, TOK822_COMMENT, (char *) 0);
			str = tok822_comment(tc, str);
		} else if (ch == ')') {
			break;
		} else {
			if (ch == '\\') {
				if ((ch = *(unsigned char *) str) == 0)
					break;
				str++;
			}
			if (!COMMENT_TEXT_TOKEN(tc))
				tc = APPEND_NEW_TOKEN(tp, TOK822_COMMENT_TEXT, (char *) 0);
			PASSERT3(tc);
            tc->vstr.append((char)ch);
		}
	}
	//    if (COMMENT_TEXT_TOKEN(tc))
	//	PString_TERMINATE(tc->vstr);
	return (str);
}

/* tok822_group - cluster a group of tokens */

static TOK822 *tok822_group(int group_type, TOK822 *left, TOK822 *right, int sync_type)
{
	TOK822 *group;
	TOK822 *sync;
	TOK822 *first;

	/*
	* Cluster the tokens between left and right under their own parse tree
	* node. Optionally insert a resync token.
	*/
	if (left != right && (first = left->next) != right) {
		tok822_cut_before(right);
		tok822_cut_before(first);
		group = tok822_alloc(group_type, (char *) 0);
		tok822_sub_append(group, first);
		tok822_append(left, group);
		tok822_append(group, right);
		if (sync_type) {
			sync = tok822_alloc(sync_type, (char *) 0);
			tok822_append(left, sync);
		}
	}
	return (left);
}

/* tok822_scan_addr - convert external address string to address token */

static TOK822 *tok822_scan_addr(const char *addr)
{
	TOK822 *tree = tok822_alloc(TOK822_ADDR, (char *) 0);

	tree->head = tok822_scan(addr, &tree->tail);
	return (tree);
}


void CommAddressNormalize(vector<PString> &addr, vector<PString> &addrn, const char * addressString)
{
	PString vp;
	PString vpn;
	TOK822 *list_;
	list_ = tok822_parse(addressString);

	TOK822 *l=list_;
	while(l)
	{
		l=tok822_normalize(vp, vpn, l,TOK822_STR_DEFL);
		if (vp.length() && vpn.length())
		{
			PString& add = *addr.insert( addr.end(), PString() );
			PString& addn = *addrn.insert( addrn.end(), PString() );
			add.assign(vp);
			addn.assign(vpn);
			addn.toLower();
		}
	}
	tok822_free_tree(list_);
}

void CommAddressNormalize(PString &addr, PString &addrn, const char * addressString)
{
	TOK822 *list_,*l;
	list_ = tok822_parse(addressString);
	l=tok822_normalize(addr, addrn, list_,TOK822_STR_DEFL);
	addrn.toLower();
	tok822_free_tree(list_);
}



#ifdef TEST

//#include <unistd.h>
//#include <vstream.h>
//#include <readlline.h>

/* tok822_print - display token */

static void tok822_print(TOK822 *list_head, int indent)
{
	TOK822 *tp;

	for (tp = list_head; tp; tp = tp->next) {
		if (tp->type < TOK822_MINTOK) {
			printf("%*s %s \"%c\"\n", indent, "", "OP", tp->type);
		} else if (tp->type == TOK822_ADDR) {
			printf("%*s %s\n", indent, "", "address");
			tok822_print(tp->head, indent + 2);
		} else if (tp->type == TOK822_COMMENT) {
			printf("%*s %s\n", indent, "", "comment");
			tok822_print(tp->head, indent + 2);
		} else if (tp->type == TOK822_STARTGRP) {
			printf("%*s %s\n", indent, "", "group \":\"");
		} else {
			printf("%*s %s \"%s\"\n", indent, "",
				tp->type == TOK822_COMMENT_TEXT ? "text" :
				tp->type == TOK822_ATOM ? "atom" :
				tp->type == TOK822_QSTRING ? "quoted string" :
				tp->type == TOK822_DOMLIT ? "domain literal" :
				tp->type == TOK822_ADDR ? "address" :
				"unknown\n", tp->vstr.c_str());
		}
	}
}

int     main(int unused_argc, char **unused_argv)
{
	char buffer[1000];
	while (gets(buffer)) 
	{
		printf("%s\n", buffer);
		vector<PString> addr,addrn;
		CommAddressNormalize(addr,addrn,buffer);
		for (int i=0; i<addr.size();i++)
		{
			printf("%s ----- %s\n",addr[i].c_str(),addrn[i].c_str());
		}
		PString res1,res2;
		CommAddressNormalize(res1,res2,buffer);
		printf("----- %s ----- %s\n",res1.c_str(),res2.c_str());
		printf("\n");

	}
	return (0);
}

#endif
