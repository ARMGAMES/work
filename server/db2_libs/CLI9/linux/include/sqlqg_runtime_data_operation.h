/******************************************************************************
**
** Source File Name: sqlqg_runtime_data_operation.h
**
** (C) COPYRIGHT International Business Machines Corp. 1987, 2003
** All Rights Reserved
** Licensed Materials - Property of IBM
**
** US Government Users Restricted Rights - Use, duplication or
** disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
**
** Function = Include File defining:
**              Query Gateway classes and routines for
**              managing runtime data
**
** Operating System: All
**
*******************************************************************************/
#ifndef SQL_H_SQLQG_RUNTIME_DATA_OPERATION
#define SQL_H_SQLQG_RUNTIME_DATA_OPERATION

#include "sqlqg_misc.h"
#include "sql.h"
#include "sqlz.h"    // for SQLZVALUE_TYPE_T

#define SQL_STATEMENT_DEFAULT_INCREMENT 512
#define SQQG_MAX_STATEMENT_LENGTH 32766

// Forward declarations.
class  sqlnq_pid;
struct sqlri_rquery;
struct sqlri_riud;
struct sqlr_djx_savepoint_info;
struct sqlri_rpassthru;
class  sqlnn_dat_TYPTYPE;
class  sqlnq_fcs;
struct sqlri_rcol;
struct sqlri_rcollist;

static inline bool sqqgSQLTypeCatOraStringEmptyMeansNull(SQLZVALUE_TYPE_T sqltype)
{
  return ((sqltype == SQL_TYP_CHAR) ||
          (sqltype == SQL_TYP_VARCHAR) ||
          (sqltype == SQL_TYP_GRAPHIC) ||
          (sqltype == SQL_TYP_VARGRAPH)); 
}
/******************************************************************************
* Externalized data representation classes:
*
* It is important to keep their interfaces the same - one set for data
* description classes and few additional methods for the classes that also
* contain a buffer attribute. These classes are used by the wrapper writer, so
* we want them to use single interface for all data/data_description
* externalized by DB2
*
* Data description:
*    Request_Exp_Type        - each request exp. node
*    Runtime_Data_Desc       - data exchange during passthru
*
* Data:
*    Runtime_Data            - data exchange during runtime
*    Request_Constant        - request exp. constant node.
*
******************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// Runtime_Data_Desc base class (used to represent a description of a
// data value to be exchanged between the Query Gateway and a wrapper).
// Used in passthru for the wrapper to return the type description back to DB2
//////////////////////////////////////////////////////////////////////////////
class Runtime_Data_Desc : public Sqlqg_Base_Class {
public:

  // Constructors
  Runtime_Data_Desc (void);

  Runtime_Data_Desc (sqlint32*          a_rc,
                     sqlint16           a_type,             
                     sqlint16           a_max_length,      
                     sqluint16          a_code_page,      
                     sqlint16           a_null_ind,
                     unsigned char      a_data_precision=0,
                     unsigned char      a_data_scale=0,
                     sqluint8*          a_data_name=NULL,
                     sqlint16           a_name_length=0,
                     sqlint16           a_remote_type=0);

  Runtime_Data_Desc (sqlint32*          a_rc,
                     sqlnn_dat_TYPTYPE* a_dt);

  Runtime_Data_Desc (sqlint32*          a_rc,
                     sqlri_rcol*        a_runtime_desc);

  // Destructor
  ~Runtime_Data_Desc (void);


  unsigned char get_for_bit_data (void);

  sqlint16 get_null_indicator (void);

  sqlint16 get_data_type (void);

  sqlint16 get_maximum_length (void);                   

  unsigned char get_precision (void);

  unsigned char get_scale (void);

  sqluint16 get_codepage (void);

  unsigned char* get_name (void);

  sqlint16  get_name_length (void);

  sqlint16 get_remote_type (void);

  friend class DJ_Wrapper_Utilities;

private:                                            
  /* Data*/
  /************************************************************************
  // IMPORTANT  NOTE:  All the data members  must use platform independent
  // data types. All the pointers must use the union types as shown below
  // If you need more examples please look at class sqlqg_FMP_Request or
  // sqlqg_FMP_Reply. The fields must be arranged in decreasing order of
  // size to ensure proper  alignment . All this will ensure that the class
  // is fixed size on both  32-bit and 64-bit platforms . Failing to do so
  // will break the bridging code for 64-bit engine to 32-bit client .
  ************************************************************************/
  
  /************************************************************************
  // Two byte aligned fields should go here. Please note char[2] is not
  // two byte aligned . Char is always aligned on 1 byte boundary
  // Enumerated types are treated as int. So always 4 byte aligned
  ************************************************************************/
  sqlint16        data_type;               // SQL type of data.
  sqlint16        length;                  // length of data.
  sqluint16       codepage;                // code page to interpret data.
  sqlint16        null_indicator;          // whether or not nulls are allowed.
  sqlint16        name_length;             // length of column name.
  sqlint16        remote_type;             // remote type (if any).
  
  /************************************************************************
  // One byte aligned fields should go here. Please note char[n] is
  // always aligned on 1 byte boundary where "n" can be any number
  *************************************************************************/
 
  unsigned char   for_bit_data;            // data value is for bit data.
  unsigned char   precision;               // precision of data (if decimal).
  unsigned char   scale;                   // scale of data (if decimal).
  unsigned char   name[SQL_MAX_IDENT+1];   // column name.

};                                                 

