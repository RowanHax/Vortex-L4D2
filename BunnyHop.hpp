#pragma once

#include "Internal.hpp"

#include "D3D9.hpp"

#include "RapidFire.hpp"

#include "Aimbot.hpp"

#include "NoSpread.hpp"

#include "Strafe.hpp"

#include "LagExploit.hpp"

#include "SafeRoomTP.hpp"

#include "Hitmarker.hpp"

#include "AntiAim.hpp"

#include "ChatSpammer.hpp"

#include "World.hpp"

typedef bool(__fastcall* CreateMove_Type)(void* Ecx, void* Edx, float Input_Sample_Frametime, UserCmd_Structure* Command);

extern CreateMove_Type Original_CreateMove;

typedef bool(__fastcall* Override_View_Type)(void* Ecx, void* Edx, void* View_Setup);

extern Override_View_Type Original_Override_View;

static void No_Visual_Recoil_Zero_Punch()
{
	static __int32 No_Visual_Recoil_Punch_Offset = -1;

	static __int32 No_Visual_Recoil_Punch_Vel_Offset = -1;

	if (No_Visual_Recoil_Punch_Offset == -1)
	{
		No_Visual_Recoil_Punch_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecPunchAngle");

		No_Visual_Recoil_Punch_Vel_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecPunchAngleVel");

	}

	const unsigned __int32 Local_Player = Get_Local_Player();

	if ((No_Visual_Recoil_Punch_Offset >= 0) && (Local_Player != 0))
	{
		float* Punch = (float*)(Local_Player + No_Visual_Recoil_Punch_Offset);

		Punch[0] = 0.f;

		Punch[1] = 0.f;

		Punch[2] = 0.f;
	}

	if ((No_Visual_Recoil_Punch_Vel_Offset >= 0) && (Local_Player != 0))
	{
		float* Punch_Vel = (float*)(Local_Player + No_Visual_Recoil_Punch_Vel_Offset);

		Punch_Vel[0] = 0.f;

		Punch_Vel[1] = 0.f;

		Punch_Vel[2] = 0.f;
	}
}

typedef float*(__fastcall* Calc_View_Punch_Getter_Type)(void* Ecx, void* Edx);

typedef void(__fastcall* Calc_View_Type)(void* Ecx, void* Edx, void* Eye_Origin, void* Eye_Angles, void* Fov);

static Calc_View_Type Original_Calc_View;

