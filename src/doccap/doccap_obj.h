// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCCAP_OBJ_H_
#define DOCCAP_DOCCAP_OBJ_H_
#pragma once

#include "doccap/doccap_api.h"

#include "doccap/doccap_help_window.h"
#include "doccap/doc_imp_info_view.h"

class SpoolThread;
class CDOCCapObj :
	public IDOCCapBaseObj {
public:
	CDOCCapObj();
	virtual ~CDOCCapObj();

public:
	BOOL Create();
	void OnCreate(HWND hWnd);
	void Destroy();
	HANDLE m_hExisted;
	bool   m_isExisted;

public:
	virtual void SetSink(IDOCCapBaseObjSink* pSink) {
		m_pSink = pSink;
	}
	virtual int OpenFile(const char* file_name, const char* ext_name);
	
//for 2000
public:
	void OnDOCCapData(DOCCapData *pDOCCapData);
	void OnDestroy();

	//MESSAGE_HANDLER(WM_DOCIMPINFOMSG, OnDOCImpInfo)
	LRESULT OnDOCImpInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
public:
	BOOL	  b_MidView;
	BOOL	  b_SaveEmf;
	int		  n_delay;
	BOOL      b_showemf;
	char	  DestDir[MAX_PATH];
	
	DOCImpInfoView	docInfoView_;

public:
	IDOCCapBaseObjSink*	m_pSink;
	SpoolThread*		m_pSplThread;
	HWND	m_hWnd;
	BOOL	m_bInited;

	// int m_nOSVer;

	// CxImage m_logoImg;

};

extern CDOCCapObj	*g_DOCCapWnd;


#endif // !defined(AFX_DOCCAPOBJ_H__C6CE6BAB_B926_4AF1_9362_A6A45265418B__INCLUDED_)
