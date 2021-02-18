/***********************************************************
	ai data source file		Tadashi Ogura
***********************************************************/
#include <ultra64.h>
#include "aiset.h"
#include "aidata.h"

// ＡＩの思考ロジック（エディット用）
AI_DATA ai_param[_AI_LOGIC_SUM][_AI_STATE_SUM];

// ＡＩの思考ロジック [ ロジック番号 (AI_LOGIC_????) ] [ プレイ状況番号 (AI_STATE_????) ]
AI_DATA ai_param_org[_AI_LOGIC_SUM][_AI_STATE_SUM] = {
	// AI_LOGIC_FastErase
	{
		// AI_STATE_DEAD
		{
			0, 0, -50, 0, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 1000, -400, 0, 0, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NARROW
		{
			5, 10, 30, 0, 100, 100,
			10, -70, -90, -80, -100,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, FALSE,
			14, 40, 45,
			14, 40, 45,
		},
		// AI_STATE_LASTVS
		{
			131, 2030, 30, 0, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			3000, 0, 0, 0, 0, FALSE,
			14, 40, 45,
			0, 0, 0,
		},
		// AI_STATE_LASTVSNH
		{
			131, 2030, 30, 0, 400, 400,
			10, -370, -390, -430, -450,
			0, 0, 0, 0, 0,
			3000, 0, 0, 0, 0, TRUE,
			14, 40, 45,
			0, 0, 0,
		},
		// AI_STATE_LASTVSN
		{
			431, 2030, 30, 0, 1000, 1000,
			10, -70, -90, -80, -100,
			-360, -420, -540, -480, -500,
			3000, 0, 0, 0, 0, TRUE,
			0, 0, 0,
			54, 140, 145,
		},
		// AI_STATE_BADLINE2
		{
			5, 10, 30, 0, 0, 500,
			10, -70, -90, -80, -100,
			0, -140, -180, -160, -200,
			0, 0, 0, 50, 50, FALSE,
			0, 0, 0,
			114, 240, 245,
		},
		// AI_STATE_BADLINE1
		{
			0, 0, 0, 0, 0, 0,
			10, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, FALSE,
			0, 0, 0,
			0, 0, 0,
		},
		// AI_STATE_NORMAL
		{
			0, 31, 50, 0, 400, 400,
			3, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, FALSE,
			14, 40, 40,
			0, 0, 0,
		},
	},
	// AI_LOGIC_SmallRensa
	{
		// AI_STATE_DEAD
		{
			0, 0, -50, 0, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 1000, -400, 0, 0, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NARROW
		{
			5, 10, 30, 0, 100, 100,
			10, -70, -90, -80, -100,
			0, 0, 0, 0, 0,
			0, 500, 0, 0, 0, FALSE,
			14, 40, 45,
			14, 40, 45,
		},
		// AI_STATE_LASTVS
		{
			131, 2030, 30, 0, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			3000, 200, -200, 0, 0, FALSE,
			64, 140, 145,
			0, 0, 0,
		},
		// AI_STATE_LASTVSNH
		{
			131, 2030, 30, 0, 400, 400,
			10, -370, -390, -530, -550,
			0, 0, 0, 0, 0,
			3000, 200, 0, 0, 0, TRUE,
			64, 140, 145,
			0, 0, 0,
		},
		// AI_STATE_LASTVSN
		{
			431, 2030, 30, 0, 1000, 1000,
			10, -70, -90, -80, -100,
			-360, -420, -540, -480, -600,
			3000, 200, 0, 0, 0, TRUE,
			0, 0, 0,
			64, 140, 145,
		},
		// AI_STATE_BADLINE2
		{
			5, 10, 30, 0, 0, 500,
			10, -70, -90, -80, -100,
			0, -140, -180, -160, -200,
			0, 200, 0, 50, 50, FALSE,
			0, 0, 0,
			114, 240, 245,
		},
		// AI_STATE_BADLINE1
		{
			31, 50, 30, 0, 400, 400,
			10, -370, -390, -430, -450,
			0, 0, 0, 0, 0,
			0, 3300, 0, 330, 330, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NORMAL
		{
			31, 50, 30, 0, 400, 400,
			10, -370, -390, -430, -450,
			0, 0, 0, 0, 0,
			0, 300, -200, 0, 0, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
	},
	// AI_LOGIC_RensaAndErase
	{
		// AI_STATE_DEAD
		{
			0, 0, -50, 0, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 1000, -400, 0, 0, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NARROW
		{
			5, 10, 30, 0, 100, 100,
			10, -70, -90, -80, -100,
			0, 0, 0, 0, 0,
			0, 500, 0, 0, 0, FALSE,
			14, 40, 45,
			14, 40, 45,
		},
		// AI_STATE_LASTVS
		{
			131, 2030, 30, 0, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			3000, 500, -500, 0, 0, FALSE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSNH
		{
			131, 2030, 30, 0, 400, 400,
			10, -370, -390, -430, -450,
			0, 0, 0, 0, 0,
			3000, 500, 0, 0, 0, TRUE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSN
		{
			431, 2030, 30, 0, 1000, 1000,
			10, -70, -90, -80, -100,
			-360, -420, -540, -480, -600,
			3000, 200, 0, 0, 0, TRUE,
			0, 0, 0,
			64, 140, 145,
		},
		// AI_STATE_BADLINE2
		{
			5, 10, 30, 0, 0, 500,
			10, -70, -90, -80, -100,
			0, -140, -180, -160, -200,
			0, 500, 0, 50, 50, FALSE,
			0, 0, 0,
			114, 240, 245,
		},
		// AI_STATE_BADLINE1
		{
			31, 50, 30, 0, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 4400, 0, 440, 440, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NORMAL
		{
			31, 50, 30, 0, 400, 400,
			30, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 1400, -800, 0, 0, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
	},
	// AI_LOGIC_RensaAndAttack
	{
		// AI_STATE_DEAD
		{
			0, 0, -50, 0, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 1000, -400, 0, 0, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NARROW
		{
			5, 10, 30, 0, 100, 100,
			10, -70, -90, -80, -100,
			0, 0, 0, 0, 0,
			0, 500, 0, 0, 0, FALSE,
			14, 40, 45,
			14, 40, 45,
		},
		// AI_STATE_LASTVS
		{
			131, 2030, 30, 0, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			3000, 500, -500, 0, 0, FALSE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSNH
		{
			131, 2030, 30, 0, 400, 400,
			10, -370, -390, -430, -450,
			0, 0, 0, 0, 0,
			3000, 500, 0, 0, 0, TRUE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSN
		{
			431, 2030, 30, 0, 1000, 1000,
			10, -70, -90, -80, -100,
			-360, -420, -540, -480, -600,
			3000, 200, 0, 0, 0, TRUE,
			0, 0, 0,
			64, 140, 145,
		},
		// AI_STATE_BADLINE2
		{
			5, 10, 30, 0, 0, 500,
			10, -70, -90, -80, -100,
			0, -140, -180, -160, -200,
			0, 500, 0, 50, 50, FALSE,
			0, 0, 0,
			114, 240, 245,
		},
		// AI_STATE_BADLINE1
		{
			0, 0, -50, 0, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 4400, 0, 440, 440, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NORMAL
		{
			0, 0, -50, 0, 100, 100,
			100, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 1400, -800, 0, 0, FALSE,
			194, 490, 990,
			0, 0, 0,
		},
	},
	// AI_LOGIC_WidErase
	{
		// AI_STATE_DEAD
		{
			0, 0, -50, 0, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 1000, -400, 0, 0, FALSE,
			0, 0, 0,
			194, 490, 490,
		},
		// AI_STATE_NARROW
		{
			5, 10, 30, 0, 100, 100,
			10, -70, -90, -80, -100,
			0, 0, 0, 0, 0,
			0, 500, 0, 0, 0, FALSE,
			14, 40, 45,
			14, 40, 45,
		},
		// AI_STATE_LASTVS
		{
			131, 2030, 30, 0, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			3000, 500, -500, 0, 0, FALSE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSNH
		{
			131, 2030, 30, 0, 400, 400,
			10, -370, -390, -430, -450,
			0, 0, 0, 0, 0,
			3000, 500, 0, 0, 0, TRUE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSN
		{
			431, 2030, 30, 0, 1000, 1000,
			10, -70, -90, -80, -100,
			-360, -420, -540, -480, -600,
			3000, 200, 0, 0, 0, TRUE,
			0, 0, 0,
			64, 140, 145,
		},
		// AI_STATE_BADLINE2
		{
			5, 10, 30, 0, 0, 500,
			10, -70, -90, -80, -100,
			0, -140, -180, -160, -200,
			0, 500, 0, 50, 50, FALSE,
			0, 0, 0,
			114, 240, 245,
		},
		// AI_STATE_BADLINE1
		{
			31, 50, 30, 0, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 4400, 0, 440, 440, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NORMAL
		{
			31, 50, 30, 0, 400, 800,
			30, -170, -190, -230, -250,
			0, -170, -190, -230, -250,
			0, 1400, 0, 0, 0, FALSE,
			0, 0, 0,
			194, 490, 490,
		},
	},
	// AI_LOGIC_MajorityRensa
	{
		// AI_STATE_DEAD
		{
			0, 0, -50, 1000, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 0, -400, 0, 0, FALSE,
			194, 490, 490,
			194, 490, 490,
		},
		// AI_STATE_NARROW
		{
			5, 10, 30, 500, 100, 100,
			10, -70, -90, -80, -100,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, FALSE,
			14, 40, 45,
			14, 40, 45,
		},
		// AI_STATE_LASTVS
		{
			131, 2030, 30, 500, 400, 400,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			3000, 0, -500, 0, 0, FALSE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSNH
		{
			131, 2030, 30, 500, 400, 400,
			10, -370, -390, -430, -450,
			0, 0, 0, 0, 0,
			3000, 0, 0, 0, 0, TRUE,
			114, 240, 245,
			0, 0, 0,
		},
		// AI_STATE_LASTVSN
		{
			431, 2030, 30, 200, 1000, 1000,
			10, -70, -90, -80, -100,
			-360, -420, -540, -480, -600,
			3000, 0, 0, 0, 0, TRUE,
			0, 0, 0,
			64, 140, 145,
		},
		// AI_STATE_BADLINE2
		{
			5, 10, 30, 500, 0, 500,
			10, -70, -90, -80, -100,
			0, -140, -180, -160, -200,
			0, 0, 0, 50, 50, FALSE,
			0, 0, 0,
			114, 240, 245,
		},
		// AI_STATE_BADLINE1
		{
			0, 0, -50, 4400, 100, 100,
			10, -170, -190, -230, -250,
			0, 0, 0, 0, 0,
			0, 0, 0, 440, 440, FALSE,
			194, 490, 490,
			0, 0, 0,
		},
		// AI_STATE_NORMAL
		{
			0, 0, -50, 1400, 100, 100,
			100, -170, -190, -230, -250,
			0, -170, -190, -230, -250,
			0, 700, -800, 0, 0, FALSE,
			194, 490, 990,
			194, 490, 990,
		},
	},
};

AI_CHAR ai_char_data[16];

// キャラの思考パターン [ キャラの番号 (CHR_????) ]
AI_CHAR ai_char_data_org[16] = {
	// CHR_MARIO
	{
		TRUE,
		AI_SPEED_FAST,
		0,

		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,

		AI_CONDITION_Random,
		AI_CONDITION_ManyVir,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		20,5,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Waver,
		AI_EFFECT_BeNoWait,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,1,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_NOKO
	{
		FALSE,
		AI_SPEED_VERYSLOW,
		0,

		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,

		AI_CONDITION_Unconditional,
		AI_CONDITION_Damage,
		AI_CONDITION_Random,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,0,20,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Waver,
		AI_EFFECT_NotMove,
		AI_EFFECT_Rotate,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,1,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_BOMB
	{
		FALSE,
		AI_SPEED_VERYSLOW,
		0,

		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,

		AI_CONDITION_Damage,
		AI_CONDITION_Damage,
		AI_CONDITION_Random,
		AI_CONDITION_Random,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,0,50,20,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_BeFast,
		AI_EFFECT_BeAttack,
		AI_EFFECT_Waver,
		AI_EFFECT_Rotate,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		5,7,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PUKU
	{
		FALSE,
		AI_SPEED_SLOW,
		0,

		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,
		AI_LOGIC_FastErase,

		AI_CONDITION_Random,
		AI_CONDITION_Random,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		30,5,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Rotate,
		AI_EFFECT_BeAttack,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,5,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_CHOR
	{
		FALSE,
		AI_SPEED_SLOW,
		0,

		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,

		AI_CONDITION_Rensa,
		AI_CONDITION_Random,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		1,30,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_BeNoWait,
		AI_EFFECT_Waver,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		2,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PROP
	{
		FALSE,
		AI_SPEED_VERYFAST,
		0,

		AI_LOGIC_WidErase,
		AI_LOGIC_WidErase,
		AI_LOGIC_WidErase,
		AI_LOGIC_WidErase,
		AI_LOGIC_WidErase,
		AI_LOGIC_WidErase,
		AI_LOGIC_WidErase,
		AI_LOGIC_WidErase,

		AI_CONDITION_Unconditional,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Rotate,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_HANA
	{
		FALSE,
		AI_SPEED_FAST,
		0,

		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,
		AI_LOGIC_SmallRensa,

		AI_CONDITION_Damage,
		AI_CONDITION_Random,
		AI_CONDITION_Last,
		AI_CONDITION_Rensa,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,20,10,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_BeAttack,
		AI_EFFECT_Waver,
		AI_EFFECT_Rotate,
		AI_EFFECT_Lose,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,0,0,1,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_TERE
	{
		TRUE,
		AI_SPEED_VERYSLOW,
		0,

		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,

		AI_CONDITION_Random,
		AI_CONDITION_Damage,
		AI_CONDITION_Random,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		20,0,50,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Rapid,
		AI_EFFECT_Lose,
		AI_EFFECT_Rotate,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,1,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PACK
	{
		TRUE,
		AI_SPEED_VERYFAST,
		0,

		AI_LOGIC_MajorityRensa,
		AI_LOGIC_MajorityRensa,
		AI_LOGIC_MajorityRensa,
		AI_LOGIC_MajorityRensa,
		AI_LOGIC_MajorityRensa,
		AI_LOGIC_MajorityRensa,
		AI_LOGIC_MajorityRensa,
		AI_LOGIC_MajorityRensa,

		AI_CONDITION_Random,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		33,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Waver,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_KAME
	{
		TRUE,
		AI_SPEED_FAST,
		0,

		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,

		AI_CONDITION_Pile,
		AI_CONDITION_Rensa,
		AI_CONDITION_Damage,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,1,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_BeNoWait,
		AI_EFFECT_BeFast,
		AI_EFFECT_Waver,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		3,3,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_KUPP
	{
		TRUE,
		AI_SPEED_VERYFAST,
		0,

		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,
		AI_LOGIC_RensaAndErase,

		AI_CONDITION_Last,
		AI_CONDITION_FewVir,
		AI_CONDITION_Damage,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		10,10,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_BeFast,
		AI_EFFECT_BeSlow,
		AI_EFFECT_BeNoWait,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,1,1,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PEACH
	{
		FALSE,
		AI_SPEED_VERYFAST,
		0,

		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,
		AI_LOGIC_RensaAndAttack,

		AI_CONDITION_ManyVir,
		AI_CONDITION_Random,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		5,10,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_BeFast,
		AI_EFFECT_Rotate,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		1,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PLAIN1
	{
		TRUE,
		AI_SPEED_FAST,
		0,

		AI_LOGIC_FastErase,			// AI_STATE_DEAD
		AI_LOGIC_SmallRensa,		// AI_STATE_NARROW
		AI_LOGIC_RensaAndErase,		// AI_STATE_LASTVS
		AI_LOGIC_RensaAndAttack,	// AI_STATE_LASTVSNH
		AI_LOGIC_WidErase,			// AI_STATE_LASTVSN
		AI_LOGIC_MajorityRensa,		// AI_STATE_BADLINE2
		AI_LOGIC_FastErase,			// AI_STATE_BADLINE1
		AI_LOGIC_FastErase,			// AI_STATE_NORMAL

		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PLAIN2
	{
		TRUE,
		AI_SPEED_FAST,
		0,

		AI_LOGIC_FastErase,			// AI_STATE_DEAD
		AI_LOGIC_SmallRensa,		// AI_STATE_NARROW
		AI_LOGIC_RensaAndErase,		// AI_STATE_LASTVS
		AI_LOGIC_RensaAndAttack,	// AI_STATE_LASTVSNH
		AI_LOGIC_WidErase,			// AI_STATE_LASTVSN
		AI_LOGIC_MajorityRensa,		// AI_STATE_BADLINE2
		AI_LOGIC_FastErase,			// AI_STATE_BADLINE1
		AI_LOGIC_FastErase,			// AI_STATE_NORMAL

		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PLAIN3
	{
		TRUE,
		AI_SPEED_FAST,
		0,

		AI_LOGIC_FastErase,			// AI_STATE_DEAD
		AI_LOGIC_SmallRensa,		// AI_STATE_NARROW
		AI_LOGIC_RensaAndErase,		// AI_STATE_LASTVS
		AI_LOGIC_RensaAndAttack,	// AI_STATE_LASTVSNH
		AI_LOGIC_WidErase,			// AI_STATE_LASTVSN
		AI_LOGIC_MajorityRensa,		// AI_STATE_BADLINE2
		AI_LOGIC_FastErase,			// AI_STATE_BADLINE1
		AI_LOGIC_FastErase,			// AI_STATE_NORMAL

		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
	// CHR_PLAIN4
	{
		TRUE,
		AI_SPEED_FAST,
		0,

		AI_LOGIC_FastErase,			// AI_STATE_DEAD
		AI_LOGIC_SmallRensa,		// AI_STATE_NARROW
		AI_LOGIC_RensaAndErase,		// AI_STATE_LASTVS
		AI_LOGIC_RensaAndAttack,	// AI_STATE_LASTVSNH
		AI_LOGIC_WidErase,			// AI_STATE_LASTVSN
		AI_LOGIC_MajorityRensa,		// AI_STATE_BADLINE2
		AI_LOGIC_FastErase,			// AI_STATE_BADLINE1
		AI_LOGIC_FastErase,			// AI_STATE_NORMAL

		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore, AI_CONDITION_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,

		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore, AI_EFFECT_Ignore,
		0,0,0,0,
		        0,0,0,0,
		0,0,0,0,0,0,0,0,
	},
};

