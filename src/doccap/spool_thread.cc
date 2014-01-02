// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/spool_thread.h"

#include "doccap/doc_imp_info_view.h"
#include "doccap/doccap_api.h"
#include "doccap/spool_to_emf.h"

//#include "doccap/doccap_obj.h"
/*

#include "StdAfx.h"
#include <windows.h>
//#include "../common/errors.h"
//#include "printutil.h"
#include "Knemf.h"
#include "osver/osver.h"
#include "DOCCapObj.h"
#include "SplThread.h"

//#include "KNES2App.h"
//#include "Global.h"
//#include "msg.h"
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOOL IsPrinterHandle( HANDLE hPrinter )
{
    DWORD       cbNeeded;
    DWORD       Error;
    BOOL        fRet = TRUE;

    // Test if the handle is to a printer by asking how big
    // a buffer is needed to describe it.
    if( !GetPrinter(hPrinter, 2, (LPBYTE)NULL, 0, &cbNeeded ))
    {
        // The function failed so the handle might not be a printer
        Error = GetLastError( );
        fRet = FALSE;

        if( Error == ERROR_INSUFFICIENT_BUFFER )
        {
            // Apparently the handle was a printer, but as expected
            // We didn't give it a buffer
            fRet = TRUE;
        }
        // Else, the handle is not to a valid printer
    }
    return fRet;

} /* end of function IsPrinterHandle */


SpoolThread::SpoolThread(const char* printer_name, const char* spl_dir, const char* temp_dir, HWND window) {
	if (printer_name) {
		printer_name_.assign(printer_name);
	}
	printer_handle_ = NULL;
	parent_window_ = window;
	memset(spooling_flags_, 0, sizeof(int)*MAX_PRINTERJOB);
	if (spl_dir) {
		spl_dir_.assign(spl_dir);
	}
	if (temp_dir) {
		temp_dir_.assign(temp_dir);
	}
}

SpoolThread::~SpoolThread() {
}

BOOL SpoolThread::Start() {
	return CreateThread();
}

BOOL SpoolThread::InitInstance() {
	if(!SelectPrinter()) {
		return FALSE;
	}
	return TRUE;
}

int SpoolThread::ExitInstance() {
	if (printer_handle_)
		ClosePrinter(printer_handle_);
	return 0;
}

