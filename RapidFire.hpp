#pragma once

#include <string.h>

#include <intrin.h>

#include "Internal.hpp"

#include "D3D9.hpp"

#include "TickbaseFix.hpp"

#include "NoSpread.hpp"

const unsigned __int32 Rapid_Fire_Pointer_Offset = 4352236;

const unsigned __int32 Rapid_Fire_Bind_Key = 'F';

typedef void(__cdecl* CL_Move_Type)(float Accumulated_Extra_Samples, bool Final_Tick);

static CL_Move_Type Original_CL_Move;

static unsigned __int32 CL_Move_Address;

struct Extended_Command_Structure
{
	__int32 Extra_Commands;

	__int32 Sequence_Shift;
};

static Extended_Command_Structure Extended_Commands[150];

static __int32 Accumulative_Correction;

static __int8 Consistent_Time;

static void* Get_Active_Weapon(void* Player);

static void Update_Auto_Pistol(UserCmd_Structure* Command)
{
	if ((Rapid_Fire_Enabled == false) || (Command == nullptr))
	{
		return;
	}

	if ((Command->Buttons & 1) == 0)
	{
		return;
	}

	const unsigned __int32 Local_Player = Get_Local_Player();

	__int8 Is_Semi_Auto = 0;

	if (Local_Player != 0)
	{
		void* Weapon = Get_Active_Weapon((void*)Local_Player);

		if (Weapon != nullptr)
		{
			void** Weapon_Vtable = *(void***)Weapon;

			if ((Weapon_Vtable != nullptr) && (Weapon_Vtable[383] != nullptr))
			{
				typedef __int32(__fastcall* Get_Weapon_Type_Type)(void* Ecx, void* Edx);

				Is_Semi_Auto = Rage_Weapon_Auto_Pistol(((Get_Weapon_Type_Type)Weapon_Vtable[383])(Weapon, nullptr)) ? 1 : 0;
			}
		}
	}

	if (Is_Semi_Auto == 0)
	{
		return;
	}

	static bool Rapid_Shoot = false;

	if ((Rapid_Shoot == true) && ((Command->Buttons & 1) != 0))
	{
		Command->Buttons &= ~1;
	}

	Rapid_Shoot = ((Command->Buttons & 1) != 0);
}

static unsigned __int32 Get_Tick_Manipulation_Queue()
{
	const unsigned __int32 Local_Player = Get_Local_Player();

	if (Local_Player == 0)
	{
		return 0;
	}

	const bool Key_Active = (GetAsyncKeyState(Rapid_Fire_Bind_Key) & 0x8000) != 0;

	if ((Tick_Manipulation_Enabled == false) && (Key_Active == false))
	{
		return 0;
	}

	if (Tick_Manipulation_Interact == true)
	{
		if (*(__int32*)(Local_Player + 228) == 3)
		{
			if ((*(unsigned __int8*)(Local_Player + 7322) == 0) && (*(__int32*)(Local_Player + 10008) == Invalid_Handle))
			{
				if ((*(__int32*)(Local_Player + 10012) != Invalid_Handle) || (*(__int32*)(Local_Player + 10024) != Invalid_Handle) || (*(__int32*)(Local_Player + 10056) != Invalid_Handle))
				{
					return Tick_Manipulation_Ticks;
				}
			}
		}
		else
		{
			if ((*(__int32*)(Local_Player + 7076) == *(__int32*)(Local_Player + 376)) || (*(__int32*)(Local_Player + 8076) != Invalid_Handle))
			{
				return Tick_Manipulation_Ticks;
			}
		}
	}

	return 0;
}

static bool Rapid_Fire_Key_Active()
{
	return (GetAsyncKeyState(Rapid_Fire_Bind_Key) & 0x8000) != 0;
}

static bool Rapid_Fire_Active()
{
	if (Tick_Manipulation_Enabled == true)
	{
		return true;
	}

	return Rapid_Fire_Key_Active();
}

