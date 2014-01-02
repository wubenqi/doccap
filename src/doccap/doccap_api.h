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
 * ת�������������Ժ�ص��ṹ
 */
struct DOCCapData {
	std::string					document_name;	// �ļ���
	std::vector<std::string>	emfs;			// ת�����ͼƬ�б�
};

/**
 * �ص�����,��Ҫʵ������ͨ��IDOCCapBaseObj��SetSink�ӿ�ע�ᵽEDFת��������������
 * һ��ת�����������������Ժ�,����ִ�д�OnDOCCapData����
 */
class IDOCCapBaseObjSink {
public:
	/**
	 * ���������񵽵������Ժ�,�����ص�ִ�д˺���
	 * �������:
	 *	DOCCapData*

	 //���´�������ο�:

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
	 * �ĵ���ת�������У�ͨ���˺����ص���ǰת����״̬��Ϣ
	 * �������: wParam״̬��, lParam��״̬����Ӧ�Ĳ���
	 *	wParam
	 *		DOCIMPINFO_BEGIN:		��ʼ�����ĵ�ת��, lParam��Ч
	 *		DOCIMPINFO_IMPORT:		�����������ĵ�ת����lParamΪת���ĵ�ǰҳ��
	 *		DOCIMPINFO_PROCESS:		�������Ѿ����ת����׼���������ݴ��� lParam��Ч
	 *		DOCIMPINFO_PROCESSING:	�������ݴ��� lParamΪ����ĵ�ǰҳ��
	 *		DOCIMPINFO_END:			�����ĵ�ת���� lParam��Ч
	 * 
		//���´�������ο�:

		int pageSize_;
		int pos_;
		CProgressBarCtrl	progressBarCtrl_;
		CStatic				docInfoStatic_;

		void CMainDlg::OnDOCImpInfo(WPARAM wParam, LPARAM lParam) {
			char tmp[256];
			switch(wParam) {
			case DOCIMPINFO_BEGIN:
				progressBarCtrl_.SetPos(0);
				docInfoStatic_.SetWindowText("��ʼ�ĵ�ת���������ĵȴ�......");
				pageSize_ = 0;
				pos_ = 0;
				break;
			case DOCIMPINFO_IMPORT:
				sprintf(tmp, "����ת���ĵ���ת���� %d ҳ", lParam);
				docInfoStatic_.SetWindowText(tmp);
				pageSize_++;
				break;
			case DOCIMPINFO_PROCESS:
				docInfoStatic_.SetWindowText("���ڴ����ĵ��������ĵȴ�......");
				break;
			case DOCIMPINFO_PROCESSING:
				if(pageSize_!=0) {
					pos_ += 100/pageSize_;
					progressBarCtrl_.SetPos(pos_);
				}
				break;
			case DOCIMPINFO_END:
				sprintf(tmp, "�ĵ�ת���ɹ�:)      ���Ŀ¼: %s", m_outDir.c_str());
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
 * ת�������������ṩ�ӿ��࣬�˶�����FN_CreateDOCCapObj������������
 * �������Ժ󼴿�ʹ��
 *	������ֱ��ʹ��OpenFile�������ĵ�ת��
 *	Ҳ���Խ��ĵ���ӡ��Epidiascope Printer Driver�����ӡ���������ĵ�ת�����������ݽ��д���
 */
class IDOCCapBaseObj {
public:
	/** 
	 * ת���ĵ�
	 * �������:
	 *	lpszFilePath: ��ת���ĵ�ȫ·���ļ���
	 *	lpszExtName:	��ת���ĵ�����չ��
	 * ����ֵ:
	 *  -1:����
	 *  0: ��ʼת��
	 * ˵��,����ص����󲶻���������ת���ɹ�,������ܷ�����δ֪����
	 */
	virtual int OpenFile(const char* file_name, const char* ext_name) = 0;

	//ע��ص��ӿڶ���
	virtual void SetSink(IDOCCapBaseObjSink* sink) = 0;
};

//����IDOCCapBaseObj�����DLL��������
typedef IDOCCapBaseObj* (WINAPI *FN_CreateDOCCapObj) ();
//����IDOCCapBaseObj�����DLL��������
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
 * FN_InstallDriver,��װ�����ӡ����DLL��������
 * �������:
 *	ָ��һ���Ѿ����ڵ�Ŀ¼��
 * ����ֵ:
 *	DocCapError
 */
typedef int (WINAPI *FN_InstallDriver) (const char* spl_dir);

//ж�������ӡ����DLL��������
typedef int (WINAPI* FN_RemoveDriver) ();


#endif // DOCCAP_IDOCCAP_BASE_OBJECT_H_

