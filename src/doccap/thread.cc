// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//


#include "thread.h"

#include <process.h>
#include <atlbase.h>

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif

#ifndef VERIFY
#define VERIFY(f) (void(f))
#endif

#ifndef TRACE
#define TRACE ATLTRACE
#endif

#define WM_KICKIDLE	0x036A

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
struct _AFX_THREAD_STARTUP {
	// following are "in" parameters to thread startup
//	_AFX_THREAD_STATE* pThreadState;    // thread state of parent thread
	CThread* pThread;    // CThread for new thread
	DWORD dwCreateFlags;    // thread creation flags
//	_PNH pfnNewHandler;     // new handler for new thread

	HANDLE hEvent;          // event triggered after success/non-success
	HANDLE hEvent2;         // event triggered after thread is resumed

	// strictly "out" -- set after hEvent is triggered
	BOOL bError;    // TRUE if error during startup
};

void WinThreadProc(  LPVOID pParam ) { // thread data 

	_AFX_THREAD_STARTUP* pStartup = (_AFX_THREAD_STARTUP*)pParam;
	ASSERT(pStartup != NULL);
	ASSERT(pStartup->pThread != NULL);
	ASSERT(pStartup->hEvent != NULL);
	ASSERT(!pStartup->bError);
	CThread* pThread = pStartup->pThread;

	// pStartup is invaid after the following SetEvent (but hEvent2 is valid)
	HANDLE hEvent2 = pStartup->hEvent2;

	// Get Current thread ID
	pThread->m_nThreadID = ::GetCurrentThreadId();
	// allow the creating thread to return from CThread::CreateThread
	VERIFY(::SetEvent(pStartup->hEvent));

	// wait for thread to be resumed
	VERIFY(::WaitForSingleObject(hEvent2, INFINITE) == WAIT_OBJECT_0);
	::CloseHandle(hEvent2);
	DWORD nResult=0;
	if (!pThread->InitInstance())
	{
		ASSERT(pThread);
	}
	else
	{
		// will stop after PostQuitMessage called
		ASSERT(pThread);
		nResult = pThread->Run();
	}
	nResult = pThread->ExitInstance();
	if (pThread != NULL)
	{
		pThread->Delete();
		pThread  = NULL;
	}
	else
		TRACE("Fail to Delete itself int CThread class");
	_endthread();
//	return ;
}

BOOL CThread::CreateThread(DWORD dwCreateFlags, UINT nStackSize,
		LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
#ifndef _MT
	dwCreateFlags;
	nStackSize;
	lpSecurityAttrs;

	return FALSE;
#else
	ASSERT(m_hThread == NULL);  // already created?

	// setup startup structure for thread initialization
	_AFX_THREAD_STARTUP startup; 
	memset(&startup, 0, sizeof(startup));
	startup.pThread = this;
	startup.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	startup.hEvent2 = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	startup.dwCreateFlags = dwCreateFlags;
	if (startup.hEvent == NULL || startup.hEvent2 == NULL)
	{
		TRACE("Warning: CreateEvent failed in CThread::CreateThread.\n");
		if (startup.hEvent != NULL)
			::CloseHandle(startup.hEvent);
		if (startup.hEvent2 != NULL)
			::CloseHandle(startup.hEvent2);
		return FALSE;
	}

	// create the thread (it may or may not start to run)

//	m_hThread = (HANDLE)::_beginthreadex(NULL, nStackSize,
//		WinThreadProc, &startup, dwCreateFlags, (UINT*)&m_nThreadID);

	m_hThread = (HANDLE)::_beginthread(WinThreadProc, nStackSize, &startup);

	if (m_hThread == NULL)
		return FALSE;

	// start the thread just for MFC initialization
//#if(WINVER >= 0x0400)
//	VERIFY(ResumeThread() != (DWORD)-1);
//#endif
	VERIFY(::WaitForSingleObject(startup.hEvent, INFINITE) == WAIT_OBJECT_0);
	::CloseHandle(startup.hEvent);

	// if created suspended, suspend it until resume thread wakes it up
	if (dwCreateFlags & CREATE_SUSPENDED)
		VERIFY(::SuspendThread(m_hThread) != (DWORD)-1);

	// if error during startup, shut things down
	if (startup.bError)
	{
		VERIFY(::WaitForSingleObject(m_hThread, INFINITE) == WAIT_OBJECT_0);
		::CloseHandle(m_hThread);
		m_hThread = NULL;
		::CloseHandle(startup.hEvent2);
		return FALSE;
	}

	// allow thread to continue, once resumed (it may already be resumed)
	::SetEvent(startup.hEvent2);
	return TRUE;
#endif //!_MT
}

