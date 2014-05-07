/////////////////////////////////////////////////////////////////////////////
//
// WMPNativeSubtitleevents.cpp : Implementation of CWMPNativeSubtitle events
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Modified by Saint Atique, September, 2013
// unix9n@gmail.com,
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WMPNativeSubtitle.h"

BOOL FAILMSG(HRESULT hr)
{
	const int MAX_STRING_SIZE = 1024;
    if (FAILED(hr))
    {
		WCHAR   wszError[MAX_STRING_SIZE];

		::swprintf_s(wszError, MAX_STRING_SIZE, L"Error code = %08X", hr);
        ::MessageBox(NULL, wszError, L"Error", MB_OK | MB_ICONERROR);
    }

    return FAILED(hr);
}

void CWMPNativeSubtitle::OpenStateChange( long NewState )
{
    switch (NewState)
    {
    case wmposUndefined:
        break;
    case wmposPlaylistChanging:
        break;
    case wmposPlaylistLocating:
        break;
    case wmposPlaylistConnecting:
        break;
    case wmposPlaylistLoading:
        break;
    case wmposPlaylistOpening:
        break;
    case wmposPlaylistOpenNoMedia:
        break;
    case wmposPlaylistChanged:
        break;
    case wmposMediaChanging:
        break;
    case wmposMediaLocating:
        break;
    case wmposMediaConnecting:
        break;
    case wmposMediaLoading:
        break;
    case wmposMediaOpening:
        break;
    case wmposMediaOpen:
    {
		// update for reopen
		UpdateMediaFilePath();

		// if file extension has no support for caption
		 if (FileExtNotSupportedByPlugin(m_sFilePath))
			 break;

		// if (smi or sami exist) do nothing
		if (CaptionAlreadyAvailable())
			break;
		if (EnableCaptionFromSubtitle() == FALSE) {
			// add more cool stuff
			break;
		}
		break;
    }
    case wmposBeginCodecAcquisition:
        break;
    case wmposEndCodecAcquisition:
        break;
    case wmposBeginLicenseAcquisition:
        break;
    case wmposEndLicenseAcquisition:
        break;
    case wmposBeginIndividualization:
        break;
    case wmposEndIndividualization:
        break;
    case wmposMediaWaiting:
        break;
    case wmposOpeningUnknownURL:
        break;
    default:
        break;
    }
}

void CWMPNativeSubtitle::PlayStateChange( long NewState )
{
    switch (NewState)
    {
    case wmppsUndefined:
        break;
    case wmppsStopped:
        break;
    case wmppsPaused:
        break;
    case wmppsPlaying:
        break;
    case wmppsScanForward:
        break;
    case wmppsScanReverse:
        break;
    case wmppsBuffering:
        break;
    case wmppsWaiting:
        break;
    case wmppsMediaEnded:
        break;
    case wmppsTransitioning:
       break;
    case wmppsReady:
        break;
    case wmppsReconnecting:
        break;
    case wmppsLast:
        break;
    default:
        break;
    }
}

void CWMPNativeSubtitle::AudioLanguageChange( long LangID )
{
}

void CWMPNativeSubtitle::StatusChange()
{
}

void CWMPNativeSubtitle::ScriptCommand( BSTR scType, BSTR Param )
{
}

void CWMPNativeSubtitle::NewStream()
{
}

void CWMPNativeSubtitle::Disconnect( long Result )
{
}

void CWMPNativeSubtitle::Buffering( VARIANT_BOOL Start )
{
}

void CWMPNativeSubtitle::Error()
{
    CComPtr<IWMPError>      spError;
    CComPtr<IWMPErrorItem>  spErrorItem;
    HRESULT                 dwError = S_OK;
    HRESULT                 hr = S_OK;

    if (m_spCore)
    {
        hr = m_spCore->get_error(&spError);

        if (SUCCEEDED(hr))
        {
            hr = spError->get_item(0, &spErrorItem);
        }

        if (SUCCEEDED(hr))
        {
            hr = spErrorItem->get_errorCode( (long *) &dwError );
        }
    }
}

void CWMPNativeSubtitle::Warning( long WarningType, long Param, BSTR Description )
{
}

void CWMPNativeSubtitle::EndOfStream( long Result )
{
}

void CWMPNativeSubtitle::PositionChange( double oldPosition, double newPosition)
{
}

void CWMPNativeSubtitle::MarkerHit( long MarkerNum )
{
}

void CWMPNativeSubtitle::DurationUnitChange( long NewDurationUnit )
{
}

