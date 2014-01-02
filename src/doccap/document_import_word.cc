// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/document_import_word.h"

#include "doccap/doccap_driver.h"
#include "doccap/office_helper.h"

DocumentImportWord::DocumentImportWord(const char* file_path)
	: DocumentImportBase(file_path) {
	m_bstrFilename.Append(file_path);
	m_bstrPrinterName.Append(DocCapDriver::kDocCapPrinter);
	pIDispatchWordApplication_ = NULL;
	pIDispatchDocs_ = NULL;
	pIDispatchDocument_ = NULL;

}

DocumentImportWord::~DocumentImportWord(void)
{
	Destroy();
}

BOOL DocumentImportWord::ImportDocument(void)
{
	return WordPrintFile(m_bstrFilename, m_bstrPrinterName);
}

BOOL DocumentImportWord::WordDocumentPrintOut(IDispatch* pIDispatchDocument)
{
	/*    VARIANT vt[14];
	for(int i = 0; i < 14; i++)
	{
	vt[i].vt = VT_ERROR;
	vt[i].scode = DISP_E_PARAMNOTFOUND;
	}

	DISPPARAMS dispArgs = {vt, NULL, 14, 0};

	HRESULT hr = pIDispatchDocument->Invoke(
	0x0000006d, 
	IID_NULL,
	LOCALE_USER_DEFAULT, 
	DISPATCH_METHOD,
	&dispArgs, 
	NULL, 
	NULL, 
	NULL);

	if(hr != S_OK)
	return FALSE;
	*/

	/***************************************************/
	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};

	DISPID	DispIDPrintOut = 0;
	GetDispIDFromName(pIDispatchDocument, _T("PrintOut"),
		&DispIDPrintOut);


	HRESULT hr = pIDispatchDocument->Invoke(
		DispIDPrintOut, 
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		NULL, 
		NULL, 
		NULL);

	if(hr != S_OK)
		return FALSE;

	/***************************************************/

	return TRUE;
}

BOOL DocumentImportWord::WordDocumentClose(IDispatch* pIDispatchDocument)
{
	/*   VARIANT vt[3];
	for(int i = 0; i < 3; i++)
	{
	vt[i].vt = VT_ERROR;
	vt[i].scode = DISP_E_PARAMNOTFOUND;
	}

	DISPPARAMS dispArgs = {vt, NULL, 3, 0};

	HRESULT hr = pIDispatchDocument->Invoke(
	0x00000451,
	IID_NULL,
	LOCALE_USER_DEFAULT, 
	DISPATCH_METHOD,
	&dispArgs, 
	NULL, 
	NULL, 
	NULL);
	*/
	VARIANT var;
	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};

	DISPID	DispIDClose=0;
	GetDispIDFromName(pIDispatchDocument, _T("Close"),
		&DispIDClose);

	HRESULT hr = pIDispatchDocument->Invoke(
		DispIDClose,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		&var, 
		NULL, 
		NULL);

	if(hr != S_OK)
		return FALSE;

	return TRUE;
}

BOOL DocumentImportWord::WordQuitApplication(IDispatch* pIDispatchApp)
{
	DISPID	DispIDQuit = 0;
	GetDispIDFromName(pIDispatchApp, _T("Quit"),
		&DispIDQuit);

	DISPPARAMS dispArgs = {NULL,NULL,0,0};

	HRESULT hr;
	hr = pIDispatchApp->Invoke(
		DispIDQuit,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		NULL, 
		NULL, 
		NULL);

	if( hr != S_OK )
		return	FALSE;

	return	TRUE;

}


