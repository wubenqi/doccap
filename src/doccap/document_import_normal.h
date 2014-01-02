// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//


#ifndef DOCCAP_DOCUMENT_IMPORT_NORMAL_H_
#define DOCCAP_DOCUMENT_IMPORT_NORMAL_H_
#pragma once

#include "doccap/document_import_base.h"

class DocumentImportNormal :
	public DocumentImportBase {
public:
	explicit DocumentImportNormal(const char* file_path);
	virtual ~DocumentImportNormal(void);

	virtual BOOL ImportDocument(void);
};

#endif

