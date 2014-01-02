// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_DOCCAP_HELP_WINDOW_H_
#define DOCCAP_DOCCAP_HELP_WINDOW_H_
#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>

#include "resource.h"

class DOCCapHelpWIndow :
	public CFrameWindowImpl<DOCCapHelpWIndow> {
public:
	DOCCapHelpWIndow() {}
	virtual ~DOCCapHelpWIndow() {}

	DECLARE_FRAME_WND_CLASS("Main Epidiascope Help Window", IDS_PROJNAME)

    BEGIN_MSG_MAP(DOCCapHelpWIndow)
		CHAIN_MSG_MAP( CFrameWindowImpl<DOCCapHelpWIndow>)
    END_MSG_MAP()

    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
};

#endif // DOCCAP_DOCCAP_HELP_WINDOW_H_
