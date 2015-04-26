#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

bool CreateLogFile()
{

}

int main()
{
	HANDLE mutex = CreateMutex(NULL, FALSE, "KeyboardLogger");
	if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED) 
		return 1;

	return 0;
}