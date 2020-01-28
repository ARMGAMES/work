#ifndef auth_h_included
#define auth_h_included

// common structures and constants for the authentication server

// login types mask for privileges and required login method
#define AUTH_LOGINTYPE_PWD (1<<0)					// PWD login (introduced for consistency, should always be set)
#define AUTH_LOGINTYPE_PSP (1<<1)					// PSP login (PokerStars PIN is allowed/required)
#define AUTH_LOGINTYPE_RSA (1<<2)					// RSA login (RSA token login is allowed/required
#define AUTH_LOGINTYPE_OTP (1<<3)					// OTP login (used only for actual login performed)
#define AUTH_LOGINTYPE_LTD (1<<4)					// Limited login (used only for actual login performed)
#define AUTH_LOGINTYPE_TEMP_PIN_EACH_LOGIN	(1<<5)	// PYR-76895. Temporary one time pin is assigned on each login.

#define AUTH_LOGINTYPE_MAX  63		// IMPORTANT: always update it when new login type is introduced

// token status values
#define TOKEN_AVAILABLE	0	// RSA token is available to be assigned
#define TOKEN_ASSIGNED	1	// RSA token is currently assigned to a player
#define TOKEN_FROZEN	2	// RSA token is frozen and unavailable 
#define TOKEN_MIGRATED	3	// RSA token was migrated 

// PYR-54466
#define PSP_MIN_LENGTH 4
#define PSP_MAX_LENGTH 6
#define PSP_MIN_DIFFERENT_DIGITS 3 // For length > PSP_MIN_LENGTH
#define PSP_MAX_REPEATED_DIGITS 2 // For length == PSP_MIN_LENGTH

#endif
