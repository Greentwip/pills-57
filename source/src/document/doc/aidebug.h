#include "aidata.h"

extern	AI_DATA	ai_param[][8];

s16		daiLevel = 3;
s16		daiMode = 7;
s16		debug_gamespeed;
s16		debug_p1cpu;

STRTBL	daiLevelStr[] = {
	{30,20,0x7fdf,"FAST ERASE\0"},
	{30,20,0xffc1,"SMALL RENSA\0"},
	{30,20,0xfbc1,"RENSA & ERASE\0"},
	{30,20,0xfbdf,"RENSA & ATTACK\0"},
};

STRTBL	daiModeStr[] = {
	{130,20,0xffff,"DEAD\0"},
	{130,20,0xffff,"NARROW\0"},
	{130,20,0xffff,"LAST\0"},
	{130,20,0xffff,"LAST VSNH\0"},
	{130,20,0xffff,"LAST VSN\0"},
	{130,20,0xffff,"BADLINE 2\0"},
	{130,20,0xffff,"BADLINE 1\0"},
	{130,20,0xffff,"NORMAL\0"},
};

STRTBL	dai_str_table[] = {
	{ 30,  42,0xffff,"OLVrs"},
	{ 70,  42,0xffff,"ErVrs"},
	{ 30,  62,0xffff,"ErL1"},
	{ 70,  62,0xffff,"HeiEr"},
	{110,  62,0xffff,"WidEr"},
	{ 30,  82,0xffff,"ALN B"},
	{ 70,  82,0xffff," FC"},
	{110,  82,0xffff," FV"},
	{150,  82,0xffff," C"},
	{190,  82,0xffff," V"},
	{240,  82,0xffff," HeightP"},
	{ 30, 112,0xffff,"OnVrs"},
	{ 70, 112,0xffff,"Rensa"},
	{110, 112,0xffff,"MRensa"},
	{ 30, 132,0xffff,"HWLine2"},
	{ 70, 132,0xffff," 3"},
	{110, 132,0xffff," 4"},
	{150, 132,0xffff," 5"},
	{190, 132,0xffff," 6"},
	{230, 132,0xffff," 7"},
	{270, 132,0xffff," 8"},
	{ 30, 162,0xffff,"HiCap"},
	{ 70, 162,0xffff,"HiVrs"},
	{110, 162,0xffff,"WallP"},
	{ 30, 192,0xffff,"DEBUG SPEED"},
	{110, 192,0xffff,"Player1 COM"},
};

STRTBL	daiDebugP1Str[] = {
	{150,200,0xffff,"----- (MAN)\0"},
	{150,200,0xffff,"MARIO\0"},
	{150,200,0xffff,"NOKONOKO\0"},
	{150,200,0xffff,"BOMHEI\0"},
	{150,200,0xffff,"PUKUPUKU\0"},
	{150,200,0xffff,"CHOROPOO\0"},
	{150,200,0xffff,"HEIHOO\0"},
	{150,200,0xffff,"HANACHAN\0"},
	{150,200,0xffff,"TELESA\0"},
	{150,200,0xffff,"PAKKUN\0"},
	{150,200,0xffff,"KAMEK\0"},
	{150,200,0xffff,"KOOPA\0"},
	{150,200,0xffff,"PEACH\0"},
	{150,200,0xffff,"PLAIN 1\0"},
	{150,200,0xffff,"PLAIN 2\0"},
	{150,200,0xffff,"PLAIN 3\0"},
	{150,200,0xffff,"PLAIN 4\0"},
	{150,200,0xffff,"MARIO (ROM)\0"},
	{150,200,0xffff,"NOKONOKO (ROM)\0"},
	{150,200,0xffff,"BOMHEI (ROM)\0"},
	{150,200,0xffff,"PUKUPUKU (ROM)\0"},
	{150,200,0xffff,"CHOROPOO (ROM)\0"},
	{150,200,0xffff,"HEIHOO (ROM)\0"},
	{150,200,0xffff,"HANACHAN (ROM)\0"},
	{150,200,0xffff,"TELESA (ROM)\0"},
	{150,200,0xffff,"PAKKUN (ROM)\0"},
	{150,200,0xffff,"KAMEK (ROM)\0"},
	{150,200,0xffff,"KOOPA (ROM)\0"},
	{150,200,0xffff,"PEACH (ROM)\0"},
	{150,200,0xffff,"PLAIN 1 (ROM)\0"},
	{150,200,0xffff,"PLAIN 2 (ROM)\0"},
	{150,200,0xffff,"PLAIN 3 (ROM)\0"},
	{150,200,0xffff,"PLAIN 4 (ROM)\0"},
};

