/////////////////////////////////////////////////////////////////////////////
//
// CPropertyDialog.h : Declaration of the CPropertyDialog
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Modified by Saint Atique, September, 2013
// unix9n@gmail.com,
// Senior Software Engineer, REVE Systems
// Founder, SAOSLab
/////////////////////////////////////////////////////////////////////////////

#include "atlwin.h"

class CPropertyDialog : public CDialogImpl<CPropertyDialog>
{
public:
    enum { IDD = IDD_PROPERTYDIALOG };

    BEGIN_MSG_MAP(CPropertyDialog)
        MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
        COMMAND_ID_HANDLER( IDOK, OnOK )
        COMMAND_ID_HANDLER( IDCANCEL, OnCancel )
    END_MSG_MAP()

    CPropertyDialog(CWMPNativeSubtitle *pPlugin)
    {
        m_pPlugin = pPlugin;
    }

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& fHandled)
	{
		CenterWindow();
		// initialize text
		// SetDlgItemText(IDC_MESSAGEEDIT, m_pPlugin->m_wszPluginText);

		BOOL bLogStatus = FALSE;

		// extra test
		if (m_pPlugin) {
			m_pPlugin->get_log_status(&bLogStatus);
			if (bLogStatus)
				SendDlgItemMessage(IDC_CHECK_LOG, BM_SETCHECK, BST_CHECKED, (int) bLogStatus);
			else
				SendDlgItemMessage(IDC_CHECK_LOG, BM_SETCHECK, BST_UNCHECKED, (int) bLogStatus);

			// MessageBox(TEXT("m_pPlugin not null"));		ref: http://msdn.microsoft.com/en-us/library/778b51zb.aspx
		}
		return TRUE;
    }

    LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hwndCtl, BOOL& fHandled)
    {
		int logStatus = 0;
		UINT32 state = IsDlgButtonChecked(IDC_CHECK_LOG);

		if (state == BST_CHECKED)
		{
			logStatus = 1;
		}
		else
		{
			logStatus = 0;
		}

        // unnecessary, save text
        // GetDlgItemText(IDC_MESSAGEEDIT, m_pPlugin->m_wszPluginText, sizeof(m_pPlugin->m_wszPluginText) / sizeof(m_pPlugin->m_wszPluginText[0]));

		// update registry
		CRegKey key;
		LONG    lResult;

		lResult = key.Create(HKEY_CURRENT_USER, kwszPrefsRegKey);
		if (ERROR_SUCCESS == lResult)
		{
			DWORD dwValue = (DWORD) logStatus;
			lResult = key.SetValue(kwszPrefsLogging, REG_DWORD, &dwValue, sizeof(dwValue));
		}

		// update plug-in class member
		if (m_pPlugin)
		{
			m_pPlugin->set_log_status(logStatus);
		}

        EndDialog( IDOK );
        return 0;
    }

    LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hwndCtl, BOOL& fHandled)
    {
        EndDialog( IDCANCEL );
        return 0;
    }

private:
    CWMPNativeSubtitle  *m_pPlugin;  // pointer to plugin object
};

