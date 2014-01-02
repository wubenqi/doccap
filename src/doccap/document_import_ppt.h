// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCUMENT_IMPORT_PPT_H_
#define DOCCAP_DOCUMENT_IMPORT_PPT_H_
#pragma once

#include "doccap/document_import_base.h"

class DocumentImportPPT :
	public DocumentImportBase {
public:
	explicit DocumentImportPPT(const char* file_path);
	virtual ~DocumentImportPPT(void);

	virtual BOOL ImportDocument(void);

private:
	BOOL Destroy(void);
	BOOL CheckPPTVersion();
	int  StartApplication();
	int  OpenFile();
	int  Print();
	int  CloseFile();
	int  EndApplication();

	BOOL GetObjectIntProperty(int* iVal, DISPID dispId, IDispatch* pIDispatch);

	CComBSTR	m_bstrFilename;
	CComBSTR	m_bstrPrinterName;
	CComBSTR	m_bstrDefPrinter;
	IDispatch*	m_pIDispApplication;
	IDispatch*	m_pIDispPresentations;
	IDispatch*	m_pIDispPresentation;
	IDispatch*	m_pIDispPrintOptions;

	int m_majorVer;
	int m_minorVer;

	int flag_;
};

#endif
