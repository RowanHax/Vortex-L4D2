#pragma once

#include <cstdio>

#include <string.h>

#include <math.h>

#include <imgui.h>

#include "Internal.hpp"

#include "D3D9.hpp"

#include "Sdk.hpp"

#include "BunnyHop.hpp"

typedef void* (__cdecl* CreateInterface_Type)(const char* Name, __int32* Return_Code);

typedef __int32(__fastcall* Get_Local_Player_Index_Type)(void* Ecx, void* Edx);

typedef bool(__fastcall* Is_In_Game_Type)(void* Ecx, void* Edx);

typedef void* (__fastcall* Get_Client_Entity_Type)(void* Ecx, void* Edx, __int32 Index);

typedef __int32(__fastcall* Get_Max_Entities_Type)(void* Ecx, void* Edx);

typedef __int32(__fastcall* Screen_Position_Type)(void* Ecx, void* Edx, const float* World, float* Screen);

typedef const void* (__fastcall* World_To_Screen_Matrix_Type)(void* Ecx, void* Edx);

static void* Engine_Client;

void* Entity_List;

static void* Debug_Overlay;

static Get_Local_Player_Index_Type Get_Local_Player_Index;

static Is_In_Game_Type Is_In_Game;

Get_Client_Entity_Type Get_Client_Entity;

Get_Max_Entities_Type Get_Max_Entities;

static Screen_Position_Type Screen_Position;

static World_To_Screen_Matrix_Type World_To_Screen_Matrix;

static float Esp_View_Matrix[4][4];

static bool Esp_Matrix_Valid;

static void* Find_Interface(HMODULE Module, const char* Name)
{
	const CreateInterface_Type Factory = (CreateInterface_Type)GetProcAddress(Module, "CreateInterface");

	if (Factory == nullptr)
	{
		return nullptr;
	}

	return Factory(Name, nullptr);
}

void Install_Esp()
{
	Engine_Client = Find_Interface(Engine_Module, "VEngineClient013");

	Entity_List = Find_Interface(Client_Module, "VClientEntityList003");

	Debug_Overlay = Find_Interface(Engine_Module, "VDebugOverlay003");


	if ((Engine_Client == nullptr) || (Entity_List == nullptr) || (Debug_Overlay == nullptr))
	{
		return;
	}

	Get_Local_Player_Index = (Get_Local_Player_Index_Type)(*(void***)Engine_Client)[12];

	Is_In_Game = (Is_In_Game_Type)(*(void***)Engine_Client)[26];

	Get_Client_Entity = (Get_Client_Entity_Type)(*(void***)Entity_List)[3];

	Get_Max_Entities = (Get_Max_Entities_Type)(*(void***)Entity_List)[8];

	Screen_Position = (Screen_Position_Type)(*(void***)Debug_Overlay)[12];

	World_To_Screen_Matrix = (World_To_Screen_Matrix_Type)(*(void***)Engine_Client)[37];

}

static float Get_Min_Box_Height(__int32 Kind, float Hitbox_Height)
{
	switch (Kind)
	{
	case Kind_Smoker: return 110.f;

	case Kind_Spitter: return 75.f;

	case Kind_Boomer: return 95.f;

	case Kind_Charger: return 100.f;

	case Kind_Hunter:
		return (Hitbox_Height < 60.f) ? Hitbox_Height : 105.f;
	}

	return Hitbox_Height;
}

struct Esp_Tracked_Entity
{
	__int32 Slot;
	__int32 Kind;
	bool In_Use;
};

static const __int32 Esp_Max_Tracked = 384;
static const unsigned __int32 Esp_Refresh_Milliseconds = 200;
static Esp_Tracked_Entity Esp_Tracked[Esp_Max_Tracked];
static __int32 Esp_Tracked_Count;
static unsigned __int32 Esp_Updated_At;
static unsigned __int32 Esp_Session_Check;
static unsigned __int32 Esp_Generation;
static bool Esp_Valid;

static void Esp_Reset_Tracked()
{
	memset(Esp_Tracked, 0, sizeof(Esp_Tracked));
	Esp_Tracked_Count = 0;
	Esp_Updated_At = 0;
	Esp_Valid = false;
}

