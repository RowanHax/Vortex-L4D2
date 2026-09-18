#pragma once

#include <math.h>

#include <windows.h>

#include "Internal.hpp"

#include "D3D9.hpp"

#include "Sdk.hpp"

#include "Aimbot.hpp"

extern bool Aim_Infected_Enabled;

extern __int32 Aim_Infected_Key;

extern float Aim_Infected_Fov;

extern float Aim_Infected_Smooth;

extern float Aim_Infected_Distance;

extern bool Aim_Infected_Target_Head;

extern bool Aim_Infected_Silent;

static const __int32 Aim_Infected_Entity_Cap = 64;

static float Aim_Infected_Normalize_Angle(float Angle)
{
	while (Angle > 180.f)
	{
		Angle -= 360.f;
	}

	while (Angle < -180.f)
	{
		Angle += 360.f;
	}

	return Angle;
}

static float Aim_Infected_Field_Of_View(const float View_Angles[2], const float Aim_Angles[2])
{
	const float Delta_Pitch = Aim_Infected_Normalize_Angle(Aim_Angles[0] - View_Angles[0]);

	const float Delta_Yaw = Aim_Infected_Normalize_Angle(Aim_Angles[1] - View_Angles[1]);

	return sqrtf((Delta_Pitch * Delta_Pitch) + (Delta_Yaw * Delta_Yaw));
}

static bool Aim_Infected_Is_Valid_Target(void* Entity)
{
	__try
	{
		__int32 Team = 0;

		__int32 Health = 0;

		unsigned __int8 Dead = 0;

		if (Sdk_Read_Entity_Basic_Fast(Entity, Team, Health, Dead) == false)
		{
			return false;
		}

		if ((Team != 2) || (Dead != 0) || (Health <= 0))
		{
			return false;
		}

		if (Sdk_Is_Readable_Range((unsigned __int8*)Entity + 0x165, sizeof(unsigned __int8)) == false)
		{
			return false;
		}

		return *(unsigned __int8*)((unsigned __int8*)Entity + 0x165) == 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

static bool Aim_Infected_Find_Target(void* Local_Player, const float Eye_Position[3], const float View_Angles[2], float Aim_Angles[2])
{
	float Best_Fov = Aim_Infected_Fov;

	bool Found = false;

	__try
	{
		const __int32 Max_Entities = min(Sdk_Get_Max_Entities_Safe(), Aim_Infected_Entity_Cap);

		for (__int32 Index = 1; Index <= Max_Entities; Index++)
		{
			void* Entity = Sdk_Get_Client_Entity_Safe(Index);

			if ((Entity == nullptr) || (Entity == Local_Player))
			{
				continue;
			}

			if (Aim_Infected_Is_Valid_Target(Entity) == false)
			{
				continue;
			}

			float Origin[3];

			if (Sdk_Read_Origin_Fast(Entity, Origin) == false)
			{
				continue;
			}

			const float Delta_X = Origin[0] - Eye_Position[0];

			const float Delta_Y = Origin[1] - Eye_Position[1];

			const float Delta_Z = Origin[2] - Eye_Position[2];

			const float Distance_Squared = (Delta_X * Delta_X) + (Delta_Y * Delta_Y) + (Delta_Z * Delta_Z);

			if (Distance_Squared > (Aim_Infected_Distance * Aim_Infected_Distance))
			{
				continue;
			}

			float Goal[3];

			if (Aim_Infected_Target_Head == true)
			{
				if (Get_Head_Origin(Entity, 0, false, Goal) == false)
				{
					Goal[0] = Origin[0];

					Goal[1] = Origin[1];

					Goal[2] = Origin[2] + Get_Head_Height(Kind_Survivor);
				}
			}
			else
			{
				Goal[0] = Origin[0];

				Goal[1] = Origin[1];

				Goal[2] = Origin[2];
			}

			float Candidate_Angles[2];

			Vortex_Calculate_Aim(Eye_Position, Goal, Candidate_Angles);

			const float Fov = Aim_Infected_Field_Of_View(View_Angles, Candidate_Angles);

			if (Fov >= Best_Fov)
			{
				continue;
			}

			Best_Fov = Fov;

			Found = true;

			Aim_Angles[0] = Candidate_Angles[0];

			Aim_Angles[1] = Candidate_Angles[1];
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return Found;
}

static void Update_Aim_Infected(UserCmd_Structure* Command)
{
	if ((Aim_Infected_Enabled == false) || (Command == nullptr))
	{
		return;
	}

	if ((Aim_Infected_Key != 0) && ((GetAsyncKeyState(Aim_Infected_Key) & 0x8000) == 0))
	{
		return;
	}

	if (Sdk_Update_Game_Session() == false)
	{
		return;
	}

	__try
	{
		void* Local_Player = Sdk_Get_Local_Player_Safe();

		if (Local_Player == nullptr)
		{
			return;
		}

		__int32 Local_Team = 0;

		__int32 Local_Health = 0;

		unsigned __int8 Local_Dead = 0;

		if (Sdk_Read_Entity_Basic(Local_Player, Local_Team, Local_Health, Local_Dead) == false)
		{
			return;
		}

		if ((Local_Dead != 0) || (Local_Health <= 0) || (Local_Team != 3))
		{
			return;
		}

		float Local_Origin[3];

		if (Sdk_Get_Origin_Safe(Local_Player, Local_Origin) == false)
		{
			return;
		}

		if (Sdk_Is_Readable_Range((unsigned __int8*)Local_Player + 0xF4, sizeof(float) * 3) == false)
		{
			return;
		}

		const float* View_Offset = (const float*)((unsigned __int8*)Local_Player + 0xF4);

		const float Eye_Position[3] =
		{
			Local_Origin[0] + View_Offset[0],
			Local_Origin[1] + View_Offset[1],
			Local_Origin[2] + View_Offset[2]
		};

		const float View_Angles[2] = { Command->View_Angles[0], Command->View_Angles[1] };

		float Aim_Angles[2];

		if (Aim_Infected_Find_Target(Local_Player, Eye_Position, View_Angles, Aim_Angles) == false)
		{
			return;
		}

		float Final_Angles[2];

		if (Aim_Infected_Smooth > 1.f)
		{
			Final_Angles[0] = View_Angles[0] + (Aim_Infected_Normalize_Angle(Aim_Angles[0] - View_Angles[0]) / Aim_Infected_Smooth);

			Final_Angles[1] = View_Angles[1] + (Aim_Infected_Normalize_Angle(Aim_Angles[1] - View_Angles[1]) / Aim_Infected_Smooth);
		}
		else
		{
			Final_Angles[0] = Aim_Angles[0];

			Final_Angles[1] = Aim_Angles[1];
		}

		Final_Angles[0] = Aim_Infected_Normalize_Angle(Final_Angles[0]);

		Final_Angles[1] = Aim_Infected_Normalize_Angle(Final_Angles[1]);

		if (Final_Angles[0] > 89.f)
		{
			Final_Angles[0] = 89.f;
		}
		else if (Final_Angles[0] < -89.f)
		{
			Final_Angles[0] = -89.f;
		}

		Command->View_Angles[0] = Final_Angles[0];

		Command->View_Angles[1] = Final_Angles[1];

		Command->View_Angles[2] = 0.f;

		if (Aim_Infected_Silent == true)
		{
			Fix_Silent_Aim_Movement(Command, View_Angles);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}