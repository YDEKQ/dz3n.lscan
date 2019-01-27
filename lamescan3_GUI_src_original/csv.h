/* -------------------------------------------------------------------------- */

#ifndef __csv_h__
#define __csv_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */

#define CSV_MODE_RD		1
#define CSV_MODE_WR		2

/* -------------------------------------------------------------------------- */

typedef struct __csv_t
{
	int mode;
	int maxcount;
	int maxline;
	char *sep;
	FILE *f;
} csv_t;

/* -------------------------------------------------------------------------- */

int csv_readline(csv_t *csv, char **items, int *count);
csv_t *csv_open_write(char *filename, char *sep, int maxline);
int csv_writeline(csv_t *csv, char **items, int count);
csv_t *csv_open_read(char *filename, char *sep, int maxline, int maxcount);
void csv_close(csv_t *csv);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
