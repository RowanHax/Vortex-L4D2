#pragma once

#include <cstdio>

#include <string.h>

#include <math.h>

#include <imgui.h>

#include "Internal.hpp"

#include "Esp.hpp"

#include "Sdk.hpp"

enum Item_Esp_Kind
{
	Item_Esp_None = 0,
	Item_Esp_Weapon,
	Item_Esp_Mounted
};

typedef __int32(__fastcall* Item_Esp_Get_Weapon_Id_Type)(void* Ecx, void* Edx);

static const __int32 Item_Esp_Get_Weapon_Id_Vtable_Slot = 0xDB;

static const char* Item_Esp_Weapon_Names[39] =
{
	"Unknown",
	"Pistol",
	"Uzi",
	"Pump Shotgun",
	"Auto Shotgun",
	"M16A1",
	"Hunting Rifle",
	"Mac10",
	"Chrome Shotgun",
	"Scar",
	"Military Sniper",
	"SpaS",
	"First Aid",
	"Molotov",
	"Pipe Bomb",
	"Pills",
	"Gas Can",
	"Propane Tank",
	"Oxygen Tank",
	"Melee Weapon",
	"Chainsaw",
	"Grenade Launcher",
	"Unknown",
	"Adrenaline",
	"Defibrillator",
	"Vomit Jar",
	"AK47",
	"Unknown",
	"Unknown",
	"Firework Crate",
	"Incendiary Ammo",
	"Explosive Ammo",
	"Deagle",
	"MP5",
	"SG552",
	"AWP",
	"Scout",
	"M60",
	"Unknown"
};

static bool Item_Esp_Is_Heal_Id(__int32 Id)
{
	return ((Id == 12) || (Id == 15) || (Id == 23) || (Id == 24));
}

static __int32 Item_Esp_Classify(void* Entity)
{
	__try
	{
		Client_Class_Structure* Class = Get_Client_Class(Entity);

		if ((Class == nullptr) || (Class->Network_Name == nullptr))
		{
			return Item_Esp_None;
		}

		const char* Name = Class->Network_Name;

		const __int32 Class_Id = *(__int32*)((unsigned __int8*)Class + 0x14);

		if ((Class_Id == 0x104) || (strstr(Name, "WeaponSpawn") != nullptr))
		{
			return Item_Esp_Weapon;
		}

		if ((Class_Id == 0x91) || (Class_Id == 0x92) || (strstr(Name, "Minigun") != nullptr) || (strstr(Name, "Mounted") != nullptr))
		{
			return Item_Esp_Mounted;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return Item_Esp_None;
}

static __int32 Item_Esp_Get_Weapon_Id(void* Entity)
{
	__try
	{
		if (Sdk_Is_Readable_Range(Entity, sizeof(void*)) == false)
		{
			return -1;
		}

		void** Vtable = *(void***)Entity;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, (Item_Esp_Get_Weapon_Id_Vtable_Slot + 1) * sizeof(void*)) == false))
		{
			return -1;
		}

		const Item_Esp_Get_Weapon_Id_Type Get_Weapon_Id = (Item_Esp_Get_Weapon_Id_Type)Vtable[Item_Esp_Get_Weapon_Id_Vtable_Slot];

		if ((Get_Weapon_Id == nullptr) || (Sdk_Address_In_Module(Client_Module, (unsigned __int32)Get_Weapon_Id) == false))
		{
			return -1;
		}

		const __int32 Id = Get_Weapon_Id(Entity, nullptr);

		if ((Id < 0) || (Id > 38))
		{
			return -1;
		}

		return Id;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
}

static __int32 Item_Esp_Heat_Offset = -2;

static float Item_Esp_Read_Heat(void* Entity)
{
	__try
	{
		if (Item_Esp_Heat_Offset == -2)
		{
			Item_Esp_Heat_Offset = Get_Net_Prop_Offset("DT_PropMinigun", "m_heat");

			if (Item_Esp_Heat_Offset < 0)
			{
				Item_Esp_Heat_Offset = Get_Net_Prop_Offset("DT_MountedGun", "m_heat");
			}
		}

		if (Item_Esp_Heat_Offset < 0)
		{
			return -1.f;
		}

		const float Heat = *(float*)((unsigned __int8*)Entity + Item_Esp_Heat_Offset);

		if ((Heat != Heat) || (Heat < 0.f) || (Heat > 10000.f))
		{
			return -1.f;
		}

		return Heat;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1.f;
	}
}

struct Item_Esp_Tracked_Entity
{
	__int32 Slot;

	__int32 Kind;
};

static const __int32 Item_Esp_Max_Tracked = 384;

static const unsigned __int32 Item_Esp_Refresh_Milliseconds = 250;

static Item_Esp_Tracked_Entity Item_Esp_Tracked[Item_Esp_Max_Tracked];

static __int32 Item_Esp_Tracked_Count;

static unsigned __int32 Item_Esp_Updated_At;

static bool Item_Esp_Valid;

