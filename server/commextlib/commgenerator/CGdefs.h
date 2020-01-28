// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGdefs_h_included
#define CGdefs_h_included

//******************************************************************************
// This is the application definitions
//******************************************************************************

#define makeSpaceMargin(s,x) s.insert(s.length(), x, '\n');
#define makeTabMargin(s,x) s.insert(s.length(), x, '\t');

#define COPYRIGHT "// Copyright(C) Stars Technologies. All rights reserved.\n// This file has been generated from a definition file. DON'T make any changes.\n\n"

// File settings
#define CG_INI_FILE_NAME		"commgenerator.ini"
#define CG_DATA_FILE_NAME		"commdata.dat"
#define CG_LOG_FILE_NAME		"commgenerator.log"
#define CG_INI_FILE_MAX			256
#define CG_STANDARD_INFILE_EXT	"inp"

// Constant values definition
extern const char* help;
extern const char* ForbiddenDefinitions[];
extern const size_t ForbiddenDefinitionsSize;

// Plugin start function
typedef void (__cdecl *PluginProc)(CGmain*);

// Type converter
typedef string (*typeConverter)(const string&);

// Dynamic plugin extension
#ifdef _WIN32
#define PLUGIN_DYN_EXT		".plg"
#else
#define PLUGIN_DYN_EXT	
#endif

// field type property
#define CG_FIELDTYPE_UNSIGNED		(uint32_t)0x00000001
#define CG_FIELDTYPE_CONTAINER		(uint32_t)0x00000002
#define CG_FIELDTYPE_ARRAY			(uint32_t)0x00000004
#define CG_FIELDTYPE_DIRECTINDEX	(uint32_t)0x00000008
#define CG_FIELDTYPE_POINTER		(uint32_t)0x00000010

// class functions property
// copy constructor
#define CG_CLASS_COPY_MASK			(uint64_t)0x0000000000000003
#define CG_CLASS_COPY_ON			(uint64_t)0x0000000000000001
#define CG_CLASS_COPY_DENY			(uint64_t)0x0000000000000002

//move constructor
#define CG_CLASS_MOVE_MASK			(uint64_t)0x000000000000000C
#define CG_CLASS_MOVE_ON			(uint64_t)0x0000000000000004
#define CG_CLASS_MOVE_DENY			(uint64_t)0x0000000000000008

// assignment operator
#define CG_CLASS_ASSIGN_MASK		(uint64_t)0x0000000000000030
#define CG_CLASS_ASSIGN_ON			(uint64_t)0x0000000000000010
#define CG_CLASS_ASSIGN_DENY		(uint64_t)0x0000000000000020

//move assignment operator
#define CG_CLASS_MASSIGN_MASK		(uint64_t)0x00000000000000C0
#define CG_CLASS_MASSIGN_ON			(uint64_t)0x0000000000000040
#define CG_CLASS_MASSIGN_DENY		(uint64_t)0x0000000000000080

// copy from casted base class
#define CG_CLASS_COPYBASE_MASK		(uint64_t)0x0000000000000100
#define CG_CLASS_COPYBASE_ON		(uint64_t)0x0000000000000100

//move from casted base class
#define CG_CLASS_MOVEBASE_MASK		(uint64_t)0x0000000000000200
#define CG_CLASS_MOVEBASE_ON		(uint64_t)0x0000000000000200

// template
#define CG_CLASS_TEMP_MASK			(uint64_t)0x0000000000000400
#define CG_CLASS_TEMP_ON			(uint64_t)0x0000000000000400

// smart pointers
#define CG_CLASS_PTRS_SMART_MASK	(uint64_t)0x0000000000000800
#define CG_CLASS_PTRS_SMART_RESTR	(uint64_t)0x0000000000000800	// for func generation