/*
NUMTBL	dai_num_table[] = {
	{120, 20,0xff01,5,-32101},
};
*/

PARAMTBL	dai_param_table[] = {
	{ 30,  30,0xffff,	3,	  0,&daiLevel},
	{130,  30,0xffff,	7,	  0,&daiMode},
	{ 30,  50,0xffff,9999,-9999,&(ai_param[0][0].dt_LnOnLinVrs)},
	{ 70,  50,0xffff,9999,-9999,&(ai_param[0][0].dt_LnEraseVrs)},
	{ 30,  70,0xffff,9999,-9999,&(ai_param[0][0].dt_EraseLinP1)},
	{ 70,  70,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiEraseLinRate)},
	{110,  70,0xffff,9999,-9999,&(ai_param[0][0].dt_WidEraseLinRate)},
	{ 70,  90,0xffff,9999,-9999,&(ai_param[0][0].dt_P_ALN_FallCap)},
	{110,  90,0xffff,9999,-9999,&(ai_param[0][0].dt_P_ALN_FallVrs)},
	{150,  90,0xffff,9999,-9999,&(ai_param[0][0].dt_P_ALN_Capsule)},
	{190,  90,0xffff,9999,-9999,&(ai_param[0][0].dt_P_ALN_Virus)},
	{240,  90,0xffff,9999,-9999,&(ai_param[0][0].dt_P_ALN_HeightP)},
	{ 30, 100,0xffff,9999,-9999,&(ai_param[0][0].dt_WP_ALN_Bottom)},
	{ 70, 100,0xffff,9999,-9999,&(ai_param[0][0].dt_WP_ALN_FallCap)},
	{110, 100,0xffff,9999,-9999,&(ai_param[0][0].dt_WP_ALN_FallVrs)},
	{150, 100,0xffff,9999,-9999,&(ai_param[0][0].dt_WP_ALN_Capsule)},
	{190, 100,0xffff,9999,-9999,&(ai_param[0][0].dt_WP_ALN_Virus)},
	{ 30, 120,0xffff,9999,-9999,&(ai_param[0][0].dt_OnVirusP)},
	{ 70, 120,0xffff,9999,-9999,&(ai_param[0][0].dt_RensaP)},
	{110, 120,0xffff,9999,-9999,&(ai_param[0][0].dt_RensaMP)},
	{ 30, 140,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiLinesAllp2)},
	{ 70, 140,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiLinesAllp3)},
	{110, 140,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiLinesAllp4)},
	{150, 140,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiLinesAllp5)},
	{190, 140,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiLinesAllp6)},
	{230, 140,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiLinesAllp7)},
	{270, 140,0xffff,9999,-9999,&(ai_param[0][0].dt_HeiLinesAllp8)},
	{ 30, 150,0xffff,9999,-9999,&(ai_param[0][0].dt_WidLinesAllp2)},
	{ 70, 150,0xffff,9999,-9999,&(ai_param[0][0].dt_WidLinesAllp3)},
	{110, 150,0xffff,9999,-9999,&(ai_param[0][0].dt_WidLinesAllp4)},
	{150, 150,0xffff,9999,-9999,&(ai_param[0][0].dt_WidLinesAllp5)},
	{190, 150,0xffff,9999,-9999,&(ai_param[0][0].dt_WidLinesAllp6)},
	{230, 150,0xffff,9999,-9999,&(ai_param[0][0].dt_WidLinesAllp7)},
	{270, 150,0xffff,9999,-9999,&(ai_param[0][0].dt_WidLinesAllp8)},
	{ 30, 170,0xffff,9999,-9999,&(ai_param[0][0].dt_LnHighCaps)},
	{ 70, 170,0xffff,9999,-9999,&(ai_param[0][0].dt_LnHighVrs)},
	{110, 170,0xffff,	1,	  0,&(ai_param[0][0].dt_aiWall_F)},

	{ 30, 200,0xffff,	20,	  1,&debug_gamespeed},
	{110, 200,0xffff,	32,	  0,&debug_p1cpu},

};



