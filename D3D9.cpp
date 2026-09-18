#include <windows.h>

#include <intrin.h>

#include <d3d9.h>

#include <imgui.h>

#include <imgui_impl_dx9.h>

#include <imgui_impl_win32.h>

#include "Menu.hpp"

#include "Esp.hpp"

#include "ItemEsp.hpp"

#include "Hitmarker.hpp"

#include "ChatSpy.hpp"

#include "World.hpp"

#include "ThirdPerson.hpp"

#include "NameStealer.hpp"

#include "D3D9.hpp"

#include "../Lua/LuaHost.hpp"

typedef long(__stdcall* EndScene_Type)(IDirect3DDevice9* Device);

typedef long(__stdcall* Reset_Type)(IDirect3DDevice9* Device, D3DPRESENT_PARAMETERS* Parameters);

static EndScene_Type Original_EndScene;

static Reset_Type Original_Reset;

static WNDPROC Original_WndProc;

static HWND Game_Window;

bool Menu_Open;

ImFont* Font_Title = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND Window, UINT Message, WPARAM W_Parameter, LPARAM L_Parameter);

static unsigned __int8 Hex_Value(const char Character)
{
	if ((Character >= '0') && (Character <= '9'))
	{
		return (unsigned __int8)(Character - '0');
	}

	if ((Character >= 'A') && (Character <= 'F'))
	{
		return (unsigned __int8)(Character - 'A' + 10);
	}

	if ((Character >= 'a') && (Character <= 'f'))
	{
		return (unsigned __int8)(Character - 'a' + 10);
	}

	return 0;
}

void* Find_Pattern(const wchar_t* Module_Name, const char* Pattern_String)
{
	const HMODULE Module = GetModuleHandleW(Module_Name);

	if (Module == nullptr)
	{
		return nullptr;
	}

	unsigned __int32 Pattern[256];

	unsigned __int8 Mask[256];

	unsigned __int32 Pattern_Length = 0;

	const char* Cursor = Pattern_String;

	while ((*Cursor != 0) && (Pattern_Length < 256))
	{
		if (*Cursor == ' ')
		{
			Cursor++;

			continue;
		}

		if (*Cursor == '?')
		{
			Pattern[Pattern_Length] = 0;

			Mask[Pattern_Length] = 0;

			Cursor++;
		}
		else
		{
			Pattern[Pattern_Length] = Hex_Value(*Cursor) * 16 + Hex_Value(*(Cursor + 1));

			Mask[Pattern_Length] = 255;

			Cursor += 2;
		}

		Pattern_Length++;
	}

	const PIMAGE_DOS_HEADER Dos_Header = (PIMAGE_DOS_HEADER)Module;

	const PIMAGE_NT_HEADERS Nt_Headers = (PIMAGE_NT_HEADERS)((unsigned __int32)Module + Dos_Header->e_lfanew);

	const unsigned __int32 Module_Start = (unsigned __int32)Module;

	const unsigned __int32 Module_End = Module_Start + Nt_Headers->OptionalHeader.SizeOfImage;

	for (unsigned __int32 Address = Module_Start; Address < Module_End - Pattern_Length; Address++)
	{
		bool Match = true;

		for (unsigned __int32 i = 0; i < Pattern_Length; i++)
		{
			if ((Mask[i] != 0) && (*(unsigned __int8*)(Address + i) != Pattern[i]))
			{
				Match = false;

				break;
			}
		}

		if (Match == true)
		{
			return (void*)Address;
		}
	}

	return nullptr;
}

static LRESULT CALLBACK WndProc_Hook(HWND Window, UINT Message, WPARAM W_Parameter, LPARAM L_Parameter)
{
	if (Menu_Open == true)
	{
		ImGui_ImplWin32_WndProcHandler(Window, Message, W_Parameter, L_Parameter);

		return 1;
	}

	return CallWindowProcW(Original_WndProc, Window, Message, W_Parameter, L_Parameter);
}

static long __stdcall Reset_Hook(IDirect3DDevice9* Device, D3DPRESENT_PARAMETERS* Parameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const long Result = Original_Reset(Device, Parameters);

	ImGui_ImplDX9_CreateDeviceObjects();

	return Result;
}

typedef void(__fastcall* Lock_Cursor_Type)(void* Ecx, void* Edx);

typedef void(__fastcall* Unlock_Cursor_Type)(void* Ecx, void* Edx);

static Lock_Cursor_Type Original_Lock_Cursor;

static Unlock_Cursor_Type Surface_Unlock_Cursor;

static void __fastcall Lock_Cursor_Hook(void* Ecx, void* Edx)
{
	if (Menu_Open == true)
	{
		if (Surface_Unlock_Cursor != nullptr)
		{
			Surface_Unlock_Cursor(Ecx, 0);
		}

		return;
	}

	Original_Lock_Cursor(Ecx, 0);
}

