#pragma once

#include <cstdio>

#include <string.h>

#include <windows.h>

#include <imgui.h>

#include "Internal.hpp"

#include "Sdk.hpp"

#include "../Lua/LuaChatHook.hpp"

struct IGameEvent
{
	void** Vtable;

	const char* Get_Name()
	{
		return ((const char* (__thiscall*)(void*))Vtable[1])(this);
	}

	__int32 Get_Int(const char* Key, __int32 Default)
	{
		return ((__int32 (__thiscall*)(void*, const char*, __int32))Vtable[6])(this, Key, Default);
	}

	bool Get_Bool(const char* Key, bool Default)
	{
		return ((bool (__thiscall*)(void*, const char*, bool))Vtable[5])(this, Key, Default);
	}

	const char* Get_String(const char* Key, const char* Default)
	{
		return ((const char* (__thiscall*)(void*, const char*, const char*))Vtable[9])(this, Key, Default);
	}
};

struct IGameEventListener2
{
	void** Vtable;
};

struct IGameEventManager2
{
	void** Vtable;

	void Add_Listener(void* Listener, const char* Event_Name, bool Server_Side)
	{
		((void (__thiscall*)(void*, void*, const char*, bool))Vtable[3])(this, Listener, Event_Name, Server_Side);
	}
};

typedef __int32(__fastcall* Get_Player_For_User_ID_Type)(void* Ecx, void* Edx, __int32 User_ID);

typedef unsigned __int8(__fastcall* Is_Enemy_Type)(void* Ecx, void* Edx, __int32 Entity_Index, void* Player_Info);

static const unsigned __int32 Engine_Client_Object_Offset = 0x425504;

static Get_Player_For_User_ID_Type Get_Player_For_User_ID;

static Is_Enemy_Type Is_Entity_Enemy;

static IGameEventManager2* Event_Manager;

static void Chat_Spy_Push_Line(const char* Text, unsigned __int32 Color)
{
	Chat_Spy_Lines[Chat_Spy_Line_Index % 8].Time = 0;

	_snprintf(Chat_Spy_Lines[Chat_Spy_Line_Index % 8].Text, sizeof(Chat_Spy_Lines[Chat_Spy_Line_Index % 8].Text), "%s", Text);

	Chat_Spy_Lines[Chat_Spy_Line_Index % 8].Time = GetTickCount();

	Chat_Spy_Lines[Chat_Spy_Line_Index % 8].Color = Color;

	Chat_Spy_Line_Index++;
}

static void Chat_Spy_Process_Event(IGameEvent* Event);

static void Chat_Spy_Forward_To_Lua(IGameEvent* Event);

class Chat_Spy_Listener_Class
{
public:
	virtual ~Chat_Spy_Listener_Class()
	{
	}

	virtual void Fire_Game_Event(IGameEvent* Event)
	{
		Chat_Spy_Forward_To_Lua(Event);

		Chat_Spy_Process_Event(Event);
	}

	virtual __int32 Get_Debug_Name()
	{
		return 0x2A;
	}
};

static Chat_Spy_Listener_Class Chat_Spy_Listener;

static const char* Chat_Spy_Team_Name(__int32 Team)
{
	if (Team == 3)
	{
		return "(Infected) ";
	}

	if (Team == 2)
	{
		return "(Survivor) ";
	}

	if (Team == 1)
	{
		return "(Spectator) ";
	}

	return "(Unknown) ";
}

