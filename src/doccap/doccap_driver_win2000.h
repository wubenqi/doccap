// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCCAP_DRIVER_WIN2000_H_
#define DOCCAP_DOCCAP_DRIVER_WIN2000_H_
#pragma once

#include "doccap/doccap_driver.h"

class DocCapDriverWin2000 :
	public DocCapDriver {
public:
	DocCapDriverWin2000() {}
	virtual ~DocCapDriverWin2000() {}

	static void InitWorkDirs();

	static std::string	static_dvw_dir_;
	static std::string	static_dvw_temp_dir_;
	static std::string	static_dvw_spool_dir_;

protected:
	// �����ӡ��������
	virtual bool IsExistsPrinterProcessor();
	virtual bool AddPrinterProcessor();
	virtual bool DeletePrinterProcessor();

	// �����ӡ��ͨ��
	virtual bool IsExistsPrinterPort();
	virtual bool AddPrinterPort();
	virtual bool DeletePrinterPort();

	// �����ӡ����������
	virtual bool IsExistsPrinterDriver();
	virtual bool AddPrinterDriver();
	virtual bool DeletePrinterDriver();

	// �����ӡ��
	virtual bool IsExistsPrinter();
	virtual bool AddPrinter();
	virtual bool DeletePrinter();

	virtual bool PreInstallDriver();
	virtual bool PostInstallDriver();

	virtual bool PreRemoveDriver();
	virtual bool PostRemoveDriver();

};

#endif // DOCCAP_DOCCAP_DRIVER_WIN2000_H_