static void* Original_Copy_Command;

static void __fastcall Redirected_Copy_Command_Body(void* Unknown_Parameter, void* User_Command, void* Frame_Base)
{
	const __int32 Command_Number = *(__int32*)((unsigned __int32)User_Command + 4);

	Extended_Command_Structure* Extended_Command = &Extended_Commands[Command_Number % 150];

	Extended_Command->Extra_Commands = 0;

	Extended_Command->Sequence_Shift = 0;

	const unsigned __int32 Local_Player = Get_Local_Player();

	if ((Local_Player != 0) && (Rapid_Fire_Active() == true))
	{
		Global_Variables_Structure* Global_Variables = *(Global_Variables_Structure**)((unsigned __int32)Client_Module + 7096744);

		if (Extra_Commands == -1)
		{
			*(float*)(Local_Player + 16) = Global_Variables->Interval_Per_Tick;

			*(__int32*)(Local_Player + 20) = Command_Number;

			if (Rapid_Fire_Key_Active() == true)
			{
				Extended_Command->Extra_Commands = max(0, Extra_Commands = min(max((__int32)Tick_Manipulation_Ticks, (__int32)(0.06f / Global_Variables->Interval_Per_Tick + 0.5f)), 14));

				*(float*)(Local_Player + 16) *= 1.f + (float)Extended_Command->Extra_Commands;

				Request_Tick_Base_Fix();
			}

			const unsigned __int32 Variables = *(__int32*)(Local_Player + 24);

			if (Variables != 0)
			{
				__int32 Variable_Number = 0;

				while (Variable_Number != *(__int32*)(Local_Player + 36))
				{
					void* Variable = *(void**)(Variables + Variable_Number * 12 + 8);

					*(float*)((unsigned __int32)Variable + 36) = *(float*)(Local_Player + 16);

					Variable_Number += 1;
				}
			}
		}

		if (*(unsigned __int8*)(Local_Player + 0x147) == 0)
		{
			Extended_Command_Structure* Initial_Extended_Command = &Extended_Commands[*(__int32*)(Local_Player + 20) % 150];

			void* Network_Channel = *(void**)(*(unsigned __int32*)((unsigned __int32)Engine_Module + Rapid_Fire_Pointer_Offset) + 24);

			auto Sequence_Shift = [&](__int32 Reserve) -> void
			{
				if (Initial_Extended_Command->Sequence_Shift == 0)
				{
					__int32 Shift = (*(__int32*)(Local_Player + 5324) - Extended_Command->Extra_Commands + 149) / 150 * 150 + (Reserve * 150);

					if (Shift != 0)
					{
						*(__int32*)((unsigned __int32)Network_Channel + 8) += Shift;

						Extended_Command->Sequence_Shift += Shift;

						__int32 Cmd = Command_Number;

						while ((Cmd - 1) >= *(__int32*)(Local_Player + 20))
						{
							Cmd -= 1;

							Extended_Commands[Cmd % 150].Sequence_Shift = Extended_Command->Sequence_Shift;
						}
					}
				}
			};

			auto Disable_Clock_Correction = [&](__int32 Queue) -> void
			{
				if (Queue > 0)
				{
					if (Extended_Command == Initial_Extended_Command)
					{
						Extended_Command->Extra_Commands = 0;

						Extra_Commands = max((__int32)(0.06f / Global_Variables->Interval_Per_Tick + 0.5f), Queue);
					}
					else
					{
						Accumulative_Correction += 1;
					}

					if (Initial_Extended_Command->Extra_Commands == 0)
					{
						*(__int32*)((unsigned __int32)Network_Channel + 16) = -1;

						*(__int32*)((unsigned __int32)Network_Channel + 28) = 255;
					}
				}
			};

			auto Correct_Extended_Command = [&]() -> void
			{
				if (*(__int32*)((unsigned __int32)Network_Channel + 16) != -1)
				{
					Extended_Command->Extra_Commands += Accumulative_Correction;

					Accumulative_Correction = 0;
				}

				Extended_Command->Sequence_Shift = Initial_Extended_Command->Sequence_Shift;
			};
			const unsigned __int32 Interact_Queue = Get_Tick_Manipulation_Queue();

			if (Interact_Queue != 0)
			{
				Disable_Clock_Correction((__int32)Interact_Queue);
			}

			if (*(__int32*)(Local_Player + 228) == 3)
			{
				if (*(unsigned __int8*)(Local_Player + 7322) == 0)
				{
					if (*(__int32*)(Local_Player + 10008) == Invalid_Handle)
					{
						if ((*(__int32*)(Local_Player + 10012) != Invalid_Handle) || (*(__int32*)(Local_Player + 10024) != Invalid_Handle) || (*(__int32*)(Local_Player + 10056) != Invalid_Handle))
						{
							*(__int32*)((unsigned __int32)User_Command + 36) |= (*(__int32*)(Local_Player + 10056) != Invalid_Handle) * 2;
						}
					}
					else
					{
						Sequence_Shift(2);
					}
				}
				else
				{
					if (*(__int32*)(Local_Player + 7324) == 0)
					{
						Sequence_Shift(-2);
					}
				}

				Correct_Extended_Command();
			}
			else
			{
				if ((*(float*)(Local_Player + 4604) + 800.f * Global_Variables->Interval_Per_Tick >= 560.f) + (*(unsigned __int8*)(Local_Player + 8068)) + (*(unsigned __int8*)(Local_Player + 9708)) != 0)
				{
					Sequence_Shift(2);
				}

				Correct_Extended_Command();

				*(__int32*)(Local_Player + 5620) = Command_Number;
			}

			*(unsigned __int8*)((unsigned __int32)Frame_Base + 235) = (unsigned __int8)max((Extra_Commands <= 0) ? 1 : 0, (*(__int32*)((unsigned __int32)Network_Channel + 16) == -1) ? 1 : 0);
		}
	}

	if (User_Command != nullptr)
	{
		Run_Vortex_No_Spread((UserCmd_Structure*)User_Command);
	}

	((void(__thiscall*)(void*, void*))Original_Copy_Command)(Unknown_Parameter, User_Command);
}

