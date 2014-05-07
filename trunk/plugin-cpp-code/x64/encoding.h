// Created by Saint Atique, September, 2013
// unix9n@gmail.com,
// Senior Software Engineer, REVE Systems
// Founder, SAOSLab

#ifndef __TEXT_ENCODING_H_
#define __TEXT_ENCODING_H_

#pragma once

enum TEXT_ENCODE_FORMAT { ANSI_TEXT, UTF8_TEXT_WITHOUT_BOM, UTF8_TEXT_WITH_BOM, UTF16_TEXT, UTF32_TEXT, UNKNOWN_TEXT_TYPE };

LPTSTR ConvertUTF8ToUTF16( __in LPCSTR pszTextUTF8 );
LPTSTR ConvertANSIToUTF16( __in LPCSTR pszTextANSI );
LPSTR ConvertUTF16ToUTF8( __in LPCWSTR pszTextUTF16 );
BOOL is_utf8_encoded(__in const unsigned char* inStr);
TEXT_ENCODE_FORMAT get_text_file_encoding(__in const unsigned char* inStr);
void ErrorExit(LPTSTR lpszFunction);

#endif //__TEXT_ENCODING_H_
