// edfdriverdemo2.cpp : 定义控制台应用程序的入口点。
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
		std::cout << "\n------------获得转换驱动器捕获的数据并处理-----------\n转换后的名字: " << pPDOCCapData->document_name << std::endl;
		//std::vector<std::string>	vecEmf;
		std::cout << "转换后每页存储路径: " << std::endl;
		for (size_t i=0; i<pPDOCCapData->emfs.size(); ++i) {
			std::cout <<"页码: " << i+1 << "存储位置: " << pPDOCCapData->emfs[i] << std::endl;
		}
		std::cout << "--------------------处理完捕获的数据-----------------\n"<<std::endl;
	}

	virtual void OnDOCImpInfo(WPARAM wParam, LPARAM lParam) {
		switch(wParam) {
		case DOCIMPINFO_BEGIN:
			std::cout << "开始文档转换，请耐心等待......" << std::endl;
			//pageSize_ = 0;
			break;
		case DOCIMPINFO_IMPORT:
			std::cout << "正在转换文档，转换第 " << lParam << " 页" << std::endl;
			//pageSize_++;
			break;
		case DOCIMPINFO_PROCESS:
			std::cout << "开始处理文档，请耐心等待....." << std::endl;
			break;
		case DOCIMPINFO_PROCESSING:
			std::cout << "正在处理文档，处理第 " << lParam << " 页" << std::endl;
			break;
		case DOCIMPINFO_END:
			std::cout << "文档转换成功:)" << std::endl;
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
