#include <windows.h>

#include <tlhelp32.h>

#include <cstdio>

#include <cwchar>

int wmain()
{
	const wchar_t* Process_Name = (__argc > 1) ? __wargv[1] : L"left4dead2.exe";

	wprintf(L"Vortex Injector - searching for %ls...\n", Process_Name);

	HANDLE Process_Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32W Process_Entry = { sizeof(Process_Entry) };

	unsigned __int32 Process_Identifier = 0;

	if (Process32FirstW(Process_Snapshot, &Process_Entry) != FALSE)
	{
		do
		{
			if (_wcsicmp(Process_Entry.szExeFile, Process_Name) == 0)
			{
				Process_Identifier = Process_Entry.th32ProcessID;

				break;
			}
		} while (Process32NextW(Process_Snapshot, &Process_Entry) != FALSE);
	}

	CloseHandle(Process_Snapshot);

	if (Process_Identifier == 0)
	{
		wprintf(L"FAILED: the process %ls is not running.\n", Process_Name);

		return 1;
	}

	wprintf(L"Process found: %lu\n", Process_Identifier);

	wchar_t Dll_Path[MAX_PATH];

	GetModuleFileNameW(nullptr, Dll_Path, MAX_PATH);

	wchar_t* Last_Slash = wcsrchr(Dll_Path, L'\\');

	wcscpy_s(Last_Slash + 1, MAX_PATH - (Last_Slash + 1 - Dll_Path), L"Vortex.dll");

	HANDLE Game_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, Process_Identifier);

	if (Game_Process == nullptr)
	{
		wprintf(L"FAILED: cannot open the process (error %lu).\n", GetLastError());

		return 1;
	}

	void* Remote_Path = VirtualAllocEx(Game_Process, nullptr, (MAX_PATH + 1) * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (Remote_Path == nullptr)
	{
		wprintf(L"FAILED: VirtualAllocEx (error %lu).\n", GetLastError());

		CloseHandle(Game_Process);

		return 1;
	}

	WriteProcessMemory(Game_Process, Remote_Path, Dll_Path, (MAX_PATH + 1) * sizeof(wchar_t), nullptr);

	const unsigned __int32 Load_Library = (unsigned __int32)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

	HANDLE Remote_Thread = CreateRemoteThread(Game_Process, nullptr, 0, (LPTHREAD_START_ROUTINE)Load_Library, Remote_Path, 0, nullptr);

	if (Remote_Thread == nullptr)
	{
		wprintf(L"FAILED: CreateRemoteThread (error %lu).\n", GetLastError());

		VirtualFreeEx(Game_Process, Remote_Path, 0, MEM_RELEASE);

		CloseHandle(Game_Process);

		return 1;
	}

	WaitForSingleObject(Remote_Thread, INFINITE);

	DWORD Load_Library_Result = 0;

	GetExitCodeThread(Remote_Thread, &Load_Library_Result);

	VirtualFreeEx(Game_Process, Remote_Path, 0, MEM_RELEASE);

	CloseHandle(Remote_Thread);

	CloseHandle(Game_Process);

	if (Load_Library_Result == 0)
	{
		wprintf(L"FAILED: the DLL was not loaded (LoadLibraryW = NULL).\n");

		return 1;
	}

	wprintf(L"Injection succeeded: %ls (module: 0x%08X)\n", Dll_Path, Load_Library_Result);

	return 0;
}