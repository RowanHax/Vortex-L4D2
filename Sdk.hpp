#pragma once

#include <windows.h>

#include <string.h>

#include <stdio.h>

#include <math.h>

#include "Internal.hpp"

#include "D3D9.hpp"

struct Recv_Table_Structure;

struct Recv_Prop_Structure
{
	char* Var_Name;
	__int32 Recv_Type;
	__int32 Flags;
	__int32 String_Buffer_Size;
	bool Inside_Array;
	const void* Extra_Data;
	Recv_Prop_Structure* Array_Prop;
	void* Array_Length_Proxy;
	void* Proxy_Fn;
	void* Data_Table_Proxy_Fn;
	Recv_Table_Structure* Data_Table;
	__int32 Offset;
	__int32 Element_Stride;
	__int32 Element_Count;
	const char* Parent_Array_Prop_Name;
};

struct Recv_Table_Structure
{
	Recv_Prop_Structure* Props;
	__int32 Prop_Count;
	void* Decoder;
	char* Table_Name;
	bool Initialized;
	bool In_Main_List;
};

struct Client_Class_Structure
{
	void* Create_Fn;
	void* Create_Event_Fn;
	char* Network_Name;
	Recv_Table_Structure* Recv_Table;
	Client_Class_Structure* Next;
	__int32 Class_Id;
};

typedef Client_Class_Structure* (__fastcall* Get_Client_Class_Type)(void* Ecx, void* Edx);

static bool Sdk_Is_Readable_Range(const void* Address, SIZE_T Size);

static bool Sdk_Address_In_Module(HMODULE Module, unsigned __int32 Address);

static void* Sdk_Get_Client_Entity_Safe(__int32 Index);

static Client_Class_Structure* Get_Client_Class(void* Entity)
{
	__try
	{
		if (Entity == nullptr)
		{
			return nullptr;
		}

		if (Sdk_Is_Readable_Range(Entity, 0x0C) == false)
		{
			return nullptr;
		}

		void* Networkable = (void*)((unsigned __int8*)Entity + 0x8);

		if (Sdk_Is_Readable_Range(Networkable, sizeof(void*)) == false)
		{
			return nullptr;
		}

		void** Vtable = *(void***)Networkable;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 2 * sizeof(void*)) == false) || (Vtable[1] == nullptr) || (Sdk_Address_In_Module(Client_Module, (unsigned __int32)Vtable[1]) == false))
		{
			return nullptr;
		}

		const Get_Client_Class_Type Get_Client_Class_Fn = (Get_Client_Class_Type)Vtable[1];

		return Get_Client_Class_Fn(Networkable, nullptr);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static Client_Class_Structure* Get_Client_Class_Fast(void* Entity)
{
	if (Entity == nullptr)
	{
		return nullptr;
	}

	void* Networkable = (void*)((unsigned __int8*)Entity + 0x8);

	void** Vtable = *(void***)Networkable;

	if ((Vtable == nullptr) || (Vtable[1] == nullptr) || (Sdk_Address_In_Module(Client_Module, (unsigned __int32)Vtable[1]) == false))
	{
		return nullptr;
	}

	const Get_Client_Class_Type Get_Client_Class_Fn = (Get_Client_Class_Type)Vtable[1];

	return Get_Client_Class_Fn(Networkable, nullptr);
}

