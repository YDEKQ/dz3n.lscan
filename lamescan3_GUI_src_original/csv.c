/* -------------------------------------------------------------------------- */

#include "csv.h"

/* -------------------------------------------------------------------------- */

csv_t *csv_open_read(char *filename, char *sep, int maxline, int maxcount)
{
	csv_t *csv;
	if((csv = malloc(sizeof(csv_t))))
	{
		if((csv->f = fopen(filename, "rt")))
		{
			csv->maxcount = maxcount;
			csv->maxline = maxline;
			csv->sep = sep;
			csv->mode = CSV_MODE_RD;
			return csv;
		}
		free(csv);
	}
	return NULL;
}

/* -------------------------------------------------------------------------- */

csv_t *csv_open_write(char *filename, char *sep, int maxline)
{
	csv_t *csv;
	if((csv = malloc(sizeof(csv_t))))
	{
		if((csv->f = fopen(filename, "wt")))
		{
			csv->maxcount = 0;
			csv->maxline = maxline;
			csv->sep = sep;
			csv->mode = CSV_MODE_WR;
			return csv;
		}
		free(csv);
	}
	return NULL;
}

/* -------------------------------------------------------------------------- */

void csv_close(csv_t *csv)
{
	if(csv)
	{
		if(csv->f)
			fclose(csv->f);
		free(csv);
	}
}

/* -------------------------------------------------------------------------- */

char *csv_pack(csv_t *csv, char *str)
{
	char *p, *q;
	char *out;
	int outlen;
	int have_spec_chars = 0;
	for(p = str; *p; ++p)
	{
		if( (strchr(csv->sep, *p)) ||
			(*p == '\"') )
		{
			have_spec_chars = 1;
			break;
		}
	}
	if(!have_spec_chars)
	{
		return strdup(str);
	}
	outlen = 3; //quotas+zero
	for(p = str; *p; ++p)
	{
		if(*p == '\"')
			outlen++;
		outlen++;
	}
	if((out = malloc(outlen)))
	{
		q = out;
		*(q++) = '\"';
		for(p = str; *p; ++p)
		{
			*(q++) = *p;
			if(*p == '\"')
				*(q++) = '\"';
		}
		*(q++) = '\"';
		*q = 0;
	}
	return out;
}

/* -------------------------------------------------------------------------- */

int csv_writeline(csv_t *csv, char **items, int count)
{
	char *buf, *q;
	char *temp;
	int i;
	if( (csv) && (csv->mode | CSV_MODE_WR) && (count) )
	{
		if((buf = malloc(csv->maxline)))
		{
			q = buf;
			for(i = 0; i < count; ++i)
			{
				if((temp = csv_pack(csv, items[i])))
				{
					strcpy(q, temp);
					q += (int)strlen(q);
					free(temp);
				}
				if(i != count - 1)
				{
					*(q++) = csv->sep[0];
					*q = 0;
				}
			}
			*(q++) = '\n';
			*q = 0;
			fputs(buf, csv->f);
			free(buf);
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

char *csv_unpack(csv_t *csv, char *str)
{
	char *out, *p, *q;
	int outlen, have_quotas = 0;
	for(p = str; *p; ++p)
	{
		if(*p == '\"')
		{
			have_quotas = 1;
			break;
		}
	}
	if(!have_quotas)
	{
		return strdup(str);
	}
	outlen = 1; //zero
	for(p = str; *p; ++p)
	{
		if(*p == '\"')
		{
			if(*(p+1) == '\"')
			{
				outlen++;
				p++;
			}
		}
		else
		{
			outlen++;
		}
	}
	if((out = malloc(outlen)))
	{
		q = out;
		for(p = str; *p; ++p)
		{
			if(*p == '\"')
			{
				if(*(p+1) == '\"')
				{
					*(q++) = '\"';
					p++;
				}
			}
			else
			{
				*(q++) = *p;
			}
		}
		*q = 0;
	}
	return out;
}

/* -------------------------------------------------------------------------- */

int csv_readline(csv_t *csv, char **items, int *count)
{
	int status = 0;
	char *buf;
	char *p, *q, *t;
	int quoted;
	*count = 0;
	if( (csv) && (csv->mode == CSV_MODE_RD) )
	{
		if((buf = malloc(csv->maxline)))
		{
			if(fgets(buf, csv->maxline, csv->f))
			{
				if((p = strpbrk(buf, "\r\n")))
					*p = 0;
				for(p = buf; p; p = q)
				{
					if(*p == '\"')
					{
						q = NULL;
						quoted = 0;
						for(t = p; *t; ++t)
						{
							if(*t == '\"')
							{
								if(*(t+1) == '\"')
								{
									t++;
								}
								else
								{
									quoted = !quoted;
								}
							}
							else if(strchr(csv->sep, *t))
							{
								if(!quoted)
								{
									q = t;
									break;
								}
							}
						}
						if(q)
						{
							*q = 0;
							q++;
						}
					}
					else
					{
						q = NULL;
						for(t = p; *t; ++t)
						{
							if(strchr(csv->sep, *t))
							{
								q = t;
								break;
							}
						}
						if(q)
						{
							*q = 0;
							q++;
						}
					}
					if(*count < csv->maxcount)
					{
						items[(*count)++] = csv_unpack(csv, p);
					}
				}
				status = 1;
			}
			free(buf);
		}
	}
	return status;
}

/* -------------------------------------------------------------------------- */
