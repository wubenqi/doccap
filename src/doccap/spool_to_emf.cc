// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/spool_to_emf.h"

#include "doccap/doc_imp_info_view.h"
#include "doccap/doccap_api.h"
#include "doccap/input_file.h"

const BYTE kEmfString1[] = {0x01, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00};
const BYTE kEmfString2[] = {0x10, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00};
const int kMaxEmrField = 16384;         // 16K*sizeof(DWORD), maximum 64 Kb

extern HWND g_hDOCCapWnd;

namespace {

#include <Rpc.h>
std::string GenerateUUID() {
	UUID * pUUID = new UUID();
	unsigned char *sUUID = NULL;
	std::string result;

	if (UuidCreate(pUUID) == RPC_S_OK) {
		UuidToString(pUUID, &sUUID);
		result = std::string((char *)sUUID);
		RpcStringFree(&sUUID);
	}

	if (pUUID) {
		delete pUUID;
	}

	return result;
}

}


typedef struct  {
	DWORD   iType;                // Enhanced metafile record type
	DWORD   nSize;                // Length of the record in bytes.
			                          // This must be a multiple of 4.
	DWORD   Para[kMaxEmrField-2];  // Parameter
} EmrRecord;


SploolToEmf::SploolToEmf(const std::string& spl, const std::string& temp) {
	m_sSplFileName = spl;
	m_strTempDir = temp;
	m_ntemf = NULL;
	pageNum_ = 0;
}


SploolToEmf::~SploolToEmf() {
	if (m_ntemf)
		delete m_ntemf;

}

std::vector<std::string> SploolToEmf::GetEmfFileName() {
	OpenSplFile();
	return m_svecEmfFileName;
}

int SploolToEmf::GetPos(LPBYTE bSou, const BYTE bDes[], int len, int status) {
	int iposLen;
	int i, j;

	if (status == 0)
		iposLen = 8;
	else
		iposLen = 9;

	for (i=0; i<len-iposLen; i++) {
		for (j=0; j<iposLen; j++) {
			if (bSou[i+j] == bDes[j])
				continue;
			else
				break;
		}
		if (j==iposLen)
			return i;
	}
	return -1;
}


BOOL SploolToEmf::OpenSplFile() {
	if (!LoadEmfFile()) {
/*
		MessageBox(g_hDOCCapWnd, 
                "文档捕获出错！您可能没有正确安装文档捕获驱动程序！",
                DOCCAP_INFO, MB_OKCANCEL | MB_ICONSTOP);
*/
		return FALSE;
	}

	return TRUE;
}	

BOOL SploolToEmf::CheckNTSpoolEmfFile(InputFile *emf, ENHMETAHEADER *header) {
	EmrRecord rec;

	emf->Reset();

	// 0x00010000 len 
	emf->Read((char *) & rec, 8);

	if (rec.iType!=0x10000) return FALSE;

	// appname document name port
	emf->Read((char *) & rec.Para, rec.nSize-8);

	while (true) {
		// 0x000000001 EMF_Stream_len
		emf->Read((char *) & rec, 8);
	
		if ( (rec.iType==1) || (rec.iType==12) ) break;

		if (rec.iType < 0xFFFF) {
			if ( rec.iType==2 )
				emf->Read(NULL, rec.nSize);  // embedded font
			else
				emf->Read(NULL, rec.nSize-8);
		}
	}
	
	emf->Read((char *)header, sizeof(ENHMETAHEADER));

	return header->iType==1;
}

BOOL SploolToEmf::LoadEmfFile() {
	ENHMETAHEADER MetaHeader;

	m_ntemf	= new InputFile;
	if (m_ntemf==NULL)
		return FALSE;

	if (!m_ntemf->Open(m_sSplFileName.c_str()))
		return FALSE;

	if (!CheckNTSpoolEmfFile(m_ntemf, &MetaHeader)) {
		m_ntemf->Close();
		return FALSE;
	}
	if (!PlayNTEnhMetaFile()) {
		m_ntemf->Close();
		return FALSE;
	}

	m_ntemf->Close();
	return TRUE;
}

BOOL SploolToEmf::PlayNTEnhMetaFile() {
	EmrRecord rec;

	// 0x00010000 len 	
	m_ntemf->Reset();
	// 0x00010000 len 
	m_ntemf->Read((char *) & rec, 8);

	if (rec.iType!=0x10000) return FALSE;

	// appname document name port
	m_ntemf->Read((char *) & rec.Para, rec.nSize-8);

	while(TRUE) {	
		while (TRUE) {
		if (m_ntemf->Eof())
			goto RE100;

			// 0x000000001 EMF_Stream_len
			m_ntemf->Read((char *) & rec, 8);
	
			if ( (rec.iType==1) || (rec.iType==12) ) 
				break;

			if (rec.iType < 0xFFFF) {
//				if ( rec.iType==2 )
					m_ntemf->Read(NULL, rec.nSize);  // embedded font
//				else
//					m_ntemf->read(NULL, rec.nSize-8);
		
			}
		}

		byte * bits = (byte *) malloc(rec.nSize);
		if ( bits == NULL )
			return FALSE;

		m_ntemf->Read((char *) bits, rec.nSize);

		pageNum_++;
		::PostMessage(g_hDOCCapWnd, WM_DOCIMPINFOMSG, DOCIMPINFO_PROCESSING, (LPARAM)pageNum_);

		std::string out_emf_name = m_strTempDir;
		out_emf_name += "\\";
		out_emf_name += GenerateUUID();
		out_emf_name += ".EMF";

		HANDLE handle = CreateFile(out_emf_name.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle==INVALID_HANDLE_VALUE) {
			free(bits);
			return FALSE;
		}

		unsigned long written;
		WriteFile(handle, bits, rec.nSize, &written, NULL);
		::CloseHandle(handle);

/*
		sprintf(szTempFile,"%s\\%s.JPG",m_strTempDir.c_str(), get_current_time3());
		CxImage xImage(bits, rec.nSize, CXIMAGE_FORMAT_WMF);
		CxImage& logoImg = g_DOCCapWnd->m_logoImg;
		long x = xImage.GetWidth()-logoImg.GetWidth();
		long y = xImage.GetHeight()-logoImg.GetHeight();
		xImage.Mix(logoImg, CxImage::OpScreen, -x, -y);
		xImage.Save(szTempFile, CXIMAGE_FORMAT_JPG);
*/

		m_svecEmfFileName.push_back(out_emf_name);
		free(bits);
	}
RE100:
	return TRUE;
}

