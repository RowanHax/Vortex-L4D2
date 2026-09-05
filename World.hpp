#pragma once

#include <windows.h>

#include <string.h>

#include <stdio.h>

#include <math.h>

#include "Internal.hpp"

#include "Sdk.hpp"

typedef void* (__thiscall* World_Find_Var_Type)(void* This, const char* Name);

typedef void(__thiscall* World_Set_Value_Int_Type)(void* This, __int32 Value);

typedef __int32(__thiscall* World_Enum_Materials_Type)(void* This);

typedef void* (__thiscall* World_Get_Material_Type)(void* This, __int32 Index);

typedef const char* (__thiscall* World_Material_Get_Name_Type)(void* This);

typedef void(__thiscall* World_Material_Color_Type)(void* This, float R, float G, float B);

struct World_Cached_Material
{
	void* Material;

	char Name[160];

	float Last_R;

	float Last_G;

	float Last_B;

	bool Applied;

	bool Is_Sky;

	bool Is_World;
};

static void* World_Cvar_Interface;

static bool World_Cvar_Tried;

static World_Cached_Material World_Materials[2048];

static __int32 World_Material_Count;

static bool World_Materials_Tried;

static unsigned __int32 World_Materials_Generation;

static void* World_Cvar_Find_Var(const char* Name)
{
	__try
	{
		if (World_Cvar_Tried == false)
		{
			World_Cvar_Tried = true;

			World_Cvar_Interface = Get_Interface(GetModuleHandleW(L"vstdlib.dll"), "VEngineCvar007");

			if (World_Cvar_Interface == nullptr)
			{
				World_Cvar_Interface = Get_Interface(GetModuleHandleW(L"vstdlib.dll"), "VEngineCvar");
			}
		}

		if ((World_Cvar_Interface == nullptr) || (Name == nullptr))
		{
			return nullptr;
		}

		void** Vtable = *(void***)World_Cvar_Interface;

		const HMODULE VStd = GetModuleHandleW(L"vstdlib.dll");

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 14 * sizeof(void*)) == false) ||
			(Vtable[13] == nullptr) || (Sdk_Address_In_Module(VStd, (unsigned __int32)Vtable[13]) == false))
		{
			return nullptr;
		}

		return ((World_Find_Var_Type)Vtable[13])(World_Cvar_Interface, Name);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static bool World_Cvar_Set_Int(void* Var, __int32 Value)
{
	__try
	{
		if (Var == nullptr)
		{
			return false;
		}

		void* Object = (unsigned __int8*)Var + 0x18;

		void** Vtable = *(void***)Object;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, sizeof(void*)) == false) || (Vtable[0] == nullptr))
		{
			return false;
		}

		((World_Set_Value_Int_Type)Vtable[0])(Object, Value);

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static void* World_Fullbright_Cvar;

static bool World_Fullbright_Cvar_Tried;

static void* World_Flashlight_Fov_Cvar;

static bool World_Flashlight_Fov_Cvar_Tried;

static void* World_Flashlight_Constant_Cvar;

static bool World_Flashlight_Constant_Cvar_Tried;

