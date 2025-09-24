#include"../Driver.hpp"
#include"../Game/Global.hpp"
#include"../Game/offset.hpp"
#pragma comment(lib,"ntdll.lib")
#include"../IMGUI/Draw/Draw.h"
using namespace std;
string GetName(ULONG Key)
{
  
    ULONG ChunkOffset;
	ULONG NameOffset;
	ULONG64 NamePoolChunk;
	USHORT NameEntry;
	ULONG NameLength;
	char EncText[1024];
	ULONG Index;
	ChunkOffset = Key >> 18;
	NameOffset = Key & 262143;
	NamePoolChunk = ProcessMgr.Read<uintptr_t>(GameData::Base::ModuleBaseAddress + offset_Gname + ChunkOffset * 8 + 8) + (NameOffset * 2);
	NameEntry = ProcessMgr.Read<USHORT>(NamePoolChunk);
	NameLength = NameEntry >> 6;
	if (NameLength > 256 || NameLength <= 0)
	{
		return "";
	}
	ProcessMgr.readv(NamePoolChunk + 2, (ULONG64)EncText, 1024);
	if (NameEntry & 1)
	{
	}
	else
	{
		if (EncText[0] != 0)
		{
			Index = 0;
			do
			{
				EncText[Index ] ^= 146;
				Index++;
			} while (Index < NameLength);
			EncText[NameLength] = 0;
		}
	}
	//NameLength = strlen((char*)EncText);
	// 
	//EncText[NameLength] = 0;
	return EncText;
}
Vector2 WorldToScreen(Vector3 Pos)
{
	Vector2 ScreenPos = {};
	float CameraZ = GameData::Base::MartixData[0][3] * Pos.x + GameData::Base::MartixData[1][3] * Pos.y + GameData::Base::MartixData[2][3] * Pos.z + GameData::Base::MartixData[3][3];
	if (CameraZ <= 0.01)
	{
		return ScreenPos;
	}
	float Scale = 1 / CameraZ;
	float CameraX = GameData::Info::GameCenterX + (GameData::Base::MartixData[0][0] * Pos.x + GameData::Base::MartixData[1][0] * Pos.y + GameData::Base::MartixData[2][0] * Pos.z + GameData::Base::MartixData[3][0]) * Scale * GameData::Info::GameCenterX;
	float CameraY = GameData::Info::GameCenterY - (GameData::Base::MartixData[0][1] * Pos.x + GameData::Base::MartixData[1][1] * Pos.y + GameData::Base::MartixData[2][1] * (Pos.z - 90) + GameData::Base::MartixData[3][1]) * Scale * GameData::Info::GameCenterY;
	float CameraY2 = GameData::Info::GameCenterY - (GameData::Base::MartixData[0][1] * Pos.x + GameData::Base::MartixData[1][1] * Pos.y + GameData::Base::MartixData[2][1] * (Pos.z + 90) + GameData::Base::MartixData[3][1]) * Scale * GameData::Info::GameCenterY;
	ScreenPos.x = CameraX;
	ScreenPos.y = GameData::Info::GameCenterY - (GameData::Base::MartixData[0][1] * Pos.x + GameData::Base::MartixData[1][1] * Pos.y + GameData::Base::MartixData[2][1] * Pos.z + GameData::Base::MartixData[3][1]) * Scale * GameData::Info::GameCenterY;
	/*ScreenPos.Left = CameraX - (CameraY2 - CameraY) / 4;
	ScreenPos.Top = CameraY;
	ScreenPos.Right = (CameraY2 - CameraY) / 2;
	ScreenPos.Bottom = CameraY2 - CameraY;*/

	return ScreenPos;

}
struct Vector4
{
public:
	Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}

	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

	float x;
	float y;
	float z;
	float w;
};
#ifndef D3DMATRIX_DEFINED
typedef struct _D3DMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
} D3DMATRIX;
#define D3DMATRIX_DEFINED
#endif
struct FRotator
{
public:
	FRotator() : Pitch(0.f), Yaw(0.f), Roll(0.f) {}

	FRotator(float _pitch, float _yaw, float _roll) : Pitch(_pitch), Yaw(_yaw), Roll(_roll) {}

	~FRotator() {}

	float Pitch;
	float Yaw;
	float Roll;

	inline double Length()
	{
		return sqrtf(powf(Pitch, 2.0) + powf(Yaw, 2.0) + powf(Roll, 2.0));
	};

