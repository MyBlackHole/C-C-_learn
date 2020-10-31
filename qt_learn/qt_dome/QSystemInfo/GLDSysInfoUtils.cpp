#include "GLDSysInfoUtils.h"

#include <lm.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32.lib")

#define _CRT_SECURE_NO_DEPRECATE

const int BUFSIZE          = 80;
const LPCWSTR productInfo  = L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions";
const LPCWSTR servicePack6 = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009";
const LPCWSTR uninstall    = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";

typedef void (WINAPI *PGetNativeSystemInfo)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);


class SystemInfo
{
public:
    static SystemInfo* getInstance()
    {
        if (nullptr == m_pSysInfo)
        {
            m_pSysInfo = new SystemInfo();

            if (!m_pSysInfo->init())
            {
                return nullptr;
            }
        }

        return m_pSysInfo;
    }

private:
    SystemInfo()
    {

    }

private:
    bool init()
    {
        BOOL canDetect = TRUE;
        PGetNativeSystemInfo pGNSI = NULL;

        m_bOsVersionInfoEx = FALSE;
        m_nWinVersion = Windows;
        m_nWinEdition = EditionUnknown;
        memset(m_szServicePack, 0, sizeof(m_szServicePack));

        // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
        ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
        m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

        __pragma(warning(push))
        __pragma(warning(disable:4996))
        if (!(m_bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&m_osvi)))
        {
            // If that fails, try using the OSVERSIONINFO structure.
            m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

            if (!GetVersionEx((OSVERSIONINFO *)&m_osvi))
            {
                canDetect = FALSE;
                return false;
            }
        }
        __pragma(warning(pop))

        DWORD major = 0;
        DWORD minor = 0;

        if (getWinMajorMinorVersion(major, minor))
        {
            m_osvi.dwMajorVersion = major;
            m_osvi.dwMinorVersion = minor;
        }
        else if (m_osvi.dwMajorVersion == 6 && m_osvi.dwMinorVersion == 2)
        {
            OSVERSIONINFOEXW osvi;
            ULONGLONG cm = 0;
            cm = VerSetConditionMask(cm, VER_MINORVERSION, VER_EQUAL);
            ZeroMemory(&osvi, sizeof(osvi));
            osvi.dwOSVersionInfoSize = sizeof(osvi);
            osvi.dwMinorVersion = 3;

            if (VerifyVersionInfoW(&osvi, VER_MINORVERSION, cm))
            {
                m_osvi.dwMinorVersion = 3;
            }
        }

        pGNSI = (PGetNativeSystemInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"),
                                                     "GetNativeSystemInfo");

        if (NULL != pGNSI)
        {
            pGNSI(&m_SysInfo);
        }
        else
        {
            GetSystemInfo(&m_SysInfo);
        }

        if (canDetect)
        {
            detectWindowsVersion();
            detectWindowsEdition();
            detectWindowsServicePack();
        }

