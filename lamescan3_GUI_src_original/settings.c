/* -------------------------------------------------------------------------- */

#include "settings.h"

/* -------------------------------------------------------------------------- */

char *set_chapter = SET_CHAPTER;
char *set_filename = NULL;

/* -------------------------------------------------------------------------- */

int set_init()
{
	char *p, *q;
	if((set_filename = malloc(MAX_PATH)))
	{
		if(GetModuleFileName(NULL, set_filename, MAX_PATH))
		{
			if((p = strrchr(set_filename, '\\')))
				if((q = strrchr(p, '.')))
					*q = 0;
			strcat(set_filename, ".ini");
		}
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

void set_final()
{
	free(set_filename);
	set_filename = NULL;
}

/* -------------------------------------------------------------------------- */

int set_setstr(char *key, char *value)
{
	if(WritePrivateProfileString(set_chapter, key, value, set_filename))
		return 1;
	return 0;
}

/* -------------------------------------------------------------------------- */

int set_getstr(char *key, char *value, int maxchars)
{
	char *def;
	if(value)
	{
		if((def = strdup(value)))
		{
			GetPrivateProfileString(set_chapter, key, def, value, maxchars, set_filename);
			free(def);
		}
	}
	return 1;
}

/* -------------------------------------------------------------------------- */

int set_getint(char *key, int def)
{
	char buf[32];

	if(set_getstr(key, itoa(def, buf, 10), sizeof(buf)))
		return atoi(buf);
	return def;
}

/* -------------------------------------------------------------------------- */

int set_setint(char *key, int value)
{
	char buf[32];
	return set_setstr(key, itoa(value, buf, 10));
}

/* -------------------------------------------------------------------------- */

unsigned int set_getip(char *key, unsigned int def)
{
	unsigned int ip;
	char buf[32];
	if(set_getstr(key, ip2text(buf, def), sizeof(buf)))
		if(text2ip(buf, &ip))
			return ip;
	return def;
}

/* -------------------------------------------------------------------------- */

int set_setip(char *key, unsigned int ip)
{
	char buf[32];
	return set_setstr(key, ip2text(buf, ip));
}

/* -------------------------------------------------------------------------- */

int set_getbool(char *key, int def)
{
	char buf[32] = "";
	if(set_getstr(key, buf, sizeof(buf)))
	{
		if(strcmpi(buf, "on") == 0)
			return 1;
		if(strcmpi(buf, "off") == 0)
			return 0;
	}
	return def;
}

/* -------------------------------------------------------------------------- */

int set_setbool(char *key, int value)
{
	return set_setstr(key, value?"on":"off");
}

/* -------------------------------------------------------------------------- */