//////////////////////////////////////////////////////////////////////////////
// Union values_union_type is used to make an array of  Runtime_Data_Desc
// objects fixed size and aligned on both 64-bit and 32-bit process .
//////////////////////////////////////////////////////////////////////////////
typedef union
{
  sqluint64          values_filler;
  Runtime_Data_Desc  *values;
} values_union_type;

//////////////////////////////////////////////////////////////////////////////
// Runtime_Data_Desc_List (used to represent a set of data value
// descriptions to be exchanged between the query gateway and wrappers).
//////////////////////////////////////////////////////////////////////////////
class Runtime_Data_Desc_List : public Sqlqg_Base_Class {
public:
  // Constructor.
  // Initialize with room for a_num_values entries
  Runtime_Data_Desc_List (sqlint32* a_rc,
                          sqlint32  a_num_values=0);      

  // Destructor
  // Destroys contained values as well
  ~Runtime_Data_Desc_List (void);

  sqlint32 get_number_of_values (void);                 
  
  sqlint32 set_number_of_values (sqlint32 a_count); 

  Runtime_Data_Desc* get_ith_value (sqlint32 a_index);    

  sqlint32 set_ith_value (Runtime_Data_Desc*  a_desc,
                          sqlint32            a_index);  

  Runtime_Data_Desc* operator[] (sqluint32  a_index);   

  Runtime_Data_Desc** get_values (void);

  void* get_values_addr (void);

  void set_values (values_union_type* a_new_values);

  friend class DJ_Wrapper_Utilities;

/*Data*/                                             
/************************************************************************
// IMPORTANT  NOTE:  All the data members  must use platform independent
// data types. All the pointers must use the union types as shown below
// If you need more examples please look at class sqlqg_FMP_Request or
// sqlqg_FMP_Reply. The fields must be arranged in decreasing order of
// size to ensure proper  alignment . All this will ensure that the class
// is fixed size on both  32-bit and 64-bit platforms . Failing to do so
// will break the bridging code for 64-bit engine to 32-bit client .
*************************************************************************/

private:
  /***********************************************************************
  // Put all the 8 bytes aligned members first . Pointers should always be
  // 8 bytes aligned too . Use union types as shown below
  ***************************************************************************/
  union{
    sqluint64          values_union_filler;
    values_union_type  *values_union;       // Array of description objects.
   };