// compare function
#define CG_CLASS_EQUAL_MASK			(uint64_t)0x000000000000F000
#define CG_CLASS_EQUAL_ON			(uint64_t)0x0000000000001000
#define CG_CLASS_EQUAL_BASE			(uint64_t)0x0000000000002000
#define CG_CLASS_EQUAL_ONBASE		(uint64_t)0x0000000000003000
#define CG_CLASS_EQUAL_LOG			(uint64_t)0x0000000000004000
#define CG_CLASS_EQUAL_ONLOG		(uint64_t)0x0000000000005000
#define CG_CLASS_EQUAL_ONBASELOG	(uint64_t)0x0000000000007000
#define CG_CLASS_EQUAL_BYFIELD		(uint64_t)0x0000000000008000

// inheritance
#define CG_CLASS_INH_MASK			(uint64_t)0x00000000000F0000
#define CG_CLASS_INH_BASE			(uint64_t)0x0000000000010000
#define CG_CLASS_INH_VIRT			(uint64_t)0x0000000000020000
#define CG_CLASS_INH_BASEVIRT		(uint64_t)0x0000000000030000

// validate necessary values
#define CG_CLASS_VALIDATE_MASK		(uint64_t)0x0000000000300000
#define CG_CLASS_VALIDATE_ON		(uint64_t)0x0000000000100000
#define CG_CLASS_VALIDATE_REST		(uint64_t)0x0000000000200000

// include headers
#define CG_CLASS_INK_MASK			(uint64_t)0x0000000000C00000
#define CG_CLASS_INK_STD			(uint64_t)0x0000000000400000
#define CG_CLASS_INK_BASE			(uint64_t)0x0000000000800000
#define CG_CLASS_INK_ALL			(uint64_t)0x0000000000C00000

// class dump
#define CG_CLASS_DUMP_MASK			(uint64_t)0x0000000001000000
#define CG_CLASS_DUMP_ON			(uint64_t)0x0000000001000000

// check parser for end of msg
#define CG_CLASS_PARSECHK_MASK		(uint64_t)0x0000000002000000
#define CG_CLASS_PARSECHK_ON		(uint64_t)0x0000000002000000

// model type support
#define CG_CLASS_SELFIELD_MASK		(uint64_t)0x0000000004000000
#define CG_CLASS_SELFIELD_ON		(uint64_t)0x0000000004000000

// field reference
#define CG_CLASS_REFFIELD_MASK		(uint64_t)0x0000000008000000
#define CG_CLASS_REFFIELD_ON		(uint64_t)0x0000000008000000

// subscriptions
#define CG_CLASS_SUBSCR_MASK		(uint64_t)0x00000000F0000000
#define CG_CLASS_SUBSCR_LEAFNODE	(uint64_t)0x0000000010000000

// Java script ignore
#define CG_JS_IGNORE_MASK			(uint64_t)0x0000000800000000
#define CG_JS_IGNORE_ON				(uint64_t)0x0000000800000000

// special work
#define CG_CLASS_SPECIAL_MASK		(uint64_t)0x000000F000000000
#define CG_CLASS_SPECIAL_1			(uint64_t)0x0000001000000000

// internal class
#define CG_CLASS_INTERNAL_MASK		(uint64_t)0x0000030000000000
#define CG_CLASS_INTERNAL_ON		(uint64_t)0x0000010000000000
#define CG_CLASS_INTERNAL_INCL		(uint64_t)0x0000020000000000

// update function
#define CG_CLASS_UPDATE_MASK		(uint64_t)0x0000040000000000
#define CG_CLASS_UPDATE_ON			(uint64_t)0x0000040000000000

// update function
#define CG_IGNORE_UPDATE_MASK		(uint64_t)0x0000080000000000
#define CG_IGNORE_UPDATE_ON			(uint64_t)0x0000080000000000

// JavaScript super constructor
#define CG_JS_SUPER_MASK			(uint64_t)0x0000100000000000
#define CG_JS_SUPER_ON				(uint64_t)0x0000100000000000

