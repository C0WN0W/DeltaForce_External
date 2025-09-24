#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <Tlhelp32.h>
#include <atlconv.h>
#define _is_invalid(v) if(v==NULL) return false
#define _is_invalid(v,n) if(v==NULL) return n
#include<iostream>
#include<winioctl.h>
#include <string>
#include <winnt.h>
#include <winternl.h>


#define IOCTL_KERNEL_ISINSTALL      10000
#define IOCTL_KERNEL_READMEMORY		10001
#define IOCTL_KERNEL_WRITEMEMORY	10002
#define IOCTL_KERNEL_GETMODULE      10003
#define IOCTL_KERNEL_MOUSE			10004
#define IOCTL_KERNEL_KEYBOARD       10005
#define IOCTL_KERNEL_HARDWARE       10006
#define IOCTL_KERNEL_SetPROCESS       10007
#define IOCTL_KERNEL_ProtectWindow       10008

#define IOCTL_KERNEL_WindowDisplayAffinity       10009
#define IOCTL_KERNEL_PUBGInit       10010
#define IOCTL_KERNEL_PUBGDecrypt       10011
#define IOCTL_KERNEL_ProtectProcess       10012
#define IOCTL_KERNEL_KeDeleteFile       10013
#define IOCTL_KERNEL_KeCreatThread       10014
#define IOCTL_KERNEL_KeEmptyDebugPort       10015
#define IOCTL_KERNEL_KeShiftReadMode       10016
#define IOCTL_KERNEL_KeGetProcessIdByName       10017
#define IOCTL_KERNEL_KeVerifyCard 10018
#define IOCTL_KERNEL_KeNoneVerifyCard 10019

enum StatusCode
{
	SUCCEED,
	FAILE_PROCESSID,
	FAILE_HPROCESS,
	FAILE_MODULE,
};
typedef struct _DataStruct {
	ULONG   Token;
	ULONG	pid;
	PVOID	addr;
	PVOID	buff;
	ULONG	size;
} DataStruct, * PDataStruct;
extern BOOL  DrvCom(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped);

class ProcessManager
{
private:

	bool   Attached = false;
	HANDLE Dr_Handle;
public:

	HANDLE hProcess = 0;
	DWORD  ProcessID = 0;
	DWORD64  ModuleAddress = 0;

public:
	~ProcessManager()
	{
		//if (hProcess)
			//CloseHandle(hProcess);
	}

	int initialize()
	{
		Dr_Handle = (HANDLE)999;
		return (int)Dr_Handle;
	}
	void ProtectProcess(DWORD PID, BOOL Flag)
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		_DataStruct  msgInfo = { NULL };
		msgInfo.pid = PID;

