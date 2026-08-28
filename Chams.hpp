#pragma once

#include <windows.h>

#include "Internal.hpp"

#include "D3D9.hpp"

struct Draw_Model_State_Structure
{
	char Padding[0x100];
};

struct Model_Render_Info_Structure
{
	char Padding[0x18];
	void* Renderable;
	void* Model;
	void* Model_To_World;
	void* Lighting_Offset;
	void* Lighting_Origin;
	__int32 Flags;
	__int32 Entity_Index;
};

typedef void(__fastcall* Draw_Model_Execute_Type)(void* Ecx, void* Edx, const Draw_Model_State_Structure& State, const Model_Render_Info_Structure& Info, void* Bone_To_World);

extern Draw_Model_Execute_Type Original_Draw_Model_Execute;

typedef void*(__fastcall* Find_Material_Type)(void* Ecx, void* Edx, const char* Name, const char* Group, bool Complain, const char* Prefix);

typedef void(__fastcall* Color_Modulate_Type)(void* Ecx, void* Edx, float R, float G, float B);

typedef void(__fastcall* Set_Material_Var_Flag_Type)(void* Ecx, void* Edx, __int32 Flag, bool On);

typedef void(__fastcall* Forced_Override_Type)(void* Ecx, void* Edx, void* Material, __int32 Override_Type);

static const __int32 Material_Var_Ignore_Z = (1 << 15);

static const __int32 Material_Var_No_Cull = (1 << 13);

static const __int32 Material_Var_No_Fog = (1 << 14);

static const __int32 Material_Var_No_Draw = (1 << 2);

static void* Material_System;

static void* Chams_Material;

static Find_Material_Type Material_Find;

static Color_Modulate_Type Material_Color;

static Set_Material_Var_Flag_Type Material_Flag;

static void* No_Vomit_Material;

static bool No_Vomit_Tried;

static void* Find_Interface(HMODULE Module, const char* Name)
{
	typedef void* (__cdecl* Create_Interface_Type)(const char* Name, __int32* Return_Code);

	const Create_Interface_Type Factory = (Create_Interface_Type)GetProcAddress(Module, "CreateInterface");

	if (Factory == nullptr)
	{
		return nullptr;
	}

	return Factory(Name, nullptr);
}

static void* Find_Model_Render_Interface()
{
	void* Interface = Find_Interface(Engine_Module, "VEngineModel016");

	if (Interface != nullptr)
	{
		return Interface;
	}


	typedef void* (__cdecl* Create_Interface_Type)(const char* Name, __int32* Return_Code);

	const Create_Interface_Type Factory = (Create_Interface_Type)GetProcAddress(Engine_Module, "CreateInterface");

	if (Factory == nullptr)
	{
		return nullptr;
	}

	const unsigned __int32 Base = (unsigned __int32)Engine_Module;

	const IMAGE_DOS_HEADER* Dos = (const IMAGE_DOS_HEADER*)Base;

	if (Dos->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return nullptr;
	}

	const IMAGE_NT_HEADERS* Nt = (const IMAGE_NT_HEADERS*)(Base + Dos->e_lfanew);

	const unsigned __int32 Size = Nt->OptionalHeader.SizeOfImage;

	static const char Pattern[] = "ModelRender";

	for (unsigned __int32 i = Base; i + sizeof(Pattern) - 1 < Base + Size; i++)
	{
		if (*(unsigned __int32*)i != 0x6C646F4D)
		{
			continue;
		}

		if (memcmp((void*)i, Pattern, sizeof(Pattern) - 1) != 0)
		{
			continue;
		}

		char* Start = (char*)i;

		while ((Start > (char*)Base) && (*(Start - 1) > 0x1F) && (*(Start - 1) < 0x7F))
		{
			Start--;
		}

		void* Result = Factory(Start, nullptr);

		if (Result != nullptr)
		{

			return Result;
		}
	}

	return nullptr;
}

