#pragma once

#include <math.h>
#include <windows.h>

#include "Internal.hpp"
#include "D3D9.hpp"
#include "Sdk.hpp"

extern bool    Vortex_Aimbot_Enabled;
extern __int32 Vortex_Aimbot_Hitbox;
extern float   Vortex_Aimbot_Fov;
extern float   Vortex_Aimbot_Smooth;
extern float   Vortex_Aimbot_Distance;
extern bool    Vortex_Aimbot_Silent;
extern bool    Vortex_Aimbot_Visible;
extern bool    Vortex_Aimbot_Auto_Fire;
extern bool    Vortex_Aimbot_Prediction;
extern bool    Vortex_Aimbot_Prioritize_Players;
extern bool    Vortex_Aimbot_Ignore_Common;
extern bool    Vortex_Aimbot_Ignore_Tank;
extern bool    Vortex_Aimbot_Ignore_Witch;

static bool Rage_Bot_Shoot_Toggle = false;

static void Vortex_Calculate_Aim(const float Eye[3], const float Target[3], float Out[2])
{
	const float Delta_X = Target[0] - Eye[0];
	const float Delta_Y = Target[1] - Eye[1];
	const float Delta_Z = Target[2] - Eye[2];
	const float Horizontal = sqrtf(Delta_X * Delta_X + Delta_Y * Delta_Y);

	Out[0] = atan2f(-Delta_Z, Horizontal) * 180.f / 3.14159265f;
	Out[1] = atan2f(Delta_Y, Delta_X) * 180.f / 3.14159265f;
}

static float Vortex_Angles_Field_Of_View(const float View[2], const float Aim[2])
{
	const float Delta_X = Aim[0] - View[0];
	float Delta_Y = Aim[1] - View[1];

	while (Delta_Y > 180.f)
		Delta_Y -= 360.f;
	while (Delta_Y < -180.f)
		Delta_Y += 360.f;

	return sqrtf(Delta_X * Delta_X + Delta_Y * Delta_Y);
}

static float Vortex_Get_Aim_Height(__int32 Kind, __int32 Chest)
{
	switch (Kind)
	{
	case 5:  return Chest ? 55.f : 72.f;
	case 7:  return Chest ? 30.f : 52.f;
	case 6:  return Chest ? 30.f : 52.f;
	case 11: return Chest ? 36.f : 66.f;
	case 9:  return Chest ? 36.f : 66.f;
	case 8:  return Chest ? 60.f : 100.f;
	}
	return Chest ? 40.f : 72.f;
}


static __int32 Vortex_Get_Target_Priority(__int32 Kind)
{
	if (Vortex_Aimbot_Prioritize_Players == true)
	{
		if (Kind == Kind_Survivor)
			return 2;
		if (Kind == Kind_Common)
			return 1;
		return 0;
	}

	if (Kind == Kind_Common)
		return 2;
	if (Kind == Kind_Survivor)
		return 1;
	return 0;
}

typedef void* (__cdecl* Create_Interface_Type)(const char* Name, __int32* Return_Code);

static void* Vortex_Get_Interface(void* Module, const char* Name)
{
	if (Module == nullptr)
		return nullptr;

	Create_Interface_Type Create_Interface = (Create_Interface_Type)GetProcAddress((HMODULE)Module, "CreateInterface");
	if (Create_Interface == nullptr)
		return nullptr;

	void* Interface = Create_Interface(Name, nullptr);
	if (Interface != nullptr)
		return Interface;

	char Buffer[128];
	for (__int32 i = 1; i < 100; i++)
	{
		sprintf_s(Buffer, "%s%03i", Name, i);
		Interface = Create_Interface(Buffer, nullptr);
		if (Interface != nullptr)
			return Interface;
	}
	return nullptr;
}

struct __declspec(align(16)) Vortex_Ray_Structure
{
	float Start[4];
	float Delta[4];
	float Start_Offset[4];
	float Extents[4];
	void* World_Axis_Transform;
	bool  Is_Ray;
	bool  Is_Swept;
};

struct Vortex_Trace_Structure
{
	float  Start[3];
	float  End[3];
	float  Plane_Normal[3];
	float  Plane_Distance;
	unsigned __int8  Plane_Type;
	unsigned __int8  Plane_Signbits;
	unsigned __int8  Pad[2];
	float  Fraction;
	__int32 Contents;
	unsigned __int16 Disp_Flags;
	bool   All_Solid;
	bool   Start_Solid;
	float  Fraction_Left_Solid;
	void*  Surface_Name;
	__int16 Surface_Props;
	unsigned __int16 Surface_Flags;
	__int32 Hit_Group;
	__int16 Physics_Bone;
	unsigned __int16 Pad2;
	void*  Hit_Entity;
	__int32 Hitbox;
};

const unsigned __int32 VORTEX_MASK_SHOT = 0x44000007;

typedef void (__fastcall* Vortex_Trace_Ray_Type)(void* Ecx, void* Edx, const Vortex_Ray_Structure* Ray, unsigned __int32 Mask, void* Filter, Vortex_Trace_Structure* Trace);

static void* Vortex_Trace_Interface = nullptr;
static bool  Vortex_Trace_Tried = false;

