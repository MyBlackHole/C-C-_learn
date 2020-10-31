/*!
 * \file GLDBitmapCompress.cpp
 *
 * \author xieg-a
 * \date ¾ÅÔÂ 2014
 *
 *
 */
#include "GLDBitmapCompress.h"
#include "GLDRichTextEditGlobal.h"

struct Bitmapinfo8Bit
{
    BITMAPINFO  bmi;  // contains the first color
    RGBQUAD     color_table[255]; // color table
};

GLDBitmapCompress::GLDBitmapCompress()
    : m_hdc(NULL)
    , m_hbm(NULL)
    , m_hbmDefault(NULL)
{
    m_hdc = ::CreateCompatibleDC(NULL);
}

GLDBitmapCompress::~GLDBitmapCompress()
{
    DeleteDC(m_hdc);
}

bool GLDBitmapCompress::loadBitmapFile(const QString & filePath)
{
    m_hbm = (HBITMAP)LoadImage(NULL, (wchar_t *)filePath.utf16(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION); //loadImageFromFile(filePath, false);

    if (NULL == m_hbm)
    {
        return false;
    }

    m_hbmDefault = (HBITMAP)::SelectObject(m_hdc, m_hbm);

    ::GetObject(m_hbm, sizeof(DIBSECTION), &m_dib);

    return true;
}

BOOL GLDBitmapCompress::saveBitmapFile(QString filename)
{
    // this can only compress 256 color bitmap
    if (m_dib.dsBmih.biBitCount != 8)
    {
        return FALSE;
    }

    int nSize = 0;
    QByteArray RLE8_bitmaps_bits(4194304, 4194304);
    //RLE8_bitmaps_bits.SetSize(4194304, 4194304); // initial 40k

    // compress the bitmap bits use Run-Legth-Ecoding (8 bit)
    compressInRLE8((BYTE*)m_dib.dsBm.bmBits, RLE8_bitmaps_bits, nSize);

    Bitmapinfo8Bit _8bit_bmi;
    _8bit_bmi.bmi.bmiHeader = m_dib.dsBmih; // copy the entire sturcture
    _8bit_bmi.bmi.bmiHeader.biCompression = BI_RLE8;
    _8bit_bmi.bmi.bmiHeader.biSizeImage = nSize; // size in bytes after RLE8 compression

    // retrieve color tables entries
    GetDIBColorTable(m_hdc, 0, _8bit_bmi.bmi.bmiHeader.biClrUsed, _8bit_bmi.bmi.bmiColors);

    HANDLE hf;                 // file handle
    BITMAPFILEHEADER hdr;       // bitmap file-header
    PBITMAPINFOHEADER pbih;     // bitmap info-header
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER)&_8bit_bmi;

    // Create the .BMP file.
    hf = CreateFileW((wchar_t *)filename.utf16(),
                     GENERIC_READ | GENERIC_WRITE,
                     (DWORD)0,
                     NULL,
                     CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL,
                     (HANDLE)NULL);

    if (hf == INVALID_HANDLE_VALUE)
        return FALSE;

    hdr.bfType = 0x4d42;  // 0x42 = "B" 0x4d = "M"
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
                         pbih->biSize + pbih->biClrUsed
                         * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
            pbih->biSize + pbih->biClrUsed
            * sizeof (RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.
    if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
                   (LPDWORD)&dwTmp, NULL))
    {
        CloseHandle(hf);
        return 0;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
    if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
                   +pbih->biClrUsed * sizeof (RGBQUAD),
                   (LPDWORD)&dwTmp, NULL))
    {
        CloseHandle(hf);
        return 0;
    }

    // Copy the array of color indices into the .BMP file.
    if (!WriteFile(hf, (LPSTR)RLE8_bitmaps_bits.data(), (int)pbih->biSizeImage, (LPDWORD)&dwTmp, NULL))
    {
        CloseHandle(hf);
        return 0;
    }

    // Close the .BMP file.
    CloseHandle(hf);

    // debug,  load the file into memory again, and compare with the original bitmap
    //#ifdef _DEBUG
    //	BOOL success = CompareWithFile(filename);
    //#endif
    //	ASSERT(success != 0);

    return 1;
}

