/************************************************************************
Snail Proprietary
Copyright (c) 2012, Snail Incorporated. All Rights Reserved

Snail, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of Snail, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef __snType_H__
#define __snType_H__

#include <stdlib.h>

#ifdef _WIN32
#include "tchar.h"
#endif // _WIN32

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
#	define SN_API __cdecl
#	define SN_CBI __stdcall
#else
#	define SN_API
#	define SN_CBI
#endif //_WIN32

/** SN_IN is used to identify inputs to an SN function.  This designation
    will also be used in the case of a pointer that points to a parameter
    that is used as an output. */
#ifndef SN_IN
#define SN_IN
#endif

/** SN_OUT is used to identify outputs from an SN function.  This
    designation will also be used in the case of a pointer that points
    to a parameter that is used as an input. */
#ifndef SN_OUT
#define SN_OUT
#endif

/** SN_INOUT is used to identify parameters that may be either inputs or
    outputs from an SN function at the same time.  This designation will
    also be used in the case of a pointer that  points to a parameter that
    is used both as an input and an output. */
#ifndef SN_INOUT
#define SN_INOUT
#endif

#ifdef __SYMBIAN32__
#define	SN_EXPORT_FUNC	EXPORT_C
#elif defined LINUX || defined _LINUX || defined _IOS || defined _MAC_OS
#if defined EXPORT
#undef EXPORT
#endif
#define EXPORT __attribute__((visibility("default")))
#define EXPORT_CLS EXPORT
#define	SN_EXPORT_FUNC
#elif defined WIN32

#if defined EXPORT
#undef EXPORT
#endif

#define EXPORT __declspec(dllexport)

#if (_SNMODULEID==0x00000001)
#define EXPORT_CLS __declspec(dllexport)
#else
#define EXPORT_CLS __declspec(dllimport)
#endif

#else
#define	SN_EXPORT_FUNC
#define	EXPORT
#endif // __SYMBIAN32__

#define SN_MAX_ENUM_VALUE	0X7FFFFFFF

/** SN_VOID */
typedef void SN_VOID;

/** SN_U8 is an 8 bit unsigned quantity that is byte aligned */
typedef unsigned char SN_U8;

/** SN_BYTE is an 8 bit unsigned quantity that is byte aligned */
typedef unsigned char SN_BYTE;

/** SN_S8 is an 8 bit signed quantity that is byte aligned */
typedef signed char SN_S8;

/** SN_CHAR is an 8 bit signed quantity that is byte aligned */
typedef char SN_CHAR;

/** SN_U16 is a 16 bit unsigned quantity that is 16 bit word aligned */
typedef unsigned short SN_U16;

/** SN_WCHAR is a 16 bit unsigned quantity that is 16 bit word aligned */
#if defined _WIN32
typedef unsigned short SN_WCHAR;
typedef unsigned short* SN_PWCHAR;
#elif defined LINUX
typedef unsigned char SN_WCHAR;
typedef unsigned char* SN_PWCHAR;
#endif

#ifdef _WIN32
#define SN_TCHAR		char
#define SN_PTCHAR		char*
#else
typedef char TCHAR, *PTCHAR;
#define SN_TCHAR		char
#define SN_PTCHAR		char*
#endif // _WIN32

/** SN_S16 is a 16 bit signed quantity that is 16 bit word aligned */
typedef signed short SN_S16;

/** SN_U32 is a 32 bit unsigned quantity that is 32 bit word aligned */
typedef unsigned int SN_U32;
#if defined (_WIN64)
    typedef unsigned long long SN_ULONG;
#else
    typedef unsigned long SN_ULONG;
#endif

/** SN_S32 is a 32 bit signed quantity that is 32 bit word aligned */
typedef signed int SN_S32;
#if defined (_WIN64)
    typedef signed long long SN_SLONG;
#else
    typedef signed long SN_SLONG;
#endif

/* Users with compilers that cannot accept the "long long" designation should
   define the SN_SKIP64BIT macro.  It should be noted that this may cause
   some components to fail to compile if the component was written to require
   64 bit integral types.  However, these components would NOT compile anyway
   since the compiler does not support the way the component was written.
*/
#ifndef SN_SKIP64BIT
#ifdef _WIN32
/** SN_U64 is a 64 bit unsigned quantity that is 64 bit word aligned */
typedef unsigned __int64  SN_U64;
/** SN_S64 is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed   __int64  SN_S64;
#else // WIN32
/** SN_U64 is a 64 bit unsigned quantity that is 64 bit word aligned */
typedef unsigned long long SN_U64;
/** SN_S64 is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed long long SN_S64;
#endif // WIN32
#endif // SN_SKIP64BIT

/** The SN_BOOL type is intended to be used to represent a true or a false
    value when passing parameters to and from the SN core and components.  The
    SN_BOOL is a 32 bit quantity and is aligned on a 32 bit word boundary.
 */
