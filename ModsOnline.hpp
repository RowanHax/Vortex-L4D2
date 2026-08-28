#pragma once

#include <windows.h>

#include "Internal.hpp"

#include "D3D9.hpp"

struct Mods_Online_Patch
{
	unsigned __int32 Offset;
	unsigned __int8 Known_Original;
	unsigned __int8 Forced;
	unsigned __int8 Original;
};

static Mods_Online_Patch Mods_Online_Patches[] =
{
	{ 0x7E829, 0x74, 0xEB, 0 },
	{ 0x121CBB, 0x7E, 0xEB, 0 },
	{ 0x121992, 0x89, 0x8B, 0 },
	{ 0x128D2B, 0x75, 0xEB, 0 },
	{ 0x5596C, 0x0F, 0x31, 0 },
	{ 0x5596D, 0xB6, 0xC9, 0 },
	{ 0x5596E, 0x4F, 0x90, 0 },
	{ 0x5596F, 0x5D, 0x90, 0 },
};

static const unsigned __int32 Mods_Online_Patch_Count = sizeof(Mods_Online_Patches) / sizeof(Mods_Online_Patches[0]);

static bool Mods_Online_Saved;

static bool Mods_Online_Last_State;

static void Mods_Online_Apply()
{
	if (Engine_Module == nullptr)
	{
		return;
	}

	if (Mods_Online_Saved == false)
	{
		for (unsigned __int32 i = 0; i < Mods_Online_Patch_Count; i++)
		{
			const unsigned __int8 Current = *(unsigned __int8*)((unsigned __int32)Engine_Module + Mods_Online_Patches[i].Offset);

			if (Current == Mods_Online_Patches[i].Forced)
			{
				Mods_Online_Patches[i].Original = Mods_Online_Patches[i].Known_Original;
			}
			else if (Current == Mods_Online_Patches[i].Known_Original)
			{
				Mods_Online_Patches[i].Original = Current;
			}
			else
			{

				Mods_Online_Patches[i].Original = Current;

				Mods_Online_Patches[i].Forced = Current;
			}
		}

		Mods_Online_Saved = true;
	}

	if (Mods_Online_Enabled != Mods_Online_Last_State)
	{
		for (unsigned __int32 i = 0; i < Mods_Online_Patch_Count; i++)
		{
			unsigned __int8* Address = (unsigned __int8*)((unsigned __int32)Engine_Module + Mods_Online_Patches[i].Offset);

			DWORD Previous_Access_Rights;

			VirtualProtect(Address, 1, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);

			*Address = (Mods_Online_Enabled == true) ? Mods_Online_Patches[i].Forced : Mods_Online_Patches[i].Original;

			VirtualProtect(Address, 1, Previous_Access_Rights, &Previous_Access_Rights);
		}

		Mods_Online_Last_State = Mods_Online_Enabled;

	}
}