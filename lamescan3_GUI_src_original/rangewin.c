/* -------------------------------------------------------------------------- */

#include "rangewin.h"

/* -------------------------------------------------------------------------- */

int parse_range(char *str, unsigned int *pb, unsigned int *pe)
{
	char *sep;
	int n, mask;
	unsigned int net;

	while( (*str == ' ') || (*str == '\t') )
		str++;
	if(*str == '#')
		return 0;

	if( (sep = strchr(str, '-')) ||
		(sep = strchr(str, ';')) ||
		(sep = strchr(str, '—')) ||
		(sep = strchr(str, '–')) )
	{
		*(sep++) = 0;
		while( (*str == ' ') || (*str == '\t') ) str++;
		while( (*sep == ' ') || (*sep == '\t') ) sep++;
		if(text2ip(str, pb) && (text2ip(sep, pe)))
			return 1;
	}

	else if( (sep = strchr(str, '/')) )
	{
		*(sep++) = 0;
		while( (*str == ' ') || (*str == '\t') ) str++;
		while( (*sep == ' ') || (*sep == '\t') ) sep++;
		if(text2ip(str, &net))
		{
			n = atoi(sep);
			if((n >= 0) && (n <= 32))
			{
				if(n == 32) 
				{
					mask = 0xffffffff;
				}
				else
				{
					mask = ~(0xffffffff >> n);
				}
				*pb = net & mask;
				*pe = *pb | ~mask;
				return 1;
			}
		}
	}

	else
	{
		while( (*str == ' ') || (*str == '\t') ) str++;
		if(text2ip(str, pb))
		{
			*pe = *pb;
			return 1;
		}
	}

	return 0;
}

/* -------------------------------------------------------------------------- */

char *build_range(char *buf, int begin, int end)
{
	char *p;
	int n;
	unsigned int rbeg, rend, temp;

	if(begin == end)
	{
		return ip2text(buf, begin);
	}

	temp = begin^end;
	for(n = 0; n < 32; ++n)
	{
		if(temp & 0x80000000)
			break;
		temp <<= 1;
	}
	temp = ~(0xffffffff >> n);
	rbeg = begin & temp;
	rend = begin | ~temp;
	if(rbeg == 0) rbeg = 1;
	if(rend == 0) rend = 1;

	p = buf;
	if( (rbeg == begin) && (rend == end) )
	{
		ip2text(p, rbeg);
		p += strlen(p);
		*(p++) = '/';
		itoa(n, p, 10);
	}
	else
	{
		ip2text(p, begin);
		p += strlen(p);
		*(p++) = '-';
		ip2text(p, end);
	}
	return buf;
}

/* -------------------------------------------------------------------------- */

INT_PTR CALLBACK RangewinProc(HWND hwin, UINT msg, WPARAM wp, LPARAM lp)
{
	rangewin_param *param;
	int ctrl;
	unsigned int beg, end, temp;
	char *buf, *p, *ptemp;
	int i, buflen;
	HWND ctlwin;
	LOGFONT lf;

	if(msg == WM_INITDIALOG)
	{
		param = (void*)lp;
		SetWindowLongPtr(hwin, DWL_USER, (LONG)param);
		SetWindowText(hwin, param->title);
		SendDlgItemMessage(hwin, IDC_RANGE_IMPORT, EM_EXLIMITTEXT, 0, 0x01000000);
		SendMessage(hwin, WM_SETICON, ICON_SMALL, (LPARAM)(param->icon));
		SendMessage(hwin, WM_SETICON, ICON_BIG, (LPARAM)(param->icon));
		memset(&lf, 0, sizeof(lf));
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -13;
		lf.lfWeight = FW_NORMAL;
		strcpy(lf.lfFaceName, "Courier New");
		if((param->font = CreateFontIndirect(&lf)))
		{
			SendDlgItemMessage(hwin, IDC_RANGE_IMPORT,
				WM_SETFONT, (WPARAM)(param->font), 0);
		}
		if((buflen = param->range_count * 40))
		{
			if((buf = malloc(buflen)))
			{
				p = buf;
				for(i = 0; i < param->range_count; ++i)
				{
					build_range(p, 
						param->range_data[i].begin, 
						param->range_data[i].end);
					p += strlen(p);
					*(p++) = '\r';
					*(p++) = '\n';
					*p = 0;
				}
				SetDlgItemText(hwin, IDC_RANGE_IMPORT, buf);
				free(buf);
			}
		}
	}

	else if(msg == WM_DESTROY)
	{
		if((param = (void*)GetWindowLongPtr(hwin, DWL_USER)))
			DeleteObject(param->font);
	}

	else if(msg == WM_CLOSE)
	{
		EndDialog(hwin, 0);
		return 1;
	}

	else if(msg == WM_COMMAND)
	{
		ctrl = LOWORD(wp);

		if(ctrl == IDOK)
		{
			param = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((ctlwin = GetDlgItem(hwin, IDC_RANGE_IMPORT)))
			{
				buflen = GetWindowTextLength(ctlwin);
				if((buf = malloc(buflen + 1)))
				{
					param->range_count = 0;
					GetWindowText(ctlwin, buf, buflen+1);
					if((p = strtok_s(buf, "\r\n", &ptemp)))
					{
						do {
							if(parse_range(p, &beg, &end))
							{
								if(beg > end)
								{
									temp = beg;
									beg = end;
									end = temp;
								}
								if(beg == 0) beg = 1;
								if(end == 0) end = 1;
								if(param->range_count == param->range_limit)
								{
									param->range_limit += 16;
									if(!(param->range_data = realloc(
										param->range_data,
										sizeof(rangewin_range_elem) * param->range_limit)))
									{
										param->range_limit = 0;
										param->range_count = 0;
									}
								}
								if(param->range_data)
								{
									param->range_data[param->range_count].begin = beg;
									param->range_data[param->range_count].end = end;
									param->range_count++;
								}
							}
						} while((p = strtok_s(NULL, "\r\n", &ptemp)));
					}
					free(buf);
				}
			}
			EndDialog(hwin, 1);
			return 1;
		}

		else if(ctrl == IDCANCEL)
		{
			EndDialog(hwin, 0);
			return 1;
		}
	}

	return 0;
}

/* -------------------------------------------------------------------------- */

int rangewin(rangewin_param *param)
{
	return DialogBoxParam(param->instance,
		MAKEINTRESOURCE(IDD_RANGEWIN),
		param->owner,
		RangewinProc,
		(LPARAM)param);
}

/* -------------------------------------------------------------------------- */
