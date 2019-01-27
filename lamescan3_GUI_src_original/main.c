/* -------------------------------------------------------------------------- */

#include <windows.h>
#include <commctrl.h>
#include <srp.h>
#include "mainwin.h"
#include "tcp.h"
#include "settings.h"

/* -------------------------------------------------------------------------- */

int WINAPI WinMain(HINSTANCE instance, HINSTANCE unused, char *cmdline, int show)
{
	mainwin_params mainwp;
	HMODULE riched20;
	if((riched20 = LoadLibrary("riched20")))
	{
		if(wsa_init())
		{
			if(set_init())
			{
				if(!SRP_initialize_library())
				{
					InitCommonControls();

					mainwp.instance = instance;
					mainwp.owner = NULL;
					mainwin(&mainwp, show);
					
					SRP_finalize_library();
				}
				set_final();
			}
			wsa_final();
		}
		FreeLibrary(riched20);
	}
	return 0;
}

/* -------------------------------------------------------------------------- */