		msgInfo.size = Flag;
		msgInfo.Token = IOCTL_KERNEL_ProtectProcess;
		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);

		return;
	}

	BOOL SetEProcessByName(char* Name)
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return 1;
		}

		_DataStruct  msgInfo = { NULL };
		msgInfo.Token = IOCTL_KERNEL_KeGetProcessIdByName;

		ULONG64 VV = 0;
		msgInfo.addr = &VV;
		msgInfo.buff = Name;
		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);
		if (VV)
		{

			return 1;
		}
		return 0;

	}

	void MouseMoveR(int x, int y)
	{
		struct
		{
			USHORT UnitId;
			USHORT Flags;
			union {
				ULONG Buttons;
				struct {
					USHORT  ButtonFlags;
					USHORT  ButtonData;
				};
			};
			ULONG RawButtons;
			LONG LastX;

			LONG LastY;


			ULONG ExtraInformation;

		} Struct;

		memset(&Struct, NULL, sizeof(Struct));


		Struct.Flags = (USHORT)0;

		Struct.ButtonFlags = (USHORT)0;

		Struct.LastX = x;

		Struct.LastY = y;

		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		_DataStruct  msgInfo = { NULL };
		msgInfo.pid = 0;
		msgInfo.addr = (PVOID)0;
		msgInfo.buff = (PVOID)&Struct;
		msgInfo.size = 0;
		msgInfo.Token = IOCTL_KERNEL_MOUSE;

		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), 0, NULL);
		return;
	}
	ULONG64 GetModuleBase(std::string ModuleName)
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return 0;
		}
		char A[255];
		strcpy_s(A, ModuleName.c_str());
		BOOL		Result = FALSE;
		_DataStruct  msgInfo = { NULL };
		ULONG64 Base;
		msgInfo.buff = (PVOID)&A;
		msgInfo.addr = &Base;
		msgInfo.Token = IOCTL_KERNEL_GETMODULE;

		DWORD FUCK = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &FUCK, NULL);

		return (ULONG64)Base;
	}

	void VerifyCard(char* SendBuffer, int Sendsize, char* RecvBuffer, int Rcvsize)
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return;
		}
		ULONG64 VV = 0;
		_DataStruct  msgInfo = { NULL };
		msgInfo.Token = IOCTL_KERNEL_KeVerifyCard;
		msgInfo.buff = SendBuffer;
		msgInfo.pid = Sendsize;
		msgInfo.addr = RecvBuffer;
		msgInfo.size = Rcvsize;
		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);

	}
	template<typename T> T Read(ULONG_PTR dwAddress)
	{
		T Buffer = T();

		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return Buffer;
		}
		BOOL		Result = FALSE;
		_DataStruct  msgInfo = { NULL };
		msgInfo.addr = (PVOID)dwAddress;
		msgInfo.size = sizeof(T);
		msgInfo.buff = &Buffer;
		msgInfo.Token = IOCTL_KERNEL_READMEMORY;
		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);
		return Buffer;
	}
	BOOL readv(ULONG_PTR dwAddress, ULONG64 buffer, int size)
	{


		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return 0;
		}
		BOOL		Result = FALSE;
		_DataStruct  msgInfo = { NULL };
		msgInfo.addr = (PVOID)dwAddress;
		msgInfo.size = size;
		msgInfo.Token = IOCTL_KERNEL_READMEMORY;

		msgInfo.buff = (PVOID)buffer;
		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);

		return 1;
	}

	BOOL SetReadWay()
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return 1;
		}
		ULONG64 VV = 0;
		_DataStruct  msgInfo = { NULL };
		msgInfo.Token = IOCTL_KERNEL_KeNoneVerifyCard;
		msgInfo.buff = &VV;
		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);

		return VV == 888 ? 1 : 0;
	}
	BOOL ISINSTALL()
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return 1;
		}
		ULONG64 VV = 0;
		_DataStruct  msgInfo = { NULL };
		msgInfo.Token = IOCTL_KERNEL_ISINSTALL;
		msgInfo.buff = &VV;
		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);

		return VV == 888 ? 1 : 0;
	}
	ULONG64 SetProcessID(ULONG64 PID)
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return 1;
		}

		_DataStruct  msgInfo = { NULL };
		msgInfo.pid = PID;
		msgInfo.Token = IOCTL_KERNEL_SetPROCESS;

		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);
		return 1;
	}
	void ProtectWindow(HWND hwnd, ULONG64 flag)
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		_DataStruct  msgInfo = { NULL };
		msgInfo.pid = (ULONG)hwnd;
		msgInfo.buff = (PVOID)flag;
		msgInfo.addr = (PVOID)GetDesktopWindow();

		msgInfo.Token = IOCTL_KERNEL_ProtectWindow;

		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);		return;
	}


	void KernelDisplayAffinity(HWND hwnd, INT Flag)
	{
		if (Dr_Handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		_DataStruct  msgInfo = { NULL };
		msgInfo.pid = (ULONG)hwnd;
		msgInfo.buff = PVOID(Flag);
		msgInfo.Token = IOCTL_KERNEL_WindowDisplayAffinity;

		DWORD A = 0;
		DrvCom(Dr_Handle, &msgInfo, sizeof(msgInfo), &A, NULL);
		return;

	}

	template <typename ReadType>
	bool ReadMemory(DWORD64 Address, ReadType& Value, int Size)
	{

		readv(Address, (ULONG64)&Value, Size);
		return true;
	}

	template <typename ReadType>
	bool ReadMemory(DWORD64 Address, ReadType& Value)
	{

		readv(Address, (ULONG64)&Value, sizeof(ReadType));

		return true;
	}

	template <typename ReadType>
	bool WriteMemory(DWORD64 Address, ReadType& Value, int Size)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		/*if (WriteProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0))
			return true;*/
		return false;
	}

	template <typename ReadType>
	bool WriteMemory(DWORD64 Address, ReadType& Value)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		/*if (WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(Address), &Value, sizeof(ReadType), 0))
			return true;*/
		return false;
	}

	std::vector<DWORD64> SearchMemory(const std::string& Signature, DWORD64 StartAddress, DWORD64 EndAddress, int SearchNum = 1);
	DWORD64 TraceAddress(DWORD64 BaseAddress, std::vector<DWORD> Offsets)
	{
		_is_invalid(hProcess, 0);
		_is_invalid(ProcessID, 0);
		DWORD64 Address = 0;

		if (Offsets.size() == 0)
			return BaseAddress;

		if (!ReadMemory<DWORD64>(BaseAddress, Address))
			return 0;
		for (int i = 0; i < Offsets.size() - 1; i++)
		{
			if (!ReadMemory<DWORD64>(Address + Offsets[i], Address))
				return 0;
		}
		return Address == 0 ? 0 : Address + Offsets[Offsets.size() - 1];
	}

public:

	DWORD GetProcessID(std::string ProcessName)
	{
		PROCESSENTRY32 ProcessInfoPE;
		ProcessInfoPE.dwSize = sizeof(PROCESSENTRY32);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(15, 0);
		Process32First(hSnapshot, &ProcessInfoPE);
		USES_CONVERSION;
		do {
			if (strcmp(W2A(ProcessInfoPE.szExeFile), ProcessName.c_str()) == 0)
			{
				CloseHandle(hSnapshot);
				return ProcessInfoPE.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &ProcessInfoPE));
		CloseHandle(hSnapshot);
		return 0;
	}

	HMODULE GetProcessModuleHandle(std::string ModuleName)
	{
		MODULEENTRY32 ModuleInfoPE;
		ModuleInfoPE.dwSize = sizeof(MODULEENTRY32);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->ProcessID);
		Module32First(hSnapshot, &ModuleInfoPE);
		USES_CONVERSION;
		do {
			if (strcmp(W2A(ModuleInfoPE.szModule), ModuleName.c_str()) == 0)
			{
				CloseHandle(hSnapshot);
				return ModuleInfoPE.hModule;
			}
		} while (Module32Next(hSnapshot, &ModuleInfoPE));
		CloseHandle(hSnapshot);
		return 0;
	}
};

inline ProcessManager ProcessMgr;