// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_SPOOL_TO_EMF_H_
#define DOCCAP_SPOOL_TO_EMF_H_
#pragma once

#include <string>
#include <vector>
#include <Windows.h>

class InputFile;

class SploolToEmf {
public:
	SploolToEmf(const std::string& spl, const std::string& temp);
	virtual ~SploolToEmf();

	std::vector<std::string> GetEmfFileName();


	int GetPos(LPBYTE bSou, const BYTE bDes[], int len, int status);
	BOOL OpenSplFile();	
	BOOL PlayNTEnhMetaFile();
	BOOL LoadEmfFile();
	BOOL CheckNTSpoolEmfFile(InputFile *emf, ENHMETAHEADER *header);

	std::vector<std::string>	m_svecEmfFileName;
	std::vector<int>	m_ivecStartPos;
	std::vector<int> m_ivecEndPos;
	std::string m_sSplFileName;
	std::string m_strTempDir;

	InputFile *m_ntemf;
	int		pageNum_;
};

#endif // !defined(AFX_KNEMF_H__D5FCB1E9_51F7_4662_A3FB_BAC44BB8E99D__INCLUDED_)
