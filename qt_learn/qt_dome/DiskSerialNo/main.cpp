#include <QCoreApplication>

#include <atlstr.h>

#include <QDebug>

char HardDriveSerialNumber[MAX_PATH];

#define  MAX_IDE_DRIVES 16
#define  DFP_GET_VERSION          0x00074080
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  IDE_ATAPI_IDENTIFY  0xA1
#define  IDE_ATA_IDENTIFY    0xEC

typedef struct _GETVERSIONOUTPARAMS
{
    BYTE bVersion;       // Binary driver version.
    BYTE bRevision;      // Binary driver revision.
    BYTE bReserved;      // Not used.
    BYTE bIDEDeviceMap;  // Bit map of IDE devices.
    DWORD fCapabilities; // Bit mask of driver capabilities.
    DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

BYTE IdOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];

char *convertToString(DWORD diskdata[256], int firstIndex, int lastIndex, char* buf)
{
    int position = 0;

    for (int index = firstIndex; index <= lastIndex; index++)
    {
        buf[position++] = (char)(diskdata[index] / 256);
        buf[position++] = (char)(diskdata[index] % 256);
    }

    buf[position] = '\0';

    for (int index = position - 1; index > 0 && isspace(buf[index]); index--)
    {
        buf[index] = '\0';
    }

    return buf;
}

void printIdeInfo(int drive, DWORD diskdata[256])
{
    Q_UNUSED(drive);
    char serialNumber[MAX_PATH];
    char bufferSize[32];

    convertToString(diskdata, 10, 19, serialNumber);
    sprintf_s(bufferSize, sizeof(bufferSize), "%u", diskdata[21] * 512);

    if (0 == HardDriveSerialNumber[0] && (isalnum(serialNumber[0]) || isalnum(serialNumber[19])))
    {
        strcpy_s(HardDriveSerialNumber, sizeof(HardDriveSerialNumber), serialNumber);
    }
}

BOOL doIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
    PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
    PDWORD lpcbBytesReturned)
{
    // Set up data structures for IDENTIFY command.
    pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
    pSCIP->irDriveRegs.bFeaturesReg = 0;
    pSCIP->irDriveRegs.bSectorCountReg = 1;
    pSCIP -> irDriveRegs.bSectorNumberReg = 1;
    pSCIP->irDriveRegs.bCylLowReg = 0;
    pSCIP->irDriveRegs.bCylHighReg = 0;

    // Compute the drive number.
    pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

    // The command can either be IDE identify or ATAPI identify.
    pSCIP->irDriveRegs.bCommandReg = bIDCmd;
    pSCIP->bDriveNumber = bDriveNum;
    pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

    return (DeviceIoControl(hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
                            (LPVOID)pSCIP,
                            sizeof(SENDCMDINPARAMS) - 1,
                            (LPVOID)pSCOP,
                            sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
                            lpcbBytesReturned,
                            NULL));
}

int getPhysicalDriveInNTWithAdminRights()
{
    int done = FALSE;
    int drive = 0;

    for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
    {
        HANDLE hPhysicalDriveIOCTL = 0;

        wchar_t driveName[MAX_PATH] = { 0 };
        swprintf_s(driveName, MAX_PATH, L"\\\\.\\PhysicalDrive%d", drive);

        hPhysicalDriveIOCTL = CreateFile(driveName,
                                         GENERIC_READ | GENERIC_WRITE,
                                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                                         NULL,
                                         OPEN_EXISTING,
                                         0,
                                         NULL);

        if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
        {

        }
        else
        {
            GETVERSIONOUTPARAMS VersionParams;
            DWORD cbBytesReturned = 0;

            memset((void*)&VersionParams, 0, sizeof(VersionParams));

            if (!DeviceIoControl(hPhysicalDriveIOCTL,
                                 DFP_GET_VERSION,
                                 NULL,
                                 0,
                                 &VersionParams,
                                 sizeof(VersionParams),
                                 &cbBytesReturned,
                                 NULL))
            {

            }

            if (VersionParams.bIDEDeviceMap > 0)
            {
                BYTE bIDCmd = 0;
                SENDCMDINPARAMS scip;
                bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

                memset(&scip, 0, sizeof(scip));
                memset(IdOutCmd, 0, sizeof(IdOutCmd));

                if (doIdentify(hPhysicalDriveIOCTL,
                               &scip,
                               (PSENDCMDOUTPARAMS)&IdOutCmd,
                               (BYTE)bIDCmd,
                               (BYTE)drive,
                                &cbBytesReturned))
                {
                    DWORD diskdata[256];

                    USHORT *pIdSector = (USHORT *)((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;

                    for (int index = 0; index < 256; index++)
                    {
                        diskdata[index] = pIdSector[index];
                    }

                    printIdeInfo(drive, diskdata);

                    done = TRUE;
                }
            }

            CloseHandle(hPhysicalDriveIOCTL);
        }
    }

    return done;
}

QString getPhysicalDriveSerialNumber()
{
    if(getPhysicalDriveInNTWithAdminRights())
    {
        return QString(HardDriveSerialNumber).trimmed();
    }

    DWORD dwResult = NO_ERROR;

    // call CreateFile to get a handle to physical drive
    HANDLE hDevice = ::CreateFile(L"\\\\.\\PHYSICALDRIVE0",
                                  0,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_EXISTING,
                                  0,
                                  NULL);

    std::shared_ptr<void> spHandle(hDevice, CloseHandle);

    if (INVALID_HANDLE_VALUE == spHandle.get())
    {
        return QString("");
    }

    // set the input STORAGE_PROPERTY_QUERY data structure
    STORAGE_PROPERTY_QUERY storagePropertyQuery;
    ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
    storagePropertyQuery.PropertyId = StorageDeviceProperty;
    storagePropertyQuery.QueryType = PropertyStandardQuery;

    // get the necessary output buffer size
    STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };
    DWORD dwBytesReturned = 0;
    if (!::DeviceIoControl(spHandle.get(), IOCTL_STORAGE_QUERY_PROPERTY,
        &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
        &storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER),
        &dwBytesReturned, NULL))
    {
        dwResult = ::GetLastError();
        return QString("");
    }

    // allocate the necessary memory for the output buffer
    const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
    char* pOutBuffer = new char[dwOutBufferSize];
    ZeroMemory(pOutBuffer, dwOutBufferSize);

    // get the storage device descriptor
    if (!::DeviceIoControl(spHandle.get(), IOCTL_STORAGE_QUERY_PROPERTY,
        &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
        pOutBuffer, dwOutBufferSize,
        &dwBytesReturned, NULL))
    {
        dwResult = ::GetLastError();
        delete[]pOutBuffer;
        return QString("");
    }

    // Now, the output buffer points to a STORAGE_DEVICE_DESCRIPTOR structure
    // followed by additional info like vendor ID, product ID, serial number, and so on.
    STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;
    const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;

    QString strSerialNumber("");

    if (dwSerialNumberOffset != 0)
    {
        // finally, get the serial number
        strSerialNumber = QString(pOutBuffer + dwSerialNumberOffset);
    }

    // perform cleanup and return
    delete[] pOutBuffer;

    return strSerialNumber.trimmed();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Disk No is: " << getPhysicalDriveSerialNumber() << "************";

    return a.exec();
}

