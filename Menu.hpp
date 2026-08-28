#pragma once

#include <imgui.h>

#include "Internal.hpp"

#include "Config.hpp"

#include "ModsOnline.hpp"

#include "LagExploit.hpp"

static ImVec4 M_Color(float R, float G, float B, float A = 1.f)
{
	return ImVec4(R / 255.f, G / 255.f, B / 255.f, A);
}

static ImU32 M_U32(float R, float G, float B, float A = 255.f)
{
	return IM_COL32((int)R, (int)G, (int)B, (int)A);
}

static const ImVec4 Accent_Color      = M_Color(0xE8, 0x35, 0x4A);
static const ImVec4 Accent_Hover_Color = M_Color(0xF0, 0x55, 0x65);

static void Setup_Menu_Style()
{
	ImGuiStyle& S = ImGui::GetStyle();

	ImGui::StyleColorsDark();

	S.DisabledAlpha = 0.38f;

	S.WindowRounding  = 6.f;
	S.ChildRounding   = 5.f;
	S.FrameRounding   = 3.f;
	S.PopupRounding   = 5.f;
	S.GrabRounding    = 3.f;
	S.ScrollbarRounding = 3.f;
	S.TabRounding     = 4.f;

	S.WindowBorderSize = 1.f;
	S.ChildBorderSize  = 1.f;
	S.FrameBorderSize  = 0.f;
	S.PopupBorderSize  = 1.f;

	S.WindowPadding    = ImVec2(8.f, 8.f);
	S.FramePadding     = ImVec2(6.f, 4.f);
	S.ItemSpacing      = ImVec2(8.f, 6.f);
	S.ItemInnerSpacing = ImVec2(5.f, 4.f);
	S.ScrollbarSize    = 6.f;
	S.GrabMinSize      = 8.f;

	S.Colors[ImGuiCol_WindowBg]        = M_Color(0x1A, 0x1A, 0x1F, 0.96f);
	S.Colors[ImGuiCol_ChildBg]         = M_Color(0x00, 0x00, 0x00, 0.00f);
	S.Colors[ImGuiCol_PopupBg]         = M_Color(0x18, 0x18, 0x1C, 0.98f);

	S.Colors[ImGuiCol_Border]          = M_Color(0x38, 0x38, 0x40, 0.80f);
	S.Colors[ImGuiCol_BorderShadow]    = M_Color(0x00, 0x00, 0x00, 0.00f);

	S.Colors[ImGuiCol_TitleBg]         = M_Color(0x13, 0x13, 0x16);
	S.Colors[ImGuiCol_TitleBgActive]   = M_Color(0x13, 0x13, 0x16);
	S.Colors[ImGuiCol_TitleBgCollapsed]= M_Color(0x13, 0x13, 0x16);

	S.Colors[ImGuiCol_FrameBg]         = M_Color(0x28, 0x28, 0x30);
	S.Colors[ImGuiCol_FrameBgHovered]  = M_Color(0x30, 0x30, 0x38);
	S.Colors[ImGuiCol_FrameBgActive]   = M_Color(0x38, 0x38, 0x42);

	S.Colors[ImGuiCol_CheckMark]       = M_Color(0xE8, 0x35, 0x4A);

	S.Colors[ImGuiCol_SliderGrab]      = M_Color(0xE8, 0x35, 0x4A);
	S.Colors[ImGuiCol_SliderGrabActive]= M_Color(0xF0, 0x55, 0x65);

	S.Colors[ImGuiCol_Button]          = M_Color(0x28, 0x28, 0x30);
	S.Colors[ImGuiCol_ButtonHovered]   = M_Color(0x35, 0x35, 0x3E);
	S.Colors[ImGuiCol_ButtonActive]    = M_Color(0xE8, 0x35, 0x4A, 0.80f);

	S.Colors[ImGuiCol_Header]          = M_Color(0x2A, 0x2A, 0x32);
	S.Colors[ImGuiCol_HeaderHovered]   = M_Color(0x32, 0x32, 0x3C);
	S.Colors[ImGuiCol_HeaderActive]    = M_Color(0xE8, 0x35, 0x4A, 0.35f);

	S.Colors[ImGuiCol_Separator]       = M_Color(0x38, 0x38, 0x40, 0.70f);
	S.Colors[ImGuiCol_SeparatorHovered]= M_Color(0xE8, 0x35, 0x4A, 0.80f);
	S.Colors[ImGuiCol_SeparatorActive] = M_Color(0xE8, 0x35, 0x4A);

	S.Colors[ImGuiCol_Text]            = M_Color(0xD8, 0xD8, 0xDC);
	S.Colors[ImGuiCol_TextDisabled]    = M_Color(0x58, 0x58, 0x60);
	S.Colors[ImGuiCol_TextSelectedBg]  = M_Color(0xE8, 0x35, 0x4A, 0.30f);

	S.Colors[ImGuiCol_ScrollbarBg]         = M_Color(0x10, 0x10, 0x14);
	S.Colors[ImGuiCol_ScrollbarGrab]       = M_Color(0x30, 0x30, 0x38);
	S.Colors[ImGuiCol_ScrollbarGrabHovered]= M_Color(0x40, 0x40, 0x48);
	S.Colors[ImGuiCol_ScrollbarGrabActive] = M_Color(0xE8, 0x35, 0x4A);

	S.Colors[ImGuiCol_NavHighlight]    = M_Color(0xE8, 0x35, 0x4A);

	S.Colors[ImGuiCol_Tab]             = M_Color(0x20, 0x20, 0x28);
	S.Colors[ImGuiCol_TabHovered]      = M_Color(0x2C, 0x2C, 0x35);
	S.Colors[ImGuiCol_TabSelected]     = M_Color(0x30, 0x30, 0x3A);
	S.Colors[ImGuiCol_TabDimmed]       = M_Color(0x18, 0x18, 0x1E);
	S.Colors[ImGuiCol_TabDimmedSelected] = M_Color(0x22, 0x22, 0x2A);
}

