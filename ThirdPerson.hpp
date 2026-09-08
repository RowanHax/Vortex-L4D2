#pragma once

#include <windows.h>

#include "Internal.hpp"

#include "Sdk.hpp"

extern bool Third_Person_Enabled;

typedef void* (__thiscall* Third_Person_Find_Var_Type)(void* This, const char* Name);

typedef void(__thiscall* Third_Person_Remove_Flags_Type)(void* This, __int32 Flags);

typedef void(__thiscall* Third_Person_Set_Float_Type)(void* This, float Value);

static void* Third_Person_Cvar_Interface;

static bool Third_Person_Cvar_Tried;

static void* Third_Person_View_Distance_Cvar;

static bool Third_Person_View_Distance_Tried;

static bool Third_Person_Last_Enabled;

static void* Third_Person_Find_Var(const char* Name)
{
	__try
	{
		if (Third_Person_Cvar_Tried == false)
		{
			Third_Person_Cvar_Tried = true;

			Third_Person_Cvar_Interface = Get_Interface(GetModuleHandleW(L"vstdlib.dll"), "VEngineCvar007");

			if (Third_Person_Cvar_Interface == nullptr)
			{
				Third_Person_Cvar_Interface = Get_Interface(GetModuleHandleW(L"vstdlib.dll"), "VEngineCvar");
			}
		}

		if ((Third_Person_Cvar_Interface == nullptr) || (Name == nullptr))
		{
			return nullptr;
		}

		void** Vtable = *(void***)Third_Person_Cvar_Interface;

		const HMODULE VStd = GetModuleHandleW(L"vstdlib.dll");

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 14 * sizeof(void*)) == false) ||
			(Vtable[13] == nullptr) || (Sdk_Address_In_Module(VStd, (unsigned __int32)Vtable[13]) == false))
		{
			return nullptr;
		}

		return ((Third_Person_Find_Var_Type)Vtable[13])(Third_Person_Cvar_Interface, Name);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

static void Third_Person_Remove_Cheat_Flag(void* Var)
{
	__try
	{
		if (Var == nullptr)
		{
			return;
		}

		void** Vtable = *(void***)Var;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 5 * sizeof(void*)) == false) ||
			(Vtable[4] == nullptr))
		{
			return;
		}

		((Third_Person_Remove_Flags_Type)Vtable[4])(Var, 16384);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void Third_Person_Set_Value(void* Var, float Value)
{
	__try
	{
		if (Var == nullptr)
		{
			return;
		}

		void** Vtable = *(void***)Var;

		if ((Vtable == nullptr) || (Sdk_Is_Readable_Range(Vtable, 14 * sizeof(void*)) == false) ||
			(Vtable[13] == nullptr))
		{
			return;
		}

		((Third_Person_Set_Float_Type)Vtable[13])(Var, Value);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static void Third_Person_Update()
{
	__try
	{
		if (Third_Person_View_Distance_Tried == false)
		{
			Third_Person_View_Distance_Tried = true;

			Third_Person_View_Distance_Cvar = Third_Person_Find_Var("z_view_distance");

			if (Third_Person_View_Distance_Cvar != nullptr)
			{
				Third_Person_Remove_Cheat_Flag(Third_Person_View_Distance_Cvar);
			}
		}

		if (Third_Person_View_Distance_Cvar == nullptr)
		{
			return;
		}

		if (Third_Person_Last_Enabled == Third_Person_Enabled)
		{
			return;
		}

		Third_Person_Last_Enabled = Third_Person_Enabled;

		Third_Person_Set_Value(Third_Person_View_Distance_Cvar, (Third_Person_Enabled == true) ? -150.f : 0.f);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}