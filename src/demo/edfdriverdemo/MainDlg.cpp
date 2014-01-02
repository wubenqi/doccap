// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "ssfiledialog.h"
#include "aboutdlg.h"
#include "MainDlg.h"
#include <atlstr.h>

class CModulePath
{
public:
	TCHAR m_szPath[MAX_PATH];
	TCHAR m_szAppName[MAX_PATH];

	static CModulePath* getInstance()
	{
		if( 0 == m_pModulePath ) {
			m_pModulePath = new CModulePath();
		}
		return m_pModulePath;
	}

	CModulePath()
	{
		Set(NULL);
	}

	CModulePath(HINSTANCE hInst)
	{
		Set(hInst);
	}

	void Set(HINSTANCE hInst)
	{
		memset( m_szAppName, 0, MAX_PATH );
		::GetModuleFileName(hInst, m_szPath, sizeof(m_szPath)/sizeof(TCHAR));
		// Remove filename portion
		LPTSTR p = m_szPath, pSep = NULL, pSep2 = NULL;
		while( *p ) {
			if( *p==_T(':') || *p==_T('\\') ) pSep = p;
			if( *p==_T('.')) pSep2 = p;
			p = ::CharNext(p);
		}
		//		ATLASSERT(pSep);
		m_szAppName[0] = *(pSep + 1);
		*(pSep + 1) = _T('\0');
		*(pSep2) = _T('\0');
		strcpy(m_szAppName+1, pSep + 2);
	}

	LPCTSTR getAppPathName() const { return m_szPath; };
	LPCTSTR getAppName() const { return m_szAppName; };

	static CModulePath*	m_pModulePath;

};

CModulePath* CModulePath::m_pModulePath = NULL;