static __int32 Current_Tab = 0;

static const struct { const char* Label; int Group; __int32 Tab; } Sidebar_Entries[7] =
{
	{ "Aimbot",         0, 0 },
	{ "ESP",            1, 1 },
	{ "Chams",          1, 2 },
	{ "Exploits",       2, 3 },
	{ "Misc",           2, 4 },
	{ "Configs",        3, 5 },
	{ "About",          3, 6 },
};

static void Sidebar_Entry(__int32 Index)
{
	ImGui::SetCursorPosX(0.f);

	ImGui::InvisibleButton(Sidebar_Entries[Index].Label, ImVec2(ImGui::GetContentRegionAvail().x, 26.f));

	if (ImGui::IsItemClicked())
		Current_Tab = Sidebar_Entries[Index].Tab;

	ImDrawList* DL  = ImGui::GetWindowDrawList();
	const ImVec2 Min = ImGui::GetItemRectMin();
	const ImVec2 Max = ImGui::GetItemRectMax();
	const bool   Active = (Current_Tab == Sidebar_Entries[Index].Tab);

	if (Active)
	{
		DL->AddRectFilled(Min, Max, M_U32(0xE8, 0x35, 0x4A, 22), 4.f);
		DL->AddRectFilled(ImVec2(Min.x, Min.y + 3.f), ImVec2(Min.x + 3.f, Max.y - 3.f), M_U32(0xE8, 0x35, 0x4A), 2.f);
	}
	else if (ImGui::IsItemHovered())
	{
		DL->AddRectFilled(Min, Max, M_U32(0xFF, 0xFF, 0xFF, 10), 4.f);
	}

	const float TY = Min.y + (Max.y - Min.y) / 2.f - ImGui::GetFontSize() / 2.f;
	const ImU32 Col = Active ? M_U32(0xF0, 0xF0, 0xF0) : M_U32(0x88, 0x88, 0x90);

	DL->AddText(ImVec2(Min.x + 12.f, TY), Col, Sidebar_Entries[Index].Label);
}

static void Sidebar_Group_Label(const char* Label)
{
	ImGui::SetCursorPosX(12.f);
	ImGui::TextColored(Accent_Color, "%s", Label);
}

