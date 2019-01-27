/* -------------------------------------------------------------------------- */

#ifndef __mainwin_h__
#define __mainwin_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include "range.h"
#include "ip2text.h"
#include "tcp.h"
#include "csv.h"
#include "scanner.h"
#include "settings.h"
#include "rangewin.h"
#include "proxydlg.h"
#include "scannerdlg.h"
#include "customizedlg.h"
#include "update.h"
#include "resource.h"

/* -------------------------------------------------------------------------- */

#define RANGE_OP_ADD		0
#define RANGE_OP_REM		1
#define RANGE_OP_MERGE		2
#define RANGE_OP_CLEAR		3

/* -------------------------------------------------------------------------- */

typedef struct __edit_range_params
{
	range_t *range;
	range_t *range2;
	int op;
	unsigned int begin;
	unsigned int end;
} edit_range_params;

/* -------------------------------------------------------------------------- */

typedef struct __range_callback_hdr
{
	range_t *range;
	HWND hwin_control;
	int image_index;
	char *group_title;
	HANDLE mutex;
} range_callback_hdr;

/* -------------------------------------------------------------------------- */

typedef struct __status_callback_hdr
{
	HWND hwin_control;
	int radmin_image;
	int radmin_warn_image;
	HANDLE mutex;
	int lastid;
} status_callback_hdr;

/* -------------------------------------------------------------------------- */

typedef struct __range_exp_hdr
{
	char *header;
	range_t *range;
} range_exp_hdr;

/* -------------------------------------------------------------------------- */

typedef struct __mainwin_params
{
	// Public
	HINSTANCE instance;
	HWND owner;

	// Main icon
	HICON icon;

	// Result context menu
	HMENU results_context;

	// Imagelist
	HIMAGELIST imagelist;
	HICON icon_host_down;
	int image_host_down;
	HICON icon_host_up;
	int image_host_up;
	HICON icon_host_warn;
	int image_host_warn;
	HICON icon_host_queue;
	int image_host_queue;
	HICON icon_radmin;
	int image_radmin;
	HICON icon_radmin_warn;
	int image_radmin_warn;

	// Controls
	HWND ranges;
	HWND results;
	status_callback_hdr status_hdr;

	// Ranges
	HANDLE range_control_mutex;
	range_callback_hdr range_queue_hdr;
	range_callback_hdr range_down_hdr;
	range_callback_hdr range_up_hdr;
	range_callback_hdr range_warn_hdr;

	// Dialogs init data
	rangewin_range_elem *add_range_init_data;
	int add_range_init_limit;
	int add_range_init_count;

	// Settings
	scanner_settings scanprops;
	int max_radmin_path;
	char *radmin_path;
	int enable_window_capture;
	int minimize_to_tray;
	int auto_update;
	int skip_update_times;

	// Scanner context
	scanner_callbacks scancallbacks;
	scanner_ranges scanranges;
	scanner_t *scanner;

	// Notify icon data
	NOTIFYICONDATA notifyicon;
} mainwin_params;

/* -------------------------------------------------------------------------- */

typedef struct __lvsort_params
{
	HWND hwincontrol;
	int subitem;
} lvsort_params;

/* -------------------------------------------------------------------------- */

typedef struct __radmin_passwd_params
{
	DWORD processId;
	char *login;
	char *password;
	int success;
	HWND edits[4];
	HWND buttons[4];
	int numedits;
	int maxedits;
	int numbuttons;
	int maxbuttons;
} radmin_passwd_params;

/* -------------------------------------------------------------------------- */

int mainwin(mainwin_params *params, int show);

/* -------------------------------------------------------------------------- */

#endif __mainwin_h__

/* -------------------------------------------------------------------------- */