static void World_Update_Cvars()
{
	__try
	{
		if ((World_Enabled == false) && (World_Fullbright_Cvar == nullptr) && (World_Flashlight_Fov_Cvar == nullptr))
		{
			return;
		}

		if (World_Fullbright_Cvar_Tried == false)
		{
			World_Fullbright_Cvar_Tried = true;

			if (World_Fullbright_Cvar == nullptr)
			{
				World_Fullbright_Cvar = World_Cvar_Find_Var("mat_fullbright");
			}
		}

		if (World_Flashlight_Fov_Cvar_Tried == false)
		{
			World_Flashlight_Fov_Cvar_Tried = true;

			if (World_Flashlight_Fov_Cvar == nullptr)
			{
				World_Flashlight_Fov_Cvar = World_Cvar_Find_Var("r_flashlightfov");
			}
		}

		if (World_Flashlight_Constant_Cvar_Tried == false)
		{
			World_Flashlight_Constant_Cvar_Tried = true;

			if (World_Flashlight_Constant_Cvar == nullptr)
			{
				World_Flashlight_Constant_Cvar = World_Cvar_Find_Var("r_flashlightconstant");
			}
		}

		const bool Active = World_Enabled == true;

		if ((Active == true) && (World_Fullbright_Enabled == true))
		{
			World_Cvar_Set_Int(World_Fullbright_Cvar, 1);
		}
		else if (World_Fullbright_Cvar != nullptr)
		{
			World_Cvar_Set_Int(World_Fullbright_Cvar, 0);
		}

		if ((Active == true) && (World_Full_Flashlight_Enabled == true))
		{
			World_Cvar_Set_Int(World_Flashlight_Fov_Cvar, (__int32)World_Flashlight_Fov);

			World_Cvar_Set_Int(World_Flashlight_Constant_Cvar, 0);
		}
		else
		{
			if (World_Flashlight_Fov_Cvar != nullptr)
			{
				World_Cvar_Set_Int(World_Flashlight_Fov_Cvar, 90);
			}

			if (World_Flashlight_Constant_Cvar != nullptr)
			{
				World_Cvar_Set_Int(World_Flashlight_Constant_Cvar, 1);
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

struct World_Fog_Offsets
{
	__int32 Enable;

	__int32 Blend;

	__int32 Color_Primary;

	__int32 Color_Secondary;

	__int32 Start;

	__int32 End;

	__int32 Max_Density;

	bool Resolved;

	bool Complete;
};

static World_Fog_Offsets World_Fog;

static __int32 World_Find_Dotted_Prop(Recv_Table_Structure* Table, const char* Path, __int32 Extra_Offset, __int32 Depth)
{
	__try
	{
		if ((Depth > 64) || (Table == nullptr) || (Path == nullptr) || (Path[0] == 0) ||
			(Sdk_Is_Readable_Range(Table, sizeof(Recv_Table_Structure)) == false) ||
			(Table->Props == nullptr) || (Table->Prop_Count <= 0) || (Table->Prop_Count > 8192))
		{
			return -1;
		}

		const char* Dot = strchr(Path, '.');

		const SIZE_T Token_Length = (Dot != nullptr) ? (SIZE_T)(Dot - Path) : strlen(Path);

		for (__int32 i = 0; i < Table->Prop_Count; i++)
		{
			Recv_Prop_Structure* Prop = &Table->Props[i];

			if ((Prop == nullptr) || (Sdk_Is_Readable_Range(Prop, sizeof(Recv_Prop_Structure)) == false) || (Prop->Var_Name == nullptr))
			{
				continue;
			}

			if (Sdk_Is_Readable_Range(Prop->Var_Name, 64) == false)
			{
				continue;
			}

			const SIZE_T Name_Length = strlen(Prop->Var_Name);

			if ((Name_Length == strlen(Path)) && (strncmp(Prop->Var_Name, Path, Name_Length) == 0))
			{
				return Prop->Offset + Extra_Offset;
			}

			if ((Dot != nullptr) && (Name_Length == Token_Length) && (strncmp(Prop->Var_Name, Path, Token_Length) == 0))
			{
				Recv_Table_Structure* Child = Prop->Data_Table;

				if (Child != nullptr)
				{
					const __int32 Found = World_Find_Dotted_Prop(Child, Dot + 1, Prop->Offset + Extra_Offset, Depth + 1);

					if (Found >= 0)
					{
						return Found;
					}
				}
			}

			Recv_Table_Structure* Child_Table = Prop->Data_Table;

			if ((Child_Table != nullptr) && (Sdk_Is_Readable_Range(Child_Table, sizeof(Recv_Table_Structure)) == true) && (Child_Table->Prop_Count > 0))
			{
				const __int32 Found = World_Find_Dotted_Prop(Child_Table, Path, Prop->Offset + Extra_Offset, Depth + 1);

				if (Found >= 0)
				{
					return Found;
				}
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return -1;
}

static __int32 World_Get_Fog_Field(const char* Field)
{
	__try
	{
		Client_Class_Structure* Chain = Get_Client_Class_Chain();

		if (Chain == nullptr)
		{
			return -1;
		}

		__int32 Class_Count = 0;

		for (Client_Class_Structure* Class = Chain; (Class != nullptr) && (Class_Count < 4096); Class = Class->Next, Class_Count++)
		{
			if (Sdk_Is_Readable_Range(Class, sizeof(Client_Class_Structure)) == false)
			{
				break;
			}

			Recv_Table_Structure* Table = Class->Recv_Table;

			if ((Table == nullptr) || (Table->Table_Name == nullptr))
			{
				continue;
			}

			const __int32 Offset = World_Find_Dotted_Prop(Table, Field, 0, 0);

			if (Offset >= 0)
			{
				return Offset;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return -1;
}

static void World_Resolve_Fog_Offsets()
{
	if (World_Fog.Resolved == true)
	{
		return;
	}

	World_Fog.Resolved = true;

	World_Fog.Enable = World_Get_Fog_Field("m_skybox3d.fog.enable");

	World_Fog.Blend = World_Get_Fog_Field("m_skybox3d.fog.blend");

	World_Fog.Color_Primary = World_Get_Fog_Field("m_skybox3d.fog.colorPrimary");

	World_Fog.Color_Secondary = World_Get_Fog_Field("m_skybox3d.fog.colorSecondary");

	World_Fog.Start = World_Get_Fog_Field("m_skybox3d.fog.start");

	World_Fog.End = World_Get_Fog_Field("m_skybox3d.fog.end");

	World_Fog.Max_Density = World_Get_Fog_Field("m_skybox3d.fog.maxdensity");

	World_Fog.Complete = ((World_Fog.Enable >= 0) && (World_Fog.Blend >= 0) && (World_Fog.Color_Primary >= 0) &&
		(World_Fog.Color_Secondary >= 0) && (World_Fog.Start >= 0) && (World_Fog.End >= 0) && (World_Fog.Max_Density >= 0));
}

static void World_Hsv_To_Rgb(float H, float S, float V, float* R, float* G, float* B)
{
	H = fmodf(H, 360.f);

	if (H < 0.f)
	{
		H += 360.f;
	}

	const float Sector = H / 60.f;

	const __int32 i = (__int32)Sector;

	const float F = Sector - (float)i;

	const float P = V * (1.f - S);

	const float Q = V * (1.f - S * F);

	const float T = V * (1.f - S * (1.f - F));

	switch (i % 6)
	{
	case 0: *R = V; *G = T; *B = P; break;
	case 1: *R = Q; *G = V; *B = P; break;
	case 2: *R = P; *G = V; *B = T; break;
	case 3: *R = P; *G = Q; *B = V; break;
	case 4: *R = T; *G = P; *B = V; break;
	default: *R = V; *G = P; *B = Q; break;
	}
}

static void World_Rainbow_Color(float Speed, float* R, float* G, float* B)
{
	if (Speed < 0.01f)
	{
		Speed = 1.f;
	}

	const float Hue = (float)(GetTickCount64() % 36000ULL) / 100.f * Speed;

	World_Hsv_To_Rgb(Hue, 1.f, 1.f, R, G, B);
}

static void World_Update_Fog()
{
	if (World_Enabled == false)
	{
		return;
	}

	__try
	{
		if (((World_No_Fog_Enabled == false) && (World_Custom_Fog_Enabled == false) && (World_Nightmode == false)))
		{
			return;
		}

		World_Resolve_Fog_Offsets();

		if (World_Fog.Complete == false)
		{
			return;
		}

		void* Local_Player = Sdk_Get_Local_Player_Safe();

		if ((Local_Player == nullptr) || (Sdk_Is_Readable_Range((unsigned __int8*)Local_Player, 0x400) == false))
		{
			return;
		}

		const bool Disable = ((World_No_Fog_Enabled == true) || (World_Nightmode == true)) && (World_Custom_Fog_Enabled == false);

		*(unsigned __int8*)((unsigned __int8*)Local_Player + World_Fog.Enable) = (unsigned __int8)(Disable ? 0 : 1);

		if (Disable == true)
		{
			return;
		}

		*(unsigned __int8*)((unsigned __int8*)Local_Player + World_Fog.Blend) = (unsigned __int8)(World_Blend_Fog_Enabled ? 1 : 0);

		float* Primary = (float*)((unsigned __int8*)Local_Player + World_Fog.Color_Primary);

		float* Secondary = (float*)((unsigned __int8*)Local_Player + World_Fog.Color_Secondary);

		float Inject_R = 0.f;

		float Inject_G = 0.f;

		float Inject_B = 0.f;

		if (World_Fog_Rainbow_Enabled == true)
		{
			World_Rainbow_Color(World_Fog_Rainbow_Speed, &Inject_R, &Inject_G, &Inject_B);
		}

		Primary[0] = (World_Fog_Rainbow_Enabled == true) ? Inject_R : World_Fog_Primary_Color[0];

		Primary[1] = (World_Fog_Rainbow_Enabled == true) ? Inject_G : World_Fog_Primary_Color[1];

		Primary[2] = (World_Fog_Rainbow_Enabled == true) ? Inject_B : World_Fog_Primary_Color[2];

		Secondary[0] = World_Fog_Secondary_Color[0];

		Secondary[1] = World_Fog_Secondary_Color[1];

		Secondary[2] = World_Fog_Secondary_Color[2];

		*(float*)((unsigned __int8*)Local_Player + World_Fog.Start) = World_Fog_Start;

		*(float*)((unsigned __int8*)Local_Player + World_Fog.End) = World_Fog_End;

		*(float*)((unsigned __int8*)Local_Player + World_Fog.Max_Density) = World_Fog_Density;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void* World_Material_System;

static bool World_Material_System_Tried;

static World_Enum_Materials_Type World_Enum_Materials;

static World_Enum_Materials_Type World_Enum_Materials_End;

static World_Get_Material_Type World_Get_Material;

static bool World_Is_World_Material(const char* Name)
{
	if ((Name == nullptr) || (Name[0] == 0))
	{
		return false;
	}

	if (strstr(Name, "models/weapons/") != nullptr)
	{
		return false;
	}

	if (strstr(Name, "v_arms") != nullptr)
	{
		return false;
	}

	if (strstr(Name, "v_models") != nullptr)
	{
		return false;
	}

	return ((strncmp(Name, "models/", 7) == 0) || (strncmp(Name, "world/", 6) == 0) || (strncmp(Name, "nature/", 7) == 0));
}

static bool World_Is_Sky_Material(const char* Name)
{
	if ((Name == nullptr) || (Name[0] == 0))
	{
		return false;
	}

	return (strstr(Name, "skybox/") != nullptr);
}

static void World_Build_Material_Cache()
{
	__try
	{
		if (World_Material_System_Tried == false)
		{
			World_Material_System_Tried = true;

			World_Material_System = Get_Interface(GetModuleHandleW(L"materialsystem.dll"), "VMaterialSystem080");

			if (World_Material_System == nullptr)
			{
				World_Material_System = Get_Interface(GetModuleHandleW(L"materialsystem.dll"), "VMaterialSystem");
			}
		}

		if (World_Material_System == nullptr)
		{
			return;
		}

		void** Vtable = *(void***)World_Material_System;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 76 * sizeof(void*)) == false))
		{
			return;
		}

		if ((Vtable[72] == nullptr) || (Vtable[74] == nullptr) || (Vtable[75] == nullptr))
		{
			return;
		}

		World_Enum_Materials = (World_Enum_Materials_Type)Vtable[72];

		World_Enum_Materials_End = (World_Enum_Materials_Type)Vtable[74];

		World_Get_Material = (World_Get_Material_Type)Vtable[75];

		const __int32 Start = World_Enum_Materials(World_Material_System);

		__int32 End = World_Enum_Materials_End(World_Material_System);

		if (End <= 0)
		{
			End = 4096;
		}

		if ((Start < 0) || (Start > 4096) || (End < Start))
		{
			return;
		}

		World_Material_Count = 0;

		for (__int32 i = Start; i < End; i++)
		{
			if (World_Material_Count >= 2048)
			{
				break;
			}

			void* Material = World_Get_Material(World_Material_System, i);

			if (Material == nullptr)
			{
				continue;
			}

			void** Material_Vtable = *(void***)Material;

			if ((Material_Vtable == nullptr) || (Sdk_Is_Readable_Range(Material_Vtable, 2 * sizeof(void*)) == false) || (Material_Vtable[0] == nullptr))
			{
				continue;
			}

			const char* Name = ((World_Material_Get_Name_Type)Material_Vtable[0])(Material);

			if ((Name == nullptr) || (Sdk_Is_Readable_Range(Name, 4) == false) || (Name[0] == 0))
			{
				continue;
			}

			World_Cached_Material& Entry = World_Materials[World_Material_Count];

			Entry.Material = Material;

			Entry.Last_R = 1.f;

			Entry.Last_G = 1.f;

			Entry.Last_B = 1.f;

			Entry.Applied = false;

			Entry.Is_Sky = World_Is_Sky_Material(Name);

			Entry.Is_World = World_Is_World_Material(Name);

			strncpy_s(Entry.Name, Name, sizeof(Entry.Name) - 1);

			World_Material_Count++;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static bool World_Use_Materials;

static void World_Update_Materials()
{
	__try
	{
		const bool Modulate = ((World_Enabled == true) && ((World_Nightmode == true) || (World_Sky_Color_Enabled == true) || (World_World_Color_Enabled == true)));

		if (Modulate == true)
		{
			World_Use_Materials = true;
		}

		if (World_Use_Materials == false)
		{
			return;
		}

		const unsigned __int32 Generation = Sdk_Get_Game_Session_Generation();

		if (World_Materials_Tried == false)
		{
			World_Materials_Tried = true;

			World_Materials_Generation = Generation;

			World_Build_Material_Cache();
		}
		else if (Generation != World_Materials_Generation)
		{
			World_Materials_Generation = Generation;

			World_Build_Material_Cache();
		}

		for (__int32 i = 0; i < World_Material_Count; i++)
		{
			World_Cached_Material& Entry = World_Materials[i];

			if ((Entry.Material == nullptr) || (Sdk_Is_Readable_Range(Entry.Material, sizeof(void*)) == false))
			{
				continue;
			}

			if ((Entry.Is_Sky == false) && (Entry.Is_World == false))
			{
				continue;
			}

			float R = 1.f;

			float G = 1.f;

			float B = 1.f;

			if (Modulate == true)
			{
				const bool Use_Sky = ((World_Nightmode == true) || (World_Sky_Color_Enabled == true)) && (Entry.Is_Sky == true);

				const bool Use_World = (World_Nightmode == true) || (World_World_Color_Enabled == true);

				if (Use_Sky == true)
				{
					if (World_Nightmode == true)
					{
						R = 0.04f;

						G = 0.04f;

						B = 0.06f;
					}
					else
					{
						R = World_Sky_Color[0];

						G = World_Sky_Color[1];

						B = World_Sky_Color[2];
					}
				}
				else if ((Use_World == true) && (Entry.Is_World == true))
				{
					if (World_Nightmode == true)
					{
						R = 0.10f;

						G = 0.10f;

						B = 0.12f;
					}
					else
					{
						R = World_World_Color[0];

						G = World_World_Color[1];

						B = World_World_Color[2];
					}
				}
				else
				{
					continue;
				}
			}

			if ((Entry.Applied == true) && (fabsf(R - Entry.Last_R) < 0.001f) && (fabsf(G - Entry.Last_G) < 0.001f) && (fabsf(B - Entry.Last_B) < 0.001f))
			{
				continue;
			}

			void** Vtable = *(void***)Entry.Material;

			if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 29 * sizeof(void*)) == false) || (Vtable[28] == nullptr))
			{
				continue;
			}

			((World_Material_Color_Type)Vtable[28])(Entry.Material, R, G, B);

			Entry.Last_R = R;

			Entry.Last_G = G;

			Entry.Last_B = B;

			Entry.Applied = true;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}