#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <iostream>

bool Inject(DWORD pId, char *dllName);
using namespace std;

int main()
{

	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	char szFilename[] = "notepad++.exe";

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	do {
		if (!Process32Next(hProcessSnap, &pe32))
		{
			std::cout << "Can't find Process:"<< szFilename << std::endl;
			Sleep(10000);
			return FALSE;
		}
		
	} while (lstrcmpi(pe32.szExeFile, szFilename));

	if (Inject(pe32.th32ProcessID, "InjectionDll.dll"))
	{
		std::cout << "___OK:" << pe32.szExeFile << std::endl;

	}
	Sleep(10000);
	return 0;
}

bool Inject(DWORD pId, char *dllName)
{
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
	if (h)
	{
		LPVOID LoadLibAddr = (LPVOID) GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		if (LoadLibAddr == NULL) {
			std::cout << "Error: the LoadLibraryA function was not found inside kernel32.dll library.\n" << std::endl;
			//Sleep(10000);
			return false;
		}
		LPVOID dereercomp = VirtualAllocEx(h, NULL, strlen(dllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (dereercomp == NULL) {
			std::cout << "Error: the memory could not be allocated inside the chosen process.\n" << std::endl;
			//Sleep(10000);
			return false;
		}
		int n = WriteProcessMemory(h, dereercomp, dllName, strlen(dllName), NULL);
		if (n == 0) {
			std::cout << "Error: there was no bytes written to the process's address space.\n" << std::endl;
			//Sleep(10000);
			return false;
		}
		HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE) LoadLibAddr, dereercomp, 0, NULL);
		if (asdc == NULL) {
			printf("Error: the remote thread could not be created.\n");
			//Sleep(10000);
			return false;
		}
		//WaitForSingleObject(asdc, INFINITE);
		//VirtualFreeEx(h, dereercomp, strlen(dllName), MEM_RELEASE);
		//CloseHandle(asdc);
		//CloseHandle(h);
		return true;
	}
	std::cout << "Error: Can't open process:" << pId << std::endl;
	//Sleep(10000);
	return false;
}
