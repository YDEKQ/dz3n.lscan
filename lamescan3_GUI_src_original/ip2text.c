/* -------------------------------------------------------------------------- */

#include "ip2text.h"

/* -------------------------------------------------------------------------- */

char *ip2text(char *buf, unsigned int ip)
{
	int i;
	char *p = buf;
	if(buf)
	{
		for(i = 0; i < 4; ++i)
		{
			itoa(ip >> 24, p, 10);
			p += strlen(p);
			if(i != 3)
			{
				*(p++) = '.';
			}
			ip <<= 8;
		}
	}
	return buf;
}

/* -------------------------------------------------------------------------- */

int text2ip(char *buf, unsigned int *ip)
{
	char *p = buf;
	int cnt = 0;
	unsigned int elem = 0, value = 0;;
	if(buf && ip)
	{
		while(p)
		{
			elem = atoi(p);
			if(elem < 0) break;
			if(elem > 255) break;
			value <<= 8;
			value |= elem;
			cnt++;
			if((p = strchr(p, '.')))
				p++;
		}
		if(cnt == 4)
		{
			*ip = value;
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */
