// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_IDOCCAP_API_H_
#define DOCCAP_IDOCCAP_API_H_
#pragma once

#include <string>
#include <vector>
#include <Windows.h>


#define		DOCIMPINFO_BEGIN		0
#define		DOCIMPINFO_IMPORT		1
#define		DOCIMPINFO_PROCESS		2
#define		DOCIMPINFO_PROCESSING	3
#define		DOCIMPINFO_END			4

/**
 * 转换器捕获数据以后回调结构
 */
struct DOCCapData {
	std::string					document_name;	// 文件名
	std::vector<std::string>	emfs;			// 转换后的图片列表
};

/**
 * 回调对象,需要实例化后通过IDOCCapBaseObj的SetSink接口注册到EDF转换驱动处理器里
 * 一旦转换驱动器捕获到数据以后,立即执行此OnDOCCapData方法
 */
class IDOCCapBaseObjSink {
public:
	/**
	 * 驱动器捕获到到数据以后,立即回调执行此函数
	 * 输入参数:
	 *	DOCCapData*

	 //如下代码仅供参考:

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

			m_outDir = m_dstDir + doc_name;
			DWORD dwAttributes = ::GetFileAttributes(m_outDir.c_str());
			if( (dwAttributes == 0xFFFFFFFF) || (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) )
				::CreateDirectory(m_outDir.c_str(), NULL); 

			for (size_t i=0; i<pPDOCCapData->emfs.size(); ++i) {
				CxFile in_file, out_file;
				in_file.Open(pPDOCCapData->emfs[i].c_str(), "rb");
				char* data = new char[in_file.Size()];
				in_file.Read(data, 1, in_file.Size());
				char out_file_path[1024];
				sprintf(out_file_path, "%s\\%d.jpg", m_outDir.c_str(), i);
				out_file.Open(out_file_path, "wb");
				out_file.Write(data, 1, in_file.Size());
				in_file.Close();
				out_file.Close();
				delete [] data;
			}
		}
	 */

	virtual void OnDOCCapData(DOCCapData *data) = 0;

	/**
	 * 文档在转换过程中，通过此函数回调当前转换的状态信息
	 * 输入参数: wParam状态码, lParam此状态码相应的参数
	 *	wParam
	 *		DOCIMPINFO_BEGIN:		开始进行文档转换, lParam无效
	 *		DOCIMPINFO_IMPORT:		驱动器进行文档转换，lParam为转换的当前页码
	 *		DOCIMPINFO_PROCESS:		驱动器已经完成转换，准备进行数据处理， lParam无效
	 *		DOCIMPINFO_PROCESSING:	进行数据处理， lParam为处理的当前页码
	 *		DOCIMPINFO_END:			结束文档转换， lParam无效
	 * 
		//如下代码仅供参考:

		int pageSize_;
		int pos_;
		CProgressBarCtrl	progressBarCtrl_;
		CStatic				docInfoStatic_;

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
	 */
	virtual void OnDOCImpInfo(WPARAM wParam, LPARAM lParam) = 0;
};

/**
 * 转换驱动器服务提供接口类，此对象由FN_CreateDOCCapObj导出函数创建
 * 创建好以后即可使用
 *	您可以直接使用OpenFile来进行文档转换
 *	也可以将文档打印到Epidiascope Printer Driver虚拟打印机来进行文档转换并捕获数据进行处理
 */
class IDOCCapBaseObj {
public:
	/** 
	 * 转换文档
	 * 输入参数:
	 *	lpszFilePath: 被转换文档全路径文件名
	 *	lpszExtName:	被转换文档的扩展名
	 * 返回值:
	 *  -1:出错
	 *  0: 开始转换
	 * 说明,如果回调对象捕获到了数据则转换成功,否则可能发生了未知错误。
	 */
	virtual int OpenFile(const char* file_name, const char* ext_name) = 0;

	//注册回调接口对象
	virtual void SetSink(IDOCCapBaseObjSink* sink) = 0;
};

//创建IDOCCapBaseObj对象的DLL导出函数
typedef IDOCCapBaseObj* (WINAPI *FN_CreateDOCCapObj) ();
//销毁IDOCCapBaseObj对象的DLL导出函数
typedef long (WINAPI *FN_DestroyDOCCapObj) (IDOCCapBaseObj* pDOCCapBaseObj);

enum DocCapError {
	kDocCapErrorOK = 0,				// Sucessful
	kDocCapErrorOKButNotRestart,	// Sucessful, but no default spooler default dir, set spoolerdir key, and must restart.
	kDocCapErrorSpooler,			// Not start spooler
	kDocCapErrorProcessor,			// AddPrinterProcessor error 
	kDocCapErrorPort,				// AddPrinterPort error
	kDocCapErrorDriver,				// AddPrinterDriver error
	kDocCapErrorPrinter,			// AddKnPrinter error
	kDocCapErrorNotSupported,		// Unsupported os
};

/**
 * FN_InstallDriver,安装虚拟打印机的DLL导出函数
 * 输入参数:
 *	指定一个已经存在的目录。
 * 返回值:
 *	DocCapError
 */
typedef int (WINAPI *FN_InstallDriver) (const char* spl_dir);

//卸载虚拟打印机的DLL导出函数
typedef int (WINAPI* FN_RemoveDriver) ();


#endif // DOCCAP_IDOCCAP_BASE_OBJECT_H_

