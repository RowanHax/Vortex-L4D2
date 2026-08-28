#pragma once

#include <cstdio>

#include <windows.h>

#include "Internal.hpp"

struct Config_Entry
{
	const char* Name;

	void* Address;

	__int32 Type;
};

static Config_Entry Config_Table[] =
{
	{ "Auto_Bhop_Enabled", (void*)&Auto_Bhop_Enabled, 0 },

	{ "Strafe_Enabled", (void*)&Strafe_Enabled, 0 },

	{ "Esp_Enabled", (void*)&Esp_Enabled, 0 },

	{ "Esp_Team2", (void*)&Esp_Team2, 0 },

	{ "Esp_Team3", (void*)&Esp_Team3, 0 },

	{ "Esp_Commons", (void*)&Esp_Commons, 0 },

	{ "Esp_Names", (void*)&Esp_Names, 0 },

	{ "Esp_Distance", (void*)&Esp_Distance, 0 },

	{ "Esp_Health_Bar", (void*)&Esp_Health_Bar, 0 },

	{ "Esp_Snap_Lines", (void*)&Esp_Snap_Lines, 0 },


	{ "Esp_Box_Style", (void*)&Esp_Box_Style, 1 },

	{ "Esp_Team2_Color", (void*)Esp_Team2_Color, 3 },

	{ "Esp_Team3_Color", (void*)Esp_Team3_Color, 3 },

	{ "Watermark_Enabled", (void*)&Watermark_Enabled, 0 },

	{ "No_Vomit_Enabled", (void*)&No_Vomit_Enabled, 0 },

	{ "Chat_Spy_Enabled", (void*)&Chat_Spy_Enabled, 1 },

	{ "Chat_Spy_Method", (void*)&Chat_Spy_Method, 0 },

	{ "Mods_Online_Enabled", (void*)&Mods_Online_Enabled, 0 },

	{ "Chams_Enabled", (void*)&Chams_Enabled, 0 },

	{ "Chams_X_Ray", (void*)&Chams_X_Ray, 0 },

	{ "Chams_Style", (void*)&Chams_Style, 0 },

	{ "Chams_Team2_Color", (void*)Chams_Team2_Color, 3 },

	{ "Chams_Team3_Color", (void*)Chams_Team3_Color, 3 },

	{ "Rapid_Fire_Enabled", (void*)&Rapid_Fire_Enabled, 0 },

	{ "Tick_Manipulation_Enabled", (void*)&Tick_Manipulation_Enabled, 0 },

	{ "Tick_Manipulation_Interact", (void*)&Tick_Manipulation_Interact, 0 },

	{ "Tick_Manipulation_Ticks", (void*)&Tick_Manipulation_Ticks, 1 },

	{ "Tick_Base_Fix_Enabled", (void*)&Tick_Base_Fix_Enabled, 0 },

	{ "Tick_Base_Correction_Msecs", (void*)&Tick_Base_Correction_Msecs, 1 },

	{ "No_Visual_Recoil_Enabled", (void*)&No_Visual_Recoil_Enabled, 0 },

	{ "Vortex_Aimbot_Enabled", (void*)&Vortex_Aimbot_Enabled, 0 },

	{ "Vortex_Aimbot_Hitbox", (void*)&Vortex_Aimbot_Hitbox, 1 },

	{ "Vortex_Aimbot_Fov", (void*)&Vortex_Aimbot_Fov, 2 },

	{ "Vortex_Aimbot_Smooth", (void*)&Vortex_Aimbot_Smooth, 2 },

	{ "Vortex_Aimbot_Distance", (void*)&Vortex_Aimbot_Distance, 2 },

	{ "Vortex_Aimbot_Silent", (void*)&Vortex_Aimbot_Silent, 0 },

	{ "Vortex_Aimbot_Visible", (void*)&Vortex_Aimbot_Visible, 0 },

	{ "Vortex_Aimbot_Auto_Fire", (void*)&Vortex_Aimbot_Auto_Fire, 0 },

	{ "Vortex_Aimbot_Prediction", (void*)&Vortex_Aimbot_Prediction, 0 },

	{ "Vortex_Aimbot_Prioritize_Players", (void*)&Vortex_Aimbot_Prioritize_Players, 0 },

	{ "Vortex_Aimbot_Ignore_Common", (void*)&Vortex_Aimbot_Ignore_Common, 0 },

	{ "Vortex_Aimbot_Ignore_Tank", (void*)&Vortex_Aimbot_Ignore_Tank, 0 },

	{ "Vortex_Aimbot_Ignore_Witch", (void*)&Vortex_Aimbot_Ignore_Witch, 0 },

	{ "Vortex_No_Spread_Enabled", (void*)&Vortex_No_Spread_Enabled, 0 },

	{ "Lag_Exploit_Enabled", (void*)&Lag_Exploit_Enabled, 0 },

	{ "Lag_Exploit_Key", (void*)&Lag_Exploit_Key, 1 },

	{ "Lag_Exploit_Value", (void*)&Lag_Exploit_Value, 1 },

	{ "Air_Stuck_Enabled", (void*)&Air_Stuck_Enabled, 0 },

	{ "Air_Stuck_Key", (void*)&Air_Stuck_Key, 1 },

	{ "Charger_Turn_Enabled", (void*)&Charger_Turn_Enabled, 0 },

};

