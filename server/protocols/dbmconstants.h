#pragma once

// SQL 

#define SQL_MAX_ID_LENGTH		128
#define GENERATOR_ID_INCREMENT	1


/* DB Tables*/
#define DB_TABLE_GENERATOR64	"GENERATOR64"
#define DB_TABLE_IDRANGES		"IDRANGES"
#define DB_TABLE_USERS			"USERS"
#define DB_TABLE_LOGGEDIN		"LOGGEDIN"
#define DB_TABLE_LOGGEDOUT		"LOGGEDOUT"

/* DB Columns*/
#define	OBJECT_NAME_LEN			31 

// USERS
#define USERS_USERID_LEN		21
#define USERS_EMAIL_LEN			101
#define USERS_FIRSTNAME_LEN		101
#define USERS_MIDDLENAME_LEN	101
#define USERS_LASTNAME_LEN		101
#define USERS_ADDR_1_LEN		301
#define USERS_ADDR_2_LEN		81
#define USERS_CITY_LEN			81
#define USERS_STATE_LEN			21
#define USERS_COUNTRY_LEN		21
#define USERS_ZIPCODE_LEN		11
#define USERS_PHONE_LEN			101
#define USERS_SEX_LEN			2
#define USERS_COMMNETS_LEN		1001
#define USERS_ENCRPWD_LEN		101
#define USERS_CURRENCY_LEN		4

// LOGGEDIN, LOGGEDOUT
#define INSTALL_ID_LEN			21
#define MAC_ADDR_LEN			21
#define ROUTERMAC_ADDR_LEN		21
#define IP_ADDR_LEN				41 //big enough for IP6 support