static void __declspec(naked) Copy_Command_Stub()
{
	__asm
	{
		push ebp
		mov ebp, esp
		push ebp
		mov edx, [ebp + 8]
		call Redirected_Copy_Command_Body
		pop ebp
		ret 4
	}
}

static void* Original_Run_Command;

static void __fastcall Redirected_Run_Command(void* Prediction, void* Ignored_Edx, void* Player, void* Command, void* Move_Helper)
{
	if (Rapid_Fire_Active() == false)
	{
		((void(__thiscall*)(void*, void*, void*, void*))Original_Run_Command)(Prediction, Player, Command, Move_Helper);

		return;
	}

	Extended_Command_Structure* Extended_Command = &Extended_Commands[*(__int32*)((unsigned __int32)Player + 5620) % 150];

	*(__int32*)((unsigned __int32)Player + 5324) -= Extended_Command->Extra_Commands + Extended_Command->Sequence_Shift;

	((void(__thiscall*)(void*, void*, void*, void*))Original_Run_Command)(Prediction, Player, Command, Move_Helper);

	if ((unsigned __int32)_ReturnAddress() == (unsigned __int32)Client_Module + 423107)
	{
		*(__int32*)((unsigned __int32)Player + 5324) += Extended_Command->Sequence_Shift;

		Global_Variables_Structure* Global_Variables = *(Global_Variables_Structure**)((unsigned __int32)Client_Module + 7096744);

		Global_Variables->Time = (float)(*(__int32*)((unsigned __int32)Player + 5324) - 1) * Global_Variables->Interval_Per_Tick;
	}
}

