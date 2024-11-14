#include "appctrl.h"

#if defined WIN32	// WIN32
#include <windows.h>
#endif

#if defined(__UNIX) || defined(unix)	// Linux
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

bool IsApplicationActive(const string &appId)
{
#if defined WIN32   //for WIN32
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, LPCWSTR(appId.c_str()));
	if (GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		return  true;
	}
			
	return false;
#endif
#if defined(__UNIX) || defined(unix)   //for Linux

	string lckFile = "/tmp/" + appId + ".lck";
	int fd = open(lckFile.c_str(), O_WRONLY | O_CREAT, 0644);

	int flock = lockf(fd, F_TLOCK, 0);
	if (fd == -1 || flock == -1) 
	{
		return true;
	}

	return false;
#endif

	return false;
}
