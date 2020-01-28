// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#ifndef DSLClassInventory_h_included
#define DSLClassInventory_h_included


//***********************************************************************
// This file contains declaration of functions for an interpreter support
//***********************************************************************
// WARNING!!! All class field operations are valid ONLY for PUBLIC fields.
//***********************************************************************

// Initialization. Must be called ones at start.
void initClassInventory(void);
// Get class field info. Returns true if the field was found and the result is valid.
// 'UINT32& offset' is this field offset, 'PString& fieldtype' is this field type
// 'PString& fieldsubtype' is this field subtype - container template type, 'PString& fullfieldtype' is this field type as it's printed in output file
bool getFieldInfo(UINT32& offset, PString& fieldtype, PString& fieldsubtype, PString& fullfieldtype, const char* classname, const char* fieldname);
// Get class field offset. Returns true if the field was found and the result is valid.
bool getFieldOffset(UINT32& offset, const char* classname, const char* fieldname);
// Returns class field direct pointer. NULL - if not found
// 'void* obj' is an input pointer for the object which field is used.
void* getFieldAddress(void* obj, const char* classname, const char* fieldname);
// Returns pointer to container element by index. NULL - if the field was not found, the index is out of range or this field is not container. 
// 'void* obj' is an input pointer for the object which field is used.
void* getFieldContainerElement(void* obj, const char* classname, const char* fieldname, UINT32 index);
// Get container size. Returns true if the field was found as container and the result is valid.
// 'void* obj' is an input pointer for the object which field is used.
bool getFieldContainerSize(UINT32& size, void* obj, const char* classname, const char* fieldname);

#endif // DSLClassInventory_h_included
