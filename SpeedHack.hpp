#pragma once

#include <windows.h>

#include "Internal.hpp"

extern bool    Speed_Hack_Enabled;
extern __int32 Speed_Hack_Factor;
extern __int32 Speed_Hack_Key;

static __int32 Speed_Hack_Extra_Moves()
{
	if (Speed_Hack_Enabled == false)
	{
		return 0;
	}

	if ((Speed_Hack_Key != 0) && ((GetAsyncKeyState(Speed_Hack_Key) & 0x8000) == 0))
	{
		return 0;
	}

	if (Speed_Hack_Factor <= 0)
	{
		return 0;
	}

	return (Speed_Hack_Factor > 16) ? 16 : Speed_Hack_Factor;
}