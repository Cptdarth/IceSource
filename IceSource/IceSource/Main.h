#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "wininet")

#pragma once

#include "stdafx.h"

#include "Rlua.h"
#include "Scan.h"
#include "Functions.h"
#include "Commands.h"
#include "Auth.h"

using namespace std;
//Command pipe
DWORD WINAPI CmdPipe(PVOID lvpParameter)
{
	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\IceCmd"),//IceCmd is the name of the command pipe
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';
				try {
					ExecuteCommand(buffer);
				}
				catch (std::exception e) {
					MessageBoxA(NULL, e.what(), " - Uh Oh", MB_OK);
				}
				catch (...) {
					MessageBoxA(NULL, "An Unhandled Error Has Occured!", "- Uh Oh", MB_OK);
					cout << "ERROR: An Unexpected Error Has Occured!" << endl;
				}
			}
		}
		DisconnectNamedPipe(hPipe);
	}
}
//Lua C pipe
DWORD WINAPI LuaCPipe(PVOID lvpParameter)
{
	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\IceLuaC"),//IceLuaC is the name of the lua c pipe
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';
				try {
					luaC(buffer);
				}
				catch (std::exception e) {
					MessageBoxA(NULL, e.what(), " - Uh Oh", MB_OK);
				}
				catch (...) {
					MessageBoxA(NULL, "An Unhandled Error Has Occured!", "- Uh Oh", MB_OK);
					cout << "ERROR: An Unexpected Error Has Occured!" << endl;
				}
			}
		}
		DisconnectNamedPipe(hPipe);
	}
}

int Init() {
	CreateThread(NULL, 0, CmdPipe, NULL, 0, NULL);//Create new thread for command pipe
	CreateThread(NULL, 0, LuaCPipe, NULL, 0, NULL);//Create new thread for lua c pipe
	//Auth(); //uncomment this function to enable the whitelist duh
	Scan();//Scan lua state
	MessageBox(NULL, "\nIce has loaded Correctly!\nnow you can use Ice!\nMade by Josh() and updated by rakion99", "Ice", MB_OK | MB_TOPMOST);//Messagebox so we can know when scan finished
	return 0;
}