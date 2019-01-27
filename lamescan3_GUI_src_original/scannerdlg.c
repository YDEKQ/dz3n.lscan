/* -------------------------------------------------------------------------- */

#include "scannerdlg.h"

/* -------------------------------------------------------------------------- */

char *dict_browse_text = "Выбрать словарь";
char *dict_filter = "Файлы словарей (*.txt;*.dic)\0*.txt;*.dic\0Все файлы (*.*)\0*.*\0";
char *dict_ext = "txt";

/* -------------------------------------------------------------------------- */

void ScannerdlgSetCheckBox(HWND hwin, int ctl, int check)
{
	SendDlgItemMessage(hwin, ctl, BM_SETCHECK, 
		check?BST_CHECKED:BST_UNCHECKED, 0);
}

/* -------------------------------------------------------------------------- */

int ScannerdlgGetCheckBox(HWND hwin, int ctl)
{
	return SendDlgItemMessage(hwin, ctl, BM_GETCHECK,
		0, 0) == BST_CHECKED;
}

/* -------------------------------------------------------------------------- */

void ScannerdlgEnableDlgItem(HWND hwin, int ctl, int enable)
{
	EnableWindow(GetDlgItem(hwin, ctl), (BOOL)enable);
}

/* -------------------------------------------------------------------------- */

