// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/document_import_normal.h"

#include "doccap/doccap_driver.h"

extern HWND g_hDOCCapWnd;

DocumentImportNormal::DocumentImportNormal(const char* file_path)
	: DocumentImportBase(file_path) {
}

DocumentImportNormal::~DocumentImportNormal(void) {
}

BOOL DocumentImportNormal::ImportDocument(void) {
	if((int)ShellExecute(NULL, _T("print"), _T(file_path_.c_str()), NULL, NULL, SW_SHOW) <=32) {
		std::string str = "很抱歉，文档转换器不支持您所选的文件格式！\n您是否要打开 (";
		str += file_path_;
		str += ") 通过打印进行文档转换？";
		if (::MessageBox(g_hDOCCapWnd, str.c_str(), "文档转换器", MB_OKCANCEL)  == IDCANCEL) {
			return FALSE;
		} else {
			ShellExecute(NULL, _T("open"), _T(file_path_.c_str()), NULL, NULL, SW_SHOW);
		}
	}
	//else
	//{
	//	::WaitForSingleObject(g_hDOCCapEvent, INFINITE);
	//	::ResetEvent(g_hDOCCapEvent);
	//}
	return TRUE;
}
