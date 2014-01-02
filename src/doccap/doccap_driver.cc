// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/doccap_driver.h"

#include "third_party/dtwinver/Dtwinver.h"
#include "doccap/doccap_driver_win2000.h"
#include "doccap/doccap_api.h"

const char DocCapDriver::kDocCapPrinter[] = "Epidiascope Printer Driver";
const char DocCapDriver::kDocCapDriver[] = "Epidiascope Printer Driver";
const char DocCapDriver::kDocCapPort[] = "EDFPort";
const char DocCapDriver::kDocCapProcessor[] = "EdfVPProc";


DocCapDriver::DriverType DocCapDriver::GetDriverType() {
	COSVersion::OS_VERSION_INFO osvi;
	memset(&osvi, 0, sizeof(osvi));

	DocCapDriver::DriverType type = kDriverTypeNotSupported;
	COSVersion os;
	if (os.GetVersion(&osvi)) {
		switch (osvi.EmulatedPlatform) {
		case COSVersion::Dos:
		case COSVersion::Windows3x:
		case COSVersion::WindowsCE:
			type = kDriverTypeNotSupported;
			break;

		case COSVersion::Windows9x:
			type = kDriverType9X;
			break;
		case COSVersion::WindowsNT:
			if (os.IsNTPreWin2k(&osvi)) {
				type = kDriverTypeNotSupported;
			} else {
				type = kDriverType2000;
			}
			break;
		default: 
			type = kDriverTypeNotSupported;
			break;
		}
	}
	return type;
}

DocCapDriver* DocCapDriver::NewDocCapDriver() {
	DriverType driver_type = GetDriverType();
	DocCapDriver* driver = NULL;

	if (driver_type==kDriverType9X) {
		// 移除支持9X
		// driver = new DocCapDriverWin9X();
	} else if (driver_type==kDriverType2000) {
		driver = new DocCapDriverWin2000();
	} else {
	}

	return driver;
}

int DocCapDriver::InstallDriver(LPCTSTR lpSplDir) {
	int ec = kDocCapErrorOK;
	DocCapDriver* driver = DocCapDriver::NewDocCapDriver();

	do {
		if (!driver) {
			// 系统不支持
			ec = kDocCapErrorNotSupported;
			break;
		}

		if (!driver->PreInstallDriver()) {
			ec = kDocCapErrorSpooler;
			break;
		}

		if (!driver->IsExistsPrinterProcessor()) {
			if (!driver->AddPrinterProcessor()) {
				ec = kDocCapErrorProcessor;
				break;
			}
		}

		if (!driver->IsExistsPrinterPort()) {
			if (!driver->AddPrinterPort()) {
				ec = kDocCapErrorPort;
				break;
			}
		}

		if (!driver->IsExistsPrinterDriver()) {
			if (!driver->AddPrinterDriver()) {
				ec = kDocCapErrorDriver;
				break;
			}
		}

		if (!driver->IsExistsPrinter()) {
			if (!driver->AddPrinter()) {
				ec = kDocCapErrorPrinter;
				break;
			}
		}

		if (!driver->PostInstallDriver()) {
			ec = kDocCapErrorOKButNotRestart;
			break;
		}

	} while (0);
	if (driver) {
		delete driver;
	}
	return ec;	
}

int DocCapDriver::RemoveDriver() {
	int ec = kDocCapErrorOK;
	DocCapDriver* driver = DocCapDriver::NewDocCapDriver();

	do {
		if (!driver) {
			// 系统不支持
			ec = kDocCapErrorNotSupported;
			break;
		}

		if (!driver->PostRemoveDriver()) {
			ec = kDocCapErrorSpooler;
			break;
		}

		if(driver->IsExistsPrinter()) {
			if(!driver->DeletePrinter()) {
				ec = kDocCapErrorPrinter;
				break;
			}
		}

		if(driver->IsExistsPrinterDriver()) {
			if(!driver->DeletePrinterDriver()) {
				ec = kDocCapErrorDriver;
				break;
			}
		}


		if(driver->IsExistsPrinterPort()) {
			if(!driver->DeletePrinterPort()) {
				ec = kDocCapErrorPort;
				break;
			}
		}

		if(driver->IsExistsPrinterProcessor()) {
			if(!driver->DeletePrinterProcessor()) {
				ec = kDocCapErrorProcessor;
				break;
			}
		}

		// 
		driver->PostRemoveDriver();

	} while (0);

	if (driver) {
		delete driver;
	}

	return ec;
}

// GetDefaultDriver和SetDefaultDriver
// 参考 http://support.microsoft.com/kb/246772

// You are explicitly linking to GetDefaultPrinter because linking 
// implicitly on Windows 95/98 or NT4 results in a runtime error.
// This block specifies which text version you explicitly link to.
#ifdef UNICODE
#define GETDEFAULTPRINTER "GetDefaultPrinterW"
#else
#define GETDEFAULTPRINTER "GetDefaultPrinterA"
#endif

