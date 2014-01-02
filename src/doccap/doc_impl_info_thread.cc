// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/doc_impl_info_thread.h"

#include "doccap/doccap_driver.h"

DOCImplInfoThread::DOCImplInfoThread(HWND hParent, LPCTSTR lpszFilePath, LPCTSTR lpszExtName):
	parentWnd_(hParent),
	filePath_(lpszFilePath),
	extName_(lpszExtName),
	docImp_(NULL),
	isFind_(false) {
	CreateThread();
}

DOCImplInfoThread::~DOCImplInfoThread(void) {
}

BOOL DOCImplInfoThread::InitInstance() {

	default_printer_ = DocCapDriver::GetDefaultDriver();
	if (default_printer_!=DocCapDriver::kDocCapPrinter) {
		DocCapDriver::SetDefaultDriver(DocCapDriver::kDocCapPrinter);
		isFind_ = true;
	}

	docImp_ = new DocumentImport(filePath_.c_str(), extName_.c_str());
	docImp_->ImportDocument();
	return TRUE;
}

int DOCImplInfoThread::ExitInstance() {
	//if(DOCImpInfoView_ && DOCImpInfoView_->IsWindow())
	//{
	//	DOCImpInfoView_->DestroyWindow();
	//}
	//if(DOCImpInfoView_)
	//	delete DOCImpInfoView_;
	//DOCImpInfoView_ = NULL;
	if(docImp_)
		delete docImp_;
	docImp_ = NULL;

	if(isFind_)
		DocCapDriver::SetDefaultDriver(default_printer_.c_str());
	return 0;
}
