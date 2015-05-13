// InjectionCodeVisualStudioProject.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <iostream>


/*int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	char szFilename[] = "explorer.exe";
	// получаем дескриптор снимка процессов	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	do {
		if (!Process32Next(hProcessSnap, &pe32)) // перечисляем процессы
			return FALSE;
	} while (lstrcmpi(pe32.szExeFile, szFilename)); // ищем нужный процесс

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pe32.th32ProcessID);
	if (!hProcess)
		return FALSE;
	// выделяем память в чужом процессе
	LPVOID lpMemory = VirtualAllocEx(hProcess, NULL, BUFFERSIZE, MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);
	if (!lpMemory)
		return FALSE;
	// пишем свой код в этот процесс (указатель на код в переменной lpBuffer)
	if (!WriteProcessMemory(hProcess, lpMemory, lpBuffer, BUFFERSIZE,
		&NumberOfBytesWritten))
		return FALSE;
	// запускаем код отдельным потоком
	if (!CreateRemoteThread(hProcess, NULL, NULL, lpMemory, NULL, NULL, NULL))
		return FALSE;

	CloseHandle(hProcessSnap);
	return 0;
}*/

bool Inject(DWORD pId, char *dllName);
using namespace std;

int main()
{

	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	char szFilename[] = "notepad++.exe";
	// получаем дескриптор снимка процессов	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	do {
		if (!Process32Next(hProcessSnap, &pe32)) // перечисляем процессы
			return FALSE;
	} while (lstrcmpi(pe32.szExeFile, szFilename)); // ищем нужный процесс


	Inject(pe32.th32ProcessID, "C:\\dllToInjection.dll");
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
		}
		LPVOID dereercomp = VirtualAllocEx(h, NULL, strlen(dllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (dereercomp == NULL) {
			std::cout << "Error: the memory could not be allocated inside the chosen process.\n" << std::endl;
		}
		int n = WriteProcessMemory(h, dereercomp, dllName, strlen(dllName), NULL);
		if (n == 0) {
			std::cout << "Error: there was no bytes written to the process's address space.\n" << std::endl;
		}
		HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE) LoadLibAddr, dereercomp, 0, NULL);
		if (asdc == NULL) {
			printf("Error: the remote thread could not be created.\n");
		}
		//WaitForSingleObject(asdc, INFINITE);
		//VirtualFreeEx(h, dereercomp, strlen(dllName), MEM_RELEASE);
		//CloseHandle(asdc);
		//CloseHandle(h);
		return true;
	}
	return false;
}

