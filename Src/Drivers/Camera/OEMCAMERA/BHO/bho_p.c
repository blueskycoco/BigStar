
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Fri Apr 14 12:46:08 2006
 */
/* Compiler settings for .\bho.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AXP64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "bho.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IObjectWithSite, ver. 0.0,
   GUID={0xFC4801A3,0x2BA9,0x11CF,{0xA2,0x29,0x00,0xAA,0x00,0x3D,0x73,0x52}} */


/* Object interface: IBHOTest, ver. 0.0,
   GUID={0xEFA49367,0x861E,0x40b7,{0xBF,0x10,0x25,0xCA,0xEC,0x89,0x1B,0xC9}} */


extern const MIDL_STUB_DESC Object_StubDesc;


#pragma code_seg(".orpc")
CINTERFACE_PROXY_VTABLE(5) _IBHOTestProxyVtbl = 
{
    0,
    &IID_IBHOTest,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IObjectWithSite::SetSite */ ,
    0 /* (void *)-1 /* IObjectWithSite::GetSite */
};


static const PRPC_STUB_FUNCTION IBHOTest_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IBHOTestStubVtbl =
{
    &IID_IBHOTest,
    0,
    5,
    &IBHOTest_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x503011e, /* MIDL Version 5.3.286 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

#pragma data_seg(".rdata")

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };

const CInterfaceProxyVtbl * _bho_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IBHOTestProxyVtbl,
    0
};

const CInterfaceStubVtbl * _bho_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IBHOTestStubVtbl,
    0
};

PCInterfaceName const _bho_InterfaceNamesList[] = 
{
    "IBHOTest",
    0
};

const IID *  _bho_BaseIIDList[] = 
{
    &IID_IObjectWithSite,
    0
};


#define _bho_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _bho, pIID, n)

int __stdcall _bho_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_bho_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo bho_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _bho_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _bho_StubVtblList,
    (const PCInterfaceName * ) & _bho_InterfaceNamesList,
    (const IID ** ) & _bho_BaseIIDList,
    & _bho_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AXP64)*/


#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Fri Apr 14 12:46:08 2006
 */
/* Compiler settings for .\bho.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win64 (32b run,appending), ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if defined(_M_IA64) || defined(_M_AXP64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "bho.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IObjectWithSite, ver. 0.0,
   GUID={0xFC4801A3,0x2BA9,0x11CF,{0xA2,0x29,0x00,0xAA,0x00,0x3D,0x73,0x52}} */


/* Object interface: IBHOTest, ver. 0.0,
   GUID={0xEFA49367,0x861E,0x40b7,{0xBF,0x10,0x25,0xCA,0xEC,0x89,0x1B,0xC9}} */


extern const MIDL_STUB_DESC Object_StubDesc;


#pragma code_seg(".orpc")
CINTERFACE_PROXY_VTABLE(5) _IBHOTestProxyVtbl = 
{
    0,
    &IID_IBHOTest,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IObjectWithSite::SetSite */ ,
    0 /* (void *)-1 /* IObjectWithSite::GetSite */
};


static const PRPC_STUB_FUNCTION IBHOTest_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IBHOTestStubVtbl =
{
    &IID_IBHOTest,
    0,
    5,
    &IBHOTest_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x503011e, /* MIDL Version 5.3.286 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

#pragma data_seg(".rdata")

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };

const CInterfaceProxyVtbl * _bho_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IBHOTestProxyVtbl,
    0
};

const CInterfaceStubVtbl * _bho_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IBHOTestStubVtbl,
    0
};

PCInterfaceName const _bho_InterfaceNamesList[] = 
{
    "IBHOTest",
    0
};

const IID *  _bho_BaseIIDList[] = 
{
    &IID_IObjectWithSite,
    0
};


#define _bho_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _bho, pIID, n)

int __stdcall _bho_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_bho_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo bho_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _bho_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _bho_StubVtblList,
    (const PCInterfaceName * ) & _bho_InterfaceNamesList,
    (const IID ** ) & _bho_BaseIIDList,
    & _bho_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AXP64)*/