static const __int32 Config_Entry_Count = sizeof(Config_Table) / sizeof(Config_Table[0]);

static const char Config_Directory[] = "C:\\Vortex";

static bool Save_Config(const char* Name)
{
	CreateDirectoryA(Config_Directory, nullptr);

	char Path[MAX_PATH];

	sprintf_s(Path, "%s\\%s.cfg", Config_Directory, Name);

	FILE* File = nullptr;

	fopen_s(&File, Path, "w");

	if (File == nullptr)
	{
		return false;
	}

	for (__int32 i = 0; i < Config_Entry_Count; i++)
	{
		const Config_Entry& Entry = Config_Table[i];

		if (Entry.Type == 0)
		{
			fprintf(File, "%s = %d\n", Entry.Name, (int)*(bool*)Entry.Address);
		}
		else if (Entry.Type == 1)
		{
			fprintf(File, "%s = %d\n", Entry.Name, *(int*)Entry.Address);
		}
		else if (Entry.Type == 2)
		{
			fprintf(File, "%s = %f\n", Entry.Name, *(float*)Entry.Address);
		}
		else
		{
			const float* Color = (const float*)Entry.Address;

			fprintf(File, "%s = %f %f %f %f\n", Entry.Name, Color[0], Color[1], Color[2], Color[3]);
		}
	}

	fclose(File);

	return true;
}

static bool Load_Config(const char* Name)
{
	char Path[MAX_PATH];

	sprintf_s(Path, "%s\\%s.cfg", Config_Directory, Name);

	FILE* File = nullptr;

	fopen_s(&File, Path, "r");

	if (File == nullptr)
	{
		return false;
	}

	char Line[256];

	while (fgets(Line, sizeof(Line), File) != nullptr)
	{
		char Key[64];

		if (sscanf_s(Line, "%63s", Key, (unsigned)sizeof(Key)) != 1)
		{
			continue;
		}

		if (strcmp(Key, "Air_Strafe_Enabled") == 0)
		{
			int Value = 0;
			sscanf_s(Line, "%*s = %d", &Value);
			Strafe_Enabled = (Value != 0);
			continue;
		}

		if ((strcmp(Key, "Strafe_Hannibal_Enabled") == 0) ||
			(strcmp(Key, "Air_Strafe_Enabled") == 0))
		{
			int Value = 0;
			sscanf_s(Line, "%*s = %d", &Value);
			Strafe_Enabled = (Value != 0);
			continue;
		}

		if ((strcmp(Key, "Chat_Spy_Mode") == 0) || (strcmp(Key, "Chat_Spy_Overlay") == 0))
		{
			int Value = 0;
			sscanf_s(Line, "%*s = %d", &Value);
			Chat_Spy_Enabled = (Value != 0);
			continue;
		}

		for (__int32 i = 0; i < Config_Entry_Count; i++)
		{
			const Config_Entry& Entry = Config_Table[i];

			if (strcmp(Key, Entry.Name) != 0)
			{
				continue;
			}

			if (Entry.Type == 0)
			{
				int Value = 0;

				sscanf_s(Line, "%*s = %d", &Value);

				*(bool*)Entry.Address = (Value != 0);
			}
			else if (Entry.Type == 1)
			{
				int Value = 0;

				sscanf_s(Line, "%*s = %d", &Value);

				*(int*)Entry.Address = Value;
			}
			else if (Entry.Type == 2)
			{
				float Value = 0.f;

				sscanf_s(Line, "%*s = %f", &Value);

				*(float*)Entry.Address = Value;
			}
			else
			{
				float* Color = (float*)Entry.Address;

				sscanf_s(Line, "%*s = %f %f %f %f", &Color[0], &Color[1], &Color[2], &Color[3]);
			}

			break;
		}
	}

	fclose(File);

	return true;
}

static void List_Configs(char Names[][64], __int32 Max, __int32* Count)
{
	*Count = 0;

	WIN32_FIND_DATAA Data;

	HANDLE Find = FindFirstFileA("C:\\Vortex\\*.cfg", &Data);

	if (Find == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if ((Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			continue;
		}

		char* Dot = strrchr(Data.cFileName, '.');

		if (Dot == nullptr)
		{
			continue;
		}

		*Dot = 0;

		if (*Count < Max)
		{
			strcpy_s(Names[*Count], 64, Data.cFileName);

			(*Count)++;
		}
	} while (FindNextFileA(Find, &Data) != 0);

	FindClose(Find);
}

static void Delete_Config(const char* Name)
{
	char Path[MAX_PATH];

	sprintf_s(Path, "%s\\%s.cfg", Config_Directory, Name);

	DeleteFileA(Path);
}