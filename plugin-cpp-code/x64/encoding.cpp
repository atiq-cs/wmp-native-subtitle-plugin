/////////////////////////////////////////////////////////////////////////////
//
// encoding.cpp : Implementation of encoding contexts
// Copyright (c) Microsoft Corporation. All rights reserved.
// Use of tchar library
// Created by Saint Atique, September, 2013
// unix9n@gmail.com,
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "encoding.h"
#include <strsafe.h> // StringCchPrintf

// Ref: http://stackoverflow.com/questions/1031645/how-to-detect-utf-8-in-plain-c
// Modified by Saint Atique
// 
BOOL is_utf8_encoded(__in const unsigned char* inStr) {
    // string assumed not to be null

    const unsigned char * bytes = (const unsigned char *) inStr;
    while(*bytes)		// that takes of null
    {
        if( (// ASCII
             // use bytes[0] <= 0x7F to allow ASCII control characters
                bytes[0] == 0x09 ||
                bytes[0] == 0x0A ||
                bytes[0] == 0x0D ||
                (0x20 <= bytes[0] && bytes[0] <= 0x7E)
            )
        ) {
            bytes += 1;
            continue;
        }

        if( (// non-overlong 2-byte
                (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF)
            )
        ) {
            bytes += 2;
            continue;
        }

        if( (// excluding overlongs
                bytes[0] == 0xE0 &&
                (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                    bytes[0] == 0xEE ||
                    bytes[0] == 0xEF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// excluding surrogates
                bytes[0] == 0xED &&
                (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            )
        ) {
            bytes += 3;
            continue;
        }

        if( (// planes 1-3
                bytes[0] == 0xF0 &&
                (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// planes 4-15
                (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// plane 16
                bytes[0] == 0xF4 &&
                (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            )
        ) {
            bytes += 4;
            continue;
        }

        return FALSE;
    }

    return TRUE;
}

// 
// Modified by Saint Atique
// Without ASCII
BOOL is_ANSI_encoded(__in const unsigned char* inStr) {
    // string assumed not to be null at first char

    const unsigned char * bytes = (const unsigned char *) inStr;
    while(*bytes)		// that takes of null
    {
        if( (// ASCII
             // use bytes[0] <= 0x7F to allow ASCII control characters
                bytes[0] == 0x09 ||
                bytes[0] == 0x0A ||
                bytes[0] == 0x0D ||
                (0x20 <= bytes[0] && bytes[0] <= 0x7E)
            )
        ) {
            bytes += 1;
            continue;
        }

        if( (// non-overlong 2-byte
                (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF)
            )
        ) {
            bytes += 2;
            return FALSE;
        }

        if( (// excluding overlongs
                bytes[0] == 0xE0 &&
                (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                    bytes[0] == 0xEE ||
                    bytes[0] == 0xEF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// excluding surrogates
                bytes[0] == 0xED &&
                (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            )
        ) {
            bytes += 3;
            return FALSE;
        }

        if( (// planes 1-3
                bytes[0] == 0xF0 &&
                (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// planes 4-15
                (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// plane 16
                bytes[0] == 0xF4 &&
                (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            )
        ) {
            bytes += 4;
            return FALSE;
        }

        return TRUE;
    }

    return TRUE;
}

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
	if (dw == 5) {	// Access denied, designed for file error logging
		StringCchPrintf((LPTSTR)lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s - couldn't create a file: %s (code %d)"),
			lpszFunction, lpMsgBuf, dw);

	}
	else
		StringCchPrintf((LPTSTR)lpDisplayBuf, 
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s failed with error %d: %s"), 
			lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_ICONERROR); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    // ExitProcess(dw); 
}


//----------------------------------------------------------------------------
// FUNCTION: ConvertUTF8ToUTF16
// DESC: Converts Unicode UTF-8 text to Unicode UTF-16 (Windows default).
// Modified by Saint Atique: ref: http://msmvps.com/blogs/gdicanio/archive/2010/01/04/conversion-between-unicode-utf-16-and-utf-8-in-c-win32.aspx
// we should add the soft error handline of this function to others as well
//  we have replaced atlthrow as it causes the application to crash
//----------------------------------------------------------------------------
LPTSTR ConvertUTF8ToUTF16( __in LPCSTR pszTextUTF8 )
{
    //
    // Special case of NULL or empty input string
    //
    if ( (pszTextUTF8 == NULL) || (*pszTextUTF8 == '\0') )
    {
        // Return empty string
        return NULL;
    }
 
 
    //
    // Consider CHAR's count corresponding to total input string length,
    // including end-of-string (\0) character
    //
    const size_t cchUTF8Max = INT_MAX - 1;
    size_t cchUTF8;
    HRESULT hr = ::StringCchLengthA( pszTextUTF8, cchUTF8Max, &cchUTF8 );
    if ( FAILED( hr ) )
    {
        // AtlThrow( hr );
		return NULL;
    }
   
    // Consider also terminating \0
    ++cchUTF8;
 
    // Convert to 'int' for use with MultiByteToWideChar API
    int cbUTF8 = static_cast<int>( cchUTF8 );
 
 
    //
    // Get size of destination UTF-16 buffer, in WCHAR's
    //
    int cchUTF16 = ::MultiByteToWideChar(
        CP_UTF8,                // convert from UTF-8
        MB_ERR_INVALID_CHARS,   // error on invalid chars
        pszTextUTF8,            // source UTF-8 string
        cbUTF8,                 // total length of source UTF-8 string,
                                // in CHAR's (= bytes), including end-of-string \0
        NULL,                   // unused - no conversion done in this step
        0                       // request size of destination buffer, in WCHAR's
        );
	
	if (cchUTF16 == 0) {
		// MessageBoxA(NULL, pszTextUTF8, "ConvertUTF8ToUTF16", MB_OK);
		return NULL;
	}

    /* ATLASSERT( cchUTF16 != 0 );
    if ( cchUTF16 == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
 
    //
    // Allocate destination buffer to store UTF-16 string
    //
    LPTSTR pszUTF16 = new TCHAR[cchUTF16+1];
 
    //
    // Do the conversion from UTF-8 to UTF-16
    //
    int result = ::MultiByteToWideChar(
        CP_UTF8,                // convert from UTF-8
        MB_ERR_INVALID_CHARS,   // error on invalid chars
        pszTextUTF8,            // source UTF-8 string
        cbUTF8,                 // total length of source UTF-8 string,
                                // in CHAR's (= bytes), including end-of-string \0
        pszUTF16,               // destination buffer
        cchUTF16                // size of destination buffer, in WCHAR's
        );

	if (result == 0) {
		//MessageBox(NULL, TEXT("MultiByteToWideChar failed"), TEXT("ConvertUTF8ToUTF16"), MB_OK);
		ErrorExit(TEXT("ConvertUTF8ToUTF16"));
		delete pszUTF16;
		pszUTF16 = NULL;
	}
		/*
    ATLASSERT( result != 0 );
    if ( result == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
    // free memory; caller has the responsibility to free it
	// delete pszUTF16;
	// pszUTF16 = NULL;
 
    // Return resulting UTF16 string
    return pszUTF16;
}

//----------------------------------------------------------------------------
// FUNCTION: ConvertUTF8ToUTF16
// DESC: Converts Unicode UTF-8 text to Unicode UTF-16 (Windows default).
// Modified by Saint Atique: ref: http://msmvps.com/blogs/gdicanio/archive/2010/01/04/conversion-between-unicode-utf-16-and-utf-8-in-c-win32.aspx
// added soft error handling here as well
//----------------------------------------------------------------------------
LPTSTR ConvertANSIToUTF16( __in LPCSTR pszTextANSI )
{
    //
    // Special case of NULL or empty input string
    //
    if ( (pszTextANSI == NULL) || (*pszTextANSI == '\0') )
    {
        // Return empty string
        return NULL;
    }
 
 
    //
    // Consider CHAR's count corresponding to total input string length,
    // including end-of-string (\0) character
    //
    const size_t cchANSIMax = INT_MAX - 1;
    size_t cchANSI;
    HRESULT hr = ::StringCchLengthA( pszTextANSI, cchANSIMax, &cchANSI );
    if ( FAILED( hr ) )
    {
        AtlThrow( hr );
    }
   
    // Consider also terminating \0
    ++cchANSI;
 
    // Convert to 'int' for use with MultiByteToWideChar API
    int cbANSI = static_cast<int>( cchANSI );
 
 
    //
    // Get size of destination UTF-16 buffer, in WCHAR's
    //
    int cchUTF16 = ::MultiByteToWideChar(
        CP_ACP,                // convert from UTF-8
        MB_ERR_INVALID_CHARS,   // error on invalid chars
        pszTextANSI,            // source UTF-8 string
        cbANSI,                 // total length of source UTF-8 string,
                                // in CHAR's (= bytes), including end-of-string \0
        NULL,                   // unused - no conversion done in this step
        0                       // request size of destination buffer, in WCHAR's
        );

	if (cchUTF16 == 0) {
		MessageBoxA(NULL, pszTextANSI, "ConvertANSIToUTF16", MB_OK);
		return NULL;
	}

    /*ATLASSERT( cchUTF16 != 0 );
    if ( cchUTF16 == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
 
    //
    // Allocate destination buffer to store UTF-16 string
    //
    LPTSTR pszUTF16 = new TCHAR[cchUTF16+1];
 
    //
    // Do the conversion from UTF-8 to UTF-16
    //
    int result = ::MultiByteToWideChar(
		CP_ACP,                // convert from UTF-8
        MB_ERR_INVALID_CHARS,   // error on invalid chars
        pszTextANSI,            // source UTF-8 string
        cbANSI,                 // total length of source UTF-8 string,
                                // in CHAR's (= bytes), including end-of-string \0
        pszUTF16,               // destination buffer
        cchUTF16                // size of destination buffer, in WCHAR's
        );

	if (result == 0) {
		MessageBox(NULL, TEXT("MultiByteToWideChar failed"), TEXT("ConvertANSIToUTF16"), MB_OK);
		delete pszUTF16;
		pszUTF16 = NULL;
	}


    /* ATLASSERT( result != 0 );
    if ( result == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
    // free memory; caller has the responsibility to free it
	// delete pszUTF16;
	// pszUTF16 = NULL;
 
    // Return resulting UTF16 string
    return pszUTF16;
}

//----------------------------------------------------------------------------
// FUNCTION: ConvertUTF16TosmiANSI
// DESC: Converts Unicode UTF-16 (Windows default) text to ANSI format.
// As smi caption actually only understands ansi we are writing ANSI
// We are replacing Unicode characters with equivalent html entities
// 
// Written by Saint Atique
// Layout:
//		1. keep the ascii characters intact as they are same in unicode
//		UCS characters U+0000 to U+007F (ASCII) are encoded simply as bytes 0x00 to 0x7F (ASCII compatibility).	This means that files
//		and strings which contain only 7-bit ASCII characters have the same encoding under both ASCII and UTF-8
//			ref: http://stackoverflow.com/questions/10361579/are-unicode-and-ascii-characters-the-
//		Methord: enumerate all the unicode characters
//			if they are in range 0-127 copy the right side byte to output buffer
//			otherwise convert to hex value and add to output buffer
//----------------------------------------------------------------------------
LPSTR ConvertUTF16TosmiANSI( __in LPCWSTR pszTextUTF16 )
{
    //
    // Special case of NULL or empty input string
    //
    if ( (pszTextUTF16 == NULL) || (*pszTextUTF16 == L'\0') )
    {
        // Return empty string
        return NULL;
    }
 
 
    //
    // Consider WCHAR's count corresponding to total input string length,
    // including end-of-string (L'\0') character.
    //
    const size_t cchUTF16Max = INT_MAX - 1;
    size_t cchUTF16;
    HRESULT hr = ::StringCchLengthW( pszTextUTF16, cchUTF16Max, &cchUTF16 );
    if ( FAILED( hr ) )
    {
         AtlThrow( hr );
    }
 
    // Consider also terminating \0
    ++cchUTF16;
 
 
    //
    // WC_ERR_INVALID_CHARS flag is set to fail if invalid input character
    // is encountered.
    // This flag is supported on Windows Vista and later.
    // Don't use it on Windows XP and previous.
    //
#if (WINVER >= 0x0600)
    DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#else
    DWORD dwConversionFlags = 0;
#endif
 
    //
    // Get size of destination UTF-8 buffer, in CHAR's (= bytes)
    //
    int cbUTF8 = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
        dwConversionFlags,      // specify conversion behavior
        pszTextUTF16,           // source UTF-16 string
        static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's,
                                        // including end-of-string \0
        NULL,                   // unused - no conversion required in this step
        0,                      // request buffer size
        NULL, NULL              // unused
        );

    if ( cbUTF8 == 0 )
    {
		MessageBox(NULL, pszTextUTF16, TEXT("ConvertUTF16ToUTF8 WideCharToMultiByte failed"), MB_OK);
		return NULL;
        // AtlThrow( hr );
    }

    /*ATLASSERT( cbUTF8 != 0 );
    if ( cbUTF8 == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
 
    //
    // Allocate destination buffer for UTF-8 string
    //
    int cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
	// caller has the responsibility to delete it
	LPSTR pszUTF8 = new CHAR[cchUTF8+1];
 
    //
    // Do the conversion from UTF-16 to UTF-8
    //
    int result = ::WideCharToMultiByte(
        CP_UTF8,                // convert to UTF-8
        dwConversionFlags,      // specify conversion behavior
        pszTextUTF16,           // source UTF-16 string
        static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's,
                                        // including end-of-string \0
        pszUTF8,                // destination buffer
        cbUTF8,                 // destination buffer size, in bytes
        NULL, NULL              // unused
        ); 

	if (result == 0) {
		MessageBox(NULL, TEXT("WideCharToMultiByte failed"), TEXT("ConvertUTF16ToUTF8"), MB_OK);
		delete pszUTF8;
		pszUTF8 = NULL;
	}

    /*ATLASSERT( result != 0 );
    if ( result == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
    // Return resulting UTF-8 string
    return pszUTF8;
}


//----------------------------------------------------------------------------
// FUNCTION: ConvertUTF16ToUTF8
// DESC: Converts Unicode UTF-16 (Windows default) text to Unicode UTF-8.
// Modified by Saint Atique
//----------------------------------------------------------------------------
LPSTR ConvertUTF16ToUTF8( __in LPCWSTR pszTextUTF16 )
{
    //
    // Special case of NULL or empty input string
    //
    if ( (pszTextUTF16 == NULL) || (*pszTextUTF16 == L'\0') )
    {
        // Return empty string
        return NULL;
    }
 
 
    //
    // Consider WCHAR's count corresponding to total input string length,
    // including end-of-string (L'\0') character.
    //
    const size_t cchUTF16Max = INT_MAX - 1;
    size_t cchUTF16;
    HRESULT hr = ::StringCchLengthW( pszTextUTF16, cchUTF16Max, &cchUTF16 );
    if ( FAILED( hr ) )
    {
         AtlThrow( hr );
    }
 
    // Consider also terminating \0
    ++cchUTF16;
 
 
    //
    // WC_ERR_INVALID_CHARS flag is set to fail if invalid input character
    // is encountered.
    // This flag is supported on Windows Vista and later.
    // Don't use it on Windows XP and previous.
    //
#if (WINVER >= 0x0600)
    DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#else
    DWORD dwConversionFlags = 0;
#endif
 
    //
    // Get size of destination UTF-8 buffer, in CHAR's (= bytes)
    //
    int cbUTF8 = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
        dwConversionFlags,      // specify conversion behavior
        pszTextUTF16,           // source UTF-16 string
        static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's,
                                        // including end-of-string \0
        NULL,                   // unused - no conversion required in this step
        0,                      // request buffer size
        NULL, NULL              // unused
        );

    if ( cbUTF8 == 0 )
    {
		MessageBox(NULL, pszTextUTF16, TEXT("ConvertUTF16ToUTF8 WideCharToMultiByte failed"), MB_OK);
		return NULL;
        // AtlThrow( hr );
    }

    /*ATLASSERT( cbUTF8 != 0 );
    if ( cbUTF8 == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
 
    //
    // Allocate destination buffer for UTF-8 string
    //
    int cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
	// caller has the responsibility to delete it
	LPSTR pszUTF8 = new CHAR[cchUTF8+1];
 
    //
    // Do the conversion from UTF-16 to UTF-8
    //
    int result = ::WideCharToMultiByte(
        CP_UTF8,                // convert to UTF-8
        dwConversionFlags,      // specify conversion behavior
        pszTextUTF16,           // source UTF-16 string
        static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's,
                                        // including end-of-string \0
        pszUTF8,                // destination buffer
        cbUTF8,                 // destination buffer size, in bytes
        NULL, NULL              // unused
        ); 

	if (result == 0) {
		MessageBox(NULL, TEXT("WideCharToMultiByte failed"), TEXT("ConvertUTF16ToUTF8"), MB_OK);
		delete pszUTF8;
		pszUTF8 = NULL;
	}

    /*ATLASSERT( result != 0 );
    if ( result == 0 )
    {
        AtlThrowLastWin32();
    }*/
 
    // Return resulting UTF-8 string
    return pszUTF8;
}


// ** Lessons learnt
// 1. hex value should be compared with unsigned char/ BYTE
// 2. local functions char array don't go out of scope in another function

TEXT_ENCODE_FORMAT get_text_file_encoding(__in const unsigned char* inStr) {
	// UTF-32 detection
	// BOM is 00 00 FE FF (for BE) or FF FE 00 00 (for LE).
	BYTE firstByte = inStr[0];
	BYTE secondByte = inStr[1];
	BYTE thirdByte = inStr[2];
	// msdn Using Byte Order Marks ref: http://msdn.microsoft.com/en-us/library/windows/desktop/dd374101(v=vs.85).aspx

	// Windows is LE
	// big endian and little endian
	if ((firstByte == 0x00 && secondByte == 0x00 && thirdByte == 0xFE && inStr[3] == 0xFF) || (firstByte == 0xFF && secondByte == 0xFE && thirdByte == 0x00 && inStr[3] == 0x00))
		return UTF32_TEXT;
	// BOM is FE FF (for BE) or FF FE (for LE). Note that the UTF-16LE BOM is found at the start of the UTF-32LE BOM, so check UTF-32 first.
	// There may be UTF-16 files without a BOM, but it would be really hard to detect them. The only reliable way to recognize UTF-16 without a BOM is to look for surrogate pairs (D[8-B]xx D[C-F]xx), but non-BMP characters are too rarely-used to make this approach practical.
	if ((firstByte == 0xFE && secondByte == 0xFF) || (firstByte == 0xFF && secondByte == 0xFE))
		return UTF16_TEXT;
	// The UTF-8 representation of the BOM is the byte sequence 0xEF,0xBB,0xBF
	// for windows notepad; double check of is_utf8_encoded, just in case
	if ((firstByte == 0xEF && secondByte == 0xBB && thirdByte == 0xBF) && is_utf8_encoded(&inStr[3]))
		return UTF8_TEXT_WITH_BOM;

	if (is_ANSI_encoded(inStr) == FALSE)
		return UTF8_TEXT_WITHOUT_BOM;

	return ANSI_TEXT;
}
