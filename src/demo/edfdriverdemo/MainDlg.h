// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <atlddx.h>
#include "../doccap/doccap_api.h"
#include "LoadLibrary.h"
#include <atlcrack.h>

class CMainDlg : 
		public CDialogImpl<CMainDlg>, 
		public CUpdateUI<CMainDlg>,
		public CMessageFilter, 
		public CIdleHandler,
		public IDOCCapBaseObjSink,
		public CWinDataExchange<CMainDlg>
{
public:
	CMainDlg();
	~CMainDlg();

	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

public:
	virtual void OnDOCCapData(DOCCapData *pPDOCCapData);
	virtual void OnDOCImpInfo(WPARAM wParam, LPARAM lParam);

	IDOCCapBaseObj* docCapObj_;
	CLoadLibrary dynLib_;
	CButton m_wndIdok3;
	
	std::string m_dstDir;
	std::string m_outDir;

	int pageSize_;
	int pos_;
	CProgressBarCtrl	progressBarCtrl_;
	CStatic				docInfoStatic_;CButton m_wndButton1;


public:
	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER_EX(IDOK3, BN_CLICKED, OnIdok3BnClicked)
		COMMAND_HANDLER_EX(IDC_BUTTON1, BN_CLICKED, OnButton1BnClicked)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainDlg)
		DDX_CONTROL_HANDLE(IDOK3, m_wndIdok3)
		DDX_CONTROL_HANDLE(IDC_BUTTON1, m_wndButton1)
	END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);
	LRESULT OnIdok3BnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnButton1BnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
};

