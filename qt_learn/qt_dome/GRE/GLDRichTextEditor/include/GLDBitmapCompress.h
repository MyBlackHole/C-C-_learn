/*!
* \file GLDBitmapCompress.h
*
* \author xieg-a
* \date ¾ÅÔÂ 2014
*
*
*/
#ifndef __GLDBitmapCompress_h__
#define __GLDBitmapCompress_h__

#include <Windows.h>
#include <QString>
#include <QByteArray>

class GLDBitmapCompress
{
public:
    GLDBitmapCompress();
    ~GLDBitmapCompress();

    bool loadBitmapFile(const QString & filePath);
    BOOL saveBitmapFile(QString filename);
    void compressInRLE8(BYTE* pSrcBits, QByteArray & pRLE_Bits, int& RLE_size);

private:
    BOOL endOfLine(int pos);

private:
    HDC        m_hdc;
    HBITMAP    m_hbm;
    HBITMAP    m_hbmDefault;
    DIBSECTION m_dib;
};

#endif // __GLDBitmapCompress_h__