static void* Original_Update;

static void __fastcall Redirected_Update(void* Unknown_Parameter_1, void* Ignored_Edx, void* Unknown_Parameter_2, void* Unknown_Parameter_3, __int32 Incoming_Sequence_Number, __int32 Outgoing_Sequence_Number)
{
	if (Rapid_Fire_Active() == false)
	{
		((void(__thiscall*)(void*, void*, void*, __int32, __int32))Original_Update)(Unknown_Parameter_1, Unknown_Parameter_2, Unknown_Parameter_3, Incoming_Sequence_Number, Outgoing_Sequence_Number);

		return;
	}

	Consistent_Time = (Outgoing_Sequence_Number - Incoming_Sequence_Number > 150) ? 1 : 0;

	((void(__thiscall*)(void*, void*, void*, __int32, __int32))Original_Update)(Unknown_Parameter_1, Unknown_Parameter_2, Unknown_Parameter_3, Incoming_Sequence_Number, Incoming_Sequence_Number + (Outgoing_Sequence_Number - Incoming_Sequence_Number) % 150);
}

static void* Original_Read_Packets;

static void __cdecl Redirected_Read_Packets(__int8 Final)
{
	static __int8 Parsed_Packets;

	if ((Parsed_Packets == 0) && (Original_Read_Packets != 0))
	{
		((void(__cdecl*)(__int8))Original_Read_Packets)(Final);
	}

	Parsed_Packets = ((unsigned __int32)_ReturnAddress() != (unsigned __int32)Engine_Module + 1631183) ? 1 : 0;
}

static void* Original_Send_Move;

static void __cdecl Redirected_Send_Move()
{
	if (Rapid_Fire_Active() == true)
	{
		unsigned __int32 Client_State = *(unsigned __int32*)((unsigned __int32)Engine_Module + Rapid_Fire_Pointer_Offset);

		unsigned __int32 Network_Channel = (Client_State != 0) ? *(unsigned __int32*)(Client_State + 24) : 0;

		if (Network_Channel != 0)
		{
			*(__int32*)(Network_Channel + 28) = 255;
		}
	}

	((void(__cdecl*)())Original_Send_Move)();
}

struct Prediction_Copy_Structure
{
	__int8 Additionals_Bytes_1[8];

	void* Destination;

	void* Source;

	__int8 Additional_Bytes_2[48];

	void Construct(void* Destination, void* Source, void* Handler)
	{
		using Construct_Type = void(__fastcall*)(void*, void*, __int32, void*, __int8, void*, __int8, __int32, void*);

		Construct_Type((unsigned __int32)Client_Module + 1564512)(this, nullptr, 2, Destination, 1, Source, 0, 3, Handler);
	}
};

static Prediction_Copy_Structure Predicton_Copy;

struct Prediction_Field_Structure
{
	__int32 Type;

	char* Name;

	__int32 Offset;

	unsigned __int16 Size;

	__int8 Additionals_Bytes_1[18];

	__int32 Bytes;

	__int8 Additionals_Bytes_2[12];

	__int32 Flat_Offset[2];

	__int8 Additionals_Bytes_3[2];
};

static void Predicton_Copy_Compare(void* Unknown_Parameter_1, void* Unknown_Parameter_2, void* Unknown_Parameter_3, void* Unknown_Parameter_4, void* Unknown_Parameter_5, void* Unknown_Parameter_6, __int8 Within_Tolerance, void* Unknown_Parameter_7)
{
	Prediction_Field_Structure* Field = *(Prediction_Field_Structure**)((unsigned __int32)_AddressOfReturnAddress() + 56);

	__int8 Should_Copy = (Field->Flat_Offset[0] * Consistent_Time == 5324) ? 1 : 0;

	if (Should_Copy == 0)
	{
		if (Within_Tolerance == 1)
		{
			Should_Copy = (((256 - Field->Flat_Offset[0]) ^ (Field->Flat_Offset[0] - 244)) != 12) ? 1 : 0;
		}
	}

	if (Should_Copy == 1)
	{
		memcpy((void*)((unsigned __int32)Predicton_Copy.Destination + Field->Flat_Offset[0]), (void*)((unsigned __int32)Predicton_Copy.Source + Field->Flat_Offset[1]), Field->Bytes);
	}
}