static void Show_Sidebar()
{
	ImGui::PushStyleColor(ImGuiCol_ChildBg, M_Color(0x13, 0x13, 0x16, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	ImGui::BeginChild("Sidebar", ImVec2(160.f, 0.f), false, ImGuiWindowFlags_NoScrollbar);

	ImDrawList* DL = ImGui::GetWindowDrawList();
	const ImVec2 WP = ImGui::GetWindowPos();

	const ImVec2 Center(WP.x + 80.f, WP.y + 56.f);
	DL->AddCircleFilled(Center, 40.f, M_U32(0x1E, 0x1E, 0x24), 64);
	DL->AddCircle(Center, 40.f, M_U32(0xE8, 0x35, 0x4A), 64, 1.5f);
	DL->AddLine(ImVec2(Center.x - 28.f, Center.y), ImVec2(Center.x - 6.f, Center.y),  M_U32(0xE8, 0x35, 0x4A), 2.f);
	DL->AddLine(ImVec2(Center.x + 6.f,  Center.y), ImVec2(Center.x + 28.f, Center.y), M_U32(0xE8, 0x35, 0x4A), 2.f);
	DL->AddLine(ImVec2(Center.x, Center.y - 28.f), ImVec2(Center.x, Center.y - 6.f),  M_U32(0xE8, 0x35, 0x4A), 2.f);
	DL->AddLine(ImVec2(Center.x, Center.y + 6.f),  ImVec2(Center.x, Center.y + 28.f), M_U32(0xE8, 0x35, 0x4A), 2.f);
	DL->AddCircleFilled(Center, 4.f, M_U32(0xE8, 0xE8, 0xE8), 16);

	ImGui::SetCursorPos(ImVec2(0.f, 108.f));
	Sidebar_Group_Label("Aimbot");

	ImGui::SetCursorPosY(128.f); Sidebar_Entry(0);

	ImGui::SetCursorPos(ImVec2(0.f, 168.f));
	Sidebar_Group_Label("Visuals");

	ImGui::SetCursorPosY(188.f); Sidebar_Entry(1);
	ImGui::SetCursorPosY(214.f); Sidebar_Entry(2);

	ImGui::SetCursorPos(ImVec2(0.f, 258.f));
	Sidebar_Group_Label("Miscellaneous");

	ImGui::SetCursorPosY(278.f); Sidebar_Entry(3);
	ImGui::SetCursorPosY(304.f); Sidebar_Entry(4);

	ImGui::SetCursorPos(ImVec2(0.f, 348.f));
	Sidebar_Group_Label("System");

	ImGui::SetCursorPosY(368.f); Sidebar_Entry(5);
	ImGui::SetCursorPosY(394.f); Sidebar_Entry(6);

	{
		const float Strip_Y = ImGui::GetWindowHeight() - 42.f;
		const ImVec2 StripMin(WP.x, WP.y + Strip_Y);
		const ImVec2 StripMax(WP.x + 160.f, WP.y + ImGui::GetWindowHeight());

		DL->AddRectFilled(StripMin, StripMax, M_U32(0x20, 0x20, 0x26));
		DL->AddLine(ImVec2(StripMin.x, StripMin.y), ImVec2(StripMax.x, StripMin.y), M_U32(0xE8, 0x35, 0x4A, 120), 1.f);

		const ImVec2 AV(WP.x + 20.f, WP.y + Strip_Y + 21.f);
		DL->AddCircleFilled(AV, 12.f, M_U32(0x2A, 0x2A, 0x32), 32);
		DL->AddCircle(AV, 12.f, M_U32(0xE8, 0x35, 0x4A), 32, 1.f);

		const float TY = WP.y + Strip_Y + 12.f;
		DL->AddText(ImVec2(WP.x + 38.f, TY),       M_U32(0xE8, 0x35, 0x4A), "Vortex");
		DL->AddText(ImVec2(WP.x + 38.f, TY + 14.f), M_U32(0x66, 0x66, 0x70), "Welcome");
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

static void Panel_Grid_Size(float& Cell_W, float& Cell_H, float& Gap, float Rows = 2.f)
{
	Gap = 8.f;
	const ImVec2 Avail = ImGui::GetContentRegionAvail();
	Cell_W = (Avail.x - Gap) / 2.f;
	Cell_H = (Avail.y - (Rows - 1.f) * Gap) / Rows;
}

static void Panel_Pos(float Col, float Row, float Cell_W, float Cell_H, float Gap)
{
	ImGui::SetCursorPos(ImVec2(Col * (Cell_W + Gap), Row * (Cell_H + Gap)));
}

static void Begin_Mini_Panel(const char* Id, const char* Title, float Cell_W, float Cell_H)
{
	ImGui::PushStyleColor(ImGuiCol_ChildBg, M_Color(0x20, 0x20, 0x28, 0.98f));
	ImGui::PushStyleColor(ImGuiCol_Border,  M_Color(0x38, 0x38, 0x45, 0.90f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 8.f));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.f);

	ImGui::BeginChild(Id, ImVec2(Cell_W, Cell_H), true);

	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "%s", Title);

	ImDrawList* DL = ImGui::GetWindowDrawList();
	const ImVec2 P  = ImGui::GetWindowPos();
	const float  W  = ImGui::GetWindowWidth();
	const float  SY = P.y + ImGui::GetCursorPosY() + 2.f;
	DL->AddLine(ImVec2(P.x + 8.f, SY), ImVec2(P.x + W - 8.f, SY), M_U32(0x38, 0x38, 0x45), 1.f);

	ImGui::Dummy(ImVec2(0.f, 4.f));
}

static void End_Mini_Panel()
{
	ImGui::EndChild();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);
}

static void Section_Label(const char* Label)
{
	ImGui::Dummy(ImVec2(0.f, 2.f));
	ImGui::TextColored(Accent_Color, "%s", Label);
	ImGui::Dummy(ImVec2(0.f, 1.f));
}

static void Red_Border_On_Focus()
{
	if (ImGui::IsItemActive())
	{
		ImGui::GetWindowDrawList()->AddRect(
			ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
			M_U32(0xE8, 0x35, 0x4A), 3.f, 0, 1.5f);
	}
}

static void Key_Badge(const char* Key, const char* Label)
{
	ImDrawList* DL = ImGui::GetWindowDrawList();
	const ImVec2 CP = ImGui::GetCursorScreenPos();
	const float  FS = ImGui::GetFontSize();
	const float  BW = FS + 6.f;
	const float  BH = FS + 2.f;

	DL->AddRectFilled(CP, ImVec2(CP.x + BW, CP.y + BH), M_U32(0x10, 0x10, 0x14), 3.f);
	DL->AddRect(CP,       ImVec2(CP.x + BW, CP.y + BH), M_U32(0x48, 0x48, 0x50), 3.f, 0, 1.f);

	const ImVec2 TS = ImGui::CalcTextSize(Key);
	DL->AddText(ImVec2(CP.x + (BW - TS.x) / 2.f, CP.y + (BH - TS.y) / 2.f),
	            M_U32(0xE0, 0xE0, 0xE4), Key);

	ImGui::Dummy(ImVec2(BW + 6.f, BH));
	ImGui::SameLine(0.f, 0.f);
	ImGui::TextColored(M_Color(0xC0, 0xC0, 0xC8), "%s", Label);
}

static void Show_Tab_Vortex()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Vortex_Main", "Vortex Aimbot", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Vortex_Aimbot_Enabled);
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "Vortex aimbot, can be used for legit and rage.");
	ImGui::BeginDisabled(Vortex_Aimbot_Enabled == false);
	ImGui::Checkbox("Silent", &Vortex_Aimbot_Silent);
	ImGui::Checkbox("Visible check", &Vortex_Aimbot_Visible);
	ImGui::Checkbox("Auto fire", &Vortex_Aimbot_Auto_Fire);
	ImGui::Checkbox("Prediction", &Vortex_Aimbot_Prediction);
	ImGui::SameLine();
	ImGui::TextColored(M_Color(0xFF, 0x50, 0x5A), "need fix");
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Vortex_Aim", "Aim", Cell_W, Cell_H);
	ImGui::BeginDisabled(Vortex_Aimbot_Enabled == false);
	ImGui::SliderFloat("FOV", &Vortex_Aimbot_Fov, 1.f, 180.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::SliderFloat("Distance (m)", &Vortex_Aimbot_Distance, 5.f, 100.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::SliderFloat("Smooth", &Vortex_Aimbot_Smooth, 1.f, 20.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::Combo("Hitbox", &Vortex_Aimbot_Hitbox, "Head\0Chest\0", 2);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(0.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Vortex_Targets", "Targets", Cell_W, Cell_H);
	ImGui::BeginDisabled(Vortex_Aimbot_Enabled == false);
	ImGui::Checkbox("Ignore Witch", &Vortex_Aimbot_Ignore_Witch);
	ImGui::Checkbox("Ignore Tank", &Vortex_Aimbot_Ignore_Tank);
	ImGui::Checkbox("Ignore common", &Vortex_Aimbot_Ignore_Common);
	ImGui::Checkbox("Prioritize players", &Vortex_Aimbot_Prioritize_Players);
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Vortex_Accuracy", "Accuracy", Cell_W, Cell_H);
	ImGui::Checkbox("No Spread", &Vortex_No_Spread_Enabled);
	End_Mini_Panel();
}

static void Show_Tab_Visuals()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Visuals_Esp", "ESP", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Esp_Enabled);
	ImGui::BeginDisabled(Esp_Enabled == false);
	ImGui::Checkbox("Survivors", &Esp_Team2);
	ImGui::Checkbox("Infected", &Esp_Team3);
	ImGui::Checkbox("Commons", &Esp_Commons);
	ImGui::Checkbox("Names", &Esp_Names);
	ImGui::SameLine();
	ImGui::Checkbox("Distance", &Esp_Distance);
	ImGui::Checkbox("Health bar", &Esp_Health_Bar);
	ImGui::SameLine();
	ImGui::Checkbox("Snap lines", &Esp_Snap_Lines);
	ImGui::Combo("Box style", &Esp_Box_Style, "Box\0Corners\0", 2);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Visuals_Colors", "Colors", Cell_W, Cell_H);
	ImGui::ColorEdit4("Survivors", Esp_Team2_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::ColorEdit4("Infected",  Esp_Team3_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	End_Mini_Panel();
}

static void Show_Tab_Exploits()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Tick", "Rapid Fire", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Tick_Manipulation_Enabled);
	ImGui::BeginDisabled(Tick_Manipulation_Enabled == false);
	ImGui::SliderInt("Ticks", (int*)&Tick_Manipulation_Ticks, 1, 30);
	Red_Border_On_Focus();
	ImGui::Checkbox("Interact", &Tick_Manipulation_Interact);
	ImGui::EndDisabled();
	ImGui::Dummy(ImVec2(0.f, 4.f));	
	End_Mini_Panel();

	Panel_Pos(0.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Lag", "Lag Switch", Cell_W, Cell_H);
	ImGui::Checkbox("Enable Lag Switch", &Lag_Exploit_Enabled);
	ImGui::BeginDisabled(Lag_Exploit_Enabled == false);
	ImGui::SliderInt("Lag Key", &Lag_Exploit_Key, 0, 255);
	Red_Border_On_Focus();
	ImGui::SliderInt("Lag Value", &Lag_Exploit_Value, 10, 100000);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Tickbase", "Tickbase Fix", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Tick_Base_Fix_Enabled);
	ImGui::BeginDisabled(Tick_Base_Fix_Enabled == false);
	ImGui::SliderInt("Clock Correction", &Tick_Base_Correction_Msecs, 0, 200);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Stuck", "Stuck", Cell_W, Cell_H);
	ImGui::Checkbox("Enable Air Stuck", &Air_Stuck_Enabled);
	ImGui::BeginDisabled(Air_Stuck_Enabled == false);
	ImGui::SliderInt("Air Stuck Key", &Air_Stuck_Key, 0, 255);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	ImGui::Dummy(ImVec2(0.f, 4.f));
	ImGui::Checkbox("Enable Charger Turn", &Charger_Turn_Enabled);
	if (ImGui::IsItemDeactivatedAfterEdit())
		Charger_Turn_Apply();
	End_Mini_Panel();
}

static void Show_Tab_Misc()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_Movement", "Movement", Cell_W, Cell_H);
	ImGui::Checkbox("Auto Bunny Hop", &Auto_Bhop_Enabled);
	ImGui::Checkbox("Air Strafe", &Strafe_Enabled);
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_Display", "Display", Cell_W, Cell_H);
	ImGui::Checkbox("No Boomer Vomit", &No_Vomit_Enabled);
	Section_Label("Chat Spy");
	ImGui::Checkbox("Enemy Chat Spy", &Chat_Spy_Enabled);
	ImGui::BeginDisabled(Chat_Spy_Enabled == false);
	ImGui::Combo("Method", &Chat_Spy_Method, "Overlay Feed\0HUD Chat\0", 2);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(0.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_Mods", "Mods", Cell_W, Cell_H);
	ImGui::Checkbox("Enable Mods Online", &Mods_Online_Enabled);
	Mods_Online_Apply();
	End_Mini_Panel();

	Panel_Pos(1.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_Weapons", "Weapons", Cell_W, Cell_H);
	ImGui::Checkbox("Auto Pistol", &Rapid_Fire_Enabled);
	ImGui::Checkbox("No Visual Recoil", &No_Visual_Recoil_Enabled);
	End_Mini_Panel();
}

static void Show_Tab_Chams()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Chams_Main", "Chams", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Chams_Enabled);
	ImGui::BeginDisabled(Chams_Enabled == false);
	ImGui::Checkbox("X-Ray", &Chams_X_Ray);
	ImGui::Combo("Style", &Chams_Style, "Flat\0Translucent\0Glow\0Wireframe\0XRay\0Mirror\0Chrome\0Laser\0");
	Red_Border_On_Focus();
	ImGui::ColorEdit4("Survivors", Chams_Team2_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::ColorEdit4("Infected",  Chams_Team3_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Chams_Info", "Info", Cell_W, Cell_H);
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "Sometimes chams will disappear");
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "The mirror chams are a little bit bugged but still goated");
	End_Mini_Panel();
}

static void Show_Tab_Configs()
{
	static char Config_Name[64] = { 0 };
	static char Config_List[16][64];
	static __int32 Config_Count    = 0;
	static __int32 Selected_Config = -1;

	List_Configs(Config_List, 16, &Config_Count);
	if (Selected_Config >= Config_Count) Selected_Config = -1;

	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap, 1.f);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Configs_SaveLoad", "Save / Load", Cell_W, Cell_H);
	ImGui::InputText("Name", Config_Name, sizeof(Config_Name));
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "Configs are stored in C:\\Vortex.");
	ImGui::BeginDisabled(Config_Name[0] == 0);
	if (ImGui::Button("Save", ImVec2(110.f, 0.f)))
	{
		if (Save_Config(Config_Name) == true) Selected_Config = -1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(110.f, 0.f))) Load_Config(Config_Name);
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Configs_List", "Configs", Cell_W, Cell_H);
	if (Config_Count == 0) ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "No config yet - type a name and save.");
	for (__int32 i = 0; i < Config_Count; i++)
	{
		ImGui::Selectable(Config_List[i], (Selected_Config == i));
		if (ImGui::IsItemClicked()) Selected_Config = i;
	}
	ImGui::BeginDisabled(Selected_Config < 0);
	if (ImGui::Button("Load selected", ImVec2(110.f, 0.f))) Load_Config(Config_List[Selected_Config]);
	ImGui::SameLine();
	if (ImGui::Button("Delete", ImVec2(110.f, 0.f))) { Delete_Config(Config_List[Selected_Config]); Selected_Config = -1; }
	ImGui::EndDisabled();
	End_Mini_Panel();
}

