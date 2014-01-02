// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/document_import_ppt.h"

#include "doccap/doccap_driver.h"
#include "doccap/office_helper.h"

#define	VT_BOOL_TRUE	((unsigned short)0xffff)
#define VT_BOOL_FALSE   0

DocumentImportPPT::DocumentImportPPT(const char* file_path)
	: DocumentImportBase(file_path) {
	m_bstrFilename.Append(file_path);
	m_bstrPrinterName.Append(DocCapDriver::kDocCapPrinter);

	m_pIDispApplication		= NULL;
	m_pIDispPresentations	= NULL;
	m_pIDispPresentation	= NULL;
	m_pIDispPrintOptions	= NULL;
	m_majorVer=0;
	m_minorVer=0;
	flag_ = -1;

}

DocumentImportPPT::~DocumentImportPPT()
{
	if( m_pIDispPrintOptions )
	{
		m_pIDispPrintOptions->Release();
		m_pIDispPrintOptions = NULL;
	}

	if( m_pIDispPresentation )
	{
		m_pIDispPresentation->Release();
		m_pIDispPresentation = NULL;
	}

	if( m_pIDispPresentations )
	{
		m_pIDispPresentations->Release();
		m_pIDispPresentations = NULL;
	}

	if( m_pIDispApplication )
	{
		m_pIDispApplication->Release();
		m_pIDispApplication = NULL;
	}
}


BOOL DocumentImportPPT::ImportDocument(void)
{
	BOOL bRet = FALSE;
	if( StartApplication() == 0 )
	{
		if( OpenFile() == 0 )
		{
			if( Print() == 0 )
			{
				bRet = TRUE;
				flag_ = 0;
			}
			else
			{
				CloseFile();
				EndApplication();
			}
			//::WaitForSingleObject(g_hDOCCapEvent, INFINITE);
			//ResetEvent(g_hDOCCapEvent);
			//CloseFile();
		}
		else
		{
			EndApplication();
		}
		//flag_ = 1;
		//EndApplication();
	}
	return bRet;
}

int DocumentImportPPT::StartApplication()
{
	CLSID clsId;
	if(CLSIDFromProgID(L"PowerPoint.Application", &clsId) != S_OK)
	{
		return	1;
	}

	HRESULT	hResult;
	m_pIDispApplication = NULL;

	CoInitialize(NULL);

	hResult = CoCreateInstance(clsId, NULL, CLSCTX_SERVER,
		IID_IDispatch, (void**)&m_pIDispApplication);

	if( !m_pIDispApplication )
	{
		CoUninitialize();
		return	2;
	}

	// Make application visible
	/*	DISPID	DispIDVisible = 0;
	GetDispIDFromName(m_pIDispApplication, "Visible",
	&DispIDVisible);

	VARIANT	varVisible;
	varVisible.vt = VT_BOOL;
	varVisible.boolVal = VT_BOOL_TRUE;
	SetObjectProperty(varVisible, DispIDVisible,
	m_pIDispApplication);

	// Minimize application window
	DISPID	DispIDWindowState;
	GetDispIDFromName(m_pIDispApplication, "WindowState",
	&DispIDWindowState);

	VARIANT	varWindowState;
	varWindowState.vt = VT_I4;
	varWindowState.lVal = 2;
	SetObjectProperty(varWindowState, DispIDWindowState,
	m_pIDispApplication);
	*/

	// Get Version String
	DISPID	DispIDVersion = 0;
	GetDispIDFromName(m_pIDispApplication, "Version",
		&DispIDVersion);

	CComBSTR bstrVersion;
	GetObjectBStrProperty(bstrVersion, DispIDVersion,
		m_pIDispApplication);

	USES_CONVERSION;
	int	majorVer = 0;
	int	minorVer = 0;
	ImpGetVersionNumberFromString(OLE2A(bstrVersion),
		&majorVer, &minorVer);
	m_majorVer=majorVer;
	m_minorVer=minorVer;

	if( majorVer<8 )
	{
		//Office 95 or earlier version
		m_pIDispApplication->Release();
		m_pIDispApplication = NULL;

		CoUninitialize();

		return	3;
	}

	return	0;
}