//
////Coded By JuryBen
//
//
//#pragma comment(lib,"user32.LIB")
//
//
//#include <iostream>
//#include <windows.h>
//#include <tlhelp32.h>
//
//PROCESSENTRY32 Process;
//HANDLE Snapshot;
//BOOL ProcessFound;
//
//
//struct RemoteData
//{
//	//Only declare, don't initialize
//
//	//MsgBox
//	wchar_t sz[256];
//	wchar_t mt[256];
//
//	//ShellExecute
//	wchar_t open[256];
//	wchar_t url[256];
//
//
//	//GetModuleFileName
//	wchar_t locationbuffer[MAX_PATH];
//
//	//DWORD vars
//	DWORD dwMessageBox;
//	DWORD dwShell;
//	DWORD dwLocation;
//
//};
//
////TYPEDEFS
//
//typedef int(__stdcall*MMessageBox)(HWND, LPCTSTR, LPCTSTR, UINT);
//typedef HINSTANCE(__stdcall*MShellExecute)(HWND, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, INT);
//typedef DWORD(__stdcall*MGetModuleFileName)(HMODULE, LPTSTR, DWORD);
//
////REMOTE THREAD
//DWORD __stdcall MHThread(RemoteData *pData)
//{
//	MMessageBox MsgBox = (MMessageBox) pData->dwMessageBox;
//	MShellExecute SE = (MShellExecute) pData->dwShell;
//	MGetModuleFileName GMFN = (MGetModuleFileName) pData->dwLocation;
//
//	SE(NULL, pData->open, pData->url, NULL, NULL, SW_SHOWDEFAULT);
//	MsgBox(0, pData->sz, pData->mt, 0);
//	GMFN(NULL, pData->locationbuffer, sizeof(pData->locationbuffer));
//	MsgBox(0, pData->locationbuffer, pData->mt, 0);
//
//	return 0;
//}
//
//
//int Inject()
//{
//
//	wchar_t ProcessName[1024];
//	std::wcin >> ProcessName;
//	bool search = false;
//	while (!search)
//	{
//		Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//		Process.dwSize = sizeof(Process);
//		ProcessFound = Process32First(Snapshot, &Process);
//		while (ProcessFound)
//		{
//			wchar_t searchbuffer[512];
//			wcscpy_s(searchbuffer, Process.szExeFile);
//			if (!wcscmp(searchbuffer, ProcessName))
//			{
//
//				Sleep(1000);
//				RemoteData data;
//				ZeroMemory(&data, sizeof (RemoteData));
//				wcscat_s(data.sz, L"im in your processes haxin your threads");
//				wcscat_s(data.mt, L"say wut nuggaaaa");
//				wcscat_s(data.open, L"open");
//				wcscat_s(data.url, L"http://lastfrag.com");
//
//				HINSTANCE hUser = LoadLibrary(L"user32.dll");
//				HINSTANCE hShell = LoadLibrary(L"shell32.dll");
//				HINSTANCE hKernel = LoadLibrary(L"kernel32.dll");
//
//				if (!hUser || !hShell || !hKernel)
//				{
//					MessageBox(0, L"LL Failed", L"Error!", 0);
//					return 0;
//				}
//
//				data.dwMessageBox = (DWORD) GetProcAddress(hUser, "MessageBoxW");
//				data.dwShell = (DWORD) GetProcAddress(hShell, "ShellExecuteW");
//				data.dwLocation = (DWORD) GetProcAddress(hKernel, "GetModuleFileNameW");
//
//				FreeLibrary(hUser);
//				FreeLibrary(hShell);
//				FreeLibrary(hKernel);
//
//				if (!data.dwMessageBox || !data.dwShell || !data.dwLocation)
//				{
//					MessageBox(0, L"Handles Failed", L"Error!", 0);
//					return 0;
//				}
//
//				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Process.th32ProcessID);
//
//				if (!hProcess)
//				{
//					MessageBox(0, L"OP Failed", L"Error!", 0);
//					return 0;
//				}
//
//				void *pRemoteThread = VirtualAllocEx(hProcess, 0, sizeof (RemoteData), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//
//				if (!pRemoteThread)
//				{
//					MessageBox(0, L"RT Failed", L"Error!", 0);
//					return 0;
//				}
//
//				if (!WriteProcessMemory(hProcess, pRemoteThread, (void*) MHThread, sizeof (RemoteData), 0))
//				{
//					MessageBox(0, L"WPM Failed", L"Error!", 0);
//					return 0;
//				}
//
//				RemoteData *pData = (RemoteData*) VirtualAllocEx(hProcess, 0, sizeof (RemoteData), MEM_COMMIT, PAGE_READWRITE);
//				if (!pData)
//				{
//					MessageBox(0, L"RT Failed", L"Error!", 0);
//					return 0;
//
//				if (!WriteProcessMemory(hProcess, pData, &data, sizeof (RemoteData), 0))
//				{
//					MessageBox(0, L"WPM Failed", L"Error!", 0);
//					return 0;
//				}
//
//				HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE) pRemoteThread, pData, 0, 0);
//				if (!hThread)
//				{
//					MessageBox(0, L"CRT Failed", L"Error!", 0);
//					return 0;
//				}
//
//				CloseHandle(hThread);
//
//				VirtualFreeEx(hProcess, pRemoteThread, sizeof (RemoteData), MEM_RELEASE);
//				CloseHandle(hProcess);
//
//				search = true;
//
//			}
//
//			ProcessFound = Process32Next(Snapshot, &Process);
//		}
//		CloseHandle(Snapshot);
//
//		Sleep(100);
//	}
//	std::cin.ignore();
//	std::cin.get();
//
//	return 0;
//}
//
//
//int main()
//{
//	Inject();
//
//	return 0;
//}
//
//Sursa: Doar utilizatorii inregistrati pot vedea linkurile.[Click aici pentru a te inregistra]
//
//	Inainte de a posta :
//	   -verifica data ultimului post, nu redeschide un topic fara un motiv serios
//		   - daca e cazul vezi mai intai ce zice prietenul Google
//		   - gandeste - te daca postul tau e util sau postezi doar ca sa te afli in treaba
//		   - asigura - te ca nu incalci regulile
//		   - fii atent la limbajul folosit, vorbeste frumos si cat mai corect gramatical
//		   - pentru orice problema legata de forum, contacteaza - ma
//
//		   Reply With Quote Reply With Quote





