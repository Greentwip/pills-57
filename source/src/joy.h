#ifndef _JOY_H_
#define _JOY_H_

#define CONTROLLER_THREAD_PRI  115

extern OSContStatus joystt[MAXCONTROLLERS];	 	// ｺﾝﾄﾛｰﾗｽﾃｰﾀｽ
extern OSContPad    joypad[MAXCONTROLLERS];	 	//       ﾃﾞｰﾀ
extern OSContPad   *joyptr[MAXCONTROLLERS];	 	//       ﾃﾞｰﾀﾎﾟｲﾝﾀ
extern u16			joynew[MAXCONTROLLERS + 1];	//       最新
extern u16			joyold[MAXCONTROLLERS + 1];	//       前回
extern u16			joyupd[MAXCONTROLLERS + 1];	//       ﾄﾘｶﾞ
extern u16			joycur[MAXCONTROLLERS + 1];	//       ﾀ...ﾀﾀﾀ
extern u16			joyflg[MAXCONTROLLERS];	 	// joycur,joycnt 起動ﾄﾘｶﾞ
extern u16			joycnt[MAXCONTROLLERS][16];	// sabccccudlrzLR
extern u16			joycur1,joycur2;			// ｶｰｿﾙﾀｲﾐﾝｸﾞ1,2
extern int			joynum;						//       no.(ｲﾝﾃﾞｸｽ)
extern u8			joygmf[MAXCONTROLLERS];	 	// ｹﾞｰﾑ用ﾌﾗｸﾞ
extern u16			joygam[MAXCONTROLLERS];	 	// ｹﾞｰﾑ用
#ifdef STICK_3D_ON
extern u8			joystk;			 			// ｼﾞｮｲｽﾃｨｯｸの十字ｷｰ化
#endif
extern int GameHalt;

/*----- 渡辺追加分 -----*/
extern	u8			main_joy[MAXCONTROLLERS];	//	メインの動作をさせるコントローラー番号
extern	u8			link_joy[MAXCONTROLLERS];	//	接続状態のフラグ
/*----------------------*/


extern int  joyInit(int);				// initialize controller
extern void joyReadStart(void);  		// controller read start
extern int  joyProcCore(void);
extern void joyCursorFastSet(u16,u8);

/*----- 渡辺追加分 -----*/
extern int joyResponseCheck(void);
/*----------------------*/

#endif // _JOY_H_