static void* Original_Post_Network_Data_Received;

static void __fastcall Redirected_Post_Network_Data_Received(void* Unknown_Parameter, void* Ignored_Edx, __int32 Commands_Acknowledged)
{
	if (Rapid_Fire_Active() == true)
	{
		void* Local_Player = *(void**)((unsigned __int32)Client_Module + 7498712);

		if (Local_Player != nullptr)
		{
			void* Prediction_Frame = *(void**)((unsigned __int32)Local_Player + 900 + (150 - (((Commands_Acknowledged - 1) % 150 + 1) * 150) % 151) * 4);

			if (Prediction_Frame != nullptr)
			{
				Predicton_Copy.Construct(Local_Player, Prediction_Frame, (void*)Predicton_Copy_Compare);

				using Transfer_Data_Type = __int32(__thiscall*)(Prediction_Copy_Structure*, void*, __int32, void*);

				Transfer_Data_Type((unsigned __int32)Client_Module + 1573744)(&Predicton_Copy, nullptr, -1, (void*)((unsigned __int32)Client_Module + 7236480));
			}
		}
	}

	((void(__thiscall*)(void*, __int32))Original_Post_Network_Data_Received)(Unknown_Parameter, Commands_Acknowledged);
}

static void __cdecl CL_Move_Hook(float Accumulated_Extra_Samples, bool Final_Tick)
{
	Extra_Commands = -1;

	if (Rapid_Fire_Active() == true)
	{
		void* Local_Player = *(void**)((unsigned __int32)Client_Module + 7498712);

		if (Local_Player != nullptr)
		{
			Redirected_Read_Packets(Final_Tick ? 1 : 0);

			((void(__cdecl*)())((unsigned __int32)Engine_Module + 527776))();

			((void(__cdecl*)())((unsigned __int32)Engine_Module + 521648))();
		}

		while (true)
		{
			Original_CL_Move(Accumulated_Extra_Samples, Final_Tick);

			((void(__cdecl*)())((unsigned __int32)Engine_Module + 527776))();

			if (Extra_Commands <= 0)
			{
				break;
			}

			Extra_Commands -= 1;
		}
	}
	else
	{
		Original_CL_Move(Accumulated_Extra_Samples, Final_Tick);
	}

	Fix_Tick_Base();
}

static void Byte_Patch(unsigned __int32 Address, const unsigned __int8* Expected, const unsigned __int8* Patch, unsigned __int32 Size, const char* Name)
{
	if (Address == 0)
	{
		return;
	}

	if (memcmp((void*)Address, Expected, Size) != 0)
	{
		return;
	}

	DWORD Previous_Access_Rights;

	VirtualProtect((void*)Address, Size, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);

	memcpy((void*)Address, Patch, Size);

	VirtualProtect((void*)Address, Size, Previous_Access_Rights, &Previous_Access_Rights);
}

static void* Install_Trampoline_Hook(unsigned __int32 Address, unsigned __int32 Copy_Size, void* Hook_Function, const char* Name)
{
	if (Address == 0)
	{
		return nullptr;
	}

	const unsigned __int32 Trampoline = (unsigned __int32)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (Trampoline == 0)
	{
		return nullptr;
	}

	memcpy((void*)Trampoline, (void*)Address, Copy_Size);

	*(unsigned __int8*)(Trampoline + Copy_Size) = 0x68;

	*(unsigned __int32*)(Trampoline + Copy_Size + 1) = Address + Copy_Size;

	*(unsigned __int8*)(Trampoline + Copy_Size + 5) = 0xC3;

	DWORD Previous_Access_Rights;

	VirtualProtect((void*)Address, 6, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);

	*(unsigned __int8*)Address = 0x68;

	*(unsigned __int32*)(Address + 1) = (unsigned __int32)Hook_Function;

	*(unsigned __int8*)(Address + 5) = 0xC3;

	VirtualProtect((void*)Address, 6, Previous_Access_Rights, &Previous_Access_Rights);

	return (void*)Trampoline;
}

