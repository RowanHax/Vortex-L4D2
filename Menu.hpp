#pragma once

#include <stdio.h>

#include <imgui.h>

#include "Internal.hpp"

#include "Config.hpp"

#include "ModsOnline.hpp"

#include "LagExploit.hpp"

#include "AimInfected.hpp"

#include "../Lua/LuaAPI.hpp"

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

static const struct { const char* Label; int Group; __int32 Tab; } Sidebar_Entries[11] =
{
	{ "Aimbot",         0, 0 },
	{ "Aim Infected",   0, 1 },
	{ "Anti-Aim",       0, 2 },
	{ "ESP",            1, 3 },
	{ "Chams",          1, 4 },
	{ "World",          1, 5 },
	{ "Exploits",       2, 6 },
	{ "Misc",           2, 7 },
	{ "Configs",        3, 8 },
	{ "Luas",           3, 9 },
	{ "About",          3, 10 },
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
	Sidebar_Group_Label("Aim");

	ImGui::SetCursorPosY(126.f); Sidebar_Entry(0);
	ImGui::SetCursorPosY(152.f); Sidebar_Entry(1);
	ImGui::SetCursorPosY(178.f); Sidebar_Entry(2);

	ImGui::SetCursorPos(ImVec2(0.f, 214.f));
	Sidebar_Group_Label("Visuals");

	ImGui::SetCursorPosY(232.f); Sidebar_Entry(3);
	ImGui::SetCursorPosY(258.f); Sidebar_Entry(4);
	ImGui::SetCursorPosY(284.f); Sidebar_Entry(5);

	ImGui::SetCursorPos(ImVec2(0.f, 318.f));
	Sidebar_Group_Label("Miscellaneous");

	ImGui::SetCursorPosY(336.f); Sidebar_Entry(6);
	ImGui::SetCursorPosY(362.f); Sidebar_Entry(7);

	ImGui::SetCursorPos(ImVec2(0.f, 396.f));
	Sidebar_Group_Label("System");

	ImGui::SetCursorPosY(414.f); Sidebar_Entry(8);
	ImGui::SetCursorPosY(440.f); Sidebar_Entry(9);
	ImGui::SetCursorPosY(466.f); Sidebar_Entry(10);

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

static const char* Vortex_Key_Name(__int32 Key)
{
	switch (Key)
	{
	case 0:        return "None";
	case 1:        return "Mouse 1";
	case 2:        return "Mouse 2";
	case 4:        return "Mouse 3";
	case 5:        return "Mouse 4";
	case 6:        return "Mouse 5";
	case VK_BACK:  return "Backspace";
	case VK_TAB:   return "Tab";
	case VK_RETURN:return "Enter";
	case VK_SHIFT: return "Shift";
	case VK_CONTROL:return "Ctrl";
	case VK_MENU:  return "Alt";
	case VK_CAPITAL:return "Caps Lock";
	case VK_ESCAPE:return "Esc";
	case VK_SPACE: return "Space";
	case VK_PRIOR: return "Page Up";
	case VK_NEXT:  return "Page Down";
	case VK_END:   return "End";
	case VK_HOME:  return "Home";
	case VK_LEFT:  return "Left";
	case VK_UP:    return "Up";
	case VK_RIGHT: return "Right";
	case VK_DOWN:  return "Down";
	case VK_INSERT:return "Insert";
	case VK_DELETE:return "Delete";
	}

	if ((Key >= '0') && (Key <= '9'))
	{
		static char Buffer[2];
		Buffer[0] = (char)Key;
		Buffer[1] = 0;
		return Buffer;
	}

	if ((Key >= 'A') && (Key <= 'Z'))
	{
		static char Buffer[2];
		Buffer[0] = (char)Key;
		Buffer[1] = 0;
		return Buffer;
	}

	if ((Key >= VK_F1) && (Key <= VK_F12))
	{
		static const char* Names[] = { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12" };
		return Names[Key - VK_F1];
	}

	if ((Key >= VK_NUMPAD0) && (Key <= VK_NUMPAD9))
	{
		static const char* Names[] = { "Num0", "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9" };
		return Names[Key - VK_NUMPAD0];
	}

	static char Fallback[16];
	sprintf_s(Fallback, "Key %d", Key);
	return Fallback;
}

static __int32* Vortex_Bind_Target;

static bool Vortex_Bind_Flush_Done;

static void Vortex_Arm_Bind(__int32* Key)
{
	Vortex_Bind_Target = Key;

	Vortex_Bind_Flush_Done = false;
}

static void Vortex_Update_Bind_Capture()
{
	if (Vortex_Bind_Target == nullptr)
	{
		return;
	}

	if (Menu_Open == false)
	{
		Vortex_Bind_Target = nullptr;
		return;
	}

	if (Vortex_Bind_Flush_Done == false)
	{
		for (__int32 Key = 1; Key <= 255; Key++)
		{
			GetAsyncKeyState(Key);
		}

		Vortex_Bind_Flush_Done = true;

		return;
	}

	if ((GetAsyncKeyState(VK_ESCAPE) & 1) != 0)
	{
		Vortex_Bind_Target = nullptr;
		return;
	}

	for (__int32 Key = 2; Key <= 255; Key++)
	{
		if ((Key == VK_INSERT) || (Key == VK_ESCAPE))
		{
			continue;
		}

		if ((GetAsyncKeyState(Key) & 1) != 0)
		{
			*Vortex_Bind_Target = Key;
			Vortex_Bind_Target = nullptr;
			return;
		}
	}
}

static void Key_Bind_Control(__int32* Key, const char* Zero_Text, const char* Label)
{
	const bool Listening = (Vortex_Bind_Target == Key);

	ImGui::PushID(Label);

	if (Listening == true)
	{
		ImGui::Button("...");
	}
	else
	{
		const char* Text = (*Key != 0) ? Vortex_Key_Name(*Key) : ((Zero_Text != nullptr) ? Zero_Text : "None");

		if (ImGui::Button(Text) == true)
		{
			Vortex_Arm_Bind(Key);
		}

		if (ImGui::IsItemClicked(1) == true)
		{
			*Key = 0;
		}

		if (ImGui::IsItemHovered() == true)
		{
			ImGui::SetTooltip("Left click: bind a key\nRight click: clear");
		}
	}

	ImGui::SameLine();
	ImGui::TextColored(M_Color(0xC0, 0xC0, 0xC8), "%s", (Listening == true) ? "Press a key... (Esc cancels)" : Label);

	ImGui::PopID();
}

static void Show_Tab_Vortex()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Vortex_Main", "Vortex Aimbot", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Vortex_Aimbot_Enabled);
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "Vortex aimbot, can be used for legit and rage.");
	Key_Bind_Control(&Vortex_Aimbot_Key, "Always", "Activation key");
	ImGui::BeginDisabled(Vortex_Aimbot_Enabled == false);
	ImGui::Checkbox("Silent", &Vortex_Aimbot_Silent);
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
	ImGui::Checkbox("No Spread/No Recoil", &Vortex_No_Spread_Enabled);
	End_Mini_Panel();
}