static void* Vortex_Get_Trace_Interface()
{
	__try
	{
		if ((Vortex_Trace_Interface == nullptr) && (Vortex_Trace_Tried == false))
		{
			Vortex_Trace_Tried = true;
			Vortex_Trace_Interface = Vortex_Get_Interface(Engine_Module, "EngineTraceClient");
		}
		return Vortex_Trace_Interface;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

class Vortex_Trace_Filter
{
public:
	void* Skip_Entity;

	virtual bool ShouldHitEntity(void* Entity, int Contents_Mask)
	{
		return (Entity != Skip_Entity);
	}

	virtual __int32 GetTraceType() const
	{
		return 0;
	}
};

static __int32 Vortex_Trace_Ray_Index = -1;

static bool Vortex_Is_Visible(void* Local_Player, void* Target, const float Eye[3], const float Point[3])
{
	__try
	{
		if (Vortex_Get_Trace_Interface() == nullptr)
			return true;

		void** Vtable = *(void***)Vortex_Trace_Interface;
		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 6 * sizeof(void*)) == false) || (Vtable[5] == nullptr) || (Sdk_Address_In_Module(Engine_Module, (unsigned __int32)Vtable[5]) == false))
			return true;

		Vortex_Trace_Ray_Index = 5;

		Vortex_Ray_Structure Ray;
		memset(&Ray, 0, sizeof(Ray));
		Ray.Start[0] = Eye[0];
		Ray.Start[1] = Eye[1];
		Ray.Start[2] = Eye[2];
		Ray.Delta[0] = Point[0] - Eye[0];
		Ray.Delta[1] = Point[1] - Eye[1];
		Ray.Delta[2] = Point[2] - Eye[2];
		Ray.Is_Ray   = true;
		Ray.Is_Swept = true;

		Vortex_Trace_Filter Filter;
		Filter.Skip_Entity = Local_Player;

		Vortex_Trace_Structure Trace;
		memset(&Trace, 0, sizeof(Trace));

		((Vortex_Trace_Ray_Type)Vtable[Vortex_Trace_Ray_Index])(Vortex_Trace_Interface, nullptr, &Ray, VORTEX_MASK_SHOT, &Filter, &Trace);

		return ((Trace.Hit_Entity == Target) || (Trace.Fraction > 0.97f));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return true;
	}
}

static __int32 Vortex_Sim_Time_Offset = -1;

static __int32 Vortex_Velocity_Offset = -1;

static float Vortex_Get_Sim_Time(void* Entity)
{
	__try
	{
		if (Vortex_Sim_Time_Offset == -1)
		{
			Vortex_Sim_Time_Offset = Get_Net_Prop_Offset("DT_BaseEntity", "m_flSimulationTime");

			if (Vortex_Sim_Time_Offset < 0)
			{
				Vortex_Sim_Time_Offset = -2;
			}
		}

		if (Vortex_Sim_Time_Offset < 0)
		{
			return 0.f;
		}

		return *(float*)((unsigned __int32)Entity + Vortex_Sim_Time_Offset);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0.f;
	}
}

