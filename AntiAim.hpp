#pragma once

#include <math.h>

#include "Internal.hpp"

#include "Sdk.hpp"

enum Anti_Aim_Yaw_Mode_Values
{
	Anti_Aim_Yaw_Off = 0,
	Anti_Aim_Yaw_Static,
	Anti_Aim_Yaw_Backwards,
	Anti_Aim_Yaw_Fake_Backwards,
	Anti_Aim_Yaw_Fake_Sideways,
	Anti_Aim_Yaw_Spin,
	Anti_Aim_Yaw_Jitter
};

enum Anti_Aim_Pitch_Mode_Values
{
	Anti_Aim_Pitch_Off = 0,
	Anti_Aim_Pitch_Up,
	Anti_Aim_Pitch_Down,
	Anti_Aim_Pitch_Jitter
};

static bool Anti_Aim_Flip;

static float Anti_Aim_Normalize(float Angle)
{
	Angle = fmodf(Angle, 360.f);

	if (Angle > 180.f)
	{
		Angle -= 360.f;
	}
	else if (Angle < -180.f)
	{
		Angle += 360.f;
	}

	return Angle;
}

static void Update_Anti_Aim(UserCmd_Structure* Command)
{
	if ((Anti_Aim_Enabled == false) || (Command == nullptr))
	{
		return;
	}

	__try
	{
		const unsigned __int32 Local_Player = Get_Local_Player();

		if ((Local_Player == 0) || (*(unsigned __int8*)(Local_Player + 0x147) != 0) || (*(__int32*)(Local_Player + 0xEC) <= 0))
		{
			return;
		}

		float Pitch = Command->View_Angles[0];

		float Yaw = Command->View_Angles[1];

		Anti_Aim_Flip = !Anti_Aim_Flip;

		switch (Anti_Aim_Yaw_Mode)
		{
		case Anti_Aim_Yaw_Static:
			Yaw = Anti_Aim_Yaw_Value;
			break;

		case Anti_Aim_Yaw_Backwards:
			Yaw += 180.f;
			break;

		case Anti_Aim_Yaw_Fake_Backwards:
			Yaw += 180.f + (Anti_Aim_Flip ? Anti_Aim_Yaw_Value : -Anti_Aim_Yaw_Value);
			break;

		case Anti_Aim_Yaw_Fake_Sideways:
			Yaw += (Anti_Aim_Flip ? 90.f : -90.f);
			break;

		case Anti_Aim_Yaw_Spin:
			Yaw += Anti_Aim_Yaw_Value;
			break;

		case Anti_Aim_Yaw_Jitter:
			Yaw += (Anti_Aim_Flip ? Anti_Aim_Yaw_Value : -Anti_Aim_Yaw_Value);
			break;
		}

		switch (Anti_Aim_Pitch_Mode)
		{
		case Anti_Aim_Pitch_Up:
			Pitch = -Anti_Aim_Pitch_Value;
			break;

		case Anti_Aim_Pitch_Down:
			Pitch = Anti_Aim_Pitch_Value;
			break;

		case Anti_Aim_Pitch_Jitter:
			Pitch += (Anti_Aim_Flip ? Anti_Aim_Pitch_Value : -Anti_Aim_Pitch_Value);
			break;
		}

		if (Pitch > 89.f)
		{
			Pitch = 89.f;
		}
		else if (Pitch < -89.f)
		{
			Pitch = -89.f;
		}

		Command->View_Angles[0] = Pitch;

		Command->View_Angles[1] = Anti_Aim_Normalize(Yaw);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}