static void Show_Tab_About()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap, 1.f);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_About_Main", "Vortex", Cell_W, Cell_H);
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "First beta (1.0) of Vortex");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Legit and Semi Rage cheat");
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "all features are made for Vortex");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Developed by Rowan <3");
	ImGui::TextColored(M_Color(0x60, 0x60, 0x68), "thanks to those who helped me with the code");
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_About_Controls", "Controls", Cell_W, Cell_H);
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "M1 for Aimbot");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "F for Rapid Fire");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Insert for open menu");
	End_Mini_Panel();
}

static void Show_Menu()
{
	ImGui::SetNextWindowPos(ImVec2(59.f, 60.f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(840.f, 540.f), ImGuiCond_Once);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	ImGui::Begin("Vortex", nullptr,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar);

	Show_Sidebar();

	ImGui::SameLine(0.f, 0.f);

	ImDrawList* WDL = ImGui::GetWindowDrawList();
	WDL->AddLine(
		ImVec2(ImGui::GetWindowPos().x + 160.f, ImGui::GetWindowPos().y),
		ImVec2(ImGui::GetWindowPos().x + 160.f, ImGui::GetWindowPos().y + ImGui::GetWindowHeight()),
		M_U32(0xE8, 0x35, 0x4A, 180),
		1.f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 10.f));
	ImGui::BeginChild("Content", ImVec2(0.f, 0.f), false);

	switch (Current_Tab)
	{
	case 0:  Show_Tab_Vortex();       break;
	case 1:  Show_Tab_Visuals();      break;
	case 2:  Show_Tab_Chams();        break;
	case 3:  Show_Tab_Exploits();     break;
	case 4:  Show_Tab_Misc();         break;
	case 5:  Show_Tab_Configs();      break;
	default: Show_Tab_About();        break;
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::End();
	ImGui::PopStyleVar();
}