BOOL DocumentImportWord::WordPrintFile(BSTR bstrFileName, BSTR bstrPrinterName)
{
	//IDispatch* pIDispatchWordApplication=NULL;

	CLSID clsId;
	if(CLSIDFromProgID(L"Word.Application", &clsId) != S_OK)
		return FALSE;

	HRESULT hResult;

	CoInitialize(NULL);
	hResult = CoCreateInstance(clsId, NULL,CLSCTX_SERVER,
		IID_IDispatch, (void**)&pIDispatchWordApplication_);

	if (hResult!=S_OK)
	{
		CoUninitialize();
		return FALSE;
	}
	USES_CONVERSION;
	BOOL bRet = TRUE;

	if( pIDispatchWordApplication_ )
	{
		// Make application visible
		/*
		VARIANT	varVisible;
		varVisible.vt = VT_BOOL;
		varVisible.boolVal = TRUE;
		SetObjectProperty(varVisible, 0x00000017,
		pIDispatchWordApplication);
		*/

		// Get Version String
		CComBSTR bstrVersion;
		GetObjectBStrProperty(bstrVersion, 0x00000018,
			pIDispatchWordApplication_);

		int	majorVer;
		int	minorVer;
		ImpGetVersionNumberFromString(OLE2A(bstrVersion),
			&majorVer, &minorVer);

		// support office 97 or higher version only
		if(majorVer<8 )
		{
			pIDispatchWordApplication_->Release();
			pIDispatchWordApplication_=NULL;
			CoUninitialize();

			return	FALSE;
		}

		// Get Documents Object
		//IDispatch* pIDispatchDocs;
		pIDispatchDocs_ = GetObjectProperty(0x00000006,
			pIDispatchWordApplication_);

		if( pIDispatchDocs_ )
		{
			// Open document
			//IDispatch* pIDispatchDocument;
			pIDispatchDocument_ = WordOpenFile(bstrFileName, pIDispatchDocs_);

			if( pIDispatchDocument_ )
			{
				// Backup default printer name
				/*	CComBSTR bstrDefPrinter;
				GetObjectBStrProperty(bstrDefPrinter, 0x00000042,
				pIDispatchWordApplication);

				// Set new printer
				VARIANT	varPrinter;
				varPrinter.vt = VT_BSTR;
				varPrinter.bstrVal = bstrPrinterName;
				SetObjectProperty(varPrinter, 0x00000042,
				pIDispatchWordApplication);

				*/
				// Do print
				bRet = WordDocumentPrintOut(pIDispatchDocument_);
			}
			else
				bRet = FALSE;
		}
		//pIDispatchDocs_->Release();
	}
	else
		bRet = FALSE;
	return bRet;
}

BOOL DocumentImportWord::Destroy()
{
			// Restore default printer
				/*	varPrinter.vt = VT_BSTR;
				varPrinter.bstrVal = bstrDefPrinter;
				SetObjectProperty(varPrinter, 0x00000042,
				pIDispatchWordApplication);
				*/

				//::WaitForSingleObject(g_hDOCCapEvent, INFINITE);
				//::ResetEvent(g_hDOCCapEvent);

	if(pIDispatchDocument_)
	{
		WordDocumentClose(pIDispatchDocument_);
		pIDispatchDocument_->Release();
	}

	if(pIDispatchDocs_)
		pIDispatchDocs_->Release();
	if (pIDispatchWordApplication_!=NULL)
	{
		WordQuitApplication(pIDispatchWordApplication_);

		pIDispatchWordApplication_->Release();
		pIDispatchWordApplication_=NULL;

		CoUninitialize();
	}
	return TRUE;
}

IDispatch* DocumentImportWord::WordOpenFile(BSTR bstrFileName, IDispatch* pIDispatchDocs)
{
	VARIANT vtFileName;
	vtFileName.vt = VT_BSTR;
	vtFileName.bstrVal = bstrFileName;

	VARIANT vt[10];
	for(int i = 0; i < 10; i++)
	{
		vt[i].vt = VT_ERROR;
		vt[i].scode = DISP_E_PARAMNOTFOUND;
	}

	//vt[9].vt = VT_VARIANT;
	//vt[9].pvarVal = &vtFileName;
	vt[9].vt = VT_BSTR;
	vt[9].bstrVal = bstrFileName;

	DISPPARAMS dispArgs = {vt, NULL, 10, 0};
	VARIANT var;

	HRESULT hr = pIDispatchDocs->Invoke(
		0x0000000c,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		&var, 
		NULL, 
		NULL);

	if(hr == S_OK)
	{
		_ASSERTE(var.vt == VT_DISPATCH);
		if( var.vt == VT_DISPATCH )
			return var.pdispVal;
	}

	return	NULL;
}
