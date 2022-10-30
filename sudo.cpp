#include <iostream>
#include <sstream>
#include <windows.h>
#include <shlobj.h>

using namespace std;

string join_params(int argc, const char* argv[])
{
	stringstream params;
	for (int i = 1; i < argc; ++i)
	{
		params << argv[i] << ' ';
	}
	return params.str();
}

int main(int argc, const char* argv[])
{
	if (argc < 2)
	{
		cout << "usage: sudo [command]" << endl;
		return 1;
	}
	string params = join_params(argc, argv);
	if (!IsUserAnAdmin())
	{
		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		SHELLEXECUTEINFOA sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOA);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
		sei.hwnd = GetConsoleWindow();
		sei.lpVerb = "runas";
		sei.lpFile = path;
		sei.lpParameters = params.c_str();
		sei.nShow = SW_HIDE;
		if (!ShellExecuteExA(&sei))
		{
			DWORD lastError = GetLastError();
			if (lastError == ERROR_CANCELLED)
			{
				cout << "Cancelled" << endl;
			}
			else
			{
				cout << "ShellExecuteExA failed: " << lastError << endl;
			}
			return 1;
		}
		WaitForSingleObject(sei.hProcess, INFINITE);
		CloseHandle(sei.hProcess);
	}
	else
	{
		FreeConsole();
		AttachConsole(ATTACH_PARENT_PROCESS);
		STARTUPINFO si = {};
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi = {};
		if (!CreateProcess(NULL, (char*)params.c_str(), NULL, NULL, true, 0, NULL, NULL, &si, &pi))
		{
			cout << "CreateProcess failed: " << GetLastError() << endl;
			return 1;
		}
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}