static const char* Chams_Style_Flat[] =
{
	"debug/debugambientcube",
	"customchamsmeow/mask_write",
	"customchamsmeow/HeavenCustomMaterial",
	"dev/glow_color",
};

static const char* Chams_Style_Translucent[] =
{
	"debug/debugtranslucentsinglecolor",
};

static const char* Chams_Style_Glow[] =
{
	"dev/glow_color",
};

static const char* Chams_Style_Wireframe[] =
{
	"debug/debugworldwireframezbuffer",
};

static const char* Chams_Style_XRay[] =
{
	"effects/flashbang_white",
};

static const char* Chams_Style_Mirror[] =
{
	"effects/flashbang",
};

static const char* Chams_Style_Chrome[] =
{
	"models/gibs/glass/glass",
};

static const char* Chams_Style_Laser[] =
{
	"models/infected/common/l4d2/cim_ceda_faceplate",
};

static bool Chams_Init()
{
	static int Last_Style = -1;

	static bool Init_Tried;

	if (Chams_Style != Last_Style)
	{
		Last_Style = Chams_Style;

		Chams_Material = nullptr;

		Init_Tried = false;
	}

	if (Init_Tried == true)
	{
		if (Chams_Material != nullptr)
		{
			__try
			{
				void** Vtab = *(void***)Chams_Material;
				if ((Vtab == nullptr) || ((unsigned __int32)Vtab < 0x10000))
				{
					Chams_Material = nullptr;
					Init_Tried = false;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				Chams_Material = nullptr;
				Init_Tried = false;
			}
		}
	}

	if (Init_Tried == true)
	{
		return Chams_Material != nullptr;
	}

	Init_Tried = true;

	if (Chams_Material != nullptr)
	{
		return true;
	}

	if (Material_System == nullptr)
	{
		Material_System = Find_Interface(LoadLibraryW(L"materialsystem.dll"), "VMaterialSystem080");

		if (Material_System == nullptr)
		{
			Material_System = Find_Interface(LoadLibraryW(L"materialsystem.dll"), "VMaterialSystem");
		}

		if (Material_System == nullptr)
		{

			return false;
		}

		Material_Find = (Find_Material_Type)(*(void***)Material_System)[71];
	}

	const char** Candidates = Chams_Style_Flat;

	unsigned __int32 Count = sizeof(Chams_Style_Flat) / sizeof(Chams_Style_Flat[0]);

	switch (Chams_Style)
	{
		case 1:
			Candidates = Chams_Style_Translucent;

			Count = sizeof(Chams_Style_Translucent) / sizeof(Chams_Style_Translucent[0]);

			break;

		case 2:
			Candidates = Chams_Style_Glow;

			Count = sizeof(Chams_Style_Glow) / sizeof(Chams_Style_Glow[0]);

			break;

		case 3:
			Candidates = Chams_Style_Wireframe;

			Count = sizeof(Chams_Style_Wireframe) / sizeof(Chams_Style_Wireframe[0]);

			break;

		case 4:
			Candidates = Chams_Style_XRay;

			Count = sizeof(Chams_Style_XRay) / sizeof(Chams_Style_XRay[0]);

			break;

		case 5:
			Candidates = Chams_Style_Mirror;

			Count = sizeof(Chams_Style_Mirror) / sizeof(Chams_Style_Mirror[0]);

			break;

		case 6:
			Candidates = Chams_Style_Chrome;

			Count = sizeof(Chams_Style_Chrome) / sizeof(Chams_Style_Chrome[0]);

			break;

		case 7:
			Candidates = Chams_Style_Laser;

			Count = sizeof(Chams_Style_Laser) / sizeof(Chams_Style_Laser[0]);

			break;
	}

	for (unsigned __int32 i = 0; i < Count; i++)
	{
		Chams_Material = Material_Find(Material_System, 0, Candidates[i], "Model textures", false, nullptr);

		if (Chams_Material != nullptr)
		{

			break;
		}
	}

	if (Chams_Material == nullptr)
	{

		return false;
	}

	Material_Color = (Color_Modulate_Type)(*(void***)Chams_Material)[28];

	Material_Flag = (Set_Material_Var_Flag_Type)(*(void***)Chams_Material)[29];

	return true;
}

static void Chams_Apply_Material(__int32 Team, bool Ignore_Z)
{
	const float* Color = (Team == 2) ? Chams_Team2_Color : Chams_Team3_Color;

	Material_Flag(Chams_Material, 0, Material_Var_Ignore_Z, Ignore_Z);

	Material_Flag(Chams_Material, 0, Material_Var_No_Cull, true);

	Material_Flag(Chams_Material, 0, Material_Var_No_Fog, true);

	Material_Color(Chams_Material, 0, Color[0], Color[1], Color[2]);

	((Forced_Override_Type)(*(void***)Model_Render)[1])(Model_Render, 0, Chams_Material, 0);
}

static void __fastcall Draw_Model_Execute_Hook(void* Ecx, void* Edx, const Draw_Model_State_Structure& State, const Model_Render_Info_Structure& Info, void* Bone_To_World)
{
	static bool Debug_Logged;

	__try
	{
		if (No_Vomit_Tried == false)
		{
			No_Vomit_Tried = true;

			Chams_Init();

			if (Material_System != nullptr)
			{
				No_Vomit_Material = Material_Find(Material_System, 0, "particle/screenspaceboomervomit", "Particle textures", false, nullptr);

				if (No_Vomit_Material != nullptr)
				{

					Material_Flag = (Set_Material_Var_Flag_Type)(*(void***)No_Vomit_Material)[29];
				}
			}
		}

		if (No_Vomit_Material != nullptr)
		{
			Material_Flag(No_Vomit_Material, 0, Material_Var_No_Draw, No_Vomit_Enabled);
		}
	}
	__except (GetExceptionCode() == 0xC0000005 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
	}

	if (Chams_Enabled == true)
	{
		__try
		{
			const __int32 Entity_Index = Info.Entity_Index;

			if ((Entity_Index > 0) && (Info.Model != nullptr) && (Chams_Init() == true))
			{
				void* Entity = Get_Client_Entity(Entity_List, 0, Entity_Index);

				if ((Entity != nullptr) && (Entity != (void*)Get_Local_Player()))
				{
					const __int32 Team = *(__int32*)((unsigned __int32)Entity + 0xE4);

					if ((Team == 2) || (Team == 3))
					{
						if (Debug_Logged == false)
						{
							Debug_Logged = true;

						}

						if (Chams_X_Ray == true)
						{
							Chams_Apply_Material(Team, true);

							Original_Draw_Model_Execute(Ecx, Edx, State, Info, Bone_To_World);
						}

						Chams_Apply_Material(Team, false);
					}
				}
			}
		}
		__except (GetExceptionCode() == 0xC0000005 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
		}
	}

	Original_Draw_Model_Execute(Ecx, Edx, State, Info, Bone_To_World);

	((Forced_Override_Type)(*(void***)Model_Render)[1])(Model_Render, 0, nullptr, 0);
}

static void Install_Chams()
{
	Model_Render = Find_Model_Render_Interface();

	if (Model_Render == nullptr)
	{

		return;
	}

	void** Vtable = *(void***)Model_Render;

	Original_Draw_Model_Execute = (Draw_Model_Execute_Type)Vtable[19];

	DWORD Previous_Access_Rights;

	VirtualProtect(&Vtable[19], sizeof(void*), PAGE_READWRITE, &Previous_Access_Rights);

	Vtable[19] = (void*)Draw_Model_Execute_Hook;

	VirtualProtect(&Vtable[19], sizeof(void*), Previous_Access_Rights, &Previous_Access_Rights);

}