std::string DocCapDriver::GetDefaultDriver() {
	std::string driver_name;
	BOOL b_flag = FALSE;

	DriverType type = GetDriverType();
	if (type==kDriverType9X) {
		PRINTER_INFO_2 *ppi2 = NULL;
		DWORD needed = 0;
		DWORD returned = 0;

		do {
			// The first EnumPrinters() tells you how big our buffer must
			// be to hold ALL of PRINTER_INFO_2. Note that this will
			// typically return FALSE. This only means that the buffer (the 4th
			// parameter) was not filled in. You do not want it filled in here.
			SetLastError(0);
			b_flag = EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, NULL, 0, &needed, &returned);
			{
				if ((GetLastError() != ERROR_INSUFFICIENT_BUFFER) || (needed == 0))
					break;
			}

			// Allocate enough space for PRINTER_INFO_2.
			ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, needed);
			if (!ppi2) {
				break;
			}

			// The second EnumPrinters() will fill in all the current information.
			b_flag = EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, (LPBYTE)ppi2, needed, &needed, &returned);
			if (!b_flag) {
				break;
			}

			driver_name = ppi2->pPrinterName;
		} while (0);

		if (ppi2) {
			GlobalFree(ppi2);
		}
	} else if (type==kDriverType2000) {

		PROC fnGetDefaultPrinter = NULL;

		BOOL
			WINAPI
			GetDefaultPrinterA(
			__out_ecount_opt(*pcchBuffer)   LPSTR   pszBuffer,
			__inout                         LPDWORD   pcchBuffer
			);

		HMODULE win_spool = NULL;

		do {
			win_spool = LoadLibrary("winspool.drv");
			if (!win_spool) {
				break;
			}
			fnGetDefaultPrinter = GetProcAddress(win_spool, GETDEFAULTPRINTER);
			if (!fnGetDefaultPrinter) {
				// FreeLibrary(win_spool);
				break;
			}

			char default_printer[MAX_PATH];
			default_printer[0] = '\0';
			DWORD default_printer_size = MAX_PATH;
			
			// 所分配的空间是否足够
			b_flag = ::GetDefaultPrinter((char*)default_printer, &default_printer_size);			// FreeLibrary(win_spool);
			if (!b_flag) {
				break;
			}

			driver_name = default_printer;
		} while (0);

		if (win_spool) {
			FreeLibrary(win_spool);
		}
	}

	return driver_name;

}
#undef GETDEFAULTPRINTER

// You are explicitly linking to SetDefaultPrinter because implicitly
// linking on Windows 95/98 or NT4 results in a runtime error.
// This block specifies which text version you explicitly link to.
#ifdef UNICODE
#define SETDEFAULTPRINTER "SetDefaultPrinterW"
#else
#define SETDEFAULTPRINTER "SetDefaultPrinterA"
#endif

bool DocCapDriver::SetDefaultDriver(const char* driver_name) {
	if (driver_name==NULL) {
		return false;
	}

	bool b_ret = false;
	BOOL b_flag = FALSE;

	DriverType type = GetDriverType();
	if (type==kDriverType9X) {

		PRINTER_INFO_2 *ppi2 = NULL;
		HANDLE printer = NULL;
		DWORD needed = 0;

		do {
			// Open this printer so you can get information about it.
			b_flag = ::OpenPrinter((char*)driver_name, &printer, NULL);
			if (!b_flag || !printer) {
				break;
			}

			// The first GetPrinter() tells you how big our buffer must
			// be to hold ALL of PRINTER_INFO_2. Note that this will
			// typically return FALSE. This only means that the buffer (the 3rd
			// parameter) was not filled in. You do not want it filled in here.
			SetLastError(0);
			b_flag = GetPrinter(printer, 2, 0, 0, &needed);
			if (!b_flag) {
				if ((GetLastError() != ERROR_INSUFFICIENT_BUFFER) || (needed == 0)) {
					break;
				}
			}

			// Allocate enough space for PRINTER_INFO_2.
			ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, needed);
			if (!ppi2) {
				break;
			}

			// The second GetPrinter() will fill in all the current information
			// so that all you have to do is modify what you are interested in.
			b_flag = GetPrinter(printer, 2, (LPBYTE)ppi2, needed, &needed);
			if (!b_flag) {
				break;
			}

			// Set default printer attribute for this printer.
			ppi2->Attributes |= PRINTER_ATTRIBUTE_DEFAULT;
			b_flag = SetPrinter(printer, 2, (LPBYTE)ppi2, 0);
			if (b_flag) {
				b_ret = true;
			}

			// Tell all open programs that this change occurred.
			// Allow each program 1 second to handle this message.
			SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L, (LPARAM)(LPCTSTR)"windows", SMTO_NORMAL, 1000, NULL);
		} while (0);

		// Clean up.
		if (printer) {
			ClosePrinter(printer);
		}
		if (ppi2) {
			GlobalFree(ppi2);
		}
	} else if (type==kDriverType2000) {
		PROC fnSetDefaultPrinter = NULL;
		HMODULE win_spool = NULL;

		do {
			win_spool = LoadLibrary("winspool.drv");
			if (!win_spool) {
				break;
			}
			fnSetDefaultPrinter = GetProcAddress(win_spool, SETDEFAULTPRINTER);
			if (!fnSetDefaultPrinter) {
				FreeLibrary(win_spool);
				break;
			}

			b_flag = ::SetDefaultPrinter(driver_name);
			if (b_flag) {
				b_ret = true;
			}

			// Tell all open programs that this change occurred. 
			// Allow each app 1 second to handle this message.
			SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L, 0L, SMTO_NORMAL, 1000, NULL);

		} while (0);

		// Clean up.
		if (win_spool) {
			FreeLibrary(win_spool);
		}
	} else {
	}

	return b_ret;
}
#undef SETDEFAULTPRINTER

