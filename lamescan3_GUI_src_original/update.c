/* -------------------------------------------------------------------------- */

#include "update.h"

/* -------------------------------------------------------------------------- */

char *update_url = "http://redsh.tk/update/lamescan.php";

/* -------------------------------------------------------------------------- */

DWORD WINAPI update_thread(update_thread_param *param)
{
	char *update;
	char *temp;
	char message[1024];
	char *lastversion;
	char *downloadlink;
	char *releasenote;
	

	if((update = http_get(update_url)))
	{
		if((lastversion = strtok_s(update, "\r\n", &temp)))
		{
			if((downloadlink = strtok_s(NULL, "\r\n", &temp)))
			{
				if((strcmp(param->currversion, lastversion)) != 0)
				{
					wsprintf(message, "Доступен %s %s\n\n"
						"Изменения:\n",
						param->softwarename,
						lastversion);
					while((releasenote = strtok_s(NULL, "\r\n", &temp)))
					{
						strcat(message, "  • ");
						strcat(message, releasenote);
						strcat(message, "\n");
					}
					strcat(message, "\nСкачать новую версию?");

					if(MessageBox(param->owner,
						message,
						"Найдена новая версия",
						MB_ICONINFORMATION|MB_YESNO) == IDYES)
					{
						ShellExecute(param->owner,
							"open",
							downloadlink,
							NULL,
							NULL,
							SW_SHOW);
					}
				}
				else
				{
					if(param->notifyerrors)
					{
						wsprintf(message, 
							"У тебя последняя версия %s %s",
							param->softwarename,
							param->currversion);
						MessageBox(param->owner,
							message,
							"Обновлений не найдено",
							MB_ICONINFORMATION|MB_OK);
					}
				}
			}
		}
		free(update);
	}
	else
	{
		if(param->notifyerrors)
		{
			MessageBox(param->owner,
				"Не удаётся соединиться с сервером обновлений",
				"Проверка обновлений",
				MB_ICONEXCLAMATION|MB_OK);
		}
	}

	free(param);
	return 0;
}

/* -------------------------------------------------------------------------- */

int update_check(HWND hwin, char *softwarename, char *currversion, int notifyerrors)
{
	HANDLE thread;
	update_thread_param *utp;

	if((utp = malloc(sizeof(update_thread_param))))
	{
		utp->owner = hwin;
		utp->notifyerrors = notifyerrors;
		utp->softwarename = softwarename;
		utp->currversion = currversion;

		if((thread = CreateThread(NULL, 0, 
			update_thread, utp, 0, NULL)))
		{
			CloseHandle(thread);
			return 1;
		}

		free(utp);
	}

	return 0;
}

/* -------------------------------------------------------------------------- */
