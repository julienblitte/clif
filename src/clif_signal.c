#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

/* current sub-process pid */
static int signal_current_pid;

#ifdef _WIN32
	#include <windows.h>
	/* action on signal (windows) */
	BOOL WINAPI signal_handler(DWORD sig)
	{
		switch(sig)
		{
		case CTRL_C_EVENT:
			printf("<C-C>\n");
		break;
		case CTRL_BREAK_EVENT:
			printf("<C-BREAK>\n");
		break;
		case CTRL_CLOSE_EVENT:
			printf("<CLOSE>\n");
		break;
		case CTRL_LOGOFF_EVENT:
			printf("<LOGOFF>\n");
		break;
		case CTRL_SHUTDOWN_EVENT:
			printf("<SHUTDOWN>\n");
		break;
		default:
			printf("<INT_%d>\n", sig);
		break;
		}

		if (signal_current_pid)
		{
			kill(signal_current_pid, sig);
		}
		return TRUE;
	}
#else
	/* action on signal (linux) */
	void signal_handler(int sig)
	{
		switch(sig)
		{
		case SIGINT:
			printf("<C-C>\n");
		break;
		case SIGTERM:
			printf("<C-BREAK>\n");
		break;
		case SIGQUIT:
			printf("<C-\\>\n");
		break;
		case SIGTSTP:
			printf("<C-Z>\n");
		break;
		default:
			printf("<INT_%d>\n", sig);
		break;
		}

		if (signal_current_pid)
		{
			kill(signal_current_pid, sig);
		}
	}
#endif


int signal_register_pid(int pid)
{
	signal_current_pid = pid;

	return pid;
}

int init_signal_handler()
{
	struct sigaction sa;
	int errors;

	errors = 0;

	sigemptyset(&sa.sa_mask);
	sa.sa_handler = signal_handler;

	if (sigaction(SIGINT, &sa, NULL) == -1) errors++;
	if (sigaction(SIGTERM, &sa, NULL) == -1) errors++;
	if (sigaction(SIGQUIT, &sa, NULL) == -1) errors++;
	if (sigaction(SIGTSTP, &sa, NULL) == -1) errors++;

#ifdef _WIN32
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)signal_handler, TRUE))
	{
		errors++;
	}
#endif

	if (errors)
	{
		fprintf(stderr, "Warning: unable to signal install handler (%d times).\n", errors);
	}

	return errors;
}

