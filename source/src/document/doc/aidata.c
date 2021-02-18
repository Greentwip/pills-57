/***********************************************************
	ai data source file		Tadashi Ogura
***********************************************************/
#include <ultra64.h>
#include "evsworks.h"
#include "aiset.h"
#include "aidata.h"

AI_DATA	ai_param[4][8];

AI_DATA	ai_param_org[4][8] = {
//	LEVEL0
	{{	// AIDT_DEAD
		0,			0,			30-80,		100,		100,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		0,			1000,		-400,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_NARROW
		15-10,		30-20,		30,			100,		100,
		-70,		-90,		-80,		-100,
		0,			0,			0,			0,			0,
		10,
		0,			0,			0,
		18-4,		45-5,		45,			45,			45,			45,			45,
		18-4,		45-5,		45,			45,			45,			45,			45,
		0,	0,	FALSE,
	},{	// AIDT_LASTVS
		15+16+100,	30+2000,	30,			400,		400,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		3000,		0,			0,
		18-4,		45-5,		45,			45,			45,			45,			45,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_LASTVSNH
		15+16+100,	30+2000,	30,			400,		400,
		-370,		-390,		-430,		-450,
		0,			0,			0,			0,			0,
		10,
		3000,		0,			0,
		18-4,		45-5,		45,			45,			45,			45,			45,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	TRUE,
	},{	// AIDT_LASTVSN
		15+16+400,	30+2000,	30,			1000,		1000,
		-70,		-90,		-80,		-100,
		-60*6,		-70*6,		-90*6,		-80*6,		-100*6,
		10,
		3000,		0,			0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+50,	45-5+100,	45+100,		45+100,		45,			45,			45,
		0,	0,	TRUE,
	},{	// AIDT_BADLINE2
		15-10,		30-20,		30,			0,			500,
		-70,		-90,		-80,		-100,
		0,			-70*2,		-90*2,		-80*2,		-100*2,
		10,
		0,			0,			0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		50,	50,	FALSE,
	},{	// AIDT_BADLINE1
		0,			0,			0,			0,		0,
		0,			0,			0,			0,
		0,			0,			0,			0,			0,
		10,
		0,			0,			0,
		0,			0,			0,			0,			0,			0,			0,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_NORMAL
		15+16,		30+20,		30,			400,		400,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		0,			0,			0,
		18-4,		45-5,		45-5,		45-5,		45-5,		45-5,		45-5,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},},

// LEVEL1
	{{	// AIDT_DEAD
		0,			0,			30-80,		100,		100,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		0,			1000,		-400,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_NARROW
		15-10,		30-20,		30,			100,		100,
		-70,		-90,		-80,		-100,
		0,			0,			0,			0,			0,
		10,
		0,			500,		0,
		18-4,		45-5,		45,			45,			45,			45,			45,
		18-4,		45-5,		45,			45,			45,			45,			45,
		0,	0,	FALSE,
	},{	// AIDT_LASTVS
		15+16+100,	30+2000,	30,			400,		400,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		3000,		200,		-200,
		18-4+50,	45-5+100,	45+100,		45+100,		45+100,		45+100,		45+100,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_LASTVSNH
		15+16+100,	30+2000,	30,			400,		400,
		-370,		-390,		-530,		-550,
		0,			0,			0,			0,			0,
		10,
		3000,		200,		0,
		18-4+50,	45-5+100,	45+100,		45+100,		45+100,		45+100,		45+100,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	TRUE,
	},{	// AIDT_LASTVSN
		15+16+400,	30+2000,	30,			1000,		1000,
		-70,		-90,		-80,		-100,
		-60*6,		-70*6,		-90*6,		-80*6,		-100*6,
		10,
		3000,		200,		0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+50,	45-5+100,	45+100,		45+100,		45,			45,			45,
		0,	0,	TRUE,
	},{	// AIDT_BADLINE2
		15-10,		30-20,		30,			0,			500,
		-70,		-90,		-80,		-100,
		0,			-70*2,		-90*2,		-80*2,		-100*2,
		10,
		0,			200,		0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		50,	50,	FALSE,
	},{	// AIDT_BADLINE1
		15+16,		30+20,		30,			400,		400,
		-370,		-390,		-430,		-450,
		0,			0,			0,			0,			0,
		10,
		0,			3300,		0,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		330,	330,	FALSE,
	},{	// AIDT_NORMAL
		15+16,		30+20,		30,			400,		400,
		-370,		-390,		-430,		-450,
		0,			0,			0,			0,			0,
		10,
		0,			300,		-200,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},},

// LEVEL2
	{{	// AIDT_DEAD
		0,			0,			30-80,		100,		100,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		0,			1000,		-400,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_NARROW
		15-10,		30-20,		30,			100,		100,
		-70,		-90,		-80,		-100,
		0,			0,			0,			0,			0,
		10,
		0,			500,		0,
		18-4,		45-5,		45,			45,			45,			45,			45,
		18-4,		45-5,		45,			45,			45,			45,			45,
		0,	0,	FALSE,
	},{	// AIDT_LASTVS
		15+16+100,	30+2000,	30,			400,		400,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		3000,		500,		-500,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_LASTVSNH
		15+16+100,	30+2000,	30,			400,		400,
		-370,		-390,		-430,		-450,
		0,			0,			0,			0,			0,
		10,
		3000,		500,		0,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	TRUE,
	},{	// AIDT_LASTVSN
		15+16+400,	30+2000,	30,			1000,		1000,
		-70,		-90,		-80,		-100,
		-60*6,		-70*6,		-90*6,		-80*6,		-100*6,
		10,
		3000,		200,		0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+50,	45-5+100,	45+100,		45+100,		45,			45,			45,
		0,	0,	TRUE,
	},{	// AIDT_BADLINE2
		15-10,		30-20,		30,			0,			500,
		-70,		-90,		-80,		-100,
		0,			-70*2,		-90*2,		-80*2,		-100*2,
		10,
		0,			500,		0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		50,	50,	FALSE,
	},{	// AIDT_BADLINE1
		15+16,		30+20,		30,			400,		400,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		0,			4400,		0,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		440,	440,	FALSE,
	},{	// AIDT_NORMAL
		15+16,		30+20,		30,			400,		400,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		30,
		0,			1400,		-800,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},},

// LEVEL3
	{{	// AIDT_DEAD
		0,			0,			30-80,		100,		100,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		0,			1000,		-400,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_NARROW
		15-10,		30-20,		30,			100,		100,
		-70,		-90,		-80,		-100,
		0,			0,			0,			0,			0,
		10,
		0,			500,		0,
		18-4,		45-5,		45,			45,			45,			45,			45,
		18-4,		45-5,		45,			45,			45,			45,			45,
		0,	0,	FALSE,
	},{	// AIDT_LASTVS
		15+16+100,	30+2000,	30,			400,		400,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		3000,		500,		-500,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},{	// AIDT_LASTVSNH
		15+16+100,	30+2000,	30,			400,		400,
		-370,		-390,		-430,		-450,
		0,			0,			0,			0,			0,
		10,
		3000,		500,		0,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	TRUE,
	},{	// AIDT_LASTVSN
		15+16+400,	30+2000,	30,			1000,		1000,
		-70,		-90,		-80,		-100,
		-60*6,		-70*6,		-90*6,		-80*6,		-100*6,
		10,
		3000,		200,		0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+50,	45-5+100,	45+100,		45+100,		45,			45,			45,
		0,	0,	TRUE,
	},{	// AIDT_BADLINE2
		15-10,		30-20,		30,			0,			500,
		-70,		-90,		-80,		-100,
		0,			-70*2,		-90*2,		-80*2,		-100*2,
		10,
		0,			500,		0,
		0,			0,			0,			0,			0,			0,			0,
		18-4+100,	45-5+200,	45+200,		45+200,		45+200,		45+200,		45+200,
		50,	50,	FALSE,
	},{	// AIDT_BADLINE1
		0,			0,			30-80,		100,		100,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		10,
		0,			4400,		0,
		18-4+180,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,	45-5+450,
		0,			0,			0,			0,			0,			0,			0,
		440,	440,	FALSE,
	},{	// AIDT_NORMAL
		0,			0,			30-80,		100,		100,
		-170,		-190,		-230,		-250,
		0,			0,			0,			0,			0,
		50,
		0,			1400,		-800,
		18-4+180,	45-5+450,	45-5+950,	45-5+950,	45-5+950,	45-5+950,	45-5+950,
		0,			0,			0,			0,			0,			0,			0,
		0,	0,	FALSE,
	},},
//						pri_point[LnOnLinVrs] = 15+16;							// ��ٽ��_���₷��
//						pri_point[LnEraseVrs] = 30+20;							//	 �V
};