void GLDBitmapCompress::compressInRLE8(BYTE* pSrcBits, QByteArray & pRLEBits, int& RLE_size)
{
    int line;
    int src_index = 0;
    int dst_index = 0;
    int counter = 0;
    int i = 0;
    // in RLE8 every pixel is one byte

    for (line = 0; line < m_dib.dsBmih.biHeight; line++)
    {

state_start:  // just at the start of a block

        if (endOfLine(src_index)) // this is the last pixel of the line
        {
            pRLEBits[dst_index++] = 1; // block of length 1
            pRLEBits[dst_index++] = pSrcBits[src_index];

            src_index++;
            goto end_of_line;
        }

        // now the block length is at least 2, look ahead to decide next state
        // next two same pixels are the same, enter compress mode
        if (pSrcBits[src_index] == pSrcBits[src_index + 1])
            goto state_compress;

        if (endOfLine(src_index + 1)) // the last 2 pixel of the line
        {  // these 2 pixel are not the same
            pRLEBits[dst_index++] = 1; // block of length 1
            pRLEBits[dst_index++] = pSrcBits[src_index++];
            pRLEBits[dst_index++] = 1; // block of length 1
            pRLEBits[dst_index++] = pSrcBits[src_index++];
            goto end_of_line;
        }

        // now the block length is at least 3
        // in next 3 pixels, the last 2 consecutive pixel are the same
        if (pSrcBits[src_index + 1] == pSrcBits[src_index + 2])
        {
            pRLEBits[dst_index++] = 1; // block of length 1
            pRLEBits[dst_index++] = pSrcBits[src_index++];
            goto state_compress;
        }
        else // in these 3 pixel, no 2 consecutive pixel are the same
            goto state_no_compress; // un-compressed block need at least 3 pixel

state_compress:  // compress mode

        // src_index is the first pixel of a compressd block
        // counter is the number of additional pixels following currrent pixel
        // (src_index+counter) point to the last pixel of current block
        for (counter = 1; counter <= 254; counter++)
        {
            // must check this condition first, or a BUG occur!
            if (pSrcBits[src_index + counter] != pSrcBits[src_index])
                break;
            if (endOfLine(src_index + counter)) // reach end of line
            {
                pRLEBits[dst_index++] = counter + 1; // block length is (counter+1)
                pRLEBits[dst_index++] = pSrcBits[src_index];
                src_index += counter + 1;
                goto end_of_line;
            }

        }
        // now pSrcBits[src_index+counter] falls out of block
        pRLEBits[dst_index++] = counter; // block length is (counter)
        pRLEBits[dst_index++] = pSrcBits[src_index];
        src_index += counter;
        goto state_start;

state_no_compress:
        // src_index is the first pixel of a un-compressd block
        // un-compressed block need at least 3 pixel
        // counter is the number of additional pixels following currrent pixel
        for (counter = 2; counter <= 254; counter++)
        {
            if (endOfLine(src_index + counter)) // reach end of line
            {
                pRLEBits[dst_index++] = 0; // escape character for un-compress block
                pRLEBits[dst_index++] = counter + 1; // block length is (counter+1)
                for (i = counter + 1; i > 0; i--)
                    pRLEBits[dst_index++] = pSrcBits[src_index++];
                if (0 != ((counter + 1) % 2)) // block length is odd
                    pRLEBits[dst_index++];     // padd with a zero byte
                goto end_of_line;
            }

            if (endOfLine(src_index + counter + 1) ||
                    pSrcBits[src_index + counter] != pSrcBits[src_index + counter + 1])
                continue;   // still no 2 consecutive pixel are the same,
            // therefore continue to extend the un-compress block
            // we found two pixels are the same
            if (endOfLine(src_index + counter + 2) ||
                    pSrcBits[src_index + counter + 1] != pSrcBits[src_index + counter + 2])
                continue; // the third pixel is not the same, no need to exit un-compressed mode
            else
            {  // // now pSrcBits[src_index+counter] and following 2 pixel are the same
                // now we need to exit the un-compressed mode
                if (counter > 2)
                    counter--; // we can shrink the block one pixel (go backward)
                pRLEBits[dst_index++] = 0; // escape character for un-compress block
                pRLEBits[dst_index++] = counter + 1; // block length is (counter+1)
                for (i = counter + 1; i > 0; i--)
                    pRLEBits[dst_index++] = pSrcBits[src_index++];
                if (0 != ((counter + 1) % 2)) // block length is odd
                    pRLEBits[dst_index++];     // padd with a zero byte
                goto state_compress;
            }
        } // end of for (counter = 2; counter <= 254; counter++)

        // now we have a full block of 255 pixels
        pRLEBits[dst_index++] = 0; // escape character for un-compress block
        pRLEBits[dst_index++] = counter; // block length is (counter)
        for (i = counter; i > 0; i--)
            pRLEBits[dst_index++] = pSrcBits[src_index++];
        if (0 != ((counter) % 2))   // block length is odd
            pRLEBits[dst_index++];  // padd with a zero byte
        goto state_start;

end_of_line:

        if (0 != (src_index % 4)) // each scan line is dword align
        {
            int pad = 4 - (src_index % 4);
            src_index += pad;
        }

        // src_index already point to the start of next line
        pRLEBits[dst_index++] = 0; // mark end-of-line
        pRLEBits[dst_index++] = 0;
    }

    pRLEBits[dst_index++] = 0; // mark end-of-bitmap
    pRLEBits[dst_index++] = 1;

    RLE_size = dst_index; // image size
}

BOOL GLDBitmapCompress::endOfLine(int pos)
{
    if (0 != m_dib.dsBmih.biWidth % 4)
    {
        int scanline_width = m_dib.dsBmih.biWidth;
        scanline_width = scanline_width + (4 - (scanline_width % 4));
        pos %= scanline_width;
    }

    return (0 == ((pos + 1) % m_dib.dsBmih.biWidth));
}
