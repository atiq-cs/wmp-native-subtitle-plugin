/////////////////////////////////////////////////////////////////////////////
//
// WMPNativeSubtitle.h : Declaration of the CWMPNativeSubtitle
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Modified by Saint Atique, September, 2013
// unix9n@gmail.com,
// Senior Software Engineer, REVE Systems
// Founder, SAOSLab
/////////////////////////////////////////////////////////////////////////////

#ifndef __WMPNativeSubtitle_H_
#define __WMPNativeSubtitle_H_

#pragma once

#include "resource.h"
#include "wmpplug.h"

// registry location for preferences, not sure whether this is the best
//const WCHAR kwszPrefsRegKey[] = L"Software\\Microsoft\\MediaPlayer\\WmpNativeSubtitlePlugin";
const WCHAR kwszPrefsRegKey[] = L"Software\\Microsoft\\MediaPlayer\\UIPlugins\\{52738E25-987F-4CA8-A674-5154267BF422}\\WmpNativeSubtitle";
const WCHAR kwszPrefsLogging[] = L"LogSettings";

// {52738E25-987F-4CA8-A674-5154267BF422}
DEFINE_GUID(CLSID_WMPNativeSubtitle, 0x52738e25, 0x987f, 0x4ca8, 0xa6, 0x74, 0x51, 0x54, 0x26, 0x7b, 0xf4, 0x22);

/////////////////////////////////////////////////////////////////////////////
// CWMPNativeSubtitle
class ATL_NO_VTABLE CWMPNativeSubtitle : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CWMPNativeSubtitle, &CLSID_WMPNativeSubtitle>,
    public IWMPEvents,
    public IWMPPluginUI
{
public:
    CWMPNativeSubtitle();
    ~CWMPNativeSubtitle();

DECLARE_REGISTRY_RESOURCEID(IDR_WMPNativeSubtitle)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWMPNativeSubtitle)
    COM_INTERFACE_ENTRY(IWMPEvents)
    COM_INTERFACE_ENTRY(IWMPPluginUI)
