// InstallDriver.cpp : main source file for InstallDriver.exe
//

#include "stdafx.h"

#include <atlmisc.h>
#include "LoadLibrary.h"

#include "resource.h"

//#include "aboutdlg.h"
//#include "MainDlg.h"

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CLoadLibrary docCapLoad;
	if(!docCapLoad.Load(_T("doccap.dll")))
		return 0;

	if(!lpstrCmdLine || lpstrCmdLine[0]=='\0' || lstrcmpi(lpstrCmdLine, "install")==0)
	{
		typedef int (WINAPI *FN_InstallKNDOCCap) (LPCTSTR lpSpLDir);

		FN_InstallKNDOCCap pfnInstallKNDOCCap = (FN_InstallKNDOCCap)docCapLoad.GetProcAddress("InstallDriver");


		if(pfnInstallKNDOCCap!=NULL)
			pfnInstallKNDOCCap(_T(""));
	} else if(0==lstrcmpi(lpstrCmdLine, "uninstall"))
	{
		typedef int (WINAPI *FN_RemoveKNDOCCap) ();
		FN_RemoveKNDOCCap pfnRemoveKNDOCCap = (FN_RemoveKNDOCCap)docCapLoad.GetProcAddress("RemoveDriver");
		if(pfnRemoveKNDOCCap!=NULL)
			pfnRemoveKNDOCCap();
		docCapLoad.Free();
	}

	return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
