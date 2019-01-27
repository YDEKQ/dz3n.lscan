/* -------------------------------------------------------------------------- */

#include "mainwin.h"

/* -------------------------------------------------------------------------- */

char *software_title = "Lamescan";
char *software_version = "3 r6";
int update_check_interval = 5;

char *host_queue_group_title = "В очереди";
char *host_down_group_title = "Оффлайн";
char *host_up_group_title = "Просканирован";
char *host_warn_group_title = "Ошибка сканир.";
char *host_group_title = "Статус";
char *host_range_begin_title = "Начало";
char *host_range_end_title = "Конец";
char *host_range_size_title = "Размер";
char *add_hosts_title = "Добавляем диапазоны";
char *remove_hosts_title = "Убираем диапазоны";
char *scan_finish_text = "Сканирование завершено";
char *scan_finish_title = "Lamescan 3";
char *scan_error_title = "Lamescan 3";
char *scan_start_error = "Сканирование уже запущено или останавливается";
char *scan_stop_error = "Сканирование не запущено или останавливается";
char *scan_pause_error = "Сканирование не запущено или останавливается";
char *save_csv_range_title = "Сохраняем диапазоны";
char *load_csv_range_title = "Загружаем диапазоны";
char *save_results_range_title = "Сохраняем результаты";
char *load_results_range_title = "Загружаем результаты";
char *export_html_results_title = "Экспортируем результаты в HTML";
char *range_export_error_title = "Ошибка";
char *results_export_error_title = "Ошибка";
char *results_export_html_error_title = "Ошибка";
char *load_csv_range_error_title = "Ошибка";
char *results_import_error_title = "Ошибка";
char *range_export_error_text = "Не удаётся сохранить диапазоны";
char *results_export_error_text = "Не удаётся сохранить результаты";
char *results_export_html_error_text = "Не удаётся экспортировать результаты";
char *load_csv_range_error_text = "Не удаётся загрузить диапазоны";
char *results_import_error_text = "Не удаётся загрузить результаты";
char *csv_filter = "CSV-таблица (*.csv;*.txt)\0*.csv;*.txt\0Все файлы (*.*)\0*.*\0";
char *html_filter = "HTML-страничка (*.htm;*.html)\0*.htm;*.html\0Все файлы (*.*)\0*.*\0";
char *csv_ext = "txt";
char *html_ext = "htm";
char *notifyicon_tip = "Lamescan 3";
char *result_status_text = "Статус";
char *result_version_text = "Верс.";
char *result_host_text = "Хост";
char *result_port_text = "Порт";
char *result_ping_text = "Пинг";
char *result_mechanism_text = "Защ.";
char *result_login_text = "Логин";
char *result_password_text = "Пароль";
char *radmin_connect_error_text = "Не удаётся запустить клиент";
char *radmin_connect_error_title = "Ошибка запуска";
char *about_title = "О программе";
char *web_path = "http://redsh.tk/";
char *email = "mailto:californ251@gmail.com";
char *manual = "lamescan3.pdf";
char *manual_miss_text = "Не могу найти lamescan3.pdf!";
char *manual_miss_title = "Упс";

/* -------------------------------------------------------------------------- */

char *ending(int n, char *end1, char *end2, char *end5)
{
	int t;
	if(n < 0)
		n = -n;
	t = n % 10;
	if( (t == 0) ||
		((t >= 5) && (t <= 9)) ||
		((n >= 11) && (n <= 20)) )
	{
		return end5;
	}
	if( (t >= 2) && (t <= 4) )
	{
		return end2;
	}
	if(t == 1)
	{
		return end1;
	}
	return "";
}

/* -------------------------------------------------------------------------- */

BOOL CALLBACK radmin_passwd_enumdialog(HWND hwin, LPARAM lp)
{
	char cls[64];
	DWORD style;
	radmin_passwd_params *params = (void*)lp;
	if(GetClassName(hwin, cls, sizeof(cls)))
	{
		if(strcmpi(cls, "edit") == 0)
		{
			if(params->numedits < params->maxedits)
			{
				style = GetWindowLong(hwin, GWL_STYLE);
				if(!(style & (ES_READONLY|ES_MULTILINE)))
					params->edits[params->numedits++] = hwin;
			}
		}
		else if(strcmpi(cls, "button") == 0)
		{
			if(params->numbuttons < params->maxbuttons)
			{
				style = GetWindowLong(hwin, GWL_STYLE);
				if( ((style & 0x0f) == BS_PUSHBUTTON) ||
					((style & 0x0f) == BS_DEFPUSHBUTTON) )
				{
					params->buttons[params->numbuttons++] = hwin;
				}
			}
		}
	}
	return 1;
}

/* -------------------------------------------------------------------------- */

BOOL CALLBACK radmin_passwd_enumwnd(HWND hwin, LPARAM lp)
{
	DWORD pid;
	radmin_passwd_params *params = (void*)lp;

	GetWindowThreadProcessId(hwin, &pid);

	if(pid == params->processId)
	{
		params->numedits = 0;
		params->maxedits = 4;
		params->numbuttons = 0;
		params->maxbuttons = 4;
		
		EnumChildWindows(hwin, 
			radmin_passwd_enumdialog,
			(LPARAM)params);

		if( (params->numedits == 1) &&
			(params->numbuttons == 2) )
		{
			SendMessage(params->edits[0], WM_SETTEXT, 0, (LPARAM)(params->password));
			SendMessage(hwin, WM_COMMAND, GetDlgCtrlID(params->buttons[0]), (LPARAM)(params->buttons[0]));
			params->success = 1;
		}

		else if( (params->numedits == 2) &&
			(params->numbuttons == 2) )
		{
			SendMessage(params->edits[0], WM_SETTEXT, 0, (LPARAM)(params->login));
			SendMessage(params->edits[1], WM_SETTEXT, 0, (LPARAM)(params->password));
			SendMessage(hwin, WM_COMMAND, GetDlgCtrlID(params->buttons[0]), (LPARAM)(params->buttons[0]));
			params->success = 1;
		}
	}
	
	return 1;
}

/* -------------------------------------------------------------------------- */

DWORD WINAPI radmin_passwd_thread(radmin_passwd_params *params)
{
	int i;
	for(i = 0; i < 100; ++i)
	{
		EnumWindows(radmin_passwd_enumwnd, (LPARAM)params);
		if(params->success)
			break;
		Sleep(100);
	}
	free(params->login);
	free(params->password);
	free(params);
	return 0;
}

/* -------------------------------------------------------------------------- */

