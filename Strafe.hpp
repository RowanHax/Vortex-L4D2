#pragma once

#include <math.h>

#include "Internal.hpp"

#include "D3D9.hpp"

#include "Sdk.hpp"

static void Update_Air_Strafe(UserCmd_Structure* Command)
{
	if ((Strafe_Enabled == false) || (Command == nullptr))
	{
		return;
	}

	const unsigned __int32 Local_Player = Get_Local_Player();

	if (Local_Player == 0)
	{
		return;
	}

	if ((*(unsigned __int8*)(Local_Player + 0x147) != 0) || (*(__int32*)(Local_Player + 0xEC) <= 0))
	{
		return;
	}

	__int32 Move_Type_Offset = Get_Net_Prop_Offset("DT_BasePlayer", "m_nMoveType");

	if (Move_Type_Offset < 0)
	{
		Move_Type_Offset = Get_Net_Prop_Offset("DT_BaseEntity", "m_nMoveType");
	}

	if (Move_Type_Offset >= 0)
	{
		const __int32 Move_Type = *(__int32*)(Local_Player + Move_Type_Offset);

		if ((Move_Type == 8) || (Move_Type == 9) || (Move_Type == 10))
		{
			return;
		}
	}

	if ((*(unsigned __int8*)(Local_Player + Flags_Offset) & FL_ONGROUND) != 0)
	{
		return;
	}

	float Velocity[3];

	Get_Velocity((void*)Local_Player, Velocity);

	const float Speed = sqrtf((Velocity[0] * Velocity[0]) + (Velocity[1] * Velocity[1]));

	if (Speed < 2.f)
	{
		return;
	}

	const float Max_Speed = 250.f;

	const float Air_Accel = 300.f;

	const float Wish_Speed = 300.f;

	const float Term = ((Wish_Speed / Air_Accel) / Max_Speed) * 100.f / Speed;

	float Perfect_Delta = 0.f;

	if ((Term > -1.f) && (Term < 1.f))
	{
		Perfect_Delta = acosf(Term);
	}

	if (Perfect_Delta != 0.f)
	{
		const float Pi = 3.14159265f;

		const float Two_Pi = Pi * 2.f;

		const float Yaw_Rad = Command->View_Angles[1] * (Pi / 180.f);

		const float Vel_Dir = atan2f(Velocity[1], Velocity[0]) - Yaw_Rad;

		const float Wish_Ang = atan2f(-Command->Side_Move, Command->Forward_Move);

		float Delta = fmodf(Vel_Dir - Wish_Ang, Two_Pi);

		if (Delta < 0.f)
		{
			Delta += Two_Pi;
		}

		if (Delta > Pi)
		{
			Delta -= Two_Pi;
		}

		const float Move_Dir = (Delta < 0.f) ? (Vel_Dir + Perfect_Delta) : (Vel_Dir - Perfect_Delta);

		Command->Forward_Move = cosf(Move_Dir) * 450.f;

		Command->Side_Move = -sinf(Move_Dir) * 450.f;
	}
}