// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOC_IMPL_INFO_THREAD_H_
#define DOCCAP_DOC_IMPL_INFO_THREAD_H_
#pragma once

#include <string>
#include "doccap/document_import.h"
#include "doccap/thread.h"

extern HWND g_DOCImplInfoWnd;

class DOCImpInfoView;
class DOCImplInfoThread :
	public CThread
{
public:
	DOCImplInfoThread(HWND hParent, LPCTSTR lpszFilePath, LPCTSTR lpszExtName);
	virtual ~DOCImplInfoThread(void);

	HWND				parentWnd_;
	std::string			filePath_, extName_;
	DOCImpInfoView*	DOCImpInfoView_;
	DocumentImport*	docImp_;//(lpszFilePath, lpszExtName);
	
	bool  isFind_;
	std::string			default_printer_;

protected:
	BOOL InitInstance();
	int ExitInstance();

};

#endif