END_COM_MAP()

    // CComCoClass methods
    HRESULT FinalConstruct();
    void    FinalRelease();

    // IWMPPluginUI methods
    STDMETHODIMP SetCore(IWMPCore *pCore);
    STDMETHODIMP Create(HWND hwndParent, HWND *phwndWindow) { return E_NOTIMPL; }
    STDMETHODIMP Destroy() { return E_NOTIMPL; }
    STDMETHODIMP TranslateAccelerator(LPMSG lpmsg) { return E_NOTIMPL; }
    STDMETHODIMP DisplayPropertyPage(HWND hwndParent);
    STDMETHODIMP GetProperty(const WCHAR *pwszName, VARIANT *pvarProperty);
    STDMETHODIMP SetProperty(const WCHAR *pwszName, const VARIANT *pvarProperty);

    // IWMPEvents methods
    void STDMETHODCALLTYPE OpenStateChange( long NewState );
    void STDMETHODCALLTYPE PlayStateChange( long NewState );
    void STDMETHODCALLTYPE AudioLanguageChange( long LangID );
    void STDMETHODCALLTYPE StatusChange();
    void STDMETHODCALLTYPE ScriptCommand( BSTR scType, BSTR Param );
    void STDMETHODCALLTYPE NewStream();
    void STDMETHODCALLTYPE Disconnect( long Result );
    void STDMETHODCALLTYPE Buffering( VARIANT_BOOL Start );
    void STDMETHODCALLTYPE Error();
    void STDMETHODCALLTYPE Warning( long WarningType, long Param, BSTR Description );
    void STDMETHODCALLTYPE EndOfStream( long Result );
    void STDMETHODCALLTYPE PositionChange( double oldPosition, double newPosition);
    void STDMETHODCALLTYPE MarkerHit( long MarkerNum );
    void STDMETHODCALLTYPE DurationUnitChange( long NewDurationUnit );
    void STDMETHODCALLTYPE CdromMediaChange( long CdromNum );
    void STDMETHODCALLTYPE PlaylistChange( IDispatch * Playlist, WMPPlaylistChangeEventType change );
    void STDMETHODCALLTYPE CurrentPlaylistChange( WMPPlaylistChangeEventType change );
    void STDMETHODCALLTYPE CurrentPlaylistItemAvailable( BSTR bstrItemName );
    void STDMETHODCALLTYPE MediaChange( IDispatch * Item );
    void STDMETHODCALLTYPE CurrentMediaItemAvailable( BSTR bstrItemName );
    void STDMETHODCALLTYPE CurrentItemChange( IDispatch *pdispMedia);
    void STDMETHODCALLTYPE MediaCollectionChange();
    void STDMETHODCALLTYPE MediaCollectionAttributeStringAdded( BSTR bstrAttribName,  BSTR bstrAttribVal );
    void STDMETHODCALLTYPE MediaCollectionAttributeStringRemoved( BSTR bstrAttribName,  BSTR bstrAttribVal );
    void STDMETHODCALLTYPE MediaCollectionAttributeStringChanged( BSTR bstrAttribName, BSTR bstrOldAttribVal, BSTR bstrNewAttribVal);
    void STDMETHODCALLTYPE PlaylistCollectionChange();
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistAdded( BSTR bstrPlaylistName);
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistRemoved( BSTR bstrPlaylistName);
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistSetAsDeleted( BSTR bstrPlaylistName, VARIANT_BOOL varfIsDeleted);
    void STDMETHODCALLTYPE ModeChange( BSTR ModeName, VARIANT_BOOL NewValue);
    void STDMETHODCALLTYPE MediaError( IDispatch * pMediaObject);
    void STDMETHODCALLTYPE OpenPlaylistSwitch( IDispatch *pItem );
    void STDMETHODCALLTYPE DomainChange( BSTR strDomain);
    void STDMETHODCALLTYPE SwitchedToPlayerApplication();
    void STDMETHODCALLTYPE SwitchedToControl();
    void STDMETHODCALLTYPE PlayerDockedStateChange();
    void STDMETHODCALLTYPE PlayerReconnect();
    void STDMETHODCALLTYPE Click( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE DoubleClick( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE KeyDown( short nKeyCode, short nShiftState );
    void STDMETHODCALLTYPE KeyPress( short nKeyAscii );
    void STDMETHODCALLTYPE KeyUp( short nKeyCode, short nShiftState );
    void STDMETHODCALLTYPE MouseDown( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE MouseMove( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE MouseUp( short nButton, short nShiftState, long fX, long fY );

	// WmpNativeSubtitle methods
	STDMETHOD(get_log_status)(BOOL *pVal);
	STDMETHOD(set_log_status)(BOOL newVal);

private:
    void         ReleaseCore();

    CComPtr<IWMPCore>           m_spCore;
    CComPtr<IConnectionPoint>   m_spConnectionPoint;
    DWORD                       m_dwAdviseCookie;

	// Added by Saint Atique
	LPTSTR		m_sFilePath;
	BOOL m_bLogSetting;

	// Private functions added by Saint Atique
	BOOL	EnableCaptionFromSubtitle();
	BOOL	CaptionAlreadyAvailable();
	LPTSTR	GetSubtitleFilePath();
	void	UpdateMediaFilePath();
};

// Added by Saint Atique
BOOL FAILMSG(HRESULT hr);
BOOL FileExists(TCHAR * file);		// ref: http://stackoverflow.com/questions/3828835/how-can-we-check-if-a-file-exists-or-not-using-win32-program
// for metro app
BOOL FileExtNotSupportedByPlugin(LPCTSTR sFile);
BOOL StringEndsWith(LPCTSTR str, LPCTSTR suffix);
#endif //__WMPNativeSubtitle_H_