static bool Esp_Read_Origin(void* Entity, float Out[3])
{
	if ((Entity == nullptr) || (Out == nullptr))
	{
		return false;
	}

	__try
	{
		const float* Origin = (const float*)((unsigned __int8*)Entity + 0x124);

		for (__int32 Axis = 0; Axis < 3; Axis++)
		{
			if ((Origin[Axis] != Origin[Axis]) || (fabsf(Origin[Axis]) > 1000000.f))
			{
				return false;
			}
			Out[Axis] = Origin[Axis];
		}
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Esp_Project(const float World[3], float Screen[3])
{
	if ((World == nullptr) || (Screen == nullptr))
	{
		return false;
	}

	if (Esp_Matrix_Valid == true)
	{
		__try
		{
			const float W = Esp_View_Matrix[3][0] * World[0] + Esp_View_Matrix[3][1] * World[1] + Esp_View_Matrix[3][2] * World[2] + Esp_View_Matrix[3][3];

			if (W < 0.001f)
			{
				return false;
			}

			const float X = (Esp_View_Matrix[0][0] * World[0] + Esp_View_Matrix[0][1] * World[1] + Esp_View_Matrix[0][2] * World[2] + Esp_View_Matrix[0][3]) / W;

			const float Y = (Esp_View_Matrix[1][0] * World[0] + Esp_View_Matrix[1][1] * World[1] + Esp_View_Matrix[1][2] * World[2] + Esp_View_Matrix[1][3]) / W;

			const ImVec2 Screen_Size = ImGui::GetIO().DisplaySize;

			Screen[0] = (Screen_Size.x / 2.f) + (X * Screen_Size.x) / 2.f;
			Screen[1] = (Screen_Size.y / 2.f) - (Y * Screen_Size.y) / 2.f;
			Screen[2] = 0.f;

			return (Screen[0] == Screen[0]) && (Screen[1] == Screen[1]) &&
				(fabsf(Screen[0]) < 100000.f) && (fabsf(Screen[1]) < 100000.f);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	if ((Debug_Overlay == nullptr) || (Screen_Position == nullptr))
	{
		return false;
	}

	__try
	{
		Screen[0] = 0.f;
		Screen[1] = 0.f;
		Screen[2] = 0.f;

		const __int32 Result = Screen_Position(Debug_Overlay, nullptr, World, Screen);

		return (Result == 0) && (Screen[0] == Screen[0]) && (Screen[1] == Screen[1]) &&
			(Screen[2] == Screen[2]) && (fabsf(Screen[0]) < 100000.f) &&
			(fabsf(Screen[1]) < 100000.f) && (fabsf(Screen[2]) < 100000.f);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static ImU32 Esp_Box_Color(__int32 Team, __int32 Kind)
{
	if (Team == 2)
	{
		return ImColor(Esp_Team2_Color[0], Esp_Team2_Color[1], Esp_Team2_Color[2], Esp_Team2_Color[3]);
	}

	switch (Kind)
	{
	case Kind_Smoker:  return IM_COL32(150, 220, 90, 255);
	case Kind_Boomer:  return IM_COL32(235, 130, 220, 255);
	case Kind_Hunter:  return IM_COL32(255, 160, 40, 255);
	case Kind_Spitter: return IM_COL32(120, 200, 255, 255);
	case Kind_Jockey:  return IM_COL32(140, 130, 255, 255);
	case Kind_Charger: return IM_COL32(150, 150, 150, 255);
	case Kind_Tank:    return IM_COL32(90, 100, 255, 255);
	case Kind_Witch:   return IM_COL32(110, 230, 170, 255);
	case Kind_Common:  return IM_COL32(235, 195, 110, 255);
	}

	return ImColor(Esp_Team3_Color[0], Esp_Team3_Color[1], Esp_Team3_Color[2], Esp_Team3_Color[3]);
}

static bool Esp_Entity_Alive(__int32 Kind, __int32 Health, unsigned __int8 Dead)
{
	if (Kind == Kind_Common)
	{
		return true;
	}

	return (Dead == 0) && (Health > 0) && (Health <= 100000);
}

static void Update_Esp()
{
	if (Esp_Enabled == false)
	{
		Esp_Reset_Tracked();
		Esp_Matrix_Valid = false;
		return;
	}

	__try
	{
		const unsigned __int32 Now = GetTickCount();

		if ((Esp_Session_Check == 0) || ((unsigned __int32)(Now - Esp_Session_Check) >= 250))
		{
			Esp_Session_Check = Now;

			if (Sdk_Update_Game_Session() == false)
			{
				Esp_Reset_Tracked();
				return;
			}
		}

		const unsigned __int32 Generation = Sdk_Get_Game_Session_Generation();

		if (Esp_Generation != Generation)
		{
			Esp_Reset_Tracked();
			Esp_Generation = Generation;
		}

		void* Local_Player = Sdk_Get_Local_Player_Safe();

		float Local_Origin[3];

		if ((Local_Player == nullptr) || (Esp_Read_Origin(Local_Player, Local_Origin) == false))
		{
			Esp_Reset_Tracked();
			Esp_Generation = Generation;
			return;
		}

		const bool Refresh = (Esp_Valid == false) || ((unsigned __int32)(Now - Esp_Updated_At) >= Esp_Refresh_Milliseconds);

		if (Refresh == true)
		{
			Esp_Tracked_Count = 0;

			const __int32 Max_Entities = min(Sdk_Get_Max_Entities_Safe(), 2048);

			for (__int32 Slot = 1; (Slot < Max_Entities) && (Esp_Tracked_Count < Esp_Max_Tracked); Slot++)
			{
				void* Entity = Sdk_Get_Client_Entity_Safe(Slot);

				if ((Entity == nullptr) || (Entity == Local_Player))
				{
					continue;
				}

				const __int32 Team = *(__int32*)((unsigned __int8*)Entity + 0xE4);
				const __int32 Health = *(__int32*)((unsigned __int8*)Entity + 0xEC);
				const unsigned __int8 Dead = *(unsigned __int8*)((unsigned __int8*)Entity + 0x147);

				if (((Team == 2) && (Esp_Team2 == false)) ||
					((Team == 3) && (Esp_Team3 == false)) ||
					((Team != 2) && (Team != 3)))
				{
					continue;
				}

				const __int32 Kind = Get_Entity_Kind(Entity);

				if (Kind == Kind_Invalid)
				{
					continue;
				}

				if (Esp_Entity_Alive(Kind, Health, Dead) == false)
				{
					continue;
				}

				if ((Kind == Kind_Common) && (Esp_Commons == false))
				{
					continue;
				}

				Esp_Tracked_Entity& Entry = Esp_Tracked[Esp_Tracked_Count++];

				Entry.Slot = Slot;
				Entry.Kind = Kind;
				Entry.In_Use = true;
			}

			Esp_Updated_At = Now;
			Esp_Valid = true;
		}

		if ((World_To_Screen_Matrix != nullptr) && (Engine_Client != nullptr))
		{
			__try
			{
				const float* Matrix = (const float*)World_To_Screen_Matrix(Engine_Client, nullptr);

			if (Matrix != nullptr)
			{
				memcpy(Esp_View_Matrix, Matrix, sizeof(Esp_View_Matrix));

				Esp_Matrix_Valid = true;

				for (__int32 Value_Index = 0; (Value_Index < 16) && (Esp_Matrix_Valid == true); Value_Index++)
				{
					const float Value = ((const float*)Esp_View_Matrix)[Value_Index];

					if ((Value != Value) || (fabsf(Value) > 1000000.f))
					{
						Esp_Matrix_Valid = false;
					}
				}
			}
			else
			{
				Esp_Matrix_Valid = false;
			}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				Esp_Matrix_Valid = false;
			}
		}

		ImDrawList* Draw_List = ImGui::GetBackgroundDrawList();

		if (Draw_List == nullptr)
		{
			return;
		}

		const ImVec2 Screen_Size = ImGui::GetIO().DisplaySize;

		for (__int32 i = 0; i < Esp_Tracked_Count; i++)
		{
			const Esp_Tracked_Entity& Entry = Esp_Tracked[i];

			void* Entity = Sdk_Get_Client_Entity_Safe(Entry.Slot);

			if ((Entity == nullptr) || (Entity == Local_Player))
			{
				continue;
			}

			const __int32 Team = *(__int32*)((unsigned __int8*)Entity + 0xE4);
			const __int32 Health = *(__int32*)((unsigned __int8*)Entity + 0xEC);
			const unsigned __int8 Dead = *(unsigned __int8*)((unsigned __int8*)Entity + 0x147);

			if (Esp_Entity_Alive(Entry.Kind, Health, Dead) == false)
			{
				continue;
			}

			if (((Team == 2) && (Esp_Team2 == false)) ||
				((Team == 3) && (Esp_Team3 == false)) ||
				((Team != 2) && (Team != 3)) ||
				((Entry.Kind == Kind_Common) && (Esp_Commons == false)))
			{
				continue;
			}

			float Origin[3];

			if (Esp_Read_Origin(Entity, Origin) == false)
			{
				continue;
			}

			const float Head_Position[3] =
			{
				Origin[0], Origin[1], Origin[2] + Get_Head_Height(Entry.Kind)
			};

			float Feet[3];
			float Head[3];

			if ((Esp_Project(Origin, Feet) == false) || (Esp_Project(Head_Position, Head) == false))
			{
				continue;
			}

			const float Height = Feet[1] - Head[1];

			if ((Height <= 1.f) || (Height > 2000.f))
			{
				continue;
			}

			const float X = Head[0] - (Height * 0.45f / 2.f);
			const float Y = Head[1];
			const float Width = Height * 0.45f;

			if ((Feet[0] < -100.f) || (Feet[0] > Screen_Size.x + 100.f) ||
				(Feet[1] < -100.f) || (Feet[1] > Screen_Size.y + 100.f) ||
				(Head[0] < -100.f) || (Head[0] > Screen_Size.x + 100.f) ||
				(Head[1] < -100.f) || (Head[1] > Screen_Size.y + 100.f))
			{
				continue;
			}

			const ImU32 Box_Color = Esp_Box_Color(Team, Entry.Kind);

			if (Esp_Box_Style == 0)
			{
				Draw_List->AddRect(ImVec2(X, Y), ImVec2(X + Width, Y + Height), Box_Color);
			}
			else
			{
				const float Corner_Length = (Width < 8.f) ? Width : 8.f;

				Draw_List->AddLine(ImVec2(X, Y + Corner_Length), ImVec2(X, Y), Box_Color);
				Draw_List->AddLine(ImVec2(X, Y), ImVec2(X + Corner_Length, Y), Box_Color);
				Draw_List->AddLine(ImVec2(X + Width - Corner_Length, Y), ImVec2(X + Width, Y), Box_Color);
				Draw_List->AddLine(ImVec2(X + Width, Y), ImVec2(X + Width, Y + Corner_Length), Box_Color);
				Draw_List->AddLine(ImVec2(X + Width, Y + Height - Corner_Length), ImVec2(X + Width, Y + Height), Box_Color);
				Draw_List->AddLine(ImVec2(X + Width - Corner_Length, Y + Height), ImVec2(X + Width, Y + Height), Box_Color);
				Draw_List->AddLine(ImVec2(X, Y + Height - Corner_Length), ImVec2(X, Y + Height), Box_Color);
				Draw_List->AddLine(ImVec2(X, Y + Height), ImVec2(X + Corner_Length, Y + Height), Box_Color);
			}

			if (Esp_Health_Bar == true)
			{
				__int32 Max_Health = Health;

				__try
				{
					const __int32 Candidate_Max_Health = *(__int32*)((unsigned __int8*)Entity + 0x1FDC);

					if ((Candidate_Max_Health > 0) && (Candidate_Max_Health <= 100000))
					{
						Max_Health = Candidate_Max_Health;
					}
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
				}

				float Ratio = (Max_Health > 0) ? (float)Health / (float)Max_Health : 0.f;

				if (Ratio < 0.f) Ratio = 0.f;
				if (Ratio > 1.f) Ratio = 1.f;

				Draw_List->AddRectFilled(ImVec2(X - 6.f, Y), ImVec2(X - 3.f, Y + Height), ImColor(0, 0, 0, 200));
				Draw_List->AddRectFilled(ImVec2(X - 5.5f, Y + Height - (Height * Ratio)), ImVec2(X - 3.5f, Y + Height), ImColor(0, 255, 0, 255));
			}

			if (Esp_Snap_Lines == true)
			{
				Draw_List->AddLine(ImVec2(Screen_Size.x / 2.f, Screen_Size.y), ImVec2(Feet[0], Feet[1]), Box_Color);
			}

			if ((Esp_Names == true) || (Esp_Distance == true))
			{
				const float Distance_X = Origin[0] - Local_Origin[0];
				const float Distance_Y = Origin[1] - Local_Origin[1];
				const float Distance_Z = Origin[2] - Local_Origin[2];
				const float Distance = sqrtf(Distance_X * Distance_X + Distance_Y * Distance_Y + Distance_Z * Distance_Z) / 100.f;

				char Text[64];

				if (Esp_Names == true)
				{
					sprintf_s(Text, (Esp_Distance == true) ? "%s - %.0fm" : "%s", Get_Kind_Name(Entry.Kind), Distance);
				}
				else
				{
					sprintf_s(Text, "%.0fm", Distance);
				}

				Draw_List->AddText(ImVec2(X, Y - 14.f), Box_Color, Text);
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Esp_Reset_Tracked();
	}
}