  /*************************************************************************
  // Four byte aligned fields should go here. Please note char[4] is not
  // four byte aligned . Char is always aligned on 1 byte boundary
  // Enumerated types are treated as int. So always 4 byte aligned
  **************************************************************************/
  sqlint32              number_of_values;   // Number of descriptions in list.
};                                               

//////////////////////////////////////////////////////////////////////////////
// Request_Constant class (used to represent a data value to be
// exchanged between the Query Gateway and a wrapper during compile time (consts)
// This class MUST have exactly the same interface as Runtime_Data
//////////////////////////////////////////////////////////////////////////////
class Request_Constant  {

public:
  static Request_Constant* mk_request_constant (sqlnq_fcs* a_fcs);

  sqlint32  get_actual_length (void);
  unsigned  char* get_data (void);
  sqlint32  is_data_null (void);

  // from Runtime_Data_Desc
  unsigned  char get_for_bit_data (void);
  sqlint16  get_null_indicator (void);
  sqlint16  get_data_type (void);
  sqlint32  get_maximum_length (void);
  unsigned  char get_precision (void);
  unsigned  char get_scale (void);
  sqluint16 get_codepage (void);
  unsigned  char* get_name (void);
  sqlint16  get_name_length (void);

protected:
  ~Request_Constant (void);    // Unimplemented

private:
  Request_Constant (void);     // Unimplemented
};

//////////////////////////////////////////////////////////////////////////////
// Request_Exp_Type class - a type of a request expression
// The interface MUST be exactly the same as Runtime_Data_Desc
//////////////////////////////////////////////////////////////////////////////
class Request_Exp_Type  {

public:
  static Request_Exp_Type* mk_request_exp_type (sqlnq_pid* a_expression);

  unsigned  char get_for_bit_data (void);
  sqlint16  get_null_indicator (void);
  sqlint16  get_data_type (void);
  sqlint32  get_maximum_length (void);
  unsigned  char get_precision (void);
  unsigned  char get_scale (void);
  sqluint16 get_codepage (void);
  unsigned  char* get_name (void);
  sqlint16  get_name_length (void);

private:
  Request_Exp_Type (void);         // Unimplemented
  ~Request_Exp_Type (void);        // Unimplemented
};



//////////////////////////////////////////////////////////////////////////////
// Runtime_Data base class. Represent a data value to be
// exchanged between the Query Gateway and a wrapper during runtime
// The interface MUST match with Request_Constant interface
//////////////////////////////////////////////////////////////////////////////
class Runtime_Data  : public Sqlqg_Base_Class{
public:
  Runtime_Data (sqlint32*   a_rc,
                sqlri_rcol* a_runtime_data);

  ~Runtime_Data (void);

  sqlint32 get_data_index (void);

  unsigned char get_invariant (void);

  void set_data_description (Runtime_Data_Desc* a_description);

  sqlint32 get_actual_length (void);

  void set_actual_length (sqlint32 a_length);

  unsigned char* get_data (void);

  sqlint32 set_data (unsigned char*  a_data_ptr,
                     sqlint32        a_copy_len,
                     const bool      a_pad_to_max_len = false);

  // Use for both semantic and "friendly arithmetic" nulls.  Will
  // return SQLQG_RC_E407 if column is not nullable
  sqlint32 set_data_null (void);

  // Clear the null indicator, including "friendly arithmetic" bits
  sqlint32 clear_null_indicator (void);

  // For "friendly arithmetic" nulls use one of these as well.  Will
  // return SQLQG_ERROR (for now) if column not set to null first
  sqlint32 set_friendly_div_by_0 (void);

  sqlint32 set_friendly_exception (void);

  // Will return TRUE for both semantic and "friendly arithmetic" nulls
  sqlint32 is_data_null (void);
  sqlint32 is_data_nullable (void);

  // Will return TRUE for semantic null only
  sqlint32 is_semantic_null (void);

  // These need not be checked unless is_data_null() returns TRUE
  sqlint32 check_friendly_div_by_0 (void);
  sqlint32 check_friendly_exception (void);

