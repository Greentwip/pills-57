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
//						pri_point[LnOnLinVrs] = 15+16;							// ｳｨﾙｽを狙いやすく
//						pri_point[LnEraseVrs] = 30+20;							//	 〃
};

#if 0
	// ワーク初期化
	pri_point[LnHighCaps] = 30;
	pri_point[LnHighVrs] = 30;
	aiWall = 0;
	AloneCapWP[ALN_Bottom] = 0;
	AloneCapWP[ALN_FallCap] = 0;
	AloneCapWP[ALN_FallVrs] = 0;
	AloneCapWP[ALN_Capsule] = 0;
	AloneCapWP[ALN_Virus] = 0;

	switch(aiSelCom) {
	case 0:					// 早消し
		// モードごとの思考ワーク設定
		if ( xpw->gs != GST_GAME ) {								// 死んだ後の練習モード
				pri_point[LnOnLinVrs] = 0;							// ｳｨﾙｽを狙いやすく
				pri_point[LnEraseVrs] = 0;							//	 〃
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
			RensaP = 600+200+200;											// 連鎖しやすく
			RensaMP = -600+200;											// 連鎖しやすく
			HeiLinesAllp[2] = 18-4+90-60+150;					// 連鎖状態を作りやすいよう２個並びやすくする
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう３個並びやすくする
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう４個以上は並びにくくする
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
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 0;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				HeiLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( xpw->vs <= 6 || vrsCnt <= 2 ) {					// 中央のｳｨﾙｽ数が８以下になった時終盤処理
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
						RensaMP = 0;											// 連鎖しやすく
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
					WidLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
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
					RensaMP = 0;											// 連鎖しやすく
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 45-5+100;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
	/*		} else if ( (xpw->vs > vrsMax*70/100 && aipn == 4) || badLine == 1 ) {	// 前半＆高い所にｶﾌﾟｾﾙが残っている時
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 500;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				HeiLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
	*/		} else if ( badLine == 2 ) {								// ｶﾌﾟｾﾙ入り口下にｶﾌﾟｾﾙが残っている時
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
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
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 0;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5+200;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// 中盤処理
				pri_point[LnOnLinVrs] = 15+16;							// ｳｨﾙｽを狙いやすく
				pri_point[LnEraseVrs] = 30+20;							//	 〃
				EraseLinP[1] = 30;
				HeiEraseLinRate = 4.0;
				WidEraseLinRate = 4.0;
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
				OnVirusP = 0;
				RensaP = 0;											// 連鎖しやすく
				RensaMP = 0;											// 連鎖しやすく
				pri_point[LnHighCaps] = 200;
				pri_point[LnHighVrs] = 200;
				HeiLinesAllp[2] = 18-4;					// 連鎖状態を作りやすいよう２個並びやすくする
				HeiLinesAllp[3] = 45-5;							// 連鎖状態を作りやすいよう３個並びやすくする
				HeiLinesAllp[4] = 45-5;							// 連鎖状態を作りやすいよう４個以上は並びにくくする
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
	case 1:					// 弱連鎖
		// モードごとの思考ワーク設定
		if ( xpw->gs != GST_GAME ) {								// 死んだ後の練習モード
				pri_point[LnOnLinVrs] = 0;							// ｳｨﾙｽを狙いやすく
				pri_point[LnEraseVrs] = 0;							//	 〃
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
			RensaP = 600+200+200;											// 連鎖しやすく
			RensaMP = -600+200;											// 連鎖しやすく
			HeiLinesAllp[2] = 18-4+90-60+150;					// 連鎖状態を作りやすいよう２個並びやすくする
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう３個並びやすくする
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう４個以上は並びにくくする
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
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 500;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				HeiLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {					// 中央のｳｨﾙｽ数が８以下になった時終盤処理
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
						RensaMP = -200;											// 連鎖しやすく
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
					WidLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
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
					RensaMP = 0;											// 連鎖しやすく
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 45-5+100;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
			} else if ( badLine == 2 ) {								// ｶﾌﾟｾﾙ入り口下にｶﾌﾟｾﾙが残っている時
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
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
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 200;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5+200;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// 中盤処理
				pri_point[LnOnLinVrs] = 15+16;							// ｳｨﾙｽを狙いやすく
				pri_point[LnEraseVrs] = 30+20;							//	 〃
				EraseLinP[1] = 30;
				HeiEraseLinRate = 4.0;
				WidEraseLinRate = 4.0;
				AloneCapP[ALN_FallCap] = -70-300+000;
				AloneCapP[ALN_FallVrs] = -90-300+000;
				AloneCapP[ALN_Capsule] = -80-350+000;
				AloneCapP[ALN_Virus] = -100-350+000;
				OnVirusP = 0;
				RensaP = 300;											// 連鎖しやすく
				if ( badLine == 1 ) {
					RensaMP = 0;											// 連鎖しやすく
					RensaP = 300+3000;						// 連鎖しやすく (+3000 上も消しやすく連鎖も狙うように)
					pri_point[LnHighCaps] = RensaP/10;
					pri_point[LnHighVrs] = RensaP/10;
				} else {
					RensaMP = -200;											// 連鎖しやすく
				}
				HeiLinesAllp[2] = 18-4+90-60+100;					// 連鎖状態を作りやすいよう２個並びやすくする
				HeiLinesAllp[3] = 45-5+90+60+150;							// 連鎖状態を作りやすいよう３個並びやすくする
				HeiLinesAllp[4] = 45-5+90+60+150;							// 連鎖状態を作りやすいよう４個以上は並びにくくする
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
	case 2:					// 強連鎖消し
		// モードごとの思考ワーク設定
		if ( xpw->gs != GST_GAME ) {								// 死んだ後の練習モード
				pri_point[LnOnLinVrs] = 0;							// ｳｨﾙｽを狙いやすく
				pri_point[LnEraseVrs] = 0;							//	 〃
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
			RensaP = 600+200+200;											// 連鎖しやすく
			RensaMP = -600+200;											// 連鎖しやすく
			HeiLinesAllp[2] = 18-4+90-60+150;					// 連鎖状態を作りやすいよう２個並びやすくする
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう３個並びやすくする
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう４個以上は並びにくくする
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
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 500;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				HeiLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {					// 中央のｳｨﾙｽ数が８以下になった時終盤処理
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
						RensaMP = -500;											// 連鎖しやすく
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
					WidLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
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
					RensaMP = 0;											// 連鎖しやすく
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 45-5+100;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
			} else if ( badLine == 2 ) {								// ｶﾌﾟｾﾙ入り口下にｶﾌﾟｾﾙが残っている時
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
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
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 500;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5+200;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// 中盤処理
				pri_point[LnOnLinVrs] = 15+16;							// ｳｨﾙｽを狙いやすく
				pri_point[LnEraseVrs] = 30+20;							//	 〃
				EraseLinP[1] = 30;
				HeiEraseLinRate = 4.0;
				WidEraseLinRate = 4.0;
				AloneCapP[ALN_FallCap] = -70-300+200;
				AloneCapP[ALN_FallVrs] = -90-300+200;
				AloneCapP[ALN_Capsule] = -80-350+200;
				AloneCapP[ALN_Virus] = -100-350+200;
				OnVirusP = 0;
				RensaP = 600+200+200+400;											// 連鎖しやすく
				if ( badLine == 1 ) {
					RensaMP = 0;											// 連鎖しやすく
					RensaP = 600+200+200+400+3000;						// 連鎖しやすく (+3000 上も消しやすく連鎖も狙うように)
					pri_point[LnHighCaps] = RensaP/10;
					pri_point[LnHighVrs] = RensaP/10;
				} else {
					RensaMP = -600-200;											// 連鎖しやすく
				}
				if ( aipn == 4 && xpw->vs > vrsMax*0/100) {
					HeiLinesAllp[2] = (18-4+90-60+10);					// 連鎖状態を作りやすいよう２個並びやすくする
					HeiLinesAllp[3] = (45-5+90+60+10+100);							// 連鎖状態を作りやすいよう３個並びやすくする
					HeiLinesAllp[4] = (10-5+90-60+10);							// 連鎖状態を作りやすいよう４個以上は並びにくくする
					HeiLinesAllp[5] = (10-5+90-60+10);
					HeiLinesAllp[6] = (10-5+90-60+10);
					HeiLinesAllp[7] = (10-5+90-60+10);
					HeiLinesAllp[8] = (10-5+90-60+10);
				} else {
					HeiLinesAllp[2] = 18-4+90-60+150;					// 連鎖状態を作りやすいよう２個並びやすくする
					HeiLinesAllp[3] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう３個並びやすくする
					HeiLinesAllp[4] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう４個以上は並びにくくする
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
	case 3:					// 強連鎖狙い
		// モードごとの思考ワーク設定
		if ( xpw->gs != GST_GAME ) {								// 死んだ後の練習モード
				pri_point[LnOnLinVrs] = 0;							// ｳｨﾙｽを狙いやすく
				pri_point[LnEraseVrs] = 0;							//	 〃
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
			RensaP = 600+200+200;											// 連鎖しやすく
			RensaMP = -600+200;											// 連鎖しやすく
			HeiLinesAllp[2] = 18-4+90-60+150;					// 連鎖状態を作りやすいよう２個並びやすくする
			HeiLinesAllp[3] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう３個並びやすくする
			HeiLinesAllp[4] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう４個以上は並びにくくする
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
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 500;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				HeiLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
			} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {					// 中央のｳｨﾙｽ数が８以下になった時終盤処理
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
						RensaMP = -500;											// 連鎖しやすく
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
					WidLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
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
					RensaMP = 0;											// 連鎖しやすく
					HeiLinesAllp[2] = 0; //18-4;
					HeiLinesAllp[3] = 0; //45-5;
					HeiLinesAllp[4] = 0; //45;
					HeiLinesAllp[5] = 0; //45;
					HeiLinesAllp[6] = 0; //45;
					HeiLinesAllp[7] = 0; //45;
					HeiLinesAllp[8] = 0; //45;
					WidLinesAllp[2] = 18-4+50;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[3] = 45-5+100;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
					WidLinesAllp[4] = 45+100;
					WidLinesAllp[5] = 45+100;
					WidLinesAllp[6] = 45;
					WidLinesAllp[7] = 45;
					WidLinesAllp[8] = 45;
				}
	/*		} else if ( (xpw->vs > vrsMax*70/100 && aipn == 4) || badLine == 1 ) {	// 前半＆高い所にｶﾌﾟｾﾙが残っている時
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				EraseLinP[1] = 30;
				HeiEraseLinRate = 1.0;
				WidEraseLinRate = 1.0;
				AloneCapP[ALN_FallCap] = -70;
				AloneCapP[ALN_FallVrs] = -90;
				AloneCapP[ALN_Capsule] = -80;
				AloneCapP[ALN_Virus] = -100;
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 500;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				HeiLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 45;
				HeiLinesAllp[5] = 45;
				HeiLinesAllp[6] = 45;
				HeiLinesAllp[7] = 45;
				HeiLinesAllp[8] = 45;
				WidLinesAllp[2] = 18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45;
				WidLinesAllp[5] = 45;
				WidLinesAllp[6] = 45;
				WidLinesAllp[7] = 45;
				WidLinesAllp[8] = 45;
	*/		} else if ( badLine == 2 ) {								// ｶﾌﾟｾﾙ入り口下にｶﾌﾟｾﾙが残っている時
				pri_point[LnOnLinVrs] = 15-10;							// 揃えようとしているﾗｲﾝ上にｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
				pri_point[LnEraseVrs] = 30-20;							// 消去できるｳｨﾙｽがある時に足されるﾎﾟｲﾝﾄ
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
				OnVirusP = 0;											// ｳｨﾙｽの上の方にある時に足される
				RensaP = 500;											// 連鎖した時に足されるﾎﾟｲﾝﾄ
				RensaMP = 0;											// 連鎖しやすく
				pri_point[LnHighCaps] = 50;
				pri_point[LnHighVrs] = 50;
				HeiLinesAllp[2] = 0; //18-4;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[3] = 0; //45-5;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				HeiLinesAllp[4] = 0; //45;
				HeiLinesAllp[5] = 0; //45;
				HeiLinesAllp[6] = 0; //45;
				HeiLinesAllp[7] = 0; //45;
				HeiLinesAllp[8] = 0; //45;
				WidLinesAllp[2] = 18-4+100;									// ２個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[3] = 45-5+200;									// ３個同じ色がそろう時に足されるﾎﾟｲﾝﾄ
				WidLinesAllp[4] = 45+200;
				WidLinesAllp[5] = 45+200;
				WidLinesAllp[6] = 45+200;
				WidLinesAllp[7] = 45+200;
				WidLinesAllp[8] = 45+200;
			} else {											// 中盤処理
				if ( aipn == 4 && xpw->vs > vrsMax*0/100 ) {
					pri_point[LnOnLinVrs] = 15+16;							// ｳｨﾙｽを狙いやすく
					pri_point[LnEraseVrs] = 30+20+50;							//	 〃
				} else {
					if ( xpw->vs > vrsMax*20/100 ) {
						pri_point[LnOnLinVrs] = 0;							// ｳｨﾙｽを狙いやすく
						pri_point[LnEraseVrs] = 0;							//	 〃
					} else {
						pri_point[LnOnLinVrs] = 15+16;							// ｳｨﾙｽを狙いやすく
						pri_point[LnEraseVrs] = 30+20;							//	 〃
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
				RensaP = 600+200+200+400;											// 連鎖しやすく
				if ( badLine == 1 ) {
					RensaMP = 0;											// 連鎖しやすく
					RensaP = 600+200+200+400+3000;						// 連鎖しやすく (+3000 上も消しやすく連鎖も狙うように)
					pri_point[LnHighCaps] = RensaP/10;
					pri_point[LnHighVrs] = RensaP/10;
				} else {
					RensaMP = -600-200;											// 連鎖しやすく
				}
				if ( aipn == 4 && xpw->vs > vrsMax*0/100) {
					HeiLinesAllp[2] = (18-4+90-60+10);					// 連鎖状態を作りやすいよう２個並びやすくする
					HeiLinesAllp[3] = (45-5+90+60+10+100);							// 連鎖状態を作りやすいよう３個並びやすくする
					HeiLinesAllp[4] = (10-5+90-60+10);							// 連鎖状態を作りやすいよう４個以上は並びにくくする
					HeiLinesAllp[5] = (10-5+90-60+10);
					HeiLinesAllp[6] = (10-5+90-60+10);
					HeiLinesAllp[7] = (10-5+90-60+10);
					HeiLinesAllp[8] = (10-5+90-60+10);
				} else {
					HeiLinesAllp[2] = 18-4+90-60+150;					// 連鎖状態を作りやすいよう２個並びやすくする
					HeiLinesAllp[3] = 45-5+90+60+150+150;							// 連鎖状態を作りやすいよう３個並びやすくする
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