static __int32 Find_Recv_Prop(Recv_Table_Structure* Table, const char* Prop_Name, __int32 Extra_Offset, __int32 Depth = 0)
{
	__try
	{
		if ((Depth > 32) || (Table == nullptr) || (Sdk_Is_Readable_Range(Table, sizeof(Recv_Table_Structure)) == false) || (Table->Props == nullptr) || (Table->Prop_Count <= 0) || (Table->Prop_Count > 4096))
		{
			return -1;
		}

		for (__int32 i = 0; i < Table->Prop_Count; i++)
		{
			Recv_Prop_Structure* Prop = &Table->Props[i];

			if ((Sdk_Is_Readable_Range(Prop, sizeof(Recv_Prop_Structure)) == false) || (Prop == nullptr))
			{
				continue;
			}

			Recv_Table_Structure* Child = Prop->Data_Table;

			if ((Child != nullptr) && (Sdk_Is_Readable_Range(Child, sizeof(Recv_Table_Structure)) == true) && (Child->Prop_Count > 0))
			{
				const __int32 Found = Find_Recv_Prop(Child, Prop_Name, Prop->Offset + Extra_Offset, Depth + 1);

				if (Found >= 0)
				{
					return Found;
				}
			}

			if ((Prop->Var_Name != nullptr) && (_stricmp(Prop->Var_Name, Prop_Name) == 0))
			{
				return Prop->Offset + Extra_Offset;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return -1;
}

static Client_Class_Structure* Get_Client_Class_Chain()
{
	__try
	{
		const HMODULE Client = Client_Module;

		if (Client != nullptr)
		{
			typedef void* (__cdecl* Create_Interface_Type)(const char* Name, __int32* Return_Code);

			const Create_Interface_Type Factory = (Create_Interface_Type)GetProcAddress(Client, "CreateInterface");

			if (Factory != nullptr)
			{
				void* Client_Dll = Factory("VClient017", nullptr);

				if (Client_Dll == nullptr)
				{
					Client_Dll = Factory("VClient016", nullptr);
				}

				if (Client_Dll != nullptr)
				{
					typedef Client_Class_Structure* (__fastcall* GetAll_Classes_Type)(void* Ecx, void* Edx);

					const GetAll_Classes_Type GetAll_Classes = (GetAll_Classes_Type)(*(void***)Client_Dll)[7];

					if (GetAll_Classes != nullptr)
					{
						Client_Class_Structure* Head = GetAll_Classes(Client_Dll, nullptr);

						if ((Head != nullptr) && (Sdk_Is_Readable_Range(Head, sizeof(Client_Class_Structure)) == true) && (Head->Next != nullptr))
						{
							return Head;
						}
					}
				}
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	__try
	{
		for (__int32 i = 0; i < 64; i++)
		{
			void* Entity = Sdk_Get_Client_Entity_Safe(i);

			if (Entity == nullptr)
			{
				continue;
			}

			Client_Class_Structure* Chain = Get_Client_Class(Entity);

			if ((Chain != nullptr) && (Chain->Next != nullptr))
			{
				return Chain;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return nullptr;
}

static __int32 Get_Net_Prop_Offset(const char* Table_Name, const char* Prop_Name)
{
	static struct Net_Prop_Cache_Entry
	{
		char Table[64];
		char Prop[64];
		__int32 Offset;
	} Cache[32];

	static __int32 Cache_Count = 0;

	__try
	{
		for (__int32 i = 0; i < Cache_Count; i++)
		{
			if ((_stricmp(Cache[i].Table, Table_Name) == 0) && (_stricmp(Cache[i].Prop, Prop_Name) == 0))
			{
				return Cache[i].Offset;
			}
		}

		Client_Class_Structure* Chain = Get_Client_Class_Chain();

		if (Chain == nullptr)
		{
			return -1;
		}

		__int32 Found_Offset = -1;

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

			if (_stricmp(Table->Table_Name, Table_Name) != 0)
			{
				continue;
			}

			Found_Offset = Find_Recv_Prop(Table, Prop_Name, 0);

			break;
		}

		if (Cache_Count < 32)
		{
			strncpy_s(Cache[Cache_Count].Table, Table_Name, 63);

			strncpy_s(Cache[Cache_Count].Prop, Prop_Name, 63);

			Cache[Cache_Count].Offset = Found_Offset;

			Cache_Count++;
		}


		return Found_Offset;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
}

typedef void* (__cdecl* Create_Interface_Type)(const char* Name, __int32* Return_Code);

static void* Trace_Interface = nullptr;

static void* Get_Interface(HMODULE Module, const char* Name)
{
	if (Module == nullptr)
	{
		return nullptr;
	}

	Create_Interface_Type Create_Interface = (Create_Interface_Type)GetProcAddress(Module, "CreateInterface");

	if (Create_Interface == nullptr)
	{
		return nullptr;
	}

	void* Interface = Create_Interface(Name, nullptr);

	if (Interface != nullptr)
	{
		return Interface;
	}

	char Buffer[128];

	for (__int32 i = 1; i < 100; i++)
	{
		sprintf_s(Buffer, "%s0%i", Name, i);

		Interface = Create_Interface(Buffer, nullptr);

		if (Interface != nullptr)
		{
			return Interface;
		}

		sprintf_s(Buffer, "%s00%i", Name, i);

		Interface = Create_Interface(Buffer, nullptr);

		if (Interface != nullptr)
		{
			return Interface;
		}
	}

	return nullptr;
}

static bool Sdk_Is_Readable_Range(const void* Address, SIZE_T Size)
{
	if ((Address == nullptr) || (Size == 0))
	{
		return false;
	}

	__try
	{
		MEMORY_BASIC_INFORMATION Memory;

		if (VirtualQuery(Address, &Memory, sizeof(Memory)) != sizeof(Memory))
		{
			return false;
		}

		if ((Memory.State != MEM_COMMIT) || ((Memory.Protect & 0xFF) == PAGE_NOACCESS) || ((Memory.Protect & PAGE_GUARD) != 0))
		{
			return false;
		}

		const unsigned __int8* Start = (const unsigned __int8*)Address;
		const unsigned __int8* End = Start + Size;
		const unsigned __int8* Region_End = (const unsigned __int8*)Memory.BaseAddress + Memory.RegionSize;

		if (End < Start)
		{
			return false;
		}

		return End <= Region_End;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Sdk_Address_In_Module(HMODULE Module, unsigned __int32 Address)
{
	__try
	{
		if ((Module == nullptr) || (Address == 0))
		{
			return false;
		}

		const unsigned __int32 Module_Start = (unsigned __int32)Module;
		const PIMAGE_DOS_HEADER Dos_Header = (PIMAGE_DOS_HEADER)Module;

		if (Dos_Header->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return false;
		}

		const PIMAGE_NT_HEADERS Nt_Headers = (PIMAGE_NT_HEADERS)(Module_Start + Dos_Header->e_lfanew);

		if (Nt_Headers->Signature != IMAGE_NT_SIGNATURE)
		{
			return false;
		}

		const unsigned __int32 Module_End = Module_Start + Nt_Headers->OptionalHeader.SizeOfImage;
		return (Address >= Module_Start) && (Address < Module_End);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static void* Sdk_Get_Local_Player_Safe()
{
	__try
	{
		if ((Client_Module == nullptr) || (Sdk_Is_Readable_Range((unsigned __int8*)Client_Module + Local_Player_Offset, sizeof(unsigned __int32)) == false))
		{
			return nullptr;
		}

		return *(void**)((unsigned __int8*)Client_Module + Local_Player_Offset);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static Global_Variables_Structure* Sdk_Get_Global_Variables_Safe()
{
	__try
	{
		if ((Client_Module == nullptr) || (Sdk_Is_Readable_Range((unsigned __int8*)Client_Module + 7096744, sizeof(void*)) == false))
		{
			return nullptr;
		}

		Global_Variables_Structure* Global_Variables = *(Global_Variables_Structure**)((unsigned __int8*)Client_Module + 7096744);

		if (Sdk_Is_Readable_Range(Global_Variables, sizeof(Global_Variables_Structure)) == false)
		{
			return nullptr;
		}

		return Global_Variables;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static void* Sdk_Engine_Client;

static bool Sdk_Engine_Interface_Tried;

static bool(__fastcall* Sdk_Is_In_Game)(void* Ecx, void* Edx);

static bool Sdk_Resolve_Engine_Interface()
{
	__try
	{
		if ((Sdk_Engine_Client != nullptr) && (Sdk_Is_In_Game != nullptr))
		{
			return true;
		}

		if (Sdk_Engine_Interface_Tried == true)
		{
			return false;
		}

		Sdk_Engine_Interface_Tried = true;
		Sdk_Engine_Client = Get_Interface(Engine_Module, "VEngineClient013");

		if (Sdk_Engine_Client == nullptr)
		{
			return false;
		}

		void** Vtable = *(void***)Sdk_Engine_Client;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 27 * sizeof(void*)) == false) || (Vtable[26] == nullptr) || (Sdk_Address_In_Module(Engine_Module, (unsigned __int32)Vtable[26]) == false))
		{
			Sdk_Engine_Client = nullptr;
			return false;
		}

		Sdk_Is_In_Game = (bool(__fastcall*)(void*, void*))Vtable[26];
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Sdk_Engine_Client = nullptr;
		Sdk_Is_In_Game = nullptr;
		Sdk_Engine_Interface_Tried = false;
		return false;
	}
}

static bool Sdk_Game_Is_Ready()
{
	__try
	{
		if ((Client_Module == nullptr) || (Engine_Module == nullptr) || (Entity_List == nullptr) || (Get_Client_Entity == nullptr) || (Get_Max_Entities == nullptr))
		{
			return false;
		}

		if (Sdk_Is_Readable_Range(Client_Module, 0x100) == false)
		{
			return false;
		}

		if (Sdk_Resolve_Engine_Interface() == false)
		{
			return false;
		}

		if (Sdk_Is_In_Game(Sdk_Engine_Client, nullptr) == false)
		{
			return false;
		}

		void* Local_Player = Sdk_Get_Local_Player_Safe();

		return (Local_Player != nullptr) && Sdk_Is_Readable_Range(Local_Player, 0x150);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Sdk_Last_Game_Ready;

static unsigned __int32 Sdk_Game_Session_Generation;

static bool Sdk_Update_Game_Session()
{
	const bool Ready = Sdk_Game_Is_Ready();

	if (Ready != Sdk_Last_Game_Ready)
	{
		Sdk_Last_Game_Ready = Ready;

		if (Ready == true)
		{
			Sdk_Game_Session_Generation += 1;
		}

	}

	return Ready;
}

static unsigned __int32 Sdk_Get_Game_Session_Generation()
{
	return Sdk_Game_Session_Generation;
}

static __int32 Sdk_Get_Max_Entities_Safe()
{
	if ((Entity_List == nullptr) || (Get_Max_Entities == nullptr))
	{
		return 0;
	}

	__try
	{
		const __int32 Max_Entities = Get_Max_Entities(Entity_List, nullptr);

		if ((Max_Entities < 1) || (Max_Entities > 8192))
		{
			return 0;
		}

		return Max_Entities;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
}

static void* Sdk_Get_Client_Entity_Safe(__int32 Index)
{
	if ((Index < 0) || (Index > 8192) || (Entity_List == nullptr) || (Get_Client_Entity == nullptr))
	{
		return nullptr;
	}

	__try
	{
		return Get_Client_Entity(Entity_List, nullptr, Index);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static bool Sdk_Read_Entity_Basic(void* Entity, __int32& Team, __int32& Health, unsigned __int8& Dead)
{
	if ((Entity == nullptr) || (Sdk_Is_Readable_Range(Entity, 0x150) == false))
	{
		return false;
	}

	__try
	{
		Team = *(__int32*)((unsigned __int8*)Entity + 0xE4);
		Health = *(__int32*)((unsigned __int8*)Entity + 0xEC);
		Dead = *(unsigned __int8*)((unsigned __int8*)Entity + 0x147);
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Sdk_Read_Entity_Ghost(void* Entity, unsigned __int8& Ghost)
{
	if ((Entity == nullptr) || (Sdk_Is_Readable_Range((unsigned __int8*)Entity + 7322, sizeof(unsigned __int8)) == false))
	{
		return false;
	}

	__try
	{
		Ghost = *(unsigned __int8*)((unsigned __int8*)Entity + 7322);
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Sdk_Read_Entity_Basic_Fast(void* Entity, __int32& Team, __int32& Health, unsigned __int8& Dead)
{
	if (Entity == nullptr)
	{
		return false;
	}

	Team = *(__int32*)((unsigned __int8*)Entity + 0xE4);
	Health = *(__int32*)((unsigned __int8*)Entity + 0xEC);
	Dead = *(unsigned __int8*)((unsigned __int8*)Entity + 0x147);
	return true;
}

static bool Sdk_Read_Entity_Ghost_Fast(void* Entity, unsigned __int8& Ghost)
{
	if (Entity == nullptr)
	{
		return false;
	}

	Ghost = *(unsigned __int8*)((unsigned __int8*)Entity + 7322);
	return true;
}

static bool Sdk_Read_Origin_Fast(void* Entity, float Out[3])
{
	if ((Entity == nullptr) || (Out == nullptr))
	{
		return false;
	}

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

static bool Sdk_Is_Entity_Usable(void* Entity)
{
	__int32 Team = 0;
	__int32 Health = 0;
	unsigned __int8 Dead = 0;

	if ((Sdk_Read_Entity_Basic(Entity, Team, Health, Dead) == false) || ((Team != 2) && (Team != 3)))
	{
		return false;
	}

	__try
	{
		void* Networkable = (void*)((unsigned __int8*)Entity + 0x8);

		if (Sdk_Is_Readable_Range(Networkable, sizeof(void*)) == false)
		{
			return false;
		}

		void** Vtable = *(void***)Networkable;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 8 * sizeof(void*)) == false) || (Vtable[7] == nullptr) || (Sdk_Address_In_Module(Client_Module, (unsigned __int32)Vtable[7]) == false))
		{
			return false;
		}

		typedef bool(__fastcall* Is_Dormant_Type)(void* Ecx, void* Edx);
		return ((Is_Dormant_Type)Vtable[7])(Networkable, nullptr) == false;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Sdk_Get_Origin_Safe(void* Entity, float Out[3])
{
	if ((Entity == nullptr) || (Out == nullptr) || (Client_Module == nullptr))
	{
		return false;
	}

	__try
	{
		typedef float* (__thiscall* Get_Origin_Type)(void* Entity);

		if (Sdk_Is_Readable_Range((unsigned __int8*)Client_Module + 297584, 1) == false)
		{
			return false;
		}

		const Get_Origin_Type Get_Origin = (Get_Origin_Type)((unsigned __int8*)Client_Module + 297584);
		float* Origin = Get_Origin(Entity);

		if (Sdk_Is_Readable_Range(Origin, sizeof(float) * 3) == false)
		{
			return false;
		}

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

static bool Trace_Tried = false;
static __int32 Trace_Ray_Index = -1;

static void* Get_Trace_Interface();

static bool Initialize_Trace()
{
	return Get_Trace_Interface() != nullptr;
}

struct __declspec(align(16)) Ray_Structure
{
	float Start[4];
	float Delta[4];
	float Start_Offset[4];
	float Extents[4];
	bool Is_Ray;
	bool Is_Swept;
};

struct Trace_Structure
{
	float Start[3];
	float End[3];
	float Plane_Normal[3];
	float Plane_Distance;
	unsigned __int8 Plane_Type;
	unsigned __int8 Plane_Signbits;
	unsigned __int8 Pad[2];
	float Fraction;
	__int32 Contents;
	unsigned __int16 Disp_Flags;
	bool All_Solid;
	bool Start_Solid;
	float Fraction_Left_Solid;
	void* Surface_Name;
	__int16 Surface_Props;
	unsigned __int16 Surface_Flags;
	__int32 Hit_Group;
	__int16 Physics_Bone;
	unsigned __int16 Pad2;
	void* Hit_Entity;
	__int32 Hitbox;
};

const unsigned __int32 MASK_SHOT = 0x44000007;

typedef void (__fastcall* Trace_Ray_Type)(void* Ecx, void* Edx, const Ray_Structure* Ray, unsigned __int32 Mask, void* Filter, Trace_Structure* Trace);

static void Trace_Ray(const Ray_Structure* Ray, unsigned __int32 Mask, void* Filter, Trace_Structure* Trace)
{
	__try
	{
		if (Get_Trace_Interface() == nullptr)
		{
			return;
		}
		void** Vtable = *(void***)Trace_Interface;
		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 6 * sizeof(void*)) == false) || (Vtable[5] == nullptr) || (Sdk_Address_In_Module(Engine_Module, (unsigned __int32)Vtable[5]) == false))
		{
			return;
		}

		Trace_Ray_Index = 5;

		((Trace_Ray_Type)Vtable[Trace_Ray_Index])(Trace_Interface, nullptr, Ray, Mask, Filter, Trace);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}
}

class Trigger_Trace_Filter
{
public:
	void* Skip_Entity;

	virtual bool ShouldHitEntity(void* Entity, int Contents_Mask)
	{
		if (Entity == Skip_Entity)
		{
			return false;
		}

		Client_Class_Structure* Class = Get_Client_Class(Entity);

		if (Class != nullptr)
		{
			const char* Name = Class->Network_Name;

			if (Name != nullptr)
			{
				if (_stricmp(Name, "SurvivorRescue") == 0)
				{
					return false;
				}
			}
		}

		return true;
	}

	virtual __int32 GetTraceType() const
	{
		return 0;
	}
};

static void* Get_Trace_Interface()
{
	__try
	{
		if ((Trace_Interface == nullptr) && (Trace_Tried == false))
		{
			Trace_Tried = true;
			Trace_Interface = Get_Interface(Engine_Module, "EngineTraceClient");
		}
		return Trace_Interface;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static void Vector_Transform(const float In[3], const float Matrix[3][4], float Out[3])
{
	Out[0] = In[0] * Matrix[0][0] + In[1] * Matrix[1][0] + In[2] * Matrix[2][0] + Matrix[0][3];

	Out[1] = In[0] * Matrix[0][1] + In[1] * Matrix[1][1] + In[2] * Matrix[2][1] + Matrix[1][3];

	Out[2] = In[0] * Matrix[0][2] + In[1] * Matrix[1][2] + In[2] * Matrix[2][2] + Matrix[2][3];
}

static void Forward_Vector(const float Angles[2], float Out[3])
{
	const float Pitch = Angles[0] * 3.14159265f / 180.f;

	const float Yaw = Angles[1] * 3.14159265f / 180.f;

	Out[0] = cosf(Pitch) * cosf(Yaw);

	Out[1] = cosf(Pitch) * sinf(Yaw);

	Out[2] = -sinf(Pitch);
}

static void Calculate_Aim(const float Origin[3], const float Target[3], float Out[2])
{
	const float Delta_X = Target[0] - Origin[0];

	const float Delta_Y = Target[1] - Origin[1];

	const float Delta_Z = Target[2] - Origin[2];

	Out[0] = atan2f(-Delta_Z, sqrtf(Delta_X * Delta_X + Delta_Y * Delta_Y)) * 180.f / 3.14159265f;

	Out[1] = atan2f(Delta_Y, Delta_X) * 180.f / 3.14159265f;

	if (Out[0] > 89.f)
	{
		Out[0] = 89.f;
	}
	else if (Out[0] < -89.f)
	{
		Out[0] = -89.f;
	}
}

static float Angles_Field_Of_View(const float My_Angles[2], const float Aim_Angles[2])
{
	float My_Forward[3];

	float Aim_Forward[3];

	Forward_Vector(My_Angles, My_Forward);

	Forward_Vector(Aim_Angles, Aim_Forward);

	const float Dot = My_Forward[0] * Aim_Forward[0] + My_Forward[1] * Aim_Forward[1] + My_Forward[2] * Aim_Forward[2];

	const float Length_Squared = My_Forward[0] * My_Forward[0] + My_Forward[1] * My_Forward[1] + My_Forward[2] * My_Forward[2];

	return acosf(Dot / Length_Squared) * 180.f / 3.14159265f;
}

static float Vector_Distance(const float A[3], const float B[3])
{
	const float Delta_X = A[0] - B[0];

	const float Delta_Y = A[1] - B[1];

	const float Delta_Z = A[2] - B[2];

	return sqrtf(Delta_X * Delta_X + Delta_Y * Delta_Y + Delta_Z * Delta_Z);
}

static void Velocity_Extrapolate(const float Origin[3], const float Velocity[3], float Out[3])
{
	Global_Variables_Structure* Global_Variables = *(Global_Variables_Structure**)((unsigned __int32)Client_Module + 7096744);

	if (Global_Variables == nullptr)
	{
		Out[0] = Origin[0];

		Out[1] = Origin[1];

		Out[2] = Origin[2];

		return;
	}

	const float Tick = Global_Variables->Interval_Per_Tick;

	Out[0] = Origin[0] + Velocity[0] * Tick;

	Out[1] = Origin[1] + Velocity[1] * Tick;

	Out[2] = Origin[2] + Velocity[2] * Tick;
}

static void Vector_To_Angles(const float Vector[3], float Out[2])
{
	const float Length = sqrtf(Vector[0] * Vector[0] + Vector[1] * Vector[1] + Vector[2] * Vector[2]);

	if (Length < 0.01f)
	{
		Out[0] = 0.f;

		Out[1] = 0.f;

		return;
	}

	Out[0] = atan2f(-Vector[2], sqrtf(Vector[0] * Vector[0] + Vector[1] * Vector[1])) * 180.f / 3.14159265f;

	Out[1] = atan2f(Vector[1], Vector[0]) * 180.f / 3.14159265f;
}

const unsigned __int32 BONE_USED_BY_HITBOX = 0x100;

static void* Model_Info_Interface = nullptr;

static bool Address_In_Client_Module(unsigned __int32 Address)
{
	__try
	{
		if ((Address == 0) || (Client_Module == nullptr))
		{
			return false;
		}

		const unsigned __int32 Module_Start = (unsigned __int32)Client_Module;

		const PIMAGE_DOS_HEADER Dos_Header = (PIMAGE_DOS_HEADER)Client_Module;

		const PIMAGE_NT_HEADERS Nt_Headers = (PIMAGE_NT_HEADERS)(Module_Start + Dos_Header->e_lfanew);

		const unsigned __int32 Module_End = Module_Start + Nt_Headers->OptionalHeader.SizeOfImage;

		return ((Address >= Module_Start) && (Address < Module_End));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static void* Get_Studio_Model(void* Model)
{
	__try
	{
		if (Model_Info_Interface == nullptr)
		{
			Model_Info_Interface = Get_Interface(Engine_Module, "VModelInfoClient");

		}

		if (Model_Info_Interface == nullptr)
		{
			return nullptr;
		}

		void** Vtable = *(void***)Model_Info_Interface;

		if ((Vtable == nullptr) || (Vtable[30] == nullptr))
		{
			return nullptr;
		}

		typedef void* (__fastcall* Get_Studio_Model_Type)(void* Ecx, void* Edx, void* Model);

		return ((Get_Studio_Model_Type)Vtable[30])(Model_Info_Interface, nullptr, Model);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static void* Prepare_Hitbox_Model(void* Entity, float Bone_Matrix[128][3][4])
{
	if (Entity == nullptr)
	{
		return nullptr;
	}

	void* Renderable = (void*)((unsigned __int32)Entity + 0x4);

	void** Renderable_Vtable = *(void***)Renderable;

	if ((Renderable_Vtable == nullptr) || (Renderable_Vtable[13] == nullptr) || (Renderable_Vtable[8] == nullptr))
	{
		return nullptr;
	}

	if ((Address_In_Client_Module((unsigned __int32)Renderable_Vtable[13]) == false) || (Address_In_Client_Module((unsigned __int32)Renderable_Vtable[8]) == false))
	{
		return nullptr;
	}

	Global_Variables_Structure* Global_Variables = *(Global_Variables_Structure**)((unsigned __int32)Client_Module + 7096744);

	if (Global_Variables == nullptr)
	{
		return nullptr;
	}

	memset(Bone_Matrix, 0, 128 * 3 * 4 * sizeof(float));

	__try
	{
		typedef bool(__fastcall* Setup_Bones_Type)(void* Ecx, void* Edx, void* Bone_Matrix, __int32 Max_Bones, __int32 Bone_Mask, float Current_Time);

		const bool Bones_Ok = ((Setup_Bones_Type)Renderable_Vtable[13])(Renderable, nullptr, Bone_Matrix, 128, BONE_USED_BY_HITBOX, Global_Variables->Time);

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

		void* Studio_Hdr = Get_Studio_Model(Model);

		if (Studio_Hdr == nullptr)
		{
			return nullptr;
		}

		if (*(unsigned __int32*)Studio_Hdr != 0x54534449)
		{
			return nullptr;
		}

		const __int32 Hitbox_Set_Count = *(__int32*)((unsigned __int32)Studio_Hdr + 0xAC);

		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int32)Studio_Hdr + 0xB0);

		if ((Hitbox_Set_Count <= 0) || (Hitbox_Set_Index <= 0))
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

static __int32 Find_Hitbox_By_Name(void* Studio_Hdr, const char* Name)
{
	__try
	{
		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int32)Studio_Hdr + 0xB0);

		if ((Hitbox_Set_Index <= 0) || (Hitbox_Set_Index >= 0x1000000))
		{
			return -1;
		}

		void* Hitbox_Set = (void*)((unsigned __int32)Studio_Hdr + Hitbox_Set_Index);

		const __int32 Num_Hitboxes = *(__int32*)((unsigned __int32)Hitbox_Set + 0x04);

		const __int32 Hitbox_Index = *(__int32*)((unsigned __int32)Hitbox_Set + 0x08);

		if ((Num_Hitboxes <= 0) || (Num_Hitboxes >= 128) || (Hitbox_Index <= 0) || (Hitbox_Index >= 0x1000000))
		{
			return -1;
		}

		for (__int32 i = 0; i < Num_Hitboxes; i++)
		{
			void* Data = (void*)((unsigned __int32)Hitbox_Set + Hitbox_Index + i * 0x44);

			const __int32 Name_Index = *(__int32*)((unsigned __int32)Data + 0x20);

			if ((Name_Index <= 0) || (Name_Index >= 0x1000000))
			{
				continue;
			}

			const char* Box_Name = (const char*)((unsigned __int32)Studio_Hdr + Name_Index);

			if (_stricmp(Box_Name, Name) == 0)
			{
				return i;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return -1;
}

static __int32 Find_Head_Hitbox(void* Studio_Hdr, float Bone_Matrix[128][3][4])
{
	__try
	{
		const __int32 Found = Find_Hitbox_By_Name(Studio_Hdr, "head");

		if (Found >= 0)
		{
			return Found;
		}

		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int32)Studio_Hdr + 0xB0);

		if ((Hitbox_Set_Index <= 0) || (Hitbox_Set_Index >= 0x1000000))
		{
			return -1;
		}

		void* Hitbox_Set = (void*)((unsigned __int32)Studio_Hdr + Hitbox_Set_Index);

		const __int32 Num_Hitboxes = *(__int32*)((unsigned __int32)Hitbox_Set + 0x04);

		const __int32 Hitbox_Index = *(__int32*)((unsigned __int32)Hitbox_Set + 0x08);

		if ((Num_Hitboxes <= 0) || (Num_Hitboxes >= 128) || (Hitbox_Index <= 0) || (Hitbox_Index >= 0x1000000))
		{
			return -1;
		}

		__int32 Best = -1;

		float Best_Top = -99999.f;

		for (__int32 i = 0; i < Num_Hitboxes; i++)
		{
			void* Data = (void*)((unsigned __int32)Hitbox_Set + Hitbox_Index + i * 0x44);

			const __int32 Bone = *(__int32*)Data;

			if ((Bone < 0) || (Bone >= 128))
			{
				continue;
			}

			const float* BB_Max = (const float*)((unsigned __int32)Data + 0x14);

			float Max[3];

			Vector_Transform(BB_Max, Bone_Matrix[Bone], Max);

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

static bool Read_Hitbox(void* Studio_Hdr, float Bone_Matrix[128][3][4], __int32 Hitbox, bool Top_Only, float Out[3])
{
	__try
	{
		const __int32 Hitbox_Set_Index = *(__int32*)((unsigned __int32)Studio_Hdr + 0xB0);

		void* Hitbox_Set = (void*)((unsigned __int32)Studio_Hdr + Hitbox_Set_Index);

		const __int32 Num_Hitboxes = *(__int32*)((unsigned __int32)Hitbox_Set + 0x04);

		const __int32 Hitbox_Index = *(__int32*)((unsigned __int32)Hitbox_Set + 0x08);

		if ((Hitbox < 0) || (Hitbox >= Num_Hitboxes) || (Hitbox_Index <= 0) || (Hitbox_Index >= 0x1000000))
		{
			return false;
		}

		void* Hitbox_Data = (void*)((unsigned __int32)Hitbox_Set + Hitbox_Index + Hitbox * 0x44);

		const __int32 Bone = *(__int32*)Hitbox_Data;

		if ((Bone < 0) || (Bone >= 128))
		{
			return false;
		}

		const float* BB_Min = (const float*)((unsigned __int32)Hitbox_Data + 0x08);

		const float* BB_Max = (const float*)((unsigned __int32)Hitbox_Data + 0x14);

		float Min[3];

		float Max[3];

		Vector_Transform(BB_Min, Bone_Matrix[Bone], Min);

		Vector_Transform(BB_Max, Bone_Matrix[Bone], Max);

		Out[0] = (Min[0] + Max[0]) * 0.5f;

		Out[1] = (Min[1] + Max[1]) * 0.5f;

		if (Top_Only == true)
		{
			Out[2] = Max[2];
		}
		else
		{
			Out[2] = (Min[2] + Max[2]) * 0.5f;
		}

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Get_Head_Origin(void* Entity, __int32 Index_Fallback, bool Top_Only, float Out[3])
{
	__try
	{
		float Bone_Matrix[128][3][4];

		void* Studio_Hdr = Prepare_Hitbox_Model(Entity, Bone_Matrix);

		if (Studio_Hdr == nullptr)
		{
			return false;
		}

		__int32 Hitbox = Find_Head_Hitbox(Studio_Hdr, Bone_Matrix);

		if (Hitbox < 0)
		{
			Hitbox = Index_Fallback;
		}

		return Read_Hitbox(Studio_Hdr, Bone_Matrix, Hitbox, Top_Only, Out);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Get_Hitbox_Origin(void* Entity, __int32 Index_Fallback, const char* Name, float Out[3])
{
	__try
	{
		float Bone_Matrix[128][3][4];

		void* Studio_Hdr = Prepare_Hitbox_Model(Entity, Bone_Matrix);

		if (Studio_Hdr == nullptr)
		{
			return false;
		}

		__int32 Hitbox = Index_Fallback;

		if (Name != nullptr)
		{
			const __int32 Found = Find_Hitbox_By_Name(Studio_Hdr, Name);

			if (Found >= 0)
			{
				Hitbox = Found;
			}
		}

		return Read_Hitbox(Studio_Hdr, Bone_Matrix, Hitbox, false, Out);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

enum { Kind_Invalid = 0, Kind_Survivor, Kind_Boomer, Kind_Smoker, Kind_Hunter, Kind_Spitter, Kind_Jockey, Kind_Charger, Kind_Tank, Kind_Witch, Kind_Common, Kind_Rock };

static const char* Get_Kind_Name(__int32 Kind)
{
	switch (Kind)
	{
	case Kind_Survivor: return "Survivor";
	case Kind_Boomer: return "Boomer";
	case Kind_Smoker: return "Smoker";
	case Kind_Hunter: return "Hunter";
	case Kind_Spitter: return "Spitter";
	case Kind_Jockey: return "Jockey";
	case Kind_Charger: return "Charger";
	case Kind_Tank: return "Tank";
	case Kind_Witch: return "Witch";
	case Kind_Common: return "Common";
	case Kind_Rock: return "Rock";
	}

	return "Invalid";
}

static __int32 Get_Entity_Kind_Fast(void* Entity)
{
	Client_Class_Structure* Class = Get_Client_Class_Fast(Entity);

	if (Class == nullptr)
	{
		return Kind_Invalid;
	}

	const char* Name = Class->Network_Name;

	if (Name == nullptr)
	{
		return Kind_Invalid;
	}

	if (_stricmp(Name, "CTerrorPlayer") == 0) return Kind_Survivor;

	if (_stricmp(Name, "SurvivorBot") == 0) return Kind_Survivor;

	if (_stricmp(Name, "Boomer") == 0) return Kind_Boomer;

	if (_stricmp(Name, "Smoker") == 0) return Kind_Smoker;

	if (_stricmp(Name, "Hunter") == 0) return Kind_Hunter;

	if (_stricmp(Name, "Spitter") == 0) return Kind_Spitter;

	if (_stricmp(Name, "Jockey") == 0) return Kind_Jockey;

	if (_stricmp(Name, "Charger") == 0) return Kind_Charger;

	if (_stricmp(Name, "Tank") == 0) return Kind_Tank;

	if (_stricmp(Name, "Witch") == 0) return Kind_Witch;

	if (_stricmp(Name, "Infected") == 0) return Kind_Common;

	if ((strstr(Name, "Infected") != nullptr) || (strstr(Name, "Common") != nullptr) || (strstr(Name, "Zombie") != nullptr))
	{
		return Kind_Common;
	}

	if (_stricmp(Name, "TankRock") == 0) return Kind_Rock;

	return Kind_Invalid;
}

static __int32 Get_Entity_Kind(void* Entity)
{
	__try
	{
		Client_Class_Structure* Class = Get_Client_Class(Entity);

		if (Class == nullptr)
		{
			return Kind_Invalid;
		}

		const char* Name = Class->Network_Name;

		if (Name == nullptr)
		{
			return Kind_Invalid;
		}

		if (_stricmp(Name, "CTerrorPlayer") == 0) return Kind_Survivor;

		if (_stricmp(Name, "SurvivorBot") == 0) return Kind_Survivor;

		if (_stricmp(Name, "Boomer") == 0) return Kind_Boomer;

		if (_stricmp(Name, "Smoker") == 0) return Kind_Smoker;

		if (_stricmp(Name, "Hunter") == 0) return Kind_Hunter;

		if (_stricmp(Name, "Spitter") == 0) return Kind_Spitter;

		if (_stricmp(Name, "Jockey") == 0) return Kind_Jockey;

		if (_stricmp(Name, "Charger") == 0) return Kind_Charger;

		if (_stricmp(Name, "Tank") == 0) return Kind_Tank;

		if (_stricmp(Name, "Witch") == 0) return Kind_Witch;

		if (_stricmp(Name, "Infected") == 0) return Kind_Common;

		if ((strstr(Name, "Infected") != nullptr) || (strstr(Name, "Common") != nullptr) || (strstr(Name, "Zombie") != nullptr))
		{
			return Kind_Common;
		}

		if (_stricmp(Name, "TankRock") == 0) return Kind_Rock;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return Kind_Invalid;
	}

	return Kind_Invalid;
}

using Get_Weapon_Type = void*(__thiscall*)(void* Player);

static void* Get_Active_Weapon(void* Player)
{
	const Get_Weapon_Type Get_Active_Weapon_Fn = (Get_Weapon_Type)((unsigned __int32)Client_Module + 74304);

	return Get_Active_Weapon_Fn(Player);
}

static float Get_Head_Height(__int32 Kind)
{
	switch (Kind)
	{
	case Kind_Tank: return 100.f;
	case Kind_Charger: return 85.f;
	case Kind_Jockey: return 52.f;
	case Kind_Spitter: return 52.f;
	case Kind_Common: return 66.f;
	case Kind_Witch: return 66.f;
	}

	return 72.f;
}

static void Get_Velocity(void* Player, float Out[3])
{
	const __int32 Velocity_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecVelocity[0]");

	if (Velocity_Offset < 0)
	{
		Out[0] = 0.f;

		Out[1] = 0.f;

		Out[2] = 0.f;

		return;
	}

	const float* Velocity = (const float*)((unsigned __int32)Player + Velocity_Offset);

	Out[0] = Velocity[0];

	Out[1] = Velocity[1];

	Out[2] = Velocity[2];
}

typedef bool(__thiscall* Sdk_Get_Player_Info_Type)(void* This, __int32 Index, void* Info);

static __int32 Sdk_Player_Info_Slot = -2;

static void* Sdk_Player_Info_Object;

static __int32 Sdk_Player_Name_Offset = 8;

static bool Sdk_Plausible_Name(const char* Buffer, __int32 Max_Length)
{
	for (__int32 i = 0; i < Max_Length; i++)
	{
		const unsigned __int8 C = (unsigned __int8)Buffer[i];

		if (C == 0)
		{
			return i > 0;
		}

		if ((C < 0x20) || (C == 0x7F))
		{
			return false;
		}
	}

	return false;
}

static void* Sdk_Engine_Client_Object()
{
	__try
	{
		const unsigned __int32 Address = (unsigned __int32)Engine_Module + 0x425504;

		if (Sdk_Is_Readable_Range((void*)Address, sizeof(void*)) == true)
		{
			void** Vtable = *(void***)Address;

			if ((Vtable != nullptr) && (Sdk_Is_Readable_Range(Vtable, sizeof(void*)) == true) && (Sdk_Address_In_Module(Engine_Module, (unsigned __int32)Vtable[0]) == true))
			{
				return (void*)Address;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return Get_Interface(Engine_Module, "VEngineClient013");
}

static bool Sdk_Probe_Player_Info_Slot(void* Object, __int32 Slot, __int32 Probe_Index, char* Buffer, __int32* Name_Offset)
{
	__try
	{
		if (Object == nullptr)
		{
			return false;
		}

		void** Vtable = *(void***)Object;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, (Slot + 1) * sizeof(void*)) == false) || (Vtable[Slot] == nullptr))
		{
			return false;
		}

		const unsigned __int32 Function = (unsigned __int32)Vtable[Slot];

		if (Sdk_Address_In_Module(Engine_Module, Function) == false)
		{
			return false;
		}

		unsigned __int8 Info[0x2000] = { 0 };

		const bool Ok = ((Sdk_Get_Player_Info_Type)Function)(Object, Probe_Index, Info);

		if (Ok == false)
		{
			return false;
		}

		if (Sdk_Plausible_Name((const char*)Info, sizeof(Info)) == true)
		{
			*Name_Offset = 0;

			return true;
		}

		if (Sdk_Plausible_Name((const char*)Info + 8, sizeof(Info) - 8) == true)
		{
			*Name_Offset = 8;

			return true;
		}

		return false;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static void Sdk_Find_Player_Info_Slot()
{
	__try
	{
		if (Sdk_Player_Info_Slot != -2)
		{
			return;
		}

		Sdk_Player_Info_Slot = -1;

		if (Engine_Module == nullptr)
		{
			return;
		}

		Sdk_Player_Info_Object = Sdk_Engine_Client_Object();

		if (Sdk_Player_Info_Object == nullptr)
		{
			return;
		}

		char Buffer[0x2000];

		const __int32 Candidates[] = { 9, 8, 10 };

		const __int32 Probes[] = { 1, 2 };

		for (__int32 c = 0; c < 3; c++)
		{
			for (__int32 p = 0; p < 2; p++)
			{
				__int32 Offset = 8;

				if (Sdk_Probe_Player_Info_Slot(Sdk_Player_Info_Object, Candidates[c], Probes[p], Buffer, &Offset) == true)
				{
					Sdk_Player_Info_Slot = Candidates[c];

					Sdk_Player_Name_Offset = Offset;

					return;
				}
			}
		}

		void** Vtable = *(void***)Sdk_Player_Info_Object;

		const unsigned __int32 Anchor = (unsigned __int32)Engine_Module + 0x5DA40;

		if ((Vtable != nullptr) && (Sdk_Is_Readable_Range(Vtable, 64 * sizeof(void*)) == true))
		{
			for (__int32 i = 0; i < 63; i++)
			{
				if ((unsigned __int32)Vtable[i] != Anchor)
				{
					continue;
				}

				__int32 Offset = 8;

				if (Sdk_Probe_Player_Info_Slot(Sdk_Player_Info_Object, i + 1, 1, Buffer, &Offset) == true)
				{
					Sdk_Player_Info_Slot = i + 1;

					Sdk_Player_Name_Offset = Offset;

					return;
				}
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

struct Sdk_Player_Info_Structure
{
	unsigned __int8 Padding[8];

	char Name[32];

	__int32 User_ID;

	char Steam_ID[33];

	unsigned __int8 Padding_2[0x27];

	bool Is_Bot;

	unsigned __int8 Padding_3[0x1B];
};

static bool Sdk_Get_Player_Info(__int32 Index, Sdk_Player_Info_Structure* Info_Out)
{
	if ((Info_Out == nullptr) || (Index < 1))
	{
		return false;
	}

	__try
	{
		Sdk_Find_Player_Info_Slot();

		if ((Sdk_Player_Info_Slot < 0) || (Sdk_Player_Info_Object == nullptr))
		{
			return false;
		}

		void** Vtable = *(void***)Sdk_Player_Info_Object;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, (Sdk_Player_Info_Slot + 1) * sizeof(void*)) == false) || (Vtable[Sdk_Player_Info_Slot] == nullptr))
		{
			return false;
		}

		unsigned __int8 Info[0x2000] = { 0 };

		bool Ok = ((Sdk_Get_Player_Info_Type)Vtable[Sdk_Player_Info_Slot])(Sdk_Player_Info_Object, Index, Info);

		if (Ok == false)
		{
			const __int32 Low_Index = Index - 0x1000;

			if (Low_Index <= 0)
			{
				return false;
			}

			memset(Info, 0, sizeof(Info));

			Ok = ((Sdk_Get_Player_Info_Type)Vtable[Sdk_Player_Info_Slot])(Sdk_Player_Info_Object, Low_Index, Info);
		}

		if (Ok == false)
		{
			return false;
		}

		memset(Info_Out, 0, sizeof(Sdk_Player_Info_Structure));

		memcpy(Info_Out, Info, sizeof(Sdk_Player_Info_Structure));

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Vortex_Get_Player_Name(__int32 Index, char* Buffer, __int32 Buffer_Size)
{
	if ((Buffer == nullptr) || (Buffer_Size <= 0))
	{
		return false;
	}

	Buffer[0] = '\0';

	Sdk_Player_Info_Structure Info;

	memset(&Info, 0, sizeof(Info));

	if (Sdk_Get_Player_Info(Index, &Info) == false)
	{
		return false;
	}

	const char* Name = Info.Name;

	if (Sdk_Plausible_Name(Name, sizeof(Info.Name)) == false)
	{
		return false;
	}

	strncpy_s(Buffer, Buffer_Size, Name, _TRUNCATE);

	return true;
}