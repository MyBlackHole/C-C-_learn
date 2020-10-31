#include "GLDRichTextEditGlobal.h"
#include <windows.h>  
#include <OCIdl.h>
#include <OleCtl.h>
#include <QSettings>
#include <QFileInfo>
#include <QFile>
#include <QImage>
#include <QDir>
#include <QUuid>

// 检测文件关联情况  
// strExt: 要检测的扩展名(例如: ".txt")  
// strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")  
// 返回TRUE: 表示已关联，FALSE: 表示未关联  
bool checkFileRelation(const wchar_t *strExt, const wchar_t *strAppKey)
{
    int nRet = FALSE;
    HKEY hExtKey;
    wchar_t szPath[_MAX_PATH];
    DWORD dwSize = sizeof(szPath);

    if (RegOpenKeyW(HKEY_CLASSES_ROOT, strExt, &hExtKey) == ERROR_SUCCESS)
    {
        RegQueryValueExW(hExtKey, NULL, NULL, NULL, (LPBYTE)szPath, &dwSize);

        if (_wcsicmp(szPath, strAppKey) == 0)
        {
            nRet = TRUE;
        }

        RegCloseKey(hExtKey);
    }

    return (nRet == FALSE ? false : true);
}

// 注册文件关联  
// strExe: 要检测的扩展名(例如: ".txt")  
// strAppName: 要关联的应用程序名(例如: "C:/MyApp/MyApp.exe")  
// strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")  
// strDefaultIcon: 扩展名为strAppName的图标文件(例如: "C:/MyApp/MyApp.exe,0")  
// strDescribe: 文件类型描述  
void  registerFileRelation(wchar_t *strExt, wchar_t *strAppName, wchar_t *strAppKey, wchar_t *strDefaultIcon, wchar_t *strDescribe)
{
    wchar_t strTemp[_MAX_PATH];
    HKEY hKey;

    RegCreateKey(HKEY_CLASSES_ROOT, strExt, &hKey);
    RegSetValue(hKey, L"", REG_SZ, strAppKey, wcslen(strAppKey) + 1);
    RegCloseKey(hKey);

    RegCreateKey(HKEY_CLASSES_ROOT, strAppKey, &hKey);
    RegSetValue(hKey, L"", REG_SZ, strDescribe, wcslen(strDescribe) + 1);
    RegCloseKey(hKey);

    swprintf(strTemp, L"%s//DefaultIcon", strAppKey);
    RegCreateKey(HKEY_CLASSES_ROOT, strTemp, &hKey);
    RegSetValue(hKey, L"", REG_SZ, strDefaultIcon, wcslen(strDefaultIcon) + 1);
    RegCloseKey(hKey);

    swprintf(strTemp, L"%s//Shell", strAppKey);
    RegCreateKey(HKEY_CLASSES_ROOT, strTemp, &hKey);
    RegSetValue(hKey, L"", REG_SZ, L"Open", wcslen(L"Open") + 1);
    RegCloseKey(hKey);

    swprintf(strTemp, L"%s//Shell//Open//Command", strAppKey);
    RegCreateKey(HKEY_CLASSES_ROOT, strTemp, &hKey);
    swprintf(strTemp, L"%s \"%%1\"", strAppName);
    RegSetValue(hKey, L"", REG_SZ, strTemp, wcslen(strTemp) + 1);
    RegCloseKey(hKey);
}

bool checkInstallOfficeWord()
{
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Winword.exe", QSettings::NativeFormat);
    QString sWordPath = reg.value("Path", "").toString();

    QFileInfo fl(sWordPath + "\\winword.exe");

    if (fl.exists())
    {
        return true;
    }

    return false;
}

bool checkInstallWpsWord()
{
    return false;
}

////////////////////////////////////////////////////////
//
// Internal helper functions prototypes
//
// Set the given key and its value.
BOOL setKeyAndValue(const char* pszPath,
    const char* szSubkey,
    const char* szValue);