#if 0
	// ���[�N������
	pri_point[LnHighCaps] = 30;
	pri_point[LnHighVrs] = 30;
	aiWall = 0;
	AloneCapWP[ALN_Bottom] = 0;
	AloneCapWP[ALN_FallCap] = 0;
	AloneCapWP[ALN_FallVrs] = 0;
	AloneCapWP[ALN_Capsule] = 0;
	AloneCapWP[ALN_Virus] = 0;

	switch(aiSelCom) {
	case 0:					// ������
		// ���[�h���Ƃ̎v�l���[�N�ݒ�
		if ( xpw->gs != GST_GAME ) {								// ���񂾌�̗��K���[�h
				pri_point[LnOnLinVrs] = 0;							// ��ٽ��_���₷��
				pri_point[LnEraseVrs] = 0;							//	 �V
				EraseLinP[1] = 30-80;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
			if ( aipn == 4 ) {
				AloneCapP[ALN_FallCap] = -70-300-500;
				AloneCapP[ALN_FallVrs] = -90-300-500;
				AloneCapP[ALN_Capsule] = -80-350-500;
				AloneCapP[ALN_Virus] = -100-350-500;
			} else {
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
			}
			OnVirusP = 0;
			RensaP = 600+200+200;											// �A�����₷��
			RensaMP = -600+200;											// �A�����₷��
			HeiLinesAllp[2] = 18-4+90-60+150;					// �A����Ԃ����₷���悤�Q���т₷������
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�R���т₷������
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
			HeiLinesAllp[5] = 45-5+90+60+150+150;
			HeiLinesAllp[6] = 45-5+90+60+150+150;
			HeiLinesAllp[7] = 45-5+90+60+150+150;
			HeiLinesAllp[8] = 45-5+90+60+150+150;
			WidLinesAllp[2] = 0;
			WidLinesAllp[3] = 0;
			WidLinesAllp[4] = 0;
			WidLinesAllp[5] = 0;
			WidLinesAllp[6] = 0;
			WidLinesAllp[7] = 0;
			WidLinesAllp[8] = 0;
		} else {
			if ( aiRootP < 4.0 ) {
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 0;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				HeiLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( xpw->vs <= 6 || vrsCnt <= 2 ) {					// �����̳�ٽ�����W�ȉ��ɂȂ������I�Տ���
				pri_point[LnEraseVrs] = 30+2000;
				OnVirusP = 3000;
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					pri_point[LnOnLinVrs] = 15+16+200/2;
					EraseLinP[1] = 30;
					if ( vrsCnt == 0 ) aiWall = wall;
					HeiEraseLinRate = 4.0;
					WidEraseLinRate = 4.0;
					RensaP = 0;
					if ( vrsCnt ) {
						RensaMP = 0;											// �A�����₷��
						AloneCapP[ALN_FallCap] = -70-100;
						AloneCapP[ALN_FallVrs] = -90-100;
						AloneCapP[ALN_Capsule] = -80-150;
						AloneCapP[ALN_Virus] = -100-150;
					} else {
						RensaMP = 0;
						AloneCapP[ALN_FallCap] = -70-100-200;
						AloneCapP[ALN_FallVrs] = -90-100-200;
						AloneCapP[ALN_Capsule] = -80-150-300;
						AloneCapP[ALN_Virus] = -100-150-300;
	//					AloneCapWP[ALN_FallCap] = 300;
	//					AloneCapWP[ALN_FallVrs] = 300;
	//					AloneCapWP[ALN_Capsule] = 350;
	//					AloneCapWP[ALN_Virus] = 350;
					}
					HeiLinesAllp[2] = 18-4;
					HeiLinesAllp[3] = 45-5;
					HeiLinesAllp[4] = 45;
					HeiLinesAllp[5] = 45;
					HeiLinesAllp[6] = 45;
					HeiLinesAllp[7] = 45;
					HeiLinesAllp[8] = 45;
					WidLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 0; //45;
					WidLinesAllp[5] = 0; //45;
					WidLinesAllp[6] = 0; //45;
					WidLinesAllp[7] = 0; //45;
					WidLinesAllp[8] = 0; //45;
				} else {
					pri_point[LnOnLinVrs] = 15+16+400;
					EraseLinP[1] = 30;
					aiWall = wall;
					HeiEraseLinRate = 10.0;
					WidEraseLinRate = 10.0;
					AloneCapP[ALN_FallCap] = -70*1;
					AloneCapP[ALN_FallVrs] = -90*1;
					AloneCapP[ALN_Capsule] = -80*1;
					AloneCapP[ALN_Virus] = -100*1;
					AloneCapWP[ALN_Bottom] = -60*6;
					AloneCapWP[ALN_FallCap] = -70*6;
					AloneCapWP[ALN_FallVrs] = -90*6;
					AloneCapWP[ALN_Capsule] = -80*6;
					AloneCapWP[ALN_Virus] = -100*6;
					RensaP = 0;
					RensaMP = 0;											// �A�����₷��
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 45-5+100;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
	/*		} else if ( (xpw->vs > vrsMax*70/100 && aipn == 4) || badLine == 1 ) {	// �O�����������ɶ�߾ق��c���Ă��鎞
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 500;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				HeiLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
	*/		} else if ( badLine == 2 ) {								// ��߾ٓ�������ɶ�߾ق��c���Ă��鎞
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 0.0;
				WidEraseLinRate = 5.0;
				AloneCapP[ALN_FallCap] = -70*1;
				AloneCapP[ALN_FallVrs] = -90*1;
				AloneCapP[ALN_Capsule] = -80*1;
				AloneCapP[ALN_Virus] = -100*1;
				AloneCapWP[ALN_FallCap] = -70*2;
				AloneCapWP[ALN_FallVrs] = -90*2;
				AloneCapWP[ALN_Capsule] = -80*2;
				AloneCapWP[ALN_Virus] = -100*2;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 0;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5+200;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// ���Տ���
				pri_point[LnOnLinVrs] = 15+16;							// ��ٽ��_���₷��
				pri_point[LnEraseVrs] = 30+20;							//	 �V
				EraseLinP[1] = 30;
				HeiEraseLinRate = 4.0;
				WidEraseLinRate = 4.0;
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
				OnVirusP = 0;
				RensaP = 0;											// �A�����₷��
				RensaMP = 0;											// �A�����₷��
				pri_point[LnHighCaps] = 200;
				pri_point[LnHighVrs] = 200;
				HeiLinesAllp[2] = 18-4;					// �A����Ԃ����₷���悤�Q���т₷������
				HeiLinesAllp[3] = 45-5;							// �A����Ԃ����₷���悤�R���т₷������
				HeiLinesAllp[4] = 45-5;							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
				HeiLinesAllp[5] = 45-5;
				HeiLinesAllp[6] = 45-5;
				HeiLinesAllp[7] = 45-5;
				HeiLinesAllp[8] = 45-5;
				WidLinesAllp[2] = 0;
				WidLinesAllp[3] = 0;
				WidLinesAllp[4] = 0;
				WidLinesAllp[5] = 0;
				WidLinesAllp[6] = 0;
				WidLinesAllp[7] = 0;
				WidLinesAllp[8] = 0;
			}
		}
		break;
	case 1:					// ��A��
		// ���[�h���Ƃ̎v�l���[�N�ݒ�
		if ( xpw->gs != GST_GAME ) {								// ���񂾌�̗��K���[�h
				pri_point[LnOnLinVrs] = 0;							// ��ٽ��_���₷��
				pri_point[LnEraseVrs] = 0;							//	 �V
				EraseLinP[1] = 30-80;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
			if ( aipn == 4 ) {
				AloneCapP[ALN_FallCap] = -70-300-500;
				AloneCapP[ALN_FallVrs] = -90-300-500;
				AloneCapP[ALN_Capsule] = -80-350-500;
				AloneCapP[ALN_Virus] = -100-350-500;
			} else {
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
			}
			OnVirusP = 0;
			RensaP = 600+200+200;											// �A�����₷��
			RensaMP = -600+200;											// �A�����₷��
			HeiLinesAllp[2] = 18-4+90-60+150;					// �A����Ԃ����₷���悤�Q���т₷������
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�R���т₷������
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
			HeiLinesAllp[5] = 45-5+90+60+150+150;
			HeiLinesAllp[6] = 45-5+90+60+150+150;
			HeiLinesAllp[7] = 45-5+90+60+150+150;
			HeiLinesAllp[8] = 45-5+90+60+150+150;
			WidLinesAllp[2] = 0;
			WidLinesAllp[3] = 0;
			WidLinesAllp[4] = 0;
			WidLinesAllp[5] = 0;
			WidLinesAllp[6] = 0;
			WidLinesAllp[7] = 0;
			WidLinesAllp[8] = 0;
		} else {
			if ( aiRootP < 4.0 ) {
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 500;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				HeiLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {					// �����̳�ٽ�����W�ȉ��ɂȂ������I�Տ���
				pri_point[LnEraseVrs] = 30+2000;
				OnVirusP = 3000;
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					pri_point[LnOnLinVrs] = 15+16+200/2;
					EraseLinP[1] = 30;
					if ( vrsCnt == 0 ) aiWall = wall;
					HeiEraseLinRate = 4.0;
					WidEraseLinRate = 4.0;
					RensaP = 200;
					if ( vrsCnt ) {
						RensaMP = -200;											// �A�����₷��
						AloneCapP[ALN_FallCap] = -70-100;
						AloneCapP[ALN_FallVrs] = -90-100;
						AloneCapP[ALN_Capsule] = -80-150;
						AloneCapP[ALN_Virus] = -100-150;
					} else {
						RensaMP = 0;
						AloneCapP[ALN_FallCap] = -70-100-200;
						AloneCapP[ALN_FallVrs] = -90-100-200;
						AloneCapP[ALN_Capsule] = -80-150-300;
						AloneCapP[ALN_Virus] = -100-150-300;
	//					AloneCapWP[ALN_FallCap] = 300;
	//					AloneCapWP[ALN_FallVrs] = 300;
	//					AloneCapWP[ALN_Capsule] = 350;
	//					AloneCapWP[ALN_Virus] = 350;
					}
					HeiLinesAllp[2] = 18-4+50;
					HeiLinesAllp[3] = 45-5+100;
					HeiLinesAllp[4] = 45+100;
					HeiLinesAllp[5] = 45+100;
					HeiLinesAllp[6] = 45+100;
					HeiLinesAllp[7] = 45+100;
					HeiLinesAllp[8] = 45+100;
					WidLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 0; //45;
					WidLinesAllp[5] = 0; //45;
					WidLinesAllp[6] = 0; //45;
					WidLinesAllp[7] = 0; //45;
					WidLinesAllp[8] = 0; //45;
				} else {
					pri_point[LnOnLinVrs] = 15+16+400;
					EraseLinP[1] = 30;
					aiWall = wall;
					HeiEraseLinRate = 10.0;
					WidEraseLinRate = 10.0;
					AloneCapP[ALN_FallCap] = -70*1;
					AloneCapP[ALN_FallVrs] = -90*1;
					AloneCapP[ALN_Capsule] = -80*1;
					AloneCapP[ALN_Virus] = -100*1;
					AloneCapWP[ALN_Bottom] = -60*6;
					AloneCapWP[ALN_FallCap] = -70*6;
					AloneCapWP[ALN_FallVrs] = -90*6;
					AloneCapWP[ALN_Capsule] = -80*6;
					AloneCapWP[ALN_Virus] = -100*6;
					RensaP = 200;
					RensaMP = 0;											// �A�����₷��
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 45-5+100;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
			} else if ( badLine == 2 ) {								// ��߾ٓ�������ɶ�߾ق��c���Ă��鎞
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 0.0;
				WidEraseLinRate = 5.0;
				AloneCapP[ALN_FallCap] = -70*1;
				AloneCapP[ALN_FallVrs] = -90*1;
				AloneCapP[ALN_Capsule] = -80*1;
				AloneCapP[ALN_Virus] = -100*1;
				AloneCapWP[ALN_FallCap] = -70*2;
				AloneCapWP[ALN_FallVrs] = -90*2;
				AloneCapWP[ALN_Capsule] = -80*2;
				AloneCapWP[ALN_Virus] = -100*2;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 200;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5+200;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// ���Տ���
				pri_point[LnOnLinVrs] = 15+16;							// ��ٽ��_���₷��
				pri_point[LnEraseVrs] = 30+20;							//	 �V
				EraseLinP[1] = 30;
				HeiEraseLinRate = 4.0;
				WidEraseLinRate = 4.0;
				AloneCapP[ALN_FallCap] = -70-300+000;
				AloneCapP[ALN_FallVrs] = -90-300+000;
				AloneCapP[ALN_Capsule] = -80-350+000;
				AloneCapP[ALN_Virus] = -100-350+000;
				OnVirusP = 0;
				RensaP = 300;											// �A�����₷��
				if ( badLine == 1 ) {
					RensaMP = 0;											// �A�����₷��
					RensaP = 300+3000;						// �A�����₷�� (+3000 ��������₷���A�����_���悤��)
					pri_point[LnHighCaps] = RensaP/10;
					pri_point[LnHighVrs] = RensaP/10;
				} else {
					RensaMP = -200;											// �A�����₷��
				}
				HeiLinesAllp[2] = 18-4+90-60+100;					// �A����Ԃ����₷���悤�Q���т₷������
				HeiLinesAllp[3] = 45-5+90+60+150;							// �A����Ԃ����₷���悤�R���т₷������
				HeiLinesAllp[4] = 45-5+90+60+150;							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
				HeiLinesAllp[5] = 45-5+90+60+150;
				HeiLinesAllp[6] = 45-5+90+60+150;
				HeiLinesAllp[7] = 45-5+90+60+150;
				HeiLinesAllp[8] = 45-5+90+60+150;
				WidLinesAllp[2] = 0;
				WidLinesAllp[3] = 0;
				WidLinesAllp[4] = 0;
				WidLinesAllp[5] = 0;
				WidLinesAllp[6] = 0;
				WidLinesAllp[7] = 0;
				WidLinesAllp[8] = 0;
			}
		}
		break;
	case 2:					// ���A������
		// ���[�h���Ƃ̎v�l���[�N�ݒ�
		if ( xpw->gs != GST_GAME ) {								// ���񂾌�̗��K���[�h
				pri_point[LnOnLinVrs] = 0;							// ��ٽ��_���₷��
				pri_point[LnEraseVrs] = 0;							//	 �V
				EraseLinP[1] = 30-80;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
			if ( aipn == 4 ) {
				AloneCapP[ALN_FallCap] = -70-300-500;
				AloneCapP[ALN_FallVrs] = -90-300-500;
				AloneCapP[ALN_Capsule] = -80-350-500;
				AloneCapP[ALN_Virus] = -100-350-500;
			} else {
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
			}
			OnVirusP = 0;
			RensaP = 600+200+200;											// �A�����₷��
			RensaMP = -600+200;											// �A�����₷��
			HeiLinesAllp[2] = 18-4+90-60+150;					// �A����Ԃ����₷���悤�Q���т₷������
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�R���т₷������
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
			HeiLinesAllp[5] = 45-5+90+60+150+150;
			HeiLinesAllp[6] = 45-5+90+60+150+150;
			HeiLinesAllp[7] = 45-5+90+60+150+150;
			HeiLinesAllp[8] = 45-5+90+60+150+150;
			WidLinesAllp[2] = 0;
			WidLinesAllp[3] = 0;
			WidLinesAllp[4] = 0;
			WidLinesAllp[5] = 0;
			WidLinesAllp[6] = 0;
			WidLinesAllp[7] = 0;
			WidLinesAllp[8] = 0;
		} else {
			if ( aiRootP < 4.0 ) {
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 500;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				HeiLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {					// �����̳�ٽ�����W�ȉ��ɂȂ������I�Տ���
				pri_point[LnEraseVrs] = 30+2000;
				OnVirusP = 3000;
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					pri_point[LnOnLinVrs] = 15+16+200/2;
					EraseLinP[1] = 30;
					if ( vrsCnt == 0 ) aiWall = wall;
					HeiEraseLinRate = 4.0;
					WidEraseLinRate = 4.0;
					RensaP = 500;
					if ( vrsCnt ) {
						RensaMP = -500;											// �A�����₷��
						AloneCapP[ALN_FallCap] = -70-100;
						AloneCapP[ALN_FallVrs] = -90-100;
						AloneCapP[ALN_Capsule] = -80-150;
						AloneCapP[ALN_Virus] = -100-150;
					} else {
						RensaMP = 0;
						AloneCapP[ALN_FallCap] = -70-100-200;
						AloneCapP[ALN_FallVrs] = -90-100-200;
						AloneCapP[ALN_Capsule] = -80-150-300;
						AloneCapP[ALN_Virus] = -100-150-300;
	//					AloneCapWP[ALN_FallCap] = 300;
	//					AloneCapWP[ALN_FallVrs] = 300;
	//					AloneCapWP[ALN_Capsule] = 350;
	//					AloneCapWP[ALN_Virus] = 350;
					}
					HeiLinesAllp[2] = 18-4+100;
					HeiLinesAllp[3] = 45-5+200;
					HeiLinesAllp[4] = 45+200;
					HeiLinesAllp[5] = 45+200;
					HeiLinesAllp[6] = 45+200;
					HeiLinesAllp[7] = 45+200;
					HeiLinesAllp[8] = 45+200;
					WidLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 0; //45;
					WidLinesAllp[5] = 0; //45;
					WidLinesAllp[6] = 0; //45;
					WidLinesAllp[7] = 0; //45;
					WidLinesAllp[8] = 0; //45;
				} else {
					pri_point[LnOnLinVrs] = 15+16+400;
					EraseLinP[1] = 30;
					aiWall = wall;
					HeiEraseLinRate = 10.0;
					WidEraseLinRate = 10.0;
					AloneCapP[ALN_FallCap] = -70*1;
					AloneCapP[ALN_FallVrs] = -90*1;
					AloneCapP[ALN_Capsule] = -80*1;
					AloneCapP[ALN_Virus] = -100*1;
					AloneCapWP[ALN_Bottom] = -60*6;
					AloneCapWP[ALN_FallCap] = -70*6;
					AloneCapWP[ALN_FallVrs] = -90*6;
					AloneCapWP[ALN_Capsule] = -80*6;
					AloneCapWP[ALN_Virus] = -100*6;
					RensaP = 200;
					RensaMP = 0;											// �A�����₷��
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 45-5+100;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
			} else if ( badLine == 2 ) {								// ��߾ٓ�������ɶ�߾ق��c���Ă��鎞
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 0.0;
				WidEraseLinRate = 5.0;
				AloneCapP[ALN_FallCap] = -70*1;
				AloneCapP[ALN_FallVrs] = -90*1;
				AloneCapP[ALN_Capsule] = -80*1;
				AloneCapP[ALN_Virus] = -100*1;
				AloneCapWP[ALN_FallCap] = -70*2;
				AloneCapWP[ALN_FallVrs] = -90*2;
				AloneCapWP[ALN_Capsule] = -80*2;
				AloneCapWP[ALN_Virus] = -100*2;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 500;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5+200;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// ���Տ���
				pri_point[LnOnLinVrs] = 15+16;							// ��ٽ��_���₷��
				pri_point[LnEraseVrs] = 30+20;							//	 �V
				EraseLinP[1] = 30;
				HeiEraseLinRate = 4.0;
				WidEraseLinRate = 4.0;
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
				OnVirusP = 0;
				RensaP = 600+200+200+400;											// �A�����₷��
				if ( badLine == 1 ) {
					RensaMP = 0;											// �A�����₷��
					RensaP = 600+200+200+400+3000;						// �A�����₷�� (+3000 ��������₷���A�����_���悤��)
					pri_point[LnHighCaps] = RensaP/10;
					pri_point[LnHighVrs] = RensaP/10;
				} else {
					RensaMP = -600-200;											// �A�����₷��
				}
				if ( aipn == 4 && xpw->vs > vrsMax*0/100) {
					HeiLinesAllp[2] = (18-4+90-60+10);					// �A����Ԃ����₷���悤�Q���т₷������
					HeiLinesAllp[3] = (45-5+90+60+10+100);							// �A����Ԃ����₷���悤�R���т₷������
					HeiLinesAllp[4] = (10-5+90-60+10);							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
					HeiLinesAllp[5] = (10-5+90-60+10);
					HeiLinesAllp[6] = (10-5+90-60+10);
					HeiLinesAllp[7] = (10-5+90-60+10);
					HeiLinesAllp[8] = (10-5+90-60+10);
				} else {
					HeiLinesAllp[2] = 18-4+90-60+150;					// �A����Ԃ����₷���悤�Q���т₷������
					HeiLinesAllp[3] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�R���т₷������
					HeiLinesAllp[4] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
					HeiLinesAllp[5] = 45-5+90+60+150+150;
					HeiLinesAllp[6] = 45-5+90+60+150+150;
					HeiLinesAllp[7] = 45-5+90+60+150+150;
					HeiLinesAllp[8] = 45-5+90+60+150+150;
				}
				WidLinesAllp[2] = 0;
				WidLinesAllp[3] = 0;
				WidLinesAllp[4] = 0;
				WidLinesAllp[5] = 0;
				WidLinesAllp[6] = 0;
				WidLinesAllp[7] = 0;
				WidLinesAllp[8] = 0;
			}
		}
		break;
	case 3:					// ���A���_��
		// ���[�h���Ƃ̎v�l���[�N�ݒ�
		if ( xpw->gs != GST_GAME ) {								// ���񂾌�̗��K���[�h
				pri_point[LnOnLinVrs] = 0;							// ��ٽ��_���₷��
				pri_point[LnEraseVrs] = 0;							//	 �V
				EraseLinP[1] = 30-80;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
			if ( aipn == 4 ) {
				AloneCapP[ALN_FallCap] = -70-300-500;
				AloneCapP[ALN_FallVrs] = -90-300-500;
				AloneCapP[ALN_Capsule] = -80-350-500;
				AloneCapP[ALN_Virus] = -100-350-500;
			} else {
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
			}
			OnVirusP = 0;
			RensaP = 600+200+200;											// �A�����₷��
			RensaMP = -600+200;											// �A�����₷��
			HeiLinesAllp[2] = 18-4+90-60+150;					// �A����Ԃ����₷���悤�Q���т₷������
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�R���т₷������
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
			HeiLinesAllp[5] = 45-5+90+60+150+150;
			HeiLinesAllp[6] = 45-5+90+60+150+150;
			HeiLinesAllp[7] = 45-5+90+60+150+150;
			HeiLinesAllp[8] = 45-5+90+60+150+150;
			WidLinesAllp[2] = 0;
			WidLinesAllp[3] = 0;
			WidLinesAllp[4] = 0;
			WidLinesAllp[5] = 0;
			WidLinesAllp[6] = 0;
			WidLinesAllp[7] = 0;
			WidLinesAllp[8] = 0;
		} else {
			if ( aiRootP < 4.0 ) {
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 500;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				HeiLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {					// �����̳�ٽ�����W�ȉ��ɂȂ������I�Տ���
				pri_point[LnEraseVrs] = 30+2000;
				OnVirusP = 3000;
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					pri_point[LnOnLinVrs] = 15+16+200/2;
					EraseLinP[1] = 30;
					if ( vrsCnt == 0 ) aiWall = wall;
					HeiEraseLinRate = 4.0;
					WidEraseLinRate = 4.0;
					RensaP = 500;
					if ( vrsCnt ) {
						RensaMP = -500;											// �A�����₷��
						AloneCapP[ALN_FallCap] = -70-100;
						AloneCapP[ALN_FallVrs] = -90-100;
						AloneCapP[ALN_Capsule] = -80-150;
						AloneCapP[ALN_Virus] = -100-150;
					} else {
						RensaMP = 0;
						AloneCapP[ALN_FallCap] = -70-100-200;
						AloneCapP[ALN_FallVrs] = -90-100-200;
						AloneCapP[ALN_Capsule] = -80-150-300;
						AloneCapP[ALN_Virus] = -100-150-300;
	//					AloneCapWP[ALN_FallCap] = 300;
	//					AloneCapWP[ALN_FallVrs] = 300;
	//					AloneCapWP[ALN_Capsule] = 350;
	//					AloneCapWP[ALN_Virus] = 350;
					}
					HeiLinesAllp[2] = 18-4+100;
					HeiLinesAllp[3] = 45-5+200;
					HeiLinesAllp[4] = 45+200;
					HeiLinesAllp[5] = 45+200;
					HeiLinesAllp[6] = 45+200;
					HeiLinesAllp[7] = 45+200;
					HeiLinesAllp[8] = 45+200;
					WidLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 0; //45;
					WidLinesAllp[5] = 0; //45;
					WidLinesAllp[6] = 0; //45;
					WidLinesAllp[7] = 0; //45;
					WidLinesAllp[8] = 0; //45;
				} else {
					pri_point[LnOnLinVrs] = 15+16+400;
					EraseLinP[1] = 30;
					aiWall = wall;
					HeiEraseLinRate = 10.0;
					WidEraseLinRate = 10.0;
					AloneCapP[ALN_FallCap] = -70*1;
					AloneCapP[ALN_FallVrs] = -90*1;
					AloneCapP[ALN_Capsule] = -80*1;
					AloneCapP[ALN_Virus] = -100*1;
					AloneCapWP[ALN_Bottom] = -60*6;
					AloneCapWP[ALN_FallCap] = -70*6;
					AloneCapWP[ALN_FallVrs] = -90*6;
					AloneCapWP[ALN_Capsule] = -80*6;
					AloneCapWP[ALN_Virus] = -100*6;
					RensaP = 200;
					RensaMP = 0;											// �A�����₷��
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// �Q�����F�����낤���ɑ�������߲��
					WidLinesAllp[3] = 45-5+100;									// �R�����F�����낤���ɑ�������߲��
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
	/*		} else if ( (xpw->vs > vrsMax*70/100 && aipn == 4) || badLine == 1 ) {	// �O�����������ɶ�߾ق��c���Ă��鎞
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 500;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				HeiLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
	*/		} else if ( badLine == 2 ) {								// ��߾ٓ�������ɶ�߾ق��c���Ă��鎞
				pri_point[LnOnLinVrs] = 15-10;							// �����悤�Ƃ��Ă���ײݏ�ɳ�ٽ�����鎞�ɑ�������߲��
				pri_point[LnEraseVrs] = 30-20;							// �����ł��鳨ٽ�����鎞�ɑ�������߲��
				EraseLinP[1] = 30;
				HeiEraseLinRate = 0.0;
				WidEraseLinRate = 5.0;
				AloneCapP[ALN_FallCap] = -70*1;
				AloneCapP[ALN_FallVrs] = -90*1;
				AloneCapP[ALN_Capsule] = -80*1;
				AloneCapP[ALN_Virus] = -100*1;
				AloneCapWP[ALN_FallCap] = -70*2;
				AloneCapWP[ALN_FallVrs] = -90*2;
				AloneCapWP[ALN_Capsule] = -80*2;
				AloneCapWP[ALN_Virus] = -100*2;
				OnVirusP = 0;											// ��ٽ�̏�̕��ɂ��鎞�ɑ������
				RensaP = 500;											// �A���������ɑ�������߲��
				RensaMP = 0;											// �A�����₷��
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// �Q�����F�����낤���ɑ�������߲��
				HeiLinesAllp[3] = 0; //45-5;									// �R�����F�����낤���ɑ�������߲��
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// �Q�����F�����낤���ɑ�������߲��
				WidLinesAllp[3] = 45-5+200;									// �R�����F�����낤���ɑ�������߲��
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// ���Տ���
				if ( aipn == 4 && xpw->vs > vrsMax*0/100 ) {
					pri_point[LnOnLinVrs] = 15+16;							// ��ٽ��_���₷��
					pri_point[LnEraseVrs] = 30+20+50;							//	 �V
				} else {
					if ( xpw->vs > vrsMax*20/100 ) {
						pri_point[LnOnLinVrs] = 0;							// ��ٽ��_���₷��
						pri_point[LnEraseVrs] = 0;							//	 �V
					} else {
						pri_point[LnOnLinVrs] = 15+16;							// ��ٽ��_���₷��
						pri_point[LnEraseVrs] = 30+20;							//	 �V
					}
				}
				EraseLinP[1] = 30-80;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
				OnVirusP = 0;
				RensaP = 600+200+200+400;											// �A�����₷��
				if ( badLine == 1 ) {
					RensaMP = 0;											// �A�����₷��
					RensaP = 600+200+200+400+3000;						// �A�����₷�� (+3000 ��������₷���A�����_���悤��)
					pri_point[LnHighCaps] = RensaP/10;
					pri_point[LnHighVrs] = RensaP/10;
				} else {
					RensaMP = -600-200;											// �A�����₷��
				}
				if ( aipn == 4 && xpw->vs > vrsMax*0/100) {
					HeiLinesAllp[2] = (18-4+90-60+10);					// �A����Ԃ����₷���悤�Q���т₷������
					HeiLinesAllp[3] = (45-5+90+60+10+100);							// �A����Ԃ����₷���悤�R���т₷������
					HeiLinesAllp[4] = (10-5+90-60+10);							// �A����Ԃ����₷���悤�S�ȏ�͕��тɂ�������
					HeiLinesAllp[5] = (10-5+90-60+10);
					HeiLinesAllp[6] = (10-5+90-60+10);
					HeiLinesAllp[7] = (10-5+90-60+10);
					HeiLinesAllp[8] = (10-5+90-60+10);
				} else {
					HeiLinesAllp[2] = 18-4+90-60+150;					// �A����Ԃ����₷���悤�Q���т₷������
					HeiLinesAllp[3] = 45-5+90+60+150+150;							// �A����Ԃ����₷���悤�R���т₷������
					HeiLinesAllp[4] = 45-5+90+60+150+150+500;
					HeiLinesAllp[5] = 45-5+90+60+150+150+500;
					HeiLinesAllp[6] = 45-5+90+60+150+150+500;
					HeiLinesAllp[7] = 45-5+90+60+150+150+500;
					HeiLinesAllp[8] = 45-5+90+60+150+150+500;
				}
				WidLinesAllp[2] = 0;
				WidLinesAllp[3] = 0;
				WidLinesAllp[4] = 0;
				WidLinesAllp[5] = 0;
				WidLinesAllp[6] = 0;
				WidLinesAllp[7] = 0;
				WidLinesAllp[8] = 0;
			}
		}
		break;
	}
#endif

