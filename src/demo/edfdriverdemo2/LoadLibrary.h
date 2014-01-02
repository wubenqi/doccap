#pragma once

/////////////////////////////////////////////////////////////////////////////
// CLoadLibrary

class CLoadLibrary
{
public:
	HINSTANCE m_hInst;

	CLoadLibrary() : m_hInst(NULL)
	{
	}
	CLoadLibrary(LPCTSTR pstrFileName)
	{
		m_hInst = NULL;
		Load(pstrFileName);
	}
	~CLoadLibrary()
	{
		Free();
	}
	BOOL Load(LPCTSTR pstrFileName, DWORD dwFlags = 0)
	{
		ATLASSERT(!::IsBadStringPtr(pstrFileName, MAX_PATH));
		Free();
		m_hInst = ::LoadLibraryEx(pstrFileName, NULL, dwFlags);
		return m_hInst != NULL;
	}
	void Free()
	{
		if( IsLoaded() ) {
			::FreeLibrary(m_hInst);
			m_hInst = NULL;
		}
	}
	HINSTANCE Detach()
	{
		HINSTANCE hInst = m_hInst;
		m_hInst = NULL;
		return hInst;
	}
	BOOL IsLoaded() const 
	{ 
		return m_hInst!=NULL; 
	};
	FARPROC GetProcAddress(LPCSTR pszFuncName) const
	{ 
		ATLASSERT(!::IsBadStringPtrA(pszFuncName,-1));
		ATLASSERT(IsLoaded()); 
		return ::GetProcAddress(m_hInst, pszFuncName);
	}
	BOOL GetFileName(LPTSTR pstrFilename, DWORD cchMax = MAX_PATH) const
	{
		ATLASSERT(IsLoaded());
		return ::GetModuleFileName(m_hInst, pstrFilename, cchMax);
	}
	operator HINSTANCE() const
	{ 
		return m_hInst; 
	}; 
};