BOOL SpoolThread::Run() {
    HANDLE                  hPrinterNotification;   /* The PrinterChangeNotification object */
    DWORD                   WaitResult;             /* High level info from the PrinterChangeNotification */
    DWORD                   OldFlags;               /* Temporary variable */
    PRINTER_NOTIFY_INFO *   pNotification = NULL;   /* The stuff that changed to cause the notification */
    WORD                    JobFields[] = 
        {                                   /* Job Fields we want notifications for */
        JOB_NOTIFY_FIELD_STATUS,            /* Status bits */
        JOB_NOTIFY_FIELD_STATUS_STRING,     /* Status String */
        JOB_NOTIFY_FIELD_DOCUMENT,          /* Document Name */
        JOB_NOTIFY_FIELD_USER_NAME,         /* Name of Documents owner */
        JOB_NOTIFY_FIELD_TOTAL_PAGES,       /* Total pages in the job */
        JOB_NOTIFY_FIELD_PAGES_PRINTED,     /* # of pages of the job that have printed */
        JOB_NOTIFY_FIELD_SUBMITTED,         /* The time and date job was submitted */
        JOB_NOTIFY_FIELD_BYTES_PRINTED,     /* # of bytes sent to printer */
        JOB_NOTIFY_FIELD_TOTAL_BYTES        /* Total bytes in the print job */
        };
    WORD                    PrinterFields[] = 
        {
        PRINTER_NOTIFY_FIELD_STATUS,        /* Status bits of printer queue */
        PRINTER_NOTIFY_FIELD_CJOBS,         /* # of jobs in printer queue */
        PRINTER_NOTIFY_FIELD_PRINTER_NAME,  /* Name of the printer queue */
        PRINTER_NOTIFY_FIELD_SERVER_NAME,   /* Name of the server when the queue is remote */
        PRINTER_NOTIFY_FIELD_SHARE_NAME     /* The queue's sharename */
        };
    PRINTER_NOTIFY_OPTIONS_TYPE Notifications[2] =                  
        {                                   /* The notification we want to be notified of */
            {
                JOB_NOTIFY_TYPE,            /* We want notifications on print jobs */
                0,
                0,
                0,
                sizeof(JobFields)/sizeof(JobFields[0]), /* We specified 9 fields in the JobFields array */
                JobFields                   /* Precisely which fields we want notifications for */
            },
            {
                PRINTER_NOTIFY_TYPE,        /* We want notification on the printer queue too */
                0,
                0,
                0,
                sizeof(PrinterFields)/sizeof(PrinterFields[0]), /* How many printer fields? */
                PrinterFields               /* Precisely what printer fields we want */
            }
        };
    PRINTER_NOTIFY_OPTIONS  NotificationOptions = 
        {
            2,                                  /* Version of structure, see docs */
            PRINTER_NOTIFY_OPTIONS_REFRESH,     /* Options for FindNextPrinterChangeNotification */
            2,                                  /* # of Printer_Notify_Info structures */
            Notifications                       /* What we want notifications of */
        };

    hPrinterNotification = FindFirstPrinterChangeNotification(
        printer_handle_,           /* The printer of interest */
        PRINTER_CHANGE_DELETE_JOB,      /* We need to know when a job is removed */
        0,                              /* reserved */
        &NotificationOptions);          /* The details of what notifications that are needed */

    /* Check for an error */
    if (hPrinterNotification == INVALID_HANDLE_VALUE) {
		// ErrorBox(GetLastError(), "FindFirstPrinterChangeNotification");
        return 0;
    }
	
	HandlePrinterJob();	
	
	while(!m_bDone && (hPrinterNotification != INVALID_HANDLE_VALUE)) {
 		if(WaitForSingleObject(hPrinterNotification, 50) == WAIT_OBJECT_0) {
            /* get the changes and reset the notification */
            if (!FindNextPrinterChangeNotification(hPrinterNotification,
                &WaitResult,            /* for the PRINTER_CHANGE_DELETE_JOB notice */
                &NotificationOptions,   /* The notifications */
                (void **)&pNotification))        /* address of pointer that gets what changed */
            {
//                ErrorBox(GetLastError(), "FindNextPrinterChangeNotification");
            }
            
            /* Did a notification overflow occur? */
            if (pNotification && pNotification->Flags & PRINTER_NOTIFY_INFO_DISCARDED) {
                /* An overflow of notifications occured, must refresh to continue */

                OldFlags = NotificationOptions.Flags;
                
                NotificationOptions.Flags = PRINTER_NOTIFY_OPTIONS_REFRESH;

                FreePrinterNotifyInfo(pNotification);
                if (!FindNextPrinterChangeNotification(hPrinterNotification,
                    &WaitResult,
                    &NotificationOptions,
                    (void **)&pNotification)) {
//                    ErrorBox(GetLastError(), "FindNextPrinterChangeNotification refresh call.");
                }
                NotificationOptions.Flags = OldFlags;

                /* Start Over with Refreshed Data */
				HandlePrinterJob(1);
                //Refresh(ThreadParam.hWnd, &Queue);

            }

            /* process the notification of changes */
            if (WaitResult & PRINTER_CHANGE_DELETE_JOB) {
                /* a job was deleted so start over clean */
				HandlePrinterJob(1);
            } else {
                /* track and show the changes */
				HandlePrinterJob(1);
            }
            FreePrinterNotifyInfo(pNotification);
            pNotification = NULL;
		
		}
	}
	return TRUE;
}

BOOL SpoolThread::SelectPrinter() {
    
    // Loop unless dialog canceled
    if (OpenPrinter((char*)printer_name_.c_str(), &printer_handle_, NULL) && IsPrinterHandle ( printer_handle_ )) {
        //hThread = StartNotifications(hWnd, hNewPrinter, *pThreadHandle);
        return TRUE;
	} else {
		// nope, user typed in something that was wrong
       
		if (printer_handle_)
            ClosePrinter ( printer_handle_ );

        // if (MessageBox(parent_window_,  "文档转换器不存在，你可能需要重新安装 Kernel DVW-2002 文档转换器", "文档转换器选择出错", MB_OKCANCEL | MB_ICONSTOP) 
        //    == IDCANCEL)
		//	return FALSE;;
    }
    return FALSE;

} /* end of function OnSelecPrinter */

/***************************************************************************
 *
 * Core Application Printer Queue Processing Functions
 *
 **************************************************************************/

//如果是第一次，则删除打印队列任务
//否则，开始处理