static void Vortex_Get_Velocity(void* Entity, float Out[3])
{
	Out[0] = 0.f;
	Out[1] = 0.f;
	Out[2] = 0.f;

	__try
	{
		if (Vortex_Velocity_Offset == -1)
		{
			Vortex_Velocity_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecVelocity[0]");

			if (Vortex_Velocity_Offset < 0)
			{
				Vortex_Velocity_Offset = -2;
			}
		}

		if (Vortex_Velocity_Offset < 0)
		{
			return;
		}

		const float* Velocity = (const float*)((unsigned __int32)Entity + Vortex_Velocity_Offset);

		Out[0] = Velocity[0];
		Out[1] = Velocity[1];
		Out[2] = Velocity[2];
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void Vortex_Apply_Prediction(void* Target, float Point[3])
{
	if ((Vortex_Aimbot_Prediction == false) || (Target == nullptr))
	{
		return;
	}

	__try
	{
		Global_Variables_Structure* Global_Variables = Sdk_Get_Global_Variables_Safe();

		if ((Global_Variables == nullptr) || (Global_Variables->Interval_Per_Tick <= 0.f) || (Global_Variables->Interval_Per_Tick > 0.1f))
		{
			return;
		}

		float Velocity[3];
		Vortex_Get_Velocity(Target, Velocity);

		for (__int32 Axis = 0; Axis < 3; Axis++)
		{
			if ((Velocity[Axis] != Velocity[Axis]) || (fabsf(Velocity[Axis]) > 4000.f))
			{
				return;
			}
		}

		const float Delay = Global_Variables->Interval_Per_Tick;
		float Offset[3] =
		{
			Velocity[0] * Delay,
			Velocity[1] * Delay,
			Velocity[2] * Delay
		};

		const float Offset_Length_Squared = Offset[0] * Offset[0] + Offset[1] * Offset[1] + Offset[2] * Offset[2];
		const float Max_Offset = 64.f;

		if (Offset_Length_Squared > (Max_Offset * Max_Offset))
		{
			const float Scale = Max_Offset / sqrtf(Offset_Length_Squared);
			Offset[0] *= Scale;
			Offset[1] *= Scale;
			Offset[2] *= Scale;
		}

		Point[0] += Offset[0];
		Point[1] += Offset[1];
		Point[2] += Offset[2];
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void Vortex_Vector_Transform(const float In[3], const float Matrix[3][4], float Out[3])
{
	Out[0] = In[0] * Matrix[0][0] + In[1] * Matrix[1][0] + In[2] * Matrix[2][0] + Matrix[0][3];
	Out[1] = In[0] * Matrix[0][1] + In[1] * Matrix[1][1] + In[2] * Matrix[2][1] + Matrix[1][3];
	Out[2] = In[0] * Matrix[0][2] + In[1] * Matrix[1][2] + In[2] * Matrix[2][2] + Matrix[2][3];
}

const unsigned __int32 VORTEX_BONE_USED_BY_HITBOX = 0x100;

static bool Vortex_Address_In_Module(HMODULE Module, unsigned __int32 Address)
{
	return Sdk_Address_In_Module(Module, Address);
}

static bool Vortex_Address_In_Client_Module(unsigned __int32 Address)
{
	return Vortex_Address_In_Module(Client_Module, Address);
}

static bool Vortex_Address_In_Engine_Module(unsigned __int32 Address)
{
	return Vortex_Address_In_Module(Engine_Module, Address);
}

static void* Vortex_Model_Info_Interface = nullptr;
static bool  Vortex_Model_Info_Tried = false;

static void* Vortex_Get_Studio_Model(void* Model)
{
	__try
	{
		if ((Vortex_Model_Info_Interface == nullptr) && (Vortex_Model_Info_Tried == false))
		{
			Vortex_Model_Info_Tried = true;
			Vortex_Model_Info_Interface = Vortex_Get_Interface(Engine_Module, "VModelInfoClient004");
			if (Vortex_Model_Info_Interface == nullptr)
				Vortex_Model_Info_Interface = Vortex_Get_Interface(Engine_Module, "VModelInfoClient");

		}

		if (Vortex_Model_Info_Interface == nullptr)
			return nullptr;

		void** Vtable = *(void***)Vortex_Model_Info_Interface;
		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 31 * sizeof(void*)) == false) || (Vtable[30] == nullptr) || (Vortex_Address_In_Engine_Module((unsigned __int32)Vtable[30]) == false))
			return nullptr;

		typedef void* (__fastcall* Get_Studio_Model_Type)(void* Ecx, void* Edx, void* Model);
		return ((Get_Studio_Model_Type)Vtable[30])(Vortex_Model_Info_Interface, nullptr, Model);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static void* Vortex_Prepare_Hitbox_Model(void* Entity, float Bone_Matrix[128][3][4])
{
	if ((Entity == nullptr) || (Bone_Matrix == nullptr))
	{
		return nullptr;
	}

	__try
	{
		if (Sdk_Is_Readable_Range(Entity, 0x0C) == false)
		{
			return nullptr;
		}

		void* Renderable = (void*)((unsigned __int8*)Entity + 0x4);

		if (Sdk_Is_Readable_Range(Renderable, sizeof(void*)) == false)
		{
			return nullptr;
		}

		void** Renderable_Vtable = *(void***)Renderable;

		if ((Renderable_Vtable == nullptr) || (Sdk_Is_Readable_Range(Renderable_Vtable, 14 * sizeof(void*)) == false) || (Renderable_Vtable[13] == nullptr) || (Renderable_Vtable[8] == nullptr))
		{
			return nullptr;
		}

		if ((Vortex_Address_In_Client_Module((unsigned __int32)Renderable_Vtable[13]) == false) ||
			(Vortex_Address_In_Client_Module((unsigned __int32)Renderable_Vtable[8]) == false))
		{
			return nullptr;
		}

		Global_Variables_Structure* Global_Variables = Sdk_Get_Global_Variables_Safe();

		if (Global_Variables == nullptr)
		{
			return nullptr;
		}

		memset(Bone_Matrix, 0, 128 * 3 * 4 * sizeof(float));

		typedef bool(__fastcall* Setup_Bones_Type)(void* Ecx, void* Edx, void* Bone_Matrix, __int32 Max_Bones, __int32 Bone_Mask, float Current_Time);
		const bool Bones_Ok = ((Setup_Bones_Type)Renderable_Vtable[13])(Renderable, nullptr, Bone_Matrix, 128, VORTEX_BONE_USED_BY_HITBOX, Global_Variables->Time);

		if (Bones_Ok == false)
		{
			return nullptr;
		}

		typedef void* (__fastcall* Get_Model_Type)(void* Ecx, void* Edx);
		void* Model = ((Get_Model_Type)Renderable_Vtable[8])(Renderable, nullptr);

		if (Model == nullptr)
		{
			return nullptr;
		}

		void* Studio_Hdr = Vortex_Get_Studio_Model(Model);

		if ((Studio_Hdr == nullptr) || (Sdk_Is_Readable_Range(Studio_Hdr, 0xB4) == false))
		{
			return nullptr;
		}

		if (*(unsigned __int32*)Studio_Hdr != 0x54534449)
		{
			return nullptr;
		}

		const __int32 Hitbox_Set_Count = *(__int32*)((unsigned __int8*)Studio_Hdr + 0xAC);
		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int8*)Studio_Hdr + 0xB0);

		if ((Hitbox_Set_Count <= 0) || (Hitbox_Set_Count > 32) || (Hitbox_Set_Index <= 0) || (Hitbox_Set_Index >= 0x1000000))
		{
			return nullptr;
		}

		return Studio_Hdr;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static __int32 Vortex_Find_Hitbox_By_Name(void* Studio_Hdr, const char* Name)
{
	__try
	{
		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int32)Studio_Hdr + 0xB0);
		if ((Hitbox_Set_Index <= 0) || (Hitbox_Set_Index >= 0x1000000))
			return -1;

		void* Hitbox_Set = (void*)((unsigned __int32)Studio_Hdr + Hitbox_Set_Index);
		const __int32 Num_Hitboxes = *(__int32*)((unsigned __int32)Hitbox_Set + 0x04);
		const __int32 Hitbox_Index  = *(__int32*)((unsigned __int32)Hitbox_Set + 0x08);

		if ((Num_Hitboxes <= 0) || (Num_Hitboxes >= 128) || (Hitbox_Index <= 0) || (Hitbox_Index >= 0x1000000))
			return -1;

		for (__int32 i = 0; i < Num_Hitboxes; i++)
		{
			void* Data = (void*)((unsigned __int32)Hitbox_Set + Hitbox_Index + i * 0x44);
			const __int32 Name_Index = *(__int32*)((unsigned __int32)Data + 0x20);

			if ((Name_Index <= 0) || (Name_Index >= 0x1000000))
				continue;

			const char* Box_Name = (const char*)((unsigned __int32)Studio_Hdr + Name_Index);
			if (_stricmp(Box_Name, Name) == 0)
				return i;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
	return -1;
}

static __int32 Vortex_Find_Head_Hitbox(void* Studio_Hdr, float Bone_Matrix[128][3][4])
{
	__try
	{
		const __int32 Found = Vortex_Find_Hitbox_By_Name(Studio_Hdr, "head");
		if (Found >= 0)
			return Found;

		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int32)Studio_Hdr + 0xB0);
		if ((Hitbox_Set_Index <= 0) || (Hitbox_Set_Index >= 0x1000000))
			return -1;

		void* Hitbox_Set = (void*)((unsigned __int32)Studio_Hdr + Hitbox_Set_Index);
		const __int32 Num_Hitboxes = *(__int32*)((unsigned __int32)Hitbox_Set + 0x04);
		const __int32 Hitbox_Index  = *(__int32*)((unsigned __int32)Hitbox_Set + 0x08);

		if ((Num_Hitboxes <= 0) || (Num_Hitboxes >= 128) || (Hitbox_Index <= 0) || (Hitbox_Index >= 0x1000000))
			return -1;

		__int32 Best = -1;
		float Best_Top = -99999.f;

		for (__int32 i = 0; i < Num_Hitboxes; i++)
		{
			void* Data = (void*)((unsigned __int32)Hitbox_Set + Hitbox_Index + i * 0x44);
			const __int32 Bone = *(__int32*)Data;
			if ((Bone < 0) || (Bone >= 128))
				continue;

			const float* BB_Max = (const float*)((unsigned __int32)Data + 0x14);
			float Max[3];
			Vortex_Vector_Transform(BB_Max, Bone_Matrix[Bone], Max);

			if (Max[2] > Best_Top)
			{
				Best_Top = Max[2];
				Best = i;
			}
		}
		return Best;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
}

static bool Vortex_Read_Hitbox(void* Studio_Hdr, float Bone_Matrix[128][3][4], __int32 Hitbox, float Out[3])
{
	__try
	{
		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int32)Studio_Hdr + 0xB0);
		void* Hitbox_Set = (void*)((unsigned __int32)Studio_Hdr + Hitbox_Set_Index);
		const __int32 Num_Hitboxes = *(__int32*)((unsigned __int32)Hitbox_Set + 0x04);
		const __int32 Hitbox_Index  = *(__int32*)((unsigned __int32)Hitbox_Set + 0x08);

		if ((Hitbox < 0) || (Hitbox >= Num_Hitboxes) || (Hitbox_Index <= 0) || (Hitbox_Index >= 0x1000000))
			return false;

		void* Hitbox_Data = (void*)((unsigned __int32)Hitbox_Set + Hitbox_Index + Hitbox * 0x44);
		const __int32 Bone = *(__int32*)Hitbox_Data;
		if ((Bone < 0) || (Bone >= 128))
			return false;

		const float* BB_Min = (const float*)((unsigned __int32)Hitbox_Data + 0x08);
		const float* BB_Max = (const float*)((unsigned __int32)Hitbox_Data + 0x14);

		float Min[3], Max[3];
		Vortex_Vector_Transform(BB_Min, Bone_Matrix[Bone], Min);
		Vortex_Vector_Transform(BB_Max, Bone_Matrix[Bone], Max);

		Out[0] = (Min[0] + Max[0]) * 0.5f;
		Out[1] = (Min[1] + Max[1]) * 0.5f;
		Out[2] = (Min[2] + Max[2]) * 0.5f;

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Vortex_Get_Aim_Position(void* Entity, __int32 Kind, __int32 Chest, float Out[3])
{
	if ((Entity == nullptr) || (Out == nullptr))
	{
		return false;
	}

	__try
	{
		float Bone_Matrix[128][3][4];
		void* Studio_Hdr = Vortex_Prepare_Hitbox_Model(Entity, Bone_Matrix);

		if (Studio_Hdr != nullptr)
		{
			if (Chest == 1)
			{
				const __int32 Hitbox = Vortex_Find_Hitbox_By_Name(Studio_Hdr, "chest");
				if ((Hitbox >= 0) && (Vortex_Read_Hitbox(Studio_Hdr, Bone_Matrix, Hitbox, Out) == true))
				{
					return true;
				}
			}
			else
			{
				const __int32 Hitbox = Vortex_Find_Head_Hitbox(Studio_Hdr, Bone_Matrix);
				if ((Hitbox >= 0) && (Vortex_Read_Hitbox(Studio_Hdr, Bone_Matrix, Hitbox, Out) == true))
				{
					return true;
				}
			}
		}

		float Origin[3];

		if (Sdk_Get_Origin_Safe(Entity, Origin) == false)
		{
			return false;
		}

		const float Height = Vortex_Get_Aim_Height(Kind, Chest);
		Out[0] = Origin[0];
		Out[1] = Origin[1];
		Out[2] = Origin[2] + Height;

		return (Out[0] == Out[0]) && (Out[1] == Out[1]) && (Out[2] == Out[2]) && (fabsf(Out[0]) < 1000000.f) && (fabsf(Out[1]) < 1000000.f) && (fabsf(Out[2]) < 1000000.f);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static void* Vortex_Last_Local_Player;

static unsigned __int32 Vortex_Last_Session_Generation;

static bool Vortex_Session_Seen;
static void* Vortex_Cached_Target;
static float Vortex_Cached_Target_Origin[3];
static float Vortex_Cached_Aim[3];
static __int32 Vortex_Cached_Target_Kind;
static __int32 Vortex_Cached_Target_Priority;
static unsigned __int32 Vortex_Cached_At;
static unsigned __int32 Vortex_Cached_Generation;
static bool Vortex_Cached_Target_Valid;
static bool Vortex_Cached_Scan_Valid;
static bool Vortex_Cached_Visibility_Valid;
static bool Vortex_Cached_Visibility_Result;
static void* Vortex_Cached_Visibility_Target;
static float Vortex_Cached_Visibility_Point[3];
static unsigned __int32 Vortex_Cached_Visibility_At;
static unsigned __int32 Vortex_Cached_Validation_At;
static __int32 Vortex_Cached_Validation_Team;
static __int32 Vortex_Cached_Validation_Health;
static unsigned __int8 Vortex_Cached_Validation_Dead;
static unsigned __int8 Vortex_Cached_Validation_Ghost;
static __int32 Vortex_Cached_Validation_Kind;
static bool Vortex_Cached_Validation_Ok;

static const unsigned __int32 Vortex_Target_Cache_Milliseconds = 300;
static const unsigned __int32 Vortex_Negative_Scan_Milliseconds = 400;
static const unsigned __int32 Vortex_Visibility_Cache_Milliseconds = 75;
static const unsigned __int32 Vortex_Validation_Cache_Milliseconds = 100;

static void Vortex_Reset_Runtime_Caches()
{
	Vortex_Trace_Interface = nullptr;
	Vortex_Trace_Tried = false;
	Vortex_Trace_Ray_Index = -1;
	Vortex_Model_Info_Interface = nullptr;
	Vortex_Model_Info_Tried = false;
	Vortex_Sim_Time_Offset = -1;
	Vortex_Velocity_Offset = -1;
	Vortex_Cached_Target = nullptr;
	Vortex_Cached_Target_Valid = false;
	Vortex_Cached_Scan_Valid = false;
	Vortex_Cached_Visibility_Valid = false;
	Vortex_Cached_Visibility_Result = false;
	Vortex_Cached_Visibility_Target = nullptr;
	Vortex_Cached_Visibility_At = 0;
	Vortex_Cached_Validation_At = 0;
	Vortex_Cached_Validation_Team = 0;
	Vortex_Cached_Validation_Health = 0;
	Vortex_Cached_Validation_Dead = 0;
	Vortex_Cached_Validation_Ghost = 0;
	Vortex_Cached_Validation_Kind = Kind_Invalid;
	Vortex_Cached_Validation_Ok = false;
}

static bool Vortex_Session_Is_Ready()
{
	if (Sdk_Update_Game_Session() == false)
	{
		Vortex_Last_Local_Player = nullptr;
		return false;
	}

	void* Local_Player = Sdk_Get_Local_Player_Safe();
	const unsigned __int32 Generation = Sdk_Get_Game_Session_Generation();

	if ((Vortex_Session_Seen == false) || (Generation != Vortex_Last_Session_Generation) || (Local_Player != Vortex_Last_Local_Player))
	{
		Vortex_Reset_Runtime_Caches();
		Vortex_Last_Session_Generation = Generation;
		Vortex_Last_Local_Player = Local_Player;
		Vortex_Session_Seen = true;
	}

	return (Local_Player != nullptr) && Sdk_Is_Readable_Range(Local_Player, 0x150);
}

static void Update_Vortex_Aimbot(UserCmd_Structure* Command)
{
	if ((Vortex_Aimbot_Enabled == false) || (Command == nullptr))
		return;

	if (Vortex_Session_Is_Ready() == false)
		return;

	__try
	{
		if ((Vortex_Aimbot_Auto_Fire == false) && ((Command->Buttons & 1) == 0))
			return;

		void* Local_Player = Sdk_Get_Local_Player_Safe();
		__int32 Local_Team = 0;
		__int32 Local_Health = 0;
		unsigned __int8 Local_Dead = 0;

		if ((Local_Player == nullptr) || (Sdk_Read_Entity_Basic(Local_Player, Local_Team, Local_Health, Local_Dead) == false) || (Local_Dead != 0) || (Local_Health <= 0))
			return;

		float Local_Origin[3];

		if (Sdk_Get_Origin_Safe(Local_Player, Local_Origin) == false)
			return;

		if (Sdk_Is_Readable_Range((unsigned __int8*)Local_Player + 0xF4, sizeof(float) * 3) == false)
			return;

		const float* Local_View_Offset = (const float*)((unsigned __int8*)Local_Player + 0xF4);
		const float Eye[3] = { Local_Origin[0] + Local_View_Offset[0], Local_Origin[1] + Local_View_Offset[1], Local_Origin[2] + Local_View_Offset[2] };
		const float View_Angles[2] = { Command->View_Angles[0], Command->View_Angles[1] };
		const float Original_Yaw = Command->View_Angles[1];
		const float Distance_Limit = Vortex_Aimbot_Distance * 100.f + 0.1f;

		const float Safe_Distance_Limit = (Distance_Limit > 0.f) ? Distance_Limit : 0.f;
		if (Safe_Distance_Limit <= 0.f)
			return;

		float Min_Fov = Vortex_Aimbot_Fov + 0.1f;
		float Best_Aim[3] = { 0.f, 0.f, 0.f };
		float Best_Target_Origin[3] = { 0.f, 0.f, 0.f };
		__int32 Best_Target_Priority = -1;
		void* Best_Target = nullptr;
		bool Refresh_Target = true;
		const unsigned __int32 Now = GetTickCount();
		const unsigned __int32 Cache_Age = (unsigned __int32)(Now - Vortex_Cached_At);

		if ((Vortex_Cached_Target_Valid == true) &&
			(Vortex_Cached_Generation == Sdk_Get_Game_Session_Generation()) &&
			(Vortex_Cached_Target != nullptr) &&
			(Cache_Age <= Vortex_Target_Cache_Milliseconds))
		{
			__int32 Cached_Team = Vortex_Cached_Validation_Team;
			__int32 Cached_Health = Vortex_Cached_Validation_Health;
			unsigned __int8 Cached_Dead = Vortex_Cached_Validation_Dead;
			unsigned __int8 Cached_Ghost = Vortex_Cached_Validation_Ghost;
			__int32 Cached_Kind = Vortex_Cached_Validation_Kind;
			float Cached_Origin[3];
			const unsigned __int32 Validation_Age = (unsigned __int32)(Now - Vortex_Cached_Validation_At);
			const bool Refresh_Validation = (Vortex_Cached_Validation_Ok == false) ||
				(Validation_Age > Vortex_Validation_Cache_Milliseconds);
			bool Cached_Basic_Ok = Vortex_Cached_Validation_Ok;

			if (Refresh_Validation == true)
			{
				Cached_Basic_Ok =
					Sdk_Is_Entity_Usable(Vortex_Cached_Target) &&
					Sdk_Read_Entity_Basic(Vortex_Cached_Target, Cached_Team, Cached_Health, Cached_Dead) &&
					((Cached_Team != Local_Team) && (Cached_Team == 2 || Cached_Team == 3)) &&
					((Cached_Team != 3) || (Sdk_Read_Entity_Ghost(Vortex_Cached_Target, Cached_Ghost) && Cached_Ghost == 0));

				Cached_Kind = Cached_Basic_Ok ? Get_Entity_Kind(Vortex_Cached_Target) : Kind_Invalid;
				Cached_Basic_Ok = Cached_Basic_Ok && (Cached_Kind != Kind_Invalid) && (Cached_Kind != 10);
				Vortex_Cached_Validation_Team = Cached_Team;
				Vortex_Cached_Validation_Health = Cached_Health;
				Vortex_Cached_Validation_Dead = Cached_Dead;
				Vortex_Cached_Validation_Ghost = Cached_Ghost;
				Vortex_Cached_Validation_Kind = Cached_Kind;
				Vortex_Cached_Validation_Ok = Cached_Basic_Ok;
				Vortex_Cached_Validation_At = Now;
			}

			Cached_Basic_Ok = Cached_Basic_Ok &&
				((Cached_Team != Local_Team) && (Cached_Team == 2 || Cached_Team == 3)) &&
				((Cached_Team != 3) || (Cached_Ghost == 0)) &&
				!(((Vortex_Aimbot_Ignore_Common == true) && (Cached_Kind == Kind_Common)) ||
				  ((Vortex_Aimbot_Ignore_Tank == true) && (Cached_Kind == Kind_Tank)) ||
				  ((Vortex_Aimbot_Ignore_Witch == true) && (Cached_Kind == Kind_Witch))) &&
				Sdk_Get_Origin_Safe(Vortex_Cached_Target, Cached_Origin);

			if (Cached_Basic_Ok == true)
			{
				const float Cached_Dx = Cached_Origin[0] - Local_Origin[0];
				const float Cached_Dy = Cached_Origin[1] - Local_Origin[1];
				const float Cached_Dz = Cached_Origin[2] - Local_Origin[2];
				const float Cached_Distance_Squared = Cached_Dx * Cached_Dx + Cached_Dy * Cached_Dy + Cached_Dz * Cached_Dz;

				if (Cached_Distance_Squared <= Safe_Distance_Limit * Safe_Distance_Limit)
				{
					const float Origin_Delta[3] =
					{
						Cached_Origin[0] - Vortex_Cached_Target_Origin[0],
						Cached_Origin[1] - Vortex_Cached_Target_Origin[1],
						Cached_Origin[2] - Vortex_Cached_Target_Origin[2]
					};
					float Cached_Aim[3] =
					{
						Vortex_Cached_Aim[0] + Origin_Delta[0],
						Vortex_Cached_Aim[1] + Origin_Delta[1],
						Vortex_Cached_Aim[2] + Origin_Delta[2]
					};
					float Cached_Angles[2];
					Vortex_Calculate_Aim(Eye, Cached_Aim, Cached_Angles);
					const float Cached_Fov = Vortex_Angles_Field_Of_View(View_Angles, Cached_Angles);
					bool Cached_Visible = true;

					if (Vortex_Aimbot_Visible == true)
					{
						const float Visibility_Dx = Cached_Aim[0] - Vortex_Cached_Visibility_Point[0];
						const float Visibility_Dy = Cached_Aim[1] - Vortex_Cached_Visibility_Point[1];
						const float Visibility_Dz = Cached_Aim[2] - Vortex_Cached_Visibility_Point[2];
						const unsigned __int32 Visibility_Age = (unsigned __int32)(Now - Vortex_Cached_Visibility_At);
						const bool Reuse_Visibility =
							(Vortex_Cached_Visibility_Valid == true) &&
							(Vortex_Cached_Visibility_Target == Vortex_Cached_Target) &&
							(Visibility_Age <= Vortex_Visibility_Cache_Milliseconds) &&
							((Visibility_Dx * Visibility_Dx + Visibility_Dy * Visibility_Dy + Visibility_Dz * Visibility_Dz) <= 16.f * 16.f);

						if (Reuse_Visibility == true)
						{
							Cached_Visible = Vortex_Cached_Visibility_Result;
						}
						else
						{
							Cached_Visible = Vortex_Is_Visible(Local_Player, Vortex_Cached_Target, Eye, Cached_Aim);
							Vortex_Cached_Visibility_Target = Vortex_Cached_Target;
							Vortex_Cached_Visibility_Point[0] = Cached_Aim[0];
							Vortex_Cached_Visibility_Point[1] = Cached_Aim[1];
							Vortex_Cached_Visibility_Point[2] = Cached_Aim[2];
							Vortex_Cached_Visibility_At = Now;
							Vortex_Cached_Visibility_Result = Cached_Visible;
							Vortex_Cached_Visibility_Valid = true;
						}
					}

					if ((Cached_Fov <= Vortex_Aimbot_Fov) && (Cached_Visible == true))
				{
						Best_Aim[0] = Cached_Aim[0];
						Best_Aim[1] = Cached_Aim[1];
						Best_Aim[2] = Cached_Aim[2];
						Best_Target_Origin[0] = Cached_Origin[0];
						Best_Target_Origin[1] = Cached_Origin[1];
						Best_Target_Origin[2] = Cached_Origin[2];
						Best_Target = Vortex_Cached_Target;
						Best_Target_Priority = Vortex_Cached_Target_Priority;
						Min_Fov = Cached_Fov;
						Refresh_Target = false;
					}
				}
			}

			if (Refresh_Target == true)
			{
				Vortex_Cached_Target = nullptr;
				Vortex_Cached_Target_Valid = false;
				Vortex_Cached_Scan_Valid = true;
				Vortex_Cached_Validation_Ok = false;
				Vortex_Cached_Visibility_Valid = false;
				Vortex_Cached_At = Now;
				Vortex_Cached_Generation = Sdk_Get_Game_Session_Generation();
				Refresh_Target = false;
			}
		}
		else if ((Vortex_Cached_Scan_Valid == true) &&
			(Vortex_Cached_Generation == Sdk_Get_Game_Session_Generation()) &&
			(Cache_Age <= Vortex_Negative_Scan_Milliseconds))
		{
			Refresh_Target = false;
		}

		if (Refresh_Target == true)
		{
			const __int32 Last_Entity_Number = Sdk_Get_Max_Entities_Safe();
			if (Last_Entity_Number < 1)
				return;

			for (__int32 Entity_Number = 1; Entity_Number <= Last_Entity_Number; Entity_Number++)
		{
			void* Entity = Sdk_Get_Client_Entity_Safe(Entity_Number);

			if ((Entity == nullptr) || (Entity == Local_Player))
			continue;

			__int32 Team = 0;
			__int32 Health = 0;
			unsigned __int8 Dead = 0;

			if ((Sdk_Read_Entity_Basic(Entity, Team, Health, Dead) == false) || (Dead != 0) || (Health <= 0))
			continue;

			if ((Team != 2) && (Team != 3))
			continue;

			if (Team == Local_Team)
			continue;

			if (Team == 3)
			{
				unsigned __int8 Ghost = 0;

				if ((Sdk_Read_Entity_Ghost(Entity, Ghost) == false) || (Ghost != 0))
					continue;
			}

			const __int32 Kind = Get_Entity_Kind(Entity);

			if ((Kind == Kind_Invalid) || (Kind == 10))
				continue;

			const bool Ignore_Target =
				((Vortex_Aimbot_Ignore_Common == true) && (Kind == Kind_Common)) ||
				((Vortex_Aimbot_Ignore_Tank == true) && (Kind == Kind_Tank)) ||
				((Vortex_Aimbot_Ignore_Witch == true) && (Kind == Kind_Witch));

			if (Ignore_Target == true)
				continue;

			float Origin[3];

			if (Sdk_Get_Origin_Safe(Entity, Origin) == false)
				continue;
			
			const float Distance_X = Origin[0] - Local_Origin[0];
			const float Distance_Y = Origin[1] - Local_Origin[1];
			const float Distance_Z = Origin[2] - Local_Origin[2];
			const float Distance_Squared = Distance_X * Distance_X + Distance_Y * Distance_Y + Distance_Z * Distance_Z;

			if (Distance_Squared > Safe_Distance_Limit * Safe_Distance_Limit)
				continue;

			const float Rough_Position[3] =
			{
				Origin[0],
				Origin[1],
				Origin[2] + Vortex_Get_Aim_Height(Kind, (Vortex_Aimbot_Hitbox == 1) ? 1 : 0)
			};
			float Rough_Angles[2];
			Vortex_Calculate_Aim(Eye, Rough_Position, Rough_Angles);
			const float Rough_Fov = Vortex_Angles_Field_Of_View(View_Angles, Rough_Angles);
			if (Rough_Fov > (Vortex_Aimbot_Fov + 5.f))
				continue;
			
			const __int32 Candidate_Priority = Vortex_Get_Target_Priority(Kind);
			if ((Best_Target_Priority >= 0) &&
				((Candidate_Priority < Best_Target_Priority) ||
				 ((Candidate_Priority == Best_Target_Priority) && (Rough_Fov > (Min_Fov + 8.f)))))
			{
				continue;
			}

			float Target_Position[3];

			if (Vortex_Get_Aim_Position(Entity, Kind, (Vortex_Aimbot_Hitbox == 1) ? 1 : 0, Target_Position) == false)
				continue;

			Vortex_Apply_Prediction(Entity, Target_Position);

			for (__int32 Axis = 0; Axis < 3; Axis++)
			{
				if ((Target_Position[Axis] != Target_Position[Axis]) || (fabsf(Target_Position[Axis]) > 1000000.f))
				{
					Target_Position[0] = 0.f;
					Target_Position[1] = 0.f;
					Target_Position[2] = 0.f;
					break;
				}
			}

			if ((Target_Position[0] == 0.f) && (Target_Position[1] == 0.f) && (Target_Position[2] == 0.f))
				continue;

			float Aim_Angles[2];
			Vortex_Calculate_Aim(Eye, Target_Position, Aim_Angles);

			const float Fov = Vortex_Angles_Field_Of_View(View_Angles, Aim_Angles);
			const float Distance = sqrtf(
				(Origin[0] - Local_Origin[0]) * (Origin[0] - Local_Origin[0]) +
				(Origin[1] - Local_Origin[1]) * (Origin[1] - Local_Origin[1]) +
				(Origin[2] - Local_Origin[2]) * (Origin[2] - Local_Origin[2]));

			if ((Fov <= Vortex_Aimbot_Fov) && (Distance <= Safe_Distance_Limit) &&
				((Candidate_Priority > Best_Target_Priority) ||
				 ((Candidate_Priority == Best_Target_Priority) && (Fov < Min_Fov))) &&
				((Vortex_Aimbot_Visible == false) || (Vortex_Is_Visible(Local_Player, Entity, Eye, Target_Position) == true)))
			{
				Min_Fov = Fov;
				Best_Target_Priority = Candidate_Priority;
				Best_Target = Entity;
				Best_Target_Origin[0] = Origin[0];
				Best_Target_Origin[1] = Origin[1];
				Best_Target_Origin[2] = Origin[2];
				Best_Aim[0] = Target_Position[0];
				Best_Aim[1] = Target_Position[1];
				Best_Aim[2] = Target_Position[2];
			}
		}
		}

		if (Refresh_Target == true)
		{
			if (Best_Target != nullptr)
			{
				Vortex_Cached_Target = Best_Target;
				Vortex_Cached_Target_Origin[0] = Best_Target_Origin[0];
				Vortex_Cached_Target_Origin[1] = Best_Target_Origin[1];
				Vortex_Cached_Target_Origin[2] = Best_Target_Origin[2];
				Vortex_Cached_Aim[0] = Best_Aim[0];
				Vortex_Cached_Aim[1] = Best_Aim[1];
				Vortex_Cached_Aim[2] = Best_Aim[2];
				Vortex_Cached_Target_Kind = Get_Entity_Kind(Best_Target);
				Vortex_Cached_Target_Priority = Best_Target_Priority;
				Vortex_Cached_At = Now;
				Vortex_Cached_Generation = Sdk_Get_Game_Session_Generation();
				Vortex_Cached_Target_Valid = true;
				Vortex_Cached_Scan_Valid = false;
				Vortex_Cached_Validation_Ok = false;
				Vortex_Cached_Visibility_Target = Best_Target;
				Vortex_Cached_Visibility_Point[0] = Best_Aim[0];
				Vortex_Cached_Visibility_Point[1] = Best_Aim[1];
				Vortex_Cached_Visibility_Point[2] = Best_Aim[2];
				Vortex_Cached_Visibility_At = Now;
				Vortex_Cached_Visibility_Result = true;
				Vortex_Cached_Visibility_Valid = Vortex_Aimbot_Visible;
			}
			else
			{
				Vortex_Cached_Target = nullptr;
				Vortex_Cached_Target_Valid = false;
				Vortex_Cached_Scan_Valid = true;
				Vortex_Cached_Validation_Ok = false;
				Vortex_Cached_At = Now;
				Vortex_Cached_Generation = Sdk_Get_Game_Session_Generation();
			}
		}

		if ((Min_Fov > Vortex_Aimbot_Fov) || (Best_Target_Priority < 0))
			return;

		float Aim_Angles[2];
		Vortex_Calculate_Aim(Eye, Best_Aim, Aim_Angles);

		if (Vortex_Aimbot_Smooth > 1.f)
		{
			float Delta[2];
			Delta[0] = Aim_Angles[0] - Command->View_Angles[0];
			Delta[1] = Aim_Angles[1] - Command->View_Angles[1];

			while (Delta[1] > 180.f)  Delta[1] -= 360.f;
			while (Delta[1] < -180.f) Delta[1] += 360.f;

			Command->View_Angles[0] += Delta[0] / Vortex_Aimbot_Smooth;
			Command->View_Angles[1] += Delta[1] / Vortex_Aimbot_Smooth;
		}
		else
		{
			Command->View_Angles[0] = Aim_Angles[0];
			Command->View_Angles[1] = Aim_Angles[1];
		}

		Command->View_Angles[2] = 0.f;

		if (Vortex_Aimbot_Auto_Fire == true)
		{
			void* Weapon = Get_Active_Weapon((void*)Local_Player);

			if (Weapon != nullptr)
			{
				typedef __int32(__fastcall* Get_Weapon_Type_Type)(void* Ecx, void* Edx);

				void** Weapon_Vtable = *(void***)Weapon;

				__int32 Weapon_Type = 0;

				if ((Weapon_Vtable != nullptr) && (Weapon_Vtable[383] != nullptr))
				{
					Weapon_Type = ((Get_Weapon_Type_Type)Weapon_Vtable[383])(Weapon, nullptr);
				}

				if (Rage_Weapon_Auto_Pistol(Weapon_Type) == false)
				{
					Command->Buttons |= 1;
				}
				else
				{
					if (Rage_Bot_Shoot_Toggle == true)
					{
						Command->Buttons |= 1;
					}

					Rage_Bot_Shoot_Toggle = !Rage_Bot_Shoot_Toggle;
				}
			}
			else
			{
				Command->Buttons |= 1;
			}
		}

		if (Vortex_Aimbot_Silent == true)
		{
			Sdk_Fix_Movement(Command, View_Angles);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}