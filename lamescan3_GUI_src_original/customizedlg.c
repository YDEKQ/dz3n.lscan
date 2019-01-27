/* -------------------------------------------------------------------------- */

#include "customizedlg.h"

/* -------------------------------------------------------------------------- */

char *radmin_browse_title = "Выбрать клиент";
char *radmin_filter = "Радмин (Radmin.exe)\0Radmin.exe\0Программы (*.exe)\0*.exe\0";
char *radmin_ext = "exe";

/* -------------------------------------------------------------------------- */

void CustomizedlgSetCheckBox(HWND hwin, int ctl, int check)
{
	SendDlgItemMessage(hwin, ctl, BM_SETCHECK, 
		check?BST_CHECKED:BST_UNCHECKED, 0);
}

/* -------------------------------------------------------------------------- */

int CustomizedlgGetCheckBox(HWND hwin, int ctl)
{
	return SendDlgItemMessage(hwin, ctl, BM_GETCHECK,
		0, 0) == BST_CHECKED;
}

/* -------------------------------------------------------------------------- */

INT_PTR CALLBACK CustomizedlgProc(HWND hwin, UINT msg, WPARAM wp, LPARAM lp)
{
	int ctl;
	customizedlg_params *params;
	char *filename;
	OPENFILENAME ofn;
	if(msg == WM_INITDIALOG)
	{
		params = (void*)lp;
		SetWindowLongPtr(hwin, DWL_USER, (LONG)params);
		SendMessage(hwin, WM_SETICON, ICON_SMALL, (LPARAM)(params->icon));
		SendMessage(hwin, WM_SETICON, ICON_BIG, (LPARAM)(params->icon));
		SetDlgItemText(hwin, IDC_RADMIN_PATH, params->radmin_path);
		CustomizedlgSetCheckBox(hwin, IDC_ENABLE_WINDOW_CAPTURE, params->enable_window_capture);
		CustomizedlgSetCheckBox(hwin, IDC_MINIMIZE_TO_TRAY, params->minimize_to_tray);
		CustomizedlgSetCheckBox(hwin, IDC_ENABLE_SOUNDS, params->enable_sounds);
		CustomizedlgSetCheckBox(hwin, IDC_CHECK_UPDATES, params->check_updates);
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
		if(ctl == IDCANCEL)
		{
			EndDialog(hwin, 0);
			return 1;
		}
		else if(ctl == IDOK)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			GetDlgItemText(hwin, IDC_RADMIN_PATH, params->radmin_path, params->max_radmin_path);
			params->enable_window_capture = CustomizedlgGetCheckBox(hwin, IDC_ENABLE_WINDOW_CAPTURE);
			params->minimize_to_tray = CustomizedlgGetCheckBox(hwin, IDC_MINIMIZE_TO_TRAY);
			params->enable_sounds = CustomizedlgGetCheckBox(hwin, IDC_ENABLE_SOUNDS);
			params->check_updates = CustomizedlgGetCheckBox(hwin, IDC_CHECK_UPDATES);
			EndDialog(hwin, 1);
			return 1;
		}
		else if(ctl == IDC_RADMIN_PATH_BROWSE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((filename = malloc(MAX_PATH)))
			{
				GetDlgItemText(hwin, IDC_RADMIN_PATH, filename, MAX_PATH);
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hInstance = params->instance;
				ofn.hwndOwner = hwin;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFile = filename;
				ofn.lpstrTitle = radmin_browse_title;
				ofn.lpstrFilter = radmin_filter;
				ofn.lpstrDefExt = radmin_ext;
				ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
				if(GetOpenFileName(&ofn))
				{
					SetDlgItemText(hwin, IDC_RADMIN_PATH, filename);
				}
			}
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int customizedlg(customizedlg_params *params)
{
	return DialogBoxParam(params->instance,
		MAKEINTRESOURCE(IDD_CUSTOMIZEDLG),
		params->owner,
		CustomizedlgProc,
		(LPARAM)params);
}

/* -------------------------------------------------------------------------- */
