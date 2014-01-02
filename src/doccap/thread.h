// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//

#ifndef THREAD_H_
#define THREAD_H_
#pragma once

#include <Windows.h>

#define WM_IDLE			WM_USER+999
#define WM_BUSY			WM_USER+998
#define WM_TEXT_IDLE	WM_USER+997
#define WM_EVENT		WM_USER+995
#define WM_ONASKER		WM_USER+994
#define WM_REFRESH		WM_USER+996
#define WM_GETWINDOW	WM_USER+993
#define WM_SENDURL		WM_USER+992
#define WM_RECVURL		WM_USER+991

class CThread;
class CThread {
public:
	CThread();
	virtual ~CThread();

	BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

	operator HANDLE() const;

	int GetThreadPriority();
	BOOL SetThreadPriority(int nPriority);

// Operations
	DWORD SuspendThread();
	DWORD ResumeThread();
	BOOL PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam);

// Overridables
	// thread initialization
	virtual BOOL InitInstance();

	// running and idle processing
	virtual int Run();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PumpMessage();     // low level message pump
	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
	virtual BOOL IsIdleMessage(MSG* pMsg);  // checks for special messages

	// thread termination
	virtual int ExitInstance(); // default will 'delete this'
	virtual int Refresh(DWORD m_nThreadID=0);

	// 'delete this' only if m_bAutoDelete == TRUE
	virtual void Delete();

	// message pump for Run
	MSG m_msgCur;                   // current message
	
	DWORD m_IdleID;
	BOOL m_bDone;
	// constructor used by implementation of AfxBeginThread
	// valid after construction
	LPVOID m_pThreadParams; // generic parameters passed to starting function

	// Attributes
	BOOL m_bAutoDelete;     // enables 'delete this' after thread termination

	// only valid while running
	HANDLE m_hThread;       // this thread's HANDLE
	DWORD m_nThreadID;      // this thread's ID

protected:
	BOOL DispatchThreadMessageEx(MSG* msg);  // helper
	void DispatchThreadMessage(MSG* msg);  // obsolete

	UINT m_nMsgLast;            // last mouse message

};

#endif // !defined(AFX_UTHREAD_H__DC71A2B1_BD16_4312_A5BD_F4B33B57880E__INCLUDED_)
