#pragma once

#include <windows.h>

#include <stdio.h>

#include "Internal.hpp"

#include "Sdk.hpp"

extern bool Name_Stealer_Enabled;

typedef void(__fastcall* Name_Stealer_Client_Cmd_Type)(void* Ecx, void* Edx, const char* Command_Text);

typedef void* (__fastcall* Name_Stealer_Get_Client_Entity_Type)(void* Ecx, void* Edx, __int32 Index);

static void* Name_Stealer_Engine_Client;

static Name_Stealer_Client_Cmd_Type Name_Stealer_Client_Cmd;

static bool Name_Stealer_Resolve_Tried;

static void* Name_Stealer_Entity_List;

static Name_Stealer_Get_Client_Entity_Type Name_Stealer_Get_Client_Entity;

static bool Name_Stealer_Entity_Tried;

static __int32 Name_Stealer_Stolen_Ids[32];

static __int32 Name_Stealer_Stolen_Count;

static unsigned __int32 Name_Stealer_Last_Change_Tick;

static bool Name_Stealer_Already_Stolen(__int32 User_ID)
{
	for (__int32 i = 0; i < Name_Stealer_Stolen_Count; i++)
	{
		if (Name_Stealer_Stolen_Ids[i] == User_ID)
		{
			return true;
		}
	}

	return false;
}

static void Name_Stealer_Update()
{
	__try
	{
		if (Name_Stealer_Enabled == false)
		{
			Name_Stealer_Stolen_Count = 0;

			return;
		}

		if (Sdk_Game_Is_Ready() == false)
		{
			return;
		}

		if (Name_Stealer_Resolve_Tried == false)
		{
			Name_Stealer_Resolve_Tried = true;

			Name_Stealer_Engine_Client = Get_Interface(Engine_Module, "VEngineClient013");

			if (Name_Stealer_Engine_Client != nullptr)
			{
				void** Vtable = *(void***)Name_Stealer_Engine_Client;

				if ((Vtable != nullptr) && (Sdk_Is_Readable_Range(Vtable, 8 * sizeof(void*)) == true) &&
					(Vtable[7] != nullptr) && (Sdk_Address_In_Module(Engine_Module, (unsigned __int32)Vtable[7]) == true))
				{
					Name_Stealer_Client_Cmd = (Name_Stealer_Client_Cmd_Type)Vtable[7];
				}
			}
		}

		if ((Name_Stealer_Engine_Client == nullptr) || (Name_Stealer_Client_Cmd == nullptr))
		{
			return;
		}

		if (Name_Stealer_Entity_Tried == false)
		{
			Name_Stealer_Entity_Tried = true;

			Name_Stealer_Entity_List = Get_Interface(Client_Module, "VClientEntityList003");

			if (Name_Stealer_Entity_List != nullptr)
			{
				void** Vtable = *(void***)Name_Stealer_Entity_List;

				if ((Vtable != nullptr) && (Sdk_Is_Readable_Range(Vtable, 4 * sizeof(void*)) == true) &&
					(Vtable[3] != nullptr) && (Sdk_Address_In_Module(Client_Module, (unsigned __int32)Vtable[3]) == true))
				{
					Name_Stealer_Get_Client_Entity = (Name_Stealer_Get_Client_Entity_Type)Vtable[3];
				}
			}
		}

		const unsigned __int32 Now = GetTickCount();

		if ((Name_Stealer_Last_Change_Tick != 0) && ((__int32)(Now - Name_Stealer_Last_Change_Tick) < 1000))
		{
			return;
		}

		Name_Stealer_Last_Change_Tick = Now;

		const unsigned __int32 Local_Player = Get_Local_Player();

		if (Local_Player == 0)
		{
			return;
		}

		const __int32 Local_User_ID = *(__int32*)(Local_Player + 0x58);

		const __int32 Local_Team = *(__int32*)(Local_Player + 0xE4);

		for (__int32 i = 1; i <= 32; i++)
		{
			Sdk_Player_Info_Structure Info;

			memset(&Info, 0, sizeof(Info));

			if (Sdk_Get_Player_Info(i, &Info) == false)
			{
				continue;
			}

			if ((Info.Is_Bot == true) || (Info.Name[0] == '\0'))
			{
				continue;
			}

			if ((Info.User_ID == 0) || (Info.User_ID == Local_User_ID))
			{
				continue;
			}

			if (Name_Stealer_Get_Client_Entity != nullptr)
			{
				void* Entity = Name_Stealer_Get_Client_Entity(Name_Stealer_Entity_List, nullptr, i);

				if (Entity == nullptr)
				{
					continue;
				}

				const __int32 Team = *(__int32*)((unsigned __int32)Entity + 0xE4);

				if (Team != Local_Team)
				{
					continue;
				}
			}

			if (Name_Stealer_Already_Stolen(Info.User_ID) == true)
			{
				continue;
			}

			char Command[128];

			_snprintf(Command, sizeof(Command), "setinfo name \"%s\x1\"", Info.Name);

			Name_Stealer_Client_Cmd(Name_Stealer_Engine_Client, nullptr, Command);

			if (Name_Stealer_Stolen_Count < 32)
			{
				Name_Stealer_Stolen_Ids[Name_Stealer_Stolen_Count] = Info.User_ID;

				Name_Stealer_Stolen_Count += 1;
			}

			return;
		}

		Name_Stealer_Stolen_Count = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}