// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/doccap_driver_win2000.h"

#include <atlbase.h>
#include <WinSpool.h>

#include <atlbase.h>
#include <atlcom.h>

#define _Module (*_pModule)

std::string	DocCapDriverWin2000::static_dvw_dir_;
std::string	DocCapDriverWin2000::static_dvw_temp_dir_;
std::string	DocCapDriverWin2000::static_dvw_spool_dir_;

namespace {
//修改注册表
bool ReadSpoolDirectory(const char* spl_dir) {
	HKEY hKey;
	bool bFlag = false;

	std::string sub_key("SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\");
	sub_key.append(DocCapDriver::kDocCapPrinter);

	LONG lRes=RegOpenKeyEx(HKEY_LOCAL_MACHINE, sub_key.c_str(), 0,KEY_QUERY_VALUE,&hKey);
	if(lRes==ERROR_SUCCESS) {
		DWORD dwType=REG_SZ;
		BYTE str[MAX_PATH];
		//memset(str, 0, MAX_PATH);
		DWORD dwLen;
		lRes=RegQueryValueEx(hKey,"SpoolDirectory",NULL,&dwType,str,&dwLen);
		if(lRes==ERROR_SUCCESS) {
			//SetEnable(dwValue);
			if(str[0]=='\0')
				bFlag = false;
			else if(lstrcmp((char *)str, spl_dir)==0)
				bFlag = true;
			else
				bFlag = false;
			RegCloseKey(hKey);
			return bFlag;
		}
	}
	RegCloseKey(hKey);
	return false;
}

BOOL WriteSpoolDirectory(const char* spl_dir) {
	HKEY hKey;
	
	std::string sub_key("SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\");
	sub_key.append(DocCapDriver::kDocCapPrinter);

	LONG lRes=RegCreateKey(HKEY_LOCAL_MACHINE, sub_key.c_str(), &hKey);
	if(lRes==ERROR_SUCCESS) {
		DWORD dwType=REG_SZ;
		lRes=RegSetValueEx(hKey,"SpoolDirectory",0,dwType,(BYTE*)spl_dir,(DWORD)strlen(spl_dir));
	}
	RegCloseKey(hKey);
	return TRUE;
}

int IsServiceStopping(const char *service_name) {
    SERVICE_STATUS ss_status; 
 	SC_HANDLE sch_service = 0, sch_scmanager = 0;

 	int ret = -1;
    // The required service object access depends on the control. 

	sch_scmanager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (sch_scmanager == NULL) 
		return ret;	

    sch_service = OpenService( 
        sch_scmanager,          // SCM database 
        service_name,          // service name
        SERVICE_ALL_ACCESS); 
 
    if (sch_service == NULL) { 
		CloseServiceHandle( sch_scmanager );
		return ret;	
    }
    
	if (!QueryServiceStatus( 
			sch_service,   // handle to service 
			&ss_status))  // address of structure
	{
		CloseServiceHandle(sch_service); 
		CloseServiceHandle( sch_scmanager );    
		return ret;
	} 
    
	if (ss_status.dwCurrentState == SERVICE_STOPPED) 
        ret = 0;
    else 
        ret = 1;
	
    CloseServiceHandle(sch_service); 
    CloseServiceHandle( sch_scmanager );    

    return ret; 
}

bool CopyPrinterDriver(const char* src_dir, const char* dst_dir, const char* src_file_name, const char* dst_file_name) {
	std::string old_file, new_file;

	old_file.assign(src_dir);
	old_file.push_back('\\');
	old_file.append(src_file_name);

	new_file.assign(dst_dir);
	new_file.push_back('\\');
	new_file.append(dst_file_name);

	CopyFile(old_file.c_str(), new_file.c_str(), FALSE);
	return true;
}

bool CopyPrinterDriver(const char* src_dir, const char* dst_dir, const char* src_file_name, const char* dst_file_name, char* new_dir_file) {
	std::string old_file, new_file;

	old_file.assign(src_dir);
	old_file.push_back('\\');
	old_file.append(src_file_name);

	new_file.assign(dst_dir);
	new_file.push_back('\\');
	new_file.append(dst_file_name);

	lstrcpy(new_dir_file, new_file.c_str());

	CopyFile(old_file.c_str(), new_file.c_str(), FALSE);
	return true;
}

}

void DocCapDriverWin2000::InitWorkDirs()
{
	char sys_tmp[MAX_PATH];
	::GetWindowsDirectory(sys_tmp, MAX_PATH);

	static_dvw_dir_.assign(sys_tmp);
	static_dvw_dir_.append("\\Temp");

	DWORD dwAttributes = ::GetFileAttributes(static_dvw_dir_.c_str());
	if( (dwAttributes == 0xFFFFFFFF) || (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) )
		::CreateDirectory(static_dvw_dir_.c_str(), NULL); 

	static_dvw_dir_.append("\\DocCapTemp");

	dwAttributes = ::GetFileAttributes(static_dvw_dir_.c_str());
	if( (dwAttributes == 0xFFFFFFFF) || (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) )
		::CreateDirectory(static_dvw_dir_.c_str(), NULL); 

	static_dvw_spool_dir_ = static_dvw_dir_+"\\DocCap";
	dwAttributes = ::GetFileAttributes(static_dvw_spool_dir_.c_str());
	if( (dwAttributes == 0xFFFFFFFF) || (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) )
		::CreateDirectory(static_dvw_spool_dir_.c_str(), NULL); 

	static_dvw_temp_dir_ = static_dvw_dir_ + "\\Temp";
	dwAttributes = ::GetFileAttributes(static_dvw_temp_dir_.c_str());
	if( (dwAttributes == 0xFFFFFFFF) || (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) )
		::CreateDirectory(static_dvw_temp_dir_.c_str(), NULL); 
}

bool DocCapDriverWin2000::IsExistsPrinterProcessor() {
	int rc;
	LPBYTE buffer;
	DWORD needed, returned;
	PRINTPROCESSOR_INFO_1 *ppi1;
	unsigned int i;
	bool b_ret = false;
	do {
		rc = EnumPrintProcessors(NULL, NULL, 1, NULL, 0, &needed, &returned);
		//if (!rc) {
		//	break;
		//}

		buffer = (BYTE*)malloc(needed);

		rc = EnumPrintProcessors(NULL, NULL, 1, buffer, needed, &needed, &returned);
		ppi1 = (PRINTPROCESSOR_INFO_1 *)buffer;
		if ((rc) && (returned)) {
			for (i=0; i<returned; i++) {	
				if (lstrcmp(ppi1[i].pName, kDocCapProcessor) == 0)
					break;
			}

			if (!(i < returned)) {
				break;
			}
		} else {
			break;
		}
		free(buffer);
		b_ret = true;

	} while (0);

	return b_ret;
}

bool DocCapDriverWin2000::AddPrinterProcessor() {
	char szDestPath[MAX_PATH], szCurPath[MAX_PATH];
	//KNDVW_ENVIRONMENT
	memset(szCurPath, 0, MAX_PATH);
	memset(szDestPath, 0, MAX_PATH);

	::GetModuleFileName(_Module.m_hInst, szCurPath, MAX_PATH);
	for (int i = lstrlen(szCurPath); i>=0; i--) {
		if (szCurPath[i]=='\\') {
			szCurPath[i]='\0';
			break;
		}
	}	

	DWORD  dwNeeded;
	//lstrcpy(szEnvironment, _T("Windows NT x86"));
	if (!GetPrintProcessorDirectory(NULL, "Windows NT x86", 1, (unsigned char *)szDestPath, MAX_PATH, &dwNeeded)) {
		return FALSE;
	}

	CopyPrinterDriver(szCurPath, szDestPath, "edfvpproc.dll", "edfvpproc.dll");

	return TRUE == ::AddPrintProcessor(NULL,NULL, "edfvpproc.dll","EdfVPProc");

}

bool DocCapDriverWin2000::DeletePrinterProcessor() {
	return TRUE == ::DeletePrintProcessor(NULL, NULL, (char*)kDocCapProcessor);
}


bool DocCapDriverWin2000::IsExistsPrinterPort() {
	DWORD needed, ports, size;
	PORT_INFO_1 *old_port_info = NULL;
	bool b_ret = false;

	do {
		if(!EnumPorts(NULL, 1, NULL, 0, &needed, &ports)) {
			// break;
		}
		size = needed;

		old_port_info = (PORT_INFO_1 *)malloc(needed);
		if (NULL == old_port_info) {
			break;
		}

		if(!EnumPorts(NULL, 1, (LPBYTE)old_port_info, size, &needed, &ports)) {
			break;
		}

		for (DWORD i=0; i<ports; i++) {	
			if (lstrcmp(old_port_info[i].pName, kDocCapPort) == 0) {
				b_ret = true;
				break;
			}
		}
	} while (0);

	if (old_port_info) {
		free(old_port_info);
	}
	return b_ret;
}

bool DocCapDriverWin2000::AddPrinterPort() {
	DWORD   dwbNeeded, dwcPorts, dwbSize;
	PORT_INFO_1 *pOldPortInfo = NULL;
	bool b_ret = false;

	EnumPorts(NULL, 1, NULL, 0, &dwbNeeded, &dwcPorts);
	dwbSize = dwbNeeded;
	pOldPortInfo = (PORT_INFO_1 *)malloc(dwbNeeded);	

	if(NULL == pOldPortInfo) return false;

	pOldPortInfo->pName = _strdup(kDocCapPort);

	HMODULE hLibHandle;
	//hLibHandle = LoadLibrary(&"winspool.DRV");
	hLibHandle = LoadLibrary("winspool.DRV");
	typedef BOOL WINAPI lpAddPortEx(LPTSTR, DWORD, LPBYTE, LPTSTR);
	lpAddPortEx *AddPortEx;

	//AddPortEx = (lpAddPortEx *) GetProcAddress( hLibHandle, &"AddPortExA");
	AddPortEx = (lpAddPortEx *) GetProcAddress( hLibHandle, "AddPortExA");
	//if ((AddPortEx)( NULL, 1, (LPBYTE) pOldPortInfo, &"Local Port"))
	if ((AddPortEx)( NULL, 1, (LPBYTE) pOldPortInfo, "Local Port")) {
		b_ret = false;
		// return false;
		// std::cout << "add error!\n";
	} else {
		b_ret = true;
		// return true;
		// std::cout << "Success!\n";
	}
	FreeLibrary(hLibHandle);	

	return b_ret;
}

bool DocCapDriverWin2000::DeletePrinterPort() {
	return TRUE == ::DeletePort(NULL, NULL, (char*)kDocCapPort);
}

bool DocCapDriverWin2000::IsExistsPrinterDriver() {
	DWORD needed = 0;
	DWORD number_of_drivers = 0;
	DRIVER_INFO_1 *driver_info = NULL;
	DWORD i;
	bool b_ret = false;

	do {
		// This is recommended by the documentation; first call it with 0 bytes tofind out the number of bytes actually needed.

		EnumPrinterDrivers (NULL, NULL, 1, NULL, 0, &needed, &number_of_drivers);

		driver_info = (DRIVER_INFO_1 *)malloc(needed);
		if (!driver_info) {
			break;
		}

		EnumPrinterDrivers (NULL, NULL, 1, (LPBYTE) driver_info, needed, &needed, &number_of_drivers);

		for (i = 0; i < number_of_drivers; i++) {
			if (lstrcmp(driver_info[i].pName, kDocCapDriver) == 0) {
				b_ret = true;
				break;
			}
		}
	} while (0);

	if (driver_info) {
		free(driver_info);
	}
	return b_ret;
}

bool DocCapDriverWin2000::AddPrinterDriver() {
	TCHAR	szDestPath[MAX_PATH],
		szCurPath[MAX_PATH],
		szDriverPath[MAX_PATH],
		szDataFile[MAX_PATH],
		szConfigFile[MAX_PATH],
		szHelpFile[MAX_PATH],
		szDependentFiles[MAX_PATH],
		szEnvironment[MAX_PATH],
		szDefaultDataType[MAX_PATH];

	memset(szCurPath, 0, MAX_PATH);
	memset(szDestPath, 0, MAX_PATH);
	memset(szDataFile, 0, MAX_PATH);
	memset(szConfigFile, 0, MAX_PATH);
	memset(szHelpFile, 0, MAX_PATH);
	memset(szDependentFiles, 0, MAX_PATH);
	memset(szEnvironment, 0, MAX_PATH);
	memset(szDefaultDataType, 0, MAX_PATH);

	//	lstrcpy(szCurPath, _T("E:\\winme\\TestPrinter\\Debug"));

	::GetModuleFileName(_Module.m_hInst, szCurPath, MAX_PATH);
	for (int i = lstrlen(szCurPath); i>=0; i--) {
		if (szCurPath[i]=='\\') {
			szCurPath[i]='\0';
			break;
		}
	}	

	DRIVER_INFO_3 Di;
	memset(&Di,0, sizeof(DRIVER_INFO_3));

	DWORD  dwNeeded;
	lstrcpy(szEnvironment, "Windows NT x86");
	if (!GetPrinterDriverDirectory(NULL, szEnvironment, 1, (unsigned char *)szDestPath, MAX_PATH, &dwNeeded)) {
		return false;
	}

	//拷贝所需要的文件
	CopyPrinterDriver(szCurPath, szDestPath, "UNIDRV.DLL", "UNIDRV.DLL", szDriverPath);
	CopyPrinterDriver(szCurPath, szDestPath, "unires.dll", "unires.dll");
	CopyPrinterDriver(szCurPath, szDestPath, "UNIDRV.HLP", "UNIDRV.HLP", szHelpFile);
	CopyPrinterDriver(szCurPath, szDestPath, "unidrvui.dll", "unidrvui.dll", szConfigFile);
	CopyPrinterDriver(szCurPath, szDestPath, "edfprint.dll", "edfprint.dll");
	CopyPrinterDriver(szCurPath, szDestPath, "EDFPRINT.GPD", "EDFPRINT.GPD", szDataFile);
	CopyPrinterDriver(szCurPath, szDestPath, "stdnames.gpd", "stdnames.gpd");

	lstrcpy(szCurPath, szDestPath);
	lstrcat(szCurPath, "\\3");

	char *dep = "edfprint.dll\0stdnames.gpd\0unires.dll\0\0";
	lstrcpy(szDefaultDataType, "NT EMF 1.008");
	memcpy(szDependentFiles, dep, 37);

	Di.cVersion = 3;
	Di.pName = (char*)kDocCapPrinter;
	Di.pEnvironment = szEnvironment;
	Di.pDriverPath = szDriverPath;
	Di.pDataFile = szDataFile;
	Di.pConfigFile = szConfigFile;
	Di.pHelpFile = szHelpFile;
	Di.pDependentFiles = szDependentFiles;
	Di.pDefaultDataType = szDefaultDataType;
	Di.pMonitorName = "";
	if (!::AddPrinterDriver( NULL, 3, (LPBYTE)&Di )) {
		return FALSE;
	}
	return TRUE;
}

bool DocCapDriverWin2000::DeletePrinterDriver() {
	return TRUE == ::DeletePrinterDriver(NULL, "Windows NT x86", (char*)kDocCapDriver);
}

bool DocCapDriverWin2000::IsExistsPrinter() {
	DWORD   needed, printers;
	PRINTER_INFO_1  *old_printers_info = NULL;
	DWORD i;
	bool b_ret = false;
	do {
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 1, NULL, 0, &needed, &printers);
		old_printers_info = (PRINTER_INFO_1 *)malloc(needed);
		if (NULL == old_printers_info) {
			break;
		}

		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 1, (LPBYTE)old_printers_info, needed, &needed, &printers);

		for (i=0; i<printers; i++) {	
			//printf("port = %s\n", pOldPrintersInfo[i].pName);
			if (lstrcmp(old_printers_info[i].pName, kDocCapPrinter) == 0) {
				b_ret = true;
				break;
			}
		}
	} while (0);

