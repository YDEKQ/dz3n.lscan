/* -------------------------------------------------------------------------- */

#ifndef __scannerdlg_h__
#define __scannerdlg_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include "resource.h"
#include "scanner.h"

/* -------------------------------------------------------------------------- */

typedef struct __scannerdlg_params
{
	HINSTANCE instance;
	HWND owner;
	HICON icon;
	scanner_settings *props;
} scannerdlg_params;

/* -------------------------------------------------------------------------- */

int scannerdlg(scannerdlg_params *params);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
