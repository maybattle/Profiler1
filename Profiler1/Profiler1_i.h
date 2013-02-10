

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Sun Feb 10 19:45:46 2013
 */
/* Compiler settings for Profiler1.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Profiler1_i_h__
#define __Profiler1_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IProfiler_FWD_DEFINED__
#define __IProfiler_FWD_DEFINED__
typedef interface IProfiler IProfiler;

#endif 	/* __IProfiler_FWD_DEFINED__ */


#ifndef __Profiler_FWD_DEFINED__
#define __Profiler_FWD_DEFINED__

#ifdef __cplusplus
typedef class Profiler Profiler;
#else
typedef struct Profiler Profiler;
#endif /* __cplusplus */

#endif 	/* __Profiler_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IProfiler_INTERFACE_DEFINED__
#define __IProfiler_INTERFACE_DEFINED__

/* interface IProfiler */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IProfiler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2DC34852-BF94-48E5-BE9E-9960C9C1A304")
    IProfiler : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IProfilerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfiler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfiler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfiler * This);
        
        END_INTERFACE
    } IProfilerVtbl;

    interface IProfiler
    {
        CONST_VTBL struct IProfilerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfiler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProfiler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProfiler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProfiler_INTERFACE_DEFINED__ */



#ifndef __Profiler1Lib_LIBRARY_DEFINED__
#define __Profiler1Lib_LIBRARY_DEFINED__

/* library Profiler1Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_Profiler1Lib;

EXTERN_C const CLSID CLSID_Profiler;

#ifdef __cplusplus

class DECLSPEC_UUID("DFE1C658-1DB9-49A4-9BE4-D08E9A65653D")
Profiler;
#endif
#endif /* __Profiler1Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


