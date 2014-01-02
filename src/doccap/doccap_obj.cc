// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/doccap_obj.h"

#include "doccap/spool_thread.h"
#include "doccap/document_import.h"
#include "doccap/doc_impl_info_thread.h"

#include "doccap/doccap_driver_win2000.h"

CDOCCapObj	*g_DOCCapWnd=NULL;

bool	g_bIsFromPrintOpen = true;
HWND	g_hDOCCapWnd = NULL;

const char *LoadStringTemp(int id) {
	static char buffer[256];
	if (!LoadString(0, id, buffer, sizeof(buffer)-2))
		buffer[0] = 0;
	return buffer;
}

///////////////////////////////////////////////////////////////////////////////
//main frame proc

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled = FALSE;

	switch (message) 
	{
	case WM_CREATE:
		{
			if(g_DOCCapWnd)
				g_DOCCapWnd->OnCreate(hWnd);
		}
		break;
	case WM_MSGDOCCAPDATA:
		{
			DOCCapData* pDOCCapData = (DOCCapData* )lParam;
			if(g_DOCCapWnd)
			{
				g_DOCCapWnd->OnDOCCapData(pDOCCapData);
				ATLTRACE("doc size: %d \n", pDOCCapData->emfs.size());	
			}
			delete pDOCCapData;
		}
		break;
	case WM_DOCIMPINFOMSG:
		{
			if(g_DOCCapWnd)
				g_DOCCapWnd->OnDOCImpInfo(message, wParam, lParam, bHandled);
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDOCCapObj::CDOCCapObj() : m_pSink(0), m_pSplThread(0)
{
	g_DOCCapWnd = this;
	m_bInited = FALSE;

	b_MidView = FALSE;
	b_SaveEmf = FALSE;
	b_showemf = FALSE;
	DestDir[0]='\0';

	m_isExisted = false;
	m_hExisted=CreateMutex(NULL,FALSE,"Epidiascope Printer Driver is Running");
	if(m_hExisted && GetLastError()==ERROR_ALREADY_EXISTS)
	{
		m_isExisted = true;
		//return Error(_T("对不起，你只能在一台机器上开一个控件对象"));
	}

	DocCapDriverWin2000::InitWorkDirs();


	//g_hDOCCapEvent = ::CreateEvent(0, TRUE, FALSE, 0);
/*
	std::string		etcDir = ident + "\\etc";
	// base case . . .if directory exists
*/
}

CDOCCapObj::~CDOCCapObj()
{
	//::CloseHandle(g_hDOCCapEvent);
	g_DOCCapWnd = NULL;

	m_isExisted = false;
	if(m_hExisted)
	{
		CloseHandle(m_hExisted);
	}

//	Destroy();
}

BOOL CDOCCapObj::Create()
{
	if(m_isExisted)
		return FALSE;

	HINSTANCE	hInstance;
	WNDCLASSEX wcex;

	hInstance	= ::GetModuleHandle(NULL);
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_TEST);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCSTR)IDC_TEST;
	wcex.lpszClassName	= "DOCCapObj Window";
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

	m_hWnd = CreateWindow("DOCCapObj Window", "DOCCapObj", WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	g_hDOCCapWnd = m_hWnd;
	return TRUE;
}

void CDOCCapObj::OnDOCCapData(DOCCapData *pDOCCapData)
{
	if(m_pSink)
		m_pSink->OnDOCCapData(pDOCCapData);
}

void CDOCCapObj::OnDestroy()
{
	//docInfoView_.DestroyWindow();
	g_hDOCCapWnd = NULL;
	//Destroy();
}

void CDOCCapObj::OnCreate(HWND hWnd)
{
/*
	HINSTANCE hinst=_Module.GetResourceInstance();
	HRSRC hsrc=FindResource(hinst,MAKEINTRESOURCE(IDR_LOGO1),"LOGO");
	if(hsrc) {
		m_logoImg.LoadResource(hsrc, CXIMAGE_FORMAT_PNG, hinst);
	}
*/

	m_pSplThread = new SpoolThread(DocCapDriver::kDocCapPrinter, DocCapDriverWin2000::static_dvw_spool_dir_.c_str(), DocCapDriverWin2000::static_dvw_temp_dir_.c_str(), hWnd);
	if(m_pSplThread) {
		m_pSplThread->Start();
		m_bInited = TRUE;
	}
}


void CDOCCapObj::Destroy()
{
	if(m_isExisted)
		return;

	m_bInited = FALSE;

	if(m_hWnd && ::IsWindow(m_hWnd))
		::DestroyWindow(m_hWnd);

	if(m_pSplThread )
	{
		m_pSplThread->m_bDone=TRUE;
		WaitForSingleObject(m_pSplThread->m_hThread,INFINITE);
		m_pSplThread=NULL;
	}
}


int CDOCCapObj::OpenFile(const char* file_name, const char* ext_name)
{
	if(!m_bInited)
		return -1;

	new DOCImplInfoThread(m_hWnd, file_name, ext_name);
	return 0;
}

LRESULT CDOCCapObj::OnDOCImpInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_pSink)
		m_pSink->OnDOCImpInfo(wParam, lParam);
	return 0;
}
