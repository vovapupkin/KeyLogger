#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

HANDLE	file;

bool CreateLogFile()
{
	CHAR path[MAX_PATH];
	SYSTEMTIME st;
	DWORD fileNum = 1;
	GetLocalTime(&st);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, "dd.MM.yy", path, MAX_PATH);
	char * ptrAfterSyl_ = path + strlen(path) + 1;
	strcat(path, "_1.txt");

	file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	while (GetLastError() == ERROR_FILE_EXISTS)
	{
		fileNum++;
		wsprintf(ptrAfterSyl_, "%d", fileNum);
		strcat(path, ".txt");
		file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	return file != INVALID_HANDLE_VALUE;
}

int main()
{
	HANDLE mutex = CreateMutex(NULL, FALSE, "KeyboardLogger");
	if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED) 
		return 1;
	if (!CreateLogFile())
	{
		ReleaseMutex(mutex);
		return 1;
	}

	return 0;
}