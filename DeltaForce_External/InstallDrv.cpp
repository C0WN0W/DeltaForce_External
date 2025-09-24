#include"Driver.hpp"
typedef NTSTATUS(WINAPI* PNtDrvCom)(__int64 a1, __int64 a2, unsigned int a3, __int64 a4, __int64 a5
	);

PNtDrvCom NtUserGetCurrentDpiInfoForWindow = NULL;

BOOL __stdcall DrvCom(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{

	if (NtUserGetCurrentDpiInfoForWindow)
	{
		//FuncIndex = NtUserGetCurrentDpiInfoForWindow;
		return NtUserGetCurrentDpiInfoForWindow((__int64)lpBuffer, 0, 0, 0, 0);

	}
	else
	{


		 NtUserGetCurrentDpiInfoForWindow = (PNtDrvCom)GetProcAddress(GetModuleHandleA(("win32u.dll")), ("NtUserDragObject"));

		//memcpy(&NtUserGetCurrentDpiInfoForWindow, (void*)&TempCom[4], 4);
		//FuncIndex = NtUserGetCurrentDpiInfoForWindow;
		return NtUserGetCurrentDpiInfoForWindow((__int64)lpBuffer, 0, 0, 0, 0);
	}

	return 0;
}