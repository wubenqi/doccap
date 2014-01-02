// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCUMENT_IMPORT_TEXT_H_
#define DOCCAP_DOCUMENT_IMPORT_TEXT_H_
#pragma once

#include "doccap/document_import_base.h"

class DocumentImportText :
	public DocumentImportBase {
public:
	explicit DocumentImportText(const char* file_path);
	virtual ~DocumentImportText(void) {}

	virtual BOOL ImportDocument(void);
};

#endif