	if (old_printers_info) {
		free(old_printers_info);
	}

	return b_ret;
}

bool DocCapDriverWin2000::AddPrinter() {
    HANDLE hPrinter;
	PRINTER_INFO_2 PIPrinter;
	TCHAR* lpwServerName = NULL;
	TCHAR szDefaultDataType[MAX_PATH];

	lstrcpy(szDefaultDataType, "NT EMF 1.008");

	PRINTER_DEFAULTS PD;

	DEVMODE *DevSettings = NULL;
	DevSettings = (DEVMODE *)new char[148];
	int DevSettingSize = 0;

	DEVMODE dm; 
	DEVMODE *pdm = &dm;
	DevSettings = &dm;
    lstrcpy((char *)(LPCTSTR)dm.dmDeviceName, kDocCapPrinter);//[CCHDEVICENAME]; 
    dm.dmSpecVersion = 1024; 
    dm.dmDriverVersion = 1025; 
    dm.dmSize = 148;//sizeof(DEVMODE); 
    dm.dmDriverExtra = 64; 
    dm.dmFields  = 67134979;//DM_ORIENTATION;//
    dm.dmPaperSize = DMPAPER_LETTER;
    dm.dmScale = 0; 
    dm.dmCopies = 1; 
    dm.dmDefaultSource = 7;//DMBIN_ONLYONE;
	dm.dmPrintQuality = 300;//DMRES_DRAFT; 
    dm.dmColor = 1;//DMCOLOR_MONOCHROME ; 
    dm.dmDuplex = 1;//DMDUP_SIMPLEX; 
    dm.dmYResolution = 300;//0; 
    dm.dmTTOption = 1;//DMTT_BITMAP; 
   // short  dmCollate; 
    lstrcpy((char *)(LPCTSTR)dm.dmFormName,"Letter");//[CCHFORMNAME]; 
//	dmLogPixels = 0; 
//    dmBitsPerPel = 0; 
//    dmPelsWidth = 0; 
//    dmPelsHeight = 0; 
    dm.dmDisplayFlags = 0;//DM_GRAYSCALE; 
//   dmDisplayFrequency = 0; 
#if(WINVER >= 0x0400) 
    dm.dmICMMethod = 0;//DMICMMETHOD_NONE;
    dm.dmICMIntent = 0;//DMICM_COLORMETRIC;
    dm.dmMediaType = 0;//DMMEDIA_STANDARD;
    dm.dmDitherType = 2;//DMDITHER_GRAYSCALE;
    dm.dmReserved1 = 0;
    dm.dmReserved2 = 0;
#if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    dm.dmPanningWidth = 0;
    //DWORD  dmPanningHeight;//
#endif
#endif // WINVER >= 0x0400 

	memset( &PD, 0, sizeof( PRINTER_DEFAULTS ) );
	PD.DesiredAccess = PRINTER_ALL_ACCESS;

	memset(&PIPrinter, 0, sizeof( PRINTER_INFO_2));  

	PIPrinter.pServerName = NULL; 
	PIPrinter.pPrinterName = (char*)kDocCapPrinter; 
    PIPrinter.pShareName = NULL; 
    PIPrinter.pPortName = (char*)kDocCapPort; 
    PIPrinter.pDriverName = (char*)kDocCapDriver; 
    PIPrinter.pComment = NULL; 
    PIPrinter.pLocation = NULL; 
    PIPrinter.pDevMode = DevSettings; 
    PIPrinter.pSepFile = ""; 
    PIPrinter.pPrintProcessor = (char*)kDocCapProcessor;
    PIPrinter.pDatatype = szDefaultDataType; 
    PIPrinter.pParameters = NULL; 
    PIPrinter.pSecurityDescriptor = NULL; 
    PIPrinter.Attributes = PRINTER_ATTRIBUTE_QUEUED | PRINTER_ATTRIBUTE_ENABLE_DEVQ;; 
    PIPrinter.Priority = 0; 
    PIPrinter.DefaultPriority = 0; 
    PIPrinter.StartTime = 0; 
    PIPrinter.UntilTime = 0; 
    PIPrinter.Status = 128; 
    PIPrinter.cJobs = 0; 
    PIPrinter.AveragePPM = 0; 

	hPrinter = ::AddPrinter( lpwServerName, 2, (LPBYTE)&PIPrinter );
	if( NULL == hPrinter ) {
		return false;
	} else {
		SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L, (LPARAM)(LPCTSTR)"windows", SMTO_NORMAL, 1000, NULL);
		::ClosePrinter( hPrinter ); 
	}
	return true;
}

