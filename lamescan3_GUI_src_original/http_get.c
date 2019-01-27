/* -------------------------------------------------------------------------- */

#include "http_get.h"

/* -------------------------------------------------------------------------- */

char *http_get(char *url)
{
	char *result = NULL;
	char *urlp, *temp;
	char *host, *path;
	char str[256];
	SOCKET sock;
	struct hostent *he;
	struct sockaddr_in addr;
	int port;
	int buflen;
	char *buff;
	char *buff_tmp;
	int buflen_tmp;
	int n;
	char *head;
	char *body;
	char *buffp;
	int bodylen;
	char *headp;
	char *proto;
	char *codep;
	int code;

	if((url = strdup(url)))
	{
		urlp = url;

		if(strnicmp(urlp, "http://", 7) == 0)
			urlp += 7;

		host = urlp;

		if((urlp = strchr(urlp, '/')))
		{
			*urlp = 0;
			path = urlp + 1;

			port = 80;

			if((temp = strchr(host, ':')))
			{
				*temp = 0;
				port = atoi(temp + 1);
			}

			if((he = gethostbyname(host)))
			{
				memset(&addr, 0, sizeof(addr));
				addr.sin_family = AF_INET;
				memcpy(&(addr.sin_addr), he->h_addr_list[0], 4);
				addr.sin_port = htons(port);

				if((sock = socket(AF_INET, SOCK_STREAM, 
					IPPROTO_TCP)) != SOCKET_ERROR)
				{
					if(connect(sock, (struct sockaddr*)&addr, 
						sizeof(addr)) != SOCKET_ERROR)
					{
						wsprintf(str,
							"GET /%s HTTP/1.0\r\n"
							"Host: %s\r\n\r\n",
							path,
							host);

						if(send(sock, str, (int)strlen(str), 0) > 0)
						{
							buflen = 0;
							buff = NULL;
							while((n = recv(sock, str, sizeof(str), 0)) > 0)
							{
								buflen_tmp = buflen + n;
								if((buff_tmp = realloc(buff, buflen_tmp)))
								{
									buff = buff_tmp;
									memcpy(buff + buflen, str, n);
									buflen = buflen_tmp;
								}
							}
							
							if(buff)
							{
								buffp = buff;

								head = buffp;

								if((buffp = strstr(buffp, "\r\n")))
								{
									*buffp = 0;
									buffp += 2;
									if((buffp = strstr(buffp, "\r\n\r\n")))
									{
										*buffp = 0;
										body = buffp + 4;

										//bodylen = (int)strlen(body);

										bodylen = buflen - (body - buff);
										
										headp = head;

										proto = headp;

										if((headp = strchr(headp, ' ')))
										{
											*headp = 0;
											codep = headp + 1;
											code = atoi(codep);

											if(code / 100 == 2) //200..299
											{
												if((result = malloc(bodylen+1)))
												{
													memcpy(result, body, bodylen);
													result[bodylen] = 0;
												}
											}
										}
									}
								}
							}
							free(buff);
						}

						shutdown(sock, 1);
					}
					closesocket(sock);
				}
			}

		}

		free(url);
	}

	return result;
}

/* -------------------------------------------------------------------------- */