INT_PTR CALLBACK ScannerdlgProc(HWND hwin, UINT msg, WPARAM wp, LPARAM lp)
{
	int ctl;
	scannerdlg_params *params;
	OPENFILENAME ofn;
	char *filename;
	if(msg == WM_INITDIALOG)
	{
		params = (void*)lp;
		SetWindowLongPtr(hwin, DWL_USER, (LONG)params);

		SendMessage(hwin, WM_SETICON, ICON_SMALL, (LPARAM)(params->icon));
		SendMessage(hwin, WM_SETICON, ICON_BIG, (LPARAM)(params->icon));

		SetDlgItemInt(hwin, IDC_MAX_THREAD_COUNT, params->props->max_thread_count, FALSE);
		SetDlgItemInt(hwin, IDC_DELAY_TIME, params->props->delay_time, FALSE);
		SetDlgItemInt(hwin, IDC_TCP_TIMEOUT, params->props->tcp_timeout, FALSE);
		SetDlgItemInt(hwin, IDC_MAX_ERRORS, params->props->max_errors, FALSE);
		SetDlgItemInt(hwin, IDC_ERROR_DELAY, params->props->error_delay, FALSE);
		ScannerdlgSetCheckBox(hwin, IDC_RANDOM_SCAN_ORDER, params->props->random_scan_order);
		ScannerdlgSetCheckBox(hwin, IDC_PING_ENABLE, params->props->ping_enable);
		SetDlgItemInt(hwin, IDC_PING_PACKET_COUNT, params->props->ping_packet_count, FALSE);
		SetDlgItemInt(hwin, IDC_PING_TIMEOUT, params->props->ping_timeout, FALSE);
		SetDlgItemInt(hwin, IDC_PING_MAX_SESSIONS, params->props->ping_max_sessions, FALSE);
		SetDlgItemText(hwin, IDC_DICT1_PATH, params->props->dict1_path);
		SetDlgItemText(hwin, IDC_DICT2_PATH, params->props->dict2_path);
		SetDlgItemText(hwin, IDC_SCAN_PORTS, params->props->port_list);
		ScannerdlgSetCheckBox(hwin, IDC_AUTO_DELETE_BAD, params->props->auto_remove_bad);

		if(!params->props->ping_enable)
		{
			ScannerdlgEnableDlgItem(hwin, IDC_PING_PACKET_COUNT, 0);
			ScannerdlgEnableDlgItem(hwin, IDC_PING_TIMEOUT, 0);
		}
		
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
		ctl = LOWORD(wp);
		if(ctl == IDOK)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			params->props->max_thread_count = GetDlgItemInt(hwin, IDC_MAX_THREAD_COUNT, NULL, FALSE);
			if(params->props->max_thread_count < 1) params->props->max_thread_count = 1;
			if(params->props->max_thread_count > 4096) params->props->max_thread_count = 4096;
			params->props->delay_time = GetDlgItemInt(hwin, IDC_DELAY_TIME, NULL, FALSE);
			if(params->props->delay_time < 0) params->props->delay_time = 0;
			if(params->props->delay_time > 3600000) params->props->delay_time = 3600000;
			params->props->tcp_timeout = GetDlgItemInt(hwin, IDC_TCP_TIMEOUT, NULL, FALSE);
			if(params->props->tcp_timeout < 0) params->props->tcp_timeout = 0;
			if(params->props->tcp_timeout > 3600000) params->props->tcp_timeout = 3600000;
			params->props->max_errors = GetDlgItemInt(hwin, IDC_MAX_ERRORS, NULL, FALSE);
			if(params->props->max_errors < 0) params->props->max_errors = 0;
			if(params->props->max_errors > 1000) params->props->max_errors = 1000;
			params->props->error_delay = GetDlgItemInt(hwin, IDC_ERROR_DELAY, NULL, FALSE);
			if(params->props->error_delay < 0) params->props->error_delay = 0;
			if(params->props->error_delay > 3600) params->props->error_delay = 3600;
			params->props->random_scan_order = ScannerdlgGetCheckBox(hwin, IDC_RANDOM_SCAN_ORDER);
			params->props->ping_enable = ScannerdlgGetCheckBox(hwin, IDC_PING_ENABLE);
			params->props->ping_packet_count = GetDlgItemInt(hwin, IDC_PING_PACKET_COUNT, NULL, FALSE);
			if(params->props->ping_packet_count < 1) params->props->ping_packet_count = 1;
			if(params->props->ping_packet_count > 256) params->props->ping_packet_count = 256;
			params->props->ping_timeout = GetDlgItemInt(hwin, IDC_PING_TIMEOUT, NULL, FALSE);
			if(params->props->ping_timeout < 1) params->props->ping_timeout = 1;
			if(params->props->ping_timeout > 60000) params->props->ping_timeout = 60000;
			params->props->ping_max_sessions = GetDlgItemInt(hwin, IDC_PING_MAX_SESSIONS, NULL, FALSE);
			if(params->props->ping_max_sessions < 1) params->props->ping_max_sessions = 1;
			if(params->props->ping_max_sessions > 256) params->props->ping_max_sessions = 256;
			GetDlgItemText(hwin, IDC_DICT1_PATH, params->props->dict1_path, params->props->max_dict1_path);
			GetDlgItemText(hwin, IDC_DICT2_PATH, params->props->dict2_path, params->props->max_dict2_path);
			GetDlgItemText(hwin, IDC_SCAN_PORTS, params->props->port_list, params->props->max_port_list);
			params->props->auto_remove_bad = ScannerdlgGetCheckBox(hwin, IDC_AUTO_DELETE_BAD);
			EndDialog(hwin, 1);
			return 1;
		}
		else if(ctl == IDCANCEL)
		{
			EndDialog(hwin, 0);
			return 1;
		}
		else if(ctl == IDC_PING_ENABLE)
		{
			if(ScannerdlgGetCheckBox(hwin, IDC_PING_ENABLE))
			{
				ScannerdlgEnableDlgItem(hwin, IDC_PING_PACKET_COUNT, 1);
				ScannerdlgEnableDlgItem(hwin, IDC_PING_TIMEOUT, 1);
				ScannerdlgEnableDlgItem(hwin, IDC_PING_MAX_SESSIONS, 1);
			}
			else
			{
				ScannerdlgEnableDlgItem(hwin, IDC_PING_PACKET_COUNT, 0);
				ScannerdlgEnableDlgItem(hwin, IDC_PING_TIMEOUT, 0);
				ScannerdlgEnableDlgItem(hwin, IDC_PING_MAX_SESSIONS, 0);
			}
			return 1;
		}
		else if( (ctl == IDC_DICT1_BROWSE) || 
			(ctl == IDC_DICT2_BROWSE) )
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((filename = malloc(MAX_PATH)))
			{
				GetDlgItemText(hwin, 
					(ctl==IDC_DICT1_BROWSE)?IDC_DICT1_PATH:IDC_DICT2_PATH, 
					filename, 
					MAX_PATH);
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hInstance = params->instance;
				ofn.hwndOwner = hwin;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFile = filename;
				ofn.lpstrFilter = dict_filter;
				ofn.lpstrTitle = dict_browse_text;
				ofn.lpstrDefExt = dict_ext;
				ofn.Flags = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
				if(GetOpenFileName(&ofn))
				{
					SetDlgItemText(hwin, 
						(ctl==IDC_DICT1_BROWSE)?IDC_DICT1_PATH:IDC_DICT2_PATH, 
						filename);
				}
				free(filename);
			}
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int scannerdlg(scannerdlg_params *params)
{
	return DialogBoxParam(params->instance,
		MAKEINTRESOURCE(IDD_SCANNERDLG),
		params->owner,
		ScannerdlgProc,
		(LPARAM)params);
}

/* -------------------------------------------------------------------------- */