// Convert a CLSID into a char string.
void convertCLSIDtochar(const CLSID& clsid,
    char* szCLSID,
    int length);
// Delete szKeyChild and all of its descendents.
LONG recursiveDeleteKey(HKEY hKeyParent, const char* szKeyChild);
////////////////////////////////////////////////////////
//
// Constants
//
// Size of a CLSID as a string
static const int s_nCLSIDStringSize = 39;
/////////////////////////////////////////////////////////
//
// Public function implementation
//
//
// Register the component in the registry.
//
HRESULT registerServer(HMODULE hModule, // DLL module handle
    const CLSID& clsid, // Class ID
    const char* szFriendlyName, // Friendly Name
    const char* szVerIndProgID, // Programmatic
    const char* szProgID) //   IDs
{
    // Get server location.
    char szModule[512];
    DWORD dwResult = ::GetModuleFileNameA(hModule, szModule, sizeof(szModule) / sizeof(char));
    assert(dwResult != 0);
    // Convert the CLSID into a char.
    char szCLSID[s_nCLSIDStringSize];
    convertCLSIDtochar(clsid, szCLSID, sizeof(szCLSID));
    // Build the key CLSID//{...}
    char szKey[64];
    strcpy(szKey, "CLSID//");
    strcat(szKey, szCLSID);
    // Add the CLSID to the registry.
    setKeyAndValue(szKey, NULL, szFriendlyName);
    // Add the server filename subkey under the CLSID key.
    setKeyAndValue(szKey, "InprocServer32", szModule);
    // Add the ProgID subkey under the CLSID key.
    setKeyAndValue(szKey, "ProgID", szProgID);
    // Add the version-independent ProgID subkey under CLSID key.
    setKeyAndValue(szKey, "VersionIndependentProgID", szVerIndProgID);
    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT.
    setKeyAndValue(szVerIndProgID, NULL, szFriendlyName);
    setKeyAndValue(szVerIndProgID, "CLSID", szCLSID);
    setKeyAndValue(szVerIndProgID, "CurVer", szProgID);
    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT.
    setKeyAndValue(szProgID, NULL, szFriendlyName);
    setKeyAndValue(szProgID, "CLSID", szCLSID);
    return S_OK;
}

//
// Remove the component from the registry.
//
LONG unregisterServer(const CLSID& clsid, // Class ID
    const char* szVerIndProgID, // Programmatic
    const char* szProgID) //   IDs
{
    // Convert the CLSID into a char.
    char szCLSID[s_nCLSIDStringSize];
    convertCLSIDtochar(clsid, szCLSID, sizeof(szCLSID));
    // Build the key CLSID//{...}
    char szKey[64];
    strcpy(szKey, "CLSID//");
    strcat(szKey, szCLSID);
    // Delete the CLSID Key - CLSID/{...}
    LONG lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szKey);
    assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.
    // Delete the version-independent ProgID Key.
    lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szVerIndProgID);
    assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.
    // Delete the ProgID key.
    lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szProgID);
    assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.
    return S_OK;
}

///////////////////////////////////////////////////////////
//
// Internal helper functions
//
// Convert a CLSID to a char string.
void convertCLSIDtochar(const CLSID& clsid,
    char* szCLSID,
    int length)
{
    assert(length >= s_nCLSIDStringSize);
    // Get CLSID
    LPOLESTR wszCLSID = NULL;
    HRESULT hr = StringFromCLSID(clsid, &wszCLSID);
    assert(SUCCEEDED(hr));
    // Covert from wide characters to non-wide.
    wcstombs(szCLSID, wszCLSID, length);
    // Free memory.
    CoTaskMemFree(wszCLSID);
}

