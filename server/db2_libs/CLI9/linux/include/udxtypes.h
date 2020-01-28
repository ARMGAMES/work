// udxtypes.h
// Type helpers for User Defined Functions and Aggregates
//
// (C) Copyright IBM Corp. 2007, 2011  All Rights Reserved.

#ifndef UDX_TYPES_INCLUDED
#define UDX_TYPES_INCLUDED

namespace nz {  
namespace udx_ver2 {

#define NZ_FUNC_MAX_ARGS 64

#define CHECK_RETURN_TYPE(type, expected, error) \
 if (type != expected && type != RV_PTR) \
    throwError(NZ_ERROR_ILLVAL, error, 0)

class ReturnValue
{
 private:
  // This union and the enums after
  // it should not be used by and UDX writers
  // and are subject to change
  union {
    int64 i8;
    int32 i4;
    int16 i2;
    int8  i1;
    void *ptr;
    double dbl;
    float flt;
  } value;

  enum types
    {
      RV_I8=0,
      RV_I4,
      RV_I2,
      RV_I1,
      RV_PTR,
      RV_DBL,
      RV_FLT
    };

  char type;
 public:

  // default constructor
  // Don't need copy constructors or operator =
  // Shallow copies are fine
  ReturnValue()
  {
    value.i1 = 0;
    type = RV_I1;
  }
  ReturnValue(void* a)
  {
    value.ptr = a;
    type = RV_PTR;
  }
  operator void*() const
  {
    CHECK_RETURN_TYPE(type, RV_PTR, "Expected pointer");
    return value.ptr;
  }
  ReturnValue(float a)
  {
    value.flt = a;
    type = RV_FLT;
  }
  operator float() const
  {
    CHECK_RETURN_TYPE(type, RV_FLT, "Expected float");
    return value.flt;
  }
  ReturnValue(double a)
  {
    value.dbl = a;
    type = RV_DBL;
  }
  operator double() const
  {
    CHECK_RETURN_TYPE(type, RV_DBL, "Expected double");
    return value.dbl;
  }
  
  ReturnValue(int32 a)    
  {
    value.i4 = a;
    type = RV_I4;
  }
  operator int32() const
  {
    CHECK_RETURN_TYPE(type, RV_I4, "Expected int32");
    return value.i4;
  }
  ReturnValue(int64 a)
  {
    value.i8 = a;
    type = RV_I8;
  }
  operator int64() const
  {
    CHECK_RETURN_TYPE(type, RV_I8, "Expected int64");
    return value.i8;
  }
  
  ReturnValue(int16 a)
  {
    value.i2 = a;
    type = RV_I2;
  }
  operator int16() const
  {
    CHECK_RETURN_TYPE(type, RV_I2, "Expected int16");
    return value.i2;
  }
  ReturnValue(int8 a)
  {
    value.i1 = a;
    type = RV_I1;
  }
  operator int8() const
  {
    CHECK_RETURN_TYPE(type, RV_I1, "Expected int8");
    return value.i1;
  }
        
};


typedef void* FuncDatum;


struct StringArg
{
  char* data;
  int   length;       // Bytes used by string data (not characters). 
  int   dec_length;   // Character declared length ie NCHAR(20) = 20. Will
                      // also be set  if VARCHAR or NVARCHAR.
};




struct StringReturn
{
  char* data;
  int   size; 
  // On enter it is the size (in bytes) allocated for string data.
  // On return it is the size (in bytes) actually used.
};


} // end namespace udx_ver2
} // end namespace nz

#endif //  #ifndef UDX_TYPES_INCLUDED
