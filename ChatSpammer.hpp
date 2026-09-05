#pragma once

#include <windows.h>

#include <cstdio>

#include "Internal.hpp"

#include "D3D9.hpp"

#include "Sdk.hpp"

typedef void(__fastcall* Engine_Client_Cmd_Type)(void* Ecx, void* Edx, const char* Command_Text);

static void* Spammer_Engine_Client;

static Engine_Client_Cmd_Type Spammer_Client_Cmd;

static bool Spammer_Resolve_Tried;

static unsigned __int64 Spammer_Last_Send;

static void Update_Chat_Spammer()
{
	if ((Chat_Spammer_Enabled == false) || (Chat_Spammer_Message[0] == 0))
	{
		return;
	}

	if (Sdk_Game_Is_Ready() == false)
	{
		return;
	}

	const unsigned __int64 Now = GetTickCount64();

	const unsigned __int64 Interval_MS = (unsigned __int64)(Chat_Spammer_Interval * 1000.0);

	if ((Now - Spammer_Last_Send) < Interval_MS)
	{
		return;
	}

	__try
	{
		if (Spammer_Resolve_Tried == false)
		{
			Spammer_Resolve_Tried = true;

			Spammer_Engine_Client = Get_Interface(Engine_Module, "VEngineClient013");

			if (Spammer_Engine_Client != nullptr)
			{
				void** Vtable = *(void***)Spammer_Engine_Client;

				if ((Vtable != nullptr) && (Sdk_Is_Readable_Range(Vtable, 8 * sizeof(void*)) == true) &&
					(Vtable[7] != nullptr) && (Sdk_Address_In_Module(Engine_Module, (unsigned __int32)Vtable[7]) == true))
				{
					Spammer_Client_Cmd = (Engine_Client_Cmd_Type)Vtable[7];
				}
			}
		}

		if ((Spammer_Engine_Client == nullptr) || (Spammer_Client_Cmd == nullptr))
		{
			return;
		}

		char Buffer[256];

		sprintf_s(Buffer, "say \"%s\"", Chat_Spammer_Message);

		Spammer_Client_Cmd(Spammer_Engine_Client, nullptr, Buffer);

		Spammer_Last_Send = Now;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}