CThread::CThread()
{
	m_IdleID=0;
	m_bDone=FALSE;
	m_pThreadParams = NULL;

	// no HTHREAD until it is created
	m_hThread = NULL;
	m_nThreadID = 0;

	// initialize message pump
	m_msgCur.message = WM_NULL;
	m_nMsgLast = WM_NULL;

	// most threads are deleted when not needed
	m_bAutoDelete = TRUE;
//	memset((void*)&m_msgCur,0,sizeof(MSG));
	// initialize OLE state
}

CThread::~CThread()
{
	// free thread object

}

void CThread::Delete()
{
	// delete thread if it is auto-deleting
//	if (m_hThread != NULL)
//	{
//		CloseHandle(m_hThread);
//		m_hThread=NULL;
//	}
	if (m_bAutoDelete)
		delete this;
}

BOOL CThread::InitInstance()
{
	ASSERT(this);

	return TRUE;   // by default don't enter run loop
}

// main running routine until thread exits
int CThread::Run()
{
	ASSERT(this);

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT
			if (!PumpMessage())
				return ExitInstance();

			// reset "no idle" state after pumping "normal" message
			if (IsIdleMessage(&m_msgCur))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

		} while (::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE));
	}

	ASSERT(FALSE);  // not reachable
}

BOOL CThread::IsIdleMessage(MSG* pMsg)
{
	// Return FALSE if the message just dispatched should _not_
	// cause OnIdle to be run.  Messages which do not usually
	// affect the state of the user interface and happen very
	// often are checked for. INVALID_SOCKET

	// redundant WM_MOUSEMOVE and WM_NCMOUSEMOVE
	if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE)
	{
		// mouse move at same position as last mouse move?
		m_nMsgLast = pMsg->message;
		return TRUE;
	}

	// WM_PAINT and WM_SYSTIMER (caret blink)
	return pMsg->message != WM_PAINT && pMsg->message != 0x0118;
}

int CThread::ExitInstance()
{
	ASSERT(this);
	int nResult = 0;  // returns the value from PostQuitMessage
	nResult = m_msgCur.wParam;
	return nResult;
}

BOOL CThread::OnIdle(LONG lCount)
{
	ASSERT(this);
	if (lCount <= 0)
	{
	}
	else if (lCount >= 0)
	{
	}

	return lCount < 0;  // nothing more to do if lCount >= 0
}

void CThread::DispatchThreadMessage(MSG* pMsg)
{
	DispatchThreadMessageEx(pMsg);
}

BOOL CThread::DispatchThreadMessageEx(MSG* pMsg)
{
	return TRUE;
}

BOOL CThread::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(this);

	// if this is a thread-message, short-circuit this function
	if (pMsg->hwnd == NULL && DispatchThreadMessageEx(pMsg))
		return TRUE;

	// walk from target to main window

	return FALSE;   // no special processing
}
/////////////////////////////////////////////////////////////////////////////
// CThread implementation helpers

BOOL CThread::PumpMessage()
{
	ASSERT(this);

	if (!::GetMessage(&m_msgCur, NULL, NULL, NULL))
	{
		return FALSE;
	}

	// process this message

	if (m_msgCur.message != WM_KICKIDLE && !PreTranslateMessage(&m_msgCur))
	{
		::TranslateMessage(&m_msgCur);
		::DispatchMessage(&m_msgCur);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
CThread ::operator HANDLE() const
{ return this == NULL ? NULL : m_hThread; 
}

BOOL CThread::SetThreadPriority(int nPriority)
 {
	 ASSERT(m_hThread != NULL); 
	 return ::SetThreadPriority(m_hThread, nPriority); 
 }
int CThread::GetThreadPriority()
{ 
	ASSERT(m_hThread != NULL); 
	return ::GetThreadPriority(m_hThread); 
}

DWORD CThread::ResumeThread()
{ 
	ASSERT(m_hThread != NULL); 
	return ::ResumeThread(m_hThread); 
}

DWORD CThread::SuspendThread()
{ 
	ASSERT(m_hThread != NULL); 
	return ::SuspendThread(m_hThread);
}

BOOL CThread::PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam)
{ 
	ASSERT(m_hThread != NULL); 
	return ::PostThreadMessage(m_nThreadID, message, wParam, lParam); 
}

int CThread::Refresh(DWORD m_nThreadID)
{
	if(m_nThreadID)
		::PostThreadMessage(m_nThreadID,WM_REFRESH,0,0);
	else
		::PostThreadMessage(m_IdleID,WM_REFRESH,0,0);
	return 0;
}