int DocumentImportPPT::OpenFile()
{

	if( !m_pIDispApplication )
	{
		_ASSERTE(FALSE);
		return	1;
	}

	DISPID DispIDPresentations = 0;
	GetDispIDFromName(m_pIDispApplication, "Presentations",
		&DispIDPresentations);
	m_pIDispPresentations = GetObjectProperty(DispIDPresentations,
		m_pIDispApplication);
	if( !m_pIDispPresentations )
	{
		_ASSERTE(FALSE);
		return	2;
	}

	VARIANT var;
	VARIANT vt[4];
	DISPPARAMS dispArgs = {vt, NULL, 4, 0};

	for(int i = 0; i < 4; i++)
	{
		vt[i].vt = VT_ERROR;
		vt[i].scode = DISP_E_PARAMNOTFOUND;
	}

	if( m_majorVer >= 8 )
	{
		vt[3].vt = VT_BSTR;
		vt[3].bstrVal = m_bstrFilename;
		vt[2].vt = VT_BOOL;
		vt[2].boolVal = VT_BOOL_TRUE;

		//If we hide the power point, we should call following sentence
		vt[0].vt = VT_BOOL;
		vt[0].boolVal = VT_BOOL_FALSE;
		//
	}
	else
	{
		vt[0].vt = VT_I4;
		vt[0].lVal = 1;
		vt[1].vt = VT_I4;
		vt[1].lVal = 0;
		vt[2].vt = VT_I4;
		vt[2].lVal = 1;
		vt[3].vt = VT_BSTR;
		vt[3].bstrVal = m_bstrFilename;
	}

	DISPID	DispIDOpen=0;
	GetDispIDFromName(m_pIDispPresentations, _T("Open"),
		&DispIDOpen);

	HRESULT hr = m_pIDispPresentations->Invoke(
		DispIDOpen,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		&var, 
		NULL, 
		NULL);

	if( hr != S_OK )
	{
		_ASSERTE(FALSE);
		return	2;
	}

	if( var.vt != VT_DISPATCH )
	{
		_ASSERTE(FALSE);
		return	3;
	}

	m_pIDispPresentation = var.pdispVal;

	return	0;
}

int DocumentImportPPT::Print()
{

	if( !m_pIDispApplication || !m_pIDispPresentations ||
		!m_pIDispPresentation )
	{
		_ASSERTE(FALSE);
		return	1;
	}


	DISPID DispIDPrintOptions = 0;
	GetDispIDFromName(m_pIDispPresentation, "PrintOptions",
		&DispIDPrintOptions);
	m_pIDispPrintOptions = GetObjectProperty(DispIDPrintOptions,
		m_pIDispPresentation);

	if( !m_pIDispPrintOptions )
	{
		_ASSERTE(FALSE);
		return	1;
	}

	VARIANT	varOptions;
	if( m_majorVer >= 8 )
	{
		// Get default printer name
		/*		DISPID	DispIDActivePrinter = 0;
		GetDispIDFromName(m_pIDispPrintOptions, "ActivePrinter",
		&DispIDActivePrinter);
		GetObjectBStrProperty(m_bstrDefPrinter, DispIDActivePrinter,
		m_pIDispPrintOptions);

		// Set new printer
		varOptions.vt = VT_BSTR;
		varOptions.bstrVal = m_bstrPrinterName;
		SetObjectProperty(varOptions, DispIDActivePrinter,
		m_pIDispPrintOptions);
		*/
	}

	if( m_majorVer >= 8 )
	{
		// Enable background print
		DISPID	DispIDBackgroundPrint = 0;
		GetDispIDFromName(m_pIDispPrintOptions, "PrintInBackground",
			&DispIDBackgroundPrint);
		varOptions.vt = VT_I4;
		varOptions.lVal = 1;
		SetObjectProperty(varOptions, DispIDBackgroundPrint,
			m_pIDispPrintOptions);
	}

	if( m_majorVer >= 8 )
	{
		// Color output
		DISPID	DispIDPrintColorType = 0;
		GetDispIDFromName(m_pIDispPrintOptions, "PrintColorType",
			&DispIDPrintColorType);
		varOptions.vt = VT_I4;
		varOptions.lVal = 1;
		SetObjectProperty(varOptions, DispIDPrintColorType,
			m_pIDispPrintOptions);
	}
	else
	{
		// Color output
		DISPID	DispIDBlackAndWhite = 0;
		GetDispIDFromName(m_pIDispPrintOptions, "BlackAndWhite",
			&DispIDBlackAndWhite);
		varOptions.vt = VT_I4;
		varOptions.lVal = 0;
		SetObjectProperty(varOptions, DispIDBlackAndWhite,
			m_pIDispPrintOptions);
	}

	// Output type
	DISPID	DispIDOutputType = 0;
	GetDispIDFromName(m_pIDispPrintOptions, "OutputType",
		&DispIDOutputType);
	varOptions.vt = VT_I4;
	varOptions.lVal = 1;
	SetObjectProperty(varOptions, DispIDOutputType,
		m_pIDispPrintOptions);

	if( m_majorVer >= 8 )
	{
		// Fit to page
		DISPID	DispIDFitToPage = 0;
		GetDispIDFromName(m_pIDispPrintOptions, "FitToPage",
			&DispIDFitToPage);
		varOptions.vt = VT_I4;
		varOptions.lVal = 1;
		SetObjectProperty(varOptions, DispIDFitToPage,
			m_pIDispPrintOptions);
	}
	else
	{
		// Fit to page
		DISPID	DispIDScaleToFit = 0;
		GetDispIDFromName(m_pIDispPrintOptions, "ScaleToFit",
			&DispIDScaleToFit);
		varOptions.vt = VT_I4;
		varOptions.lVal = 1;
		SetObjectProperty(varOptions, DispIDScaleToFit,
			m_pIDispPrintOptions);
	}

	if( m_majorVer >= 8 )
	{
		// Print out
		VARIANT var;
		DISPPARAMS dispArgs = {NULL, NULL, 0, 0};

		DISPID	DispIDPrintOut = 0;
		GetDispIDFromName(m_pIDispPresentation, _T("PrintOut"),
			&DispIDPrintOut);

		HRESULT hr = m_pIDispPresentation->Invoke(
			DispIDPrintOut,
			IID_NULL,
			LOCALE_USER_DEFAULT, 
			DISPATCH_METHOD,
			&dispArgs, 
			&var,
			NULL,
			NULL);
		if( hr != S_OK )
			return	2;
	}
	else
	{
		VARIANT	var;
		VARIANT vt;
		DISPPARAMS dispArgs = {&vt, NULL, 1, 0};

		DISPID	DispIDPrint = 0;
		GetDispIDFromName(m_pIDispPresentation, _T("Print"),
			&DispIDPrint);
		vt.vt = VT_I4;
		vt.lVal = 1;

		HRESULT	hr = m_pIDispPresentation->Invoke(
			DispIDPrint,
			IID_NULL,
			LOCALE_USER_DEFAULT, 
			DISPATCH_METHOD,
			&dispArgs, 
			&var,
			NULL,
			NULL);
		if( hr != S_OK )
			return	2;
	}

	return	0;
}

