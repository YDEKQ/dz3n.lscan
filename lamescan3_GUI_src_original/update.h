/* -------------------------------------------------------------------------- */

#ifndef __update_h__
#define __update_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include "http_get.h"

/* -------------------------------------------------------------------------- */

typedef struct __update_thread_param
{
	HWND owner;
	int notifyerrors;
	char *softwarename;
	char *currversion;
} update_thread_param;

/* -------------------------------------------------------------------------- */

int update_check(HWND hwin, char *softwarename, char *currversion, int notifyerrors);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