// JSON
#define CG_JN_MASK					(uint64_t)0x0000200000000000
#define CG_JN_ON					(uint64_t)0x0000200000000000

// object archived
#define CG_CLASS_ARCHIVED_MASK		(uint64_t)0x0001000000000000
#define CG_CLASS_ARCHIVED_ON		(uint64_t)0x0001000000000000

// object ID set
#define CG_CLASS_OBJECTID_MASK		(uint64_t)0x0002000000000000
#define CG_CLASS_OBJECTID_ON		(uint64_t)0x0002000000000000

// version set
#define CG_CLASS_VERSION_MASK		(uint64_t)0x0004000000000000
#define CG_CLASS_VERSION_ON			(uint64_t)0x0004000000000000

// update handler
#define CG_CLASS_UPDATEHANDLER_MASK	(uint64_t)0x0008000000000000
#define CG_CLASS_UPDATEHANDLER_ON	(uint64_t)0x0008000000000000

// object hidden
#define CG_FIELD_HIDDEN_MASK		(uint64_t)0x0010000000000000
#define CG_FIELD_HIDDEN_ON			(uint64_t)0x0010000000000000

// 
#define CG_CPP_IGNORE_MASK			(uint64_t)0x0020000000000000
#define CG_CPP_IGNORE_ON			(uint64_t)0x0020000000000000

// update handler 2
#define CG_CLASS_UPDATEHANDLER2_MASK (uint64_t)0x0040000000000000
#define CG_CLASS_UPDATEHANDLER2_ON	(uint64_t)0x0040000000000000

#define CG_AUTO_UPDATE_MASK			(uint64_t)0x0F00000000000000
// version increase
#define CG_UPDVER_INC_MASK			(uint64_t)0x0100000000000000
#define CG_UPDVER_INC_ON			(uint64_t)0x0100000000000000

// updater
#define CG_UPDATER_MASK				(uint64_t)0x0200000000000000
#define CG_UPDATER_ON				(uint64_t)0x0200000000000000

// current time 
#define CG_CURR_TIME_MASK			(uint64_t)0x0400000000000000
#define CG_CURR_TIME_ON				(uint64_t)0x0400000000000000

// string restrictions value
#define CG_CLASS_RESTR_STR_MASK		(uint64_t)0xF000000000000000
#define CG_CLASS_RESTR_STR_SHIFT	60


//////////////////////////
// Java transient
#define CG_JAVA_TRANSIENT_MASK		(uint64_t)0x0000000000000001
#define CG_JAVA_TRANSIENT_ON		(uint64_t)0x0000000000000001

// Java overload
#define CG_JAVA_OVERRIDE_MASK		(uint64_t)0x0000000600000030
#define CG_JAVA_OVERRIDE_PARSE		(uint64_t)0x0000000000000010
#define CG_JAVA_OVERRIDE_COMPOSE	(uint64_t)0x0000000000000020
#define CG_JAVA_OVERRIDE_ALL		(uint64_t)0x0000000000000030

// Java field override
#define CG_JAVA_FIELDOVERRIDE_MASK	(uint64_t)0x0000000000000100
#define CG_JAVA_FIELDOVERRIDE_ON	(uint64_t)0x0000000000000100

// Java model type
#define CG_JAVA_MODEL_ARCH_MASK		(uint64_t)0x0000000000000200
#define CG_JAVA_MODEL_ARCH_ON		(uint64_t)0x0000000000000200



enum class objTypes {gTypeUnknown, dTypeDocument, gTypeStruct, gTypeField, gTypeAccess, gTypeInclude, gTypeComment, gTypeEmptyLine, gTypeText};

struct gsSubFieldTypes
{
	string				name;
	int					subFieldsAmount;
	bool				directIndex = false;
	string				subTypeForIndexed;
	string				baseContainerType;
};

extern const gsSubFieldTypes gsSubFieldTypesData[];
extern const size_t gsSubFieldTypesSize;

#endif	// CGdefs_h_included
