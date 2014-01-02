// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCUMENT_IMPORT_H_
#define DOCCAP_DOCUMENT_IMPORT_H_
#pragma once

#include <Windows.h>

class DocumentImportBase;
class DocumentImport {
public:
	DocumentImport(const char* file_path, const char* ext_name);
	virtual ~DocumentImport(void);

public:
	BOOL ImportDocument(void);

private:
	DocumentImportBase*	docImp_;
};


#endif
