// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCCAP_DRIVER_H_
#define DOCCAP_DOCCAP_DRIVER_H_
#pragma once

#include <string>
#include <Windows.h>

class DocCapDriver {
public:
	static const char kDocCapPrinter[];
	static const char kDocCapDriver[];
	static const char kDocCapPort[];
	static const char kDocCapProcessor[];

	// DocCap֧�ֵ���������
	enum DriverType {
		kDriverTypeNotSupported	= 0,	// Not supported
		kDriverType9X			= 1,	// 95ϵ��
		kDriverType2000			= 2,	// 2000���Ժ�汾ϵ��
	};

	static DocCapDriver* NewDocCapDriver();

	static int InstallDriver(const char* spl_dir);
	static int RemoveDriver();

	static std::string GetDefaultDriver();
	static bool SetDefaultDriver(const char* driver_name);

	//////////////////////////////////////////////////////////////////////////
	virtual bool IsWin9XDriverType() { return false; }
	virtual bool IsWin2000DriverType() { return false; }

protected:
	DocCapDriver() {}
	virtual ~DocCapDriver() {}

	// �����ӡ��������
	virtual bool IsExistsPrinterProcessor() { return true; }
	virtual bool AddPrinterProcessor() { return true; }
	virtual bool DeletePrinterProcessor() { return true; }

	// �����ӡ��ͨ��
	virtual bool IsExistsPrinterPort() { return false; }
	virtual bool AddPrinterPort() { return true; }
	virtual bool DeletePrinterPort() { return false; }

	// �����ӡ������
	virtual bool IsExistsPrinterDriver() { return false; }
	virtual bool AddPrinterDriver() { return true; }
	virtual bool DeletePrinterDriver() { return true; }

	// �����ӡ��
	virtual bool IsExistsPrinter() { return false; }
	virtual bool AddPrinter() { return true; }
	virtual bool DeletePrinter() { return false; }

	virtual bool PreInstallDriver() { return true; }
	virtual bool PostInstallDriver() { return true; }

	virtual bool PreRemoveDriver() { return true; }
	virtual bool PostRemoveDriver() { return true; }

private:
	DocCapDriver(const DocCapDriver&);
	void operator=(const DocCapDriver&);

	// ���DocCap֧�ֵ���������
	static DriverType GetDriverType();
};

#endif // DOCCAP_DOCCAP_DRIVER_H_