static void Install_Rapid_Fire()
{

	const unsigned __int32 Copy_Command_Address = (unsigned __int32)Find_Pattern(L"client.dll", "55 8B EC 8B 45 08 85 C0 74 0C 50 81 C1 9C 00 00 00 E8 ? ? ? ? 5D C2 04 00");

	if (Copy_Command_Address != 0)
	{
		const unsigned __int32 Trampoline = (unsigned __int32)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		if (Trampoline != 0)
		{
			memcpy((void*)Trampoline, (void*)Copy_Command_Address, 22);

			*(unsigned __int32*)(Trampoline + 0x12) = (Copy_Command_Address + 0x16 + *(__int32*)(Copy_Command_Address + 0x12)) - (Trampoline + 0x16);

			*(unsigned __int8*)(Trampoline + 22) = 0x68;

			*(unsigned __int32*)(Trampoline + 23) = Copy_Command_Address + 22;

			*(unsigned __int8*)(Trampoline + 27) = 0xC3;

			Original_Copy_Command = (void*)Trampoline;

			DWORD Previous_Access_Rights;

			VirtualProtect((void*)Copy_Command_Address, 6, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);

			*(unsigned __int8*)Copy_Command_Address = 0x68;

			*(unsigned __int32*)(Copy_Command_Address + 1) = (unsigned __int32)Copy_Command_Stub;

			*(unsigned __int8*)(Copy_Command_Address + 5) = 0xC3;

			VirtualProtect((void*)Copy_Command_Address, 6, Previous_Access_Rights, &Previous_Access_Rights);

		}
	}

	CL_Move_Address = (unsigned __int32)Find_Pattern(L"engine.dll", "55 8B EC 81 EC 9C 00 00 00 A1 ? ? ? ? 33 C5 89 45 FC 56 E8 ? ? ? ? 8B F0 83 7E 68 02 0F 8C");

	Original_CL_Move = (CL_Move_Type)Install_Trampoline_Hook(CL_Move_Address, 9, (void*)CL_Move_Hook, "CL_Move");

	Original_Run_Command = Install_Trampoline_Hook((unsigned __int32)Client_Module + 0x17B460, 6, (void*)Redirected_Run_Command, "Run_Command");

	Original_Update = Install_Trampoline_Hook((unsigned __int32)Client_Module + 0x17B780, 6, (void*)Redirected_Update, "Update");

	Original_Read_Packets = Install_Trampoline_Hook((unsigned __int32)Engine_Module + 0x79E40, 6, (void*)Redirected_Read_Packets, "Read_Packets");

	Original_Send_Move = Install_Trampoline_Hook((unsigned __int32)Engine_Module + 0x7CEC0, 8, (void*)Redirected_Send_Move, "Send_Move");

	Original_Post_Network_Data_Received = Install_Trampoline_Hook((unsigned __int32)Client_Module + 0x17BB40, 6, (void*)Redirected_Post_Network_Data_Received, "Post_Network_Data_Received");

	const unsigned __int8 Expected_74[1] = { 0x74 };
	const unsigned __int8 Patch_EB[1] = { 0xEB };

	Byte_Patch((unsigned __int32)Client_Module + 0x17B860, Expected_74, Patch_EB, 1, "Update branch (0x17B860)");

	Byte_Patch((unsigned __int32)Client_Module + 0x17C510, Expected_74, Patch_EB, 1, "RunSimulation branch (0x17C510)");

}