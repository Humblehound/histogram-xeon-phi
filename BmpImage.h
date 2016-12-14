/* This program is free software. It comes without any warranty, to
     * the extent permitted by applicable law. You can redistribute it
     * and/or modify it under the terms of the Do What The Fuck You Want
     * To Public License, Version 2, as published by Sam Hocevar. See
     * http://www.wtfpl.net/ for more details. */


#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <cstring>

#pragma pack(1)


typedef unsigned int        DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef int                 LONG;

typedef struct tagRGBQUAD
{
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagRGBTRIPLE
{
	BYTE    rgbtBlue;
	BYTE    rgbtGreen;
	BYTE    rgbtRed;
} RGBTRIPLE;

typedef struct tagBITMAPFILEHEADER
{
	WORD    bfType;        // must be 'BM' 
	DWORD   bfSize;        // size of the whole .bmp file
	WORD    bfReserved1;   // must be 0
	WORD    bfReserved2;   // must be 0
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD  biSize;            // size of the structure
	LONG   biWidth;           // image width
	LONG   biHeight;          // image height
	WORD   biPlanes;          // bitplanes
	WORD   biBitCount;        // resolution
	DWORD  biCompression;     // compression
	DWORD  biSizeImage;       // size of the image
	LONG   biXPelsPerMeter;   // pixels per meter X
	LONG   biYPelsPerMeter;   // pixels per meter Y
	DWORD  biClrUsed;         // colors used
	DWORD  biClrImportant;    // important colors
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
} BITMAPINFO;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

class BmpImage
{
public:
	BmpImage();
	BmpImage(const BmpImage &other);
	BmpImage& operator=(const BmpImage& other);

	bool Load(std::string fileName);
	bool Save(std::string fileName);
	bool GetPixel1(int x, int y);
	BYTE GetPixel8(int x, int y);
	RGBTRIPLE GetPixel24(int x, int y);
	void SetPixel8(int x, int y, BYTE val);
	void SetPixel24(int x, int y, RGBTRIPLE val);
	int GetWidth();
	int GetHeight();
	int GetSize();
	int GetBitsPerPixel();
	bool CreateGreyscaleDIB(int width, int height);
	bool ConvertToGreyScale(BmpImage& other);
	std::vector<BYTE> GetRawData();
	std::vector<int> GetLuminosityVector();
	void SetRawData(std::vector<BYTE>& rawData);

private:
	BITMAPFILEHEADER mHeader;
	BITMAPINFO *mBitmapInfo;
	BYTE* mBitmap;
	std::vector<BYTE> mData;
};
