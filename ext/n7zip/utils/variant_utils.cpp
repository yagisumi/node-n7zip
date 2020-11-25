#include "variant_utils.h"
#include "string_utils.h"

/*
enum VARENUM
{
  VT_EMPTY = 0,
  VT_NULL = 1,
  VT_I2 = 2,
  VT_I4 = 3,
  VT_R4 = 4,
  VT_R8 = 5,
  VT_CY = 6,
  VT_DATE = 7,
  VT_BSTR = 8,
  VT_DISPATCH = 9,
  VT_ERROR = 10,
  VT_BOOL = 11,
  VT_VARIANT = 12,
  VT_UNKNOWN = 13,
  VT_DECIMAL = 14,
  VT_I1 = 16,
  VT_UI1 = 17,
  VT_UI2 = 18,
  VT_UI4 = 19,
  VT_I8 = 20,
  VT_UI8 = 21,
  VT_INT = 22,
  VT_UINT = 23,
  VT_VOID = 24,
  VT_HRESULT = 25,
  VT_FILETIME = 64
};

typedef struct tagPROPVARIANT
{
  VARTYPE vt;
  PROPVAR_PAD1 wReserved1;
  PROPVAR_PAD2 wReserved2;
  PROPVAR_PAD3 wReserved3;
  union
  {
    CHAR cVal;
    UCHAR bVal;
    SHORT iVal;
    USHORT uiVal;
    LONG lVal;
    ULONG ulVal;
    INT intVal;
    UINT uintVal;
    LARGE_INTEGER hVal;
    ULARGE_INTEGER uhVal;
    VARIANT_BOOL boolVal;
    SCODE scode;
    FILETIME filetime;
    BSTR bstrVal;
  };
} PROPVARIANT;
*/

namespace n7zip {

Napi::Value
ConvertPropVariant(Napi::Env env, NWindows::NCOM::CPropVariant& prop)
{
  switch (prop.vt) {
    case VT_BSTR:
      return ConvertBStrToNapiString(env, prop.bstrVal);
    case VT_FILETIME:
      LARGE_INTEGER date;
      date.HighPart = prop.filetime.dwHighDateTime;
      date.LowPart = prop.filetime.dwLowDateTime;
      date.QuadPart -= 116444736000000000LL;
      return Napi::Number::New(env, date.QuadPart / 10000);
    case VT_NULL:
      return env.Null();
    case VT_I2:
      return Napi::Number::New(env, prop.iVal);
    case VT_I4:
      return Napi::Number::New(env, prop.lVal);
    case VT_I1:
      return Napi::Number::New(env, prop.cVal);
    case VT_INT:
      return Napi::Number::New(env, prop.intVal);
    case VT_I8:
      return Napi::Number::New(env, prop.hVal.QuadPart);
    case VT_UI1:
      return Napi::Number::New(env, prop.bVal);
    case VT_UI2:
      return Napi::Number::New(env, prop.uiVal);
    case VT_UI4:
      return Napi::Number::New(env, prop.ulVal);
    case VT_UI8:
      return Napi::Number::New(env, prop.uhVal.QuadPart);
    case VT_UINT:
      return Napi::Number::New(env, prop.uintVal);
    case VT_BOOL:
      return Napi::Boolean::New(env, prop.boolVal != VARIANT_FALSE);
    case VT_ERROR:
      return Napi::Number::New(env, prop.scode);
    default:
      return env.Undefined();
  }
}

} // namespace n7zip