static void Install_Surface_Cursor_Hook()
{
	void* Surface = Find_Interface(GetModuleHandleW(L"vguimatsurface.dll"), "VGUI_Surface031");

	if (Surface == nullptr)
	{
		Surface = Find_Interface(GetModuleHandleW(L"vguimatsurface.dll"), "VGUI_Surface");
	}

	if (Surface == nullptr)
	{
		Surface = Find_Interface(GetModuleHandleW(L"vgui2.dll"), "VGUI_Surface031");
	}

	if (Surface == nullptr)
	{
		return;
	}

	void** Vtable = *(void***)Surface;

	Original_Lock_Cursor = (Lock_Cursor_Type)Vtable[59];

	Surface_Unlock_Cursor = (Unlock_Cursor_Type)Vtable[58];

	DWORD Previous_Access_Rights;

	VirtualProtect(&Vtable[59], sizeof(void*), PAGE_READWRITE, &Previous_Access_Rights);

	Vtable[59] = (void*)Lock_Cursor_Hook;

	VirtualProtect(&Vtable[59], sizeof(void*), Previous_Access_Rights, &Previous_Access_Rights);
}

static long __stdcall EndScene_Hook(IDirect3DDevice9* Device)
{
	static void* First_Address = _ReturnAddress();

	if (First_Address != _ReturnAddress())
	{
		return Original_EndScene(Device);
	}

	static bool Initialized;

	static bool ImGui_Ready;

	if (Initialized == false)
	{
		Game_Window = FindWindowW(L"Valve001", nullptr);

		if (Game_Window == nullptr)
		{
			return Original_EndScene(Device);
		}

		ImGui::CreateContext();

		ImGuiIO& IO = ImGui::GetIO();

		IO.IniFilename = nullptr;

		IO.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NavEnableKeyboard;

		if (GetFileAttributesA("C:\\Windows\\Fonts\\segoeui.ttf") != INVALID_FILE_ATTRIBUTES)
		{
			ImFontGlyphRangesBuilder Builder;

			Builder.AddRanges(IO.Fonts->GetGlyphRangesDefault());

			Builder.AddRanges(IO.Fonts->GetGlyphRangesGreek());

			Builder.AddRanges(IO.Fonts->GetGlyphRangesCyrillic());

			Builder.AddRanges(IO.Fonts->GetGlyphRangesVietnamese());

			Builder.AddChar(0x25CE);
			Builder.AddChar(0x25C9);
			Builder.AddChar(0x25B8);
			Builder.AddChar(0x2630);
			Builder.AddChar(0x25A6);
			Builder.AddChar(0x25C8);
			Builder.AddChar(0x24D8);

			ImVector<ImWchar> Ranges;

			Builder.BuildRanges(&Ranges);

			IO.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 14.f, nullptr, Ranges.Data);

			if (GetFileAttributesA("C:\\Windows\\Fonts\\seguisb.ttf") != INVALID_FILE_ATTRIBUTES)
			{
				Font_Title = IO.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguisb.ttf", 16.f, nullptr, Ranges.Data);
			}
		}

		ImGui_ImplWin32_Init(Game_Window);

		ImGui_ImplDX9_Init(Device);

		Original_WndProc = (WNDPROC)SetWindowLongPtrW(Game_Window, GWLP_WNDPROC, (LONG_PTR)WndProc_Hook);

		Setup_Menu_Style();

		ImGui_Ready = true;

		Initialized = true;
	}

	if ((GetAsyncKeyState(VK_INSERT) & 1) != 0)
	{
		Menu_Open = !Menu_Open;
	}

	ImGui_ImplDX9_NewFrame();

	ImGui_ImplWin32_NewFrame();

	if (Menu_Open == true)
	{
		POINT Mouse_Position;

		GetCursorPos(&Mouse_Position);

		ScreenToClient(Game_Window, &Mouse_Position);

		ImGuiIO& IO = ImGui::GetIO();

		IO.AddMousePosEvent((float)Mouse_Position.x, (float)Mouse_Position.y);

		IO.AddMouseButtonEvent(0, (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);

		IO.AddMouseButtonEvent(1, (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);

	}

	ImGui::NewFrame();

	if (Esp_Enabled == true)
	{
		Update_Esp();
	}

	Update_Item_Esp();

	Update_Chat_Spy_Feed();

	World_Update_Cvars();

	World_Update_Materials();

	Third_Person_Update();

	Name_Stealer_Update();

	if (No_Visual_Recoil_Enabled == true)
	{
		static __int32 No_Visual_Recoil_Punch_Offset = -1;

		static __int32 No_Visual_Recoil_Punch_Vel_Offset = -1;

		if (No_Visual_Recoil_Punch_Offset == -1)
		{
			No_Visual_Recoil_Punch_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecPunchAngle");

			No_Visual_Recoil_Punch_Vel_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_vecPunchAngleVel");
		}

		const unsigned __int32 Local_Player = Get_Local_Player();

		if ((No_Visual_Recoil_Punch_Offset >= 0) && (Local_Player != 0))
		{
			float* Punch = (float*)(Local_Player + No_Visual_Recoil_Punch_Offset);

			Punch[0] = 0.f;

			Punch[1] = 0.f;

			Punch[2] = 0.f;
		}

		if ((No_Visual_Recoil_Punch_Vel_Offset >= 0) && (Local_Player != 0))
		{
			float* Punch_Vel = (float*)(Local_Player + No_Visual_Recoil_Punch_Vel_Offset);

			Punch_Vel[0] = 0.f;

			Punch_Vel[1] = 0.f;

			Punch_Vel[2] = 0.f;
		}
	}

	Draw_Hitmarker();

	LuaHost_RenderFrame();

	ImGui::GetIO().MouseDrawCursor = Menu_Open;

	if (Menu_Open == true)
	{
		Show_Menu();
	}

	{
		ImDrawList* Draw_List = ImGui::GetBackgroundDrawList();

		const float Padding = 8.f;

		const ImVec2 Position(10.f, 10.f);

		ImFont* Font = (Font_Title != nullptr) ? Font_Title : ImGui::GetFont();

		const float Font_Size = (Font_Title != nullptr) ? Font_Title->LegacySize : ImGui::GetFontSize();

		ImVec2 Title_Size = Font->CalcTextSizeA(Font_Size, FLT_MAX, 0.f, "VORTEX");

		ImVec2 Sub_Size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, " v1.3 - beta");

		const ImVec2 Box_Size(Title_Size.x + Sub_Size.x + Padding * 2.f, Font_Size + Padding * 2.f);

		Draw_List->AddRectFilled(Position, ImVec2(Position.x + Box_Size.x, Position.y + Box_Size.y), M_U32(0x45, 0x45, 0x45), 6.f);

		Draw_List->AddRect(Position, ImVec2(Position.x + Box_Size.x, Position.y + Box_Size.y), M_U32(0xFF, 0x50, 0x5A), 6.f, 0, 1.f);

		const float Text_Y = Position.y + Padding - 2.f;

		Draw_List->AddText(Font, Font_Size, ImVec2(Position.x + Padding, Text_Y), M_U32(0xFF, 0x50, 0x5A), "VORTEX");

		Draw_List->AddText(ImVec2(Position.x + Padding + Title_Size.x, Text_Y + 2.f), M_U32(0xB5, 0xB5, 0xB5), "v1.3 - beta");
	}

	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return Original_EndScene(Device);
}

