#ifndef __DFCOMMON_H__
#define __DFCOMMON_H__

#pragma once

#pragma comment(linker, "/subsystem:Windows,5")
#pragma comment(linker, "/version:5")

#define SPIN_COUNT		5000


// UNICODE Related.
#ifndef _M_IX86
#define UNICODE
#endif

//#define UNICODE		// if not want UNICODE, Comment it.

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

#define _WIN32_WINNT	0x0500
#ifdef WINVER
#undef WINVER
#define WINVER			0x0500
#endif

#pragma warning(push, 4)
// Tae-Hyuk
#define _WINSOCKAPI_

#include <windows.h>
#include <process.h>    

#ifdef _X86_
#define DebugBreak() _asm { int 3 }
#endif

typedef unsigned (__stdcall *PTHREAD_START)(void*);

#define BEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadId) \
	((HANDLE)_beginthreadex((void*)(psa), \
	(unsigned)(cbStack), (PTHREAD_START)(pfnStartAddr),\
	(void*)(pvParam), (unsigned)(fdwCreate), (unsigned*)(pdwThreadId)))

inline void DFMB(PCSTR s)
{
	char sz[256];
	GetModuleFileNameA(NULL, sz, sizeof(sz)/sizeof(sz[0]));
	HWND hWnd = GetActiveWindow();
#ifdef _MFC_USED
	MessageBoxA(hWnd, s, sz, MB_OK);
#else
	MessageBoxA(hWnd, s, sz, MB_OK | ((hWnd == NULL) ? MB_SERVICE_NOTIFICATION : 0));
#endif
}


inline void DFMBANDDEBUG(PSTR szMsg)
{
	DFMB(szMsg);
	DebugBreak();
}

inline void DFASSERTFAIL(LPCSTR file, int line, PCSTR expr)
{
	char sz[256];
	wsprintfA(sz, "File %s, line %d : %s", file, line, expr);
	DFMBANDDEBUG(sz);
}

#ifdef _DEBUG
#define DFASSERT(x) if(!(x)) DFASSERTFAIL(__FILE__, __LINE__, #x)
#else
#define DFASSERT(x)
#endif

#ifdef _DEBUG
#define DFFAIL() DFASSERTFAIL(__FILE__, __LINE__, "")
#else
#define DFFAIL()
#endif

#ifdef _DEBUG
#define DFVERIFY(x) DFASSERT(x)
#else
#define DFVERIFY(x) (x)
#endif

#ifdef _DEBUG
#define DFVERIFYNOT(x) DFASSERT(!x)
#else
#define DFVERIFYNOT(x) (x)
#endif

class DFSystemInfo : public SYSTEM_INFO
{
public:
	DFSystemInfo() {GetSystemInfo(this);};
};

#endif
