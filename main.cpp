#include <tchar.h>
#include <windows.h>
#include <tlhelp32.h>

typedef LONG (NTAPI *_NtSuspendProcess)(HANDLE);
typedef LONG (NTAPI *_NtResumeProcess)(HANDLE);
_NtSuspendProcess NtSuspendProcess;
_NtResumeProcess NtResumeProcess;

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hDll;
	HANDLE hProcess;
	HANDLE hSnapshot;
	PROCESSENTRY32 pe;
	DWORD ExitCode;
	hDll = GetModuleHandle(_T("ntdll.dll"));
	NtSuspendProcess = (_NtSuspendProcess)GetProcAddress(hDll, "NtSuspendProcess");
	NtResumeProcess = (_NtResumeProcess)GetProcAddress(hDll, "NtResumeProcess");
	while(true)
	{
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(hSnapshot != INVALID_HANDLE_VALUE)
		{
			memset(&pe, 0, sizeof(PROCESSENTRY32));
			pe.dwSize = sizeof(PROCESSENTRY32);
			if(Process32First(hSnapshot, &pe))
			{
				do
				{
					if(_tcsicmp(pe.szExeFile, _T("RazerCentralService.exe")) == 0)
					{
						break;
					}
				}
				while(Process32Next(hSnapshot, &pe));
			}
			CloseHandle(hSnapshot);
			if(pe.th32ProcessID != 0)
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
				if(hProcess != NULL)
				{
					while(GetExitCodeProcess(hProcess, &ExitCode) && ExitCode == STATUS_PENDING)
					{
						NtSuspendProcess(hProcess);
						Sleep(500);
						NtResumeProcess(hProcess);
						Sleep(10);
					}
					CloseHandle(hProcess);
				}
			}
		}
		Sleep(500);
	}
	return 0;
}

