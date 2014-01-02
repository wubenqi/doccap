// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCUMENT_IMPORT_EXCEL_H_
#define DOCCAP_DOCUMENT_IMPORT_EXCEL_H_
#pragma once

#include "doccap/document_import_base.h"
#include "doccap/office_helper.h"

class DocumentImportExcel :
	public DocumentImportBase {
public:
	explicit DocumentImportExcel(const char* file_path);
	virtual ~DocumentImportExcel(void);

	virtual BOOL ImportDocument(void);

private:
	BOOL Destroy(void);
	IDispatch* ExcelWorkbooksOpenFile(DocVersionStruct& struDocVersion,
		BSTR bstrFileName, IDispatch* pIDispatchWorkbooks);
	BOOL ExcelWorkbookPrintOut(DocVersionStruct& struDocVersion,
		BSTR bstrPrinter, IDispatch* pIDispatchWorkbook);
	BOOL ExcelWorkbookClose(IDispatch* pIDispatchWorkbook);
	BOOL ExcelQuitApplication(IDispatch* pIDispatchApp);
	BOOL ExcelPrintFile(BSTR bstrFileName, BSTR bstrPrinterName);

	CComBSTR	m_bstrFilename;
	CComBSTR	m_bstrPrinterName;

	IDispatch* pIDispatchApp_;
	IDispatch* pIDispatchWorkbooks_;
	IDispatch* pIDispatchWorkbook_ ;
	IDispatch* pIDispatchWorksheet_;

};

#endif