//
// Delete a key and all of its descendents.
//
LONG recursiveDeleteKey(HKEY hKeyParent, // Parent of key to delete
    const char* lpszKeyChild)  // Key to delete
{
    // Open the child.
    HKEY hKeyChild;
    LONG lRes = RegOpenKeyExA(hKeyParent, lpszKeyChild, 0, KEY_ALL_ACCESS, &hKeyChild);
    if (lRes != ERROR_SUCCESS)
    {
        return lRes;
    }
    // Enumerate all of the decendents of this child.
    FILETIME time;
    char szBuffer[256];
    DWORD dwSize = 256;
    while (RegEnumKeyExA(hKeyChild, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time) == S_OK)
    {
        // Delete the decendents of this child.
        lRes = recursiveDeleteKey(hKeyChild, szBuffer);
        if (lRes != ERROR_SUCCESS)
        {
            // Cleanup before exiting.
            RegCloseKey(hKeyChild);
            return lRes;
        }
        dwSize = 256;
    }
    // Close the child.
    RegCloseKey(hKeyChild);
    // Delete this child.
    return RegDeleteKeyA(hKeyParent, lpszKeyChild);
}

//
// Create a key and set its value.
//   - This helper function was borrowed and modifed from
//     Kraig Brockschmidt's book Inside OLE.
//
BOOL setKeyAndValue(const char* szKey,
    const char* szSubkey,
    const char* szValue)
{
    HKEY hKey;
    char szKeyBuf[1024];
    // Copy keyname into buffer.
    strcpy(szKeyBuf, szKey);
    // Add subkey name to buffer.
    if (szSubkey != NULL)
    {
        strcat(szKeyBuf, "//");
        strcat(szKeyBuf, szSubkey);
    }
    // Create and open key and subkey.
    long lResult = RegCreateKeyExA(HKEY_CLASSES_ROOT, szKeyBuf, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if (lResult != ERROR_SUCCESS)
    {
        return FALSE;
    }
    // Set the Value.
    if (szValue != NULL)
    {
        RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)szValue, (DWORD)strlen(szValue) + 1);
    }
    RegCloseKey(hKey);
    return TRUE;
}

BOOL dllRegisterServer(LPCWSTR lpszFileName, BOOL bUnregister)
{
	typedef HRESULT(WINAPI * FREG)();

	BOOL bRet = FALSE;

	HMODULE hDLL = ::LoadLibraryW(lpszFileName);
	if (NULL == hDLL)
	{
		return FALSE;
	}

	CHAR * lpszFuncName;
	if (FALSE == bUnregister)
	{
		lpszFuncName = "DllRegisterServer";
	}
	else
	{
		lpszFuncName = "DllUnregisterServer";
	}

	FREG lpfunc = (FREG)::GetProcAddress(hDLL, lpszFuncName);
	if (lpfunc != NULL)
	{
		lpfunc();
		bRet = TRUE;
	}

	::FreeLibrary(hDLL);

	return bRet;
}

HBITMAP loadImageFromFile(const QString & pszBitmapFile, bool bMonochrome)
{
	HBITMAP hBitmap = NULL;
	do 
	{
		QImage image;
		if (!image.load(pszBitmapFile))
		{
			break;
		}
		QImage::Format imgFormat = image.format();
		if (QImage::Format_Mono == imgFormat)
		{
			hBitmap = (HBITMAP)::LoadImageW(NULL, (LPCWSTR)pszBitmapFile.utf16(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else
		{
			if (bMonochrome)
			{
				if (QImage::Format_Mono != imgFormat)
				{
					image = image.convertToFormat(QImage::Format_Mono);
					QString sTempFile;
					sTempFile = QString("%1/%2.bmp").arg(QDir::tempPath()).arg(QUuid::createUuid().toString());
					image.save(sTempFile);
					hBitmap = (HBITMAP)::LoadImageW(NULL, (LPCWSTR)sTempFile.utf16(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					QFile::remove(sTempFile);
				}
			}
			else
			{
				if (QImage::Format_RGB32 != imgFormat)
				{
					image = image.convertToFormat(QImage::Format_RGB32);
				}
				HDC hdc = ::GetDC(NULL);
				hBitmap = ::CreateBitmap(image.width(), image.height(), 1, 32, image.bits());
				ReleaseDC(NULL, hdc);
			}
		}
	} while (false);
	
	return hBitmap;
}

