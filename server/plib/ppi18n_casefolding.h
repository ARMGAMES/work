#ifndef ppi18n_casefolding_h_included
#define ppi18n_casefolding_h_included

#include "ppi18n.h"

PUNICHAR_32 i18nCharToLowerCS( PUNICHAR_32 c );

inline PUNICHAR_RET i18nCharToLowerCS( PUNICHAR_RET c )
{
	return static_cast< PUNICHAR_RET >( i18nCharToLowerCS( static_cast< PUNICHAR_32 >( c ) ) );
}

PUNICHAR_32 i18nCharToUpperCS( PUNICHAR_32 c );

inline PUNICHAR_RET i18nCharToUpperCS( PUNICHAR_RET c )
{
	return static_cast< PUNICHAR_RET >( i18nCharToUpperCS( static_cast< PUNICHAR_32 >( c ) ) );
}

#endif //ppi18n_casefolding_h_included
