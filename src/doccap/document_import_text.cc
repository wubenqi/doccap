// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/document_import_text.h"

DocumentImportText::DocumentImportText(const char* file_path)
	: DocumentImportBase(file_path) {
}

BOOL DocumentImportText::ImportDocument(void) {
	std::string strParam = "/p \"";
	strParam += file_path_;
	strParam += "\"";

	if((int)ShellExecute(NULL, "open", "wordpad.exe", _T(strParam.c_str()), NULL, SW_SHOW) <=32) {
		return FALSE;
	}
	//::WaitForSingleObject(g_hDOCCapEvent, INFINITE);
	//::ResetEvent(g_hDOCCapEvent);
	return TRUE;
}

