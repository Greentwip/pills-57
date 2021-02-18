
#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>
#include "nnsched.h"
#include "def.h"
#include "joy.h"
#include "message.h"
#include "osHalt.h"
#include "debug.h"

#define CONT_HALT_ON (U_JPAD | D_JPAD)
#define CONT_HALT_OFF (L_JPAD | R_JPAD)

OSContStatus joystt[MAXCONTROLLERS];	 // ｺﾝﾄﾛｰﾗｽﾃｰﾀｽ
OSContPad    joypad[MAXCONTROLLERS];	 //       ﾃﾞｰﾀ
OSContPad   *joyptr[MAXCONTROLLERS];	 //       ﾃﾞｰﾀﾎﾟｲﾝﾀ
u16			 joynew[MAXCONTROLLERS + 1]; //       最新
u16			 joyold[MAXCONTROLLERS + 1]; //       前回
u16			 joyupd[MAXCONTROLLERS + 1]; //       ﾄﾘｶﾞ
u16			 joycur[MAXCONTROLLERS + 1]; //       ﾀ...ﾀﾀﾀ
u16			 joyflg[MAXCONTROLLERS];	 // joycur,joycnt 起動ﾄﾘｶﾞ
u16			 joycnt[MAXCONTROLLERS][16]; // sabccccudlrzLR
u16			 joycur1,joycur2;			 // ｶｰｿﾙﾀｲﾐﾝｸﾞ1,2
int          joynum;					 //       no.(ｲﾝﾃﾞｸｽ)
u8			 joygmf[MAXCONTROLLERS];	 // ｹﾞｰﾑ用ﾌﾗｸﾞ
u16			 joygam[MAXCONTROLLERS];	 // ｹﾞｰﾑ用

/*----- 渡辺追加分 -----*/
u8			main_joy[MAXCONTROLLERS];	//	メインの動作をさせるコントローラー番号
u8			link_joy[MAXCONTROLLERS];	//	接続状態のフラグ
/*----------------------*/

#ifdef STICK_3D_ON
u8			 joystk = FALSE;			 // ｼﾞｮｲｽﾃｨｯｸの十字ｷｰ化
#endif

OSMesgQueue 	joyMsgQ;	// ﾒｯｾｰｼﾞｷｭｰ
OSMesg      	joyMsgBuf;	//       ﾊﾞｯﾌｧ

int GameHalt = FALSE;