static void Update_Item_Esp()
{
	if (Item_Esp_Enabled == false)
	{
		Item_Esp_Tracked_Count = 0;

		Item_Esp_Valid = false;

		return;
	}

	__try
	{
		const unsigned __int32 Now = GetTickCount();

		const bool Refresh = (Item_Esp_Valid == false) || ((unsigned __int32)(Now - Item_Esp_Updated_At) >= Item_Esp_Refresh_Milliseconds);

		if (Refresh == true)
		{
			Item_Esp_Tracked_Count = 0;

			Item_Esp_Updated_At = Now;

			const __int32 Max_Entities = min(Sdk_Get_Max_Entities_Safe(), 2048);

			for (__int32 Slot = 1; (Slot < Max_Entities) && (Item_Esp_Tracked_Count < Item_Esp_Max_Tracked); Slot++)
			{
				__try
				{
				void* Entity = Sdk_Get_Client_Entity_Safe(Slot);

				if (Entity == nullptr)
				{
					continue;
				}

				const __int32 Kind = Item_Esp_Classify(Entity);

				if (Kind == Item_Esp_None)
				{
					continue;
				}

				Item_Esp_Tracked_Entity& Entry = Item_Esp_Tracked[Item_Esp_Tracked_Count++];

				Entry.Slot = Slot;

				Entry.Kind = Kind;
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					continue;
				}
			}

			Item_Esp_Valid = true;
		}

		if (Item_Esp_Tracked_Count == 0)
		{
			return;
		}

		ImDrawList* Draw_List = ImGui::GetBackgroundDrawList();

		if (Draw_List == nullptr)
		{
			return;
		}

		const ImVec2 Screen_Size = ImGui::GetIO().DisplaySize;

		for (__int32 i = 0; i < Item_Esp_Tracked_Count; i++)
		{
			__try
			{
			const Item_Esp_Tracked_Entity& Entry = Item_Esp_Tracked[i];

			void* Entity = Sdk_Get_Client_Entity_Safe(Entry.Slot);

			if (Entity == nullptr)
			{
				continue;
			}

			float Origin[3];

			if (Esp_Read_Origin(Entity, Origin) == false)
			{
				continue;
			}

			if (((Origin[0] != Origin[0]) || (Origin[1] != Origin[1]) || (Origin[2] != Origin[2])) ||
				(fabsf(Origin[0]) > 100000.f) || (fabsf(Origin[1]) > 100000.f) || (fabsf(Origin[2]) > 100000.f))
			{
				continue;
			}

			if (Entry.Kind == Item_Esp_Mounted)
			{
				if (Item_Esp_Mounted_Enabled == false)
				{
					continue;
				}

				const bool Draw_Text = Item_Esp_Mounted_Text;

				const bool Draw_Boxes = Item_Esp_Mounted_Boxes;

				if ((Draw_Text == false) && (Draw_Boxes == false))
				{
					continue;
				}

				const float Box_Height = 92.f;

				const float Box_Width = 72.f;

				const float Top[3] = { Origin[0], Origin[1], Origin[2] + Box_Height };

				float Feet[3];

				float Head[3];

				if ((Esp_Project(Origin, Feet) == false) || (Esp_Project(Top, Head) == false))
				{
					continue;
				}

				const float Height = Feet[1] - Head[1];

				if ((Height <= 1.f) || (Height > 2000.f))
				{
					continue;
				}

				const float X = Head[0] - Box_Width / 2.f;

				const float Y = Head[1];

				const float Width = Box_Width;

				const ImU32 Color = ImColor(Item_Esp_Mounted_Color[0], Item_Esp_Mounted_Color[1], Item_Esp_Mounted_Color[2], Item_Esp_Mounted_Color[3]);

				if (Draw_Boxes == true)
				{
					Draw_List->AddRect(ImVec2(X, Y), ImVec2(X + Width, Y + Height), Color);
				}

				if (Draw_Text == true)
				{
					char Text[96];

					const float Heat = Item_Esp_Read_Heat(Entity);

					if (Heat >= 0.f)
					{
						sprintf_s(Text, "mounted weapon - heat: %.1f", Heat);
					}
					else
					{
						sprintf_s(Text, "mounted weapon");
					}

					Draw_List->AddText(ImVec2(X, Y - 14.f), Color, Text);
				}
			}
			else
			{
				const __int32 Weapon_Id = Item_Esp_Get_Weapon_Id(Entity);

				const bool Heal = (Weapon_Id >= 0) && (Item_Esp_Is_Heal_Id(Weapon_Id) == true) && (Item_Esp_Heal_Enabled == true);

				const bool Draw_Text = Heal ? Item_Esp_Heal_Text : Item_Esp_Weapons_Text;

				const bool Draw_Boxes = Heal ? Item_Esp_Heal_Boxes : Item_Esp_Weapons_Boxes;

				if ((Draw_Text == false) && (Draw_Boxes == false))
				{
					continue;
				}

				if ((Heal == false) && (Item_Esp_Weapons_Text == false) && (Item_Esp_Weapons_Boxes == false))
				{
					continue;
				}

				const float Box_Height = 56.f;

				const float Box_Width = 46.f;

				const float Top[3] = { Origin[0], Origin[1], Origin[2] + Box_Height };

				float Feet[3];

				float Head[3];

				if ((Esp_Project(Origin, Feet) == false) || (Esp_Project(Top, Head) == false))
				{
					continue;
				}

				const float Height = Feet[1] - Head[1];

				if ((Height <= 1.f) || (Height > 2000.f))
				{
					continue;
				}

				const float X = Head[0] - Box_Width / 2.f;

				const float Y = Head[1];

				const float Width = Box_Width;

				const float* Color_Floats = Heal ? Item_Esp_Heal_Color : Item_Esp_Weapons_Color;

				const ImU32 Color = ImColor(Color_Floats[0], Color_Floats[1], Color_Floats[2], Color_Floats[3]);

				if (Draw_Boxes == true)
				{
					Draw_List->AddRect(ImVec2(X, Y), ImVec2(X + Width, Y + Height), Color);
				}

				if (Draw_Text == true)
				{
					const char* Name = ((Weapon_Id >= 0) && (Weapon_Id <= 38)) ? Item_Esp_Weapon_Names[Weapon_Id] : "Unknown";

					Draw_List->AddText(ImVec2(X, Y - 14.f), Color, Name);
				}
			}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				continue;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Item_Esp_Tracked_Count = 0;

		Item_Esp_Valid = false;

		Item_Esp_Updated_At = GetTickCount();
	}
}