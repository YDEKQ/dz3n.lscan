/* -------------------------------------------------------------------------- */

#ifndef __settings_h__
#define __settings_h__

/* -------------------------------------------------------------------------- */

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "ip2text.h"

/* -------------------------------------------------------------------------- */

#define SET_CHAPTER		"settings"

/* -------------------------------------------------------------------------- */

int set_init();
void set_final();
int set_getint(char *key, int def);
int set_setint(char *key, int value);
int set_setstr(char *key, char *value);
int set_getstr(char *key, char *value, int maxchars);
unsigned int set_getip(char *key, unsigned int def);
int set_setip(char *key, unsigned int ip);
int set_getbool(char *key, int def);
int set_setbool(char *key, int value);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */
