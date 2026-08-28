#pragma once

#include "Internal.hpp"

static __int32 Tick_Base_Offset;

static __int32 Resolve_Tick_Base_Offset()
{
	if (Tick_Base_Offset != 0)
	{
		return Tick_Base_Offset == -1 ? 0 : Tick_Base_Offset;
	}

	const unsigned __int32 String_Address = (unsigned __int32)Find_Pattern(L"client.dll", "6D 5F 6E 54 69 63 6B 42 61 73 65");

	if (String_Address == 0)
	{
		Tick_Base_Offset = -1;


		return 0;
	}

	const unsigned __int32 Modules[2] = { (unsigned __int32)Client_Module, (unsigned __int32)Engine_Module };

	for (int Module_Index = 0; Module_Index < 2; Module_Index++)
	{
		const unsigned __int32 Module_Start = Modules[Module_Index];

		if (Module_Start == 0)
		{
			continue;
		}

		const IMAGE_DOS_HEADER* Dos_Header = (const IMAGE_DOS_HEADER*)Module_Start;

		const IMAGE_NT_HEADERS* Nt_Headers = (const IMAGE_NT_HEADERS*)(Module_Start + Dos_Header->e_lfanew);

		const unsigned __int32 Image_Size = Nt_Headers->OptionalHeader.SizeOfImage;

		for (unsigned __int32 Address = Module_Start; Address < Module_Start + Image_Size - 4; Address += 4)
		{
			if (*(unsigned __int32*)Address == String_Address)
			{
				const __int32 Field_Type = *(__int32*)(Address - 4);

				const __int32 Field_Offset = *(__int32*)(Address + 4);

				const __int16 Field_Size = *(__int16*)(Address + 8);

				if ((Field_Type >= 1) && (Field_Type <= 0x20) && (Field_Offset > 0x100) && (Field_Offset < 0x20000) && (Field_Size >= 1) && (Field_Size <= 8))
				{
					Tick_Base_Offset = Field_Offset;


					return Field_Offset;
				}
			}
		}
	}

	Tick_Base_Offset = -1;


	return 0;
}

static bool Tick_Base_Fix_Requested;

static void Request_Tick_Base_Fix()
{
	Tick_Base_Fix_Requested = true;
}

static void Fix_Tick_Base()
{
	if (Tick_Base_Fix_Requested == false)
	{
		return;
	}

	Tick_Base_Fix_Requested = false;

	if (Tick_Base_Fix_Enabled == false)
	{
		return;
	}

	const unsigned __int32 Local_Player = Get_Local_Player();

	if (Local_Player == 0)
	{
		return;
	}

	if ((*(unsigned __int8*)(Local_Player + 0x147) != 0) || (*(__int32*)(Local_Player + 0xEC) <= 0))
	{
		return;
	}

	const __int32 Tick_Base = Resolve_Tick_Base_Offset();

	if (Tick_Base == 0)
	{
		return;
	}

	const unsigned __int32 Client_State = *(unsigned __int32*)((unsigned __int32)Engine_Module + 4352236);

	if (Client_State == 0)
	{
		return;
	}

	const unsigned __int32 Network_Channel = *(unsigned __int32*)(Client_State + 24);

	if (Network_Channel == 0)
	{
		return;
	}

	const __int32 Simulation_Ticks = *(__int32*)(Client_State + 0x4A50);

	if (Simulation_Ticks < 0)
	{
		return;
	}

	const __int32 Server_Tick = *(__int32*)(Client_State + 0x4A40);

	const __int32 Tick_Base_Value = *(__int32*)(Local_Player + Tick_Base);

	const unsigned __int32 Global_Variables = *(unsigned __int32*)((unsigned __int32)Client_Module + 7096744);

	if (Global_Variables == 0)
	{
		return;
	}

	const float Interval_Per_Tick = *(float*)(Global_Variables + 0x1C);

	if (Interval_Per_Tick <= 0.0f)
	{
		return;
	}

	float Correction_Seconds = (float)Tick_Base_Correction_Msecs / 1000.0f;

	if (Correction_Seconds < 0.0f)
	{
		Correction_Seconds = 0.0f;
	}

	if (Correction_Seconds > 1.1f)
	{
		Correction_Seconds = 1.1f;
	}

	const __int32 Correction_Ticks = (__int32)(0.5f + Correction_Seconds / Interval_Per_Tick);

	const __int32 Ideal_Final_Tick = Server_Tick + Correction_Ticks;

	const __int32 Estimated_Final_Tick = Tick_Base_Value + Simulation_Ticks;

	const __int32 Too_Fast_Limit = Ideal_Final_Tick + Correction_Ticks;

	const __int32 Too_Slow_Limit = Ideal_Final_Tick - Correction_Ticks;

	float Latency = 0.0f;

	const unsigned __int32 VTable = *(unsigned __int32*)Network_Channel;

	if (VTable != 0)
	{
		typedef float(__thiscall* Get_Latency_Type)(void* This, int Flow);

		const Get_Latency_Type Get_Latency = (Get_Latency_Type)(*(unsigned __int32*)(VTable + 0x24));

		if (Get_Latency != nullptr)
		{
			Latency = Get_Latency((void*)Network_Channel, 0) + Get_Latency((void*)Network_Channel, 1);
		}
	}

	if ((Latency < 0.0f) || (Latency > 1.1f))
	{
		Latency = 0.0f;
	}

	const __int32 Sim_Ticks_This_Frame = *(__int32*)(Global_Variables + 0x20);

	const __int32 Process = Sim_Ticks_This_Frame + (__int32)(0.5f + Latency / Interval_Per_Tick);

	if ((Estimated_Final_Tick > Too_Fast_Limit) || (Estimated_Final_Tick < Too_Slow_Limit))
	{
		const __int32 Corrected_Tick = Ideal_Final_Tick - Simulation_Ticks + Process;

		*(__int32*)(Local_Player + Tick_Base) = Corrected_Tick;

	}
}