BOOL SpoolThread::HandlePrinterJob(int nFlag) {
    DWORD               cByteNeeded, 
                        cByteUsed; 
    int                 nReturned=0;
    JOB_INFO_2          *pJobStorage = NULL;
    PRINTER_INFO_2      *pPrinterInfo = NULL;
    HANDLE              hPrinter = printer_handle_;
	
	std::string spl_file_name;
    
	if (hPrinter) {
        /* Get the buffer size needed */
        if (!GetPrinter(hPrinter, 2, NULL, 0, &cByteNeeded)) {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                goto Fail;
        }

        pPrinterInfo = (PRINTER_INFO_2 *)malloc(cByteNeeded);
        if (!(pPrinterInfo))
            /* failure to allocate memory */
            goto Fail;
        
        /* get the printer info */
        if (!GetPrinter(hPrinter, 2, (LPBYTE)pPrinterInfo, cByteNeeded, &cByteUsed)) {
            /* failure to access the printer */
            free(pPrinterInfo);
            pPrinterInfo = NULL;
            goto Fail;
        } 
        
        /* Get job storage space */
        if (!EnumJobs(hPrinter, 
                 0, 
                 (pPrinterInfo)->cJobs, 
                 2, 
                 NULL, 
                 0,
                 (LPDWORD)&cByteNeeded,
                 (LPDWORD)&nReturned))
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                goto Fail;
        }

        pJobStorage = (JOB_INFO_2 *)malloc(cByteNeeded);
        if (!pJobStorage)
            /* failure to allocate Job storage space */
            goto Fail;
        
        ZeroMemory(pJobStorage, cByteNeeded);

        /* get the list of jobs */
        if (!EnumJobs(hPrinter, 
                 0, 
                 (pPrinterInfo)->cJobs, 
                 2, 
                 (LPBYTE)pJobStorage, 
                 cByteNeeded,
                 (LPDWORD)&cByteUsed,
                 (LPDWORD)&nReturned))
        {
            goto Fail;
        }

        /* Update the count of jobs
         * It is possible for cJobs != nReturned after this sequence so
         * we reset the printer data structure to only reflect what we
         * actually retrieved this time around
         */
        (pPrinterInfo)->cJobs = nReturned;

    }
    else
        goto Fail;

    /* Fill in the Printer Queue with the jobs we retrieved */
    //开始处理
	if (nFlag == 0)
	{
		for (int i = 0; i < nReturned; i++)
		{
			SetJob(printer_handle_, pJobStorage[i].JobId, 0, (LPBYTE)pJobStorage, JOB_CONTROL_DELETE);
		}
	}
	else
	{
		for (int i = 0; i < nReturned; i++)
		{
/*			if(pJobStorage[i].pStatus!=NULL)
			{
				GetSplFileName(pJobStorage[i].JobId, strSplFileName);
				CKnEmf  knEmf(strSplFileName);
				vector<string> svecEmf = knEmf.GetEmfFileName();
				int j;
				for(j=0; j<svecEmf.size(); j++)
					TRACE("EMF LIST: %s\n", svecEmf[j].c_str());
				TRACE("EMF LIST: %d  %d  %d\n", pJobStorage[i].TotalPages, pJobStorage[i].PagesPrinted, spooling_flags_[i]++);

				SetJob(m_hPrinter, pJobStorage[i].JobId, 0, (LPBYTE)pJobStorage, JOB_CONTROL_DELETE);
				spooling_flags_[i] = 0;

			}
			else
			{
*/
				switch(pJobStorage[i].Status)
				{
				
				case JOB_STATUS_SPOOLING:
					{
						if (spooling_flags_[i] == 0) {
							::PostMessage(parent_window_, WM_DOCIMPINFOMSG, DOCIMPINFO_BEGIN, 0);
						}
						// TRACE("EMF LIST: %d  %d  %d  %d\n", pJobStorage[i].TotalPages, pJobStorage[i].Position, spooling_flags_[i], pJobStorage[i].Size);
						::PostMessage(parent_window_, WM_DOCIMPINFOMSG, DOCIMPINFO_IMPORT, (LPARAM)pJobStorage[i].TotalPages);
						spooling_flags_[i]++;
					}
					break;
				default:
					{
						if(spooling_flags_[i])
						{

							::PostMessage(parent_window_, WM_DOCIMPINFOMSG, DOCIMPINFO_PROCESS, 0);

							DOCCapData* data = new DOCCapData;
							GetSplFileName(pJobStorage[i].JobId, &spl_file_name);

							SploolToEmf spool_to_emf(spl_file_name, temp_dir_);
							data->document_name = pJobStorage[i].pDocument;
							data->emfs = spool_to_emf.GetEmfFileName();

							PostMessage(parent_window_, WM_MSGDOCCAPDATA, MAKEWPARAM(0,0),(LPARAM)data);
							SetJob(printer_handle_, pJobStorage[i].JobId, 0, (LPBYTE)pJobStorage, JOB_CONTROL_DELETE);
							spooling_flags_[i] = 0;
							
							::PostMessage(parent_window_, WM_DOCIMPINFOMSG, DOCIMPINFO_END, 0);
							//::SetEvent(g_hDOCCapEvent);
						}
						break;
					}	
				}
//		}
		}
	}
	
    free(pJobStorage);
    free(pPrinterInfo);
    
    return TRUE;

Fail:

    free(pJobStorage);
    free(pPrinterInfo);
    return FALSE;

} /* end of function GetQueue */


BOOL SpoolThread::GetSplFileName(int job_id, std::string* out_spl_name) {
	if (!out_spl_name) {
		return FALSE;
	}

	char str_job_id[MAX_PATH];

	wsprintf(str_job_id, "%s\\%05d.SPL", spl_dir_.c_str(), job_id);
	*out_spl_name = str_job_id;
	return TRUE;
}