int DocumentImportPPT::CloseFile()
{

	if( !m_pIDispApplication || !m_pIDispPresentations ||
		!m_pIDispPresentation || !m_pIDispPrintOptions )
	{
		_ASSERTE(FALSE);
		return	1;
	}

	// Set back default printer
	/*	DISPID	DispIDActivePrinter = 0;
	VARIANT varOptions;
	GetDispIDFromName(m_pIDispPrintOptions, "ActivePrinter",
	&DispIDActivePrinter);
	varOptions.vt = VT_BSTR;
	varOptions.bstrVal = m_bstrDefPrinter;
	SetObjectProperty(varOptions, DispIDActivePrinter,
	m_pIDispPrintOptions);
	*/
	// Release DispPrintOptions
	m_pIDispPrintOptions->Release();
	m_pIDispPrintOptions = NULL;

	// Close Presentation
	VARIANT var;
	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};

	DISPID	DispIDClose=0;
	GetDispIDFromName(m_pIDispPresentation, _T("Close"),
		&DispIDClose);

	HRESULT hr = m_pIDispPresentation->Invoke(
		DispIDClose,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		&var, 
		NULL, 
		NULL);

	if( hr != S_OK )
	{
		_ASSERTE(FALSE);
	}

	// Release Presentation
	//	m_pIDispPresentation->Release();
	//	m_pIDispPresentation = NULL;

	return	0;
}

int DocumentImportPPT::EndApplication()
{

	if( !m_pIDispApplication || !m_pIDispPresentations )
	{
		_ASSERTE(FALSE);
		return	1;
	}

	// Get how many presentation still opened
	int		nPresentationCount = 0;
	DISPID	DispIDPresentationCount = 0;
	GetDispIDFromName(m_pIDispPresentations, "Count",
		&DispIDPresentationCount);
	GetObjectIntProperty(&nPresentationCount, DispIDPresentationCount,
		m_pIDispPresentations);

	// Release Presentations
	m_pIDispPresentations->Release();
	m_pIDispPresentations = NULL;

	if( nPresentationCount==0 )
	{
		VARIANT	var;
		DISPPARAMS dispArgs = {NULL, NULL, 0, 0};

		DISPID	DispIDQuit = 0;
		GetDispIDFromName(m_pIDispApplication, _T("Quit"),
			&DispIDQuit);

		HRESULT hr = m_pIDispApplication->Invoke(
			DispIDQuit,
			IID_NULL,
			LOCALE_USER_DEFAULT, 
			DISPATCH_METHOD,
			&dispArgs, 
			&var, 
			NULL, 
			NULL);

		if( hr != S_OK )
		{
			_ASSERTE(FALSE);
		}
	}

	// Release application
	m_pIDispApplication->Release();
	m_pIDispApplication = NULL;

	CoUninitialize();
	return	0;
}

BOOL DocumentImportPPT::GetObjectIntProperty(int* iVal, DISPID dispId, IDispatch* pIDispatch)
{
	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};
	VARIANT var;

	HRESULT hr = pIDispatch->Invoke(
		dispId,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_PROPERTYGET,
		&dispArgs,
		&var,
		NULL,
		NULL);

	if(hr == S_OK)
	{
		_ASSERTE(var.vt==VT_I4);
		if( var.vt==VT_I4)
		{
			*iVal = var.intVal;
			return	TRUE;
		}
	}

	return	FALSE;
}

BOOL DocumentImportPPT::Destroy(void)
{
	if(flag_ == 0)
		CloseFile();
	EndApplication();
	return TRUE;
}
