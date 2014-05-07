/////////////////////////////////////////////////////////////////////////////
//
// SAMIConversion.cpp : Implementation of conversion to SAMI from different formats
// Copyright (c) Microsoft Corporation. All rights reserved.
// Created by Saint Atique, September, 2013
// unix9n@gmail.com,
// TCHAR library has been used
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SAMIConversion.h"
#include "encoding.h"

/////////////////////////////////////////////////////////////////////////////
// SubToSAMIConverter::SubToSAMIConverter
// Constructor
SubToSAMIConverter::SubToSAMIConverter(LPTSTR infileName, BOOL bLogStatus) :
errorOccurred(FALSE),
firstReadData(TRUE),
encodingFormat(UNKNOWN_TEXT_TYPE),
m_bLogEnabled(bLogStatus)
{
	inSubFileName = infileName;

	// get output file name from in file name
	const int smiFNSize = _tcslen(inSubFileName) + 1;
	outSmiFileName = new TCHAR[smiFNSize];
	_tcsncpy_s(outSmiFileName, smiFNSize, inSubFileName, smiFNSize - 4);
	_tcscpy_s(&outSmiFileName[smiFNSize - 4], 4, TEXT("smi"));

	outLogFileName = new TCHAR[smiFNSize + 5];
	_tcsncpy_s(outLogFileName, smiFNSize, inSubFileName, smiFNSize - 5);
	_tcscpy_s(&outLogFileName[smiFNSize - 5], 9, TEXT("_log.txt"));
	// MessageBox(NULL, outLogFileName, TEXT("SAMI File Out info"), MB_OK);

	// initialize file handle
	hInFile = CreateFile(inSubFileName,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hInFile == INVALID_HANDLE_VALUE)
		errorOccurred = TRUE;

	// open output file handle
	hOutFile = CreateFile(outSmiFileName,                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_NEW,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hOutFile == INVALID_HANDLE_VALUE) {
		errorOccurred = TRUE;
		ErrorExit(outSmiFileName);
	}

	if (m_bLogEnabled) {
		// open log file handle
		hLogFile = CreateFile(outLogFileName,                // name of the write
			GENERIC_WRITE,          // open for writing
			0,                      // do not share
			NULL,                   // default security
			CREATE_NEW,             // create new file only
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);                  // no attr. template

		if (hLogFile == INVALID_HANDLE_VALUE)
			errorOccurred = TRUE;
	}
	else
		hLogFile = NULL;

	samiBuffer[0] = L'\0';
}

/////////////////////////////////////////////////////////////////////////////
// SubToSAMIConverter::~SubToSAMIConverter
// Destructor
SubToSAMIConverter::~SubToSAMIConverter() {
	// free occupied memories
	delete inSubFileName;
	inSubFileName = NULL;
	delete outSmiFileName;
	outSmiFileName = NULL;
	delete outLogFileName;
	outLogFileName=NULL;
}

