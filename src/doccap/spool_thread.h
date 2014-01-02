// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_SPOOL_THREAD_H_
#define DOCCAP_SPOOL_THREAD_H_
#pragma once

#include <string>
#include <vector>

#include "doccap/thread.h"

#define MAX_PRINTERJOB 100
#define WM_MSGDOCCAPDATA	WM_USER+9999

//´òÓ¡¼à¿ØÏß³Ì
class SpoolThread : 
	public CThread {
public:
	SpoolThread(const char* printer_name, const char* spl_dir, const char* temp_dir, HWND window);
	virtual ~SpoolThread();

	BOOL Start();

	virtual BOOL Run();

protected:
	BOOL InitInstance();
	int ExitInstance();

private:
	BOOL SelectPrinter();
	BOOL HandlePrinterJob(int spooling_flag = 0);
	BOOL GetSplFileName(int job_id, std::string* out_spl_name);

	//DataMember
	std::vector<std::string>	emfs_;
	HANDLE			printer_handle_;                   /* the printer to watch */
	std::string		printer_name_;
	HWND			parent_window_;

	std::string		spl_dir_;
	std::string		temp_dir_;

	int				spooling_flags_[MAX_PRINTERJOB];
};

#endif // DOCCAP_SPL_THREAD_H_

