// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/doc_imp_info_view.h"

HWND g_DOCImplInfoWnd = NULL;

#define		DOCIMPINFO_BEGIN		0
#define		DOCIMPINFO_IMPORT		1
#define		DOCIMPINFO_PROCESS		2
#define		DOCIMPINFO_PROCESSING	3
#define		DOCIMPINFO_END			4


DOCImpInfoView::DOCImpInfoView(void) :
	parent_window_(NULL),
	page_size_(0),
	pos_(0) {
}

DOCImpInfoView::~DOCImpInfoView(void) {
}

HWND DOCImpInfoView::CreateUI(HWND parent_window) {
	parent_window_ = parent_window;
	return Create(parent_window);
}

LRESULT DOCImpInfoView::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	CenterWindow();

	g_DOCImplInfoWnd = m_hWnd;
	doc_info_static_ = GetDlgItem(IDC_DOCINFO);
	doc_info_static_.SetWindowText("开始文档转换，请耐心等待......");
	progress_bar_ctrl_ = GetDlgItem(IDC_DOCINFOPROGRESS);
	progress_bar_ctrl_.SetRange ( 0, 100 );
	return TRUE;
}

//#define		DOCIMPINFO_START		0
//#define		DOCIMPINFO_OPEN			1
//#define		DOCIMPINFO_IMPORT		2
//#define		DOCIMPINFO_PROCESS		3
//#define		DOCIMPINFO_STOP			4

LRESULT DOCImpInfoView::OnDOCImpInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	char* info = NULL;
	switch(wParam) {
	case DOCIMPINFO_BEGIN:
		doc_info_static_.SetWindowText("开始文档转换，请耐心等待......");
		break;
	case DOCIMPINFO_IMPORT:
		page_size_++;
		info = (char*)lParam;
		doc_info_static_.SetWindowText(info);
		delete info;
		break;
	case DOCIMPINFO_PROCESS:
		//info = (char*)lParam;
		//doc_info_static_.SetWindowText(info);
		//delete info;
		//page_size_
		if(page_size_!=0) {
			pos_ += 100/page_size_;
			// TRACE("pos = %d\n", pos_);
			progress_bar_ctrl_.SetPos(pos_);
		}
		break;
	case DOCIMPINFO_END:
		progress_bar_ctrl_.SetPos(100);
		DestroyWindow();
		PostQuitMessage(0);
		//doc_info_static_.SetWindowText("文档转换结束");
		break;
	default:
		break;
	}
	return 0;
}

LRESULT DOCImpInfoView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	g_DOCImplInfoWnd = NULL;
	return 0;
}

void DOCImpInfoView::ShowDOCImpInfo(BOOL flag)
{
	if(!IsWindow())
		return;
	if(flag) {
		doc_info_static_.SetWindowText("开始文档转换，请耐心等待......");
		progress_bar_ctrl_.SetPos(0);
		ShowWindow(SW_SHOW);
	} else {
		ShowWindow(SW_HIDE);
	}
}
