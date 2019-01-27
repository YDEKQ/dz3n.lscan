/* -------------------------------------------------------------------------- */

#ifndef __scanner_h__
#define __scanner_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include "range.h"
#include "ping.h"
#include "tcp.h"
#include "csv.h"
#include "radmin.h"
#include "resource.h"

/* -------------------------------------------------------------------------- */

typedef struct __scanned_host_status
{
	int success;
	char *status;
	char *client;
	unsigned int host;
	int port;
	char *ping;
	char *mechanism;
	char *login;
	char *password;
} scanned_host_status;

/* -------------------------------------------------------------------------- */

typedef struct __scanner_wordlist
{
	int capacity, length;
	char **items;
} scanner_wordlist;

/* -------------------------------------------------------------------------- */

typedef int (*scanner_on_error_callback)(void *param, char *title, char *message);
typedef int (*scanner_on_finish_callback)(void *param);
typedef int (*scanner_on_progress_callback)(void *param, int progress, int threadcount, int started, int paused);
typedef int (*scanner_on_status_callback)(void *param, int ident, int remove, scanned_host_status *status);
typedef int (*scanner_on_result_progress)(void *param, int total, int good);

/* -------------------------------------------------------------------------- */

typedef struct __scanner_settings
{
	int proxy_type;
	unsigned int proxy_host;
	int proxy_port;
	int max_proxy_username;
	char *proxy_username;
	int max_proxy_password;
	char *proxy_password;
	int max_thread_count;
	int random_scan_order;
	int delay_time;
	int ping_enable;
	int ping_packet_count;
	int ping_timeout;
	int ping_max_sessions;
	int max_dict1_path;
	char *dict1_path;
	int max_dict2_path;
	char *dict2_path;
	int max_port_list;
	char *port_list;
	int auto_remove_bad;
	int enable_sounds;
	HINSTANCE instance;
	int tcp_timeout;
	int max_errors;
	int error_delay;
} scanner_settings;

/* -------------------------------------------------------------------------- */

typedef struct __scanner_ranges
{
	range_t *queue;
	range_t *down;
	range_t *up;
	range_t *warn;
} scanner_ranges;

/* -------------------------------------------------------------------------- */

typedef struct __scanner_callbacks
{
	void *on_finish_param;
	scanner_on_finish_callback on_finish;
	void *on_progress_param;
	scanner_on_progress_callback on_progress;
	void *on_status_param;
	scanner_on_status_callback on_status;
	void *on_error_param;
	scanner_on_error_callback on_error;
	void *on_result_progress_param;
	scanner_on_result_progress on_result_progress;
} scanner_callbacks;

/* -------------------------------------------------------------------------- */

typedef struct __scanner_t
{
	scanner_settings *props;
	scanner_ranges *ranges;
	scanner_callbacks *callbacks;
	int threadcount;
	int started; // =1 если есть потоки
	int enabled; // =0 если сканер завершает работу
	int paused;
	int progress;
	int numports;
	int *portlist;
	scanner_wordlist *logins;
	scanner_wordlist *passwords;
	HANDLE statmutex;
	HANDLE pingsemaphore;
	int result_count;
	int result_count_good;
} scanner_t;

/* -------------------------------------------------------------------------- */

scanner_t *scanner_new(scanner_settings *props, scanner_ranges *ranges, scanner_callbacks *callbacks);
void scanner_free(scanner_t *cx);

int scanner_start(scanner_t *cx);
int scanner_stop(scanner_t *cx);
int scanner_pause(scanner_t *cx);
int scanner_isstarted(scanner_t *cx);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