        return true;
    }

    void detectWindowsVersion()
    {
        if (m_bOsVersionInfoEx)
        {
            switch (m_osvi.dwPlatformId)
            {
            case VER_PLATFORM_WIN32s:
            {
                m_nWinVersion = Windows32s;
            }
            break;

            // Test for the Windows 95 product family.
            case VER_PLATFORM_WIN32_WINDOWS:
            {
                switch (m_osvi.dwMajorVersion)
                {
                case 4:
                {
                    switch (m_osvi.dwMinorVersion)
                    {
                    case 0:
                        if (m_osvi.szCSDVersion[0] == 'B' || m_osvi.szCSDVersion[0] == 'C')
                        {
                            m_nWinVersion = Windows95OSR2;
                        }
                        else
                        {
                            m_nWinVersion = Windows95;
                        }
                        break;

                    case 10:
                        if (m_osvi.szCSDVersion[0] == 'A')
                        {
                            m_nWinVersion = Windows98SE;
                        }
                        else
                        {
                            m_nWinVersion = Windows98;
                        }
                        break;

                    case 90:
                        m_nWinVersion = WindowsMillennium;
                        break;
                    }
                }
                break;
                }
            }
            break;

            // Test for the Windows NT product family.
            case VER_PLATFORM_WIN32_NT:
            {
                switch (m_osvi.dwMajorVersion)
                {
                case 3:
                    m_nWinVersion = WindowsNT351;
                    break;

                case 4:
                    switch (m_osvi.wProductType)
                    {
                    case 1:
                        m_nWinVersion = WindowsNT40;
                        break;

                    case 3:
                        m_nWinVersion = WindowsNT40Server;
                        break;
                    }
                    break;

                case 5:
                {
                    switch (m_osvi.dwMinorVersion)
                    {
                    case 0:
                        m_nWinVersion = Windows2000;
                        break;

                    case 1:
                        m_nWinVersion = WindowsXP;
                        break;

                    case 2:
                    {
                        if (m_osvi.wSuiteMask == VER_SUITE_WH_SERVER)
                        {
                            m_nWinVersion = WindowsHomeServer;
                        }
                        else if (m_osvi.wProductType == VER_NT_WORKSTATION &&
                            m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                        {
                            m_nWinVersion = WindowsXPProfessionalx64;
                        }
                        else
                        {
                            m_nWinVersion = ::GetSystemMetrics(SM_SERVERR2) == 0 ? WindowsServer2003 : WindowsServer2003R2;
                        }
                    }
                    break;
                    }
                }
                break;

                case 6:
                {
                    switch (m_osvi.dwMinorVersion)
                    {
                    case 0:
                    {
                        m_nWinVersion = m_osvi.wProductType == VER_NT_WORKSTATION ? WindowsVista : WindowsServer2008;
                    }
                    break;

                    case 1:
                    {
                        m_nWinVersion = m_osvi.wProductType == VER_NT_WORKSTATION ? Windows7 : WindowsServer2008R2;
                    }
                    break;

                    case 2:
                    {
                        m_nWinVersion = m_osvi.wProductType == VER_NT_WORKSTATION ? Windows8 : WindowsServer2012;
                    }
                    break;

                    case 3:
                    {
                        m_nWinVersion = m_osvi.wProductType == VER_NT_WORKSTATION ? Windows8Point1 : WindowsServer2012R2;
                    }
                    break;
                    }
                }
                break;

                case 10:
                {
                    switch (m_osvi.dwMinorVersion)
                    {
                    case 0:
                    {
                        m_nWinVersion = m_osvi.wProductType == VER_NT_WORKSTATION ? Windows10 : WindowsServer2016TechnicalPreview;
                    }
                    break;

                    }
                }
                break;
                }
            }
            break;
            }
        }
        else // Test for specific product on Windows NT 4.0 SP5 and earlier
        {
            HKEY hKey;
            WCHAR szProductType[BUFSIZE];
            DWORD dwBufLen = BUFSIZE;
            LONG lRet;

            lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                productInfo,
                                0,
                                KEY_QUERY_VALUE,
                                &hKey);

            if (lRet != ERROR_SUCCESS)
            {
                return;
            }

            lRet = RegQueryValueEx(hKey,
                                   L"ProductType",
                                   NULL,
                                   NULL,
                                   (LPBYTE)szProductType,
                                   &dwBufLen);

            if ((lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE))
            {
                return;
            }

            RegCloseKey(hKey);

            if (lstrcmpi(L"WINNT", szProductType) == 0)
            {
                if (m_osvi.dwMajorVersion <= 4)
                {
                    m_nWinVersion = WindowsNT40;
                    m_nWinEdition = Workstation;
                }
            }

            if (lstrcmpi(L"LANMANNT", szProductType) == 0)
            {
                if (m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2)
                {
                    m_nWinVersion = WindowsServer2003;
                }

                if (m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0)
                {
                    m_nWinVersion = Windows2000;
                    m_nWinEdition = Server;
                }

                if (m_osvi.dwMajorVersion <= 4)
                {
                    m_nWinVersion = WindowsNT40;
                    m_nWinEdition = Server;
                }
            }

            if (lstrcmpi(L"SERVERNT", szProductType) == 0)
            {
                if (m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2)
                {
                    m_nWinVersion = WindowsServer2003;
                    m_nWinEdition = EnterpriseServer;
                }

                if (m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0)
                {
                    m_nWinVersion = Windows2000;
                    m_nWinEdition = AdvancedServer;
                }

                if (m_osvi.dwMajorVersion <= 4)
                {
                    m_nWinVersion = WindowsNT40;
                    m_nWinEdition = EnterpriseServer;
                }
            }
        }
    }

    void detectWindowsEdition()
    {
        if (m_bOsVersionInfoEx)
        {
            switch (m_osvi.dwMajorVersion)
            {
            case 4:
            {
                switch (m_osvi.wProductType)
                {
                case VER_NT_WORKSTATION:
                    m_nWinEdition = Workstation;
                    break;

                case VER_NT_SERVER:
                    m_nWinEdition = (m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0 ? EnterpriseServer : StandardServer;
                    break;
                }
            }
            break;

            case 5:
            {
                switch (m_osvi.wProductType)
                {
                case VER_NT_WORKSTATION:
                {
                    m_nWinEdition = (m_osvi.wSuiteMask & VER_SUITE_PERSONAL) != 0 ? Home : Professional;
                }
                break;

                case VER_NT_SERVER:
                {
                    switch (m_osvi.dwMinorVersion)
                    {
                    case 0:
                    {
                        if ((m_osvi.wSuiteMask & VER_SUITE_DATACENTER) != 0)
                        {
                            m_nWinEdition = DatacenterServer;
                        }
                        else if ((m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                        {
                            m_nWinEdition = AdvancedServer;
                        }
                        else
                        {
                            m_nWinEdition = Server;
                        }
                    }
                    break;

                    default:
                    {
                        if ((m_osvi.wSuiteMask & VER_SUITE_DATACENTER) != 0)
                        {
                            m_nWinEdition = DatacenterServer;
                        }
                        else if ((m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                        {
                            m_nWinEdition = EnterpriseServer;
                        }
                        else if ((m_osvi.wSuiteMask & VER_SUITE_BLADE) != 0)
                        {
                            m_nWinEdition = WebServer;
                        }
                        else
                        {
                            m_nWinEdition = StandardServer;
                        }
                    }
                    break;
                    }
                }
                break;
                }
            }
            break;

            case 6:
            case 10:
            {
                DWORD dwReturnedProductType = detectProductInfo();
                switch (dwReturnedProductType)
                {
                case PRODUCT_UNDEFINED:
                    m_nWinEdition = EditionUnknown;
                    break;

                case PRODUCT_ULTIMATE:
                    m_nWinEdition = Ultimate;
                    break;

                case PRODUCT_HOME_BASIC:
                    m_nWinEdition = HomeBasic;
                    break;

                case PRODUCT_HOME_PREMIUM:
                    m_nWinEdition = HomePremium;
                    break;

                case PRODUCT_ENTERPRISE:
                    m_nWinEdition = Enterprise;
                    break;

                case PRODUCT_HOME_BASIC_N:
                    m_nWinEdition = HomeBasic_N;
                    break;

                case PRODUCT_BUSINESS:
                    m_nWinEdition = Business;
                    break;

                case PRODUCT_STANDARD_SERVER:
                    m_nWinEdition = StandardServer;
                    break;

                case PRODUCT_DATACENTER_SERVER:
                    m_nWinEdition = DatacenterServer;
                    break;

                case PRODUCT_SMALLBUSINESS_SERVER:
                    m_nWinEdition = SmallBusinessServer;
                    break;

                case PRODUCT_ENTERPRISE_SERVER:
                    m_nWinEdition = EnterpriseServer;
                    break;

                case PRODUCT_STARTER:
                    m_nWinEdition = Starter;
                    break;

                case PRODUCT_DATACENTER_SERVER_CORE:
                    m_nWinEdition = DatacenterServerCore;
                    break;

                case PRODUCT_STANDARD_SERVER_CORE:
                    m_nWinEdition = StandardServerCore;
                    break;

                case PRODUCT_ENTERPRISE_SERVER_CORE:
                    m_nWinEdition = EnterpriseServerCore;
                    break;

                case PRODUCT_ENTERPRISE_SERVER_IA64:
                    m_nWinEdition = EnterpriseServerIA64;
                    break;

                case PRODUCT_BUSINESS_N:
                    m_nWinEdition = Business_N;
                    break;

                case PRODUCT_WEB_SERVER:
                    m_nWinEdition = WebServer;
                    break;

                case PRODUCT_CLUSTER_SERVER:
                    m_nWinEdition = ClusterServer;
                    break;

                case PRODUCT_HOME_SERVER:
                    m_nWinEdition = HomeServer;
                    break;

                case PRODUCT_STORAGE_EXPRESS_SERVER:
                    m_nWinEdition = StorageExpressServer;
                    break;

                case PRODUCT_STORAGE_STANDARD_SERVER:
                    m_nWinEdition = StorageStandardServer;
                    break;

                case PRODUCT_STORAGE_WORKGROUP_SERVER:
                    m_nWinEdition = StorageWorkgroupServer;
                    break;

                case PRODUCT_STORAGE_ENTERPRISE_SERVER:
                    m_nWinEdition = StorageEnterpriseServer;
                    break;

                case PRODUCT_SERVER_FOR_SMALLBUSINESS:
                    m_nWinEdition = ServerForSmallBusiness;
                    break;

                case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                    m_nWinEdition = SmallBusinessServerPremium;
                    break;

#if _WIN32_WINNT >= 0x0601 // windows 7以上
                case PRODUCT_HOME_PREMIUM_N:
                    m_nWinEdition = HomePremium_N;
                    break;

                case PRODUCT_ENTERPRISE_N:
                    m_nWinEdition = Enterprise_N;
                    break;

                case PRODUCT_ULTIMATE_N:
                    m_nWinEdition = Ultimate_N;
                    break;

                case PRODUCT_WEB_SERVER_CORE:
                    m_nWinEdition = WebServerCore;
                    break;

                case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
                    m_nWinEdition = MediumBusinessServerManagement;
                    break;

                case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
                    m_nWinEdition = MediumBusinessServerSecurity;
                    break;

                case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
                    m_nWinEdition = MediumBusinessServerMessaging;
                    break;

                case PRODUCT_SERVER_FOUNDATION:
                    m_nWinEdition = ServerFoundation;
                    break;

                case PRODUCT_HOME_PREMIUM_SERVER:
                    m_nWinEdition = HomePremiumServer;
                    break;

                case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
                    m_nWinEdition = ServerForSmallBusiness_V;
                    break;

                case PRODUCT_STANDARD_SERVER_V:
                    m_nWinEdition = StandardServer_V;
                    break;

                case PRODUCT_DATACENTER_SERVER_V:
                    m_nWinEdition = DatacenterServer_V;
                    break;

                case PRODUCT_ENTERPRISE_SERVER_V:
                    m_nWinEdition = EnterpriseServer_V;
                    break;

                case PRODUCT_DATACENTER_SERVER_CORE_V:
                    m_nWinEdition = DatacenterServerCore_V;
                    break;

                case PRODUCT_STANDARD_SERVER_CORE_V:
                    m_nWinEdition = StandardServerCore_V;
                    break;

                case PRODUCT_ENTERPRISE_SERVER_CORE_V:
                    m_nWinEdition = EnterpriseServerCore_V;
                    break;

                case PRODUCT_HYPERV:
                    m_nWinEdition = HyperV;
                    break;

                case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
                    m_nWinEdition = StorageExpressServerCore;
                    break;

                case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
                    m_nWinEdition = StorageStandardServerCore;
                    break;

                case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
                    m_nWinEdition = StorageWorkgroupServerCore;
                    break;

                case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
                    m_nWinEdition = StorageEnterpriseServerCore;
                    break;

                case PRODUCT_STARTER_N:
                    m_nWinEdition = Starter_N;
                    break;

                case PRODUCT_PROFESSIONAL:
                    m_nWinEdition = Professional;
                    break;

                case PRODUCT_PROFESSIONAL_N:
                    m_nWinEdition = Professional_N;
                    break;

                case PRODUCT_SB_SOLUTION_SERVER:
                    m_nWinEdition = SBSolutionServer;
                    break;

                case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
                    m_nWinEdition = ServerForSBSolution;
                    break;

                case PRODUCT_STANDARD_SERVER_SOLUTIONS:
                    m_nWinEdition = StandardServerSolutions;
                    break;

                case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
                    m_nWinEdition = StandardServerSolutionsCore;
                    break;

                case PRODUCT_SB_SOLUTION_SERVER_EM:
                    m_nWinEdition = SBSolutionServer_EM;
                    break;

                case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
                    m_nWinEdition = ServerForSBSolution_EM;
                    break;

                case PRODUCT_SOLUTION_EMBEDDEDSERVER:
                    m_nWinEdition = SolutionEmbeddedServer;
                    break;

                case PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE:
                    m_nWinEdition = SolutionEmbeddedServerCore;
                    break;

                case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
                    m_nWinEdition = SmallBusinessServerPremiumCore;
                    break;

                case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
                    m_nWinEdition = EssentialBusinessServerMGMT;
                    break;

                case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
                    m_nWinEdition = EssentialBusinessServerADDL;
                    break;

                case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
                    m_nWinEdition = EssentialBusinessServerMGMTSVC;
                    break;

                case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
                    m_nWinEdition = EssentialBusinessServerADDLSVC;
                    break;

                case PRODUCT_CLUSTER_SERVER_V:
                    m_nWinEdition = ClusterServer_V;
                    break;

                case PRODUCT_EMBEDDED:
                    m_nWinEdition = Embedded;
                    break;

                case PRODUCT_STARTER_E:
                    m_nWinEdition = Starter_E;
                    break;

                case PRODUCT_HOME_BASIC_E:
                    m_nWinEdition = HomeBasic_E;
                    break;

                case PRODUCT_HOME_PREMIUM_E:
                    m_nWinEdition = HomePremium_E;
                    break;

                case PRODUCT_PROFESSIONAL_E:
                    m_nWinEdition = Professional_E;
                    break;

                case PRODUCT_ENTERPRISE_E:
                    m_nWinEdition = Enterprise_E;
                    break;

                case PRODUCT_ULTIMATE_E:
                    m_nWinEdition = Ultimate_E;
                    break;
#endif
                }
            }
            break;
            }
        }
    }

    void detectWindowsServicePack()
    {
        // Display service pack (if any) and build number.
        if (m_osvi.dwMajorVersion == 4 &&
            lstrcmpi(m_osvi.szCSDVersion, L"Service Pack 6") == 0)
        {
            HKEY hKey;
            LONG lRet;

            // Test for SP6 versus SP6a.
            lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                servicePack6,
                                0,
                                KEY_QUERY_VALUE,
                                &hKey);

            if (lRet == ERROR_SUCCESS)
            {
                wsprintf(m_szServicePack,
                         L"Service Pack 6a(Build %d)",
                         m_osvi.dwBuildNumber & 0xFFFF);
            }
            else
            {
                // Windows NT 4.0 prior to SP6a
                wsprintf(m_szServicePack,
                         L"%s(Build %d)",
                         m_osvi.szCSDVersion,
                         m_osvi.dwBuildNumber & 0xFFFF);
            }

            RegCloseKey(hKey);
        }
        else
        {
            // Windows NT 3.51 and earlier or Windows 2000 and later
            wsprintf(m_szServicePack,
                     L"%s(Build %d)",
                     m_osvi.szCSDVersion,
                     m_osvi.dwBuildNumber & 0xFFFF);
        }
    }

    DWORD detectProductInfo()
    {
        DWORD dwProductInfo = PRODUCT_UNDEFINED;

#if _WIN32_WINNT >= 0x0600 
        if (m_osvi.dwMajorVersion >= 6)
        {
            PGetProductInfo lpProducInfo = (PGetProductInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"),
                                            "GetProductInfo");

            if (NULL != lpProducInfo)
            {
                lpProducInfo(m_osvi.dwMajorVersion,
                             m_osvi.dwMinorVersion,
                             m_osvi.wServicePackMajor,
                             m_osvi.wServicePackMinor,
                             &dwProductInfo);
            }
        }
#endif

        return dwProductInfo;
    }

    bool getWinMajorMinorVersion(DWORD& major, DWORD& minor)
    {
        bool bRetCode = false;
        LPBYTE pinfoRawData = 0;

        if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
        {
            WKSTA_INFO_100* pworkstationInfo = (WKSTA_INFO_100*)pinfoRawData;
            major = pworkstationInfo->wki100_ver_major;
            minor = pworkstationInfo->wki100_ver_minor;
            ::NetApiBufferFree(pinfoRawData);
            bRetCode = true;
        }

        return bRetCode;
    }

public:
    WindowsVersion SystemInfo::getWindowsVersion() const
    {
        return m_nWinVersion;
    }

    WindowsEdition SystemInfo::getWindowsEdition() const
    {
        return m_nWinEdition;
    }

    bool isNTPlatform() const
    {
        return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT;
    }

    bool isWindowsPlatform() const
    {
        return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
    }

    bool isWin32sPlatform() const
    {
        return m_osvi.dwPlatformId == VER_PLATFORM_WIN32s;
    }

    DWORD SystemInfo::getMajorVersion() const
    {
        return m_osvi.dwMajorVersion;
    }

    DWORD SystemInfo::getMinorVersion() const
    {
        return m_osvi.dwMinorVersion;
    }

    DWORD SystemInfo::getBuildNumber() const
    {
        return m_osvi.dwBuildNumber;
    }

    DWORD SystemInfo::getPlatformID() const
    {
        return m_osvi.dwPlatformId;
    }

    std::string SystemInfo::getServicePackInfo() const
    {
        char packInfo[MAX_PATH] = { 0 };
        WideCharToMultiByte(CP_ACP, 0, m_szServicePack, -1, packInfo, sizeof(packInfo), NULL, NULL);
        return packInfo;
    }

    bool SystemInfo::is64bitPlatform() const
    {
        return (
            m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
            m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
            m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64);
    }

private:
    WindowsVersion     m_nWinVersion;
    WindowsEdition     m_nWinEdition;
    TCHAR              m_szServicePack[128];
    OSVERSIONINFOEX    m_osvi;
    SYSTEM_INFO        m_SysInfo;
    BOOL               m_bOsVersionInfoEx;

    static SystemInfo* m_pSysInfo;

    //处理释放问题
    class InnerAutoDestroy
    {
    public:
        ~InnerAutoDestroy()
        {
            if (SystemInfo::m_pSysInfo)
            {
                delete SystemInfo::m_pSysInfo;
            }
        }

    };

    // 定义一个静态成员,在程序结束时,系统会调用它的析构
    static InnerAutoDestroy innerCleanUp;
};

SystemInfo* SystemInfo::m_pSysInfo = NULL;
SystemInfo::InnerAutoDestroy SystemInfo::innerCleanUp;

WindowsVersion getWindowsVersion()
{
    return SystemInfo::getInstance()->getWindowsVersion();
}

WindowsEdition getWindowsEdition()
{
    return SystemInfo::getInstance()->getWindowsEdition();
}

std::string getPlatformType()
{
    std::string PT = "Unknown";

    if (SystemInfo::getInstance()->isNTPlatform())
    {
        PT = "NT";
    }
    else if (SystemInfo::getInstance()->isWindowsPlatform())
    {
        PT = "Windows";
    }
    else if (SystemInfo::getInstance()->isWin32sPlatform())
    {
        PT = "Win32s";
    }

    return PT;
}

DWORD getMajorVersion()
{
    return SystemInfo::getInstance()->getMajorVersion();
}

DWORD getMinorVersion()
{
    return SystemInfo::getInstance()->getMinorVersion();
}

DWORD getBuildNumber()
{
    return SystemInfo::getInstance()->getBuildNumber();
}

DWORD getPlatformID()
{
    return SystemInfo::getInstance()->getPlatformID();
}

std::string getServicePackInfo()
{
    return SystemInfo::getInstance()->getServicePackInfo();
}

bool is32bitPlatform()
{
    return !is64bitPlatform();
}

bool is64bitPlatform()
{
    return SystemInfo::getInstance()->is64bitPlatform();
}

bool checkIsInstalled(std::string name)
{
    HKEY hUninstKey = NULL;
    HKEY hAppKey = NULL;
    WCHAR sAppKeyName[MAX_PATH] = { 0 };
    WCHAR sSubKey[MAX_PATH] = { 0 };
    WCHAR sDisplayName[MAX_PATH] = { 0 };
    long lResult = ERROR_SUCCESS;
    DWORD dwType = KEY_ALL_ACCESS;
    DWORD dwBufferSize = 0;

    //Open the "Uninstall" key.
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, uninstall, 0, KEY_READ, &hUninstKey) != ERROR_SUCCESS)
    {
        return false;
    }

    for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
    {
        //Enumerate all sub keys
        dwBufferSize = sizeof(sAppKeyName);
        if ((lResult = RegEnumKeyEx(hUninstKey, dwIndex, sAppKeyName,
            &dwBufferSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            //Open the sub key.
            wsprintf(sSubKey, L"%s\\%s", uninstall, sAppKeyName);
            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_READ, &hAppKey) != ERROR_SUCCESS)
            {
                RegCloseKey(hAppKey);
                RegCloseKey(hUninstKey);
                return false;
            }

            //Get the display name value from the application's sub key.
            dwBufferSize = sizeof(sDisplayName);
            LSTATUS status = RegQueryValueEx(hAppKey,
                                             L"DisplayName",
                                             NULL,
                                             &dwType,
                                             (unsigned char*)sDisplayName,
                                             &dwBufferSize);
            if (status == ERROR_SUCCESS)
            {
                char softwareName[MAX_PATH] = { 0 };
                WideCharToMultiByte(CP_ACP, 0, sDisplayName, -1, softwareName, sizeof(softwareName), NULL, NULL);

                if (name == softwareName)
                {
                    RegCloseKey(hAppKey);
                    RegCloseKey(hUninstKey);
                    return true;
                }
            }
            else
            {
                RegCloseKey(hAppKey);
            }
        }
    }

    RegCloseKey(hUninstKey);
    return false;
}