  unsigned char  get_for_bit_data (void);
  sqlint16       get_null_indicator (void);
  void           set_null_indicator (sqlint16 a_indicator);
  sqlint16       get_data_type (void);
  sqlint32       get_maximum_length (void);
  unsigned char  get_precision (void);
  void           set_precision (unsigned char a_precision);
  unsigned char  get_scale (void);
  void           set_scale (unsigned char a_scale);
  sqluint16      get_codepage (void);
  unsigned char* get_name (void);
  sqlint16       get_name_length (void);
  sqlint16       get_remote_type (void);
  sqlint32       get_remote_length (void);
  unsigned char  get_remote_precision (void);
  unsigned char  get_remote_scale (void);

  friend class DJ_Wrapper_Utilities;
  friend class Runtime_Data_List;

private:
  ////////////////
  // Data.
  ////////////////

  sqlri_rcol* runtime_info;         // pointer to internal runtime information.

  ////////////////
  // Methods.
  ////////////////
  // For internal use only.
  sqlri_rcol* get_runtime_info (void);

protected:
  void set_runtime_info (sqlri_rcol *a_rcol);
};

//////////////////////////////////////////////////////////////////////////////
// Runtime_Data list (used to represent a set of data values to be exchanged
// between the query gateway and wrappers).
//////////////////////////////////////////////////////////////////////////////
class Runtime_Data_List : public Sqlqg_Base_Class {
public:
  // Constructor
  Runtime_Data_List (sqlint32*        a_rc,
                     sqlri_rcollist*  a_runtime_data_list);

  // Destructor
  ~Runtime_Data_List (void);

  sqlint32 get_number_of_values (void);

  void refresh (sqlint32*       a_rc,
                sqlri_rcollist* a_runtime_values);

  Runtime_Data* get_ith_value (sqlint32 a_index);

  Runtime_Data* operator[] (sqluint32 a_index);

  friend class DJ_Wrapper_Utilities;

private:

  ////////////////
  // Data.
  ////////////////
  sqlint32      number_of_values;  // Number of columns in list.
  Runtime_Data  **values;          // Array of column objects.
};

// Inline methods

// Runtime_Data_Desc
inline
Runtime_Data_Desc::Runtime_Data_Desc (void)
{}

inline unsigned char
Runtime_Data_Desc::get_for_bit_data (void)
{
  return for_bit_data;
}

inline sqlint16
Runtime_Data_Desc::get_null_indicator (void)
{
  return null_indicator;
}

inline sqlint16
Runtime_Data_Desc::get_data_type (void)
{
  return data_type;
}

inline sqlint16                                  
Runtime_Data_Desc::get_maximum_length (void)
{
  return length;
}

inline sqluint16
Runtime_Data_Desc::get_codepage (void)
{
  return codepage;
}

inline unsigned char*
Runtime_Data_Desc::get_name (void)
{
  return name;
}

inline sqlint16
Runtime_Data_Desc::get_name_length (void)
{
  return name_length;
}

inline sqlint16
Runtime_Data_Desc::get_remote_type (void)
{
  return remote_type;
}

// Runtime_Data_Desc_List
inline sqlint32                            
Runtime_Data_Desc_List::get_number_of_values (void)
{
  return  number_of_values;
}

inline Runtime_Data_Desc**
Runtime_Data_Desc_List::get_values (void)
{
  return &(values_union->values);
}

inline void*
Runtime_Data_Desc_List::get_values_addr (void)
{
  return &values_union;
}

inline void
Runtime_Data_Desc_List::set_values (values_union_type* a_new_values)
{
  values_union = a_new_values;
}

// Runtime_Data
inline sqlri_rcol*
Runtime_Data::get_runtime_info (void)
{
  return runtime_info;
}

inline sqlint32
Runtime_Data_List::get_number_of_values (void)
{
  return  number_of_values;
}

#endif