void CWMPNativeSubtitle::CdromMediaChange( long CdromNum )
{
}

void CWMPNativeSubtitle::PlaylistChange( IDispatch * Playlist, WMPPlaylistChangeEventType change )
{
    switch (change)
    {
    case wmplcUnknown:
        break;
    case wmplcClear:
        break;
    case wmplcInfoChange:
        break;
    case wmplcMove:
        break;
    case wmplcDelete:
        break;
    case wmplcInsert:
        break;
    case wmplcAppend:
        break;
    case wmplcPrivate:
        break;
    case wmplcNameChange:
        break;
    case wmplcMorph:
        break;
    case wmplcSort:
        break;
    case wmplcLast:
        break;
    default:
        break;
    }
}

void CWMPNativeSubtitle::CurrentPlaylistChange( WMPPlaylistChangeEventType change )
{
    switch (change)
    {
    case wmplcUnknown:
        break;
    case wmplcClear:
        break;
    case wmplcInfoChange:
        break;
    case wmplcMove:
        break;
    case wmplcDelete:
        break;
    case wmplcInsert:
        break;
    case wmplcAppend:
        break;
    case wmplcPrivate:
        break;
    case wmplcNameChange:
        break;
    case wmplcMorph:
        break;
    case wmplcSort:
        break;
    case wmplcLast:
        break;
    default:
        break;
    }
}

void CWMPNativeSubtitle::CurrentPlaylistItemAvailable( BSTR bstrItemName )
{
}

void CWMPNativeSubtitle::MediaChange( IDispatch * Item )
{
}

void CWMPNativeSubtitle::CurrentMediaItemAvailable( BSTR bstrItemName )
{
}

void CWMPNativeSubtitle::CurrentItemChange( IDispatch *pdispMedia)
{
}

void CWMPNativeSubtitle::MediaCollectionChange()
{
}

void CWMPNativeSubtitle::MediaCollectionAttributeStringAdded( BSTR bstrAttribName,  BSTR bstrAttribVal )
{
}

void CWMPNativeSubtitle::MediaCollectionAttributeStringRemoved( BSTR bstrAttribName,  BSTR bstrAttribVal )
{
}

void CWMPNativeSubtitle::MediaCollectionAttributeStringChanged( BSTR bstrAttribName, BSTR bstrOldAttribVal, BSTR bstrNewAttribVal)
{
}

void CWMPNativeSubtitle::PlaylistCollectionChange()
{
}

void CWMPNativeSubtitle::PlaylistCollectionPlaylistAdded( BSTR bstrPlaylistName)
{
}

void CWMPNativeSubtitle::PlaylistCollectionPlaylistRemoved( BSTR bstrPlaylistName)
{
}

void CWMPNativeSubtitle::PlaylistCollectionPlaylistSetAsDeleted( BSTR bstrPlaylistName, VARIANT_BOOL varfIsDeleted)
{
}

void CWMPNativeSubtitle::ModeChange( BSTR ModeName, VARIANT_BOOL NewValue)
{
}

void CWMPNativeSubtitle::MediaError( IDispatch * pMediaObject)
{
}

void CWMPNativeSubtitle::OpenPlaylistSwitch( IDispatch *pItem )
{
}

void CWMPNativeSubtitle::DomainChange( BSTR strDomain)
{
}

void CWMPNativeSubtitle::SwitchedToPlayerApplication()
{
}

void CWMPNativeSubtitle::SwitchedToControl()
{
}

void CWMPNativeSubtitle::PlayerDockedStateChange()
{
}

void CWMPNativeSubtitle::PlayerReconnect()
{
}

void CWMPNativeSubtitle::Click( short nButton, short nShiftState, long fX, long fY )
{
}

void CWMPNativeSubtitle::DoubleClick( short nButton, short nShiftState, long fX, long fY )
{
}

void CWMPNativeSubtitle::KeyDown( short nKeyCode, short nShiftState )
{
}

void CWMPNativeSubtitle::KeyPress( short nKeyAscii )
{
}

void CWMPNativeSubtitle::KeyUp( short nKeyCode, short nShiftState )
{
}

void CWMPNativeSubtitle::MouseDown( short nButton, short nShiftState, long fX, long fY )
{
}

void CWMPNativeSubtitle::MouseMove( short nButton, short nShiftState, long fX, long fY )
{
}

void CWMPNativeSubtitle::MouseUp( short nButton, short nShiftState, long fX, long fY )
{
}
