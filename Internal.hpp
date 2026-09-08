#pragma once

#include <windows.h>

struct UserCmd_Structure
{
	void* Vtable;
	__int32 Command_Number;
	__int32 Tick_Count;
	float View_Angles[3];
	float Forward_Move;
	float Side_Move;
	float Up_Move;
	__int32 Buttons;
	unsigned __int8 Impulse;
	__int32 Weapon_Select;
	__int32 Weapon_Subtype;
	__int32 Random_Seed;
	short Mouse_DX;
	short Mouse_DY;
	bool Has_Been_Predicted;
};

const unsigned __int32 Invalid_Handle = 0xFFFFFFFF;

struct Global_Variables_Structure
{
	__int8 Additional_Bytes_1[12];
	float Time;
	float Frame_Time;
	__int8 Additional_Bytes_2[8];
	float Interval_Per_Tick;
};

extern bool Chat_Spy_Enabled;

extern __int32 Chat_Spy_Method;

struct Chat_Spy_Line_Structure
{
	char Text[256];

	unsigned __int32 Time;

	unsigned __int32 Color;
};

extern Chat_Spy_Line_Structure Chat_Spy_Lines[8];

extern unsigned __int32 Chat_Spy_Line_Index;

inline bool Rage_Weapon_Auto_Pistol(__int32 Weapon_Type)
{
	switch (Weapon_Type)
	{
	case 1:

	case 3:

	case 4:

	case 6:

	case 8:

	case 10:

	case 0x20:

	case 0x23:

	case 0x24:

		return true;
	}

	return false;
}

struct ImFont;

extern ImFont* Font_Title;

extern HMODULE Client_Module;

extern HMODULE Engine_Module;

extern bool Auto_Bhop_Enabled;

extern bool Strafe_Enabled;

extern bool Esp_Enabled;

extern bool Esp_Team2;

extern bool Esp_Team3;

extern bool Esp_Commons;

extern bool Esp_Names;

extern bool Esp_Distance;

extern bool Esp_Health_Bar;extern bool Esp_Snap_Lines;
extern bool Hitmarker_Enabled;

extern __int32 Esp_Box_Style;

extern float Esp_Team2_Color[4];

extern float Esp_Team3_Color[4];

extern bool No_Vomit_Enabled;

extern bool Chams_Enabled;

extern bool Chams_X_Ray;

extern int Chams_Style;

extern float Chams_Team2_Color[4];

extern float Chams_Team3_Color[4];

extern void* Model_Render;extern bool Rapid_Fire_Enabled;

extern bool Lag_Exploit_Enabled;
extern __int32 Lag_Exploit_Key;
extern __int32 Lag_Exploit_Value;
extern bool Air_Stuck_Enabled;
extern __int32 Air_Stuck_Key;
extern bool Roll_TP_Enabled;
extern __int32 Roll_TP_Key;
extern bool Charger_Turn_Enabled;
extern unsigned __int8  Charger_Turn_Original_Byte;
extern bool            Charger_Turn_Patched;
extern unsigned __int32 Charger_Turn_Patch_Address;
extern bool Safe_Room_TP_Enabled;
extern __int32 Safe_Room_TP_Key;
extern bool Tick_Base_Fix_Enabled;

extern __int32 Tick_Base_Correction_Msecs;

extern bool Tick_Manipulation_Enabled;

extern bool Tick_Manipulation_Interact;

extern unsigned __int32 Tick_Manipulation_Ticks;

extern __int32 Rapid_Fire_Bind_Key;

extern __int32 Extra_Commands;

extern void* Entity_List;

extern void* (__fastcall* Get_Client_Entity)(void* Ecx, void* Edx, __int32 Index);extern __int32 (__fastcall* Get_Max_Entities)(void* Ecx, void* Edx);

extern bool No_Visual_Recoil_Enabled;
extern bool Scope_Bypass_Enabled;
extern bool Third_Person_Enabled;
extern bool Name_Stealer_Enabled;

extern bool Vortex_Aimbot_Enabled;
extern __int32 Vortex_Aimbot_Key;
extern __int32 Vortex_Aimbot_Hitbox;
extern float Vortex_Aimbot_Fov;
extern float Vortex_Aimbot_Smooth;
extern float Vortex_Aimbot_Distance;
extern bool Vortex_Aimbot_Silent;
extern bool Vortex_Aimbot_Auto_Fire;
extern bool Vortex_Aimbot_Prediction;
extern bool Vortex_Aimbot_Prioritize_Players;
extern bool Vortex_Aimbot_Ignore_Common;
extern bool Vortex_Aimbot_Ignore_Tank;
extern bool Vortex_Aimbot_Ignore_Witch;

extern bool Vortex_No_Spread_Enabled;

extern bool Mods_Online_Enabled;

extern bool Item_Esp_Enabled;

extern bool Item_Esp_Weapons_Text;

extern bool Item_Esp_Weapons_Boxes;

extern float Item_Esp_Weapons_Color[4];

extern bool Item_Esp_Heal_Enabled;

extern bool Item_Esp_Heal_Text;

extern bool Item_Esp_Heal_Boxes;

extern float Item_Esp_Heal_Color[4];

extern bool Item_Esp_Mounted_Enabled;

extern bool Item_Esp_Mounted_Text;

extern bool Item_Esp_Mounted_Boxes;

extern float Item_Esp_Mounted_Color[4];

extern bool Anti_Aim_Enabled;

extern bool Anti_Aim_Silent;

extern __int32 Anti_Aim_Yaw_Mode;

extern float Anti_Aim_Yaw_Value;

extern __int32 Anti_Aim_Pitch_Mode;

extern float Anti_Aim_Pitch_Value;

extern bool Chat_Spammer_Enabled;

extern float Chat_Spammer_Interval;

extern char Chat_Spammer_Message[128];

extern bool World_Enabled;

extern bool World_Nightmode;

extern bool World_Sky_Color_Enabled;

extern float World_Sky_Color[4];

extern bool World_World_Color_Enabled;

extern float World_World_Color[4];

extern bool World_Fullbright_Enabled;

extern bool World_Full_Flashlight_Enabled;

extern float World_Flashlight_Fov;

extern bool World_No_Fog_Enabled;

extern bool World_Custom_Fog_Enabled;

extern bool World_Blend_Fog_Enabled;

extern bool World_Fog_Rainbow_Enabled;

extern float World_Fog_Rainbow_Speed;

extern float World_Fog_Primary_Color[4];

extern float World_Fog_Secondary_Color[4];

extern float World_Fog_Start;

extern float World_Fog_End;

extern float World_Fog_Density;

const unsigned __int32 Local_Player_Offset = 7498712;

const unsigned __int32 Flags_Offset = 0xF0;

const unsigned __int32 FL_ONGROUND = 1;

static unsigned __int32 Get_Local_Player()
{
	return *(unsigned __int32*)((unsigned __int32)Client_Module + Local_Player_Offset);
}