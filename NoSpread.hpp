#pragma once

#include <windows.h>

#include <math.h>

#include <string.h>

#include "Internal.hpp"

#include "D3D9.hpp"

#include "Sdk.hpp"

extern bool Vortex_No_Spread_Enabled;

typedef float(__cdecl* Vortex_NS_Shared_Random_Float_Type)(const char* Name, float Low, float High, __int32 Seed);

typedef void(__thiscall* Vortex_NS_Update_Spread_Type)(void* Weapon);

static Vortex_NS_Shared_Random_Float_Type Vortex_NS_Shared_Random_Float;

static Vortex_NS_Update_Spread_Type Vortex_NS_Update_Spread;

static __int32* Vortex_NS_Prediction_Seed;

static bool Vortex_No_Spread_Is_Weapon(void* Weapon, unsigned __int32 Local_Player)
{
	__try
	{
		if ((Weapon == nullptr) || (Client_Module == nullptr))
		{
			return false;
		}

		MEMORY_BASIC_INFORMATION Mbi;

		if (VirtualQuery(Weapon, &Mbi, sizeof(Mbi)) == 0)
		{
			return false;
		}

		if ((Mbi.State != MEM_COMMIT) || ((Mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) == 0))
		{
			return false;
		}

		void* Weapon_Vtable = *(void**)Weapon;

		if ((Weapon_Vtable == nullptr) || (Address_In_Client_Module((unsigned __int32)Weapon_Vtable) == false))
		{
			return false;
		}

		void* Networkable = *(void**)((unsigned __int32)Weapon + 8);

		if (Networkable != nullptr)
		{
			void** Vtable = *(void***)Networkable;

			if ((Vtable != nullptr) && (Vtable[1] != nullptr) && (Address_In_Client_Module((unsigned __int32)Vtable[1]) == true))
			{
				using Get_Class_Type = void* (__fastcall*)(void* Ecx, void* Edx);

				void* Client_Class = ((Get_Class_Type)Vtable[1])(Networkable, nullptr);

				if (Client_Class != nullptr)
				{
					const char* Name = *(const char**)((unsigned __int32)Client_Class + 8);

					if ((Name != nullptr) && (Name[0] != 0))
					{
						return true;
					}
				}
			}
		}

		static bool Dumped;

		if (Dumped == false)
		{
			Dumped = true;

			const __int32 Handle = (Local_Player != 0) ? *(__int32*)(Local_Player + 0x1084) : 0;
			void* Entity_List_Weapon = ((Handle & 0xFFF) > 0) ? Sdk_Get_Client_Entity_Safe(Handle & 0xFFF) : nullptr;
			void** Primary_Vtable = *(void***)Weapon;


			Client_Class_Structure* Local_Class = (Local_Player != 0) ? Get_Client_Class((void*)Local_Player) : nullptr;

		}

		void** Primary = *(void***)Weapon;

		if ((Primary == nullptr) || (Sdk_Is_Readable_Range(Primary, 384 * sizeof(void*)) == false) || (Primary[383] == nullptr) || (Address_In_Client_Module((unsigned __int32)Primary[383]) == false))
		{
			return false;
		}

		if (Sdk_Is_Readable_Range((unsigned __int8*)Weapon + 0xD0C, sizeof(float)) == false)
		{
			return false;
		}

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static void Run_Vortex_No_Spread(UserCmd_Structure* Command)
{
	if (Vortex_No_Spread_Enabled == false)
	{
		return;
	}

	if (Command == nullptr)
	{
		return;
	}

	if ((Command->Buttons & 1) == 0)
	{
		return;
	}

	if ((Command->Buttons & 32) != 0)
	{
		return;
	}

	__try
	{
		const unsigned __int32 Local_Player = Get_Local_Player();

		if (Local_Player == 0)
		{
			return;
		}

		if (Command->Random_Seed == 0)
		{
			Command->Command_Number = -2134739495;

			Command->Random_Seed = 11144000;
		}

		if (Vortex_NS_Prediction_Seed != nullptr)
		{
			*Vortex_NS_Prediction_Seed = Command->Random_Seed;
		}

		using Get_Weapon_Type = void*(__thiscall*)(void* Player);

		const Get_Weapon_Type Get_Active_Weapon = (Get_Weapon_Type)((unsigned __int32)Client_Module + 74304);

		void* Weapon = Get_Active_Weapon((void*)Local_Player);

		if ((Weapon == nullptr) || (Vortex_No_Spread_Is_Weapon(Weapon, Local_Player) == false))
		{
			return;
		}

		if ((Vortex_NS_Shared_Random_Float == nullptr) || (Vortex_NS_Update_Spread == nullptr))
		{
			return;
		}

		Vortex_NS_Update_Spread(Weapon);

		const float Maximum_Spread = *(float*)((unsigned __int32)Weapon + 0xD0C);

		if ((Maximum_Spread > 0.f) && (Maximum_Spread < 100.f))
		{
			const float* Recoil = (const float*)(Local_Player + 4612);

			const float Old_Angles[3] = { Command->View_Angles[0], Command->View_Angles[1], Command->View_Angles[2] };

			const float Alea_X = Vortex_NS_Shared_Random_Float("CTerrorGun::FireBullet HorizSpread", -Maximum_Spread, Maximum_Spread, 0);

			const float Alea_Y = Vortex_NS_Shared_Random_Float("CTerrorGun::FireBullet VertSpread", -Maximum_Spread, Maximum_Spread, 0);

			Command->View_Angles[0] -= Alea_X + Recoil[0];

			Command->View_Angles[1] -= Alea_Y + Recoil[1];

			Command->View_Angles[2] -= Recoil[2];
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void Install_Vortex_No_Spread()
{
	Vortex_NS_Shared_Random_Float = (Vortex_NS_Shared_Random_Float_Type)Find_Pattern(L"client.dll",
		"55 8B EC 83 EC 08 A1 ? ? ? ? 53 56 57 8B 7D 14 8D 4D 14 51 89 7D F8 89 45 FC E8 ? ? ? ? 6A 04 8D 55 FC 52 8D 45 14 50 E8 ? ? ? ? 6A 04 8D 4D F8 51 8D 55 14 52 E8 ? ? ? ? 8B 75 08 56 E8 ? ? ? ? 50 8D 45 14 56 50 E8 ? ? ? ? 8D 4D 14 51 E8 ? ? ? ? 8B 15 ? ? ? ? 8B 5D 14 83 C4 30 83 7A 30 00 74 26");

	Vortex_NS_Update_Spread = (Vortex_NS_Update_Spread_Type)Find_Pattern(L"client.dll",
		"53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 28 56 57 8B F9 E8 ? ? ? ? 8B CF 89 45 F0 E8 ? ? ? ? 8B F0 85 F6 75 1B");

	if (Vortex_NS_Shared_Random_Float != nullptr)
	{
		Vortex_NS_Prediction_Seed = (__int32*)(*(unsigned __int32*)((unsigned __int32)Vortex_NS_Shared_Random_Float + 7));
	}

	if ((Vortex_NS_Shared_Random_Float == nullptr) || (Vortex_NS_Update_Spread == nullptr) || (Vortex_NS_Prediction_Seed == nullptr))
	{

		return;
	}

}