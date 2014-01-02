// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/document_import.h"

#include "doccap/document_import_excel.h"
#include "doccap/document_import_word.h"
#include "doccap/document_import_ppt.h"
#include "doccap/document_import_text.h"
#include "doccap/document_import_normal.h"

DocumentImport::DocumentImport(const char* file_path, const char* ext_name) {
	do{
		if( !lstrcmpi(ext_name, "PPT") ) {
			docImp_ = new DocumentImportPPT(file_path);
			break;
		}
		if( !lstrcmpi(ext_name, "DOC") || 
			!lstrcmpi(ext_name, "DOCX")
			) {
			docImp_ = new DocumentImportWord(file_path);
			break;
		}
		if( !lstrcmpi(ext_name, "XLS") || 
			!lstrcmpi(ext_name, "XLT")
			) {
			docImp_ = new DocumentImportExcel(file_path);
			break;
		}
		if( !lstrcmpi(ext_name, "TXT") ||
			!lstrcmpi(ext_name, "LOG") ||
			!lstrcmpi(ext_name, "INF") ||
			!lstrcmpi(ext_name, "BAT") ||
			!lstrcmpi(ext_name, "CPP") ||
			!lstrcmpi(ext_name, "H")   ||
			!lstrcmpi(ext_name, "C")
			) {
			docImp_ = new DocumentImportText(file_path);
			break;
		}
		docImp_ = new DocumentImportNormal(file_path);
	}
	while(FALSE);
}

DocumentImport::~DocumentImport(void) {
	if(docImp_)
		delete docImp_;
	docImp_ = NULL;
}

BOOL DocumentImport::ImportDocument(void) {
	return docImp_->ImportDocument();
}
