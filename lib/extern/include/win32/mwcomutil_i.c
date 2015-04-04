

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Dec 29 19:29:59 2014
 */
/* Compiler settings for win32\mwcomutil.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IMWUtil,0xC47EA90E,0x56D1,0x11d5,0xB1,0x59,0x00,0xD0,0xB7,0xBA,0x75,0x44);


MIDL_DEFINE_GUID(IID, LIBID_MWComUtil,0x7353810C,0xD335,0x4D50,0xBB,0x94,0xFF,0x0A,0xAF,0x0F,0xF1,0x26);


MIDL_DEFINE_GUID(CLSID, CLSID_MWField,0x59DDE98E,0xA8C3,0x458D,0xAF,0x8D,0x35,0x86,0x7A,0x0D,0xB2,0x00);


MIDL_DEFINE_GUID(CLSID, CLSID_MWStruct,0x5960F19B,0x0567,0x4C85,0x83,0x24,0x17,0x92,0xD8,0xA2,0x82,0x45);


MIDL_DEFINE_GUID(CLSID, CLSID_MWComplex,0x7812E472,0x0087,0x427C,0x9D,0xBF,0x4C,0xFC,0x90,0xCB,0x82,0x50);


MIDL_DEFINE_GUID(CLSID, CLSID_MWSparse,0x1F65F63E,0xA9E5,0x496C,0x9C,0xCB,0x09,0x68,0x48,0x82,0x1D,0xA3);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArg,0x7AFC5877,0x9C6E,0x417A,0xA9,0x0B,0xCB,0x43,0xB1,0xCC,0x64,0x5C);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArrayFormatFlags,0x92A4C794,0xED3F,0x4E73,0x9B,0x2D,0x27,0xF1,0x38,0x40,0xD6,0xBF);


MIDL_DEFINE_GUID(CLSID, CLSID_MWDataConversionFlags,0xEEB855D9,0xB87B,0x4FFC,0xAF,0xB3,0x00,0x51,0x90,0xBC,0x98,0xB0);


MIDL_DEFINE_GUID(CLSID, CLSID_MWUtil,0x17E60371,0x69FF,0x4BE8,0xBE,0x6E,0x62,0xC4,0x3C,0xD5,0x86,0x0B);


MIDL_DEFINE_GUID(CLSID, CLSID_MWFlags,0x01928A05,0xA07B,0x405D,0x82,0x9D,0x5A,0x60,0x97,0x3C,0xB5,0x2D);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



