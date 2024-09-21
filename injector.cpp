#include <iostream>
#include <Windows.h>
#include <VersionHelpers.h>
#include <Psapi.h>
#include <atlstr.h>
#include <string>

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)


BOOL InjectDLL(DWORD ProcessID)
{
	LPCSTR DLL_PATH = "C:\\Users\\mishe\\source\\repos\\dllinject\\x64\\Debug\\dllinject.dll";
	LPVOID LoadLibAddy, RemoteString;

	if (!ProcessID)
		return FALSE;
	HANDLE Proc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, ProcessID);

	if (!Proc)
	{
		std::cout << "OpenProcess() failed: " << GetLastError() << std::endl;
		return FALSE;
	}
	LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

	RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(DLL_PATH) + 1, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(Proc, RemoteString, (LPVOID)DLL_PATH, strlen(DLL_PATH) + 1, NULL);
	CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, RemoteString, NULL, NULL);

	CloseHandle(Proc);
	
	return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwThreadID, dwProcessId;
	HINSTANCE hInstance;
	char String[255];
	if (!hWnd) 
		return TRUE; // Not a window
	if(!::IsWindowVisible(hWnd))
		return TRUE; // Not visible
	if (!SendMessage(hWnd, WM_GETTEXT, sizeof(String), (LPARAM)String))
		return TRUE; // No window title
	hInstance = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
	dwThreadID = GetWindowThreadProcessId(hWnd, &dwProcessId);
	std::cout << "PID: " << dwProcessId << '\t' << String << '\t' << std::endl;
	return TRUE;
}

int main() {
	if (IsWindowsXPOrGreater) {
		std::cout << "Available Targets:\n\n" << std::endl;
		EnumWindows(EnumWindowsProc, NULL);
		std::cout << "\n Pick target ProcessID" << std::endl;
		DWORD PID;
		std::cin >> PID;
		InjectDLL(PID);
	}
	else {
		std::cout << "os version not supported" << std::endl;
		return 0;
	}


	return 0;
}