//////////////////////////////////////////////////////////////////////////////
// ｺﾝﾄﾛｰﾗﾃﾞｰﾀ読み込み初期設定
int joyInit(int maxjoys)
{
	u8          pattern;
	OSMesgQueue serialMsgQ;
	OSMesg      serialMsg;
	int i,j;

	osCreateMesgQueue(&serialMsgQ, &serialMsg, 1);			// ﾒｯｾｰｼﾞQ初期化
	osSetEventMesg(OS_EVENT_SI, &serialMsgQ, (OSMesg)0);	// ﾒｯｾｰｼﾞｷｭｰをSIｲﾍﾞﾝﾄと関連付ける( OS_EVENT_SIｲﾍﾞﾝﾄに反応する )
	osContSetCh(MAXCONTROLLERS);
	osContInit(&serialMsgQ, &pattern, &joystt[0]);
	osCreateMesgQueue(&joyMsgQ, &joyMsgBuf, 1);
	osSetEventMesg(OS_EVENT_SI, &joyMsgQ, (OSMesg)0);

	for(i = 0;i < MAXCONTROLLERS + 1;i++){
		joynew[i] = joyold[i] = joyupd[i] = joycur[i] = 0;
	}

	for(i=0;i<MAXCONTROLLERS;i++){
		for(j=0;j<16;j++) joycnt[i][j] = 0;
		joygmf[i] = joygam[i] = joyflg[i] = 0;
	}
	joycur1 = 20;
	joycur2 = 4;

#ifdef JOY_PADCOUNT_ON
	joynum = 0;
	for(i=0;i<MAXCONTROLLERS;i++){
		if((pattern & (1<<i)) && !(joystt[i].errno & CONT_NO_RESPONSE_ERROR)) {
			joyptr[i] = &joypad[i];
			joynum++;
			if(joynum == maxjoys) return joynum;
		}
	}
	return joynum;
#else
	return MAXCONTROLLERS;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// ｺﾝﾄﾛｰﾗ読み込みｽﾚｯﾄﾞ,Core
int joyProcCore( void )
{
	int halt = GameHalt, ignore = FALSE;
	u16 i,j,f;
	int jx,jy;
	u16 button;

#ifdef NN_SC_PERF
	nnScPushPerfMeter(NN_SC_PERF_JOY);
#endif

	osContStartReadData(&joyMsgQ);
	(void)osRecvMesg(&joyMsgQ, NULL, OS_MESG_BLOCK);
	osContGetReadData(joypad);

#if defined(USE_HALT)
	// HALT処理
	for (i=0; i<MAXCONTROLLERS; i++) {
		// 
#if defined(DEBUG)
		if(joypad[i].stick_y > 40) {
#else
		if((joypad[i].button & CONT_HALT_ON) == CONT_HALT_ON) {
#endif
			halt = TRUE;
			ignore = TRUE;
		}
#if defined(DEBUG)
		else if(joypad[i].stick_y < -40) {
#else
		else if((joypad[i].button & CONT_HALT_OFF) == CONT_HALT_OFF) {
#endif
			halt = FALSE;
			ignore = TRUE;
		}
	}

	// HALT処理が発生した場合、パッドの入力を無効にする。
	if(ignore) {
		for(i = 0; i < MAXCONTROLLERS; i++) {
			joypad[i].button = 0;
			joypad[i].stick_x = 0;
			joypad[i].stick_y = 0;
		}
	}
#endif // USE_HALT

	for (i=0; i<MAXCONTROLLERS; i++) {
		// joynew,joyupd 設定
#ifdef JOY_PADCOUNT_ON
		joynew[i] = joyptr[i]->button;
#else
		joynew[i] = joypad[i].button;
#endif

#ifdef STICK_3D_ON
		if( joystk ) {
			jx=joyptr[i]->stick_x;
			jy=joyptr[i]->stick_y;
			if( jx < 0 ) jx*=-1;
			if( jy < 0 ) jy*=-1;
			if( jy >= jx ) {
				if( ( joyptr[i]->stick_y/8 )>0 ) joynew[i] |= U_JPAD;
				if( ( joyptr[i]->stick_y/8 )<0 ) joynew[i] |= D_JPAD;
			} else {
				if( ( joyptr[i]->stick_x/8 )<0 ) joynew[i] |= L_JPAD;
				if( ( joyptr[i]->stick_x/8 )>0 ) joynew[i] |= R_JPAD;
			}
		}
#endif
		joyupd[i] = ( joynew[i] | joyold[i] )&(~joyold[i]);
		joycur[i] = 0;

		// joycur 設定
		for(j=0,f=0x8000;j<16;j++,f>>=1) {
			if( f & joyflg[i] ) {
				if( joynew[i] & f ) {
					joycnt[i][j]++;
					if(joycnt[i][j] == 1
					||(joycnt[i][j] >= joycur1 && ( joycnt[i][j] - joycur1 ) % joycur2 == 0 ))
					{
						joycur[i] |= f;
					}
				}
				else {
					joycnt[i][j] = 0;
				}
			}
		}

		// 後処理
		joyold[i] = joynew[i];
	}

	i = 0;
	if(halt) i++;
	if(GameHalt) i++;

	if(i == 1) {
		GameHalt = halt;
		osHaltSet(GameHalt);
	}

#ifdef NN_SC_PERF
	nnScPopPerfMeter();
#endif
}

// 指定されたｶｰｿﾙｶｳﾝﾀをTa...Taの前数値に再設定
void joyCursorFastSet
(
	u16	button,	// ﾎﾞﾀﾝﾌﾗｸﾞ
	u8	index	// joy???[ｲﾝﾃﾞｸｽ]
){
	int	i,j=(u16)button;
	for(i=15;i>=0;i--) {
		if( j & 1 ) break;
		j>>=1;
	}
	joycnt[index][i]=joycur1+joycur2-1;
}

/*---------- 渡辺追加分 ----------*/

/*--------------------------------------
	コントローラー接続状況確認関数
--------------------------------------*/
int	joyResponseCheck(void)
{
	OSContStatus	joyStatus[MAXCONTROLLERS];
	int	max_cont = 0;
	s8	i,j;

	//	コントローラの接続状況の確認命令の発行
	osContStartQuery(&joyMsgQ);

	//	メッセージ待ち
	(void)osRecvMesg(&joyMsgQ, NULL, OS_MESG_BLOCK);

	//	コントローラの接続状況の取得
	osContGetQuery(joyStatus);

	for(i = 0;i < MAXCONTROLLERS;i++){
		main_joy[i] = 4;
	}
	for(i = j = 0;i < MAXCONTROLLERS;i++)
	{
		link_joy[i] = 0;
		if( joyStatus[i].errno != CONT_NO_RESPONSE_ERROR ){
			//	コントローラー接続あり
			if( (joyStatus[i].type & CONT_TYPE_MASK) == CONT_TYPE_NORMAL)
			{
				//	標準コントローラだった
				link_joy[i] = 1;
				max_cont ++;
				main_joy[j] = i;
				j++;
			}
		}
	}

	return	max_cont;
}

/*---------- 渡辺追加分 ----------*/

