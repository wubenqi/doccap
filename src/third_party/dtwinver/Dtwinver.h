/*
Module : DTWINVER.H
Purpose: Declaration of a comprehensive class to perform OS version detection
Created: PJN / 11-05-1996

Copyright (c) 1997 - 2010 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////////////// Defines ////////////////////////////////////

#ifndef __DTWINVER_H__                                          
#define __DTWINVER_H__

#define COSVERSION_SUITE_SMALLBUSINESS                        0x00000001
#define COSVERSION_SUITE_ENTERPRISE                           0x00000002
#define COSVERSION_SUITE_PRIMARY_DOMAIN_CONTROLLER            0x00000004
#define COSVERSION_SUITE_BACKUP_DOMAIN_CONTROLLER             0x00000008
#define COSVERSION_SUITE_TERMINAL                             0x00000010
#define COSVERSION_SUITE_DATACENTER                           0x00000020
#define COSVERSION_SUITE_PERSONAL                             0x00000040
#define COSVERSION_SUITE_WEBEDITION                           0x00000080
#define COSVERSION_SUITE_EMBEDDEDNT                           0x00000100
#define COSVERSION_SUITE_REMOTEADMINMODE_TERMINAL             0x00000200
#define COSVERSION_SUITE_UNIPROCESSOR_FREE                    0x00000400
#define COSVERSION_SUITE_UNIPROCESSOR_CHECKED                 0x00000800
#define COSVERSION_SUITE_MULTIPROCESSOR_FREE                  0x00001000
#define COSVERSION_SUITE_MULTIPROCESSOR_CHECKED               0x00002000
#define COSVERSION_SUITE_MEDIACENTER                          0x00004000
#define COSVERSION_SUITE_TABLETPC                             0x00008000
#define COSVERSION_SUITE_STARTER_EDITION                      0x00010000
#define COSVERSION_SUITE_R2_EDITION                           0x00020000
#define COSVERSION_SUITE_COMPUTE_SERVER                       0x00040000
#define COSVERSION_SUITE_STORAGE_SERVER                       0x00080000
#define COSVERSION_SUITE_SECURITY_APPLIANCE                   0x00100000
#define COSVERSION_SUITE_BACKOFFICE                           0x00200000
#define COSVERSION_SUITE_ULTIMATE                             0x00400000
#define COSVERSION_SUITE_N                                    0x00800000
#define COSVERSION_SUITE_HOME_BASIC                           0x01000000
#define COSVERSION_SUITE_HOME_PREMIUM                         0x02000000
#define COSVERSION_SUITE_HYPERV_TOOLS                         0x04000000
#define COSVERSION_SUITE_BUSINESS                             0x08000000
#define COSVERSION_SUITE_HOME_SERVER                          0x10000000
#define COSVERSION_SUITE_SERVER_CORE                          0x20000000
#define COSVERSION_SUITE_ESSENTIAL_BUSINESS_SERVER_MANAGEMENT 0x40000000
#define COSVERSION_SUITE_ESSENTIAL_BUSINESS_SERVER_MESSAGING  0x80000000

#define COSVERSION_SUITE2_ESSENTIAL_BUSINESS_SERVER_SECURITY  0x00000001
#define COSVERSION_SUITE2_CLUSTER_SERVER                      0x00000002
#define COSVERSION_SUITE2_SMALLBUSINESS_PREMIUM               0x00000004
#define COSVERSION_SUITE2_STORAGE_EXPRESS_SERVER              0x00000008
#define COSVERSION_SUITE2_STORAGE_WORKGROUP_SERVER            0x00000010
#define COSVERSION_SUITE2_STANDARD                            0x00000020
#define COSVERSION_SUITE2_E                                   0x00000040
#define COSVERSION_SUITE2_PROFESSIONAL                        0x00000080
#define COSVERSION_SUITE2_FOUNDATION                          0x00000100
#define COSVERSION_SUITE2_MULTIPOINT                          0x00000200
#define COSVERSION_SUITE2_HYPERV_SERVER                       0x00000400


////////////////////////////////// Classes ////////////////////////////////////

class COSVersion
{
public:
//Enums
  enum OS_PLATFORM
  {
    Dos = 0,
    Windows3x = 1,
    Windows9x = 2,
    WindowsNT = 3,
    WindowsCE = 4,
  };

  enum OS_TYPE
  {
    Workstation = 0,
    Server = 1,
    DomainController = 2,
  };

  enum PROCESSOR_TYPE
  {
    UNKNOWN_PROCESSOR = 0,
    IA32_PROCESSOR = 1,
    MIPS_PROCESSOR = 2,
    ALPHA_PROCESSOR = 3,
    PPC_PROCESSOR = 4,
    IA64_PROCESSOR = 5,
    AMD64_PROCESSOR = 6,
    ALPHA64_PROCESSOR = 7,
    MSIL_PROCESSOR = 8,
    ARM_PROCESSOR = 9,
    SHX_PROCESSOR = 10,
  };
  
//Defines
  typedef struct _OS_VERSION_INFO
  {
  #ifndef UNDER_CE
    //What version of OS is being emulated
    DWORD          dwEmulatedMajorVersion;
    DWORD          dwEmulatedMinorVersion;
    DWORD          dwEmulatedBuildNumber;
    OS_PLATFORM    EmulatedPlatform;
    PROCESSOR_TYPE EmulatedProcessorType; //The emulated processor type
  #ifdef _WIN32                    
    TCHAR          szEmulatedCSDVersion[128];
  #else
    char           szEmulatedCSDVersion[128];
  #endif  
    WORD           wEmulatedServicePackMajor;
    WORD           wEmulatedServicePackMinor;
  #endif

    //What version of OS is really running                 
    DWORD       dwUnderlyingMajorVersion;
    DWORD       dwUnderlyingMinorVersion;
    DWORD       dwUnderlyingBuildNumber;
    OS_PLATFORM UnderlyingPlatform;   
  #ifdef _WIN32                      
    TCHAR       szUnderlyingCSDVersion[128];
  #else  
    char        szUnderlyingCSDVersion[128];
  #endif  
    WORD    wUnderlyingServicePackMajor;
    WORD    wUnderlyingServicePackMinor;

    DWORD          dwSuiteMask;             //Bitmask of various OS suites
    DWORD          dwSuiteMask2;            //Second set of Bitmask of various OS suites
    OS_TYPE        OSType;                  //The basic OS type
  #ifndef UNDER_CE
    PROCESSOR_TYPE UnderlyingProcessorType; //The underlying processor type
  #endif

  #ifdef UNDER_CE
    TCHAR szOEMInfo[256];
    TCHAR szPlatformType[256];
  #endif
  } OS_VERSION_INFO, *POS_VERSION_INFO, FAR *LPOS_VERSION_INFO;

//Constructors / Destructors
  COSVersion(); 
  ~COSVersion();

//Methods:
  BOOL GetVersion(LPOS_VERSION_INFO lpVersionInformation);
                                 
//Please note that the return values for the following group of functions 
//are mutually exclusive for example if you are running on 
//95 OSR2 then IsWindows95 will return FALSE etc.
  BOOL IsWindows30(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows31(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows311(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsForWorkgroups(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsCE(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows95(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows95SP1(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows95B(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows95C(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows98(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows98SP1(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows98SE(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsME(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT31(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT35(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT351(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT4(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows2000(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsXPOrWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsVistaOrWindowsServer2008(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows7OrWindowsServer2008R2(LPOS_VERSION_INFO lpVersionInformation);

//Returns the various flavours of the "os" that is installed. Note that these
//functions are not completely mutually exlusive
  BOOL IsWin32sInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTPreWin2k(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsNTWorkstation(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTStandAloneServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTPDC(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTBDC(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTEnterpriseServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTDatacenterServer(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWindows2000Professional(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows2000Server(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows2000AdvancedServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows2000DatacenterServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows2000DomainController(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWindowsXP(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsXPPersonal(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsXPProfessional(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWindows7(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsVista(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWebWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsStandardWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEnterpriseWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDatacenterWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDomainControllerWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWebWindowsServer2003R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsServer2003R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsStandardWindowsServer2003R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEnterpriseWindowsServer2003R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDatacenterWindowsServer2003R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDomainControllerWindowsServer2003R2(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWebWindowsServer2008R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsServer2008R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsStandardWindowsServer2008R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEnterpriseWindowsServer2008R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDatacenterWindowsServer2008R2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDomainControllerWindowsServer2008R2(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsHomeBasicInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsHomeBasicPremium(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsBusinessInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsProfessionalInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEnterpriseInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsUltimateInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsPersonalInstalled(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWebWindowsServer2008(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsServer2008(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsStandardWindowsServer2008(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEnterpriseWindowsServer2008(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDatacenterWindowsServer2008(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDomainControllerWindowsServer2008(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsTerminalServicesInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL ISSmallBusinessServerInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEmulated64Bit(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsUnderlying64Bit(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEmbedded(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsTerminalServicesInRemoteAdminMode(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsMediaCenterInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsTabletPCInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsStarterEditionInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsComputeClusterServerEditionInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsHomeServerEditionInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsSecurityApplianceInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsBackOfficeInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNEditionInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEEditionInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsHyperVToolsInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsHyperVServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsServerCoreInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsMultiPointServerInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsServerFoundationInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsUniprocessorFree(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsUniprocessorChecked(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsMultiprocessorFree(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsMultiprocessorChecked(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEssentialBusinessServerManagement(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEssentialBusinessServerMessaging(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEssentialBusinessServerSecurity(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsClusterServerInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL ISSmallBusinessServerPremiumInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsStorageServerInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEnterpriseStorageServerInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsExpressStorageServerInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsStandardStorageServerInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWorkgroupStorageServerInstalled(LPOS_VERSION_INFO lpVersionInformation);

protected:
//Defines / typedefs
#if (defined(_WINDOWS) || defined(_DOS)) && (!defined(_WIN32) && !defined(_WIN64))  
  #define CPEX_DEST_STDCALL        0x00000000L
  #define HKEY32                   DWORD                                                               
  #define HKEY_LOCAL_MACHINE       (( HKEY32 ) 0x80000002 )    
  #define TCHAR                    char
  #define _T
  #define _tcsicmp                 strcmpi
  #define _tcscpy                  strcpy
  #define _tcslen                  strlen
  #define _istdigit                isdigit
  #define _ttoi                    atoi
  #define _tcsupr                  strupr
  #define _tcsstr                  strstr
  #define LPCTSTR                  LPCSTR
  #define ERROR_CALL_NOT_IMPLEMENTED  120
  #define REG_DWORD                ( 4 )                                                               
  #define REG_MULTI_SZ             ( 7 )
  #define VER_PLATFORM_WIN32s             0
  #define VER_PLATFORM_WIN32_WINDOWS      1
  #define VER_PLATFORM_WIN32_NT           2
  #define VER_PLATFORM_WIN32_CE           3
#endif                      
                      
#if defined(_WINDOWS) && !defined(_WIN32) && !defined(_WIN64)  
//Defines / Macros
  #define LPTSTR LPSTR

  typedef struct OSVERSIONINFO
  { 
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    char szCSDVersion[128]; 
  } OSVERSIONINFO, *POSVERSIONINFO, FAR *LPOSVERSIONINFO; 

  typedef struct _SYSTEM_INFO 
  {
    union 
    {
      DWORD dwOemId;          // Obsolete field...do not use
      struct 
      {
        WORD wProcessorArchitecture;
        WORD wReserved;
      };
    };
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
  } SYSTEM_INFO, *LPSYSTEM_INFO;

  #define PROCESSOR_ARCHITECTURE_INTEL            0
  #define PROCESSOR_ARCHITECTURE_MIPS             1
  #define PROCESSOR_ARCHITECTURE_ALPHA            2
  #define PROCESSOR_ARCHITECTURE_PPC              3
  #define PROCESSOR_ARCHITECTURE_SHX              4
  #define PROCESSOR_ARCHITECTURE_ARM              5
  #define PROCESSOR_ARCHITECTURE_IA64             6
  #define PROCESSOR_ARCHITECTURE_ALPHA64          7
  #define PROCESSOR_ARCHITECTURE_MSIL             8
  #define PROCESSOR_ARCHITECTURE_AMD64            9
  #define PROCESSOR_ARCHITECTURE_IA32_ON_WIN64    10
  #define PROCESSOR_ARCHITECTURE_UNKNOWN          0xFFFF
  
//Methods
  DWORD GetVersion();
  BOOL GetVersionEx(LPOSVERSIONINFO lpVersionInfo);
  void GetProcessorType(LPOS_VERSION_INFO lpVersionInformation);
  LONG RegQueryValueEx(HKEY32 hKey, LPSTR  lpszValueName, LPDWORD lpdwReserved, LPDWORD lpdwType, LPBYTE  lpbData, LPDWORD lpcbData);
  static BOOL WFWLoaded();                                     

//Function Prototypes
  typedef DWORD (FAR PASCAL  *lpfnLoadLibraryEx32W) (LPCSTR, DWORD, DWORD);
  typedef BOOL  (FAR PASCAL  *lpfnFreeLibrary32W)   (DWORD);
  typedef DWORD (FAR PASCAL  *lpfnGetProcAddress32W)(DWORD, LPCSTR);
  typedef DWORD (FAR __cdecl *lpfnCallProcEx32W)    (DWORD, DWORD, DWORD, DWORD, ...);
  typedef WORD  (FAR PASCAL  *lpfnWNetGetCaps)      (WORD);
  typedef BOOL  (FAR PASCAL  *lpfnGetProductInfo)   (DWORD, DWORD, DWORD, DWORD, PDWORD);

//Member variables
  lpfnLoadLibraryEx32W  m_lpfnLoadLibraryEx32W;
  lpfnFreeLibrary32W    m_lpfnFreeLibrary32W;
  lpfnGetProcAddress32W m_lpfnGetProcAddress32W;
  lpfnCallProcEx32W     m_lpfnCallProcEx32W;
  DWORD                 m_hAdvApi32;    
  DWORD                 m_hKernel32;
  DWORD                 m_lpfnRegQueryValueExA;
  DWORD                 m_lpfnGetVersionExA;
  DWORD                 m_lpfnGetVersion;
  DWORD                 m_lpfnGetSystemInfo;
  DWORD                 m_lpfnGetNativeSystemInfo;
  DWORD                 m_lpfnGetProductInfo;
#endif //defined(_WINDOWS) && (!defined(_WIN32) && !defined(_WIN64))

#ifdef _WIN32
//Function Prototypes
  typedef BOOL (WINAPI *lpfnIsWow64Process)(HANDLE, PBOOL);  
  typedef void (WINAPI *lpfnGetNativeSystemInfo)(LPSYSTEM_INFO);
  typedef BOOL (WINAPI *lpfnGetProductInfo)(DWORD, DWORD, DWORD, DWORD, DWORD*);
#endif

#if defined(_WIN32) || defined(_WINDOWS)
//Defines / Macros
  typedef struct OSVERSIONINFOEX 
  {  
    DWORD dwOSVersionInfoSize;  
    DWORD dwMajorVersion;  
    DWORD dwMinorVersion;  
    DWORD dwBuildNumber;  
    DWORD dwPlatformId;  
    TCHAR szCSDVersion[128];  
    WORD wServicePackMajor;  
    WORD wServicePackMinor;  
    WORD wSuiteMask;  
    BYTE wProductType;  
    BYTE wReserved;
  } OSVERSIONINFOEX, *POSVERSIONINFOEX, *LPOSVERSIONINFOEX;
                  
//Function Prototypes                  
  typedef BOOL (WINAPI *lpfnGetVersionEx) (LPOSVERSIONINFO);  
#endif  

#ifndef _DOS         
//Methods
  void GetNTOSTypeFromRegistry(LPOS_VERSION_INFO lpVersionInformation, BOOL bOnlyUpdateDCDetails);
  void GetProductSuiteDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation) ;
  void GetTerminalServicesRemoteAdminModeDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation);
  void GetMediaCenterDetails(LPOS_VERSION_INFO lpVersionInformation);
  void GetProductInfo(LPOS_VERSION_INFO lpVersionInformation);
  void GetTabletPCDetails(LPOS_VERSION_INFO lpVersionInformation);
  void GetStarterEditionDetails(LPOS_VERSION_INFO lpVersionInformation);
  void GetR2Details(LPOS_VERSION_INFO lpVersionInformation);
  void GetNTSP6aDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation, BOOL bUpdateEmulatedAlso);
  void GetXPSP1aDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation, BOOL bUpdateEmulatedAlso);
  void GetNTHALDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation);
#else
  BOOL GetInfoBySpawingWriteVer(COSVersion::LPOS_VERSION_INFO lpVersionInformation);
  void GetWinInfo();
#endif

#if defined(_WIN32) || defined(_WIN64)
  WORD GetNTServicePackFromRegistry();
#endif    
  WORD GetNTServicePackFromCSDString(LPCTSTR pszCSDVersion);
};

#endif //__DTWINVER_H__