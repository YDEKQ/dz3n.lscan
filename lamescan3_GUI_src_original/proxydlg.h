/* -------------------------------------------------------------------------- */

#ifndef __proxydlg_h__
#define __proxydlg_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include <windowsx.h>
#include "ip2text.h"
#include "resource.h"

/* -------------------------------------------------------------------------- */

typedef struct __proxydlg_params
{
	HINSTANCE instance;
	HWND owner;
	HICON icon;
	int proxy_type;
	unsigned int proxy_host;
	int proxy_port;
	int max_username;
	char *username;
	int max_password;
	char *password;
	HWND combo;
} proxydlg_params;

/* -------------------------------------------------------------------------- */

int proxydlg(proxydlg_params *params);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
