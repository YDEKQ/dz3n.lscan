/* -------------------------------------------------------------------------- */

#ifndef __ip2text_h__
#define __ip2text_h__

/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */

char *ip2text(char *buf, unsigned int ip);
int text2ip(char *buf, unsigned int *ip);

/* -------------------------------------------------------------------------- */

#endif

/* -------------------------------------------------------------------------- */