int radmin_enter_password(DWORD processId, char *username, char *password)
{
	HANDLE thread;
	radmin_passwd_params *params;
	if((params = malloc(sizeof(radmin_passwd_params))))
	{
		params->success = 0;
		params->login = strdup(username);
		params->password = strdup(password);
		if((params->login) && (params->password))
		{
			params->processId = processId;
			if((thread = CreateThread(NULL, 0, radmin_passwd_thread,
				params, 0, NULL)))
			{
				return params->success;
			}
		}
		free(params->login);
		free(params->password);
		free(params);
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int spawn_and_connect_radmin(char *radmin_path, int enable_window_capture,
						 char *host, char *port, char *login, char *password,
						 int mode)
{
	char commandline[500];
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	strcpy(commandline, "\"");
	strcat(commandline, radmin_path);
	strcat(commandline, "\"");

	strcat(commandline, " /connect:");
	strcat(commandline, host);
	if(atoi(port) != 4899)
	{
		strcat(commandline, ":");
		strcat(commandline, port);
	}

	if(mode == 1)
		strcat(commandline, " /noinput");
	else if(mode == 2)
		strcat(commandline, " /shutdown");
	else if(mode == 3)
		strcat(commandline, " /file");
	else if(mode == 4)
		strcat(commandline, " /telnet");

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	if(CreateProcess(NULL, commandline, NULL, 
		NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		if(enable_window_capture)
		{
			radmin_enter_password(pi.dwProcessId,
				login,
				password);
		}
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_control_callback(range_callback_hdr *hdr, range_elem_t *elem, int mode)
{
	int index;
	LVITEM item;
	LVFINDINFO find;
	char buf[32];

	if(WaitForSingleObject(hdr->mutex, INFINITE) == WAIT_OBJECT_0)
	{
		if(mode == RANGE_ADD)
		{
			item.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
			item.iItem = 0x7fffffff;
			item.iSubItem = 0;
			item.pszText = hdr->group_title;
			item.iImage = hdr->image_index;
			item.lParam = (LPARAM)elem;
			if((index = ListView_InsertItem(hdr->hwin_control, &item)) != -1)
			{
				ListView_SetItemText(hdr->hwin_control, index, 1, ip2text(buf, elem->begin));
				ListView_SetItemText(hdr->hwin_control, index, 2, ip2text(buf, elem->end));
				ListView_SetItemText(hdr->hwin_control, index, 3, ultoa(elem->end - elem->begin + 1, buf, 10));
			}
		}

		else if(mode == RANGE_REMOVE)
		{
			find.flags = LVFI_PARAM;
			find.lParam = (LPARAM)elem;
			if((index = ListView_FindItem(hdr->hwin_control, -1, &find)) != -1)
			{
				ListView_DeleteItem(hdr->hwin_control, index);
			}
		}

		else if(mode == RANGE_SET)
		{
			find.flags = LVFI_PARAM;
			find.lParam = (LPARAM)elem;
			if((index = ListView_FindItem(hdr->hwin_control, -1, &find)) != -1)
			{
				ListView_SetItemText(hdr->hwin_control, index, 1, ip2text(buf, elem->begin));
				ListView_SetItemText(hdr->hwin_control, index, 2, ip2text(buf, elem->end));
				ListView_SetItemText(hdr->hwin_control, index, 3, ultoa(elem->end - elem->begin + 1, buf, 10));
			}
		}
		
		ReleaseMutex(hdr->mutex);
	}

	return 1;
}

/* -------------------------------------------------------------------------- */

int scan_status_callback(status_callback_hdr *hdr, int ident, int remove, scanned_host_status *status)
{
	LVFINDINFO find;
	LVITEM item;
	int index = -1;
	char buf[32];

	if(WaitForSingleObject(hdr->mutex, INFINITE) == WAIT_OBJECT_0)
	{
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
		item.iItem = 0;
		item.iSubItem = 0;
		item.pszText = status->status;
		item.iImage = status->success ? hdr->radmin_image : hdr->radmin_warn_image;
		if(ident != -1)
		{
			find.flags = LVFI_PARAM;
			find.lParam = ident;
			if((index = ListView_FindItem(hdr->hwin_control, -1, &find)) != -1)
				item.iItem = index;
		}
		if(remove)
		{
			if(index != -1)
			{
				ListView_DeleteItem(hdr->hwin_control, index);
				ident = -1;
			}
		}
		else
		{
			if(index != -1)
			{
				item.lParam = ident;
				ListView_SetItem(hdr->hwin_control, &item);
			}
			else
			{
				ident = ++hdr->lastid;
				item.lParam = ident;
				index = ListView_InsertItem(hdr->hwin_control, &item);
			}
			if(index != -1)
			{
				ListView_SetItemText(hdr->hwin_control, index, 1, ip2text(buf, status->host));
				ListView_SetItemText(hdr->hwin_control, index, 2, itoa(status->port, buf, 10));
				ListView_SetItemText(hdr->hwin_control, index, 3, status->ping);
				ListView_SetItemText(hdr->hwin_control, index, 4, status->client);
				ListView_SetItemText(hdr->hwin_control, index, 5, status->mechanism);
				ListView_SetItemText(hdr->hwin_control, index, 6, status->login);
				ListView_SetItemText(hdr->hwin_control, index, 7, status->password);
			}
		}
		ReleaseMutex(hdr->mutex);
	}

	return ident;
}

/* -------------------------------------------------------------------------- */

int scan_finish_callback(HWND hwin)
{
	MessageBox(hwin, 
		scan_finish_text, 
		scan_finish_title, 
		MB_ICONINFORMATION|MB_OK);
	return 1;
}

/* -------------------------------------------------------------------------- */

int scan_finish_exit_callback(HWND hwin)
{
	PostMessage(hwin, WM_CLOSE, 0, 0);
	return 1;
}

/* -------------------------------------------------------------------------- */

int scan_progress_callback(HWND hwin, int position, int threadcount, int started, int paused)
{
	char buf[64], buf2[64];
	int dperc;
	itoa(position/10, buf, 10);
	if((dperc = position%10))
	{
		itoa(dperc, buf2, 10);
		strcat(buf, ".");
		strcat(buf, buf2);
	}
	strcat(buf, "%");
	SendDlgItemMessage(hwin, IDC_SCAN_PROGRESS, PBM_SETPOS, position, 0);
	SetDlgItemText(hwin, IDC_SCAN_PROGRESS_PERCENT, buf);
	if(threadcount == 0)
	{
		SetDlgItemText(hwin, IDC_THREADCOUNT, "");
	}
	else
	{
		wsprintf(buf, "%s (%d поток%s)...", 
			started ? "Запущен" : "Останов", 
			threadcount,
			ending(threadcount, "", "а", "ов"));
		if(paused) strcat(buf, " (пауза)");
		SetDlgItemText(hwin, IDC_THREADCOUNT, buf);
	}
	return 1;
}

/* -------------------------------------------------------------------------- */

int result_progress_callback(HWND hwin, int total, int count)
{
	char buff[64];
	int perc = 100;
	if(total) {
		perc = (count * 100 + (total / 2)) / total;
	}
	wsprintf(buff, "Успешно - %d хост%s из %d (%d%%)", 
		count, ending(count, "", "а", "ов"), total, perc);
	SetDlgItemText(hwin, IDC_RESULT_PROGRESS, buff);
	return 1;
}

/* -------------------------------------------------------------------------- */

int scan_msg_callback(HWND owner, char *title, char *text)
{
	MessageBox(owner, text, title, MB_ICONEXCLAMATION|MB_OK);
	return 1;
}

/* -------------------------------------------------------------------------- */

int range_export(char *filename, range_t *que, range_t *up, 
				 range_t *down, range_t *err)
{
	range_t *range[4] = {que, up, down, err};
	char *title[4] = {NULL, "on", "off", "error"};
	int i, cnt;
	csv_t *csv;
	char *text[3];
	char start[32], stop[32];
	range_elem_t *elem;
	if((csv = csv_open_write(filename, ";", 4096)))
	{
		for(i = 0; i < 4; ++i)
		{
			if(range_lock(range[i]))
			{
				for(elem = range[i]->items; elem; elem = elem->flink)
				{
					cnt = 0;
					ip2text(start, elem->begin);
					text[cnt++] = start;
					if(elem->begin != elem->end)
					{
						ip2text(stop, elem->end);
						text[cnt++] = stop;
					}
					if(title[i] != NULL)
					{
						text[cnt++] = title[i];
					}
					csv_writeline(csv, text, cnt);
				}
				range_unlock(range[i]);
			}
		}
		csv_close(csv);
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_import(char *filename, range_t *que, range_t *up, 
				 range_t *down, range_t *err)
{
	csv_t *csv;
	char *text[3];
	int i, cnt;
	unsigned int start, stop;
	if((csv = csv_open_read(filename, ";,-–—", 4096, 3)))
	{
		while(csv_readline(csv, text, &cnt))
		{
			if(cnt == 1)
			{
				if(text2ip(text[0], &start))
				{
					range_add_single(que, start);
				}
			}
			else if(cnt == 2)
			{
				if(text2ip(text[0], &start))
				{
					if(strcmpi(text[1], "que") == 0)
						range_add_single(que, start);
					else if(strcmpi(text[1], "on") == 0)
						range_add_single(up, start);
					else if(strcmpi(text[1], "off") == 0)
						range_add_single(down, start);
					else if(strcmpi(text[1], "error") == 0)
						range_add_single(err, start);
					else if(text2ip(text[1], &stop))
						range_add(que, start, stop);
				}
			}
			else if(cnt == 3)
			{
				if(text2ip(text[0], &start) &&
					text2ip(text[1], &stop))
				{
					if(strcmpi(text[2], "que") == 0)
						range_add(que, start, stop);
					else if(strcmpi(text[2], "on") == 0)
						range_add(up, start, stop);
					else if(strcmpi(text[2], "off") == 0)
						range_add(down, start, stop);
					else if(strcmpi(text[2], "error") == 0)
						range_add(err, start, stop);
				}
			}
			for(i = 0; i < cnt; ++i)
				free(text[i]);
		}
		csv_close(csv);
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int results_export_csv(char *filename, HWND hwin_control)
{
	csv_t *csv;
	int count, i, j, cnt;
	char *buff[8], *items[8];
	if((csv = csv_open_write(filename, ";", 4096)))
	{
		for(i = 0; i < 8; ++i)
			buff[i] = malloc(256);
		count = ListView_GetItemCount(hwin_control);
		for(i = 0; i < count; ++i)
		{
			cnt = 0;
			for(j = 0; j < 8; ++j)
			{
				strcpy(buff[j], "");
				ListView_GetItemText(hwin_control, i, j, buff[j], 256);
				items[cnt++] = buff[j];
			}
			csv_writeline(csv, items, cnt);
		}
		for(i = 0; i < 8; ++i)
			free(buff[i]);
		csv_close(csv);
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int results_import_csv(char *filename, HWND hwin_control, int image)
{
	csv_t *csv;
	char *items[8];
	int i, j, cnt, index;
	LVITEM item;
	if((csv = csv_open_read(filename, ";,", 4096, 8)))
	{
		while(csv_readline(csv, items, &cnt))
		{
			if(cnt)
			{
				item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
				item.iItem = 0x7fffffff;
				item.iSubItem = 0;
				item.pszText = items[0];
				item.lParam = 0xffffffff;
				item.iImage = image;
				if((index = ListView_InsertItem(hwin_control, &item)) != -1)
				{
					for(j = 1; j < cnt; ++j)
					{
						ListView_SetItemText(hwin_control, index, j, items[j]);
					}
				}
				for(i = 0; i < cnt; ++i)
					free(items[i]);
			}
		}
		csv_close(csv);
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

void html_enc(char *dst, char *src, int maxchars)
{
	char *p, *q;
	q = dst;
	for(p = src; *p; ++p)
	{
		if(*p == '\"')
		{
			*(q++) = '&';
			*(q++) = 'q';
			*(q++) = 'u';
			*(q++) = 'o';
			*(q++) = 't';
			*(q++) = ';';
		}
		else if(*p == '&')
		{
			*(q++) = '&';
			*(q++) = 'a';
			*(q++) = 'm';
			*(q++) = 'p';
			*(q++) = ';';
		}
		else if(*p == '<')
		{
			*(q++) = '&';
			*(q++) = 'l';
			*(q++) = 't';
			*(q++) = ';';
		}
		else if(*p == '>')
		{
			*(q++) = '&';
			*(q++) = 'g';
			*(q++) = 't';
			*(q++) = ';';
		}
		else
		{
			if(maxchars >= 1)
			{
				*(q++) = *p;
				maxchars--;
			}
		}
	}
	*q = 0;
}

/* -------------------------------------------------------------------------- */

int results_export_html(char *filename, HWND hwin_control)
{
	FILE *f;
	int i, j, count;
	char buff[256];
	char buff_enc[384];
	if((f = fopen(filename, "wt")))
	{
		fputs("<!DOCTYPE HTML PUBLIC "
			"\"-//W3C//DTD HTML 4.01 Transitional//EN\" "
			"\"http://www.w3.org/TR/html4/loose.dtd\">\n\n", f);
		fputs("<html>\n", f);
		fputs("\t<head>\n", f);
		fputs("\t\t<title>Список результатов</title>\n", f);
		fputs("\t\t<meta http-equiv=\"Content-Type\" "
			"content=\"text/html; charset=windows-1251\">\n", f);
		fputs("\t</head>\n", f);
		fputs("\t<body>\n", f);
		if((count = ListView_GetItemCount(hwin_control)))
		{
			fputs("\t\t<table border=\"1\">\n", f);
			for(i = 0; i < count; ++i)
			{
				fputs("\t\t\t<tr>\n", f);
				for(j = 0; j < 8; ++j)
				{
					strcpy(buff, "");
					ListView_GetItemText(hwin_control, i, j, buff, sizeof(buff));
					html_enc(buff_enc, buff, sizeof(buff_enc));
					fputs("\t\t\t\t<td>", f);
					fputs(buff_enc, f);
					fputs("</td>\n", f);
				}
				fputs("\t\t\t</tr>\n", f);
			}
			fputs("\t\t</table>\n", f);
		}
		fputs("\t</body>\n", f);
		fputs("</html>\n", f);
		fclose(f);
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int results_connect(HWND hwin_control, 
					char *radmin_path, 
					int enable_window_capture,
					int mode)
{
	int i, count, status = 1;
	char host[64], port[64], login[256], password[256];
	if((count = ListView_GetItemCount(hwin_control)))
	{
		for(i = 0; i < count; ++i)
		{
			if(ListView_GetItemState(hwin_control, i, 
				LVIS_SELECTED) == LVIS_SELECTED)
			{
				strcpy(host, "");
				ListView_GetItemText(hwin_control, i, 1, host, sizeof(host));
				strcpy(port, "");
				ListView_GetItemText(hwin_control, i, 2, port, sizeof(port));
				strcpy(login, "");
				ListView_GetItemText(hwin_control, i, 6, login, sizeof(login));
				strcpy(password, "");
				ListView_GetItemText(hwin_control, i, 7, password, sizeof(password));
				if(!spawn_and_connect_radmin(radmin_path,
					enable_window_capture,
					host, port,
					login, password,
					mode))
				{
					status = 0;
				}
			}
		}
	}
	return status;
}

/* -------------------------------------------------------------------------- */

void put_to_clipboard(char *buff)
{
	HANDLE mem, temp;
	int bufflen;
	if(OpenClipboard(NULL))
	{
		EmptyClipboard();
		bufflen = strlen(buff) + 1;
		if(mem = GlobalAlloc(GMEM_MOVEABLE, bufflen))
		{
			if(temp = GlobalLock(mem))
			{
				memcpy(temp, buff, bufflen);
				GlobalUnlock(temp);

				SetClipboardData(CF_TEXT, mem);
			}
		}
		CloseClipboard();
	}
}

/* -------------------------------------------------------------------------- */

int results_copy_to_clipboard(HWND hwin_control, int mask)
{
	int i, j, count, test;
	char *line;
	int linelen;
	int outlim = 0, outlen = 0;
	char *output = NULL;
	char buff[256];

	if((count = ListView_GetItemCount(hwin_control)))
	{
		if((line = malloc(4096)))
		{
			for(i = 0; i < count; ++i)
			{
				if(ListView_GetItemState(hwin_control, i, 
					LVIS_SELECTED) == LVIS_SELECTED)
				{
					strcpy(line, "");
					test = 0;
					for(j = 0; j < 8; ++j)
					{
						if(mask & (1<<j))
						{
							strcpy(buff, "");
							ListView_GetItemText(hwin_control, i, j, 
								buff, sizeof(buff));
							if(test) 
								strcat(line, "\t");
							test = 1;
							strcat(line, buff);
						}
					}
					strcat(line, "\r\n");
					if((linelen = (int)strlen(line)))
					{
						if(outlim - outlen < linelen)
						{
							while(outlim - outlen < linelen)
								outlim += 1024;
							if(!(output = realloc(output, outlim)))
							{
								outlen = 0;
								outlim = 0;
							}
						}
						if(output)
						{
							strcpy(output+outlen, line);
							outlen += linelen;
						}
					}
				}
			}
			if(output && (outlen > 0) )
			{
				put_to_clipboard(output);
			}
			free(output);
			free(line);
		}
	}
	return 1;
}

/* -------------------------------------------------------------------------- */

DWORD WINAPI edit_range_thread(edit_range_params *params)
{
	if(params->op == RANGE_OP_ADD)
		range_add(params->range, params->begin, params->end);
	else if(params->op == RANGE_OP_REM)
		range_remove(params->range, params->begin, params->end);
	else if(params->op == RANGE_OP_MERGE)
		range_merge(params->range, params->range2);
	else if(params->op == RANGE_OP_CLEAR)
		range_clear(params->range);
	free(params);
	return 0;
}

/* -------------------------------------------------------------------------- */

int edit_range(int op, range_t *range, range_t *range2, unsigned int begin, unsigned int end)
{
	HANDLE thread;
	edit_range_params *erp;
	if((erp = malloc(sizeof(edit_range_params))))
	{
		erp->op = op;
		erp->range = range;
		erp->range2 = range2;
		erp->begin = begin;
		erp->end = end;
		if((thread = CreateThread(NULL, 0, edit_range_thread, erp, 0, NULL)))
		{
			CloseHandle(thread);
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int CALLBACK lvsort_proc(LPARAM lp1, LPARAM lp2, lvsort_params *params)
{
	LVFINDINFO find;
	char buf1[256], buf2[256];
	int n1, n2;
	int index1, index2;
	if(lp1 == 0xffffffff)
	{
		if(lp2 == 0xffffffff)
			return 0;
		return -1;
	}
	if(lp2 == 0xffffffff)
		return 1;
	find.flags = LVFI_PARAM;
	find.lParam = lp1;
	index1 = ListView_FindItem(params->hwincontrol, -1, &find);
	find.lParam = lp2;
	index2 = ListView_FindItem(params->hwincontrol, -1, &find);
	if( (index1 != -1) && (index2 != -1) )
	{
		ListView_GetItemText(params->hwincontrol, 
			index1, params->subitem, buf1, sizeof(buf1));
		ListView_GetItemText(params->hwincontrol, 
			index2, params->subitem, buf2, sizeof(buf2));
		n1 = atoi(buf1);
		n2 = atoi(buf2);
		if(n1 > n2)
			return 1;
		if(n1 < n2)
			return -1;
		return lstrcmpi(buf1, buf2);
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

INT_PTR CALLBACK MainwinProc(HWND hwin, UINT msg, WPARAM wp, LPARAM lp)
{
	mainwin_params *params;
	LVCOLUMN column;
	int count, i, mode, ctl, status;
	rangewin_param rangewp;
	proxydlg_params proxydp;
	customizedlg_params customizedp;
	scannerdlg_params scannerdp;
	OPENFILENAME ofn;
	char *filename;
	HICON icon;
	HMENU menu;
	NMHDR *nmhdr;
	POINT point;
	char buf[128];
	lvsort_params lvsortp;
	NMLISTVIEW *nmlv;
	MSGBOXPARAMS mbp;

	if(msg == WM_INITDIALOG)
	{
		params = (void*)lp;
		SetWindowLongPtr(hwin, DWL_USER, lp);

		status = 1;

		// Load resources
		params->icon = LoadIcon(params->instance, MAKEINTRESOURCE(IDI_MAIN));
		params->icon_host_down = LoadIcon(params->instance, MAKEINTRESOURCE(IDI_HOST_DOWN));
		params->icon_host_up = LoadIcon(params->instance, MAKEINTRESOURCE(IDI_HOST_UP));
		params->icon_host_warn = LoadIcon(params->instance, MAKEINTRESOURCE(IDI_HOST_WARN));
		params->icon_host_queue = LoadIcon(params->instance, MAKEINTRESOURCE(IDI_HOST_QUEUE));
		params->icon_radmin = LoadIcon(params->instance, MAKEINTRESOURCE(IDI_RADMIN));
		params->icon_radmin_warn = LoadIcon(params->instance, MAKEINTRESOURCE(IDI_RADMIN_WARN));
		if( (!params->icon) || (!params->icon_host_down) ||
			(!params->icon_host_up) || (!params->icon_host_warn) ||
			(!params->icon_host_queue) || (!params->icon_radmin) ||
			(!params->icon_radmin_warn) )
		{
			status = 0;
		}

		// настраиваем окно
		SendMessage(hwin, WM_SETICON, ICON_SMALL, (LPARAM)(params->icon));
		SendMessage(hwin, WM_SETICON, ICON_BIG, (LPARAM)(params->icon));
		wsprintf(buf, "%s %s", software_title, software_version);
		SetWindowText(hwin, buf);

		// настраиваем кнопочки
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_ADD), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_ADD_RANGE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_REMOVE), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_REMOVE_RANGE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_CLEAR), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_REMOVE_ALL, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		SendDlgItemMessage(hwin, IDC_CLEAR_RESULTS, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_REQUE), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_ADD_DOWN_WARN, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_START), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_SCAN_START, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_PAUSE), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_SCAN_PAUSE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_STOP), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_SCAN_STOP, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_LOAD), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_LOAD_RANGE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		SendDlgItemMessage(hwin, IDC_LOAD_RESULTS, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_SAVE), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_SAVE_RANGE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		SendDlgItemMessage(hwin, IDC_SAVE_RESULTS, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
		icon = LoadImage(params->instance, MAKEINTRESOURCE(IDI_ICON_HTML), IMAGE_ICON, 16, 16, 0);
		SendDlgItemMessage(hwin, IDC_EXPORT_HTML_RESULTS, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);


		// создаём список картинок для лист контролов
		params->imagelist = ImageList_Create(16, 16, ILC_COLOR4|ILC_MASK, 6, 0);
		params->image_host_down = ImageList_AddIcon(params->imagelist, params->icon_host_down);
		params->image_host_up = ImageList_AddIcon(params->imagelist, params->icon_host_up);
		params->image_host_warn = ImageList_AddIcon(params->imagelist, params->icon_host_warn);
		params->image_host_queue = ImageList_AddIcon(params->imagelist, params->icon_host_queue);
		params->image_radmin = ImageList_AddIcon(params->imagelist, params->icon_radmin);
		params->image_radmin_warn = ImageList_AddIcon(params->imagelist, params->icon_radmin_warn);
		if( (!params->imagelist) || (params->image_host_down == -1) ||
			(params->image_host_up == -1) || (params->image_host_warn == -1) ||
			(params->image_host_queue == -1) || (params->image_radmin == -1) ||
			(params->image_radmin_warn == -1) )
		{
			status = 0;
		}

		// настраиваем лист контрол диапазонов
		params->ranges = GetDlgItem(hwin, IDC_RANGES);
		ListView_SetExtendedListViewStyleEx(params->ranges,
			LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES,
			LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
		ListView_SetImageList(params->ranges, params->imagelist, LVSIL_SMALL);
		column.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		column.cx = 150;
		column.iSubItem = 0;
		column.pszText = host_group_title;
		ListView_InsertColumn(params->ranges, column.iSubItem, &column);
		column.cx = 120;
		column.iSubItem++;
		column.pszText = host_range_begin_title;
		ListView_InsertColumn(params->ranges, column.iSubItem, &column);
		column.iSubItem++;
		column.pszText = host_range_end_title;
		ListView_InsertColumn(params->ranges, column.iSubItem, &column);
		column.iSubItem++;
		column.pszText = host_range_size_title;
		ListView_InsertColumn(params->ranges, column.iSubItem, &column);

		// настраиваем лист контрол результов
		params->results = GetDlgItem(hwin, IDC_RESULTS);
		ListView_SetExtendedListViewStyleEx(params->results,
			LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES,
			LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
		ListView_SetImageList(params->results, params->imagelist, LVSIL_SMALL);
		column.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		column.cx = 100;
		column.iSubItem = 0;
		column.pszText = result_status_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);
		column.iSubItem++;
		column.cx = 100;
		column.pszText = result_host_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);
		column.iSubItem++;
		column.cx = 40;
		column.pszText = result_port_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);
		column.iSubItem++;
		column.pszText = result_ping_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);
		column.iSubItem++;
		column.cx = 40;
		column.pszText = result_version_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);
		column.iSubItem++;
		column.cx = 40;
		column.pszText = result_mechanism_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);
		column.iSubItem++;
		column.cx = 75;
		column.pszText = result_login_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);
		column.cx = 75;
		column.iSubItem++;
		column.pszText = result_password_text;
		ListView_InsertColumn(params->results, column.iSubItem, &column);

		// настриваем прогресс бар
		SendDlgItemMessage(hwin, IDC_SCAN_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));

		// загружаем контекстное меню
		menu = LoadMenu(params->instance, MAKEINTRESOURCE(IDR_RESULTS_CONTEXT_MENU));
		params->results_context = GetSubMenu(menu, 0);

		// инициализируем диапазоны адресов
		params->range_control_mutex = CreateMutex(NULL, FALSE, NULL);
		params->range_queue_hdr.range = range_new(range_control_callback, &(params->range_queue_hdr));
		params->range_queue_hdr.hwin_control = params->ranges;
		params->range_queue_hdr.image_index = params->image_host_queue;
		params->range_queue_hdr.group_title = host_queue_group_title;
		params->range_queue_hdr.mutex = params->range_control_mutex;
		params->range_down_hdr.range = range_new(range_control_callback, &(params->range_down_hdr));
		params->range_down_hdr.hwin_control = params->ranges;
		params->range_down_hdr.image_index = params->image_host_down;
		params->range_down_hdr.group_title = host_down_group_title;
		params->range_down_hdr.mutex = params->range_control_mutex;
		params->range_up_hdr.range = range_new(range_control_callback, &(params->range_up_hdr));
		params->range_up_hdr.hwin_control = params->ranges;
		params->range_up_hdr.image_index = params->image_host_up;
		params->range_up_hdr.group_title = host_up_group_title;
		params->range_up_hdr.mutex = params->range_control_mutex;
		params->range_warn_hdr.range = range_new(range_control_callback, &(params->range_warn_hdr));
		params->range_warn_hdr.hwin_control = params->ranges;
		params->range_warn_hdr.image_index = params->image_host_warn;
		params->range_warn_hdr.group_title = host_warn_group_title;
		params->range_warn_hdr.mutex = params->range_control_mutex;
		if((!params->range_queue_hdr.range) || (!params->range_down_hdr.range) ||
			(!params->range_up_hdr.range) || (!params->range_warn_hdr.range) ||
			(!params->range_control_mutex) )
		{
			status = 0;
		}

		// задаём дефолтовые настроечки
		params->add_range_init_data = NULL;
		params->add_range_init_limit = 0;
		params->add_range_init_count = 0;
		params->scanprops.ping_enable = 1;
		params->scanprops.ping_packet_count = 2;
		params->scanprops.ping_timeout = 1000;
		params->scanprops.ping_max_sessions = 80;
		params->scanprops.delay_time = 100;
		params->scanprops.max_thread_count = 200;
		params->scanprops.tcp_timeout = 45000;
		params->scanprops.max_errors = 15;
		params->scanprops.error_delay = 20;
		params->scanprops.random_scan_order = 0;
		params->scanprops.max_dict1_path = MAX_PATH;
		params->scanprops.max_dict2_path = MAX_PATH;
		params->scanprops.dict1_path = malloc(params->scanprops.max_dict1_path);
		params->scanprops.dict2_path = malloc(params->scanprops.max_dict2_path);
		params->scanprops.max_port_list = 1024;
		params->scanprops.port_list = malloc(params->scanprops.max_port_list);
		params->scanprops.proxy_type = 0;
		params->scanprops.proxy_host = tcp_addr("127.0.0.1");
		params->scanprops.proxy_port = 2323;
		params->scanprops.max_proxy_username = 256;
		params->scanprops.proxy_username = malloc(params->scanprops.max_proxy_username);
		params->scanprops.max_proxy_password = 256;
		params->scanprops.proxy_password = malloc(params->scanprops.max_proxy_password);
		params->scanprops.auto_remove_bad = 1;
		params->enable_window_capture = 1;
		params->max_radmin_path = MAX_PATH;
		params->radmin_path = malloc(params->max_radmin_path);
		params->minimize_to_tray = 0;
		params->scanprops.enable_sounds = 1;
		params->auto_update = 1;
		params->skip_update_times = update_check_interval;

		if( (params->scanprops.proxy_username) && 
			(params->scanprops.proxy_password) &&
			(params->scanprops.dict1_path) &&
			(params->scanprops.dict2_path) &&
			(params->radmin_path) &&
			(params->scanprops.port_list) )
		{
			strcpy(params->scanprops.proxy_username, "");
			strcpy(params->scanprops.proxy_password, "");
			strcpy(params->scanprops.dict1_path, "dic\\loginlist.txt");
			strcpy(params->scanprops.dict2_path, "dic\\passlist.txt");
			strcpy(params->radmin_path, "Radmin.exe");
			strcpy(params->scanprops.port_list, "4899");
		}
		else
		{
			status = 0;
		}

		// загружаем настроечки
		params->scanprops.ping_enable = set_getbool("ping_enable", params->scanprops.ping_enable);
		params->scanprops.ping_packet_count = set_getint("ping_packet_count", params->scanprops.ping_packet_count);
		params->scanprops.ping_timeout = set_getint("ping_timeout", params->scanprops.ping_timeout);
		params->scanprops.ping_max_sessions = set_getint("ping_max_sessions", params->scanprops.ping_max_sessions);
		params->scanprops.delay_time = set_getint("scan_delay_time", params->scanprops.delay_time);
		params->scanprops.max_thread_count = set_getint("max_thread_count", params->scanprops.max_thread_count);
		params->scanprops.tcp_timeout = set_getint("tcp_timeout", params->scanprops.tcp_timeout);
		params->scanprops.max_errors = set_getint("max_errors", params->scanprops.max_errors);
		params->scanprops.error_delay = set_getint("error_delay", params->scanprops.error_delay);
		params->scanprops.random_scan_order = set_getbool("random_scan_order", params->scanprops.random_scan_order);
		set_getstr("port_list", params->scanprops.port_list, params->scanprops.max_port_list);
		set_getstr("login_dict", params->scanprops.dict1_path, params->scanprops.max_dict1_path);
		set_getstr("pass_dict", params->scanprops.dict2_path, params->scanprops.max_dict2_path);
		params->scanprops.proxy_type = set_getint("proxy_type", params->scanprops.proxy_type);
		params->scanprops.proxy_host = set_getip("proxy_host", params->scanprops.proxy_host);
		params->scanprops.proxy_port = set_getint("proxy_port", params->scanprops.proxy_port);
		set_getstr("proxy_username", params->scanprops.proxy_username, params->scanprops.max_proxy_username);
		set_getstr("proxy_password", params->scanprops.proxy_password, params->scanprops.max_proxy_password);
		params->scanprops.auto_remove_bad = set_getbool("auto_remove_bad", params->scanprops.auto_remove_bad);
		params->enable_window_capture = set_getbool("enable_window_capture", params->enable_window_capture);
		set_getstr("radmin_path", params->radmin_path, params->max_radmin_path);
		params->minimize_to_tray = set_getbool("minimize_to_tray", params->minimize_to_tray);
		params->scanprops.enable_sounds = set_getbool("enable_sounds", params->scanprops.enable_sounds);
		params->auto_update = set_getbool("auto_update", params->auto_update);
		params->skip_update_times = set_getint("skip_update_checks", params->skip_update_times);
		if(params->skip_update_times < 0) params->skip_update_times = 0;
		if(params->skip_update_times > update_check_interval) params->skip_update_times = update_check_interval;

		// инфа для коллбэка вывода результатов
		params->status_hdr.hwin_control = params->results;
		params->status_hdr.radmin_image = params->image_radmin;
		params->status_hdr.radmin_warn_image = params->image_radmin_warn;
		params->status_hdr.mutex = CreateMutex(NULL, FALSE, NULL);
		params->status_hdr.lastid = 0;

		// инициализируем сканер
		params->scanprops.instance = params->instance;
		params->scanranges.queue = params->range_queue_hdr.range;
		params->scanranges.down = params->range_down_hdr.range;
		params->scanranges.up = params->range_up_hdr.range;
		params->scanranges.warn = params->range_warn_hdr.range;
		params->scancallbacks.on_finish_param = hwin;
		params->scancallbacks.on_finish = scan_finish_callback;
		params->scancallbacks.on_progress_param = hwin;
		params->scancallbacks.on_progress = scan_progress_callback;
		params->scancallbacks.on_status_param = &(params->status_hdr);
		params->scancallbacks.on_status = scan_status_callback;
		params->scancallbacks.on_error_param = hwin;
		params->scancallbacks.on_error = scan_msg_callback;
		params->scancallbacks.on_result_progress_param = hwin;
		params->scancallbacks.on_result_progress = result_progress_callback;
		if(!(params->scanner = scanner_new(&(params->scanprops),
			&(params->scanranges), &(params->scancallbacks))))
		{
			status = 0;
		}

		// инициализируем иконку в трее
		memset(&(params->notifyicon), 0, sizeof(params->notifyicon));
		params->notifyicon.cbSize = sizeof(params->notifyicon);
		params->notifyicon.hWnd = hwin;
		params->notifyicon.uID = 1;
		params->notifyicon.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
		params->notifyicon.uCallbackMessage = WM_USER+1;
		params->notifyicon.hIcon = params->icon;
		strcpy(params->notifyicon.szTip, notifyicon_tip);

		if(!status)
		{
			DestroyWindow(hwin);
		}

		// проверяем обновления
		if(params->auto_update)
		{
			if(!params->skip_update_times)
			{
				update_check(hwin, software_title, software_version, 0);
				params->skip_update_times = update_check_interval;
			}
			else
			{
				params->skip_update_times--;
			}
		}

		return 1;
	}

	else if(msg == WM_DESTROY)
	{
		params = (void*)GetWindowLongPtr(hwin, DWL_USER);

		Sleep(100);//let all threads to exit

		// удаляем сканер
		scanner_free(params->scanner);

		// сохраняем настроечки
		set_setbool("ping_enable", params->scanprops.ping_enable);
		set_setint("ping_packet_count", params->scanprops.ping_packet_count);
		set_setint("ping_timeout", params->scanprops.ping_timeout);
		set_setint("ping_max_sessions", params->scanprops.ping_max_sessions);
		set_setint("scan_delay_time", params->scanprops.delay_time);
		set_setint("max_thread_count", params->scanprops.max_thread_count);
		set_setint("tcp_timeout", params->scanprops.tcp_timeout);
		set_setint("max_errors", params->scanprops.max_errors);
		set_setint("error_delay", params->scanprops.error_delay);
		set_setbool("random_scan_order", params->scanprops.random_scan_order);
		set_setstr("port_list", params->scanprops.port_list);
		set_setstr("login_dict", params->scanprops.dict1_path);
		set_setstr("pass_dict", params->scanprops.dict2_path);
		set_setint("proxy_type", params->scanprops.proxy_type);
		set_setip("proxy_host", params->scanprops.proxy_host);
		set_setint("proxy_port", params->scanprops.proxy_port);
		set_setstr("proxy_username", params->scanprops.proxy_username);
		set_setstr("proxy_password", params->scanprops.proxy_password);
		set_setbool("auto_remove_bad", params->scanprops.auto_remove_bad);
		set_setbool("enable_window_capture", params->enable_window_capture);
		set_setstr("radmin_path", params->radmin_path);
		set_setbool("minimize_to_tray", params->minimize_to_tray);
		set_setbool("enable_sounds", params->scanprops.enable_sounds);
		set_setbool("auto_update", params->auto_update);
		set_setint("skip_update_checks", params->skip_update_times);

		// удаляем диапазоны
		range_free(params->range_queue_hdr.range);
		range_free(params->range_down_hdr.range);
		range_free(params->range_up_hdr.range);
		range_free(params->range_warn_hdr.range);
		CloseHandle(params->range_control_mutex);

		// удаляем инфу для вывода результатов
		CloseHandle(params->status_hdr.mutex);

		// удаляем список картинок
		ImageList_Destroy(params->imagelist);

		// удаляем настройки
		free(params->scanprops.port_list);
		free(params->add_range_init_data);
		free(params->scanprops.proxy_username);
		free(params->scanprops.proxy_password);
		free(params->scanprops.dict1_path);
		free(params->scanprops.dict2_path);
		free(params->radmin_path);

		PostQuitMessage(0);
		return 1;
	}

	else if(msg == WM_CLOSE)
	{
		params = (void*)GetWindowLongPtr(hwin, DWL_USER);
		if(scanner_isstarted(params->scanner))
		{
			params->scancallbacks.on_finish_param = hwin;
			params->scancallbacks.on_finish = scan_finish_exit_callback;
			scanner_stop(params->scanner);
			return 1;
		}
		DestroyWindow(hwin);
		return 1;
	}

	else if(msg == WM_SIZE)
	{
		if(wp == SIZE_MINIMIZED)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if(params->minimize_to_tray)
			{
				ShowWindow(hwin, SW_HIDE);
				Shell_NotifyIcon(NIM_ADD, &(params->notifyicon));
			}
		}
		return 1;
	}

	else if(msg == WM_USER+1)
	{
		if( (lp == WM_LBUTTONDOWN) ||
			(lp == WM_RBUTTONDOWN) )
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			Shell_NotifyIcon(NIM_DELETE, &(params->notifyicon));
			ShowWindow(hwin, SW_RESTORE);
			SetForegroundWindow(hwin);
		}
	}

	else if(msg == WM_NOTIFY)
	{
		ctl = wp;
		nmhdr = (void*)lp;

		if(ctl == IDC_RESULTS)
		{

			nmlv = (void*)lp;

			if(nmlv->hdr.code == NM_RCLICK)
			{
				if(ListView_GetSelectedCount(nmlv->hdr.hwndFrom) > 0)
				{
					GetCursorPos(&point);
					params = (void*)GetWindowLongPtr(hwin, DWL_USER);
					TrackPopupMenu(params->results_context,
						0,
						point.x,
						point.y,
						0,
						hwin,
						NULL);
				}
			}
			else if(nmlv->hdr.code == LVN_COLUMNCLICK)
			{
				lvsortp.hwincontrol = nmlv->hdr.hwndFrom;
				lvsortp.subitem = nmlv->iSubItem;
				ListView_SortItems(nmlv->hdr.hwndFrom, lvsort_proc, &lvsortp);
			}
			else if(nmlv->hdr.code == LVN_ITEMACTIVATE)
			{
				SendMessage(hwin, WM_COMMAND, IDC_RESULT_LINES_CONNECT, 0);
			}
		}

		else if(ctl == IDC_RANGES)
		{
			nmlv = (void*)lp;
			
			if(nmlv->hdr.code == LVN_COLUMNCLICK)
			{
				lvsortp.hwincontrol = nmlv->hdr.hwndFrom;
				lvsortp.subitem = nmlv->iSubItem;
				ListView_SortItems(nmlv->hdr.hwndFrom, lvsort_proc, &lvsortp);
			}
		}

	}

	else if(msg == WM_COMMAND)
	{
		ctl = LOWORD(wp);

		if(ctl == IDC_ADD_RANGE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			rangewp.instance = params->instance;
			rangewp.owner = hwin;
			rangewp.icon = params->icon;
			rangewp.title = add_hosts_title;
			rangewp.range_limit = params->add_range_init_limit;
			rangewp.range_count = params->add_range_init_count;
			rangewp.range_data = params->add_range_init_data;
			if(rangewin(&rangewp))
			{
				for(i = 0; i < rangewp.range_count; ++i)
				{
					edit_range(RANGE_OP_ADD,
						params->range_queue_hdr.range,
						NULL,
						rangewp.range_data[i].begin,
						rangewp.range_data[i].end);
				}
				params->add_range_init_limit = rangewp.range_limit;
				params->add_range_init_count = rangewp.range_count;
				params->add_range_init_data = rangewp.range_data;
			}
			return 1;
		}

		else if(ctl == IDC_REMOVE_RANGE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			rangewp.instance = params->instance;
			rangewp.owner = hwin;
			rangewp.icon = params->icon;
			rangewp.title = remove_hosts_title;
			rangewp.range_limit = params->add_range_init_limit;
			rangewp.range_count = params->add_range_init_count;
			rangewp.range_data = params->add_range_init_data;
			if(rangewin(&rangewp))
			{
				for(i = 0; i < rangewp.range_count; ++i)
				{
					edit_range(RANGE_OP_REM,
						params->range_queue_hdr.range,
						NULL,
						rangewp.range_data[i].begin,
						rangewp.range_data[i].end);
					edit_range(RANGE_OP_REM,
						params->range_down_hdr.range,
						NULL,
						rangewp.range_data[i].begin,
						rangewp.range_data[i].end);
					edit_range(RANGE_OP_REM,
						params->range_up_hdr.range,
						NULL,
						rangewp.range_data[i].begin,
						rangewp.range_data[i].end);
					edit_range(RANGE_OP_REM,
						params->range_warn_hdr.range,
						NULL,
						rangewp.range_data[i].begin,
						rangewp.range_data[i].end);
				}
				params->add_range_init_limit = rangewp.range_limit;
				params->add_range_init_count = rangewp.range_count;
				params->add_range_init_data = rangewp.range_data;
			}
			return 1;
		}

		else if(ctl == IDC_REMOVE_QUEUE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_CLEAR, params->range_queue_hdr.range, NULL, 0, 0);
			return 1;
		}

		else if(ctl == IDC_REMOVE_DOWN)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_CLEAR, params->range_down_hdr.range, NULL, 0, 0);
			return 1;
		}

		else if(ctl == IDC_REMOVE_UP)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_CLEAR, params->range_up_hdr.range, NULL, 0, 0);
			return 1;
		}

		else if(ctl == IDC_REMOVE_WARN)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_CLEAR, params->range_warn_hdr.range, NULL, 0, 0);
			return 1;
		}

		else if(ctl == IDC_REMOVE_ALL)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_CLEAR, params->range_queue_hdr.range, NULL, 0, 0);
			edit_range(RANGE_OP_CLEAR, params->range_down_hdr.range, NULL, 0, 0);
			edit_range(RANGE_OP_CLEAR, params->range_up_hdr.range, NULL, 0, 0);
			edit_range(RANGE_OP_CLEAR, params->range_warn_hdr.range, NULL, 0, 0);
			return 1;
		}

		else if(ctl == IDC_ADD_DOWN)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_down_hdr.range, 0, 0);
			return 1;
		}

		else if(ctl == IDC_ADD_WARN)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_warn_hdr.range, 0, 0);
			return 1;
		}

		else if(ctl == IDC_ADD_DOWN_WARN)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_down_hdr.range, 0, 0);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_warn_hdr.range, 0, 0);
			return 1;
		}

		else if(ctl == IDC_ADD_UP)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_up_hdr.range, 0, 0);
			return 1;
		}

		else if(ctl == IDC_ADD_ALL)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_down_hdr.range, 0, 0);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_warn_hdr.range, 0, 0);
			edit_range(RANGE_OP_MERGE, params->range_queue_hdr.range,
				params->range_up_hdr.range, 0, 0);
			return 1;
		}

		else if(ctl == IDC_PROXY_SETUP)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			proxydp.instance = params->instance;
			proxydp.owner = hwin;
			proxydp.icon = params->icon;
			proxydp.proxy_type = params->scanprops.proxy_type;
			proxydp.proxy_host = params->scanprops.proxy_host;
			proxydp.proxy_port = params->scanprops.proxy_port;
			proxydp.max_username = params->scanprops.max_proxy_username;
			proxydp.username = params->scanprops.proxy_username;
			proxydp.max_password = params->scanprops.max_proxy_password;
			proxydp.password = params->scanprops.proxy_password;
			if(proxydlg(&proxydp))
			{
				params->scanprops.proxy_type = proxydp.proxy_type;
				params->scanprops.proxy_host = proxydp.proxy_host;
				params->scanprops.proxy_port = proxydp.proxy_port;
			}
			return 1;
		}

		else if(ctl == IDC_SCANNER_SETUP)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			scannerdp.instance = params->instance;
			scannerdp.icon = params->icon;
			scannerdp.owner = hwin;
			scannerdp.props = &(params->scanprops);
			scannerdlg(&scannerdp);
			return 1;
		}

		else if(ctl == IDC_CUSTOMIZE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			customizedp.instance = params->instance;
			customizedp.owner = hwin;
			customizedp.icon = params->icon;
			customizedp.max_radmin_path = params->max_radmin_path;
			customizedp.radmin_path = params->radmin_path;
			customizedp.enable_window_capture = params->enable_window_capture;
			customizedp.minimize_to_tray = params->minimize_to_tray;
			customizedp.check_updates = params->auto_update;
			customizedp.enable_sounds = params->scanprops.enable_sounds;
			if(customizedlg(&customizedp))
			{
				params->enable_window_capture = customizedp.enable_window_capture;
				params->minimize_to_tray = customizedp.minimize_to_tray;
				params->auto_update = customizedp.check_updates;
				params->scanprops.enable_sounds = customizedp.enable_sounds;
			}
			return 1;
		}

		else if(ctl == IDC_SCAN_START)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if(!scanner_start(params->scanner))
			{
				MessageBox(hwin,
					scan_start_error,
					scan_error_title,
					MB_ICONINFORMATION|MB_OK);
			}
			return 1;
		}

		else if(ctl == IDC_SCAN_STOP)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if(!scanner_stop(params->scanner))
			{
				MessageBox(hwin,
					scan_stop_error,
					scan_error_title,
					MB_ICONINFORMATION|MB_OK);
			}
			return 1;
		}

		else if(ctl == IDC_SCAN_PAUSE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if(!scanner_pause(params->scanner))
			{
				MessageBox(hwin,
					scan_pause_error,
					scan_error_title,
					MB_ICONINFORMATION|MB_OK);
			}
			return 1;
		}

		else if(ctl == IDC_SAVE_RANGE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((filename = malloc(MAX_PATH)))
			{
				strcpy(filename, "");
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hInstance = params->instance;
				ofn.hwndOwner = hwin;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFile = filename;
				ofn.lpstrFilter = csv_filter;
				ofn.lpstrDefExt = csv_ext;
				ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				ofn.lpstrTitle = save_csv_range_title;
				if(GetSaveFileName(&ofn))
				{
					if(!range_export(filename,
						params->range_queue_hdr.range,
						params->range_up_hdr.range,
						params->range_down_hdr.range,
						params->range_warn_hdr.range))
					{
						MessageBox(hwin,
							range_export_error_text,
							range_export_error_title,
							MB_ICONEXCLAMATION|MB_OK);
					}
				}
				free(filename);
			}
			return 1;
		}

		else if(ctl == IDC_LOAD_RANGE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((filename = malloc(MAX_PATH)))
			{
				strcpy(filename, "");
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hInstance = params->instance;
				ofn.hwndOwner = hwin;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFile = filename;
				ofn.lpstrFilter = csv_filter;
				ofn.lpstrDefExt = csv_ext;
				ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
				ofn.lpstrTitle = load_csv_range_title;
				if(GetOpenFileName(&ofn))
				{
					if(!range_import(filename,
						params->range_queue_hdr.range,
						params->range_up_hdr.range,
						params->range_down_hdr.range,
						params->range_warn_hdr.range))
					{
						MessageBox(hwin,
							load_csv_range_error_text,
							load_csv_range_error_title,
							MB_ICONEXCLAMATION|MB_OK);
					}
				}
				free(filename);
			}
			return 1;
		}

		else if(ctl == IDC_SAVE_RESULTS)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((filename = malloc(MAX_PATH)))
			{
				strcpy(filename, "");
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hInstance = params->instance;
				ofn.hwndOwner = hwin;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFile = filename;
				ofn.lpstrFilter = csv_filter;
				ofn.lpstrDefExt = csv_ext;
				ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				ofn.lpstrTitle = save_results_range_title;
				if(GetSaveFileName(&ofn))
				{
					if(!results_export_csv(filename,
						params->results))
					{
						MessageBox(hwin,
							results_export_error_text,
							results_export_error_title,
							MB_ICONEXCLAMATION|MB_OK);
					}
				}
				free(filename);
			}
			return 1;
		}

		else if(ctl == IDC_LOAD_RESULTS)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((filename = malloc(MAX_PATH)))
			{
				strcpy(filename, "");
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hInstance = params->instance;
				ofn.hwndOwner = hwin;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFile = filename;
				ofn.lpstrFilter = csv_filter;
				ofn.lpstrDefExt = csv_ext;
				ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
				ofn.lpstrTitle = load_results_range_title;
				if(GetOpenFileName(&ofn))
				{
					if(!results_import_csv(filename,
						params->results,
						params->image_radmin))
					{
						MessageBox(hwin,
							results_import_error_text,
							results_import_error_title,
							MB_ICONEXCLAMATION|MB_OK);
					}
				}
				free(filename);
			}
			return 1;
		}

		else if(ctl == IDC_EXPORT_HTML_RESULTS)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((filename = malloc(MAX_PATH)))
			{
				strcpy(filename, "");
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hInstance = params->instance;
				ofn.hwndOwner = hwin;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFile = filename;
				ofn.lpstrFilter = html_filter;
				ofn.lpstrDefExt = html_ext;
				ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				ofn.lpstrTitle = export_html_results_title;
				if(GetSaveFileName(&ofn))
				{
					if(!results_export_html(filename,
						params->results))
					{
						MessageBox(hwin,
							results_export_html_error_text,
							results_export_html_error_title,
							MB_ICONEXCLAMATION|MB_OK);
					}
				}
				free(filename);
			}
			return 1;
		}

		else if(ctl == IDC_CLEAR_RESULTS)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			ListView_DeleteAllItems(params->results);
			return 1;
		}

		else if(ctl == IDC_RESULT_LINES_REMOVE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if((count = ListView_GetItemCount(params->results)))
			{
				i = 0;
				while(i < count)
				{
					if(ListView_GetItemState(params->results, i, 
						LVIS_SELECTED) == LVIS_SELECTED)
					{
						ListView_DeleteItem(params->results, i);
					}
					else
					{
						i++;
					}
				}
			}
			return 1;
		}

		else if( (ctl == IDC_RESULT_LINES_CONNECT) ||
			(ctl == IDC_RESULT_LINES_CONNECT_NOINPUT) ||
			(ctl == IDC_RESULT_LINES_CONNECT_SHUTDOWN) ||
			(ctl == IDC_RESULT_LINES_CONNECT_FILE) ||
			(ctl == IDC_RESULT_LINES_CONNECT_TELNET) )
		{
			mode = 0;
			if(ctl == IDC_RESULT_LINES_CONNECT_NOINPUT)
				mode = 1;
			else if(ctl == IDC_RESULT_LINES_CONNECT_SHUTDOWN)
				mode = 2;
			else if(ctl == IDC_RESULT_LINES_CONNECT_FILE)
				mode = 3;
			else if(ctl == IDC_RESULT_LINES_CONNECT_TELNET)
				mode = 4;
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			if(!results_connect(params->results, 
				params->radmin_path, 
				params->enable_window_capture,
				mode))
			{
				MessageBox(hwin,
					radmin_connect_error_text,
					radmin_connect_error_title,
					MB_ICONEXCLAMATION|MB_OK);
			}
			return 1;
		}

		else if(ctl == IDC_RESULTS_COPY_ADDRESS)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			results_copy_to_clipboard(params->results, 0x06);
			return 1;
		}

		else if(ctl == IDC_RESULTS_COPY_ACCOUNT)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			results_copy_to_clipboard(params->results, 0xC0);
			return 1;
		}

		else if(ctl == IDC_RESULTS_COPY_ADDRESS_AND_ACCOUNT)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			results_copy_to_clipboard(params->results, 0xC6);
			return 1;
		}

		else if(ctl == IDC_RESULTS_COPY_LINE)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			results_copy_to_clipboard(params->results, 0xff);
			return 1;
		}

		else if(ctl == IDC_EXIT)
		{
			SendMessage(hwin, WM_CLOSE, 0, 0);
			return 1;
		}

		else if(ctl == ID_ABOUT)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			wsprintf(buf, "%s %s от redsh\r\n\r\nИзготовлено в СССР", 
				software_title, 
				software_version);
			memset(&mbp, 0, sizeof(mbp));
			mbp.cbSize = sizeof(mbp);
			mbp.hwndOwner = hwin;
			mbp.lpszCaption = about_title;
			mbp.lpszText = buf;
			mbp.hInstance = params->instance;
			mbp.lpszIcon = MAKEINTRESOURCE(IDI_MAIN);
			mbp.dwStyle = MB_USERICON|MB_OK;
			MessageBoxIndirect(&mbp);
			return 1;
		}

		else if(ctl == ID_WEBSITE)
		{
			ShellExecute(hwin, "open", web_path, NULL, NULL, SW_SHOW);
			return 1;
		}

		else if(ctl == ID_EMAIL)
		{
			ShellExecute(hwin, "open", email, NULL, NULL, SW_SHOW);
			return 1;
		}

		else if(ctl == ID_MANUAL)
		{
			if(GetFileAttributes(manual) == 0xffffffff) {
				MessageBox(hwin, 
					manual_miss_text,
					manual_miss_title,
					MB_ICONINFORMATION|MB_OK);
			}
			else {
				ShellExecute(hwin, "open", manual, NULL, NULL, SW_SHOW);
			}
			return 1;
		}

		else if(ctl == ID_UPDATECHECK)
		{
			params = (void*)GetWindowLongPtr(hwin, DWL_USER);
			update_check(hwin, software_title, software_version, 1);
			params->skip_update_times = update_check_interval;
			return 1;
		}
	}

	return 0;
}

/* -------------------------------------------------------------------------- */

int mainwin(mainwin_params *params, int show)
{
	MSG msg;
	HWND hwin;
	HACCEL accel;

	if((hwin = CreateDialogParam(params->instance, 
		MAKEINTRESOURCE(IDD_MAINWIN),
		params->owner,
		MainwinProc,
		(LPARAM)params)))
	{
		ShowWindow(hwin, show);
		accel = LoadAccelerators(params->instance, MAKEINTRESOURCE(IDR_MAINACCEL));
		while(GetMessage(&msg, NULL, 0, 0))
		{
			if(!TranslateAccelerator(hwin, accel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return 0;
}

/* -------------------------------------------------------------------------- */