	void Clamp()
	{
		if (Pitch > 180)
			Pitch -= 360;

		else if (Pitch < -180)
			Pitch += 360;

		if (Yaw > 180)
			Yaw -= 360;

		else if (Yaw < -180)
			Yaw += 360;

		if (Pitch < -89)
			Pitch = -89;

		if (Pitch > 89)
			Pitch = 89;

		while (Yaw < -180.0f)
			Yaw += 360.0f;

		while (Yaw > 180.0f)
			Yaw -= 360.0f;

		Roll = 0;
		return;
	}

	Vector3 ToVector()
	{
		float radPitch = (float)(Pitch * 3.1415926f / 180.f);
		float radYaw = (float)(Yaw * 3.1415926f / 180.f);

		float SP = (float)sinf(radPitch);
		float CP = (float)cosf(radPitch);
		float SY = (float)sinf(radYaw);
		float CY = (float)cosf(radYaw);

		return  Vector3(CP * CY, CP * SY, SP);
	}

	D3DMATRIX GetAxes()
	{
		auto tempMatrix = Matrix();
		return tempMatrix;
	}

	D3DMATRIX Matrix(Vector3 origin = Vector3(0, 0, 0))
	{
		float radPitch = (Pitch * 3.1415926f / 180.f);
		float radYaw = (Yaw * 3.1415926f / 180.f);
		float radRoll = (Roll * 3.1415926f / 180.f);
		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	FRotator operator+(FRotator v)
	{
		return FRotator(Pitch + v.Pitch, Yaw + v.Yaw, Roll + v.Roll);
	}

	FRotator operator-(FRotator v)
	{
		return FRotator(Pitch - v.Pitch, Yaw - v.Yaw, Roll - v.Roll);
	}

	FRotator operator*(FRotator v)
	{
		return FRotator(Pitch * v.Pitch, Yaw * v.Yaw, Roll * v.Roll);
	}

	FRotator operator/(FRotator v)
	{
		return FRotator(Pitch / v.Pitch, Yaw / v.Yaw, Roll / v.Roll);
	}
};
struct FTransform
{
public:
	Vector4 Rotation;
	Vector3 Translation;
	char Pad_1[4];
	Vector3 Scale3D;
	char Pad_2[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = Translation.x;
		m._42 = Translation.y;
		m._43 = Translation.z;

		float x2 = Rotation.x + Rotation.x;
		float y2 = Rotation.y + Rotation.y;
		float z2 = Rotation.z + Rotation.z;

		float xx2 = Rotation.x * x2;
		float yy2 = Rotation.y * y2;
		float zz2 = Rotation.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * Scale3D.x;
		m._22 = (1.0f - (xx2 + zz2)) * Scale3D.y;
		m._33 = (1.0f - (xx2 + yy2)) * Scale3D.z;

		float yz2 = Rotation.y * z2;
		float wx2 = Rotation.w * x2;
		m._32 = (yz2 - wx2) * Scale3D.z;
		m._23 = (yz2 + wx2) * Scale3D.y;

		float xy2 = Rotation.x * y2;
		float wz2 = Rotation.w * z2;
		m._21 = (xy2 - wz2) * Scale3D.y;
		m._12 = (xy2 + wz2) * Scale3D.x;

		float xz2 = Rotation.x * z2;
		float wy2 = Rotation.w * y2;
		m._31 = (xz2 + wy2) * Scale3D.z;
		m._13 = (xz2 - wy2) * Scale3D.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

Vector3 GetBonePosition(ULONG_PTR Mesh, INT ID)
{
	ULONG_PTR BoneArry = ProcessMgr.Read<ULONG_PTR>(Mesh + 1792);
	if(!BoneArry)BoneArry = ProcessMgr.Read<ULONG_PTR>(Mesh + 1792+16);
	FTransform Bone = ProcessMgr.Read<FTransform>(BoneArry + (static_cast<unsigned long long>(ID) * 0x30));

	FTransform ComponentToWorld = ProcessMgr.Read<FTransform>(Mesh + 528);

	D3DMATRIX Matrix = MatrixMultiplication(Bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}
namespace Bone
{
	enum Bone : INT
	{
		//---------//
		Õ∑≤ø = 15,
		æ±≤ø = 31,
		//---------//
		◊ÛºÁ = 34,
		”“ºÁ = 6,
		//---------//
		◊Û÷‚ = 35,
		”“÷‚ = 7,
		//---------//
		◊Û ÷ = 36,
		”“ ÷ = 8,
		//---------//
		π«≈Ë = 1,
		//---------//
		◊ÛÕ» = 58,
		”“Õ» = 62,
		//---------//
		◊Ûœ• = 59,
		”“œ• = 63,
		//---------//
		◊ÛΩ≈∏˙ = 60,
		◊ÛΩ≈«∞ = 64,
		”“Ω≈∏˙ = 64,
		”“Ω≈«∞ = 175,
		//---------//
		Ω≈µ◊ = 193,
		…ÌÃÂ = 4,
	};

	//list<INT> _…œ≤ø = { Bone::æ±≤ø, Bone::Õ∑≤ø };
	list<INT> _”“±€ = { Bone::æ±≤ø, Bone::”“ºÁ, Bone::”“÷‚, Bone::”“ ÷ };
	list<INT> _◊Û±€ = { Bone::æ±≤ø, Bone::◊ÛºÁ, Bone::◊Û÷‚, Bone::◊Û ÷ };
	list<INT> _ºπ÷˘ = { Bone::æ±≤ø, 4, 3, 2, Bone::π«≈Ë };
	list<INT> _”“Õ» = { Bone::π«≈Ë, Bone::”“Õ», Bone::”“œ•, Bone::”“Ω≈∏˙ };
	list<INT> _◊ÛÕ» = { Bone::π«≈Ë, Bone::◊ÛÕ», Bone::◊Ûœ•, Bone::◊ÛΩ≈∏˙};
	list<list<INT>> ∆¥Ω”π«˜¿ = {  _”“±€, _◊Û±€, _ºπ÷˘, _”“Õ», _◊ÛÕ» };
}
VOID DrawPlayerBone(ULONG_PTR Mesh, ImVec4 Color, FLOAT Width, BOOL  «∑Òø… ”)
{
	Vector3 NeckPosition = GetBonePosition(Mesh, Bone::æ±≤ø);
	Vector3 WaistPosition = GetBonePosition(Mesh, Bone::π«≈Ë);
	Vector3 previous, current;
	Vector2 p1, c1;
	for (auto i : Bone::∆¥Ω”π«˜¿)
	{
		previous = Vector3(0, 0, 0);
		for (int bone : i)
		{
			current = bone == Bone::æ±≤ø ? NeckPosition : (bone == Bone::π«≈Ë ? WaistPosition : GetBonePosition(Mesh, bone));
			if (previous.x == 0.f)
			{
				previous = current;
				continue;
			}
			//ImVec4 ColorTemp =  «∑Òø… ” && Isitvisible(Memory::Read<Vector3>(GameData::Õ∂”∞œ‡ª˙ + _CameraPos), GameData::CameraCacheData.POV.Location) ? Color : Menu::ÕÊº“Õ∏ ”::Õ∏ ”—’…´::≤ªø… ”;
			p1 = WorldToScreen(previous);
			c1 = WorldToScreen(current);
			draw::Line(p1.x, p1.y, c1.x, c1.y, Color, Width);
			previous = current;
		}
	}
}
BOOL IsVisable(ULONG64 Mesh)
{
	float fvisiontick = 0.07f;
	float fLastSubmitTime = ProcessMgr.Read<float>(Mesh + offset_Eyes);
	float fLastRenderTimeOnScreen = ProcessMgr.Read<float>(Mesh + offset_Eyes + 4);
	return fLastRenderTimeOnScreen + fvisiontick >= fLastSubmitTime;
}
float GetScreenClosestDis(Vector2 pos)
{


	float DistanceX = GameData::Info::GameCenterX - pos.x;
	float DistanceY = GameData::Info::GameCenterY - pos.y;
	return sqrtf(DistanceX * DistanceX + DistanceY * DistanceY);

}
void OverlyCall()
{
	FLOAT CloestDis = 9999999;
	GameData::Base::AimTarget.Actor = 0;
	GameData::Base::AimTarget.Mesh = 0;
	vector<ActorStruct>TempActor;
	TempActor.clear();
	TempActor = GameData::Array::Actors;
	
	Vector3 LocalPlayerPos = ProcessMgr.Read<Vector3>(ProcessMgr.Read<uintptr_t>(GameData::Base::AcknowledgedPawn + offset_RootComponent) + offset_Vector3d);
	//MartixData
	ProcessMgr.readv(GameData::Base::Martix, (ULONG64)&GameData::Base::MartixData, 64);
	DWORD LocalPlayerTeamNum2 = ProcessMgr.Read<DWORD>(ProcessMgr.Read<uintptr_t>(GameData::Base::AcknowledgedPawn + offset_TeamComp) + offset_TeamID+4);

	DWORD LocalPlayerTeamNum = ProcessMgr.Read<DWORD>(ProcessMgr.Read<uintptr_t>(GameData::Base::AcknowledgedPawn + offset_TeamComp) + offset_TeamID);
	for (size_t i = 0; i < TempActor.size(); i++)
	{
		uintptr_t PlayerPtr = TempActor[i].Actor;
		uintptr_t MeshPtr = TempActor[i].Mesh;
		INT Type = TempActor[i].Type;
		DWORD TeamNumber = TempActor[i].TeamNumber;
		DWORD TeamNumber2 = TempActor[i].TeamNumber2;

		char* Name = TempActor[i].Name;
		if (LocalPlayerTeamNum == TeamNumber|| LocalPlayerTeamNum2 == TeamNumber2)continue;
		Vector3 PlayerPos = GetBonePosition(MeshPtr, Bone::æ±≤ø); 

		float Distance = PlayerPos.GetDistance(LocalPlayerPos);
	
		float Health = ProcessMgr.Read<float>(ProcessMgr.Read<uintptr_t>(ProcessMgr.Read<uintptr_t>(PlayerPtr + 0xe30) + 0x240) + 0x48+0x14);
		//printf("%d\n", Health);

		if (Health <= 0)
		{
			Vector2 ScreenPos = WorldToScreen(ProcessMgr.Read<Vector3>(ProcessMgr.Read<uintptr_t>(PlayerPtr + offset_RootComponent) + offset_Vector3d));


			draw::TextCenter(ScreenPos.x, ScreenPos.y, ImVec4(1, 1, 1, 1), 16, Name);


			continue;
		}
		Vector2 ScreenPos = WorldToScreen(PlayerPos);
		if (ScreenPos.x <= 0 && ScreenPos.y <= 0)
		{
			continue;
		}

		//draw::Text(ScreenPos.x, ScreenPos.y , ImVec4(1, 1, 1, 1), 12, "Distance:%s", TempActor[i].ObjName.c_str());
		draw::Text(ScreenPos.x, ScreenPos.y + 24, ImVec4(1, 1, 1, 1), 12, "Health:%.2f", Health);
		
		//
		if (IsVisable(MeshPtr))
		{
			float ScreenDistance = GetScreenClosestDis(ScreenPos);
			if (ScreenDistance < CloestDis && ScreenDistance< Menu::AimDistance)
			{
				CloestDis = ScreenDistance;
				GameData::Base::AimTarget.Actor = TempActor[i].Actor;
				GameData::Base::AimTarget.Mesh = TempActor[i].Mesh;

			}
			if (MeshPtr == GameData::Base::AimTarget.Mesh)
			{
				DrawPlayerBone(MeshPtr, ImVec4(1, 0, 0, 1), 2, 1);

			}
			else
			{
				DrawPlayerBone(MeshPtr, ImVec4(1, 0, 1, 1), 2, 1);

			}
		}
		{
			DrawPlayerBone(MeshPtr, ImVec4(1, 1, 1, 1), 1, 1);

		}





	}
	if (GameData::Base::AimTarget.Actor != 0)
	{
		Vector3 PlayerPos = GetBonePosition(GameData::Base::AimTarget.Mesh, GetAsyncKeyState(VK_SHIFT) ? Bone::æ±≤ø : 1);
		Vector2 ScreenPos = WorldToScreen(PlayerPos);
		if (ScreenPos.x <= 0 && ScreenPos.y <= 0)
		{
			return;
		}
		Menu::AimSmooth;
		Vector2 AimPos = { ScreenPos.x,ScreenPos.y };
		AimPos.x = AimPos.x - GameData::Info::GameCenterX;
		AimPos.y = AimPos.y - GameData::Info::GameCenterY;
		AimPos.x = AimPos.x / 2;
		AimPos.y = AimPos.y / 2;


		if (GetAsyncKeyState(2))
		{
			ProcessMgr.MouseMoveR(AimPos.x, AimPos.y);
		}
	//	draw::Line(GameData::Info::GameCenterX, GameData::Info::GameCenterY, ScreenPos.x, ScreenPos.y, ImVec4(1, 0, 0, 1), 1);
	}
}
void FlashDataThread()
{
	while (GameData::Info::PID == 0)
	{
		GameData::Info::GameWindow = FindWindowA("UnrealWindow", "»˝Ω«÷ﬁ––∂Ø  ");
		GetWindowThreadProcessId(GameData::Info::GameWindow, &GameData::Info::PID);
		Sleep(1000);
	}
	printf("PID:%d\n", GameData::Info::PID);
	ProcessMgr.SetProcessID(GameData::Info::PID);
	while (GameData::Base::ModuleBaseAddress != 0x140000000)
	{
		GameData::Base::ModuleBaseAddress = ProcessMgr.GetModuleBase("DeltaForceClient-Win64-Shipping.exe");
		Sleep(1000);
	}
	GameData::initNameMap();
	while (true)
	{

		GameData::Array::Actors.clear();

		GameData::Base::IsSwapData = 0;
        GameData::Base::Uworld = ProcessMgr.Read<uintptr_t>(GameData::Base::ModuleBaseAddress+offset_Uworld);
		GameData::Base::Ulevel = ProcessMgr.Read<uintptr_t>(GameData::Base::Uworld + offset_ULevel);
		GameData::Base::Count = ProcessMgr.Read<int>(GameData::Base::Ulevel + offset_Count);
		GameData::Base::Actor = ProcessMgr.Read<uintptr_t>(GameData::Base::Ulevel + offset_Acotr);
		GameData::Base::GameInstance = ProcessMgr.Read<uintptr_t>(GameData::Base::Uworld + offset_GameInstance);
		GameData::Base::LocalPlayer = ProcessMgr.Read<uintptr_t>(GameData::Base::GameInstance + offset_LocalPlayer);
		GameData::Base::LocalPlayers = ProcessMgr.Read<uintptr_t>(GameData::Base::LocalPlayer + offset_LocalPlayers);
		GameData::Base::PlayerController = ProcessMgr.Read<uintptr_t>(GameData::Base::LocalPlayers + offset_PlayerController);
		GameData::Base::AcknowledgedPawn = ProcessMgr.Read<uintptr_t>(GameData::Base::PlayerController + offset_AcknowledgedPawn);
		GameData::Base::Martix = ProcessMgr.Read<uintptr_t>(ProcessMgr.Read<uintptr_t>(GameData::Base::ModuleBaseAddress + offset_Matix) + 32) + 640;
		for (int i = 0; i < GameData::Base::Count; i++)
		{
			uintptr_t ObjectIterator = ProcessMgr.Read<uintptr_t>(GameData::Base::Actor + (i * 8));
			if (ObjectIterator == GameData::Base::AcknowledgedPawn)
			{
				continue;
			}
			if (ObjectIterator == 0)
			{
				continue;
			}
			std::string ObjectName = GetName(ProcessMgr.Read<int>(ObjectIterator + offset_ObjectID));
			std::unordered_map<std::string, int>::iterator IDTypeMapIt = std::unordered_map<std::string, int>::iterator();

			//if (!strcmp("BP_DFMCharacter_AI_DT_C", ObjectName.c_str())|| !strcmp("BP_DFMCharacter_C", ObjectName.c_str()) || !strcmp("BP_DFM_Character_Breakthrough_C", ObjectName.c_str()))//BP_DFM_Character_Breakthrough_C
			{
				ActorStruct Temp;
				Temp.Actor = ObjectIterator;
				 ProcessMgr.readv(ProcessMgr.Read<uintptr_t>(ProcessMgr.Read<uintptr_t>(ObjectIterator + offset_PlayerState) + offset_PlayerNamePrivate), (ULONG64)&Temp.Name,64);
				 RtlUnicodeToMultiByteSize((PULONG)&Temp.Name, (PWCH)Temp.Name, 64);
				 //printf("%llx %s\n", Temp.Actor, ObjectName.c_str ());
				//Temp.Name = ObjectName;
				 Temp.TeamNumber2 = ProcessMgr.Read<DWORD>(ProcessMgr.Read<uintptr_t>(ObjectIterator + offset_TeamComp) + offset_TeamID+4);
				 Temp.TeamNumber = ProcessMgr.Read<DWORD>(ProcessMgr.Read<uintptr_t>(ObjectIterator+ offset_TeamComp) + offset_TeamID);
				Temp.Mesh = ProcessMgr.Read<uintptr_t>(ObjectIterator + offset_MeshComponent);
				//Temp.Type = IDTypeMapIt->second;
				//if (!GameData::Base::IsSwapData)break;
				//Temp.ObjName = ObjectName;
				GameData::Array::Actors.push_back(Temp);

				continue;

			}

			//printf(" %s\n", ObjectName.c_str());


			//printf("%s\n", ObjectName.c_str());
			/*if (ProcessMgr.ActorFilter(ObjectName))
			{
				GameData::Base::MeshArray.push_back(ObjectIterator);
			}*/
		}
		GameData::Base::IsSwapData = 1;
		Sleep(3000);
	}
}

