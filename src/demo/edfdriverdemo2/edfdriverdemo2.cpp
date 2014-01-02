// edfdriverdemo2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <windows.h>
#include <signal.h>
#include <iostream>
#include "../doccap/doccap_api.h"
#include "LoadLibrary.h"

bool g_running_ = true;

/*** Signal Handler ***/
void _OnSignal(int s) {
	g_running_ = false;
}

class DOCCapObjApp : 
	public IDOCCapBaseObjSink {
public:
	DOCCapObjApp() {
		docCapObj_ = NULL;
	}

	~DOCCapObjApp() {
		Destroy();
	}

	BOOL Create() {
		if(!docCapLoad_.Load(_T("doccap.dll")))
			return FALSE;
		FN_CreateDOCCapObj pfn = (FN_CreateDOCCapObj)docCapLoad_.GetProcAddress("CreateDOCCapObj");
		if(pfn==NULL)
			return FALSE;
		docCapObj_ = pfn();
		if (docCapObj_==NULL)
			return FALSE;
		docCapObj_->SetSink(this);
		return TRUE;
	}

	void Destroy() {
		if (docCapObj_) {
			FN_DestroyDOCCapObj pfn = (FN_DestroyDOCCapObj)docCapLoad_.GetProcAddress("DestroyDOCCapObj");
			if(pfn!=NULL)
				pfn(docCapObj_);
			docCapObj_ = NULL;
		}
		docCapLoad_.Free();
	}

	void OpenFile(LPCTSTR lpszFilePath, LPCTSTR lpszExtName) {
		if (docCapObj_) {
			docCapObj_->OpenFile(lpszFilePath, lpszExtName);
		}
	}

public:
	virtual void OnDOCCapData(DOCCapData *pPDOCCapData) {
		std::cout << "\n------------���ת����������������ݲ�����-----------\nת���������: " << pPDOCCapData->document_name << std::endl;
		//std::vector<std::string>	vecEmf;
		std::cout << "ת����ÿҳ�洢·��: " << std::endl;
		for (size_t i=0; i<pPDOCCapData->emfs.size(); ++i) {
			std::cout <<"ҳ��: " << i+1 << "�洢λ��: " << pPDOCCapData->emfs[i] << std::endl;
		}
		std::cout << "--------------------�����겶�������-----------------\n"<<std::endl;
	}

	virtual void OnDOCImpInfo(WPARAM wParam, LPARAM lParam) {
		switch(wParam) {
		case DOCIMPINFO_BEGIN:
			std::cout << "��ʼ�ĵ�ת���������ĵȴ�......" << std::endl;
			//pageSize_ = 0;
			break;
		case DOCIMPINFO_IMPORT:
			std::cout << "����ת���ĵ���ת���� " << lParam << " ҳ" << std::endl;
			//pageSize_++;
			break;
		case DOCIMPINFO_PROCESS:
			std::cout << "��ʼ�����ĵ��������ĵȴ�....." << std::endl;
			break;
		case DOCIMPINFO_PROCESSING:
			std::cout << "���ڴ����ĵ�������� " << lParam << " ҳ" << std::endl;
			break;
		case DOCIMPINFO_END:
			std::cout << "�ĵ�ת���ɹ�:)" << std::endl;
			//pageSize_ = 0;
			break;
		default:
			break;
		}
	}
private:
	IDOCCapBaseObj* docCapObj_;
	CLoadLibrary docCapLoad_;
	//int pageSize_;
};

int _tmain(int argc, _TCHAR* argv[]) 
{
	signal(SIGINT, _OnSignal);
	DOCCapObjApp app;
	app.Create();
	if (argc >= 3 ) {
		app.OpenFile(argv[1], argv[2]);
	}
	MSG msg;
	while(g_running_) {
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		} else {
			Sleep(1);
		}
	}
	app.Destroy();
	return 0;
}
