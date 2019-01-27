/* -------------------------------------------------------------------------- */

#include "proxydlg.h"

/* -------------------------------------------------------------------------- */

void ProxydlgEnableDlgItem(HWND hwin, int ctl, int enable)
{
	EnableWindow(GetDlgItem(hwin, ctl), (BOOL)enable);
}

/* -------------------------------------------------------------------------- */

void EnableCtls(HWND hwin, int sel)
{
	if(sel == 0)
	{
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_HOST, 0);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_PORT, 0);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_USERNAME, 0);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_PASSWORD, 0);
	}
	else if(sel == 1)
	{
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_HOST, 1);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_PORT, 1);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_USERNAME, 0);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_PASSWORD, 0);
	}
	else if(sel == 2)
	{
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_HOST, 1);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_PORT, 1);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_USERNAME, 1);
		ProxydlgEnableDlgItem(hwin, IDC_PROXY_PASSWORD, 1);
	}
}

/* -------------------------------------------------------------------------- */

INT_PTR CALLBACK ProxydlgProc(HWND hwin, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hwinctl;
	int ctrl, code;
	proxydlg_params *params;
	char buf[32];

	if(msg == WM_INITDIALOG)
	{
		params = (void*)lp;
		SetWindowLongPtr(hwin, DWL_USER, (LPARAM)params);

		SendMessage(hwin, WM_SETICON, ICON_SMALL, (LPARAM)params->icon);
		SendMessage(hwin, WM_SETICON, ICON_BIG, (LPARAM)params->icon);

		params->combo = GetDlgItem(hwin, IDC_PROXY_TYPE);
		ComboBox_AddString(params->combo, "Выкл");
		ComboBox_AddString(params->combo, "SOCKS4");
		ComboBox_AddString(params->combo, "SOCKS5");
		if(params->proxy_type == 0)
		{
			ComboBox_SetCurSel(params->combo, 0);
			EnableCtls(hwin, 0);
		}
		else if(params->proxy_type == 4)
		{
			ComboBox_SetCurSel(params->combo, 1);
			EnableCtls(hwin, 1);
		}
		else if(params->proxy_type == 5)
		{
			ComboBox_SetCurSel(params->combo, 2);
		}
		SetDlgItemText(hwin, IDC_PROXY_HOST, ip2text(buf, params->proxy_host));
		SetDlgItemInt(hwin, IDC_PROXY_PORT, params->proxy_port, FALSE);
		SetDlgItemText(hwin, IDC_PROXY_USERNAME, params->username);
		SetDlgItemText(hwin, IDC_PROXY_PASSWORD, params->password);

		return 1;
	}

	else if(msg == WM_DESTROY)
	{
		return 1;
	}

	else if(msg == WM_CLOSE)
	{
		EndDialog(hwin, 0);
		return 1;
	}

	else if(msg == WM_COMMAND)
	{
		ctrl = LOWORD(wp);
		code = HIWORD(wp);
		hwinctl = (void*)lp;

		if(ctrl == IDOK)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if(ComboBox_GetCurSel(params->combo) == 0)
				params->proxy_type = 0;
			else if(ComboBox_GetCurSel(params->combo) == 1)
				params->proxy_type = 4;
			else if(ComboBox_GetCurSel(params->combo) == 2)
				params->proxy_type = 5;
			GetDlgItemText(hwin, IDC_PROXY_HOST, buf, sizeof(buf));
			if(!text2ip(buf, &(params->proxy_host)))
				params->proxy_host = 0;
			params->proxy_port = GetDlgItemInt(hwin, IDC_PROXY_PORT, NULL, FALSE);
			if(params->proxy_port < 1) params->proxy_port = 1;
			if(params->proxy_port > 65535) params->proxy_port = 65535;
			GetDlgItemText(hwin, IDC_PROXY_USERNAME, 
				params->username, params->max_username);
			GetDlgItemText(hwin, IDC_PROXY_PASSWORD,
				params->password, params->max_password);
			EndDialog(hwin, 1);
			return 1;
		}
		
		else if(ctrl == IDCANCEL)
		{
			EndDialog(hwin, 0);
			return 1;
		}

		else if(ctrl == IDC_PROXY_TYPE)
		{
			if(code == CBN_SELCHANGE)
			{
				EnableCtls(hwin, ComboBox_GetCurSel(hwinctl));
			}
		}
	}
	
	return 0;
}

/* -------------------------------------------------------------------------- */

int proxydlg(proxydlg_params *params)
{
	return DialogBoxParam(params->instance,
		MAKEINTRESOURCE(IDD_PROXYDLG),
		params->owner,
		ProxydlgProc,
		(LPARAM)params);
}

/* -------------------------------------------------------------------------- */
