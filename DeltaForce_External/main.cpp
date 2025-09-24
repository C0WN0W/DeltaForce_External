#include <iostream>
#include <windows.h>

#include"Game/Engine.hpp"
#include"Driver.hpp"
HWND ToolCreateWindow(PVOID Call);
int main()
{
	ProcessMgr.initialize();
	if (!ProcessMgr.ISINSTALL())
	{
		MessageBoxA(0, "Driver ERROR", 0, 0);
	}
	//Create Thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)FlashDataThread, 0, 0, 0));
	ToolCreateWindow(OverlyCall);


	return 0;
}