static void Show_Tab_Aim_Infected()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_AimInfected_Main", "Aim Infected", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Aim_Infected_Enabled);
	Key_Bind_Control(&Aim_Infected_Key, "Always", "Activation key");
	ImGui::BeginDisabled(Aim_Infected_Enabled == false);
	ImGui::Checkbox("Silent", &Aim_Infected_Silent);
	ImGui::Checkbox("Target head", &Aim_Infected_Target_Head);
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_AimInfected_Aim", "Aim", Cell_W, Cell_H);
	ImGui::BeginDisabled(Aim_Infected_Enabled == false);
	ImGui::SliderFloat("FOV", &Aim_Infected_Fov, 1.f, 180.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::SliderFloat("Smooth", &Aim_Infected_Smooth, 1.f, 20.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::SliderFloat("Distance", &Aim_Infected_Distance, 100.f, 8012.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

}

static void Show_Tab_AntiAim()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_AntiAim_Main", "Anti-Aim", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Anti_Aim_Enabled);
	ImGui::BeginDisabled(Anti_Aim_Enabled == false);
	ImGui::Checkbox("Silent", &Anti_Aim_Silent);
	ImGui::Text("Yaw mode");
	ImGui::SameLine();
	ImGui::Combo("##YawMode", &Anti_Aim_Yaw_Mode, "Off\0Static\0Backwards\0Fake Backwards\0Fake Sideways\0Spin\0Jitter\0", 7);
	Red_Border_On_Focus();
	ImGui::SliderFloat("Yaw amount", &Anti_Aim_Yaw_Value, 1.f, 180.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::Text("Pitch mode");
	ImGui::SameLine();
	ImGui::Combo("##PitchMode", &Anti_Aim_Pitch_Mode, "Off\0Up\0Down\0Jitter\0", 4);
	Red_Border_On_Focus();
	ImGui::SliderFloat("Pitch amount", &Anti_Aim_Pitch_Value, 1.f, 89.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_AntiAim_Info", "Info", Cell_W, Cell_H);
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "Note that its kinda buggy and might do weird things.");
	End_Mini_Panel();
}

static void Show_Tab_World()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_World_Fog", "Fog", Cell_W, Cell_H);
	ImGui::Checkbox("Enable World", &World_Enabled);
	ImGui::BeginDisabled(World_Enabled == false);
	ImGui::Checkbox("No Fog", &World_No_Fog_Enabled);
	ImGui::Checkbox("Custom Fog", &World_Custom_Fog_Enabled);
	ImGui::BeginDisabled(World_Custom_Fog_Enabled == false);
	ImGui::Checkbox("Blend Fog", &World_Blend_Fog_Enabled);
	ImGui::Checkbox("Rainbow Fog", &World_Fog_Rainbow_Enabled);
	ImGui::BeginDisabled(World_Fog_Rainbow_Enabled == false);
	ImGui::SliderFloat("Rainbow Speed", &World_Fog_Rainbow_Speed, 0.1f, 20.f, "%.1f");
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	ImGui::ColorEdit4("Primary Color", World_Fog_Primary_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::ColorEdit4("Secondary Color", World_Fog_Secondary_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::SliderFloat("Start", &World_Fog_Start, 0.f, 20000.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::SliderFloat("End", &World_Fog_End, 100.f, 40000.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::SliderFloat("Density", &World_Fog_Density, 0.f, 2.f, "%.2f");
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_World_Sky", "Sky", Cell_W, Cell_H);
	ImGui::BeginDisabled(World_Enabled == false);
	ImGui::Checkbox("Nightmode", &World_Nightmode);
	ImGui::Checkbox("Enable Sky Color", &World_Sky_Color_Enabled);
	ImGui::BeginDisabled((World_Sky_Color_Enabled == false) || (World_Nightmode == true));
	ImGui::ColorEdit4("Sky Color", World_Sky_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::EndDisabled();
	ImGui::Checkbox("Enable World Color", &World_World_Color_Enabled);
	ImGui::BeginDisabled((World_World_Color_Enabled == false) || (World_Nightmode == true));
	ImGui::ColorEdit4("World Color", World_World_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::EndDisabled();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(0.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_World_Light", "Lighting", Cell_W, Cell_H);
	ImGui::BeginDisabled(World_Enabled == false);
	ImGui::Checkbox("FullBright", &World_Fullbright_Enabled);
	ImGui::Checkbox("Full Flashlight", &World_Full_Flashlight_Enabled);
	ImGui::BeginDisabled(World_Full_Flashlight_Enabled == false);
	ImGui::SliderFloat("Flashlight FOV", &World_Flashlight_Fov, 45.f, 180.f, "%.0f");
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_World_Info", "Info", Cell_W, Cell_H);
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "This can make fps loss on shit pc.");
	End_Mini_Panel();
}

static void Show_Tab_Visuals()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap, 3.f);

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

	Panel_Pos(0.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Visuals_Items", "Items", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Item_Esp_Enabled);
	ImGui::BeginDisabled(Item_Esp_Enabled == false);
	ImGui::Checkbox("Weapon spawns text", &Item_Esp_Weapons_Text);
	ImGui::SameLine();
	ImGui::Checkbox("Boxes", &Item_Esp_Weapons_Boxes);
	ImGui::ColorEdit4("Weapon spawns", Item_Esp_Weapons_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Visuals_Heal", "Heal Items", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Item_Esp_Heal_Enabled);
	ImGui::BeginDisabled(Item_Esp_Heal_Enabled == false);
	ImGui::Checkbox("Text", &Item_Esp_Heal_Text);
	ImGui::SameLine();
	ImGui::Checkbox("Boxes", &Item_Esp_Heal_Boxes);
	ImGui::ColorEdit4("Heal items", Item_Esp_Heal_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(0.f, 2.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Visuals_Mounted", "Mounted Weapons", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Item_Esp_Mounted_Enabled);
	ImGui::BeginDisabled(Item_Esp_Mounted_Enabled == false);
	ImGui::Checkbox("Text", &Item_Esp_Mounted_Text);
	ImGui::SameLine();
	ImGui::Checkbox("Boxes", &Item_Esp_Mounted_Boxes);
	ImGui::ColorEdit4("Mounted", Item_Esp_Mounted_Color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop);
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "Shows heat value when available.");
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 2.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Visuals_Items_Info", "Info", Cell_W, Cell_H);
	End_Mini_Panel();
}

static void Show_Tab_Exploits()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap, 3.f);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Tick", "Rapid Fire", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Tick_Manipulation_Enabled);
	ImGui::BeginDisabled(Tick_Manipulation_Enabled == false);
	ImGui::SliderInt("Ticks", (int*)&Tick_Manipulation_Ticks, 1, 100);
	Red_Border_On_Focus();
	ImGui::Checkbox("Interact", &Tick_Manipulation_Interact);
	ImGui::EndDisabled();
	Key_Bind_Control(&Rapid_Fire_Bind_Key, "None", "Rapid Fire key");
	ImGui::Dummy(ImVec2(0.f, 4.f));	
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Lag", "Lag Exploit", Cell_W, Cell_H);
	ImGui::Checkbox("Enable Lag Exploit", &Lag_Exploit_Enabled);
	ImGui::BeginDisabled(Lag_Exploit_Enabled == false);
	Key_Bind_Control(&Lag_Exploit_Key, "Always", "Lag key");
	ImGui::SliderInt("Lag Value", &Lag_Exploit_Value, 10, 100000);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(0.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Tickbase", "Tickbase Fix", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Tick_Base_Fix_Enabled);
	ImGui::BeginDisabled(Tick_Base_Fix_Enabled == false);
	ImGui::SliderInt("Clock Correction", &Tick_Base_Correction_Msecs, 0, 200);
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 1.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Stuck", "Airstuck", Cell_W, Cell_H);
	ImGui::Checkbox("Enable Air Stuck", &Air_Stuck_Enabled);
	ImGui::BeginDisabled(Air_Stuck_Enabled == false);
	Key_Bind_Control(&Air_Stuck_Key, "None", "Air Stuck key");
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(0.f, 2.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_RollTP", "Teleport", Cell_W, Cell_H);
	ImGui::Checkbox("Enable Roll TP", &Roll_TP_Enabled);
	ImGui::BeginDisabled(Roll_TP_Enabled == false);
	Key_Bind_Control(&Roll_TP_Key, "None", "TP key");
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 2.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Exploits_Speed", "Speedhack", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Speed_Hack_Enabled);
	ImGui::BeginDisabled(Speed_Hack_Enabled == false);
	ImGui::SliderInt("Factor", &Speed_Hack_Factor, 1, 30);
	Red_Border_On_Focus();
	Key_Bind_Control(&Speed_Hack_Key, "Always", "Speed key");
	ImGui::EndDisabled();
	End_Mini_Panel();
}

static void Show_Tab_Misc()
{
	float Cell_W, Cell_H, Gap;
	Panel_Grid_Size(Cell_W, Cell_H, Gap, 3.f);

	Panel_Pos(0.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_Movement", "Movement", Cell_W, Cell_H);
	ImGui::Checkbox("Auto Bunny Hop", &Auto_Bhop_Enabled);
	ImGui::Checkbox("Air Strafe", &Strafe_Enabled);
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_Display", "Display", Cell_W, Cell_H);
	ImGui::Checkbox("No Boomer Vomit", &No_Vomit_Enabled);
	ImGui::Checkbox("Name Stealer", &Name_Stealer_Enabled);
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

	Panel_Pos(0.f, 2.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_ChatSpammer", "Chat Spammer", Cell_W, Cell_H);
	ImGui::Checkbox("Enable", &Chat_Spammer_Enabled);
	ImGui::BeginDisabled(Chat_Spammer_Enabled == false);
	ImGui::SliderFloat("Interval (s)", &Chat_Spammer_Interval, 1.0f, 60.f, "%.1f");
	Red_Border_On_Focus();
	ImGui::InputText("Message", Chat_Spammer_Message, sizeof(Chat_Spammer_Message));
	Red_Border_On_Focus();
	ImGui::EndDisabled();
	End_Mini_Panel();

	Panel_Pos(1.f, 2.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_Misc_Info", "Camera", Cell_W, Cell_H);
	ImGui::Checkbox("Enable Charger Turn", &Charger_Turn_Enabled);
	if (ImGui::IsItemDeactivatedAfterEdit())
		Charger_Turn_Apply();
	ImGui::Checkbox("Third Person", &Third_Person_Enabled);
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
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Beta (1.3)");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Legit, Semi Rage and Rage cheat");
	ImGui::TextColored(M_Color(0x80, 0x80, 0x88), "all features are made for Vortex");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Developed by Rowan <3");
	ImGui::TextColored(M_Color(0x60, 0x60, 0x68), "thanks to Murayefeskamus who helped me a lot with the code");
	ImGui::TextColored(M_Color(0x60, 0x60, 0x68), "also thanks to the beta testers who reported bugs");
	End_Mini_Panel();

	Panel_Pos(1.f, 0.f, Cell_W, Cell_H, Gap);
	Begin_Mini_Panel("MP_About_Features", "Features", Cell_W, Cell_H);
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Aimbot");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Aim Infected");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Anti-Aim");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "No Spread/No Recoil");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Auto Bunnyhop");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Strafe");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "ESP");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Chams");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "SpeedHack");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Rapid Fire");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Lag Exploit (needs fix)");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Airstuck");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Charger Turn");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Teleport");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Nightmode");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Fog Controller");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Skybox Color");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "World Color");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Flashlight Bright");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "No Visual Recoil");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "No Boomer Vomit");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Enemy Chat Spy");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Enable Mods Online");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Chat Spammer");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Name Stealer");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Third Person");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Config System");
	ImGui::TextColored(M_Color(0xE0, 0xE0, 0xE4), "Lua API");
	End_Mini_Panel();
}

static void Show_Tab_Lua()
{
	Render_Lua_Section();
}

static void Show_Menu()
{
	Vortex_Update_Bind_Capture();

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
	case 1:  Show_Tab_Aim_Infected(); break;
	case 2:  Show_Tab_AntiAim();      break;
	case 3:  Show_Tab_Visuals();      break;
	case 4:  Show_Tab_Chams();        break;
	case 5:  Show_Tab_World();        break;
	case 6:  Show_Tab_Exploits();     break;
	case 7:  Show_Tab_Misc();         break;
	case 8:  Show_Tab_Configs();      break;
	case 9:  Show_Tab_Lua();          break;
	default: Show_Tab_About();        break;
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::End();
	ImGui::PopStyleVar();
}