/////////////////////////////////////////////////////////////////////////////
// SubToSAMIConverter::convertToSAMI
// Main convertion outline function
// returns true if conversion is successful
BOOL SubToSAMIConverter::convert_to_sami() {
	// initialize local variables
	// BOOL sub_found = FALSE;

	// check error status
	if (errorOccurred)
		return FALSE;

	LPTSTR linestr=NULL;		// will be allocated by getline, we are supporting UNICODE
	TCHAR substr[READBUFFERSIZE] = {0};
	int len=0;

	// write initial style description and header
	writeSmiText(L"<SAMI>\r\n\
<head>\r\n\
  <STYLE TYPE=\"text/css\">\r\n\
	<!-- \r\n\
		P {\r\n\
			font-size:1.2em;\r\n\
			font-family: Arial, Sans-Serif;\r\n\
			font-weight: normal;\r\n\
			color: #FFFFFF;\r\n\
			background-color: transparent;\r\n\
			text-align: center; }\r\n\
			.SACAPTION { name: English; lang: EN-US; }\r\n\
	-->\r\n\
  </STYLE>\r\n\
</head>\r\n\
<body>\r\n"/*, TRUE*/);

	writeLog(TEXT("convert_to_sami:: initial texts written\r\n"));

	DWORDLONG start_time = -1;
	DWORDLONG ending_time = -1;
	TCHAR tempStr[12] = {0};
	LPTSTR tempTimeStr = NULL;

	while (get_sub_line(&linestr, &len)) {
		// if line is sequence number ignore it
		// if line contains timestamp calculate time in ms and put time with tags
		// if line is sub put the sub; close tag for this whenever we get newline or sequence number
		// newline, can be \n or \r but get_line should only send \n: closes tag if not first time
		switch (get_line_type(linestr, this)) {
		case NEWLINE:
			writeLog(TEXT("convert_to_sami:: line type = NEWLINE\r\n\r\n"));
			// wiki: A blank line containing no text indicating the start of a new subtitle
			//  The subtitle separator, a blank line, is the double byte MS-DOS CR+LF pair, though the POSIX single byte linefeed is also well supported.
			// previous line type can only be TEXTLINE
			// 
			// output subtitle text and re-initialize buffer
			if (start_time == -1)
				return FALSE;
			if (ending_time == -1)
				return FALSE;

			if (substr[0] == L'\0')
				break;
			// output timestamp and sub with tags
			writeSmiText(L"  <SYNC Start=\"");
			tempStr[0] = _T('\0');
			_ui64tot_s(start_time, tempStr, 12, 10);
			writeSmiText(tempStr);
			writeSmiText(L"\">\r\n\t<p class=\"SACAPTION\">\r\n\t\t");

			// write subitle
			writeSmiText(substr);
			// close tag
			writeSmiText(L"\r\n\t</p>\r\n  </SYNC>\r\n");

			if (ending_time>0) {
				// output timestamp and sub with tags
				writeSmiText(L"  <SYNC Start=\"");
				tempStr[0] = _T('\0');
				_ui64tot_s(ending_time, tempStr, 12, 10);
				writeSmiText(tempStr);
				writeSmiText(L"\">\r\n\t<p class=\"SACAPTION\">\r\n\t\t");
				writeSmiText(L"&nbsp;");
				// end tag
				writeSmiText(L"\r\n\t</p>\r\n  </SYNC>\r\n");
			}
			else {
				writeLog(TEXT("convert_to_sami:: ending time problem "));
			
			}

			substr[0] = L'\0';
			break;
		case SEQUENCE:
			// wiki: A number beginning with 1 indicating which subtitle it is in the sequence
			//   - Subtitles are numbered sequentially, starting at 1
			// previous line type can only be NEWLINE or empty
			// keep the sequence number
			// if mismatch output to log file
			writeLog(TEXT("convert_to_sami:: sequence number = "));
			writeLog(linestr);
			writeLog(TEXT("\r\n\r\n"));
			break;
		case TIMESTAMP:
			// wiki: The timecode format used is hours:minutes:seconds,milliseconds with time units fixed to two zero padded digits and fractions fixed
			// - to three zero padded digits (00:00:00,000). The fractional separator used is the comma, since the program was written in France
			// output previous subline buffer from previous starting time if available
			// output nbsp from ending time
			// backup linestr as calculate_timestamp_ms will call destructive tokenizing function
			tempTimeStr = new TCHAR[_tcslen(linestr)+1];
			_tcscpy_s(tempTimeStr, _tcslen(linestr)+1, linestr);
			start_time = calculate_timestamp_ms(linestr, FALSE, this);
			delete linestr;
			linestr = tempTimeStr;
			tempTimeStr = NULL;
			tempTimeStr = new TCHAR[_tcslen(linestr)+1];
			_tcscpy_s(tempTimeStr, _tcslen(linestr)+1, linestr);
			ending_time = calculate_timestamp_ms(linestr, TRUE, this);
			delete linestr;
			linestr = tempTimeStr;
			tempTimeStr = NULL;
			writeLog(TEXT("convert_to_sami:: timestamp = "));
			writeLog(linestr);
			writeLog(TEXT("\r\n\r\n"));
			break;
		case TEXTLINE:
			// wiki: The subtitle itself on multiple lines.
			// add to sub buffer
			if (substr[0] != _T('\0'))
				wcscat_s(substr, READBUFFERSIZE, TEXT("<br>\r\n\t\t"));
			wcscat_s(substr, READBUFFERSIZE, linestr);
			writeLog(TEXT("convert_to_sami:: text = "));
			writeLog(linestr);
			writeLog(TEXT("\r\n\r\n"));

			break;
		default:		// shouldn't come here
			return FALSE;
		}

	}
	// if srt file is not properly ended with an extra blank line, handle this
				writeLog(TEXT("convert_to_sami:: line type = NEWLINE\r\n\r\n"));
			// wiki: A blank line containing no text indicating the start of a new subtitle
			//  The subtitle separator, a blank line, is the double byte MS-DOS CR+LF pair, though the POSIX single byte linefeed is also well supported.
			// previous line type can only be TEXTLINE
			// 
			// output subtitle text and re-initialize buffer
		if (substr[0] != L'\0') {
			if (start_time == -1)
				return FALSE;
			if (ending_time == -1)
				return FALSE;

			// output timestamp and sub with tags
			writeSmiText(L"  <SYNC Start=\"");
			tempStr[0] = _T('\0');
			_ui64tot_s(start_time, tempStr, 12, 10);
			writeSmiText(tempStr);
			writeSmiText(L"\">\r\n\t<p class=\"SACAPTION\">\r\n\t\t");

			// write subitle
			writeSmiText(substr);
			// close tag
			writeSmiText(L"\r\n\t</p>\r\n  </SYNC>\r\n");

			if (ending_time>0) {
				// output timestamp and sub with tags
				writeSmiText(L"  <SYNC Start=\"");
				tempStr[0] = _T('\0');
				_ui64tot_s(ending_time, tempStr, 12, 10);
				writeSmiText(tempStr);
				writeSmiText(L"\">\r\n\t<p class=\"SACAPTION\">\r\n\t\t");
				writeSmiText(L"&nbsp;");
				// end tag
				writeSmiText(L"\r\n\t</p>\r\n  </SYNC>\r\n");
			}
			substr[0] = L'\0';
		}


	// finish tag
	writeSmiText(L"</body>\r\n</SAMI>\r\n");
	if (linestr)
		delete linestr;

    // It is always good practice to close the open file handles even though
    // the app will exit here and clean up open handles anyway.
	if (hInFile) {
		CloseHandle(hInFile);
		hInFile = NULL;
	}
    CloseHandle(hOutFile);
	hOutFile = NULL;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::writeSmiText
// Take Unicode string to write into UTF-8 file
// if unicode to utf-8 conversion fails or write fails it returns an error
BOOL SubToSAMIConverter::writeSmiText(LPTSTR bigStr/*, BOOL BOM_flag*/) {
	LPSTR DataBuffer = ConvertUTF16ToUTF8(bigStr);
	if (DataBuffer == NULL) {
		return FALSE;
	}

	// convert bigStr to multi-byte first
	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
    DWORD dwBytesWritten = 0;

	BOOL bErrorFlag = WriteFile( 
                    hOutFile,           // open file handle
                    DataBuffer,      // start of data to write
                    dwBytesToWrite,  // number of bytes to write
                    &dwBytesWritten, // number of bytes that were written
                    NULL);            // no overlapped structure
	delete DataBuffer;
	DataBuffer = NULL;

	return (bErrorFlag && (dwBytesWritten == dwBytesToWrite));
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::writeLog
// Take Unicode string to write into UTF-8 file
// if unicode to utf-8 conversion fails or write fails it returns an error
void SubToSAMIConverter::writeLog(LPTSTR str) {
	if (m_bLogEnabled == FALSE)
		return;
	LPSTR DataBuffer = ConvertUTF16ToUTF8(str);
	if (DataBuffer == NULL) {
		MessageBox(NULL, TEXT("Data buffer null"), TEXT("buffer info"), MB_OK);
		return;
	}

	// convert bigStr to multi-byte first
	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
    DWORD dwBytesWritten = 0;

	BOOL bErrorFlag = WriteFile( 
					hLogFile,           // open file handle
                    DataBuffer,      // start of data to write
                    dwBytesToWrite,  // number of bytes to write
                    &dwBytesWritten, // number of bytes that were written
                    NULL);            // no overlapped structure

	delete DataBuffer;
	DataBuffer = NULL;

	if (bErrorFlag && (dwBytesWritten == dwBytesToWrite))
		return;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::GetNonSAMISubtitleName
// Get a line from file and convert to Unicode
// return false on eof or a failure
BOOL SubToSAMIConverter::get_sub_line(LPTSTR *lineStr, int *length) {
	if (should_read_new_data(samiBuffer, length, this)) {
		writeLog(TEXT("get_sub_line:: requests new data from file\r\n"));

		if (read_data_into_buffer() == FALSE)
			return FALSE;
		writeLog(TEXT("read_data_into_buffer:: \r\n-----------------------------------------------------------------------------------\r\n"));
		writeLog(samiBuffer);
		writeLog(TEXT("\r\n-----------------------------------------------------------------------------------\r\n"));
		writeLog(TEXT("get_sub_line:: successfully read data into buffer\r\n"));

		// we are running it now to get newline position
		if (should_read_new_data(samiBuffer, length, this)) {
			// if it requests again for new data that means something is wrong
			writeLog(TEXT("get_sub_line:: request for new data 2nd time not allowed.\r\n"));
			return FALSE;
		}
	}

	// get the line length, len is the length of data to be copied
	int len = *length;
	writeLog(TEXT("get_sub_line:: length upto newline = "));
	TCHAR tempStr[20] = {0};
	_itow_s((int) len, tempStr, 10);
	writeLog(tempStr);
	writeLog(TEXT("\r\n"));
	// corrected logic for \n and \r\n
	if (len == 0) {
		len += 1;		// allow allocating 1 extra character
	}
	// allocate memory for the line we'll extract now
	if (*lineStr)
		delete *lineStr;
	*lineStr = new TCHAR[len+1];
	(*lineStr)[0] = L'\0';
	
	// corrected logic for \n and \r\n, len has changed but *length hasn't
	if (*length == 0) {
		writeLog(TEXT("get_sub_line:: putting a blank line...\r\n"));
		(*lineStr)[0] = L'\n';
		(*lineStr)[1] = L'\0';
		len--;
	}
	else {
		// copy till newline, 1 for \n or \r
		wcsncpy_s(*lineStr, len+1, samiBuffer, len);
		(*lineStr)[len] = L'\0';		// otherwise it may result crash and other problems
	}

	// log
	writeLog(TEXT("get_sub_line: line = \""));
	writeLog(*lineStr);
	writeLog(TEXT("\"\r\n"));

	// now fix sami buffer for next use
	// point to next position \r\n
	if (samiBuffer[len] == L'\r')
		len += 2;		// for \r\n
	else
		len += 1;		// for \n


	int buf_len = wcslen(samiBuffer);
	// align buffer to initial
	wcsncpy_s(samiBuffer, SMIBUFFERSIZE, &samiBuffer[len], buf_len-len);
	samiBuffer[buf_len-len] = L'\0';

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::read_data_into_buffer
// read data and add to buffer
// return false on eof or a failure
BOOL SubToSAMIConverter::read_data_into_buffer() {
	// Read entire input file here
    DWORD  dwBytesRead = 0;
	BYTE ReadBuffer[READBUFFERSIZE] = {0};

    // Read one character less than the buffer size to save room for
    // the terminating NULL character. 

    if( FALSE == ReadFile(hInFile, ReadBuffer, READBUFFERSIZE-1, &dwBytesRead, NULL) )
    {
        CloseHandle(hInFile);
		hInFile = NULL;
		// returns false on eof
		writeLog(TEXT("read_data_into_buffer:: Buffer reading failed.\r\n"));
        return FALSE;
    }

    // This is the section of code that assumes the file is ANSI text. 
    // Modify this block for other data types if needed.

    if (dwBytesRead > 0 && dwBytesRead <= READBUFFERSIZE-1)
    {
        ReadBuffer[dwBytesRead]='\0'; // NULL character
		LPTSTR ReadBufferW = NULL;

		if (firstReadData) {
			encodingFormat =  get_text_file_encoding(ReadBuffer);
		}

		switch (encodingFormat) {
		case UTF32_TEXT:
			writeLog(TEXT("read_data_into_buffer:: UTF-32 encoding detected, returning false\r\n"));
			return FALSE;
		case UTF16_TEXT:
			writeLog(TEXT("read_data_into_buffer:: UTF-16 encoding detected\r\n"));
			if (firstReadData) {
				int ReadBufferWSize = dwBytesRead/2;
				ReadBufferW = new TCHAR[ReadBufferWSize];
				memcpy_s(ReadBufferW, dwBytesRead, &ReadBuffer[2], dwBytesRead-2);
				ReadBufferW[ReadBufferWSize-1] = _T('\0');
			}
			else {
				int ReadBufferWSize = dwBytesRead/2+1;
				ReadBufferW = new TCHAR[ReadBufferWSize];
				memcpy_s(ReadBufferW, dwBytesRead+2, ReadBuffer, dwBytesRead);
				ReadBufferW[ReadBufferWSize-1] = _T('\0');
			}
			break;
		case UTF8_TEXT_WITHOUT_BOM:
			// convert to Unicode
			// window thinks these are ANSI and conversion still works
			writeLog(TEXT("read_data_into_buffer:: UTF-8 encoding without BOM detected\r\n"));
			ReadBufferW = ConvertUTF8ToUTF16((LPCSTR) ReadBuffer);
			break;
		case UTF8_TEXT_WITH_BOM:		// fix for text files converted to utf-8 using windows notepad
			// special case; convert to Unicode; detect using bom
			writeLog(TEXT("read_data_into_buffer:: UTF-8 encoding with BOM detected\r\n"));
			// ReadBufferW = ConvertUTF8ToUTF16((LPCSTR) &ReadBuffer[3]);
			if (firstReadData)
				ReadBufferW = ConvertUTF8ToUTF16((LPCSTR) &ReadBuffer[3]);
			else
				ReadBufferW = ConvertUTF8ToUTF16((LPCSTR) ReadBuffer);
			break;
		case ANSI_TEXT:
			// convert to Unicode
			writeLog(TEXT("read_data_into_buffer:: ANSI encoding detected\r\n"));
			ReadBufferW = ConvertANSIToUTF16((LPCSTR) ReadBuffer);
			break;
		default:
			writeLog(TEXT("read_data_into_buffer:: Unknown encoding\r\n"));
			return FALSE;
		}
		if (firstReadData) {
			firstReadData = false;
		}

		if (ReadBufferW) {
			int len = wcslen(samiBuffer);
			if (SMIBUFFERSIZE-(DWORD)len < dwBytesRead) {
				// we just faced a buffer overflow
				writeLog(TEXT("read_data_into_buffer:: buffer overflow\r\n"));
				delete ReadBufferW;
				ReadBufferW = NULL;
				return FALSE;
			}

			wcscat_s(&samiBuffer[len], SMIBUFFERSIZE-len, ReadBufferW);
			if (ReadBufferW) {
				delete ReadBufferW;
				ReadBufferW = NULL;
			}

			writeLog(TEXT("read_data_into_buffer:: read data successful\r\n"));
			return TRUE;
		}
		else
			writeLog(TEXT("read_data_into_buffer:: buffer conversion failed\r\n"));
    }
    else if (dwBytesRead == 0)
    {
		// log to file
		if (GetLastError() == ERROR_HANDLE_EOF)
			writeLog(TEXT("read_data_into_buffer:: encountered EOF while reading from file.\r\n"));
		else {
			writeLog(TEXT("read_data_into_buffer:: no data read from file - probably EOF.\r\n"));
		}
    }
    else
    {
		// log to file
		writeLog(TEXT("read_data_into_buffer:: Unexpected value for dwBytesRead.\r\n"));
    }
	return FALSE;
}

// shouldn't allocate data here as caller pointer will not point to the allocated memory location
// but there is way to do this using pointer to pointer, however we are not going to make design change now
// this function should set newline_length with length of data only
// in case of \r\n we get 1 character length extra which we subtract
BOOL should_read_new_data(LPWSTR buf, int* newline_length, SubToSAMIConverter* pSamiConverter) {
	*newline_length = 0;
	LPTSTR pch;

	pch = _tcschr(buf, L'\n');
	if (pch) {
		int len = *newline_length = (int)(pch - buf);
		if (len > 0 && buf[len-1] == L'\r')
			(*newline_length)--;
		return FALSE;
	}

	return TRUE;
}

LINETYPE get_line_type(LPTSTR line, SubToSAMIConverter* pSamiConverter) {
	int length = _tcslen(line);

	if (line == NULL)
		return UNKNOWN_LINE_TYPE;

	if (line[0] == L'\n' && line[1] == L'\0')
		return NEWLINE;

	if (length<6) {// considering highest length of sequence is 5
		//int
		if (isNumber(line)) {
			return SEQUENCE;
		}
	}

	if (length>16) {
		// first digit must be numeric, 2nd one should be same or a :
		if (saIsDigit(line[0]) && (line[1] == L':' || saIsDigit(line[1]))) {
			// total number of colons should be 4
			int count=0;
			for (int i=0; line[i] != L'\0'; i++)
				if (line[i] == L':')
					count++;

			if (count == 4)
				return TIMESTAMP;
		}
	}

	return TEXTLINE;

}

bool isNumber(LPTSTR str) {
    // Does not accepts number preceding a - sign, as sequence in sub never negative
	int i, len = _tcslen(str);
	if (len == 0)
		return false;
    else if (len == 1 && (str[len-1] == L'\n' ||  str[len-1] == L'-'))
		return false;

	// quick hack for number lines ending with one or multiple spaces (happens sometimes) or newlines (adopted from old code)
	// in my opinion, a proper solution would be to first set variable
	// EXPECTED_LINE to sequence number when newline is found
	// when see that EXPECTED_LINE is sequence number we consider trimming spaces or illegal characters as such as that..
	// another advantage of EXPECTED_LINE would be to detect mistakes in sequence numbers
	
	// current suggestion is to correct these types of errors using regular expresion of notepad++ manually
	// currently we don't support such non-standard srt files; hence disabling that segment of code
    i=0;
	for (; i<len; i++)
		if (str[i] < L'0' || str[i] > L'9') {
			return false;
		}
	return true;
}

bool saIsDigit(const TCHAR ch) {
	if (ch >= L'0' && ch <= L'9')
		return true;
	return false;
}

// on error returns 0
DWORDLONG calculate_timestamp_ms(LPTSTR str, BOOL isEndingTime, SubToSAMIConverter* pSamiConverter) {
	LPTSTR delim=L"-> ";
	LPTSTR next_token = NULL;

	// Get first part of the string which containts starting time
	// wcstok_s modifies original string and returns pointer to the original as token
	LPTSTR time_str = wcstok_s(str, delim, &next_token);
	if (time_str == NULL)
		return 0;

	if (isEndingTime) {
		time_str = wcstok_s(NULL, delim, &next_token);
		if (time_str == NULL)
			return 0;
		pSamiConverter->writeLog(TEXT("calculate_timestamp_ms:: got end string = "));
		pSamiConverter->writeLog(time_str);
		pSamiConverter->writeLog(TEXT("\r\n"));
	}

	next_token = NULL;
	LPTSTR delim_time=TEXT(":,");
	// get hour
	LPTSTR token = wcstok_s(time_str, delim_time, &next_token);
	if (token == NULL)
		return 0;
	int hour = _wtoi(token);

	// get minute
	token = wcstok_s(NULL, delim_time, &next_token);
	if (token == NULL)
		return 0;
	int minute = _wtoi(token);

	// get second
	token = wcstok_s(NULL, delim_time, &next_token);
	if (token == NULL)
		return 0;
	int second = _wtoi(token);
	// get milisecond
	token = wcstok_s(NULL, delim_time, &next_token);
	if (token == NULL)
		return 0;
	int mili_sec = _wtoi(token);

	return ((DWORDLONG) mili_sec + (second + (minute + hour * 60) * (DWORDLONG) 60) * (DWORDLONG) 1000);
}
