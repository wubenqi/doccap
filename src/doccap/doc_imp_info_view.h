// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOC_IMPL_INFO_VIEW_H_
#define DOCCAP_DOC_IMPL_INFO_VIEW_H_
#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlctrls.h>

#include "resource.h"

#define		WM_DOCIMPINFOMSG		WM_USER+20001

class DOCImpInfoView :
	public CDialogImpl<DOCImpInfoView> {
public:
	DOCImpInfoView(void);
	virtual ~DOCImpInfoView(void);

	enum { IDD = IDD_DOCIMPINFO_VIEW };

	BEGIN_MSG_MAP(DOCImpInfoView)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DOCIMPINFOMSG, OnDOCImpInfo)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDOCImpInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	HWND CreateUI(HWND parent_window);
	void ShowDOCImpInfo(BOOL flag = TRUE);

private:
	HWND	parent_window_;
	CProgressBarCtrl	progress_bar_ctrl_;
	CStatic				doc_info_static_;

	int page_size_;
	int pos_;
};

#endif