static void Hook_Vtable(void** Vtable)
{

	Original_EndScene = (EndScene_Type)Vtable[42];

	Original_Reset = (Reset_Type)Vtable[16];

	DWORD Previous_Access_Rights;

	VirtualProtect(&Vtable[42], sizeof(void*) * 2, PAGE_READWRITE, &Previous_Access_Rights);

	Vtable[42] = (void*)EndScene_Hook;

	Vtable[16] = (void*)Reset_Hook;

	VirtualProtect(&Vtable[42], sizeof(void*) * 2, Previous_Access_Rights, &Previous_Access_Rights);
}

void Initialize_D3D9()
{

	const void* Device_Pattern = Find_Pattern(L"shaderapidx9.dll", "A1 ? ? ? ? 8B 08 8B 51 ? 50 FF D2 8B F8");

	if (Device_Pattern != nullptr)
	{
		void* Device = **reinterpret_cast<void***>((unsigned __int32)Device_Pattern + 1);

		Hook_Vtable(*(void***)Device);

		Install_Surface_Cursor_Hook();

		return;
	}

	const HMODULE D3D9_Module = LoadLibraryW(L"d3d9.dll");

	typedef IDirect3D9*(__stdcall* Direct3DCreate9_Type)(unsigned __int32 SDK_Version);

	const Direct3DCreate9_Type Direct3DCreate9 = (Direct3DCreate9_Type)GetProcAddress(D3D9_Module, "Direct3DCreate9");

	IDirect3D9* D3D = Direct3DCreate9(D3D_SDK_VERSION);

	WNDCLASSEXW Class = { sizeof(WNDCLASSEXW), CS_CLASSDC, DefWindowProcW, 0L, 0L, GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr, L"VortexTemp", nullptr };

	RegisterClassExW(&Class);

	HWND Temp_Window = CreateWindowW(L"VortexTemp", L"Temp", WS_OVERLAPPED, 0, 0, 100, 100, nullptr, nullptr, Class.hInstance, nullptr);

	D3DPRESENT_PARAMETERS Parameters = { };

	Parameters.Windowed = TRUE;

	Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;

	Parameters.BackBufferFormat = D3DFMT_UNKNOWN;

	Parameters.BackBufferCount = 1;

	Parameters.hDeviceWindow = Temp_Window;

	Parameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	IDirect3DDevice9* Temp_Device = nullptr;

	if (D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Temp_Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &Parameters, &Temp_Device) == D3D_OK)
	{
		Hook_Vtable(*(void***)Temp_Device);

		Temp_Device->Release();
	}

	D3D->Release();

	DestroyWindow(Temp_Window);

	UnregisterClassW(L"VortexTemp", Class.hInstance);

	Install_Surface_Cursor_Hook();
}