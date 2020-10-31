// SystemVersion.cpp : Defines the entry point for the console application.
//

#include <QCoreApplication>

#include "GLDSysInfoUtils.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WindowsVersion windowVersion = getWindowsVersion();
    cout << "Windows's Version is: " << windowVersion << endl;

    WindowsEdition windowEdition = getWindowsEdition();
    cout << "Windows's Edition is: " << windowEdition << endl;

    DWORD majorVersion = getMajorVersion();
    cout << "Windows's majorVersion is: " << majorVersion << endl;

    DWORD minorVersion = getMinorVersion();
    cout << "Windows's minorVersion is: " << minorVersion << endl;

    DWORD platformID = getPlatformID();
    cout << "Windows's platformID is: " << platformID << endl;

    string servicePack = getServicePackInfo();
    cout << "Windows's servicePack is: " << servicePack << endl;

    DWORD builderNum = getBuildNumber();
    cout << "Windows's builderNum is: " << builderNum << endl;

    string PT = getPlatformType();
    cout << "platformType is: " << PT << endl;

    std::string softWareName = "Visual Studio 2013";
    if (checkIsInstalled(softWareName))
    {
        cout << "Visual Studio 2013 is Installed " << endl;
    }
    else
    {
        cout << "Visual Studio 2013 is not Installed " << endl;
    }


    return a.exec();
}