typedef enum SN_BOOL {
    SN_FALSE = 0,
    SN_TRUE = !SN_FALSE,
	SN_BOOL_MAX = SN_MAX_ENUM_VALUE
} SN_BOOL;

/** The SN_PTR type is intended to be used to pass pointers between the SN
    applications and the SN Core and components.  This is a 32 bit pointer and
    is aligned on a 32 bit boundary.
 */
typedef void* SN_PTR;
typedef const void* SN_CPTR;

/** The SN_HANDLE type is intended to be used to pass pointers between the SN
    applications and the SN Core and components.  This is a 32 bit pointer and
    is aligned on a 32 bit boundary.
 */
typedef void* SN_HANDLE;

/** The SN_STRING type is intended to be used to pass "C" type strings between
    the application and the core and component.  The SN_STRING type is a 32
    bit pointer to a zero terminated string.  The  pointer is word aligned and
    the string is byte aligned.
 */
typedef char* SN_PCHAR;

/** The SN_PBYTE type is intended to be used to pass arrays of bytes such as
    buffers between the application and the component and core.  The SN_PBYTE
    type is a 32 bit pointer to a zero terminated string.  The  pointer is word
    aligned and the string is byte aligned.
 */
typedef unsigned char* SN_PBYTE;

/** The SN_PTCHAR type is intended to be used to pass arrays of wchar such as
    unicode char between the application and the component and core.  The SN_PTCHAR
    type is a 32 bit pointer to a zero terminated string.  The  pointer is word
    aligned and the string is byte aligned.
 */

#ifdef _UNICODE
typedef unsigned short* SN_PTTCHAR;
typedef unsigned short SN_TTCHAR;
#else
typedef char* SN_PTTCHAR;
typedef char SN_TTCHAR;
#endif


#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

/**
 * Input stream format, Frame or Stream..
 */
typedef enum {
    SN_INPUT_FRAME	= 1,	/*!< Input contains completely frame(s) data. */
    SN_INPUT_STREAM,		/*!< Input is stream data. */
	SN_INPUT_STREAM_MAX = SN_MAX_ENUM_VALUE
} SN_INPUT_TYPE;

/**
 * General data buffer, used as input or output.
 */
typedef struct {
	SN_S32		nFlag;		/*!< add flag    */
	SN_PBYTE	Buffer;		/*!< Buffer pointer */
	SN_U32		Length;		/*!< Buffer size in byte */
	SN_S64		Time;		/*!< The time of the buffer */
	SN_U32		nReserved;  /*!< The reserved data for later use.*/
	SN_PTR		pReserved;  /*!< The reserved data for later use.*/
} SN_USERBUFFER;

/**
 * General data buffer, used as input or output.
 */
typedef struct {
	SN_PBYTE	Buffer;			/*!< Buffer pointer */
	SN_U32		Length;			/*!< Buffer size in byte */
	SN_U64		Time;			/*!< The time of the buffer */
	SN_PTR		UserData;		/*!< The user data for later use.*/
} SN_CODECBUFFER;


/**
 * GUID structure...
 */
typedef struct _SN_GUID {
	SN_U32	Data1;		/*!< Data1 value */
	SN_U16	Data2;		/*!< Data2 value */
	SN_U16	Data3;		/*!< Data3 value */
	SN_U8	Data4[8];	/*!< Data4 value */
} SN_GUID;


/**
 * Head Info description
 */
typedef struct _SN_HEAD_INFO {
	SN_PBYTE	Buffer;				/*!< [In] Head Buffer pointer */
	SN_U32		Length;				/*!< [In] Head Buffer size in byte */
	SN_PCHAR	Description;		/*!< [In/Out] Allocated by Caller. The char buffer of description  */
	SN_U32		Size;				/*!< [In] The size of description  */
} SN_HEAD_INFO;


/**
 * The init memdata flag.
 */
typedef enum{
	SN_IMF_USERMEMOPERATOR		=0,	/*!< memData is  the pointer of memoperator function, H265 not use*/
	SN_IMF_PREALLOCATEDBUFFER	=1,	/*!< memData is  preallocated memory*/
	SN_IMF_USERMEMOPERATOR2		=2,	/*!< memData is  the pointer of memoperator function, for H265*/
	SN_IMF_MAX = SN_MAX_ENUM_VALUE
}SN_INIT_MEM_FlAG;