static void __fastcall Calc_View_Hook(void* Ecx, void* Edx, void* Eye_Origin, void* Eye_Angles, void* Fov)
{
	if ((No_Visual_Recoil_Enabled == true) && (Ecx != nullptr))
	{
		__try
		{
			static __int32 Calc_View_Dead_Flag_Offset = -1;

			if (Calc_View_Dead_Flag_Offset == -1)
			{
				Calc_View_Dead_Flag_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "deadflag");
			}

			const bool Alive = ((Calc_View_Dead_Flag_Offset < 0) || (*(unsigned __int8*)((unsigned __int32)Ecx + Calc_View_Dead_Flag_Offset) == 0));

			if (Alive == true)
			{
				static __int32 Calc_View_Punch_Offset = -1;

				if (Calc_View_Punch_Offset == -1)
				{
					Calc_View_Punch_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecPunchAngle");
				}

				if (Calc_View_Punch_Offset >= 0)
				{
					const Calc_View_Punch_Getter_Type Punch_Getter = (Calc_View_Punch_Getter_Type)(*(void***)Ecx)[0x137];

					float* Saved = Punch_Getter(Ecx, nullptr);

					float* Punch = (float*)((unsigned __int32)Ecx + Calc_View_Punch_Offset);

					const float Old[3] = { Saved[0], Saved[1], Saved[2] };

					Punch[0] = 0.f;

					Punch[1] = 0.f;

					Punch[2] = 0.f;

					Original_Calc_View(Ecx, Edx, Eye_Origin, Eye_Angles, Fov);

					Punch[0] = Old[0];

					Punch[1] = Old[1];

					Punch[2] = Old[2];

					return;
				}
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	Original_Calc_View(Ecx, Edx, Eye_Origin, Eye_Angles, Fov);
}

static void Install_Calc_View_Hook()
{
	const unsigned __int32 Calc_View_Address = (unsigned __int32)Find_Pattern(L"client.dll", "55 8B EC 83 EC 1C 53 56 8B F1 8B 0D ? ? ? ? 8B 01 8B 50 38 57 FF D2 84 C0 75 0D");

	if (Calc_View_Address == 0)
	{

		return;
	}

	const unsigned __int8 Expected[6] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x1C };

	bool Prologue_OK = true;

	for (unsigned __int32 i = 0; i < 6; i++)
	{
		if (*(unsigned __int8*)(Calc_View_Address + i) != Expected[i])
		{
			Prologue_OK = false;
		}
	}

	if (Prologue_OK == false)
	{

		return;
	}

	const unsigned __int32 Trampoline = (unsigned __int32)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (Trampoline == 0)
	{
		return;
	}

	memcpy((void*)Trampoline, (void*)Calc_View_Address, 6);

	*(unsigned __int8*)(Trampoline + 6) = 0x68;

	*(unsigned __int32*)(Trampoline + 7) = Calc_View_Address + 6;

	*(unsigned __int8*)(Trampoline + 11) = 0xC3;

	Original_Calc_View = (Calc_View_Type)Trampoline;

	DWORD Previous_Access_Rights;

	VirtualProtect((void*)Calc_View_Address, 5, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);

	*(unsigned __int8*)Calc_View_Address = 0xE9;

	*(unsigned __int32*)(Calc_View_Address + 1) = (unsigned __int32)Calc_View_Hook - (Calc_View_Address + 5);

	VirtualProtect((void*)Calc_View_Address, 5, Previous_Access_Rights, &Previous_Access_Rights);

}

static bool __fastcall Override_View_Hook(void* Ecx, void* Edx, void* View_Setup)
{
	if (No_Visual_Recoil_Enabled == true)
	{
		No_Visual_Recoil_Zero_Punch();
	}

	const bool Result = Original_Override_View(Ecx, Edx, View_Setup);

	World_Update_Fog();

	if (No_Visual_Recoil_Enabled == true)
	{
		const unsigned __int32 Local_Player = Get_Local_Player();

		static __int32 Punch_Offset_2 = -1;

		if (Punch_Offset_2 == -1)
		{
			Punch_Offset_2 = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecPunchAngle");
		}

		if ((View_Setup != nullptr) && (Local_Player != 0) && (Punch_Offset_2 >= 0))
		{
			float* Angles = (float*)((unsigned __int32)View_Setup + 0x48);

			const float* Punch = (const float*)(Local_Player + Punch_Offset_2);

			Angles[0] -= Punch[0];

			Angles[1] -= Punch[1];

			Angles[2] -= Punch[2];
		}
	}

	return Result;
}

static bool __fastcall CreateMove_Hook(void* Ecx, void* Edx, float Input_Sample_Frametime, UserCmd_Structure* Command)
{
	Sdk_Update_Game_Session();

	const bool Result = Original_CreateMove(Ecx, Edx, Input_Sample_Frametime, Command);
	
	Lag_Exploit_Apply(Command);
	Safe_Room_TP_Arm_From_Client(Command);
	Roll_TP_Apply(Command);

	if (Auto_Bhop_Enabled == true)
	{
		if ((Command != nullptr) && ((Command->Buttons & 2) != 0))
		{
			const unsigned __int32 Local_Player = Get_Local_Player();

			if ((Local_Player != 0) && ((*(unsigned __int8*)(Local_Player + Flags_Offset) & FL_ONGROUND) == 0))
			{
				Command->Buttons &= ~2;
			}
		}
	}

	Update_Anti_Aim(Command);

	Update_Vortex_Aimbot(Command);

	Update_Auto_Pistol(Command);

	Update_Air_Strafe(Command);

	Update_Hitmarker(Command);

	Update_Chat_Spammer();

	if (No_Visual_Recoil_Enabled == true)
	{
		No_Visual_Recoil_Zero_Punch();
	}

	Air_Stuck_Apply(Command);

	if ((Command != nullptr) &&
		(((Vortex_Aimbot_Enabled == true) && (Vortex_Aimbot_Silent == true)) ||
		 ((Anti_Aim_Enabled == true) && (Anti_Aim_Silent == true))))
	{
		return false;
	}

	return Result;
}

static void Install_BunnyHop()
{

	void* Client_Mode_Pattern = Find_Pattern(L"client.dll", "89 04 B5 ? ? ? ? E8");

	if (Client_Mode_Pattern == nullptr)
	{

		return;
	}

	void* Client_Mode = **reinterpret_cast<void***>((unsigned __int32)Client_Mode_Pattern + 3);


	if (Client_Mode == nullptr)
	{
		return;
	}

	void** Vtable = *(void***)Client_Mode;


	Original_CreateMove = (CreateMove_Type)Vtable[27];

	DWORD Previous_Access_Rights;

	VirtualProtect(&Vtable[27], sizeof(void*), PAGE_READWRITE, &Previous_Access_Rights);

	Vtable[27] = (void*)CreateMove_Hook;

	VirtualProtect(&Vtable[27], sizeof(void*), Previous_Access_Rights, &Previous_Access_Rights);

	Original_Override_View = (Override_View_Type)Vtable[19];

	VirtualProtect(&Vtable[19], sizeof(void*), PAGE_READWRITE, &Previous_Access_Rights);

	Vtable[19] = (void*)Override_View_Hook;

	VirtualProtect(&Vtable[19], sizeof(void*), Previous_Access_Rights, &Previous_Access_Rights);

	Install_Calc_View_Hook();
}