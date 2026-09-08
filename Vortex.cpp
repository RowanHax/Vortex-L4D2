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

#include "ThirdPerson.hpp"

#include "NameStealer.hpp"

HMODULE Client_Module;

HMODULE Engine_Module;

bool Auto_Bhop_Enabled;

bool Strafe_Enabled;

bool Esp_Enabled;

bool Esp_Team2 = false;

bool Esp_Team3 = false;

bool Esp_Commons = false;

bool Esp_Names = false;

bool Esp_Distance = false;

bool Esp_Health_Bar = false;

bool Esp_Snap_Lines;

bool Hitmarker_Enabled = false;

__int32 Esp_Box_Style;

float Esp_Team2_Color[4] = { 0.f, 1.f, 0.f, 1.f };

float Esp_Team3_Color[4] = { 1.f, 0.25f, 0.25f, 1.f };

bool No_Vomit_Enabled;

bool Chams_Enabled;

bool Chams_X_Ray = false;

int Chams_Style;

float Chams_Team2_Color[4] = { 0.f, 0.75f, 1.f, 1.f };

float Chams_Team3_Color[4] = { 1.f, 0.f, 0.f, 1.f };

void* Model_Render;

bool Rapid_Fire_Enabled;

bool Lag_Exploit_Enabled = false;
__int32 Lag_Exploit_Key = 'C';
__int32 Lag_Exploit_Value = 1200;
bool Air_Stuck_Enabled = false;
__int32 Air_Stuck_Key = 'X';
bool Roll_TP_Enabled = false;
__int32 Roll_TP_Key = 'P';
bool Charger_Turn_Enabled = false;
unsigned __int8  Charger_Turn_Original_Byte = 0;
bool            Charger_Turn_Patched = false;
unsigned __int32 Charger_Turn_Patch_Address = 0;

bool Safe_Room_TP_Enabled = false;
__int32 Safe_Room_TP_Key = 'P';

bool Tick_Base_Fix_Enabled = false;

__int32 Tick_Base_Correction_Msecs = 60;

bool Tick_Manipulation_Enabled;

bool Tick_Manipulation_Interact = false;

unsigned __int32 Tick_Manipulation_Ticks = 10;

__int32 Rapid_Fire_Bind_Key = 'F';

__int32 Extra_Commands = -1;

bool No_Visual_Recoil_Enabled;

bool Scope_Bypass_Enabled = false;

bool Third_Person_Enabled = false;

bool Name_Stealer_Enabled = false;

bool Vortex_Aimbot_Enabled;

__int32 Vortex_Aimbot_Key = 0;

__int32 Vortex_Aimbot_Hitbox = 0;

float Vortex_Aimbot_Fov = 30.f;

float Vortex_Aimbot_Smooth = 5.f;

float Vortex_Aimbot_Distance = 30.f;

bool Vortex_Aimbot_Silent;

bool Vortex_Aimbot_Auto_Fire = false;

bool Vortex_Aimbot_Prediction = false;

bool Vortex_Aimbot_Prioritize_Players = false;

bool Vortex_Aimbot_Ignore_Common = false;

bool Vortex_Aimbot_Ignore_Tank;

bool Vortex_Aimbot_Ignore_Witch;

bool Vortex_No_Spread_Enabled;

bool Mods_Online_Enabled;

bool Item_Esp_Enabled = false;

bool Item_Esp_Weapons_Text = false;

bool Item_Esp_Weapons_Boxes = false;

float Item_Esp_Weapons_Color[4] = { 1.f, 0.8f, 0.8f, 1.f };

bool Item_Esp_Heal_Enabled = false;

bool Item_Esp_Heal_Text = false;

bool Item_Esp_Heal_Boxes = false;

float Item_Esp_Heal_Color[4] = { 1.f, 0.6f, 1.f, 1.f };

bool Item_Esp_Mounted_Enabled = false;

bool Item_Esp_Mounted_Text = false;

bool Item_Esp_Mounted_Boxes = false;

float Item_Esp_Mounted_Color[4] = { 1.f, 1.f, 1.f, 1.f };

bool Anti_Aim_Enabled;

bool Anti_Aim_Silent = false;

__int32 Anti_Aim_Yaw_Mode;

float Anti_Aim_Yaw_Value = 15.f;

__int32 Anti_Aim_Pitch_Mode;

float Anti_Aim_Pitch_Value = 89.f;

bool Chat_Spammer_Enabled;

float Chat_Spammer_Interval = 5.f;

char Chat_Spammer_Message[128] = "Hello!";

bool World_Enabled;

bool World_Nightmode;

bool World_Sky_Color_Enabled;

float World_Sky_Color[4] = { 0.35f, 0.55f, 1.f, 1.f };

bool World_World_Color_Enabled;

float World_World_Color[4] = { 0.6f, 0.6f, 0.62f, 1.f };

bool World_Fullbright_Enabled;

bool World_Full_Flashlight_Enabled;

float World_Flashlight_Fov = 150.f;

bool World_No_Fog_Enabled;

bool World_Custom_Fog_Enabled;

bool World_Blend_Fog_Enabled;

bool World_Fog_Rainbow_Enabled;

float World_Fog_Rainbow_Speed = 5.f;

float World_Fog_Primary_Color[4] = { 0.55f, 0.65f, 0.75f, 1.f };

float World_Fog_Secondary_Color[4] = { 0.55f, 0.65f, 0.75f, 1.f };

float World_Fog_Start = 1000.f;

float World_Fog_End = 12000.f;

float World_Fog_Density = 1.f;

CreateMove_Type Original_CreateMove;

Override_View_Type Original_Override_View;

Draw_Model_Execute_Type Original_Draw_Model_Execute;

bool Chat_Spy_Enabled = false;

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

	Install_Hitmarker();

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