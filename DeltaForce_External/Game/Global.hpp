#pragma once
#include<windows.h>
#include <unordered_map>
#include<vector>
#include <string>
class Vector2
{
public:
	float x, y;

};

class Vector3
{
public:
	float x, y, z;
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3() : x(0.f), y(0.f), z(0.f) {}

	float GetDistance(Vector3 b)
	{
		
		return sqrt(pow(this->x - b.x, 2) + pow(this->y - b.y, 2) + pow(this->z - b.z, 2)/100);

	}
};
using namespace std;
class ActorStruct
{
public:
	uintptr_t Actor;
	uintptr_t Mesh;
	INT Type;
	DWORD TeamNumber;
	DWORD TeamNumber2;

	char Name[64] ;
	string ObjName;
};
class AimStruct	
{
public:
	uintptr_t Actor;
	uintptr_t Mesh;
	
};
typedef enum : ULONG
{
	Player = 0,
	Exit = 1,
	AI = 2,
	Box = 3,
}ClassType;
inline namespace Menu
{
	float AimFov = 100.f;
	float AimDistance = 100.f;
	float AimSmooth = 0.1f;
};
inline namespace GameData 
{
	std::unordered_map<std::string, INT> NameMap = {};
	namespace Array
	{
		vector<ActorStruct> Actors;
		vector<uintptr_t> Iteams;

	}

	void initNameMap()
	{
		NameMap["BP_DFMCharacter_C"] = Player;
		NameMap["BP_FXProxy_Exit_PC_C"] = Exit;
		NameMap["BP_DFMCharacter_AI_DT_C"] = AI;
		NameMap["BP_Inventory_CarryBody_C"] = Box;

	}
	namespace Base
	{
		uintptr_t ModuleBaseAddress = 0;
		AimStruct AimTarget;
		uintptr_t Uworld = 0;
        uintptr_t Ulevel = 0;
		uintptr_t Count = 0;
		uintptr_t Actor = 0;
		uintptr_t GameInstance = 0;
		uintptr_t LocalPlayer = 0;
		uintptr_t LocalPlayers = 0;
		uintptr_t PlayerController = 0;
		uintptr_t AcknowledgedPawn = 0;
		uintptr_t Martix = 0;
		float MartixData[4][4];
		BOOL IsSwapData = 0;


	}

	namespace Info
	{
		HWND GameWindow = NULL;
		HWND OverlyWindow = NULL;
		DWORD PID = 0;
		DWORD GameCenterX;
		DWORD GameCenterY;

	}
}
