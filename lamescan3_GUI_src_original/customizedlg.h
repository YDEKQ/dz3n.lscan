/* -------------------------------------------------------------------------- */

#ifndef __customize_h__
#define __customize_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include "resource.h"

/* -------------------------------------------------------------------------- */

typedef struct __customizedlg_params
{
	HINSTANCE instance;
	HWND owner;
	HICON icon;
	int max_radmin_path;
	char *radmin_path;
	int enable_window_capture;
	int minimize_to_tray;
	int enable_sounds;
	int check_updates;
} customizedlg_params;

/* -------------------------------------------------------------------------- */

int customizedlg(customizedlg_params *params);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
