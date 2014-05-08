/////////////////////////////////////////////////////////////////////////////
//
// SAMIConversion.h : Declaration of the CWMPNativeSubtitle
//
// Copyright (c) SAOS
// Author	:	Saint Atique
//				This header provides declarations for subtitle to SAMI conversion
// Date		:	September, 2013
// unix9n@gmail.com,
// Senior Software Engineer, REVE Systems
// Founder, SAOSLab
/////////////////////////////////////////////////////////////////////////////

#ifndef __SAMI_CONVERSION_H_
#define __SAMI_CONVERSION_H_

#pragma once

// Partial definition
enum TEXT_ENCODE_FORMAT;
#define SMIBUFFERSIZE	2049
#define MAX_SEQ_LENGTH	8

enum LINETYPE { NEWLINE, SEQUENCE, TIMESTAMP, TEXTLINE, UNKNOWN_LINE_TYPE };

class SubToSAMIConverter {
public:
	SubToSAMIConverter(LPTSTR infileName, BOOL bLogStatus);
	~SubToSAMIConverter();
	BOOL convert_to_sami();
	void writeLog(LPTSTR str);		// need as public for external access

private:
	LPTSTR inSubFileName;
	LPTSTR outSmiFileName;
	TCHAR samiBuffer[SMIBUFFERSIZE];		// we maintain buffer to speed up I/O operations

	// Related to file i/o operations 
	BOOL errorOccurred;
	HANDLE hInFile;
	HANDLE hOutFile;
	HANDLE hLogFile;
	BOOL firstReadData;
	TEXT_ENCODE_FORMAT encodingFormat;
	BOOL m_bLogEnabled;

	// private functions
	BOOL writeSmiText(LPWSTR bigStr);		// bom utf-8 not supported by WMP
	// BOOL writeSmiText(LPTSTR bigStr, BOOL BOM_flag = FALSE);
	BOOL get_sub_line(LPWSTR *lineStr, int *length);
	BOOL read_data_into_buffer();
	void EmbedUnicodeSymbols(LPWSTR pSubStr, LPWSTR pLine);
};

#define READBUFFERSIZE	1025
BOOL should_read_new_data(LPWSTR buf, int* newline_length, SubToSAMIConverter* pSamiConverter);
LINETYPE get_line_type(LPTSTR line, SubToSAMIConverter* pSamiConverter);
bool saIsDigit(const TCHAR ch);
DWORDLONG calculate_timestamp_ms(LPTSTR str, BOOL isEndingTime, SubToSAMIConverter* pSamiConverter);
bool isNumber(LPTSTR str, size_t length_to_check);
// void DisplayFileManagementError(LPTSTR lpszFunction);
BOOL CheckUnicodeSymbol(LPWSTR pLine);

#endif //__SAMI_CONVERSION_H_
