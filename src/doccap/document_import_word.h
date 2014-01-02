// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCUMENT_IMPORT_WORD_H_
#define DOCCAP_DOCUMENT_IMPORT_WORD_H_
#pragma once

#include "doccap/document_import_base.h"

class DocumentImportWord :
	public DocumentImportBase {
public:
	explicit DocumentImportWord(const char* file_path);
	virtual ~DocumentImportWord(void);

	virtual BOOL ImportDocument(void);


private:
	BOOL Destroy();

	BOOL WordPrintFile(BSTR bstrFileName, BSTR bstrPrinterName);
	BOOL WordDocumentPrintOut(IDispatch* pIDispatchDocument);
	BOOL WordDocumentClose(IDispatch* pIDispatchDocument);
	BOOL WordQuitApplication(IDispatch* pIDispatchApp);
	IDispatch* WordOpenFile(BSTR bstrFileName, IDispatch* pIDispatchDocs);

	CComBSTR	m_bstrFilename;
	CComBSTR	m_bstrPrinterName;

	IDispatch* pIDispatchWordApplication_;
	IDispatch* pIDispatchDocs_;
	IDispatch* pIDispatchDocument_;

};

#endif