//////////////////////////////////////////////////////////////////////////
BOOL __fastcall GetMyDir(int id, const char* szDir, int size=MAX_PATH)
{
	LPITEMIDLIST  pidl;
	LPMALLOC      pShellMalloc;
	if(size>MAX_PATH)
		return FALSE;
	if(SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if(SUCCEEDED(SHGetSpecialFolderLocation(NULL,
			id,
			&pidl)))
		{
			// 如果成功返回true　　
			if(SHGetPathFromIDList(pidl, (LPTSTR)szDir))
				return TRUE; 
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	return TRUE;
}

CMainDlg::CMainDlg() {
	docCapObj_ = NULL;
	pageSize_ = 0;
	pos_ = 0;
}

CMainDlg::~CMainDlg() {
	if (docCapObj_) {
	}
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

void CMainDlg::OnDOCCapData(DOCCapData *pPDOCCapData) {
	std::string doc_name = pPDOCCapData->document_name;
	if (doc_name.empty()) {
		doc_name = "document";
	} else {
		size_t pos = doc_name.rfind('\\');
		if (pos != std::string::npos) {
			doc_name = doc_name.substr(pos+1, doc_name.length()-pos-1);
		}
	}

	//todo
	//没有处理同一个文件转换两次的情况,目前只简单的覆盖
	m_outDir = m_dstDir + doc_name;
	DWORD dwAttributes = ::GetFileAttributes(m_outDir.c_str());
	if( (dwAttributes == 0xFFFFFFFF) || (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) )
		::CreateDirectory(m_outDir.c_str(), NULL); 

	for (size_t i=0; i<pPDOCCapData->emfs.size(); ++i) {
		char out_file_path[1024];
		sprintf(out_file_path, "%s\\%d.emf", m_outDir.c_str(), i+1);

		CopyFile(pPDOCCapData->emfs[i].c_str(), out_file_path, FALSE);
	}
}

void CMainDlg::OnDOCImpInfo(WPARAM wParam, LPARAM lParam) {
	char tmp[256];
	switch(wParam) {
	case DOCIMPINFO_BEGIN:
		progressBarCtrl_.SetPos(0);
		docInfoStatic_.SetWindowText("开始文档转换，请耐心等待......");
		pageSize_ = 0;
		pos_ = 0;
		break;
	case DOCIMPINFO_IMPORT:
		sprintf(tmp, "正在转换文档，转换第 %d 页", lParam);
		docInfoStatic_.SetWindowText(tmp);
		pageSize_++;
		break;
	case DOCIMPINFO_PROCESS:
		docInfoStatic_.SetWindowText("正在处理文档，请耐心等待......");
		break;
	case DOCIMPINFO_PROCESSING:
		if(pageSize_!=0) {
			pos_ += 100/pageSize_;
			progressBarCtrl_.SetPos(pos_);
		}
		break;
	case DOCIMPINFO_END:
		sprintf(tmp, "文档转换成功:)      存放目录: %s", m_outDir.c_str());
		docInfoStatic_.SetWindowText(tmp);
		pageSize_ = 0;
		progressBarCtrl_.SetPos(100);
		break;
	default:
	    break;
	}
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	m_dstDir = CModulePath::getInstance()->getAppPathName();
	m_outDir = m_dstDir;
	::SetWindowText(GetDlgItem(IDC_EDIT2), m_dstDir.c_str());
	docInfoStatic_ = GetDlgItem(IDC_STATIC_STATUS);
	progressBarCtrl_ = GetDlgItem(IDC_PROGRESS1);
	progressBarCtrl_.SetRange ( 0, 100 );

	dynLib_.Load("doccap.dll");
	FN_CreateDOCCapObj fn = (FN_CreateDOCCapObj)dynLib_.GetProcAddress("CreateDOCCapObj");
	docCapObj_ = fn();
	docCapObj_->SetSink(this);
	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates

	FN_DestroyDOCCapObj fn = (FN_DestroyDOCCapObj)dynLib_.GetProcAddress("DestroyDOCCapObj");
	fn(docCapObj_);
	docCapObj_ = NULL;
	dynLib_.Free();

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	TCHAR szMyDocument[MAX_PATH];
	TCHAR szTempDir[MAX_PATH]; 

	lstrcpy(szTempDir, _T("C:\\WINNT\\TEMP\\"));
	if (!GetMyDir(CSIDL_PERSONAL, (const char*)szMyDocument)) {
		return 0;
	}

	LPCTSTR lpcstrFilter;

	lpcstrFilter = 
		_T("All Files (*.*)|*.*|")
		_T("Epidiascope Files (*.edf)|*.edf|")
		_T("Microsoft PowerPoint(*.ppt;*.pps;*.ppe)|*.ppt;*.pps;*.ppe|")
		_T("Microsoft Word(*.doc)|*.doc|")
		_T("Microsoft Execel(*.xls;*.xlt)|*.xls;*.xlt|")
		_T("Text File(*.txt)|*.txt|")
		_T("Microsoft Project(*.mpp;*.mpc)|*.mpp;*.mpc|")
		_T("Write Format(*.wri)|*.wri|")
		_T("WordPerfect Document(*.wpd)|*.wpd|")
		_T("Lotus 1-2-3 Sheet(*.wk?)|*.wk?|")
		_T("Freelance File(*.pre)|*.pre|")
		_T("AutoCAD Drawing File(*.dwg)|*.dwg|")
		_T("Rich Text Format(*.rtf)|*.rtf|")
		_T("");

	CString sPath = szMyDocument;

	CSSFileDialog  dlg(TRUE,  _T("edf"),NULL, OFN_EXPLORER | OFN_HIDEREADONLY, lpcstrFilter, m_hWnd);

	dlg.m_ofn.lpstrInitialDir = sPath;
	sPath += _T("..\\");
	if( dlg.DoModal()!=IDOK ) return 0;

	char szFName[_MAX_PATH];
	char szFExName[_MAX_EXT], szFExNameMid[_MAX_EXT]; 
	_splitpath(dlg.m_ofn.lpstrFile, NULL, NULL, szFName, szFExNameMid);
	strcpy(szFExName,&szFExNameMid[1]);
	docCapObj_->OpenFile(dlg.m_ofn.lpstrFile, szFExName);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnIdok3BnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	// TODO: 在此添加控件通知处理程序代码
	CString sSelectedDir;
	CFolderDialog fldDlg ( NULL, _T("请选择存放已转换文档的目录"), BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE );

	if ( IDOK == fldDlg.DoModal() ) {
		sSelectedDir = fldDlg.m_szFolderPath;
		::EnableWindow(GetDlgItem(IDOK), TRUE);
	}
	return 0;
}

LRESULT CMainDlg::OnButton1BnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	char tmp[1024];
	sprintf(tmp, "explorer.exe %s", m_outDir.c_str());
	WinExec(tmp, SW_SHOWNORMAL);
	return 0;
}