bool DocCapDriverWin2000::DeletePrinter() {
	HANDLE printer_handle;
	PRINTER_DEFAULTS defaults;
	ZeroMemory(&defaults, sizeof(PRINTER_DEFAULTS));
	defaults.DesiredAccess = PRINTER_ALL_ACCESS;

	bool b_ret = false;
	do {
		if(!::OpenPrinter((char*)kDocCapPrinter, (LPHANDLE)&printer_handle, &defaults)) {
			// 打不开打印机
			break;
		}

		if(!::DeletePrinter(printer_handle)) {
			// 不能删除
			::ClosePrinter(printer_handle);
			break;
		}

		// 因为已经打开了，所以需要关闭
		::ClosePrinter(printer_handle);
		b_ret = true;

	} while (0);

	return b_ret;
}

bool DocCapDriverWin2000::PreInstallDriver() {
	InitWorkDirs();
	if(IsServiceStopping("spooler") == 0)
		return false;
	return true;
}

bool DocCapDriverWin2000::PostInstallDriver() {
	if (!ReadSpoolDirectory(static_dvw_spool_dir_.c_str())) {
		WriteSpoolDirectory(static_dvw_spool_dir_.c_str());
	}
	return true;
}

bool DocCapDriverWin2000::PreRemoveDriver() {
	if(IsServiceStopping("spooler") == 0)
		return false;
	return true;
}

bool DocCapDriverWin2000::PostRemoveDriver() {
	return true;
}