typedef struct
{
	SN_PTR	pUserData;
	SN_PTR	(SN_API * LoadLib) (SN_PTR pUserData, SN_PCHAR pLibName, SN_S32 nFlag);
	SN_PTR	(SN_API * GetAddress) (SN_PTR pUserData, SN_PTR hLib, SN_PCHAR pFuncName, SN_S32 nFlag);
	SN_S32	(SN_API * FreeLib) (SN_PTR pUserData, SN_PTR hLib, SN_S32 nFlag);
} SN_LIB_OPERATOR;

/**
 * The init memory structure..
 */
typedef struct{
	SN_U32						memflag;		/*!<memory and other param flag  0X000X for memData type, 0X0010 for lib op, 0X0100 for Thumbnail or Video */
	SN_PTR						memData;		/*!<a pointer to SN_MEM_OPERATOR or a preallocated buffer  */
	SN_LIB_OPERATOR *			libOperator;	/*!<Library operator function pointer. If memflag is 0X1x, the param is available  */
	SN_ULONG					reserved1;		/*!<reserved  */
	SN_ULONG					reserved2;		/*!<reserved */
}SN_CODEC_INIT_USERDATA;

/**
 * Thread create function
 * \param pHandle [in/out] the handle of thread
 * \param nID [in] the id of thread
 * \param fProc [in] the function entry pointer
 * \param pParam [in] the parameter in call function.
 */
typedef int (* SNThreadCreate) (void ** pHandle, int * pID, void * fProc, void * pParam);

/**
 * Input stream format, Frame or Stream..
 */
typedef enum {
    SN_CPU_ARMV4		= 1,	/*!< The CPU is ARMV4. */
    SN_CPU_ARMV5,				/*!< The CPU is ARMV5. */
    SN_CPU_ARMV5x,				/*!< The CPU is ARMV5+. */
    SN_CPU_ARMV6,				/*!< The CPU is ARMV6. */
    SN_CPU_ARMV7,				/*!< The CPU is ARMV7. */
    SN_CPU_ARM_NEON,			/*!< The CPU is ARM neon. */
    SN_CPU_WMMX			=100,	/*!< The CPU is WMMX. */
	SN_CPU_MAX = SN_MAX_ENUM_VALUE
} SN_CPU_VERSION;


typedef int (* SNLOG_PRINT) (void * pUserData, int nLevel, SN_TCHAR * pLogText);

/**
 * The log print call back function
 */
typedef struct{
	void *					pUserData;		/*! .he user data for call back*/
	SNLOG_PRINT				fCallBack;		/*! call back function pointer */
}SN_LOG_PRINT_CB;


/**
 * The public license information structure
 */
typedef struct{
	int		type;					/*! the license type, customer level defined in snCheck.h*/
	int		start_year;		/*! the start year of license authorization*/
	int		start_month;		/*! the start month of license authorization*/
	int		start_day;			/*! the start day of license authorization*/
	int		valid_days;		/*! the total valid days of license authorization*/
	int		left_days;			/*! the left valid days of license authorization*/
	SN_TCHAR		workpath[1024];	/*! the work path of license file*/
}SN_PUB_LCS_INFO;

typedef struct {

	void*			pData;
	int				nDataLen;
	int				nBuffLen;
} SN_BUFFER;

typedef unsigned int (*SN_Notifier) (const unsigned int event, const unsigned long p1, const unsigned long p2, void* const agent);

/*!
 * Definition of event callback
 */
typedef struct {
	void*			agent;
	SN_Notifier		notify;
}SN_LISTENER;

typedef struct {
	unsigned char* pData;
	unsigned int nSize;
	unsigned int filled;
}SN_AUDIO_DATA;

/**
*the status of the thread	
*/
typedef enum {
	SN_STAT_INITED				= 0,	/*!<The status is initialized*/
	SN_STAT_LOADING				= 1,	/*!<The status is loading */
	SN_STAT_RUNNING				= 2,	/*!<The status is running */
	SN_STAT_SEEKING				= 3,	/*!<The status is seeking */
	SN_STAT_PAUSED				= 4,	/*!<The status is paused */
	SN_STAT_STOPPED				= 5,	/*!<The status is stopped */
	SN_STAT_SUSPENDED		    = 6,	/*!<The status is suspended */
} SN_STATUS;

#define SN_PI 3.14159265359f
#define SN_RAD_UNIT (SN_PI / 180)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __snType_H__

