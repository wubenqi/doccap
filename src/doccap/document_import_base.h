// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCUMENT_IMPORT_BASE_H_
#define DOCCAP_DOCUMENT_IMPORT_BASE_H_
#pragma once

#include <string>

#include <atlbase.h>

class DocumentImportBase {
public:
	explicit DocumentImportBase(const char* file_path) {
		if(file_path)
			file_path_ = file_path;
	}

	virtual ~DocumentImportBase(void) { }

	virtual BOOL ImportDocument(void) { return TRUE; }

public:
	std::string	file_path_;
};

#endif