static void Chat_Spy_Process_Overlay(IGameEvent* Event)
{
	if (Event == nullptr)
	{
		return;
	}

	__try
	{

	if (Event->Vtable == nullptr)
	{
		return;
	}

	const char* Event_Name = Event->Get_Name();

	if (Event_Name == nullptr)
	{
		return;
	}

	const bool Is_Vote = (strcmp(Event_Name, "vote_cast_yes") == 0) || (strcmp(Event_Name, "vote_cast_no") == 0);

	const bool Is_Chat = (strcmp(Event_Name, "player_say") == 0);

	if ((Is_Vote == false) && (Is_Chat == false))
	{
		return;
	}

	const unsigned __int32 Local_Player = Get_Local_Player();

	if (Local_Player == 0)
	{
		return;
	}

	const __int32 Local_User_ID = *(__int32*)(Local_Player + 0x58);

	__int32 Entity_Index = -1;

	const char* Name = "";

	const char* Text = "";

	if (Is_Vote == true)
	{
		Entity_Index = Event->Get_Int("entityid", 0xffffffff);
	}
	else
	{
		const __int32 User_ID = Event->Get_Int("userid", 0);

		Name = Event->Get_String("name", "");

		Text = Event->Get_String("text", "");

		if (User_ID == 0)
		{
			return;
		}

		__try
		{
			Entity_Index = Get_Player_For_User_ID((void*)((unsigned __int32)Engine_Module + Engine_Client_Object_Offset), nullptr, User_ID);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	if ((Entity_Index < 1) || (Entity_Index == Local_User_ID))
	{
		return;
	}

	void* Entity = nullptr;

	__try
	{
		Entity = *(void**)((unsigned __int32)Client_Module + 0x74A574 + (Entity_Index - 0x1001) * 16);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

	if (Entity == nullptr)
	{
		return;
	}

	const __int32 Team = *(__int32*)((unsigned __int32)Entity + 0xE4);

	const unsigned __int32 Local_Player_Ptr = Get_Local_Player();

	const __int32 Local_Team = (Local_Player_Ptr != 0) ? *(__int32*)(Local_Player_Ptr + 0xE4) : 0;

	const bool Is_Enemy = ((Local_Team == 2) && (Team == 3)) || ((Local_Team == 3) && (Team == 2));

	if (Is_Enemy == false)
	{
		return;
	}

	unsigned __int8 Info[0x1A4] = { 0 };

	__try
	{
		Is_Entity_Enemy((void*)((unsigned __int32)Engine_Module + Engine_Client_Object_Offset), nullptr, Entity_Index, Info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	if ((Name == nullptr) || (Name[0] == '\0'))
	{
		static char Fallback_Name[64];

		if (Vortex_Get_Player_Name(Entity_Index, Fallback_Name, sizeof(Fallback_Name)) == true)
		{
			Name = Fallback_Name;
		}
	}

	const unsigned __int32 Color = (Team == 3) ? 0xFF6F7D : 0xD0D0D0;

	char Line[256];

	if (Is_Vote == true)
	{
		_snprintf(Line, sizeof(Line), "%s%s Voted: %s", Chat_Spy_Team_Name(Team), Name, (strcmp(Event_Name, "vote_cast_yes") == 0) ? "Yes" : "No");
	}
	else
	{
		_snprintf(Line, sizeof(Line), "%s%s : %s", Chat_Spy_Team_Name(Team), Name, Text);
	}

	Chat_Spy_Push_Line(Line, Color);

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}
}

static void* Chat_Spy_Find_Pattern_Hud(const wchar_t* Module_Name, const unsigned __int8* Pattern, const char* Mask)
{
	unsigned __int8* Module = (unsigned __int8*)GetModuleHandleW(Module_Name);

	if (Module == nullptr)
	{
		return nullptr;
	}

	const IMAGE_DOS_HEADER* Dos = (const IMAGE_DOS_HEADER*)Module;
	const IMAGE_NT_HEADERS* Nt = (const IMAGE_NT_HEADERS*)(Module + Dos->e_lfanew);
	unsigned __int8* Section = Module + Nt->OptionalHeader.BaseOfCode;
	const unsigned __int32 Section_Size = Nt->OptionalHeader.SizeOfCode;
	const unsigned __int32 Mask_Length = (unsigned __int32)strlen(Mask);

	for (unsigned __int32 i = 0; i < Section_Size - Mask_Length; i++)
	{
		bool Found = true;

		for (unsigned __int32 j = 0; j < Mask_Length; j++)
		{
			if ((Mask[j] != '?') && (Pattern[j] != Section[i + j]))
			{
				Found = false;
				break;
			}
		}

		if (Found == true)
		{
			return Section + i;
		}
	}

	return nullptr;
}

static void* Chat_Spy_Get_CHud_Chat()
{
	__try
	{
		static void* Chud_Chat = (void*)-1;

		if (Chud_Chat != (void*)-1)
		{
			return Chud_Chat;
		}

		const unsigned __int8 Signature[] = { 0x89, 0x04, 0xB5, 0x00, 0x00, 0x00, 0x00, 0xE8 };
		const unsigned __int8* Sig = (const unsigned __int8*)Chat_Spy_Find_Pattern_Hud(L"client.dll", Signature, "xxx????x");
		void* Result = nullptr;

		if (Sig != nullptr)
		{
			const unsigned __int32 Global_Addr = *(const unsigned __int32*)(Sig + 3);

			if (Global_Addr != 0)
			{
				void* Client_Mode = *(void**)Global_Addr;

				if (Client_Mode != nullptr)
				{
					Result = *(void**)((unsigned __int32)Client_Mode + 0x18);
				}
			}
		}

		Chud_Chat = Result;
		return Result;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static void Chat_Spy_Hud_Printf(__int32 Index, const char* Message)
{
	__try
	{
		void* Chud_Chat = Chat_Spy_Get_CHud_Chat();

		if (Chud_Chat == nullptr)
		{
			return;
		}

		void** Vtable = *(void***)Chud_Chat;

		if (Vtable == nullptr)
		{
			return;
		}

		void* Function = Vtable[23];

		if (Function == nullptr)
		{
			return;
		}

		typedef void(__stdcall* Chat_Printf_Type)(void* This, __int32 Player_Index, __int32 Secondary, const char* Message);

		((Chat_Printf_Type)Function)(Chud_Chat, Index, 0, Message);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void Chat_Spy_Process_Hud(IGameEvent* Event)
{
	if (Event == nullptr)
	{
		return;
	}

	__try
	{
		if (Event->Vtable == nullptr)
		{
			return;
		}

		const char* Event_Name = Event->Get_Name();

		if (Event_Name == nullptr)
		{
			return;
		}

		if ((strcmp(Event_Name, "player_say") != 0) && (strcmp(Event_Name, "player_say_team") != 0))
		{
			return;
		}

		const __int32 User_ID = Event->Get_Int("userid", 0);
		const char* Text = Event->Get_String("text", "");

		if ((User_ID == 0) || (Text == nullptr) || (Text[0] == '\0'))
		{
			return;
		}

		__int32 Entity_Index = -1;

		__try
		{
			Entity_Index = Get_Player_For_User_ID((void*)((unsigned __int32)Engine_Module + Engine_Client_Object_Offset), nullptr, User_ID);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}

		if (Entity_Index < 1)
		{
			return;
		}

		void* Entity = nullptr;

		__try
		{
			Entity = *(void**)((unsigned __int32)Client_Module + 0x74A574 + (Entity_Index - 0x1001) * 16);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}

		if (Entity == nullptr)
		{
			return;
		}

		const __int32 Team = *(__int32*)((unsigned __int32)Entity + 0xE4);

		const unsigned __int32 Local_Player_Ptr = Get_Local_Player();

		const __int32 Local_Team = (Local_Player_Ptr != 0) ? *(__int32*)(Local_Player_Ptr + 0xE4) : 0;

		const bool Is_Enemy = ((Local_Team == 2) && (Team == 3)) || ((Local_Team == 3) && (Team == 2));

		if (Is_Enemy == false)
		{
			return;
		}

		unsigned __int8 Info[0x1A4] = { 0 };

		__try
		{
			Is_Entity_Enemy((void*)((unsigned __int32)Engine_Module + Engine_Client_Object_Offset), nullptr, Entity_Index, Info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}


		const char* Name = (const char*)Info;

		if ((Name == nullptr) || (Sdk_Plausible_Name(Name, sizeof(Info)) == false))
		{
			static char Fallback_Name[64];

			if (Vortex_Get_Player_Name(Entity_Index, Fallback_Name, sizeof(Fallback_Name)) == true)
			{
				Name = Fallback_Name;
			}
			else
			{
				Name = "(unknown)";
			}
		}

		const char* Team_Label = "(unknown)";
		const char* Color_Code = "\x01";

		if (Team == 2) { Team_Label = "(survivor)"; Color_Code = "\x05"; }
		else if (Team == 3) { Team_Label = "(infected)"; Color_Code = "\x05"; }
		else if (Team == 1) { Team_Label = "(spectator)"; Color_Code = "\x04"; }

		char Line[512];
		_snprintf(Line, sizeof(Line), "%s%s \x01%s: %s", Color_Code, Team_Label, Name, Text);

		Chat_Spy_Hud_Printf(Entity_Index, Line);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void Chat_Spy_Forward_To_Lua(IGameEvent* Event)
{
	if ((Event == nullptr) || (Lua_ChatDispatchFn == nullptr))
	{
		return;
	}

	static bool In_Dispatch = false;

	if (In_Dispatch == true)
	{
		return;
	}

	__int32 Entity_Index = -1;
	const char* Name = "";
	const char* Text = "";
	__int32 Team = 0;
	int Team_Only = 0;
	int Vote = (int)Lua_ChatVote_None;

	__try
	{
		if (Event->Vtable == nullptr)
		{
			return;
		}

		const char* Event_Name = Event->Get_Name();

		if (Event_Name == nullptr)
		{
			return;
		}

		const bool Is_Vote_Yes = (strcmp(Event_Name, "vote_cast_yes") == 0);
		const bool Is_Vote_No = (strcmp(Event_Name, "vote_cast_no") == 0);
		const bool Is_Team_Say = (strcmp(Event_Name, "player_say_team") == 0);
		const bool Is_Say = (strcmp(Event_Name, "player_say") == 0) || Is_Team_Say;
		const bool Is_Vote = Is_Vote_Yes || Is_Vote_No;

		if ((Is_Say == false) && (Is_Vote == false))
		{
			return;
		}

		const unsigned __int32 Local_Player = Get_Local_Player();

		if (Local_Player == 0)
		{
			return;
		}

		const __int32 Local_User_ID = *(__int32*)(Local_Player + 0x58);

		if (Is_Vote == true)
		{
			Entity_Index = Event->Get_Int("entityid", 0xffffffff);
		}
		else
		{
			const __int32 User_ID = Event->Get_Int("userid", 0);

			const char* Event_Name_Field = Event->Get_String("name", "");
			const char* Event_Text = Event->Get_String("text", "");

			if (User_ID == 0)
			{
				return;
			}

			if (User_ID == Local_User_ID)
			{
				return;
			}

			if ((Event_Text == nullptr) || (Event_Text[0] == '\0'))
			{
				return;
			}

			Name = (Event_Name_Field != nullptr) ? Event_Name_Field : "";
			Text = Event_Text;

			__try
			{
				Entity_Index = Get_Player_For_User_ID((void*)((unsigned __int32)Engine_Module + Engine_Client_Object_Offset), nullptr, User_ID);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return;
			}
		}

		if (Entity_Index < 1)
		{
			return;
		}

		void* Entity = nullptr;

		__try
		{
			Entity = *(void**)((unsigned __int32)Client_Module + 0x74A574 + (Entity_Index - 0x1001) * 16);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}

		if (Entity == nullptr)
		{
			return;
		}

		Team = *(__int32*)((unsigned __int32)Entity + 0xE4);

		if ((Name == nullptr) || (Name[0] == '\0'))
		{
			static char Fallback_Name[64];

			if (Vortex_Get_Player_Name(Entity_Index, Fallback_Name, sizeof(Fallback_Name)) == true)
			{
				Name = Fallback_Name;
			}
			else
			{
				Name = "(unknown)";
			}
		}

		Team_Only = Is_Team_Say ? 1 : 0;
		Vote = Is_Vote_Yes ? (int)Lua_ChatVote_Yes : (Is_Vote_No ? (int)Lua_ChatVote_No : (int)Lua_ChatVote_None);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

	if (Entity_Index < 1)
	{
		return;
	}

	In_Dispatch = true;

	__try
	{
		Lua_ChatDispatchFn(Entity_Index, Name, Text, Team, Team_Only, Vote);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	In_Dispatch = false;
}

static void Chat_Spy_Process_Event(IGameEvent* Event)
{
	if ((Chat_Spy_Enabled == false) || (Event == nullptr))
	{
		return;
	}

	if (Chat_Spy_Method == 1)
	{
		Chat_Spy_Process_Hud(Event);
		return;
	}

	Chat_Spy_Process_Overlay(Event);
}

static void Update_Chat_Spy_Feed()
{
	if ((Chat_Spy_Enabled == false) || (Chat_Spy_Method == 1))
	{
		return;
	}

	ImDrawList* Draw_List = ImGui::GetBackgroundDrawList();

	const unsigned __int32 Now = GetTickCount();

	const float Line_Height = ImGui::GetFontSize() + 3.f;

	const float Start_Y = (ImGui::GetIO().DisplaySize.y * 0.22f);

	float Y = Start_Y;

	unsigned __int32 Drawn = 0;

	for (__int32 i = 0; i < 8; i++)
	{
		const Chat_Spy_Line_Structure& Line = Chat_Spy_Lines[(Chat_Spy_Line_Index + 7 - i) % 8];

		if ((Line.Time == 0) || (Line.Text[0] == '\0'))
		{
			continue;
		}

		const __int32 Age = (__int32)(Now - Line.Time);

		if (Age > 10000)
		{
			continue;
		}

		const float Alpha = 1.f - (float)Age / 10000.f;

		const unsigned __int32 Color = (((unsigned __int32)(Alpha * 255.f)) << 24) | (((Line.Color >> 16) & 0xFF) << 16) | (((Line.Color >> 8) & 0xFF) << 8) | (Line.Color & 0xFF);

		Draw_List->AddText(ImVec2(12.f, Y), Color, Line.Text);

		Y += Line_Height;

		Drawn++;
	}

	(void)Drawn;
}

typedef void* (__cdecl* Chat_Spy_Create_Interface_Type)(const char* Name, __int32* Return_Code);

static void* Chat_Spy_Find_Interface(HMODULE Module, const char* Name)
{
	const Chat_Spy_Create_Interface_Type Factory = (Chat_Spy_Create_Interface_Type)GetProcAddress(Module, "CreateInterface");

	if (Factory == nullptr)
	{
		return nullptr;
	}

	return Factory(Name, nullptr);
}

static void Install_Chat_Spy()
{
	Get_Player_For_User_ID = (Get_Player_For_User_ID_Type)((unsigned __int32)Engine_Module + 0x5DA40);

	Is_Entity_Enemy = (Is_Enemy_Type)((unsigned __int32)Engine_Module + 0x5F4D0);

	Event_Manager = (IGameEventManager2*)Chat_Spy_Find_Interface(Engine_Module, "GAMEEVENTSMANAGER002");

	if (Event_Manager == nullptr)
	{
		Event_Manager = (IGameEventManager2*)Chat_Spy_Find_Interface(Engine_Module, "GAMEEVENTSMANAGER001");
	}


	if (Event_Manager == nullptr)
	{
		return;
	}

	__try
	{
		Event_Manager->Add_Listener(&Chat_Spy_Listener, "player_say", false);

		Event_Manager->Add_Listener(&Chat_Spy_Listener, "player_say_team", false);

		Event_Manager->Add_Listener(&Chat_Spy_Listener, "vote_cast_yes", false);

		Event_Manager->Add_Listener(&Chat_Spy_Listener, "vote_cast_no", false);

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}