#include <windows.h>

#include <cstdio>

#include <cstdarg>

#include <stdlib.h>

#include "Internal.hpp"

#include "D3D9.hpp"

#include "BunnyHop.hpp"

#include "Chams.hpp"

#include "Hitmarker.hpp"

#include "ModsOnline.hpp"

#include "ChatSpy.hpp"

#include "LagExploit.hpp"

#include "SafeRoomTP.hpp"

#include "ScopeBypass.hpp"

HMODULE Client_Module;

HMODULE Engine_Module;

bool Auto_Bhop_Enabled;

bool Strafe_Enabled;

bool Esp_Enabled;

bool Esp_Team2 = true;

bool Esp_Team3 = true;

bool Esp_Commons = true;

bool Esp_Names = true;

bool Esp_Distance = true;

bool Esp_Health_Bar = true;

bool Esp_Snap_Lines;

bool Hitmarker_Enabled = false;

__int32 Esp_Box_Style;

float Esp_Team2_Color[4] = { 0.f, 1.f, 0.f, 1.f };

float Esp_Team3_Color[4] = { 1.f, 0.25f, 0.25f, 1.f };

bool Watermark_Enabled = true;

bool No_Vomit_Enabled;

bool Chams_Enabled;

bool Chams_X_Ray = true;

int Chams_Style;

float Chams_Team2_Color[4] = { 0.f, 0.75f, 1.f, 1.f };

float Chams_Team3_Color[4] = { 1.f, 0.f, 0.f, 1.f };

void* Model_Render;

bool Rapid_Fire_Enabled;

bool Lag_Exploit_Enabled = false;
__int32 Lag_Exploit_Key = VK_MBUTTON;
__int32 Lag_Exploit_Value = 200;
bool Air_Stuck_Enabled = false;
__int32 Air_Stuck_Key = VK_XBUTTON1;
bool Charger_Turn_Enabled = false;
unsigned __int8  Charger_Turn_Original_Byte = 0;
bool            Charger_Turn_Patched = false;
unsigned __int32 Charger_Turn_Patch_Address = 0;

bool Safe_Room_TP_Enabled = false;
__int32 Safe_Room_TP_Key = 'P';

bool Tick_Base_Fix_Enabled = false;

__int32 Tick_Base_Correction_Msecs = 60;

bool Tick_Manipulation_Enabled;

bool Tick_Manipulation_Interact = true;

unsigned __int32 Tick_Manipulation_Ticks = 10;

__int32 Extra_Commands = -1;

bool No_Visual_Recoil_Enabled;

bool Scope_Bypass_Enabled = false;

bool Vortex_Aimbot_Enabled;

__int32 Vortex_Aimbot_Hitbox = 0;

float Vortex_Aimbot_Fov = 30.f;

float Vortex_Aimbot_Smooth = 5.f;

float Vortex_Aimbot_Distance = 30.f;

bool Vortex_Aimbot_Silent;

bool Vortex_Aimbot_Visible = true;

bool Vortex_Aimbot_Auto_Fire = true;

bool Vortex_Aimbot_Prediction = true;

bool Vortex_Aimbot_Prioritize_Players = false;

bool Vortex_Aimbot_Ignore_Common = false;

bool Vortex_Aimbot_Ignore_Tank;

bool Vortex_Aimbot_Ignore_Witch;

bool Vortex_No_Spread_Enabled;

bool Mods_Online_Enabled;

CreateMove_Type Original_CreateMove;

Override_View_Type Original_Override_View;

Draw_Model_Execute_Type Original_Draw_Model_Execute;

bool Chat_Spy_Enabled = true;

__int32 Chat_Spy_Method = 0;

Chat_Spy_Line_Structure Chat_Spy_Lines[8];

unsigned __int32 Chat_Spy_Line_Index;

static void __cdecl Invalid_Parameter_Handler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned __int32, unsigned __intptr)
{
}

static LONG WINAPI Exception_Hook(EXCEPTION_POINTERS* Info)
{
	if (Info->ExceptionRecord->ExceptionCode == 0xC0000417)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

static DWORD WINAPI Main_Thread(void*)
{
	_set_invalid_parameter_handler(Invalid_Parameter_Handler);

	AddVectoredExceptionHandler(1, Exception_Hook);

	Client_Module = GetModuleHandleW(L"client.dll");

	Engine_Module = GetModuleHandleW(L"engine.dll");

	if (Client_Module == nullptr)
	{
		return 0;
	}

	Mods_Online_Apply();

	Install_Lag_Exploit_And_Charger_Turn();

	Safe_Room_TP_Try_Install();

	Install_Scope_Bypass();

	Install_BunnyHop();

	Install_Chat_Spy();

	Install_Rapid_Fire();

	Install_Vortex_No_Spread();

	Install_Chams();

	Initialize_D3D9();

	if (Hitmarker_Enabled == true)
	{
		Install_Hitmarker();
	}

	Install_Esp();

	while (true)
	{
		Safe_Room_TP_Try_Install();

		Sleep(1000);
	}

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE Instance, unsigned __int32 Call_Reason, void*)
{
	if (Call_Reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(Instance);

		CreateThread(nullptr, 0, Main_Thread, nullptr, 0, nullptr);
	}

	return TRUE;
}