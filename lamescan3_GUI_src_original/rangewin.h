/* -------------------------------------------------------------------------- */

#ifndef __rangewin_h__
#define __rangewin_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include <richedit.h>
#include "ip2text.h"
#include "resource.h"

/* -------------------------------------------------------------------------- */

typedef struct __rangewin_range_elem
{
	unsigned int begin;
	unsigned int end;
} rangewin_range_elem;

/* -------------------------------------------------------------------------- */

typedef struct __rangewin_param
{
	HINSTANCE instance;
	HWND owner;
	HICON icon;
	char *title;
	int range_limit;
	int range_count;
	rangewin_range_elem *range_data;
	HFONT font;
} rangewin_param;

/* -------------------------------------------------------------------------- */

int rangewin(rangewin_param *param);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
