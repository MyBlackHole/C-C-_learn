/*!
 * \file GLDRichTextEditGlobal.h
 *
 * \author xg
 * \date 八月 2014
 *
 * 
 */
#ifndef __GLDRichTextEditGlobal_H__
#define __GLDRichTextEditGlobal_H__

#include <objbase.h>
#include <assert.h>
#pragma warning(disable:4996)


#include <QDebug>
#include <QString>

#define GLD_TEXT(text) (L##text)

#define GLD_REICH_TEXT(text) QString::fromStdWString(GLD_TEXT(text))

#define GLD_RICH_ERROR_POS(x) qDebug() << QString("%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(x)

// 检测文件关联情况  
// strExt: 要检测的扩展名(例如: ".txt")  
// strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")  
// 返回TRUE: 表示已关联，FALSE: 表示未关联  
bool checkFileRelation(const wchar_t *strExt, const wchar_t *strAppKey);
// 注册文件关联  
// strExe: 要检测的扩展名(例如: ".txt")  
// strAppName: 要关联的应用程序名(例如: "C:/MyApp/MyApp.exe")  
// strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")  
// strDefaultIcon: 扩展名为strAppName的图标文件(例如: "C:/MyApp/MyApp.exe,0")  
// strDescribe: 文件类型描述  
void  registerFileRelation(wchar_t *strExt, wchar_t *strAppName, wchar_t *strAppKey, wchar_t *strDefaultIcon, wchar_t *strDescribe);

bool checkInstallOfficeWord();
bool checkInstallWpsWord();


// This function will register a component in the Registry.
// The component calls this function from its DllRegisterServer function.
HRESULT registerServer(HMODULE hModule,
    const CLSID& clsid,
    LPCTSTR szFriendlyName,
    LPCTSTR szVerIndProgID,
    LPCTSTR szProgID);
// This function will unregister a component.  Components
// call this function from their DllUnregisterServer function.
HRESULT unregisterServer(const CLSID& clsid,
    LPCTSTR szVerIndProgID,
    LPCTSTR szProgID);

BOOL dllRegisterServer(LPCWSTR lpszFileName, BOOL bUnregister);

HBITMAP loadImageFromFile(const QString & pszBitmapFile, bool bMonochrome);

#endif/*__GLDRichTextEditGlobal_H__*/
