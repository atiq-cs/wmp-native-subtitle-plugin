/////////////////////////////////////////////////////////////////////////////
//
// CWMPNativeSubtitle.cpp : Implementation of CWMPNativeSubtitle
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Modified by Saint Atique, September, 2013
// unix9n@gmail.com,
//
// Primary ref: http://msdn.microsoft.com/en-us/library/windows/desktop/dd564686(v=vs.85).aspx
// For pulgin type we are using ref: http://msdn.microsoft.com/en-us/library/windows/desktop/dd562476(v=vs.85).aspx
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WMPNativeSubtitle.h"
#include "CPropertyDialog.h"
// Saint Atique: for subtitle to sami conversion
#include "SAMIConversion.h"

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::CWMPNativeSubtitle
// Constructor

CWMPNativeSubtitle::CWMPNativeSubtitle():
		m_sFilePath(NULL),
		m_bLogSetting(FALSE)
{
    // wcsncpy_s(m_wszPluginText, sizeof(m_wszPluginText) / sizeof(m_wszPluginText[0]), L"CWMPNativeSubtitle Plugin", sizeof(m_wszPluginText) / sizeof(m_wszPluginText[0]));
    m_dwAdviseCookie = 0;

	// read m_bLogSetting from registry
	CRegKey key;
	LONG    lResult;

	// also consider whether this location kwszPrefsRegKey is readable/writeable
	lResult = key.Open(HKEY_CURRENT_USER, kwszPrefsRegKey, KEY_READ);
	if (ERROR_SUCCESS == lResult)
	{
		DWORD   dwValue = 0;
		DWORD dwType = 0;
		ULONG uLength = sizeof(dwValue);
		lResult = key.QueryValue(kwszPrefsLogging, &dwType, &dwValue, &uLength);

		if (ERROR_SUCCESS == lResult)
		{
			m_bLogSetting = (BOOL) (dwValue & 0x0001);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::~CWMPNativeSubtitle
// Destructor

CWMPNativeSubtitle::~CWMPNativeSubtitle()
{
	if (m_sFilePath)
		delete m_sFilePath;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle:::FinalConstruct
// Called when an plugin is first loaded. Use this function to do one-time
// intializations that could fail instead of doing this in the constructor,
// which cannot return an error.

HRESULT CWMPNativeSubtitle::FinalConstruct()
{
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle:::FinalRelease
// Called when a plugin is unloaded. Use this function to free any
// resources allocated in FinalConstruct.

void CWMPNativeSubtitle::FinalRelease()
{
    ReleaseCore();
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::SetCore
// Set WMP core interface

HRESULT CWMPNativeSubtitle::SetCore(IWMPCore *pCore)
{
    HRESULT hr = S_OK;

    // release any existing WMP core interfaces
    ReleaseCore();

    // If we get passed a NULL core, this  means
    // that the plugin is being shutdown.

    if (pCore == NULL)
    {
        return S_OK;
    }

    m_spCore = pCore;

    // connect up the event interface
    CComPtr<IConnectionPointContainer>  spConnectionContainer;

    hr = m_spCore->QueryInterface( &spConnectionContainer );

    if (SUCCEEDED(hr))
    {
        hr = spConnectionContainer->FindConnectionPoint( __uuidof(IWMPEvents), &m_spConnectionPoint );
    }

    if (SUCCEEDED(hr))
    {
        hr = m_spConnectionPoint->Advise( GetUnknown(), &m_dwAdviseCookie );

        if ((FAILED(hr)) || (0 == m_dwAdviseCookie))
        {
            m_spConnectionPoint = NULL;
        }
    }
	/* / only causes error
    if (SUCCEEDED(hr))
    {
		hr = m_spCore->QueryInterface(&m_spWMPPlayer);
	}*/

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::ReleaseCore
// Release WMP core interfaces

void CWMPNativeSubtitle::ReleaseCore()
{
    if (m_spConnectionPoint)
    {
        if (0 != m_dwAdviseCookie)
        {
            m_spConnectionPoint->Unadvise(m_dwAdviseCookie);
            m_dwAdviseCookie = 0;
        }
        m_spConnectionPoint = NULL;
    }

    if (m_spCore)
    {
        m_spCore = NULL;
    }

	/* // error on previous parts
    if (m_spWMPPlayer)
    {
        m_spWMPPlayer = NULL;
    }*/ 
}


/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::DisplayPropertyPage
// Display property page for plugin

HRESULT CWMPNativeSubtitle::DisplayPropertyPage(HWND hwndParent)
{
    CPropertyDialog dialog(this);

    dialog.DoModal(hwndParent);

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::GetProperty
// Get plugin property

HRESULT CWMPNativeSubtitle::GetProperty(const WCHAR *pwszName, VARIANT *pvarProperty)
{
    if (NULL == pvarProperty)
    {
        return E_POINTER;
    }

    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::SetProperty
// Set plugin property

HRESULT CWMPNativeSubtitle::SetProperty(const WCHAR *pwszName, const VARIANT *pvarProperty)
{
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::get_log_status
//
// Property get to retrieve log status via the public interface.
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CWMPNativeSubtitle::get_log_status(BOOL *pVal)
{
	if (NULL == pVal)
	{
		return E_POINTER;
	}

	*pVal = m_bLogSetting;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::set_log_status
//
// Property put to store the scale value via the public interface.
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CWMPNativeSubtitle::set_log_status(BOOL newVal)
{
	m_bLogSetting = newVal;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::EnableCaptionFromSubtitle
// Convert caption and load
// if this procedure is successful it returns true otherwise false
BOOL CWMPNativeSubtitle::EnableCaptionFromSubtitle() {
	// subtitles are available for conversion
	// currently we only support srt format
	LPTSTR subInputName = GetSubtitleFilePath();
	if (subInputName == NULL)
		return FALSE;

	// subInputName will be freed when SubToSAMIConverter object is destroyed
	SubToSAMIConverter subToSAMIConverter(subInputName, (BOOL) (m_bLogSetting & 0x0001));
	if (subToSAMIConverter.convert_to_sami()) {
		BSTR mediaFileName;
		HRESULT hr = m_spCore->get_URL(&mediaFileName);
		if (FAILMSG(hr))
			return FALSE;
		m_spCore->close();
		m_spCore->put_URL(mediaFileName);
		::SysFreeString(mediaFileName);
		return TRUE;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::CaptionAlreadyAvailable
// Check if SAMI caption is already available/loaded
// if an error occurs returns true, to avoid problems
// use the IWMPClosedCaption interface to retrieve info
BOOL CWMPNativeSubtitle::CaptionAlreadyAvailable() {
    CComPtr<IWMPClosedCaption> spWMPClosedCaption;

	HRESULT hr = E_FAIL;

	hr = m_spCore->get_closedCaption(&spWMPClosedCaption);
	if (FAILMSG(hr))
	    return true;

    if (spWMPClosedCaption)
    {
		BSTR smiFileName;
		hr = spWMPClosedCaption->get_SAMIFileName(&smiFileName);
		if (FAILMSG(hr))
			return TRUE;

		if (smiFileName != NULL) {
			BOOL isNotEmpty = (BOOL) wcscmp(smiFileName, L"");
			::SysFreeString(smiFileName);
			return isNotEmpty;
		}
    }
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::UpdateMediaFilePath
// Updates member variable string containing media file path
void CWMPNativeSubtitle::UpdateMediaFilePath() {
	// prepare pointer for update
	if (m_sFilePath)
		delete m_sFilePath;

	// set mediafilename
	BSTR sFileName;
	HRESULT hr = m_spCore->get_URL(&sFileName);
	if (FAILMSG(hr))
		return;
	// 1 for terminating NULL char
	const int sfnSize = SysStringLen(sFileName)+1;
	// will be freed during destruction of object as well
	m_sFilePath = new TCHAR[sfnSize];	// Ref: http://msdn.microsoft.com/en-us/library/ms221240(v=vs.85).aspx
	_tcscpy_s(m_sFilePath, sfnSize, sFileName);
	::SysFreeString(sFileName);
}

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle::GetNonSAMISubtitleName
// Search for open subtitle formats in a preferred order
// return subtitle file name if found
// if not found return null
LPTSTR CWMPNativeSubtitle::GetSubtitleFilePath() {
	// Get stream file name/URL
	BSTR sFileName;
	HRESULT hr = m_spCore->get_URL(&sFileName);
	if (FAILMSG(hr))
		return NULL;
	// 10 extra chars just in case, actually we may need 3/4 max
	const int sfnSize = SysStringLen(sFileName)+10;
	// caller function has the responsibility to free this memory
	LPTSTR streamFileName = new WCHAR[sfnSize];	// Ref: http://msdn.microsoft.com/en-us/library/ms221240(v=vs.85).aspx
	wcscpy_s(streamFileName, sfnSize, sFileName);
	::SysFreeString(sFileName);

	// get position where base file name ends = position of last dot
	int pos;
	for (pos = sfnSize-1; streamFileName[pos] != L'.' && pos>0; pos--);
	// streamFileName[pos] = L'X';
	pos++;
	// replace extension
	_tcscpy_s(&streamFileName[pos], sfnSize-pos, TEXT("srt"));
	if (FileExists(streamFileName))
		return streamFileName;
	else {
		delete streamFileName;
		streamFileName = NULL;
	}
	//::MessageBox(NULL, streamFileName, L"Stream info", MB_OK);
	return NULL;
}

////////////////////////////////////////////////////
// functions which are not members of class
// Helper function for GetSubtitleFilePath
BOOL FileExists(TCHAR * file)
{
   WIN32_FIND_DATA FindFileData;
   HANDLE handle = FindFirstFile(file, &FindFileData) ;
   int found = handle != INVALID_HANDLE_VALUE;
   if(found) 
   {
       //FindClose(&handle); this will crash
       FindClose(handle);
   }
   return (found != 0);
}

// check file extension for support
// Only include extensions that most possibly contain audio formats or formats that are not supported with caption
// Whether the extension is supported by ZuneVido or not it is not much important now
// Because excluding such a format will also block for WMP
// Include Zune video supported formats on FileExtSupportedInZuneVideoAppX
BOOL FileExtNotSupportedByPlugin(LPCTSTR sFile) {
	// ref: 
	if (StringEndsWith(sFile, TEXT(".m4a")) || StringEndsWith(sFile, TEXT(".mp3")) || StringEndsWith(sFile, TEXT(".wma")) || StringEndsWith(sFile, TEXT(".wav")) || \
		StringEndsWith(sFile, TEXT(".mp2")) || StringEndsWith(sFile, TEXT(".ivf")) || StringEndsWith(sFile, TEXT(".mpa")) || StringEndsWith(sFile, TEXT(".m3u")) || \
		StringEndsWith(sFile, TEXT(".wax")) || StringEndsWith(sFile, TEXT(".cda")) || StringEndsWith(sFile, TEXT(".mid")) || StringEndsWith(sFile, TEXT(".midi")) || \
		StringEndsWith(sFile, TEXT(".rmi")) || StringEndsWith(sFile, TEXT(".au")) || StringEndsWith(sFile, TEXT(".aac")))
		return TRUE;
	return FALSE;
}

/*  returns TRUE if str ends with suffix  */
BOOL StringEndsWith(LPCTSTR str, LPCTSTR suffix) {
  if( str == NULL || suffix == NULL )
    return 0;

  size_t str_len = _tcslen(str);
  size_t suffix_len = _tcslen(suffix);

  if(suffix_len > str_len)
    return FALSE;

  return 0 == _tcsncmp( str + str_len - suffix_len, suffix, suffix_len );
}
