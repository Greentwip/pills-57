/**********************************************************

	ai	of	dr.mario	with	vs.	cpu

**********************************************************/
#include <ultra64.h>
#include "evsworks.h"
#include "aiset.h"
#include "aidata.h"
#include "nnsched.h"
#include "vr_init.h"
#include "dm_game_main.h"
#include "joy.h"


extern	u32		framecont;
// あほフラグ   TRUE=あほ  通常はFALSE
//aifGameInit(); の後に設定すること
int		fool_mode;
// 超ハードフラグ	TRUE=超ハード  通常はFALSE
//aifGameInit(); の後に設定すること
int		s_hard_mode;
// 失敗させる割合 1/1000[%]単位
u32		MissRate;
u32		PlayTime;

// フラッシュ用ワーク
static	int		flash_special(void);
static	game_state		*pGameState;
static	int				delpos_tbl[100];
static	int				delpos_cnt;
static	int				OnVirusP_org;
static	int				last_flash;
static int flash_virus(int x, int y);


/*
if ( evs_gamemode == GMD_FLASH ) {
	//フラッシュモード？

          [ 各フラッシュウィルスへのインデックス(0 ~ flash_virus_count-1) ]
          [ 0:X座標, 1:Y座標, 2:色(0~2, -1で消滅状態) ]

	pGameState->flash_virus_pos[i][0];	// X
	pGameState->flash_virus_pos[i][1];	// Y
	pGameState->flash_virus_pos[i][2];	// Color 0: 1: 2: -1:消滅
	pGameState->flash_virus_count;		// 数
*/



/***********************************************
	define
***********************************************/

#define FIELDX		8		// ﾌｨｰﾙﾄﾞｻｲｽﾞ（上の１ラインは常に空白）
#define FIELDY		16+1
#define WALL		0xff	// ﾙｰﾄ検索用ﾃﾞｰﾀ
#define WALK		0x0f
#define MFx			12	// mﾌｨｰﾙﾄﾞMAX x,y
#define MFy			21
#define DELETE_LINE	   4	//

#define	BADLINE		4

// 縦 or 横ｽﾃｰﾀｽ
enum {
	tate,
	yoko
};
// ｻﾌﾞ有効度
enum {
	inval,	// 無効
	val,	// 有効
	non,	// 影響なし
};
// ﾒｲﾝ検出
enum {
	left,	// 左
	right,	// 右
	both	// 両方
};

// CPU ｷｰ操作速度
#define	ROLL_SPEED		5	// カプセル回転時のスピード
#define	VIRUSSPEEDMAX	1	// 各種操作の最速時のスピード
//#define	DOWN_SPEED		3
//#define	DEFAULT_SPEED	30

u8	aiResSpeed[][3] =	  { {  3,  2,  1 },
							{  3,  2,  1 },
							{  2,  2,  1 },
							{  2,  1,  1 },
							{  1,  1,  1 },
							{  1,  1,  1 },
							{  1,  1,  1 },
							{  1,  1,  1 }, };

u8	aiVirusLevel[][3] =	  { { 30, 24, 18 },		// EASY,NORMAL,HARD
							{ 30, 24, 18 },
							{ 24, 19, 14 },
							{ 18, 14, 10 },
							{ 14, 11,  8 },
							{  6,  6,  6 },
							{  1,  1,  1 },
							{  6,  6,  6 }, };
u8	aiDownSpeed[][3] =	  { {  7,  4,  4 },
							{  7,  4,  4 },
							{  7,  4,  4 },
							{  7,  4,  4 },
							{  7,  4,  4 },
							{  4,  3,  2 },
							{  1,  1,  1 },
							{  7,  4,  4 }, };
u8	aiSlideFSpeed[][3] =  { { 30, 24, 18 },
							{ 30, 24, 18 },
							{ 25, 20, 16 },
							{ 21, 17, 13 },
							{ 17, 14, 11 },
							{ 10,  6,  4 },
							{  1,  1,  1 },
							{ 17, 14, 11 }, };
u8	aiSlideSpeed[][3] =	  { { 30, 22, 14 },
							{ 30, 22, 14 },
							{ 15, 12, 10 },
							{  8,  7,  6 },
							{  5,  5,  5 },
							{  5,  3,  2 },
							{  1,  1,  1 },
							{  5,  5,  5 }, };

typedef struct {
	u8	ca;			// 初期左ｶﾌﾟｾﾙ色
	u8	cb;			//	〃 右　〃
} AI_NEXT;
AI_NEXT		aiNext;		// 次ｶﾌﾟｾﾙ色

typedef struct {
	u8	co;			// 色情報
	u8	st;			// ｽﾃｰﾀｽ情報
} AI_FIELD;
AI_FIELD	aiFieldData[FIELDY][FIELDX];		// ﾌｨｰﾙﾄﾞ情報
AI_FIELD	aiRecurData[FIELDY+1][FIELDX+2];	// ﾌｨｰﾙﾄﾞ情報(再帰検索用)

typedef struct {
	u8	tory;		// 縦/横ﾌﾗｸﾞ
	u8	x;			// X座標
	u8	y;			// Y座標
} AI_EDGE;			// ｴｯｼﾞ情報

u8	aiTEdgeCnt;		// 縦置き可能ｴｯｼﾞ数
u8	aiYEdgeCnt;		// 横置き	 〃

AI_FLAG	aiFlag[100];	// ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
u8	aiFlagCnt;			// ｶﾌﾟｾﾙを置ける位置数
u8	success;			// 経路okﾌﾗｸﾞ
u8	decide;				// 最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号

AI_ROOT	aiRoot[ROOTCNT];	// 投下ｶﾌﾟｾﾙのﾙｰﾄ
u8	aiRootCnt;				// ﾙｰﾄのｶｳﾝﾀ
u8	aiRollFinal;			// 最後に回転操作するかどうかﾌﾗｸﾞ

u8	aipn;					// player 番号

s8	aiDebugP1 = -1;			// ﾃﾞﾊﾞｯｸﾞﾓｰﾄﾞ時の１ＰのＣＯＭ番号

float	aiRootP;			// 移動距離比率(移動範囲が狭くなっていることを警告する)

s16	aiHiEraseCtr;			// 連鎖ﾁｪｯｸ時に上の方が崩れた所をｶｳﾝﾄ
s16	aiHiErR,aiHiErB,aiHiErY;

u16	aiGoalX,aiGoalY;			// 検索開始座標(通常は4,1)

u8	aiWall;					// 左右の壁を検出

u8	aiMoveSF;				// 落下速度が速くなった時左右移動ステップを制限する

s8	aiSelSpeed;				// ＣＯＭの操作スピード速度番号(AI_SPEED_????)
u8	aiSelCom;				// ＣＯＭの思考番号(AI_LOGIC_????)
//s8	aiSelSpeedRensa;		// ＣＯＭの操作スピード速度番号（連鎖出来る時だけｽﾋﾟｰﾄﾞを変えたいとき使用）

u16	aiPriOfs;				// ＣＯＭの下手さ（優先順位にﾗﾝﾀﾞﾑで足す）

enum {
	AGST_GAME,		// ゲーム中
	AGST_TRAIN,		// 練習中
};

//*** 渡辺ﾜｰｸを思考用ﾜｰｸに変換 ************************
//	aifMakeWork
//in:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	uupw			ﾌﾟﾚｲﾔｰ構造体
//		AICAPS		uupw->cap		AI用操作ｶﾌﾟｾﾙ情報を受け取るﾜｰｸ
//
void aifMakeWork(game_state *uupw)
{
	uupw->cap.mx = uupw->now_cap.pos_x[0];
	uupw->cap.my = uupw->now_cap.pos_y[0];
	uupw->cap.cn = uupw->cap_speed_count;
	uupw->cap.sp = uupw->cap_speed;
	uupw->cap.ca = uupw->now_cap.capsel_p[0];
	uupw->cap.cb = uupw->now_cap.capsel_p[1];

	uupw->vs = uupw->virus_number;
	uupw->lv = uupw->virus_level;
	uupw->gs = (uupw->cnd_now == dm_cnd_training) ? AGST_TRAIN : AGST_GAME;
	uupw->pn = uupw->player_no;

	if ( uupw->pn == 0 ) {
		game_state_data[0].think_level = game_state_data[1].think_level;
	}
}

//*** 渡辺ﾌｨｰﾙﾄﾞﾜｰｸを思考用ﾌｨｰﾙﾄﾞﾜｰｸに変換 ************************
//	aifMakeBlkWork
//in:	game_map	*game_map_data	ﾌﾟﾚｲﾔｰﾏｯﾌﾟﾌｨｰﾙﾄﾞ
//out:	AIBLK		uupw->blk		mﾌｨｰﾙﾄﾞ上のｶﾌﾟｾﾙ&ｳｨﾙｽの状態ﾜｰｸ
//
void aifMakeBlkWork(game_state *uupw)
{
	static u8	capsGCnv[] = {
		MB_CAPS_U,		// capsel_u,		//	カプセル	上
		MB_CAPS_D,		// capsel_d,		//				下
		MB_CAPS_L,		// capsel_l,		//				左
		MB_CAPS_R,		// capsel_r,		//				右
		MB_CAPS_BALL,	// capsel_b,		//				ボール
		MB_ERASE_CAPS,	// erase_cap_a,	//				消滅１
		MB_ERASE_CAPS,	// erase_cap_b,	//				消滅２
		MB_VIRUS_A,		// virus_a1,		//	ウイルス	a1
		MB_VIRUS_A,		// virus_a2,		//				a2
		MB_VIRUS_B,		// virus_b1,		//				b1
		MB_VIRUS_B,		// virus_b2,		//				b2
		MB_VIRUS_C,		// virus_c1,		//				c1
		MB_VIRUS_C,		// virus_c2,		//				c2
		MB_ERASE_VIRUS,	// erase_virus_a,	//				消滅１
		MB_ERASE_VIRUS,	// erase_virus_b,	//				消滅２
		MB_NOTHING,		// no_item,		//	何も無し
		MB_VIRUS_A,		// virus_a3,		//				a3
		MB_VIRUS_A,		// virus_a4,		//				a4
		MB_VIRUS_B,		// virus_b3,		//				b3
		MB_VIRUS_B,		// virus_b4,		//				b4
		MB_VIRUS_C,		// virus_c3,		//				c3
		MB_VIRUS_C,		// virus_c4		//				c4
	};

	static u8	capsCCnv[] = {
		COL_RED,	// capsel_red,			//	カプセル	赤
		COL_YELLOW,	// capsel_yellow,		//				黄
		COL_BLUE,	// capsel_blue,		//				青
		COL_RED,	// capsel_b_red,		//			暗い赤
		COL_YELLOW,	// capsel_b_yellow,	//			暗い黄
		COL_BLUE,	// capsel_b_blue,		//			暗い青
	};

	int		x,y;

	for ( x = 0;x < FIELDX;x++) {
		uupw->blk[0][x].st = MB_NOTHING;
		uupw->blk[0][x].co = COL_NOTHING;
	}
	for ( y = 1;y < FIELDY;y++) {
		for ( x = 0;x < FIELDX;x++) {
			if ( game_map_data[uupw->pn][((y-1)<<3) + x].capsel_m_flg[0] ) {
				uupw->blk[y][x].st = capsGCnv[game_map_data[uupw->pn][((y-1)<<3) + x].capsel_m_g];
				uupw->blk[y][x].co = capsCCnv[game_map_data[uupw->pn][((y-1)<<3) + x].capsel_m_p];
			} else {
				uupw->blk[y][x].st = MB_NOTHING;
				uupw->blk[y][x].co = COL_NOTHING;
			}
		}
	}

	if ( uupw->now_cap.pos_y[0] ) {
		uupw->blk[uupw->now_cap.pos_y[0]-1][uupw->now_cap.pos_x[0]].st = MB_NOTHING;
		uupw->blk[uupw->now_cap.pos_y[0]-1][uupw->now_cap.pos_x[0]].co = COL_NOTHING;
		uupw->blk[uupw->now_cap.pos_y[1]-1][uupw->now_cap.pos_x[1]].st = MB_NOTHING;
		uupw->blk[uupw->now_cap.pos_y[1]-1][uupw->now_cap.pos_x[1]].co = COL_NOTHING;
	}

}

//*** 思考開始フラグ設定（落下開始時思考処理を呼び出す）************************
//	aifMakeFlagSet
//in:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifMakeFlagSet( game_state *uupw )
{
	uupw->ai.aiok = FALSE;
	uupw->ai.aiOldRollCnt = 0;
	uupw->ai.aiRollCnt = 0;
	uupw->ai.aiRollFinal = 0;
	uupw->ai.aiRollHabit = 0;
}


//*** ゲーム開始時初期化処理 ************************
//	aifGameInit
//in:	none
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifGameInit(void)
{
	int i, j;

	fool_mode = FALSE;
	s_hard_mode  = FALSE;
	MissRate = 0;
	PlayTime = 0;

	for(i=0;i<4;i++) {
		game_state_data[i].ai.aiok = TRUE;
		game_state_data[i].ai.aiRandFlag = TRUE;
//		game_state_data[i].ai.aiNum = 0;
		game_state_data[i].ai.aiState = 0;
		game_state_data[i].ai.aiRootP = 100;

		for(j = 0; j < NUM_AI_EFFECT; j++) {
			game_state_data[i].ai.aiEffectNo[j] = AI_EFFECT_Ignore;
			game_state_data[i].ai.aiEffectCount[j] = 0;
		}
	}

//	game_state_data[0].player_type = PUF_PlayerCPU;
}

//*** プログラム開始時初期化処理 ************************
//	aifFirstInit
//in:	none
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifFirstInit(void)
{
	int i, j;

	for(i = 0;i < _AI_STATE_SUM;i++) {
		for(j = 0;j < _AI_LOGIC_SUM;j++) {
			ai_param[j][i] = ai_param_org[j][i];
		}
	}

	for(i = 0; i < 16; i++) {
		ai_char_data[i] = ai_char_data_org[i];
	}

	aifGameInit();
}


/***********************************************
	cpu ai set
***********************************************/
//*** ＣＰＵＡＩ処理メインプログラム ************************
//	aifMake
//in:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifMake(game_state* uupw)
{
	int	i;

	pGameState = uupw;

	// もう思考した？
	if ( uupw->ai.aiok ) return;

	// ワークの初期化
	aifMakeWork(uupw);
	aifMakeBlkWork(uupw);

	aiGoalX = uupw->cap.mx+1; // カプセル位置の設定
	aiGoalY = uupw->cap.my;
	if ( aiGoalY == 0 ) aiGoalY++; // y=0の時は落下開始前なので1に設定

	aipn = uupw->pn;

	uupw->ai.aivl = uupw->think_level;

	if ( FallSpeed[uupw->cap.sp] > aiSlideFSpeed[aiSelSpeed][uupw->ai.aivl] ) {
		// 落下経路捜索時落ちる速度が操作速度よりは焼いときは横移動を１落下につき１回のみとして検索
		aiMoveSF = 1;
	} else {
		aiMoveSF = 0;
	}

	// ｶｻﾞﾏ a
	aifFieldCopy(uupw);	// ﾌｨｰﾙﾄﾞﾃﾞｰﾀｺﾋﾟｰ
	aifPlaceSearch();	// ｶﾌﾟｾﾙ置き場ｻｰﾁ
	aifMoveCheck();		// 移動可能ﾁｪｯｸ

	// フラッシュモード
	delpos_cnt = 0;
	if ( evs_gamemode == GMD_FLASH ) {
		flash_special();
		last_flash = 0;
		for ( i = 0; i < pGameState->flash_virus_count; i++ ) {		// 数
			if ( pGameState->flash_virus_pos[i][2] >= 0 ) last_flash++;
		}
	}

	// ｵｸﾞﾗ
	aiSetCharacter(uupw);	// キャラクター別思考パラメータ設定

	// ﾋﾙﾋﾙ
	aiHiruAllPriSet(uupw);		// 置き場所決定!
	aiHiruSideLineEraser(uupw);	// 塔の破壊

	// ｶｻﾞﾏ b
	aifReMoveCheck();	// 移動ﾙｰﾄ記録
	aifKeyMake(uupw);		// ｷｰ情報作成
	uupw->ai.aiKeyCount = 0;

	uupw->ai.aiok = TRUE;
}

//*** ＣＰＵＡＩ処理メインプログラム ************************
//	aifMake2
//in:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
int aifMake2(game_state* uupw, int x, int y, int tateFlag, int revFlag)
{
	// もう思考した？
	if(uupw->ai.aiok) return 0;

	// ワークの初期化
	aifMakeWork(uupw);
	aifMakeBlkWork(uupw);
	MissRate = 0;

	aiGoalX = uupw->cap.mx+1; // カプセル位置の設定
	aiGoalY = uupw->cap.my;
	if ( aiGoalY == 0 ) aiGoalY++; // y=0の時は落下開始前なので1に設定

	aipn = uupw->pn;

	uupw->ai.aivl = uupw->think_level;

	if ( FallSpeed[uupw->cap.sp] > aiSlideFSpeed[aiSelSpeed][uupw->ai.aivl] ) {
		// 落下経路捜索時落ちる速度が操作速度よりは焼いときは横移動を１落下につき１回のみとして検索
		aiMoveSF = 1;
	} else {
		aiMoveSF = 0;
	}

	// ｶｻﾞﾏ a
	aifFieldCopy(uupw);	// ﾌｨｰﾙﾄﾞﾃﾞｰﾀｺﾋﾟｰ
//	aifPlaceSearch();	// ｶﾌﾟｾﾙ置き場ｻｰﾁ
//	aifMoveCheck();		// 移動可能ﾁｪｯｸ

	// ｵｸﾞﾗ
//	aiSetCharacter(uupw);	// キャラクター別思考パラメータ設定

	// ﾋﾙﾋﾙ
//	aiHiruAllPriSet(uupw);		// 置き場所決定!
//	aiHiruSideLineEraser(uupw);	// 塔の破壊

	aiFlag[0].tory = (tateFlag ? tate : yoko);
	aiFlag[0].x    = x + 1;
	aiFlag[0].y    = y;
	aiFlag[0].rev  = (revFlag ? 1 : 0);
	aiFlagCnt = 1;
	decide = 0;

	// ｶｻﾞﾏ b
	aifReMoveCheck();	// 移動ﾙｰﾄ記録
	aifKeyMake(uupw);		// ｷｰ情報作成
	uupw->ai.aiKeyCount = 0;

	uupw->ai.aiok = TRUE;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// 各位置に於ける優先度 ｻｰﾁ
enum {	// af->hei(wid)[x][]:
	LnMaster,	// 主: 土台
	LnSlave,	// 従: 浮き(土台)
};
/*
enum {	// af->hei(wid)[][x]: ﾃｰﾌﾞﾙ要素
	LnEraseLin,	// 消去ﾗｲﾝ数
	LnEraseVrs,	// 消去ｳｲﾙｽ数
	LnRinsetsu,	// 隣接数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ )
	LnLinesAll,	// 並び数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ,歯欠の間1ならばｶｳﾝﾄ )
	LnOnLinVrs,	// 並びの中のｳｲﾙｽ数
	LnLinSpace,	// ﾗｲﾝｽﾍﾟｰｽ( 3以下なら無効? )
	LnEraseVrsSide,	// ｳｨﾙｽ周辺消去数(旧Dummy)
	LnHighCaps,	// 入り口付近にあるｶﾌﾟｾﾙの数
	LnHighVrs,	// 入り口付近にあるｳｨﾙｽの数
	LnNonCount,	// ﾉｰｶｳﾝﾄ( ex.ﾀﾃが消去の時のﾖｺ )
	LnTableMax,	//------ MAX
};
*/
enum {	// 停止ﾌﾗｸﾞ
	SL_Both,	// 縦横検索
	SL_OnlyHei,	// 縦のみ
	SL_OnlyWid,	// 横のみ
};
#define NonTouch -1000000

// ｸﾞﾛｰﾊﾞﾙ
AI_FIELD	aif_field[FIELDY][FIELDX];	// 画面疑似情報
u8	hei_data[LnTableMax];						// ﾀﾃ列ﾁｪｯｸ
u8	wid_data[LnTableMax];						// ﾖｺ列ﾁｪｯｸ

//////////////////////////////////////////////////////////////////////////////
// 強引横列破壊指令とその実行
// 天井が近すぎてｶﾌﾟｾﾙを積んでも消せないｳｨﾙｽを塔を建てて消しに行く処理
//

//#define DontTouch	0xff
//enum {
//	SRH_Virus,
//	SRH_Caps,
//	SRH_None,
//	SRH_Decide,
//};
u8	aiLinePri[] = { 4,3,5,2,6,1,7,0 };	// 塔検索順序
//u8	aiVirusCnt	= 0;	// ｳｲﾙｽｶｳﾝﾀ
//u8	aiVrsEqCnt	= 0;	// ｳｲﾙｽ数の変動がなかった回数
//u8	aiVrsEqMax	= VRS_EQ_CNTa;
#define CAPS_HIGH 5			// ライン以上にｶﾌﾟｾﾙがあって↓
#define VIRUS_HIGH 14		// ライン以上にしかｳｨﾙｽが無い時､塔を作る

//*** 天井が近すぎてｶﾌﾟｾﾙを積んでも消せないｳｨﾙｽを塔を建てて消しに行く処理 ************************
//	aiHiruSideLineEraser
//in:	game_state	xpw				ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	xpw				ﾌﾟﾚｲﾔｰ構造体
//
void aiHiruSideLineEraser
(
	game_state*	xpw
){
	int		x,y,z,i,j,f,f2,fc,r,h[2],w[2],m,k,p,v,c,a,b,e,g,g2,n,xx,cf[2];
	u8		low_virus[MFx];		// 下に4ﾗｲﾝ以上空間があるｳｨﾙｽのY座標
	u8		low_capsx[MFx];		// low_virus[]の下のｶﾌﾟｾﾙのY座標
	u8		lst_virus[MFx];		// ﾗｲﾝ上の一番下のｳｨﾙｽのY座標
	u8		lst_capsx[MFx];		// ﾗｲﾝ上の一番上のｶﾌﾟｾﾙのY座標

	u8		chk_line[8];

//	if ( aiFlag[decide].hei[LnMaster][LnOnLinVrs] && aiFlag[decide].tory == tate ) return;
	if ( aiFlag[decide].hei[LnMaster][LnOnLinVrs] || aiFlag[decide].hei[LnSlave][LnOnLinVrs] ) return;


	// 問題の状態の検索
	fc = FALSE;
	f2 = TRUE;
	f = FALSE;
	for( x=0;x<8;x++ ) {
		low_virus[x] = low_capsx[x] = lst_capsx[x] = 17;
		lst_virus[x] = 0;
	}
	for( x=0;x<8;x++ ) {
		for( y=16,z=g=g2=0;y>1;y-- ) {
			if( MB_NOTHING == xpw->blk[y][x].st ) {						// 何もない時
				z++;
			} else if( MB_VIRUS_A <= xpw->blk[y][x].st && xpw->blk[y][x].st <= MB_VIRUS_C ) {
				if ( !g2 ) {											// ｳｨﾙｽだった時
//					if( !g ) {
						lst_virus[x] = y;
//						g++;
//					}
					if( z >= 3 ) {
						low_virus[x] = y;
					} else {
						e = xpw->blk[y][x].co;
						if(( xpw->blk[y+2][x].st == MB_NOTHING || xpw->blk[y+2][x].co == e )&& xpw->blk[y+3][x].co == e ) {
							low_virus[x] = y;
						} else {
							if ( (x == 2) || (x == 3) || (x == 4) || (x == 5) ) {
								lst_virus[x] = 0;
								g2 = -1;
								low_capsx[x] = y;
								z = 0;
								lst_capsx[x] = y;
							}
						}
					}
					g2++;
				}
			} else {													// ｶﾌﾟｾﾙだった時
				if ( !g2 ) {
					low_capsx[x] = y;
					z=0;
				}
				lst_capsx[x] = y;
			}
		}
		if( (lst_virus[x] != 0) && (lst_capsx[x] <= CAPS_HIGH) ) {		// ｳｨﾙｽの上のカプセルを消せない時塔を作る
			f2 = FALSE;
			if ( (x == 2) || (x == 3) || (x == 4) || (x == 5) ) {
				fc = TRUE;
				f = FALSE;
				break;
			}
		}
		if( lst_virus[x] >= VIRUS_HIGH ) {								// ｳｨﾙｽが下のほうに残っている時はまだ塔は作らない
			f = TRUE;
		}
	}

	if( !(f|f2) ) {

//if ( aipn == 0 ) osSyncPrintf("\npass");
//osSyncPrintf("\npass %d",aipn);

		for( x=0;x<8;x++ ) chk_line[x] = TRUE;
		cf[0] = cf[1] = TRUE;
		f = FALSE;
		for( p=0;p<8 && !f;p++ ) {										// 全ﾗｲﾝ検索
			x = aiLinePri[p];											// ﾌﾟﾗｲｵﾘﾃｨｰの高い順
			v = low_virus[x];
			if( VIRUS_HIGH > v ) chk_line[x] = FALSE;					// 問題のﾗｲﾝか?
			if ( VIRUS_HIGH > v && cf[x/4] ) {
				cf[x/4] = FALSE;										// ﾁｪｯｸしたﾗｲﾝより外側はﾁｪｯｸしない
//				if ( x > 0 ) chk_line[x-1] = TRUE;
//				if ( x < 7 ) chk_line[x+1] = TRUE;
				c = low_capsx[x];										// c : ｶﾌﾟｾﾙを積み上げる位置のY座標
				r = c - v;												// r : 積む位置からｳｨﾙｽまでの間隔
				if( x >= 4 ) xx = x - 1;								// 横置きの時、左右どちらにはみ出すか
				else		 xx = x;
				for( m=h[0]=h[1]=w[0]=w[1]=0;m<aiFlagCnt;m++ ) {		// 置く位置までの移動ｶﾌﾟｾﾙの経路を検索
					if( aiFlag[m].ok == TRUE && aiFlag[m].y == c - 1 ) {	// （ﾊﾞｸﾞ : 前は && が || になってた）
						if( aiFlag[m].tory == tate ) {
							if( aiFlag[m].x -1 == x ) {
								h[aiFlag[m].rev] = m+1;
							}
						} else {
							if( aiFlag[m].x -1 == xx ) {
								w[aiFlag[m].rev] = m+1;
							}
						}
					}
				}
				if( h[0] + w[0] + h[1] + w[1] ) {
					e = xpw->blk[v][x].co;									// e : 消したいｳｨﾙｽの色
					g = n = FALSE;
					if( r == 5 ) {											// 5ﾗｲﾝ離れている時は適当に横に置く
						if( w[0] + w[1] ) {
							f = w[0];
						}
						if ( h[0] + h[1] ) {								// ｳｨﾙｽと同じ色がある時はそれを上向きに縦に置く
							if( aiNext.ca == e ) {
								f = h[0];
							} else if( aiNext.cb == e ) {
								f = h[1];
							}
						}
					} else if( r <= 4 ) {									// 4ﾗｲﾝ以下の時
						if( w[0] + w[1] ) {									// 異色ｶﾌﾟｾﾙでｳｨﾙｽと同じ色がある時は色を合わせて横に置く
							if( aiNext.ca == e ) {
								if( x < 4 ) f = w[0];
								else		f = w[1];
							} else if( aiNext.cb == e ) {
								if( x < 4 ) f = w[1];
								else		f = w[0];
							}
						}
						if( h[0] + h[1] ) {
							if( aiNext.ca == aiNext.cb && e == aiNext.ca ) {// 同色ｶﾌﾟｾﾙでｳｨﾙｽと同じ色なら縦に置く
								f = h[0];
							}
						}
					} else {												// 5ﾗｲﾝより離れている時は適当に縦に置く
						if( h[0] ) {
							f = h[0];
						}
					}
				}
			}
		}
		if( f ) {
			decide = f - 1;											// 置く場所が決定していたら経路番号を改変

//if ( aipn == 0 ) osSyncPrintf(" set!!!");

		} else if ( !fc ) {
			z = -1;
			for( i=0,x=NonTouch-1;i<aiFlagCnt;i++ ) {
				if ( aiFlag[i].tory == tate ) {
					if( (chk_line[aiFlag[i].x-1] || (low_virus[aiFlag[i].x-1] > aiFlag[i].y)) && aiFlag[i].pri > x ) {
						x = aiFlag[i].pri;
						z = i;
					}
				} else {
#if defined(DEBUG)
if ( aiFlag[i].x == 0 ) osSyncPrintf("Error !!");
#endif
					if( (chk_line[aiFlag[i].x-1] || (low_virus[aiFlag[i].x-1] > aiFlag[i].y)) && (chk_line[aiFlag[i].x] || (low_virus[aiFlag[i].x] > aiFlag[i].y)) &&	 aiFlag[i].pri > x ) {
						x = aiFlag[i].pri;
						z = i;
					}
				}
			}
			if ( z != -1 ) decide = z;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// 必殺穴熊囲いとその実行
// 左右３ﾗｲﾝ分にｶﾌﾟｾﾙで天井を作り、相手の攻撃を防御し操作不能時間を短くする必殺技！(未使用)
//

//#define DontTouch	0xff
//enum {
//	SRH_Virus,
//	SRH_Caps,
//	SRH_None,
//	SRH_Decide,
//};
//u8	aiLinePri[] = { 4,3,5,2,6,1,7,0 };
//u8	aiVirusCnt	= 0;	// ｳｲﾙｽｶｳﾝﾀ
//u8	aiVrsEqCnt	= 0;	// ｳｲﾙｽ数の変動がなかった回数
//u8	aiVrsEqMax	= VRS_EQ_CNTa;
//#define CAPS_HIGH 4			// ライン以上にｶﾌﾟｾﾙがあって↓
//#define VIRUS_HIGH 12		// ライン以上にしかｳｨﾙｽが無い時､塔を作る

/*
void aiKumaSideLineblocker
(
	game_state*	xpw
){
	int		x,y,z,i,j,f,f2,r,h[2],w[2],m,k,p,v,c,a,b,e,g,g2,n,xx;
	u8		low_virus[MFx];		// 下に4ﾗｲﾝ以上空間があるｳｨﾙｽのY座標
	u8		low_capsx[MFx];		// low_virus[]の下のｶﾌﾟｾﾙのY座標
	u8		lst_virus[MFx];		// ﾗｲﾝ上の一番下のｳｨﾙｽのY座標
	u8		lst_capsx[MFx];		// ﾗｲﾝ上の一番上のｶﾌﾟｾﾙのY座標

	// 問題の状態の検索
	f2 = TRUE;
	f = FALSE;
	for( x=0;x<8;x++ ) {
		low_virus[x] = low_capsx[x] = lst_capsx[x] = 17;
		lst_virus[x] = 0;
		for( y=16,z=g=g2=0;y>1;y-- ) {
			if( MB_NOTHING == xpw->blk[y][x].st ) {
				z++;
			} else if( MB_VIRUS_A <= xpw->blk[y][x].st && xpw->blk[y][x].st <= MB_VIRUS_C ) {
				if ( !g2 ) {
					if( !g ) {
						lst_virus[x] = y;
						g++;
					}
					if( z >= 3 ) {
						low_virus[x] = y;
					} else {
						e = xpw->blk[y][x].co;
						if(( xpw->blk[y+2][x].st == MB_NOTHING || xpw->blk[y+2][x].co == e )&& xpw->blk[y+3][x].co == e ) {
							low_virus[x] = y;
						}
					}
					g2++;
				}
			} else {
				if ( !g2 ) {
					low_capsx[x] = y;
					z=0;
				}
				lst_capsx[x] = y;
			}
		}
		if( (lst_virus[x] != 0) && (lst_capsx[x] <= CAPS_HIGH) ) {		// ｳｨﾙｽの上のカプセルを消せない時塔を作る
			f2 = FALSE;
		}
		if( lst_virus[x] >= VIRUS_HIGH ) {								// ｳｨﾙｽが下のほうに残っている時はまだ塔は作らない
			f = TRUE;
		}
	}

	if( !(f|f2) ) {

//osSyncPrintf("\npass");

		f = FALSE;
		for( p=0;p<8 && !f;p++ ) {										// 全ﾗｲﾝ検索
			x = aiLinePri[p];											// ﾌﾟﾗｲｵﾘﾃｨｰの高い順
			v = low_virus[x];
			if( VIRUS_HIGH > v ) {										// 問題のﾗｲﾝか?
				c = low_capsx[x];										// c : ｶﾌﾟｾﾙを積み上げる位置のY座標
				r = c - v;												// r : 積む位置からｳｨﾙｽまでの間隔
				if( x >= 4 ) xx = x - 1;								// 横置きの時、左右どちらにはみ出すか
				else		 xx = x;
				for( m=h[0]=h[1]=w[0]=w[1]=0;m<aiFlagCnt;m++ ) {		// 置く位置までの移動ｶﾌﾟｾﾙの経路を検索
					if( aiFlag[m].ok == TRUE && aiFlag[m].y == c - 1 ) {	// （ﾊﾞｸﾞ : 前は && が || になってた）
						if( aiFlag[m].tory == tate ) {
							if( aiFlag[m].x -1 == x ) {
								h[aiFlag[m].rev] = m+1;
							}
						} else {
							if( aiFlag[m].x -1 == xx ) {
								w[aiFlag[m].rev] = m+1;
							}
						}
					}
				}
				if( !( h[0] + w[0] + h[1] + w[1] )) break;				// 置けない時通常のまま
				e = xpw->blk[v][x].co;									// e : 消したいｳｨﾙｽの色
				g = n = FALSE;
				if( r == 5 ) {											// 5ﾗｲﾝ離れている時は適当に横に置く
					if( w[0] + w[1] ) {
						f = w[0];
					}
				} else if( r <= 4 ) {									// 4ﾗｲﾝ以下の時
					if( h[0] + h[1] ) {
						if( aiNext.ca == aiNext.cb && e == aiNext.ca ) {// 同色ｶﾌﾟｾﾙでｳｨﾙｽと同じ色なら縦に置く
							f = h[0];
							break;
						}
					}
					if( w[0] + w[1] ) {									// 異色ｶﾌﾟｾﾙでｳｨﾙｽと同じ色がある時は色を合わせて横に置く
						if( aiNext.ca == e ) {
							if( x < 4 ) f = w[0];
							else		f = w[1];
						} else if( aiNext.cb == e ) {
							if( x < 4 ) f = w[1];
							else		f = w[0];
						}
					}
				} else {												// 5ﾗｲﾝより離れている時は適当に縦に置く
					if( h[0] ) {
						f = h[0];
					}
				}
			}
		}
		if( f ) decide = f - 1;											// 置く場所が決定していたら経路番号を改変
	}
}
*/

//////////////////////////////////////////////////////////////////////////////
// ｲﾚｰｽﾗｲﾝ

//*** 消去されたｶﾌﾟｾﾙの反対側を玉に変換する ************************
//	aif_MiniChangeBall
//in:	u8			x,y			消去されたｶﾌﾟｾﾙの座標位置
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//out:	AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//
aif_MiniChangeBall( u8 y, u8 x )
{
	static s8 srh[4][2] = {{ 1,0,},{ -1,0,},{ 0,1,},{ 0,-1,}};
	s8	yy,xx,f;

	if( MB_CAPS_BALL != aif_field[y][x].st ) {
		f = aif_field[y][x].st - MB_CAPS_U;
		yy = y + srh[f][0];
		xx = x + srh[f][1];
		if( yy > 0 && yy < 17 && xx >= 0 && xx < 8 ) {
			aif_field[yy][xx].st = MB_CAPS_BALL;
		}
	}
}

//*** 実際に消去した状況を作る、消去ｳｨﾙｽ数もｶｳﾝﾄ ************************
//	aifEraseLineCore
//in:	int			mx,my		消去ﾁｪｯｸ座標
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//out:	int						(未使用)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//		u8			hei_data	消去ﾁｪｯｸ座標の縦ラインの情報
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//
int aifEraseLineCore
(
	int	mx, int my	// ﾀｰｹﾞｯﾄﾎﾟｼﾞｼｮﾝ
){
	int	x,y,z = FALSE,f;
	u8	tc = aif_field[my][mx].co;
	int		n, ret;

	ret = FALSE;

	if( hei_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		for( y=my-1;y>0 && y>my-4;y-- ) {
			if( aif_field[y][mx].co == tc ) {
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					hei_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( y,mx );
				}

				if ( evs_gamemode == GMD_FLASH ) {
					//フラッシュモード
					if ( flash_virus(mx, y) ) ret = TRUE;
				}

				aif_field[y][mx].st = MB_NOTHING;
				aif_field[y][mx].co = COL_NOTHING;
			} else {
				y = 0;
			}
		}
		for( y=my+1;y<my+4 && y<17;y++ ) {
			if( aif_field[y][mx].co == tc ) {
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					hei_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( y,mx );
				}
				aif_field[y][mx].st = MB_NOTHING;
				aif_field[y][mx].co = COL_NOTHING;
			} else {
				y = 17;
			}
		}
	}
	if( wid_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		for( x=mx-1;x>-1 && x>mx-4;x-- ) {
			if( aif_field[my][x].co == tc ) {
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( my,x );
				}
				aif_field[my][x].st = MB_NOTHING;
				aif_field[my][x].co = COL_NOTHING;
			} else {
				x = -1;
			}
		}
		for( x=mx+1;x<mx+4 && x<8;x++ ) {
			if( aif_field[my][x].co == tc ) {
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( my,x );
				}

				if ( evs_gamemode == GMD_FLASH ) {
					//フラッシュモード
					if ( flash_virus(x, my) ) ret = TRUE;
				}

				aif_field[my][x].st = MB_NOTHING;
				aif_field[my][x].co = COL_NOTHING;
			} else {
				x = 8;
			}
		}
	}
	if( z ) {
		aif_MiniChangeBall( my,mx );									// 消去があった場合自分も消す
		aif_field[my][mx].st = MB_NOTHING;
		aif_field[my][mx].co = COL_NOTHING;
	}

	return(ret);
}

//////////////////////////////////////////////////////////////////////////////
// ﾗｲﾝｻｰﾁ
#define	Sub_Divide 3	// ｻﾌﾞ指定されたﾎﾟｲﾝﾄの割り算値

enum {	// 単一ｶﾌﾟｾﾙの状況: AloneCapP[af->only[x]]
	ALN_FALSE,	// 単一ｶﾌﾟｾﾙではない
	ALN_Bottom,	// ﾌｨｰﾙﾄﾞ最下層
	ALN_FallCap,// 落下してｶﾌﾟｾﾙの上
	ALN_FallVrs,//	  ..   ｳｲﾙｽの上
	ALN_Capsule,// ｶﾌﾟｾﾙの上
	ALN_Virus,	// ｳｲﾙｽの上
	ALN_MAX,	//--- Max = 6
};

// ｸﾞﾛｰﾊﾞﾙﾃﾞｰﾀ
s16	bad_point[] = { -90,-180-90,-270-90,-900,-900,-270-90,-180-90,-90 };
s16	bad_point2[] = { -90,-180-90,-270-90,-9000,-9000,-270-90,-180-90,-90 };
s16	pri_point[] = {
	 NULL,	// Erase: Line			  -> EraseLinP[x]
	 30+150,	//		  Virus
	  9,	//		  BombCount
	 NULL,	// No Erase: InLineCount  -> LinesAllp[x]
	 15+16,	//			 Virus
	 NULL,	//			 LineSpace( < 4:BadLine )
	 0,		//
	 0,		//
	 0,		//
	 NULL,
};
/*
	LnEraseLin,	// 消去ﾗｲﾝ数
	LnEraseVrs,	// 消去ｳｲﾙｽ数
	LnRinsetsu,	// 隣接数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ )
	LnLinesAll,	// 並び数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ,歯欠の間1ならばｶｳﾝﾄ )
	LnOnLinVrs,	// 並びの中のｳｲﾙｽ数
	LnLinSpace,	// ﾗｲﾝｽﾍﾟｰｽ( 3以下なら無効? )
	LnEraseVrsSide,	// ｳｨﾙｽ周辺消去数(旧Dummy)
	LnHighCaps,	// 入り口付近にあるｶﾌﾟｾﾙの数
	LnHighVrs,	// 入り口付近にあるｳｨﾙｽの数
	LnNonCount,	// ﾉｰｶｳﾝﾄ( ex.ﾀﾃが消去の時のﾖｺ )
	LnTableMax,	//------ MAX
*/
s16	EraseLinP[] = { NULL,  30, 90,180,270,360,540,540,540, };	// [LnEraseLin]	消去した時加算されるポイント
float	HeiEraseLinRate = 1.0;
float	WidEraseLinRate = 1.0;
s16	HeiLinesAllp[] = { NULL,NULL, 18-4, 45-5, 45, 45, 45, 45, 45, };	// [LnLinesAll]	同色ラインの長さで加算されるポイント（縦ﾗｲﾝ）
s16	WidLinesAllp[] = { NULL,NULL, 18-4, 45-5, 45, 45, 45, 45, 45, };	// [LnLinesAll]	同色ラインの長さで加算されるポイント（横ﾗｲﾝ）
s16	AloneCapP[] = { NULL, -60,-70,-90,-80,-100 };				// af->only[] 置き場所がない時に適当な所に置く時の減算ﾎﾟｲﾝﾄ
s16	AloneCapWP[] = { NULL, 0, 0, 0, 0, 0 };						// af->wonly[] 置き場所がない時に適当な所に置く時の減算ﾎﾟｲﾝﾄ(横ﾗｲﾝ専用)
int	OnVirusP = 0;
s16	HiEraseP = 40;
s16	RensaP = 300;
s16	RensaMP = -300;

s16	LPriP = 0;

u8	BadLineRate[][8] = { { 6,7,8,9,9,8,7,6,},
						 { 6,7,8,9,9,8,7,6,},
						 { 2,2,4,7,7,4,2,2,},
						 { 1,1,2,4,4,2,1,1,} };

s16	WallRate[][8] = { { 10,10,10,10,10,10,10,10, },
					  { 64,64,32,16, 8, 4, 2, 1, },
					  {	 1, 2, 4, 8,16,32,64,64, },
					  { 64,64,16, 4, 4,16,64,64, } };


//*** 連鎖数ﾁｪｯｸ（同時に危険ゾーンのカプセルが消えるかどうかの計算も含む） ************************
//	aifRensaCheckCore
//in:	game_state	uupw		ﾌﾟﾚｲﾔｰ構造体
//		AI_FLAG		af			移動ｶﾌﾟｾﾙの状態
//		u8			mx,my		ﾏｽﾀｰｶﾌﾟｾﾙの座標x､y
//		u8			mco,mst		ﾏｽﾀｰｶﾌﾟｾﾙの色､形
//		u8			sx,sy		ｽﾚｰﾌﾞｶﾌﾟｾﾙの座標x､y
//		u8			sco,sst		ｽﾚｰﾌﾞｶﾌﾟｾﾙの色､形
//out:	int						消去で１、同時消しで２、消せない時は０を返す
//		s16			aiHiEraseCtr	上の方が崩れた所をｶｳﾝﾄ
//
int aifRensaCheckCore(game_state* uupw ,AI_FLAG* af ,u8 mx,u8 my,u8 mco,u8 mst,u8 sx,u8 sy,u8 sco,u8 sst)
{
	int	idx,idy;
	game_state*	upw;
	u8		ctr = 0;
	u8		co;
	u8		c,i;
	u8		x,y;

	upw = uupw;

	/*----- field data copy -----*/
	for ( idx=0 ; idx<FIELDX ; idx++ ) {
	for ( idy=0 ; idy<FIELDY ; idy++ ) {
		aif_field[idy][idx].co = upw->blk[idy][idx].co;	// 色情報ｺﾋﾟｰ
		aif_field[idy][idx].st = upw->blk[idy][idx].st;	// ｽﾃｰﾀｽ情報ｺﾋﾟｰ
	}}

	aiHiEraseCtr = 0;										// 危険ゾーンのカプセル数を数えておく
	for ( idx=0 ; idx < FIELDX ; idx++ ) {
	for ( idy=1 ; idy < 4 ; idy++ ) {
//		if ( (idx != mx || idy != my) && (idx != sx || idy != sy) && aif_field[idy][idx].st <= MB_VIRUS_C ) {
		if ( aif_field[idy][idx].st <= MB_VIRUS_C ) {
			aiHiEraseCtr += BadLineRate[idy][idx];
		}
	}}

	if ( my ) {
		aif_field[my][mx].st = mst;
		aif_field[my][mx].co = mco;
	}
	if ( sy ) {
		aif_field[sy][sx].st = sst;
		aif_field[sy][sx].co = sco;
	}

	// 落下前消去ﾁｪｯｸ
	for(idy = 1;idy < FIELDY;idy++) {
		for(idx = 0;idx < FIELDX-3;idx++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idx+c < FIELDX;c++) {
					if ( (aif_field[idy][idx+c].st == MB_NOTHING) || (aif_field[idy][idx+c].co != co) ) break;
				}
				if ( c >= DELETE_LINE ) {
					ctr += c / DELETE_LINE;
//					if ( ctr > 1 ) return 2;
					for(i = 0;i < c;i++) {
						aif_MiniChangeBall( idy, idx+i);
						aif_field[idy][idx+i].st = MB_ERASE_CAPS;
					}
				}
				idx += c - 1;
			}
		}
	}
	for(idx = 0;idx < FIELDX;idx++) {
		for(idy = 1;idy < FIELDY-3;idy++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idy+c < FIELDY;c++) {
					if ( (aif_field[idy+c][idx].st == MB_NOTHING) || (aif_field[idy+c][idx].co != co) ) break;
				}
				if ( c >= DELETE_LINE ) {
					ctr += c / DELETE_LINE;
//					if ( ctr > 1 ) return 2;
					for(i = 0;i < c;i++) {
						aif_MiniChangeBall( idy+i, idx);
						aif_field[idy+i][idx].st = MB_ERASE_CAPS;
					}
				}
				idy += c - 1;
			}
		}
	}

	// 落下処理
	c = TRUE;
	for(idy = FIELDY-1;idy >= 1;idy--) {
		for(idx = FIELDX-1;idx >= 0;idx--) {
			switch( aif_field[idy][idx].st ) {
			case MB_ERASE_CAPS:
				aif_field[idy][idx].st = MB_NOTHING;
				break;
			case MB_CAPS_BALL:
				if ( idy == FIELDY-1 ) break;
				if ( aif_field[idy+1][idx].st != MB_NOTHING ) break;
				c = FALSE;
				for(i = idy+1;i < FIELDY && aif_field[i][idx].st == MB_NOTHING;i++ );
				i--;
				aif_field[i][idx].st = aif_field[idy][idx].st;
				aif_field[i][idx].co = aif_field[idy][idx].co;
				aif_field[idy][idx].st = MB_NOTHING;
				break;
			case MB_CAPS_D:
				if ( idy == FIELDY-1 ) break;
				if ( aif_field[idy+1][idx].st != MB_NOTHING ) break;
				c = FALSE;
				for(i = idy+1;i < FIELDY && aif_field[i][idx].st == MB_NOTHING;i++ );
				i--;
				aif_field[i][idx].st = aif_field[idy][idx].st;
				aif_field[i][idx].co = aif_field[idy][idx].co;
				aif_field[idy][idx].st = MB_NOTHING;
				aif_field[i-1][idx].st = aif_field[idy-1][idx].st;
				aif_field[i-1][idx].co = aif_field[idy-1][idx].co;
				aif_field[idy-1][idx].st = MB_NOTHING;
				break;
			case MB_CAPS_L:
				if ( idy == FIELDY-1 ) break;
				if ( aif_field[idy+1][idx].st != MB_NOTHING || aif_field[idy+1][idx+1].st != MB_NOTHING ) break;
				c = FALSE;
				for(i = idy+1;i < FIELDY && aif_field[i][idx].st == MB_NOTHING && aif_field[i][idx+1].st == MB_NOTHING;i++ );
				i--;
				aif_field[i][idx].st = aif_field[idy][idx].st;
				aif_field[i][idx].co = aif_field[idy][idx].co;
				aif_field[idy][idx].st = MB_NOTHING;
				aif_field[i][idx+1].st = aif_field[idy][idx+1].st;
				aif_field[i][idx+1].co = aif_field[idy][idx+1].co;
				aif_field[idy][idx+1].st = MB_NOTHING;
				break;
			}
		}
	}

	for ( idx=0 ; idx < FIELDX ; idx++ ) {					// 危険ゾーンからどれだけカプセルが減ったかカウント
	for ( idy=1 ; idy < 4 ; idy++ ) {
		if ( aif_field[idy][idx].st <= MB_VIRUS_C ) {
			aiHiEraseCtr -= BadLineRate[idy][idx];
		}
	}}

	if ( ctr > 1 ) {
		if ( c ) return 2;
		return 1;
	}

	// 落下後消去ﾁｪｯｸ
	for(idy = 1;idy < FIELDY;idy++) {
		for(idx = 0;idx < FIELDX-3;idx++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idx+c < FIELDX && aif_field[idy][idx+c].st != MB_NOTHING && aif_field[idy][idx+c].co == co;c++);
				if ( c >= DELETE_LINE ) return 1;
			}
		}
	}
	for(idx = 0;idx < FIELDX;idx++) {
		for(idy = 1;idy < FIELDY-3;idy++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idy+c < FIELDY && aif_field[idy+c][idx].st != MB_NOTHING && aif_field[idy+c][idx].co == co;c++);
				if ( c >= DELETE_LINE ) return 1;
			}
		}
	}

	return 0;
}

//*** 現在の操作カプセルを設定して連鎖チェック（同時に危険ゾーンのカプセルが消えるかどうかの計算も含む） ************************
//	aifRensaCheck
//in:	game_state	uupw		ﾌﾟﾚｲﾔｰ構造体
//		AI_FLAG		af			移動ｶﾌﾟｾﾙの状態
//out:	int						消去で１、同時消しで２、消せない時は０を返す
//		s16			aiHiEraseCtr	上の方が崩れた所をｶｳﾝﾄ
//
int aifRensaCheck(game_state* uupw ,AI_FLAG* af)
{
	int	idx,idy;
	game_state*	upw;
	u8		ctr = 0;
	u8		co;
	u8		c,i;
	u8		x,y;
	u8		mx,my,mco,mst;
	u8		sx,sy,sco,sst;

	upw = uupw;

	x = af->x - 1;
	y = af->y;

	if( af->tory == tate ) {
		mx = x;
		my = y;
		mst = MB_CAPS_D;
		sx = x;
		sy = y-1;
		sst = MB_CAPS_U;
		if( af->rev == FALSE ) {
			mco = aiNext.cb;
			if( y - 1 > 0 ) {
				sco = aiNext.ca;
			}
		} else {
			mco = aiNext.ca;
			if( y - 1 > 0 ) {
				sco = aiNext.cb;
			}
		}
	} else {
		mx = x;
		my = y;
		mst = MB_CAPS_L;
		sx = x+1;
		sy = y;
		sst = MB_CAPS_R;
		if( af->rev == FALSE ) {
			mco = aiNext.ca;
			sco = aiNext.cb;
		} else {
			mco = aiNext.cb;
			sco = aiNext.ca;
		}
	}

	return aifRensaCheckCore(upw ,af ,mx,my,mco,mst, sx,sy,sco,sst);
}

//////////////////////////////////////////////////////////////////////////////
// ﾗｲﾝｻｰﾁ Core

//*** 操作カプセルを置いた所の周辺ラインを検索 ************************
//	aifSearchLineCore
//in:	int			mx,my		検索する座標
//		int			fg			縦､横どこを検索するかﾌﾗｸﾞ(SL_???)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//out:	int						TRUE：消去でFALSE：未消去、NULL：消す前から何もない
//		u8			hei_data	消去ﾁｪｯｸ座標の縦ラインの情報
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//
int aifSearchLineCore
(
	int	mx,int my,	// ﾀｰｹﾞｯﾄﾎﾟｼﾞｼｮﾝ
	int	fg			// SL_???
){
	int	i,x,y,z,w,p;
	u8	tc = aif_field[my][mx].co;
	s8	rx,ry;

	for( i=0;i<LnTableMax;i++ ) {
		hei_data[i] = wid_data[i] = 0;
	}
	if( aif_field[my][mx].st == MB_NOTHING ) {
		return NULL;
	}

	// Heightﾁｪｯｸ
	if( SL_OnlyWid != fg ) {
		for( y=my-1,z=TRUE;y>0 && y>my-4;y-- ) {						// ﾁｪｯｸ位置から上3個をｻｰﾁ
			if( aif_field[y][mx].co != tc ) {
				if( aif_field[y][mx].co != COL_NOTHING ) {
					y = 0;												// 違う色がある時はｻｰﾁ終了
				} else {
					z = FALSE;											// 何もないときは隣接ﾗｲﾝ数ｶｳﾝﾄﾌﾗｸﾞON
				}
			} else {
				hei_data[LnLinesAll]++;									// 同色数ｶｳﾝﾄ
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					hei_data[LnOnLinVrs]++;								// 同色のｳｨﾙｽのとき、ｳｨﾙｽ数ｶｳﾝﾄ
					if ( y < BADLINE ) hei_data[LnHighVrs] += BadLineRate[y][mx];			// 高い所ｶｳﾝﾄ
				} else {
					if ( y < BADLINE ) hei_data[LnHighCaps] += BadLineRate[y][mx];			// 高い所ｶｳﾝﾄ
				}
				if( z ) hei_data[LnRinsetsu]++;							// 一度空白を挟んだとき隣接数もｶｳﾝﾄ
			}
			if( y > 0 ) hei_data[LnLinSpace]++;							// 同色か物が置けるﾗｲﾝ数ｶｳﾝﾄ（4個並べられるかのﾁｪｯｸ用）
		}
		for( y=my+1,z=TRUE,w=0;y<17;y++,w++ ) {							// ﾁｪｯｸ位置からした全部をｻｰﾁ（落下後に揃う場合をﾁｪｯｸするため）
			if( aif_field[y][mx].co != tc ) {
				if( aif_field[y][mx].co != COL_NOTHING ) {
					y = 17;
				} else {
					z = FALSE;
				}
			} else {
				hei_data[LnLinesAll]++;
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					if( w < 3 ) {											// ｳｨﾙｽは落ちないのでﾁｪｯｸ位置から3個のみﾁｪｯｸ
						hei_data[LnOnLinVrs]++;
						if ( y < BADLINE ) hei_data[LnHighVrs] += BadLineRate[y][mx];			// 高い所ｶｳﾝﾄ
					}
				} else {
					if ( y < BADLINE ) hei_data[LnHighCaps] += BadLineRate[y][mx];				// 高い所ｶｳﾝﾄ
				}
				if( z ) hei_data[LnRinsetsu]++;
				hei_data[LnLinSpace]++;
			}
		}
		hei_data[LnRinsetsu]++;											// 自分の数を足す
		hei_data[LnLinesAll]++;
		hei_data[LnLinSpace]++;
	}

	// Widthﾁｪｯｸ
	if( SL_OnlyHei != fg ) {
		for( x=mx-1,z=w=TRUE;x>-1 && x>mx-4;x-- ) {						// ﾁｪｯｸ位置から左3個をｻｰﾁ
			if( aif_field[my][x].co != tc ) {
				if( aif_field[my][x].co != COL_NOTHING ) {
					x = -1;												// 違う色があるときはｻｰﾁ終了
				} else {												// 何もないとき
					z = FALSE;											//	隣接数ｶｳﾝﾄﾌﾗｸﾞON
					if( w ) {
						for(i=p=0;i<aiFlagCnt;i++) {					// その空白位置に物が置けるかどうかﾁｪｯｸ
							if( aiFlag[i].ok == TRUE ) {
								if( aiFlag[i].tory == tate ) {
									if( aiFlag[i].x -1 == x ) {
										if(	aiFlag[i].y == my || aiFlag[i].y == my+1 ) {
											p = TRUE;					// 物が置ける場合、
											wid_data[LnLinSpace]++;		// 物が置けるﾗｲﾝ数ｶｳﾝﾄ（4個並べられるかのﾁｪｯｸ用）
											i=aiFlagCnt;				// 空白ﾁｪｯｸ終了
										}
									}
								}
							}
						}
						if( !p ) {										// 空白で物が置けない場合、それより先は連鎖しない場所とする
							w = FALSE;
						}
					}
				}
			} else {
				wid_data[LnLinesAll]++;									// 同色数ｶｳﾝﾄ
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnOnLinVrs]++;								// 同色ｳｨﾙｽ数ｶｳﾝﾄ
					if ( my < BADLINE ) wid_data[LnHighVrs] += BadLineRate[my][x];			// 高い所ｶｳﾝﾄ
				} else {
					if ( my < BADLINE ) wid_data[LnHighCaps] += BadLineRate[my][x];			// 高い所ｶｳﾝﾄ
				}
				if( z ) wid_data[LnRinsetsu]++;							// 隣接数ｶｳﾝﾄ
				if( w ) wid_data[LnLinSpace]++;							// 同色か物が置けるﾗｲﾝ数ｶｳﾝﾄ
			}
		}
		for( x=mx+1,z=w=TRUE;x<mx+4 && x<8;x++ ) {						// ﾁｪｯｸ位置から右3個をｻｰﾁ
			if( aif_field[my][x].co != tc ) {
				if( aif_field[my][x].co != COL_NOTHING ) {
					x = 8;
				} else {
					z = FALSE;
					if( w ) {
						for(i=p=0;i<aiFlagCnt;i++) {
							if( aiFlag[i].ok == TRUE ) {
								if( aiFlag[i].tory == tate ) {
									if( aiFlag[i].x -1 == x ) {
										if(	aiFlag[i].y == my || aiFlag[i].y == my+1 ) {
											p = TRUE;
											wid_data[LnLinSpace]++;
											i=aiFlagCnt;
										}
									}
								}
							}
						}
						if( !p ) {
							w = FALSE;
						}
					}
				}
			} else {
				wid_data[LnLinesAll]++;
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnOnLinVrs]++;
					if ( my < BADLINE ) wid_data[LnHighVrs] += BadLineRate[my][x];				// 高い所ｶｳﾝﾄ
				} else {
					if ( my < BADLINE ) wid_data[LnHighCaps] += BadLineRate[my][x];				// 高い所ｶｳﾝﾄ
				}
				if( z ) wid_data[LnRinsetsu]++;
				if( w ) wid_data[LnLinSpace]++;
			}
		}
		wid_data[LnRinsetsu]++;											// 自分の数を足す
		wid_data[LnLinesAll]++;
		wid_data[LnLinSpace]++;
	}

	// 消去ﾗｲﾝ数ﾁｪｯｸ
	z = FALSE;
	if( hei_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		hei_data[LnEraseLin] = 1;
		if( hei_data[LnRinsetsu] == DELETE_LINE*2 ) {
			hei_data[LnEraseLin]++;
		}
	}
	if( wid_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		wid_data[LnEraseLin] = 1;
		if( wid_data[LnRinsetsu] == DELETE_LINE*2 ) {
			wid_data[LnEraseLin]++;
		}
	}

	// 消去時"未消去側ﾗｲﾝ"のﾎﾟｲﾝﾄ計算を無効にする
	if( z ) {
		if( hei_data[LnEraseLin] ) {
			if( !wid_data[LnEraseLin] ) {
				wid_data[LnNonCount] = TRUE;
			}
		} else {
			if( !hei_data[LnEraseLin] ) {
				hei_data[LnNonCount] = TRUE;
			}
		}
	}

	return z;

}

//*** ラインごとのﾎﾟｲﾝﾄ計算処理 ************************
//	aifMiniPointK3
//in:	u8			*tbl		消去ﾁｪｯｸ座標のラインの情報のﾎﾟｲﾝﾀ
//		u8			sub			このラインはﾎﾟｲﾝﾄ算出上ﾒｲﾝかｻﾌﾞか？（TRUE:ｻﾌﾞ）
//		u8			*elin		消去ﾗｲﾝ数保存ﾜｰｸのﾎﾟｲﾝﾀ
//		u8			flag		縦横どちらのﾎﾟｲﾝﾄを算出するか(TRUE:縦)
//		u8			tory		操作ｶﾌﾟｾﾙの向き
//		u8			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//out:	int						算出ポイント数
//
int	aifMiniPointK3( u8* tbl,u8 sub,u8* elin,u8 flag,u8 tory,u8 ec)
{
	int ex = 0,i;
	if( tbl[LnEraseLin] != 0 ) {							// 消去するとき足すポイント
		*elin += tbl[LnEraseLin];

		tbl[LnHighCaps] = 0;
		tbl[LnHighVrs] = 0;

		for( i=LnEraseVrs;i<=LnHighVrs;i++ ) {
			ex = ex +( (int)tbl[i] * (int)pri_point[i] );
		}
	} else {												// 消去しないときに足すポイント
		if( !tbl[LnNonCount] ) {
			if( tbl[LnLinSpace] >= DELETE_LINE ) {
				if ( flag ) {
					if ( (!ec) || (tory != tate) || (tbl[LnLinesAll] > 2) )
						ex = ex +( HeiLinesAllp[tbl[LnLinesAll]] );
				} else {
					if ( (!ec) || (tory != yoko) || (tbl[LnLinesAll] > 2) )
						ex = ex +( WidLinesAllp[tbl[LnLinesAll]] );
				}
				ex = ex +( (int)tbl[LnOnLinVrs] * (int)pri_point[LnOnLinVrs] );
			}
		}
	}
	if( sub == TRUE ) {
		ex /= Sub_Divide;
	}
	return ex;
}

//*** 色の違う所にカプセルを置いてしまったかどうかのチェック ************************
//	aifMiniAloneCapNumber
//in:	u8			x,y			置いた位置をﾁｪｯｸする座標
//		u8			f			落下したかどうか(TRUE:落下した)
//		int			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//		u8			hei_data	消去ﾁｪｯｸ座標の縦ラインの情報
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//out:	int						違う色の所においてしまった時の状態番号(ALN_????)
//
int	aifMiniAloneCapNumber
(
	u8 x,u8 y,	//
	u8 f,		// TRUE = 落下した
	int	ec
){
	int	m = ALN_FALSE;
	int d = aif_field[y+1][x].st;
	if( (( hei_data[LnRinsetsu] == 1 || hei_data[LnRinsetsu] != 0 && hei_data[LnLinSpace] < DELETE_LINE )&&
		( wid_data[LnRinsetsu] == 1 || wid_data[LnRinsetsu] != 0 && wid_data[LnLinSpace] < DELETE_LINE )) ||
		( (ec == TRUE) && (hei_data[LnRinsetsu] > 0) && (wid_data[LnRinsetsu] > 0) && (((hei_data[LnLinSpace]<DELETE_LINE)?0:hei_data[LnRinsetsu])+((wid_data[LnLinSpace]<DELETE_LINE)?0:wid_data[LnRinsetsu]) <= 3) ) ) {
//		( (ec == TRUE) && (hei_data[LnRinsetsu]+wid_data[LnRinsetsu] > 9) ) ) {
			if( y == 16 ) {
				m = ALN_Bottom;
			} else {
				if( f ) {
					if( d < MB_VIRUS_A ) {
						m = ALN_FallCap;
					} else {
						m = ALN_FallVrs;
					}
				} else {
					if( d < MB_VIRUS_A ) {
						m = ALN_Capsule;
					} else {
						m = ALN_Virus;
					}
				}
			}
	}
	return m;
}

//*** 色の違う所にカプセルを置いてしまったかどうかのチェック（横ライン専用） ************************
//	aifMiniAloneCapNumber
//in:	u8			x,y			置いた位置をﾁｪｯｸする座標
//		u8			f			落下したかどうか(TRUE:落下した)
//		int			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//out:	int						違う色の所においてしまった時の状態番号(ALN_????)
//
int	aifMiniAloneCapNumberW
(
	u8 x,u8 y,	//
	u8 f,		// TRUE = 落下した
	int	ec
){
	int	m = ALN_FALSE;
	int d = aif_field[y+1][x].st;
//	if( ( wid_data[LnRinsetsu] == 1 || wid_data[LnRinsetsu] != 0 && wid_data[LnLinSpace] < DELETE_LINE ) ||
//		( (ec == TRUE) && ((wid_data[LnLinSpace] < DELETE_LINE) || (wid_data[LnRinsetsu] <= 2)) ) ) {
	if( ( wid_data[LnRinsetsu] == 1 ) ||
		( (ec == TRUE) && (wid_data[LnRinsetsu] == 2) ) ) {
			if( y == 16 ) {
				m = ALN_Bottom;
			} else {
				if( f ) {
					if( d < MB_VIRUS_A ) {
						m = ALN_FallCap;
					} else {
						m = ALN_FallVrs;
					}
				} else {
					if( d < MB_VIRUS_A ) {
						m = ALN_Capsule;
					} else {
						m = ALN_Virus;
					}
				}
			}
	}
	return m;
}



//-----------------------------------------------------------------------------
// 指定の場所はフラッシュか？
//-----------------------------------------------------------------------------
static int flash_virus(int x, int y)
{
	int	i;

	for ( i = 0; i < pGameState->flash_virus_count; i++ ) {
		if ( pGameState->flash_virus_pos[i][2] >= 0 ) {
			if ( x == pGameState->flash_virus_pos[i][0] && y == pGameState->flash_virus_pos[i][1] ) {
				return ( pGameState->flash_virus_pos[i][2] );
			}
		}
	}

	return(-1);
}



//-----------------------------------------------------------------------------
// 指定の場所から下に見て、フラッシュがあるか？
//-----------------------------------------------------------------------------
static int search_Vflash(int x, int y, int col)
{
	int	i, j;

	for ( i = (y+1); i <= (y+3); i++ ) {
		if ( i >= FIELDY ) return(FALSE);
		if ( aiFieldData[i][x].co == COL_NOTHING ) continue;
		if ( aiFieldData[i][x].co != col ) break;
		if ( aiFieldData[i][x].co == col ) {
			for ( j = 0; j < pGameState->flash_virus_count; j++ ) {
				if ( pGameState->flash_virus_pos[j][2] >= 0 && x == pGameState->flash_virus_pos[j][0] ) return(TRUE);
			}
			break;
		}
	}

	return(FALSE);
}



//-----------------------------------------------------------------------------
// 指定の場所から左右に見て、同じ色が続きながらフラッシュがあるか？
//-----------------------------------------------------------------------------
static int search_Lflash(int x, int y, int col)
{
	int	i;

	for ( i = 0; i < pGameState->flash_virus_count; i++ ) {
		if ( pGameState->flash_virus_pos[i][2] >= 0 && y == pGameState->flash_virus_pos[i][1] ) {
			return(TRUE);
		}
	}
	return(FALSE);
}



//*** 優先順位決定用ポイント数計算処理メイン ************************
//	aifSearchLineMS
//in:	AI_FLAG		af			移動ｶﾌﾟｾﾙの状態
//		int			mx,my,mco	操作ｶﾌﾟｾﾙのﾏｽﾀｰの座標x,yと色
//		int			sx,sy,sco	操作ｶﾌﾟｾﾙのｽﾚｰﾌﾞの座標x,yと色
//		int			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//out:	int						1:ﾏｽﾀｰが消去、2:ｽﾚｰﾌﾞが消去、0:消去していない
//		AI_FLAG		pri			ﾎﾟｲﾝﾄ計算算出値
//
int aifSearchLineMS
(
	AI_FLAG*  af,
	int	mx,	int	my,	int	mco,	// master
	int	sx,	int	sy,	int	sco,	// slave
	int	ec						// 同色 FALSE/TRUE
){
	int	i,f,z,z2,x,y,j,m,s,opt;
	int	my2,sy2;
	u8	ss[8];

	my2 = my;
	sy2 = sy;
	opt = 0;

	for(j=0;j<LnTableMax;j++) {
		af->wid[LnMaster][j] = af->hei[LnMaster][j] = af->wid[LnSlave][j] = af->hei[LnSlave][j] = 0;
	}
	af->elin[FALSE] = af->elin[TRUE] = 0;
	af->sub = FALSE;
	af->only[LnMaster] = af->only[LnSlave] = FALSE;
	af->wonly[LnMaster] = af->wonly[LnSlave] = FALSE;

	z = aifSearchLineCore(mx,my,SL_Both);								// ｶﾌﾟｾﾙのﾏｽﾀｰ側のﾗｲﾝをｻｰﾁ (Z=0:消えなかった、>0:消えた)
	z2 = FALSE;
	if ( aifEraseLineCore(mx,my) ) {					// ﾏｽﾀｰを消去消してみる
		opt = 1;
		af->pri += 10000;
	}
	if ( !z ) {
		af->only[LnMaster] = aifMiniAloneCapNumber(mx,my,FALSE,ec);			// ﾏｽﾀｰの止まる位置をｻｰﾁ
		af->wonly[LnMaster] = aifMiniAloneCapNumberW(mx,my,FALSE,ec);			// ﾏｽﾀｰの止まる位置をｻｰﾁ
	}

	for(i=0;i<LnTableMax;i++) {
		af->hei[LnMaster][i] = hei_data[i];
		af->wid[LnMaster][i] = wid_data[i];
	}
	if( aif_field[sy][sx].st != MB_NOTHING ) {							// ﾏｽﾀｰ消去でｽﾚｰﾌﾞが消えてない時
		if( ec ) {														// 同色ｶﾌﾟｾﾙはﾏｽﾀｰ検索時でｶﾌﾟｾﾙ向きのﾗｲﾝは検索済み残りﾗｲﾝのみ検索
			if( af->tory == tate ) {
				f = SL_OnlyWid;
			} else {
				f = SL_OnlyHei;
			}
		} else {
			f = SL_Both;
		}
		m = FALSE;
		if( FALSE == (z2 = aifSearchLineCore(sx,sy,f)) ) {				// 落ちる前にはｽﾚｰﾌﾞは消えなかった時
			if( z ) {
				for( y=16;y>sy;y-- ) {									// ﾏｽﾀｰで消せていてｽﾚｰﾌﾞが消えなかった時、ｽﾚｰﾌﾞ下のｶﾌﾟｾﾙｦ落す
					if( aif_field[y][sx].st == MB_CAPS_BALL ) {			// ﾏｽﾀｰ消去によりｽﾚｰﾌﾞ下のｶﾌﾟｾﾙが落ちるかもしれないので落ちるなら落す
						for( i=y+1;i<17 && aif_field[i][sx].st == MB_NOTHING;i++ );
						if( i!=y+1 ) {
							aif_field[i-1][sx].st = MB_CAPS_BALL;
							aif_field[i-1][sx].co = aif_field[y][sx].co;
							aif_field[ y ][sx].st = MB_NOTHING;
							aif_field[ y ][sx].co = COL_NOTHING;
						}
					}
				}
			}
			for( i=sy+1;i<17 && aif_field[i][sx].st == MB_NOTHING;i++ );// ｽﾚｰﾌﾞを落す
			if( i!=sy+1 ) {
				m = TRUE;
				aif_field[i-1][sx].st = MB_CAPS_BALL;
				aif_field[i-1][sx].co = aif_field[sy][sx].co;
				aif_field[sy ][sx].st = MB_NOTHING;
				aif_field[sy ][sx].co = COL_NOTHING;
				sy=i-1;
				if( !z ) af->sub = TRUE;
				z2 = aifSearchLineCore(sx,sy,SL_Both);
			}
		}
		if ( aifEraseLineCore(sx,sy) ) {								// ﾗｲﾝ消去してみる
			opt = 1;
			af->pri += 10000;
		}
		if ( !z2 ) {
			af->only[LnSlave] = aifMiniAloneCapNumber(sx,sy,m,ec);			// ｽﾚｰﾌﾞを止まる位置に移動
			af->wonly[LnSlave] = aifMiniAloneCapNumberW(sx,sy,m,ec);		// ｽﾚｰﾌﾞを止まる位置に移動
		}
		for(i=0;i<LnTableMax;i++) {
			af->hei[LnSlave][i] = hei_data[i];
			af->wid[LnSlave][i] = wid_data[i];
		}
	}

	//-----------------------------------------------------------------------------
	// フラッシュモード
	if ( evs_gamemode == GMD_FLASH ) {
		OnVirusP = OnVirusP_org + 2000;
		// 下にフラッシュがあるのなら掘りやすくする
		if ( opt == 0 ) {
			if ( ec ) {
				if ( search_Vflash(mx, my, mco) ) opt = 1;
				if ( search_Vflash(sx, sy, sco) ) opt = 1;
				if ( opt ) af->pri += 3000;
			} else {
				if ( af->tory == yoko ) {
					if ( search_Vflash(mx, my, mco) ) opt = 1;
					if ( search_Vflash(sx, sy, sco) ) opt = 1;
					if ( opt ) af->pri += 1000;
				}
				//if ( search_Lflash(mx, my, mco) ) opt = 1;
				//if ( search_Lflash(sx, sy, sco) ) opt = 1;
			}
		}
	}
	//-----------------------------------------------------------------------------

	// ﾎﾟｲﾝﾄ計算
	af->pri += aifMiniPointK3( &af->hei[LnMaster][0],FALSE	,&af->elin[FALSE  ] ,TRUE ,af->tory ,ec );
	af->pri += aifMiniPointK3( &af->hei[LnSlave ][0],af->sub,&af->elin[af->sub] ,TRUE ,af->tory ,ec );
	af->pri += aifMiniPointK3( &af->wid[LnMaster][0],FALSE	,&af->elin[FALSE  ] ,FALSE ,af->tory ,ec );
	af->pri += aifMiniPointK3( &af->wid[LnSlave ][0],af->sub,&af->elin[af->sub] ,FALSE ,af->tory ,ec );

	if ( AloneCapP[af->only[LnMaster]] ) af->pri += AloneCapP[af->only[LnMaster]];
	if ( AloneCapP[af->only[LnSlave ]] ) af->pri += AloneCapP[af->only[LnSlave ]];
	if ( AloneCapP[af->only[LnMaster]] != 0 && AloneCapP[af->only[LnSlave ]] != 0 ) af->pri -= (FIELDY - my2)*LPriP;

	if ( AloneCapWP[af->wonly[LnMaster]] ) af->pri += AloneCapWP[af->only[LnMaster]];
	if ( AloneCapWP[af->wonly[LnSlave ]] ) af->pri += AloneCapWP[af->only[LnSlave ]];
//	af->pri += EraseLinP[af->elin[FALSE]];
//	af->pri += EraseLinP[af->elin[TRUE]]/Sub_Divide;
	af->pri += (s32)((float)EraseLinP[af->hei[LnMaster][LnEraseLin]+af->hei[LnSlave][LnEraseLin]] * HeiEraseLinRate);
	af->pri += (s32)((float)EraseLinP[af->wid[LnMaster][LnEraseLin]+af->wid[LnSlave][LnEraseLin]] * WidEraseLinRate);

	// ｳｨﾙｽ上置き加点（終盤ｳｨﾙｽの上を優先的に消す処理）
	if ( OnVirusP && my2 < FIELDY -1 ) {
		m = TRUE;
		for(y = my2-1;y > 3;y--) {
			if ( MB_VIRUS_A <= aiFieldData[y][mx].st && aiFieldData[y][mx].st <= MB_VIRUS_C ) {		// Ｍの上の方にｳｨﾙｽがある時
				m = FALSE;
				break;
			}
		}
		if ( m == TRUE ) {
			m = FALSE;
			for(y = my2+1;y < FIELDY;y++) {
				if ( MB_VIRUS_A <= aiFieldData[y][mx].st && aiFieldData[y][mx].st <= MB_VIRUS_C ) {		// Ｍの下の方にｳｨﾙｽがある時
					m = TRUE;
					break;
				}
			}
		}
		s = TRUE;
		for(y = sy2-1;y > 3;y--) {
			if ( MB_VIRUS_A <= aiFieldData[y][sx].st && aiFieldData[y][sx].st <= MB_VIRUS_C ) {		// Ｓの上の方にｳｨﾙｽがある時
				s = FALSE;
				break;
			}
		}
		if ( s ) {
			s = FALSE;
			for(y = sy2+1;y < FIELDY;y++) {
				if ( MB_VIRUS_A <= aiFieldData[y][sx].st && aiFieldData[y][sx].st <= MB_VIRUS_C ) {		// Ｓの下の方にｳｨﾙｽがある時
					s = TRUE;
					break;
				}
			}
		}


		// フラッシュウイルスボーナス点
		opt = 0;
		if ( evs_gamemode == GMD_FLASH ) {
			if ( m ) {
				for(y = my2+1;y < FIELDY;y++) {
					if ( flash_virus(mx, y) != -1 ) opt += OnVirusP;
				}
			}
			if ( s ) {
				for(y = sy2+1;y < FIELDY;y++) {
					if ( flash_virus(sx, y) != -1 ) opt += OnVirusP;
				}
			}
		}


		if ( aiFieldData[my+1][mx].st <= MB_VIRUS_C ) {			// ﾏｽﾀｰとｽﾚｰﾌﾞのどちらが引っかかった?
			if ( m ) {
				if ( af->hei[LnMaster][LnRinsetsu] > ((ec && af->tory == tate)?2:1) && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
					if ( af->tory == tate ) {
						if ( ec ) {
							af->pri += ( OnVirusP + opt ) * 2;	// 同色の場合２倍点を入れる
						} else if ( !z ) {
							af->pri -= OnVirusP * 2;
						}
					} else {
						if ( s ) {
							if ( af->hei[LnSlave][LnLinesAll] > 1 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
								af->pri += ( OnVirusP + opt );
							} else {
								af->pri -= OnVirusP * 2;
							}
						} else {
							af->pri += ( OnVirusP + opt );
						}
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			} else if ( s ) {
				if ( af->hei[LnSlave][LnLinesAll] > 1 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
					if ( af->hei[LnMaster][LnRinsetsu] >= 3 && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
						af->pri += ( OnVirusP + opt );
					} else {
						af->pri -= OnVirusP * 2;
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			}
		} else {
			if ( s ) {
				if ( af->hei[LnSlave][LnRinsetsu] > 1 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
					if ( m ) {
						if ( af->hei[LnMaster][LnLinesAll] > 1 && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
							af->pri += ( OnVirusP + opt );
						} else {
							af->pri -= OnVirusP * 2;
						}
					} else {
						af->pri += ( OnVirusP + opt );
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			} else if ( m ) {
				if ( af->hei[LnMaster][LnLinesAll] > 1 && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
					if ( af->hei[LnSlave][LnRinsetsu] >= 3 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
						af->pri += ( OnVirusP + opt );
					} else {
						af->pri -= OnVirusP * 2;
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			}
		}

		if ( z && !ec && af->tory == tate && my2 < FIELDY - 1 ) {
			i = FALSE;
			for(m = my2+1;m < FIELDY;m++) {
				if ( aiFieldData[m][sx].st > MB_VIRUS_C || aiFieldData[m][sx].co != mco ) {
					i = TRUE;
					break;
				}
			}
			if ( i ) {
				i = FALSE;
				for(y = m;y < FIELDY;y++) {
					if ( MB_VIRUS_A <= aiFieldData[y][sx].st && aiFieldData[y][sx].st <= MB_VIRUS_C ) {
						i = TRUE;
						break;
					}
				}
			}
			if ( i ) {
				for(y = m;y < FIELDY;y++) {
					if ( aiFieldData[y][sx].st <= MB_VIRUS_C ) {
						if ( aiFieldData[y][sx].co == sco ) {
							af->pri += ( OnVirusP + opt ) * 4;
						} else {
							af->pri -= OnVirusP * 2;
						}
						break;
					}
				}
			}
		}
	}
#if 0 // 削除
/*
	// ｳｨﾙｽ横上単一カプセル置き減点
	if ( (AloneCapP[af->only[LnSlave]] == ALN_Capsule || AloneCapP[af->only[LnSlave]] == ALN_Virus)
	 && (my < FIELDY - 1 && ) ) {
		for(y = my+1;y < FIELDY;y++) {
			if ( MB_VIRUS_A <= aiFieldData[y][mx].st && aiFieldData[y][mx].st <= MB_VIRUS_C ) {
				af->pri += OnVirusP;
				break;
			}
		}
	}

	// 高消し加点	あまり意味がない
	if ( my <= BADLINE || sy <= BADLINE ) {
		if ( af->hei[LnMaster][LnEraseLin] + af->hei[LnSlave][LnEraseLin] + af->wid[LnMaster][LnEraseLin] + af->wid[LnSlave][LnEraseLin] ) {
			af->pri += HiEraseP;
		}
	}

	// 中央寄り加点	逆に弱くなる
	af->pri -= abs(mx*20 - 70) + abs(sx*20 - 70);
*/
#endif
	// 高積み減点
	for(x=0;x<8;x++) {
		for(y=1;y<17;y++) {
			if( aif_field[y][x].st != MB_NOTHING ) {
				ss[x] = y-1;
				break;
			}
		}
	}
	af->dead = 0;
	m = FALSE;
	if( ss[sx] < BADLINE && sy <= BADLINE && sy > 0 ) {
		if( aif_field[sy][sx].st != MB_NOTHING ) {
			if( af->hei[LnSlave][LnRinsetsu] + ss[sx] < 4 ) {
				af->dead += bad_point[sx] / (sy*2-1);
				m = TRUE;
			}
		}
	}
	if( ss[mx] < BADLINE && my < BADLINE && my > 0 && (!ec || af->tory != tate) ) {
		if( aif_field[my][mx].st != MB_NOTHING ) {
			if( af->hei[LnMaster][LnRinsetsu] + ss[mx] < 4 ) {
				if ( m ) {
					af->dead += bad_point2[mx] + bad_point2[sx] - bad_point[sx] / (sy*2-1);
				} else {
					af->dead += bad_point[mx] / (my*2-1);
				}
			}
		}
	}
	af->pri += af->dead;

	// 左右に壁が出来たとき、壁を優先的に消す処理
	if ( aiWall ) {
		if ( WallRate[aiWall][mx] > WallRate[aiWall][sx] ) {
			af->pri = af->pri*(s32)WallRate[aiWall][mx]/10;
		} else {
			af->pri = af->pri*(s32)WallRate[aiWall][sx]/10;
		}
	}

	if ( z ) return 1;
	if ( z2 ) return 2;
	return	0;
}

//*** 置き場所決定 ************************
//	aiHiruAllPriSet
//in:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//		AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//out:	u8			decide		最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号
//
void aiHiruAllPriSet(game_state *xpw)
{
	AI_FLAG*	af;
	int	i,j,x,y,z,f,t,r;
	u8		mx,my,mco,mst;
	u8		sx,sy,sco,sst;
	u8		rcr,rcy,rcb,rco;

	// 要素計算
	for( i=0;i<aiFlagCnt;i++ ) {
		//aiFlag[i].pri = 0;
		if( aiFlag[i].ok != 0 ) {
			af = &aiFlag[i];
			bcopy(&aiFieldData,&aif_field,sizeof(AI_FIELD)*(FIELDY)*(FIELDX));
			x = af->x-1;
			y = af->y;
			f = FALSE;
			if( af->tory == tate ) {
				mx = x;
				my = y;
				mst = MB_CAPS_D;
				sx = x;
				sy = y - 1;
				if( y - 1 > 0 ) {
					sst = MB_CAPS_U;
				}
				if( af->rev == FALSE ) {
					mco = aiNext.cb;
					if( y - 1 > 0 ) {
						sco = aiNext.ca;
					}
				} else {
					mco = aiNext.ca;
					if( y - 1 > 0 ) {
						sco = aiNext.cb;
					}
				}
				if ( my ) {
					aif_field[my][mx].st = mst;
					aif_field[my][mx].co = mco;
				}
				if ( sy ) {
					aif_field[sy][sx].st = sst;
					aif_field[sy][sx].co = sco;
				}
				if( aif_field[y-1][x].co == aif_field[y][x].co ) f = TRUE;
				z = aifSearchLineMS(af,x,y,mco,x,y-1,sco,f);
			} else {
				if( aif_field[y+1][x].st != MB_NOTHING ) {
					mx = x;
					my = y;
					mst = MB_CAPS_L;
					sx = x+1;
					sy = y;
					sst = MB_CAPS_R;
					if( af->rev == FALSE ) {
						mco = aiNext.ca;
						sco = aiNext.cb;
					} else {
						mco = aiNext.cb;
						sco = aiNext.ca;
					}
				} else {
					mx = x+1;
					my = y;
					mst = MB_CAPS_R;
					sx = x;
					sy = y;
					sst = MB_CAPS_L;
					if( af->rev == FALSE ) {
						mco = aiNext.cb;
						sco = aiNext.ca;
					} else {
						mco = aiNext.ca;
						sco = aiNext.cb;
					}
				}
				if( mco == sco ) f = TRUE;
				if ( my ) {
					aif_field[my][mx].st = mst;
					aif_field[my][mx].co = mco;
				}
				if ( sy ) {
					aif_field[sy][sx].st = sst;
					aif_field[sy][sx].co = sco;
				}
				z = aifSearchLineMS(af,mx,my,mco,sx,sy,sco,f);
			}
			if (z && RensaP) {													// ｶﾌﾟｾﾙが消える時３種類の消えない方の色で連鎖ﾁｪｯｸ
				if ( z == 2 ) {
					rco = mx;mx = sx;sx = rco;
					rco = my;my = sy;sy = rco;
					rco = mco;mco = sco;sco = rco;
					rco = mst;mst = sst;sst = rco;
				}
				rcr = aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
				aiHiErR = aiHiEraseCtr;
				rcy = aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
				aiHiErY = aiHiEraseCtr;
				rcb = aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
				aiHiErB = aiHiEraseCtr;
				rco = FALSE;
				if ( mx == sx ) {
					if ( my > sy ) {
						mst = MB_CAPS_R;
						sx--;
						sy++;
						sst = MB_CAPS_L;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
						mst = MB_CAPS_L;
						sx += 2;
						sst = MB_CAPS_R;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
					} else {
						mst = MB_CAPS_R;
						sx--;
						sy--;
						sst = MB_CAPS_L;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
						mst = MB_CAPS_L;
						sx += 2;
						sst = MB_CAPS_R;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
					}
				} else if ( mx > sx ) {
					mst = MB_CAPS_L;
					sx += 2;
					sst = MB_CAPS_R;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
					mst = MB_CAPS_D;
					sx--;
					sy--;
					sst = MB_CAPS_U;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
				} else {
					mst = MB_CAPS_R;
					sx -= 2;
					sst = MB_CAPS_L;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
					mst = MB_CAPS_D;
					sx++;
					sy--;
					sst = MB_CAPS_U;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
				}
				switch ( sco ) {
				case COL_RED:
					af->pri += aiHiErR * pri_point[LnHighCaps];
					if ( rcr ) {
						if ( rcy || rcb || rco ) {
							af->pri += RensaP * rcr;
						} else {
							af->pri += RensaP * rcr;
//osSyncPrintf("+ %d,%d\n",rcy|rcb,rco);
						}
					} else {
						if ( rcy || rcb || rco ) {
							if ( my > 2 ) af->pri += RensaMP;
//osSyncPrintf("- %d,%d\n",rcy|rcb,rco);
						} else {
							af->pri += 0;
						}
					}
					break;
				case COL_YELLOW:
					af->pri += aiHiErY * pri_point[LnHighCaps];
					if ( rcy ) {
						if ( rcr || rcb || rco ) {
							af->pri += RensaP * rcy;
						} else {
							af->pri += RensaP * rcy;
//osSyncPrintf("+ %d,%d\n",rcr|rcb,rco);
						}
					} else {
						if ( rcr || rcb || rco ) {
							if ( my > 2 ) af->pri += RensaMP;
//osSyncPrintf("- %d,%d\n",rcr|rcb,rco);
						} else {
							af->pri += 0;
						}
					}
					break;
				case COL_BLUE:
					af->pri += aiHiErB * pri_point[LnHighCaps];
					if ( rcb ) {
						if ( rcy || rcr || rco ) {
							af->pri += RensaP * rcb;
						} else {
							af->pri += RensaP * rcb;
//osSyncPrintf("+ %d,%d\n",rcy|rcy,rco);
						}
					} else {
						if ( rcy || rcr || rco ) {
							if ( my > 2 ) af->pri += RensaMP;
//osSyncPrintf("- %d,%d\n",rcy|rcy,rco);
						} else {
							af->pri += 0;
						}
					}
					break;
				}
			}
		} else {
			aiFlag[i].pri = NonTouch;
		}
	}

	// 一番下のに1点入れておく(今はもう無意味)
/*	for( i=j=y=0;i<aiFlagCnt;i++ ) {
		if( aiFlag[i].pri != NonTouch && aiFlag[i].y >= y ) {
			y = aiFlag[i].y;
			j = i;
		}
	}
	aiFlag[j].pri += 1;
*/
	// 最上位ﾎﾟｲﾝﾄは誰だ
	if ( xpw->ai.aiRandFlag ) {								// ４Ｐ用同じ思考のとき全く同じ操作をしないよう１回だけ乱数を加える処理
		for( i=z=0,x=NonTouch-1;i<aiFlagCnt;i++ ) {
			if ( aiPriOfs ) {
				y = genrand(aiPriOfs);
			} else {
				y = 0;
			}
			if( aiFlag[i].pri + y > x ) {
				x = aiFlag[i].pri + y;
				z = i;
			}
		}
	} else {
		for( i=z=0,x=t=NonTouch-1;i<aiFlagCnt;i++ ) {
			if ( aiPriOfs ) {
				y = genrand(100+aiPriOfs);
			} else {
				y = genrand(100);
			}
			if( aiFlag[i].pri + y > t ) {
				t = aiFlag[i].pri + y;
				if ( aiFlag[i].pri > x ) x = aiFlag[i].pri;
				z = i;
			}
			if ( aiFlag[z].pri < x ) xpw->ai.aiRandFlag = TRUE;
		}
	}

	y = aiFlag[z].y;
	x = aiFlag[z].x;
	t = aiFlag[z].tory;
	r = aiFlag[z].rev;
	decide = z;
}

//*** キャラクターの性格設定 ************************
//	aiSetCharacter
//in:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//		AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//		AI_DATA		ai_param	思考ﾊﾟﾗﾒｰﾀﾃｰﾌﾞﾙ
//out:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//
void aiSetCharacter(game_state *xpw)
{
	int		i,j,x,y,z,f,t,r;
	u8		badLine,leading,wall;
	u8		hicaps,lowvrs;
	int		vrsMax,vrsCnt;
	u8		char_no;
	int		aidt;

	AI_DATA	(*aiDataPtr)[][8];
	AI_CHAR *char_data;

	vrsMax = xpw->lv;			// ｳｲﾙｽ総数作成
	if ( vrsMax > LEVLIMIT ) vrsMax = LEVLIMIT;
	vrsMax = (vrsMax+1)*4;

	// 高積みチェック
	// badLine = 1:上から消去できる積み上げライン
	// badLine = 2:上から消去できない積み上げライン
	badLine = 0;
	for(x = 2;x < FIELDX - 2;x++) {
		i = 0;
		for(y = 1;y < BADLINE;y++) {
			if ( !i ) {
				if ( aiFieldData[y][x].st != MB_NOTHING ) {
					i = 1;
					j = aiFieldData[y][x].co;
				}
			}
			if ( i ) {
				if ( (aiFieldData[y+1][x].st != MB_NOTHING && aiFieldData[y+1][x].co != j) || (aiFieldData[y+1][x].st == MB_NOTHING) ) {
					if ( (x == 3) || (x == 4) ) i = 2;
					else i = 0;
					break;
				}
			}
		}
		if ( i ) {
			badLine = i;
			if ( i == 2 ) break;
		}
	}

	// 壁チェック（vrsCntに壁内のｳｨﾙｽ数を計算）
	wall = 0;
	hicaps = FIELDY;
	vrsCnt = 0;
	for(x = 4;x < FIELDX;x++) {
		for(y = 1;y < 4;y++) {
			if ( aiFieldData[y][x].st != MB_NOTHING ) {
				break;
			}
		}
		if ( y < 4 ) {
			wall |= 2;
			j = x;
			break;
		} else {
			for(;y < FIELDY;y++) {
				if ( aiFieldData[y][x].st != MB_NOTHING && hicaps > y ) hicaps = y;
				if ( MB_VIRUS_A <= aiFieldData[y][x].st && aiFieldData[y][x].st <= MB_VIRUS_C ) {
					vrsCnt++;
				}
			}
		}
	}
	for(x = 3;x >= 0;x--) {
		for(y = 1;y < 4;y++) {
			if ( aiFieldData[y][x].st != MB_NOTHING ) {
				break;
			}
		}
		if ( y < 4 ) {
			wall |= 1;
			if ( j - x <= 3 ) {
				wall = 0;
				vrsCnt = xpw->vs;
			}
			break;
		} else {
			for(;y < FIELDY;y++) {
				if ( aiFieldData[y][x].st != MB_NOTHING && hicaps > y ) hicaps = y;
				if ( MB_VIRUS_A <= aiFieldData[y][x].st && aiFieldData[y][x].st <= MB_VIRUS_C ) {
					vrsCnt++;
				}
			}
		}
	}

	// 一番下にあるｳｨﾙｽの位置
	lowvrs = 0;
	for(x = 0;x < FIELDX;x++) {
		for(y = FIELDY;y > 3;y--) {
			if ( MB_VIRUS_A <= aiFieldData[y][x].st && aiFieldData[y][x].st <= MB_VIRUS_C ) {
				if ( lowvrs < y ) lowvrs = y;
				break;
			}
		}
	}

	// 現在自分がトップかどうか？
	leading = TRUE;
	if ( evs_gamesel == GSL_VSCPU ) {
		for(i=0;i<evs_playcnt;i++) {
			if ( game_state_data[i].vs < game_state_data[aipn].vs ) leading = FALSE;
		}
	}

	// キャラ別思考選択
	char_no = xpw->think_type;
	aiDataPtr = &ai_param;

//	if ( aipn == 0 && evs_gamesel <= GSL_VSCPU ) { // GSL_1PLAY,GSL_2PLAY,GSL_4PLAY,GSL_VSCPU
	if ( aipn == 0 && aiDebugP1 >= 0 ) { // GSL_1PLAY,GSL_2PLAY,GSL_4PLAY,GSL_VSCPU
		char_no = aiDebugP1;
//		if ( char_no > CHR_PLAIN4 ) {
//			char_no -= CHR_PLAIN4 + 1;
//			aiDataPtr = &ai_param_org;
//		}
	}

	char_data = &ai_char_data[char_no];

	xpw->ai.aiRollHabit &= 0x01;
	xpw->ai.aiState &= ~AIF_AGAPE;
	xpw->ai.aiPriOfs = 0;
	aiPriOfs = 0;
//	aiSelSpeedRensa = -1;

	// 状態を取得
	if ( xpw->gs != AGST_GAME ) {
		// 死んだ後の練習モード
		aidt = AI_STATE_DEAD;
	} else {
		if ( aiRootP < 4.0 ) {
			aidt = AI_STATE_NARROW;
		} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {
			// 中央のｳｨﾙｽ数が８以下になった時終盤処理
			if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
				if ( vrsCnt ) {
					aidt = AI_STATE_LASTVS;
				} else {
					aidt = AI_STATE_LASTVSNH;
				}
			} else {
				aidt = AI_STATE_LASTVSN;
			}
		} else if ( badLine == 2 ) {
			// ｶﾌﾟｾﾙ入り口下にｶﾌﾟｾﾙが残っている時
			aidt = AI_STATE_BADLINE2;
		} else {
			// 中盤処理
			if ( badLine == 1 ) {
				aidt = AI_STATE_BADLINE1;
			} else {
				aidt = AI_STATE_NORMAL;
			}
		}
	}

	// キャラ毎の行動パターンを決定
	aiSelCom = char_data->logic[aidt];
	aiSelSpeed = char_data->speed;

	// フラッシュモード
	if ( evs_gamemode == GMD_FLASH ) {
		// フラッシュモードでは強制的に早消し
		if ( last_flash > 1 ) {
			aiSelCom = AI_LOGIC_FastErase;
		} else {
			aiSelCom = AI_LOGIC_WidErase;
		}
	}

	// 超ハード
	if ( s_hard_mode ) {
		if ( aidt == AI_STATE_NORMAL ) {
			switch ( aiSelCom ) {
				case AI_LOGIC_FastErase:
					aiSelCom = AI_LOGIC_RensaAndErase;
					break;
				case AI_LOGIC_SmallRensa:
					aiSelCom = AI_LOGIC_RensaAndAttack;
					break;
				case AI_LOGIC_WidErase:
					aiSelCom = AI_LOGIC_WidErase;
					break;
				case AI_LOGIC_MajorityRensa:
					aiSelCom = AI_LOGIC_MajorityRensa;
					break;
			}
		}
	}

	// あほモード
	if ( fool_mode == TRUE && ( framecont & 0x200 ) != 0 ) {
		// 特殊処理は発生させない
		for(j = 0; j < NUM_AI_EFFECT; j++) {
			xpw->ai.aiEffectNo[j] = AI_EFFECT_Ignore;
		}
		// 攻撃しない
		aiSelCom = AI_LOGIC_FastErase;
		// 遅くする
		//aiSelSpeed -= 2;
		//if ( aiSelSpeed < AI_SPEED_SUPERSLOW ) aiSelSpeed = AI_SPEED_SUPERSLOW;
	}

	for(i = 0; i < NUM_AI_EFFECT; i++) {
		int doEffectFlag = FALSE;

		// 特殊処理の持続時間が過ぎていないかチェックする
		xpw->ai.aiEffectCount[i] = MIN(999, xpw->ai.aiEffectCount[i] + 1);
		switch(xpw->ai.aiEffectNo[i]) {
		case AI_EFFECT_Rotate:
		case AI_EFFECT_Waver:
			xpw->ai.aiEffectNo[i] = AI_EFFECT_Ignore;
			break;

		case AI_EFFECT_NotMove:
		case AI_EFFECT_BeFast:
		case AI_EFFECT_BeSlow:
		case AI_EFFECT_BeNoWait:
		case AI_EFFECT_Rapid:
		case AI_EFFECT_BeAttack:
		case AI_EFFECT_BeErase:
			if(xpw->ai.aiEffectParam[i] == 0) break;
			if(xpw->ai.aiEffectParam[i] <= xpw->ai.aiEffectCount[i]) {
				xpw->ai.aiEffectNo[i] = AI_EFFECT_Ignore;
			}
			break;
		}

		if(xpw->ai.aiEffectNo[i] != AI_EFFECT_Ignore) {
			continue;
		}

		// 特殊処理発生条件が満たされているか調べる
		switch(char_data->condition[i]) {
		// 無条件で効果が発生する。
		case AI_CONDITION_Unconditional:
			doEffectFlag = TRUE;
			break;

		// ランダムで効果が発生する。
		// Subパラメータが存在し、発生率を設定する。
		case AI_CONDITION_Random:
			doEffectFlag = (genrand(100) <= char_data->condition_param[i]);
			break;

		// 対戦相手から攻撃を受けると効果が発生する。
		case AI_CONDITION_Damage:
			if(xpw->ai.aiState & AIF_DAMAGE) {
				doEffectFlag = TRUE;
				xpw->ai.aiState &= ~AIF_DAMAGE;
			}
			break;

		// 対戦相手よりも自分のウィルスが多いときに効果が発生する。
		// Subパラメータが存在し、発生するウィルス差を設定する。
		case AI_CONDITION_ManyVir:
			for(j = 0; j < evs_playcnt; j++) {
				if(j == aipn) continue;
				if(game_state_data[aipn].vs - game_state_data[j].virus_number >= char_data->condition_param[i]) {
					doEffectFlag = TRUE;
					break;
				}
			}
			break;

		// 対戦相手よりも自分のウィルスが少ないときに効果が発生する。
		// Subパラメータが存在し、発生するウィルス差を設定する。
		case AI_CONDITION_FewVir:
			for(j = 0; j < evs_playcnt; j++) {
				if(j == aipn) continue;
				if(game_state_data[j].virus_number - game_state_data[aipn].vs >= char_data->condition_param[i]) {
					doEffectFlag = TRUE;
					break;
				}
			}
			break;

		// 対戦相手のカプセルが積み上がると効果が発生する。
		case AI_CONDITION_Pile:
			for(j = 0; j < evs_playcnt; j++) {
				if(j == aipn) continue;
				if(game_state_data[j].ai.aiRootP < 5) {
					doEffectFlag = TRUE;
					break;
				}
			}
			break;

		// ＣＯＭレベルがＥＡＳＹのときに効果が発生する。
		case AI_CONDITION_Easy:
			doEffectFlag = (xpw->ai.aivl == 0);
			break;

		// ＣＯＭレベルがＮＯＲＭＡＬのとき効果が発生する。
		case AI_CONDITION_Normal:
			doEffectFlag = (xpw->ai.aivl == 1);
			break;

		// ＣＯＭレベルがＨＡＲＤのときに効果が発生する。
		case AI_CONDITION_Hard:
			doEffectFlag = (xpw->ai.aivl == 2);
			break;

		// 自分の残りウィルスが少なくなると効果が発生する。
		// Subパラメータが存在し、ウィルスが何匹以下になると発生するか設定する。
		case AI_CONDITION_Last:
			doEffectFlag = (xpw->vs <= char_data->condition_param[i]);
			break;

		// 連鎖をさせると効果が発生する。
		// Subパラメータが存在し、発生する連鎖数を設定する。
		case AI_CONDITION_Rensa:
			doEffectFlag = (xpw->ai.aiFlagDecide.rensa != 0);
			break;

		// ゲーム開始時のウィルスが指定した数以上の時に発生する
		// Subパラメータが存在し、ウィルス数を設定する。
		case AI_CONDITION_PreemPtive:
			doEffectFlag = (dm_get_first_virus_count(evs_gamemode, xpw) >= char_data->condition_param[i]);
			break;
		}

		if(!doEffectFlag) continue;

		xpw->ai.aiEffectNo[i] = char_data->effect[i];
		xpw->ai.aiEffectParam[i] = char_data->effect_param[i];
		xpw->ai.aiEffectCount[i] = 0;
	}

	// 特殊処理を発生させる
	for(i = 0; i < NUM_AI_EFFECT; i++) {

		switch(xpw->ai.aiEffectNo[i]) {
		// カプセルを無意味に回転させる。
		case AI_EFFECT_Rotate:
			xpw->ai.aiRollHabit |= 0x02;
			break;

		// カプセルを無意味に横移動させて、迷っているように見せる。
		case AI_EFFECT_Waver:
			break;

		// カプセルを操作できなくなる。
		// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
		case AI_EFFECT_NotMove:
			xpw->ai.aiState |= AIF_AGAPE;
			break;

		// カプセルの操作速度が速くなる。
		// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
		case AI_EFFECT_BeFast:
			aiSelSpeed = MAX(aiSelSpeed, AI_SPEED_MAX);
			break;

		// カプセルの操作速度が遅くなる。
		// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
		case AI_EFFECT_BeSlow:
			aiSelSpeed = MIN(aiSelSpeed, AI_SPEED_VERYSLOW);
			break;

		// カプセルの操作速度がノーウエイトになる。
		// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
		case AI_EFFECT_BeNoWait:
			aiSelSpeed = AI_SPEED_FLASH;
			break;

		// カプセルの操作速度がどんどん速くなる。
		// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
		case AI_EFFECT_Rapid:
			aiSelSpeed = xpw->ai.aiEffectCount[i] / 5 + 1;
			aiSelSpeed = MIN(aiSelSpeed, AI_SPEED_FLASH);
			break;

		// 消去ロジックが“攻撃”方向へ１段階進む。
		// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
		// 消去ロジックは　「早消し」　FastErase-SmallRensa-Rensa&Erase-Rensa&Attack　「攻撃」　と、並ぶ。
		case AI_EFFECT_BeAttack:
			aiSelCom = MIN(aiSelCom + 1, AI_LOGIC_RensaAndAttack);
			break;

		// 消去ロジックが“早消し”方向へ１段階進む。
		// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
		// 消去ロジックは　「早消し」　FastErase-SmallRensa-Rensa&Erase-Rensa&Attack　「攻撃」　と、並ぶ。
		case AI_EFFECT_BeErase:
			aiSelCom = MAX(aiSelCom - 1, AI_LOGIC_FastErase);
			break;

		// 発生している特殊処理を全て無効にする。
		// Subパラメータが存在し、無効にする特殊処理を指定する。（0は全て。）
		case AI_EFFECT_Lose:
			if(xpw->ai.aiEffectParam[i]) {
				xpw->ai.aiEffectNo[ (xpw->ai.aiEffectParam[i] - 1) % NUM_AI_EFFECT ] = AI_EFFECT_Ignore;
			}
			else {
				for(j = 0; j < NUM_AI_EFFECT; j++) {
					xpw->ai.aiEffectNo[j] = AI_EFFECT_Ignore;
				}
			}
			break;
		}
	}

	// ノーウェイトの場合は、AI_EFFECT_Rotate と AI_EFFECT_Waver を無効にする
	if(aiSelSpeed == AI_SPEED_FLASH) {
		// AI_EFFECT_Rotate を無効にする
		xpw->ai.aiRollHabit &= ~0x02;

		// AI_EFFECT_Waver を無効にする
		for(i = 0; i < NUM_AI_EFFECT; i++) {
			if(xpw->ai.aiEffectNo[i] == AI_EFFECT_Waver) {
				xpw->ai.aiEffectNo[i] = AI_EFFECT_Ignore;
			}
		}
	}

#if 0
/*
	switch(char_no) {
	case CHR_NOKO:
		aiPriOfs = 600;						// 頭を悪くする
		aiSelCom = AI_LOGIC_FastErase;
		aiSelSpeed = AI_SPEED_VERYSLOW;
		// 攻撃されると次の操作が少し止まる
		xpw->ai.aiState &= ~AIF_AGAPE;
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiState |= AIF_AGAPE;
			xpw->ai.aiTimer = 2 + genrand(4);
		}
		break;
	case CHR_BOMB:
		aiPriOfs = 600;						// 頭を悪くする
		aiSelCom = AI_LOGIC_FastErase;
		// 攻撃されると一定期間速くなる
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 3 + genrand(5);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelSpeed = AI_SPEED_VERYSLOW;
			break;
		case 1:
			aiSelSpeed = AI_SPEED_MAX;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_PUKU:
		// たまに連鎖を狙ってくる
		if ( xpw->ai.aiNum == 0 && xpw->vs < vrsMax*75/100 && xpw->vs > vrsMax*25/100 && genrand(10) == 0 ) {
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 3 + genrand(4);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelCom = AI_LOGIC_FastErase;
			aiSelSpeed = AI_SPEED_SLOW;
			break;
		case 1:
			aiSelCom = AI_LOGIC_RensaAndErase;
			aiSelSpeed = AI_SPEED_SLOW;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_CHOR:
		// 連鎖攻撃後一定期間速くなる
		if ( xpw->ai.aiFlagDecide.rensa ) {
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 3 + genrand(5);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelCom = AI_LOGIC_SmallRensa;
			aiSelSpeed = AI_SPEED_SLOW;
			break;
		case 1:
			aiSelCom = AI_LOGIC_SmallRensa;
			aiSelSpeed = AI_SPEED_VERYFAST;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_PROP:
		// 操作ｶﾌﾟｾﾙを無意味に回す
		xpw->ai.aiRollHabit |= 0x02;
		aiSelCom = AI_LOGIC_FastErase;
		aiSelSpeed = AI_SPEED_FAST;
		break;
	case CHR_HANA:
		// ２連続で降らされると怒って攻撃レベルが上がる
		xpw->ai.aiState &= ~AIF_DAMAGE;
		if ( xpw->ai.aiState & AIF_DAMAGE2 ) {
			xpw->ai.aiState &= ~AIF_DAMAGE2;
			xpw->ai.aiNum++;
			if ( xpw->ai.aiNum > 3 ) xpw->ai.aiNum = 3;
		}
		aiSelCom = xpw->ai.aiNum;
		aiSelSpeed = AI_SPEED_FAST;
		break;
	case CHR_TERE:
		// 攻撃されると遅くなる､されていないと少しずつ速くなる
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiNum = 0;
		}
		aiSelCom = AI_LOGIC_RensaAndErase;
		aiSelSpeed = xpw->ai.aiNum / 5 + AI_SPEED_VERYSLOW;
		if ( aiSelSpeed > AI_SPEED_MAX ) aiSelSpeed = AI_SPEED_MAX;
		xpw->ai.aiNum++;
		break;
	case CHR_PACK:
		// 攻撃されると連鎖を狙ってくる
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 5 + genrand(5);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelCom = AI_LOGIC_FastErase;
			aiSelSpeed = AI_SPEED_FAST;
			break;
		case 1:
			aiSelCom = AI_LOGIC_RensaAndAttack;
			aiSelSpeed = AI_SPEED_FAST;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_KAME:
		// 終盤連鎖を狙わなくなる
		if ( xpw->vs <= 8 ) {
			aiSelCom = AI_LOGIC_FastErase;
			aiSelSpeed = AI_SPEED_FAST;
		} else {
			aiSelCom = AI_LOGIC_RensaAndAttack;
			aiSelSpeed = AI_SPEED_FAST;
		}
		// 連鎖攻撃時最速で落す
//		aiSelSpeedRensa = AI_SPEED_FLASH;
		break;
	case CHR_KUPP:
		// 後半は集中力が高まり自由落下時間が無くなる(HARDのみ)
		if ( xpw->vs <= vrsMax/2 && xpw->ai.aivl == 2 ) {
			aiSelCom = AI_LOGIC_RensaAndErase;
			aiSelSpeed = AI_SPEED_VERYFASTNW;
		} else {
			aiSelCom = AI_LOGIC_RensaAndErase;
			aiSelSpeed = AI_SPEED_VERYFAST;
		}
		// 相手が弱すぎ（ｳｨﾙｽ数）で遅くなる(VSCPUのみ)
		if ( evs_gamesel == GSL_VSCPU && game_state_data[1 - aipn].vs - 15 >= xpw->vs ) {
			aiSelSpeed = AI_SPEED_VERYSLOW;
		}
		break;
	case CHR_PEACH:
		aiSelSpeed = AI_SPEED_VERYFAST;
		// 中盤以降強連鎖消しに変わる(HARDのみ)
		if ( xpw->vs <= vrsMax/2 && xpw->ai.aivl == 2 ) {
			aiSelCom = AI_LOGIC_RensaAndErase;
		} else {
			aiSelCom = AI_LOGIC_RensaAndAttack;
		}
		// 相手が高く積み上げると連鎖をしなくなる
		if ( evs_gamesel == GSL_VSCPU && game_state_data[1 - aipn].ai.aiRootP < 5 ) {
			aiSelCom = AI_LOGIC_FastErase;
		}
		break;
	case CHR_MARIO:
		aiSelCom = AI_LOGIC_RensaAndErase;
		aiSelSpeed = AI_SPEED_FAST;
		break;
	case CHR_PLAIN1:										// デバッグ用
		aiSelCom = AI_LOGIC_FastErase;
		aiSelSpeed = AI_SPEED_VERYFASTNW;
		break;
	case CHR_PLAIN2:
		aiSelCom = AI_LOGIC_SmallRensa;
		aiSelSpeed = AI_SPEED_VERYFASTNW;
		break;
	case CHR_PLAIN3:
		aiSelCom = AI_LOGIC_RensaAndErase;
		aiSelSpeed = AI_SPEED_VERYFASTNW;
		break;
	case CHR_PLAIN4:
		aiSelCom = AI_LOGIC_RensaAndAttack;
		aiSelSpeed = AI_SPEED_VERYFASTNW;
		break;
	}

	// 状態設定
	switch(aiSelCom) {
	// 早消し
	case AI_LOGIC_FastErase:
		// 死んだ後の練習モード
		if ( xpw->gs != AGST_GAME ) {
			aidt = AI_STATE_DEAD;
		} else {
			if ( aiRootP < 4.0 ) {
				aidt = AI_STATE_NARROW;
			}
			// 中央のｳｨﾙｽ数が８以下になった時終盤処理
			else if ( xpw->vs <= 6 || vrsCnt <= 2 ) {
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					if ( vrsCnt ) {
						aidt = AI_STATE_LASTVS;
					} else {
						aidt = AI_STATE_LASTVSNH;
					}
				} else {
					aidt = AI_STATE_LASTVSN;
				}
			}
			// ｶﾌﾟｾﾙ入り口下にｶﾌﾟｾﾙが残っている時
			else if ( badLine == 2 ) {
				aidt = AI_STATE_BADLINE2;
			}
			// 中盤処理
			else {
				aidt = AI_STATE_NORMAL;
			}
		}
		break;

	// 弱連鎖 // 強連鎖消し // 強連鎖狙い
	case AI_LOGIC_SmallRensa:
	case AI_LOGIC_RensaAndErase:
	case AI_LOGIC_RensaAndAttack:
	case AI_LOGIC_WidErase:
	case AI_LOGIC_MajorityRensa:
		// 死んだ後の練習モード
		if ( xpw->gs != AGST_GAME ) {
			aidt = AI_STATE_DEAD;
		} else {
			if ( aiRootP < 4.0 ) {
				aidt = AI_STATE_NARROW;
			}
			// 中央のｳｨﾙｽ数が８以下になった時終盤処理
			else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					if ( vrsCnt ) {
						aidt = AI_STATE_LASTVS;
					} else {
						aidt = AI_STATE_LASTVSNH;
					}
				} else {
					aidt = AI_STATE_LASTVSN;
				}
			}
			// ｶﾌﾟｾﾙ入り口下にｶﾌﾟｾﾙが残っている時
			else if ( badLine == 2 ) {
				aidt = AI_STATE_BADLINE2;
			}
			// 中盤処理
			else {
				if ( badLine == 1 ) {
					aidt = AI_STATE_BADLINE1;
				} else {
					aidt = AI_STATE_NORMAL;
				}
			}
		}
		break;
	}
*/
#endif

//if (aipn == 0) osSyncPrintf("selcom mode %d\n",aidt);

	// パラメータワーク設定
	{
		AI_DATA *ptr = &(*aiDataPtr)[aiSelCom][aidt];

		pri_point[LnOnLinVrs]	= ptr->dt_LnOnLinVrs;
		pri_point[LnEraseVrs]	= ptr->dt_LnEraseVrs;
		EraseLinP[1]			= ptr->dt_EraseLinP1;
		EraseLinP[2]			= (ptr->dt_EraseLinP1 + ptr->dt_EraseLinP3) >> 1;
		for(i = 3; i <= 8; i++) {
			EraseLinP[i] = ptr->dt_EraseLinP3;
		}
		HeiEraseLinRate			= ptr->dt_HeiEraseLinRate * 0.01f;
		WidEraseLinRate			= ptr->dt_WidEraseLinRate * 0.01f;

		LPriP					= ptr->dt_P_ALN_HeightP;
		AloneCapP[ALN_FallCap]	= ptr->dt_P_ALN_FallCap;
		AloneCapP[ALN_FallVrs]	= ptr->dt_P_ALN_FallVrs;
		AloneCapP[ALN_Capsule]	= ptr->dt_P_ALN_Capsule;
		AloneCapP[ALN_Virus]	= ptr->dt_P_ALN_Virus;

		AloneCapWP[ALN_Bottom]	= ptr->dt_WP_ALN_Bottom;
		AloneCapWP[ALN_FallCap]	= ptr->dt_WP_ALN_FallCap;
		AloneCapWP[ALN_FallVrs]	= ptr->dt_WP_ALN_FallVrs;
		AloneCapWP[ALN_Capsule]	= ptr->dt_WP_ALN_Capsule;
		AloneCapWP[ALN_Virus]	= ptr->dt_WP_ALN_Virus;

		OnVirusP				= (int)ptr->dt_OnVirusP;
		RensaP					= ptr->dt_RensaP;
		RensaMP					= ptr->dt_RensaMP;
		pri_point[LnHighCaps]	= ptr->dt_LnHighCaps;
		pri_point[LnHighVrs]	= ptr->dt_LnHighVrs;
		aiWall = (ptr->dt_aiWall_F) ? wall : 0;
		OnVirusP_org			= OnVirusP;

		HeiLinesAllp[2]			= ptr->dt_HeiLinesAllp2;
		HeiLinesAllp[3]			= ptr->dt_HeiLinesAllp3;
		HeiLinesAllp[4]			= ptr->dt_HeiLinesAllp4;
		HeiLinesAllp[5]			= ptr->dt_HeiLinesAllp4;
		HeiLinesAllp[6]			= ptr->dt_HeiLinesAllp4;
		HeiLinesAllp[7]			= ptr->dt_HeiLinesAllp4;
		HeiLinesAllp[8]			= ptr->dt_HeiLinesAllp4;

		WidLinesAllp[2]			= ptr->dt_WidLinesAllp2;
		WidLinesAllp[3]			= ptr->dt_WidLinesAllp3;
		WidLinesAllp[4]			= ptr->dt_WidLinesAllp4;
		WidLinesAllp[5]			= ptr->dt_WidLinesAllp4;
		WidLinesAllp[6]			= ptr->dt_WidLinesAllp4;
		WidLinesAllp[7]			= ptr->dt_WidLinesAllp4;
		WidLinesAllp[8]			= ptr->dt_WidLinesAllp4;
	}

	// お邪魔カプセルのストックがある対戦相手がいる場合、攻撃を行わないようにする。
	if(char_data->wait_attack) {
		for(i = 0; i < evs_playcnt; i++) {
			if(i == aipn) {
				continue;
			}

			if(game_state_data[i].cap_attack_work[0][0] == 0) {
				continue;
			}

			for(j = 2; j < 8; j++) {
				if(EraseLinP[j] > 0) {
					EraseLinP[j] = -EraseLinP[j];
				}
			}

			i = evs_playcnt;
		}
	}
}


//*** ﾌｨｰﾙﾄﾞﾃﾞｰﾀの再設定（コピー） ************************
//	aifFieldCopy
//in:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//out:	AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//
void aifFieldCopy(game_state* uupw)
{
	int	idx,idy;
	game_state*	upw;

	upw = uupw;

	/*----- next capsule data copy -----*/
	aiNext.ca = upw->cap.ca;
	aiNext.cb = upw->cap.cb;

	/*----- field data copy -----*/
	for ( idx=0 ; idx<FIELDX ; idx++ ) {
	for ( idy=0 ; idy<FIELDY ; idy++ ) {
		// for search
		aiRecurData[idy][idx+1].co = aiFieldData[idy][idx].co = upw->blk[idy][idx].co;	// 色情報ｺﾋﾟｰ
		aiRecurData[idy][idx+1].st = aiFieldData[idy][idx].st = upw->blk[idy][idx].st;	// ｽﾃｰﾀｽ情報ｺﾋﾟｰ
	}}

//	aifRecurCopy();

	/*----- 再帰検索用ﾃﾞｰﾀ 壁で囲む -----*/
	for ( idx=0 ; idx<FIELDX+2 ; idx++ ) {	// 横壁(下のみ）
		aiRecurData[FIELDY][idx].co = WALL;
		aiRecurData[FIELDY][idx].st = WALL;
	}
	for ( idy=0 ; idy<FIELDY+1 ; idy++ ) {	// 縦壁(左右)
		aiRecurData[idy][0].co = WALL;
		aiRecurData[idy][0].st = WALL;
		aiRecurData[idy][FIELDX+1].co = WALL;
		aiRecurData[idy][FIELDX+1].st = WALL;
	}
}

//*** ﾙｰﾄ検索専用ﾌｨｰﾙﾄﾞﾃﾞｰﾀの再設定（コピー） ************************
//	aifRecurCopy
//in:	AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//out:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//
void aifRecurCopy(void)
{
	int	idx,idy;

	/*----- field data copy -----*/
	for ( idx=0 ; idx<FIELDX ; idx++ ) {
	for ( idy=0 ; idy<FIELDY ; idy++ ) {
		aiRecurData[idy][idx+1].co = aiFieldData[idy][idx].co;	// (再帰検索用)色情報ｺﾋﾟｰ
		aiRecurData[idy][idx+1].st = aiFieldData[idy][idx].st;	// (再帰検索用)ｽﾃｰﾀｽ情報ｺﾋﾟｰ
	}}
}



//*** ｶﾌﾟｾﾙを置ける場所の検索 ************************
//	aifPlaceSearch
//in:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			aiFlagCnt	ｶﾌﾟｾﾙを置ける位置の数
//
void aifPlaceSearch(void)
{
	int		idx,idy;
	int		cnt;
	AI_EDGE	aiEdge[100];

	cnt = 0;						// counter reset
	aiTEdgeCnt = aiYEdgeCnt = 0;	// edge counter reset

	/*----- 縦置きｻｰﾁ -----*/
	for ( idy=1 ; idy<FIELDY ; idy++ ) {
		for ( idx=1 ; idx<FIELDX+1 ; idx++ ) {
			if ( aiRecurData[idy][idx].co == COL_NOTHING ) {			// なにもなし？
				if ( aiRecurData[idy+1][idx].co != COL_NOTHING ) {		// 下なにかある？
					if ( aiRecurData[idy-1][idx].co == COL_NOTHING ) {	// 上なにもなし？
						aiEdge[cnt].tory = tate;
						aiEdge[cnt].x	 = idx;
						aiEdge[cnt].y	 = idy;
						cnt++;
						aiTEdgeCnt++;		// 縦置き可能ｴｯｼﾞ数
					}
				}
			}
		}
	}

	/*----- 横置きｻｰﾁ -----*/
	for ( idy=1 ; idy<FIELDY ; idy++ ) {
		for ( idx=1 ; idx<FIELDX ; idx++ ) {
			if ( aiRecurData[idy][idx].co == COL_NOTHING ) {			// なにもなし？
				if ( aiRecurData[idy+1][idx].co != COL_NOTHING ||		// 下or右下なにかある？
					 aiRecurData[idy+1][idx+1].co != COL_NOTHING ) {
					if ( aiRecurData[idy][idx+1].co == COL_NOTHING ) {		// 右なにもなし？
						aiEdge[cnt].tory = yoko;
						aiEdge[cnt].x	 = idx;
						aiEdge[cnt].y	 = idy;
						cnt++;
						aiYEdgeCnt++;		// 横置き可能ｴｯｼﾞ数
					}
				}
			}
		}
	}

	/*----- ﾌﾗｸﾞﾃﾞｰﾀ ｾｯﾄ -----*/
	cnt = 0;
	for ( idx=0 ; idx<aiTEdgeCnt ; idx++ ) {			// 縦置き　反転無し
		aiFlag[cnt].tory = tate;
		aiFlag[cnt].x	 = aiEdge[idx].x;
		aiFlag[cnt].y	 = aiEdge[idx].y;
		aiFlag[cnt].rev	 = 0;
		cnt++;
	}
	if( aiNext.ca != aiNext.cb ) {
		for ( idx=0 ; idx<aiTEdgeCnt ; idx++ ) {			// 縦置き　反転有り
			aiFlag[cnt].tory = tate;
			aiFlag[cnt].x	 = aiEdge[idx].x;
			aiFlag[cnt].y	 = aiEdge[idx].y;
			aiFlag[cnt].rev	 = 1;
			cnt++;
		}
	}
	for ( idx=0 ; idx<aiYEdgeCnt ; idx++ ) {			// 横置き　反転無し
		aiFlag[cnt].tory = yoko;
		aiFlag[cnt].x	 = aiEdge[aiTEdgeCnt+idx].x;
		aiFlag[cnt].y	 = aiEdge[aiTEdgeCnt+idx].y;
		aiFlag[cnt].rev	 = 0;
		cnt++;
	}
	if( aiNext.ca != aiNext.cb ) {
		for ( idx=0 ; idx<aiYEdgeCnt ; idx++ ) {			// 横置き　反転有り
			aiFlag[cnt].tory = yoko;
			aiFlag[cnt].x	 = aiEdge[aiTEdgeCnt+idx].x;
			aiFlag[cnt].y	 = aiEdge[aiTEdgeCnt+idx].y;
			aiFlag[cnt].rev	 = 1;
			cnt++;
		}
	}
	aiFlagCnt = cnt;		// ﾌﾗｸﾞﾃﾞｰﾀ数

}



//*** ｶﾌﾟｾﾙを置ける所から移動開始位置まで移動可能検索 ************************
//	aifMoveCheck
//in:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//		AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			aiFlagCnt	ｶﾌﾟｾﾙを置ける位置の数
//out:	AI_FLAG		aiFlag[].ok	移動可能かどうかフラグ(1:成功､0:失敗)
//		float		aiRootP		全移動量の平均値（移動範囲が狭すぎるのをﾁｪｯｸするためのﾜｰｸ）
//
void aifMoveCheck(void)
{
	int	cnt,okctr,okrootctr;
	int	idx,idy;
	int	i;

	for( i = 0; i < aiFlagCnt; i++ ) aiFlag[i].pri = 0;

	for ( cnt=okctr=okrootctr=0 ; cnt<aiFlagCnt ; cnt++ ) {
		for(i = 0;i < FIELDX+2;i++) {
			aiRecurData[0][i].co = WALK;
		}
		aifRecurCopy();
		aiRootCnt = 0;
		aiRollFinal = 0;
		success = 0;
		if ( aiFlag[cnt].tory == tate )	aifTRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt );
		else							aifYRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt);
		aiFlag[cnt].ok = success;
		if ( (!success) && (aiSelCom > 1) ) {
			aiRollFinal = 1;
			if ( aiFlag[cnt].tory == tate )	{
				if ( aiFlag[cnt].x < 7 && aiRecurData[aiFlag[cnt].y][aiFlag[cnt].x+1].st == MB_NOTHING ) {
					aifYRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt );
				}
			} else {
				if ( aiRecurData[aiFlag[cnt].y-1][aiFlag[cnt].x].st == MB_NOTHING ) {
					aifTRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt );
				} else if ( aiRecurData[aiFlag[cnt].y-1][aiFlag[cnt].x+1].st == MB_NOTHING && (aiRecurData[aiFlag[cnt].y][aiFlag[cnt].x+2].st != MB_NOTHING || aiFlag[cnt].x == 6) ) {
					aifTRecur(aiFlag[cnt].x+1 , aiFlag[cnt].y, cnt );
				}
			}
			aiFlag[cnt].ok = success;
		}
		if ( success ) {
			okctr++;
			okrootctr += aiRootCnt;
		}
	}

	if ( okctr ) aiRootP = (float)okrootctr / (float)okctr;
	else aiRootP = 0.0;
}

//*** 縦置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ ************************
//	aifTRecur
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifTRecur(u8 x,u8 y,u8 cnt)
{
	aiRecurData[y][x].co = WALK;

	if ( x==aiGoalX && y==aiGoalY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-2][x].st==MB_NOTHING )
		aifTRecur(x,y-1,cnt);
	if ( success!=1 && aiRecurData[y][x+1].co==COL_NOTHING && aiRecurData[y-1][x+1].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifTRecur(x+1,y,cnt);
		else aifTRecurUP(x+1,y,cnt);
	}
	if ( success!=1 && aiRecurData[y][x-1].co==COL_NOTHING && aiRecurData[y-1][x-1].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifTRecur(x-1,y,cnt);
		else aifTRecurUP(x-1,y,cnt);
	}

	//---- ﾙｰﾄ記録 -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;
#if defined(DEBUG)
		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");
#endif
	}
}

//*** 縦置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ(上移動専用) ************************
//	aifTRecurUP
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifTRecurUP(u8 x,u8 y,u8 cnt)
{
//	aiRecurData[y][x].co = WALK;

	if ( x==aiGoalX && y==aiGoalY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-2][x].st==MB_NOTHING )
		aifTRecur(x,y-1,cnt);

	//---- ﾙｰﾄ記録 -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;
#if defined(DEBUG)
		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");
#endif
	}
}

//*** 横置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ ************************
//	aifYRecur
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifYRecur(u8 x,u8 y,u8 cnt)
{
	aiRecurData[y][x].co = WALK;

	if ( x==aiGoalX && y==aiGoalY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-1][x+1].st==MB_NOTHING )
		aifYRecur(x,y-1,cnt);
	if ( success!=1 && aiRecurData[y][x+1].co==COL_NOTHING && aiRecurData[y][x+2].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifYRecur(x+1,y,cnt);
		else aifYRecurUP(x+1,y,cnt);
	}
	if ( success!=1 && aiRecurData[y][x-1].co==COL_NOTHING && aiRecurData[y][x].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifYRecur(x-1,y,cnt);
		else aifYRecurUP(x-1,y,cnt);
	}


	//---- ﾙｰﾄ記録 -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;
#if defined(DEBUG)
		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");
#endif
	}
}

//*** 横置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ(上移動専用) ************************
//	aifYRecurUP
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifYRecurUP(u8 x,u8 y,u8 cnt)
{
//	aiRecurData[y][x].co = WALK;

	if ( x==aiGoalX && y==aiGoalY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-1][x+1].st==MB_NOTHING )
		aifYRecur(x,y-1,cnt);

	//---- ﾙｰﾄ記録 -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;
#if defined(DEBUG)
		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");
#endif
	}
}

//*** 落す位置決定後もう一度ﾙｰﾄ検索 ************************
//	aifReMoveCheck
//in:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//		AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			aiFlagCnt	ｶﾌﾟｾﾙを置ける位置の数
//		u8			decide		最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号
//out:	AI_FLAG		aiFlag[].ok	移動可能かどうかフラグ(1:成功､0:失敗)
//		float		aiRootP		全移動量の平均値（移動範囲が狭すぎるのをﾁｪｯｸするためのﾜｰｸ）
//
void aifReMoveCheck(void)
{
	int	idx;

	for ( idx=0 ; idx<ROOTCNT ; idx++ ) {
		aiRoot[idx].x=0;
		aiRoot[idx].y=0;
	}
	aiRootCnt = 0;
	aiRollFinal = 0;
	aiMoveSF = 1;			// 再検索時は移動量を最大にしたいので
	success = 0;
	aifRecurCopy();
	if ( aiFlag[decide].tory == tate )	aifTRecur(aiFlag[decide].x , aiFlag[decide].y, decide );
	else							aifYRecur(aiFlag[decide].x , aiFlag[decide].y, decide);
	if ( !success ) {
		aiRollFinal = 1;
		aifRecurCopy();
		if ( aiFlag[decide].tory == tate )	{
			if ( aiRecurData[aiFlag[decide].y][aiFlag[decide].x+1].st == MB_NOTHING ) {
				aifYRecur(aiFlag[decide].x , aiFlag[decide].y, decide );
			}
		} else {
			if ( aiRecurData[aiFlag[decide].y-1][aiFlag[decide].x].st == MB_NOTHING ) {
				aifTRecur(aiFlag[decide].x , aiFlag[decide].y, decide );
			} else if ( aiRecurData[aiFlag[decide].y-1][aiFlag[decide].x+1].st == MB_NOTHING && (aiRecurData[aiFlag[decide].y][aiFlag[decide].x+2].st != MB_NOTHING || aiFlag[decide].x == 6) ) {
				aifTRecur(aiFlag[decide].x+1 , aiFlag[decide].y, decide );
			}
		}
	}
	aiRoot[aiRootCnt].x = aiRoot[aiRootCnt-1].x;
	aiRoot[aiRootCnt].y = aiRoot[aiRootCnt-1].y+1;
}


/************************************************
	キー情報作成
************************************************/

//*** 移動位置決定後キー情報作成 ************************
//	aifKeyMake
//in:	game_state	*uupw		ﾌﾟﾚｲﾔｰ構造体ﾎﾟｲﾝﾀ
//		AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			decide		最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号
//		s8			aiSelSpeedRensa		ＣＯＭの操作スピード速度番号（連鎖出来る時だけｽﾋﾟｰﾄﾞを変えたいとき使用）
//out:	AIWORK		uupw->ai	各種ｷｰ操作情報
//		s8			aiSelSpeed	ＣＯＭの操作スピード速度番号
//
void aifKeyMake(game_state* uupw)
{
	int	cnt;
	int	roll[2][2] = {{ 1,3 },{ 0,2 }};
	u8	lev;

	//----- 思考ﾌﾗｸﾞの保存 -----
	uupw->ai.aiFlagDecide = aiFlag[decide];

//if (aipn == 0) osSyncPrintf("pri = %d\n",aiFlag[decide].pri);

	for ( cnt=0 ; cnt<ROOTCNT ; cnt++ ) {
		uupw->ai.aiRootDecide[cnt].x=aiRoot[cnt].x;
		uupw->ai.aiRootDecide[cnt].y=aiRoot[cnt].y;
	}
	uupw->ai.aiEX = aiRoot[aiRootCnt-1].x;
	uupw->ai.aiEY = aiRoot[aiRootCnt-1].y;

//if (aipn == 0) {
//	osSyncPrintf("%d,%d,%d\n",uupw->ai.aiEY,aiRootCnt,decide);
//	osSyncPrintf("ai0:%d %d,%d\n",aiFlag[decide].pri,aiNext.ca,aiNext.cb);
//}

	//----- 連鎖チェック -----
	uupw->ai.aiFlagDecide.rensa = aifRensaCheck(uupw,&(uupw->ai.aiFlagDecide));
//	if ( uupw->ai.aiFlagDecide.rensa = aifRensaCheck(uupw,&(uupw->ai.aiFlagDecide)) && aiSelSpeedRensa != -1 )	{
//		aiSelSpeed = aiSelSpeedRensa;
//	}

	//----- 回転情報作成 -----
	uupw->ai.aiRollCnt = (40 + roll[uupw->ai.aiFlagDecide.tory][uupw->ai.aiFlagDecide.rev] - uupw->ai.aiOldRollCnt + uupw->ai.aiRollCnt - aiRollFinal + uupw->ai.aiRollFinal ) % 4;
	uupw->ai.aiOldRollCnt = uupw->ai.aiRollCnt;
	uupw->ai.aiRollFinal = aiRollFinal;

	//----- 移動ｽﾋﾟｰﾄﾞﾌﾗｸﾞ設定 -----
	uupw->ai.aiSelSpeed = aiSelSpeed;
	uupw->ai.aiSpeedCnt = 2;
	uupw->ai.aiKRFlag = 1;
	uupw->ai.aiRootP = (aiRootP > 100)?100:(u8)aiRootP;

	lev = genrand(aiVirusLevel[aiSelSpeed][uupw->ai.aivl]);

//if ( aipn == 0 ) osSyncPrintf(" %d %d %d \n",aiSelCom,aiSelSpeed,uupw->ai.aivl);

	if ( lev>6) {
		uupw->ai.aiSpUpFlag = FALSE;
	} else {
		uupw->ai.aiSpUpFlag = TRUE;
		uupw->ai.aiSpUpStart = genrand(5);
	}
}

/***********************************************
	キー情報出力
***********************************************/

//*** ＣＯｷｰ操作情報出力 ************************
//	aifKeyOut
//in:	game_state	*uupw		ﾌﾟﾚｲﾔｰ構造体ﾎﾟｲﾝﾀ
//		AIWORK		uupw->ai	各種ｷｰ操作情報
//out:	u16			joygam		ｺﾝﾄﾛｰﾗｰ仕様のｷｰ情報出力
//
void aifKeyOut(game_state* uupw)
{
	int idx, idy, i, j, k;

	aifMakeWork(uupw);

	joygam[uupw->pn] = 0;
	if (!uupw->ai.aiok) return;
	if (uupw->cap.my == 0) return;

	if(uupw->cap.mx == uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].x-1 &&
		uupw->cap.my == uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].y)
	{
		uupw->ai.aiKeyCount++;
	}
	uupw->ai.aiSpeedCnt--;

	idx = (uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].x-1) - uupw->cap.mx;
	idy =  uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].y    - uupw->cap.my;

	// カプセルを無意味に横移動させて、迷っているように見せる。
	if(idy && uupw->cap.my < uupw->ai.aiEY - 3) {
		for(i = 0; i < NUM_AI_EFFECT; i++) {
			static const int wave_tbl[8] = { 0, 1, 1, 0, -1, -1, 0, 1};
			int wave_dir, dx;

			if(uupw->ai.aiEffectNo[i] != AI_EFFECT_Waver) {
				continue;
			}

			wave_dir = wave_tbl[(uupw->ai.aiEY + uupw->cap.my) & 7];
			dx = idx + wave_dir;

			// 無意味に横移動させて、間違った場所に置いてしまう事が無ければ移動する
			if(
				uupw->blk[uupw->cap.my + 1][uupw->cap.mx + dx - 1].st == MB_NOTHING &&
				uupw->blk[uupw->cap.my + 1][uupw->cap.mx + dx + 0].st == MB_NOTHING &&
				uupw->blk[uupw->cap.my + 1][uupw->cap.mx + dx + 1].st == MB_NOTHING)
			{
				idx = dx;
			}

			break;
		}
	}

	if(uupw->ai.aiSpeedCnt == 0 && (uupw->ai.aiState & AIF_AGAPE) == 0) {

		if(uupw->ai.aiRollCnt == 0 && (uupw->ai.aiRollHabit & 0x02) &&
			((genrand(5) == 0 && uupw->cap.my < uupw->ai.aiEY - 3) || (uupw->ai.aiRollHabit & 0x01)))
		{
			uupw->ai.aiRollCnt += 2;
			uupw->ai.aiRollHabit ^= 1;
		}

		// ｶﾌﾟｾﾙ回転
		if ( uupw->ai.aiRollCnt!=0 ) {
			if ( uupw->ai.aiRollCnt==3 ) {
				joygam[uupw->pn] = B_BUTTON;
				uupw->ai.aiRollCnt = 0;
			} else {
				joygam[uupw->pn] = A_BUTTON;
				uupw->ai.aiRollCnt--;
			}
			uupw->ai.aiSpeedCnt = ROLL_SPEED;
		}

		// 落ちすぎている
		if ( idy<0 ) {
			uupw->ai.aiKeyCount++;
//			uupw->ai.aiSpeedCnt = aiDownSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];

			uupw->ai.aiSpeedCnt = 1;
/*
			// ちょっと遅れる
			if ( aiResSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl] > 1 ) {
				uupw->ai.aiSpeedCnt += genrand( aiResSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl] );
			}
*/
//			uupw->ai.aiKRFlag = 0;
			if ( uupw->ai.aiEY < uupw->cap.my ) {
				uupw->ai.aiok = FALSE;
			}
		}

		// 右移動
		if ( idx > 0 ) {
			joygam[uupw->pn] |= R_JPAD;
			uupw->ai.aiSpeedCnt = (!uupw->ai.aiKRFlag)?
				aiSlideFSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl]:
				aiSlideSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
			uupw->ai.aiKRFlag++;								// ｷｰﾘﾋﾟｰﾄON
		}

		// 左移動
		if ( idx < 0 ) {
			joygam[uupw->pn] |= L_JPAD;
			uupw->ai.aiSpeedCnt = (!uupw->ai.aiKRFlag)?
				aiSlideFSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl]:
				aiSlideSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
			uupw->ai.aiKRFlag++;
		}

/*
		// 間違える
		if ( ( genrand(1000) + 1 ) < MissRate ) {
			if ( joygam[uupw->pn] == 0 ) {
				switch ( genrand(4) ) {
					case 0:
						joygam[uupw->pn] |= L_JPAD;
						break;
					case 1:
						joygam[uupw->pn] |= L_JPAD;
						break;
					case 2:
						joygam[uupw->pn] |= A_BUTTON;
						break;
					case 3:
						joygam[uupw->pn] |= B_BUTTON;
						break;
				}
			}
		}
*/

		// 落下させる
		if ( idy>0 ) {
			if (!uupw->ai.aiKRFlag) {
				if(	(uupw->ai.aiSelSpeed == AI_SPEED_FLASH) ||
					(uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].y !=
					 uupw->ai.aiRootDecide[uupw->ai.aiKeyCount + 1].y))
				{
					if(	(uupw->ai.aiSelSpeed == AI_SPEED_MAX) ||
						(uupw->ai.aiSelSpeed == AI_SPEED_FLASH) ||
						(uupw->cap.my >= 3) ||
						((uupw->ai.aiSelSpeed >= AI_SPEED_SLOW) &&
						 (uupw->cap.my == 2) &&
						 (uupw->cap.cn > FallSpeed[uupw->cap.sp]/2)))
					{
						if( evs_gamesel == GSL_VSCPU) {
							if ( !uupw->ai.aiFlagDecide.rensa || (game_state_data[1-aipn].cap_attack_work[0][0] == 0) ) {
								joygam[uupw->pn] |= D_JPAD;
							}
						} else {
							joygam[uupw->pn] |= D_JPAD;
						}
					}
					if ( uupw->ai.aiSpUpFlag == TRUE ) {
						if ( uupw->ai.aiKeyCount > uupw->ai.aiSpUpStart ) {
							uupw->ai.aiSpeedCnt += VIRUSSPEEDMAX;
						} else {
							uupw->ai.aiSpeedCnt += aiDownSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
						}
					} else {
						uupw->ai.aiSpeedCnt += aiDownSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
					}
				} else {
					uupw->ai.aiSpeedCnt += VIRUSSPEEDMAX;
				}
			} else {
				uupw->ai.aiSpeedCnt += aiSlideFSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
				uupw->ai.aiKRFlag = 0;
			}
		}
	}

	if(	uupw->cap.mx==uupw->ai.aiEX-1 &&
		uupw->cap.my==uupw->ai.aiEY &&
		uupw->ai.aiRollFinal &&
		joygam[uupw->pn] != A_BUTTON &&
		joygam[uupw->pn] != B_BUTTON)
	{
		joygam[uupw->pn] = A_BUTTON;
		uupw->ai.aiRollFinal = 0;
	}

	if ( uupw->ai.aiSelSpeed == AI_SPEED_FLASH ) uupw->ai.aiSpeedCnt = 1;
}



//-----------------------------------------------------------------------------
// フラッシュに関係する場所か？
//-----------------------------------------------------------------------------
static int search_flash_3(int cx, int cy, int col, int tory ,int ec)
{
	static int tbl[] = { -3, -2, -1, 0, 1, 2, 3 };
	int	i, j, k, cnt, x, y, idx;
	s8	tmp[8], tmp2[8], flg[2];

	for ( i = 0; i < pGameState->flash_virus_count; i++ ) {
		if ( pGameState->flash_virus_pos[i][2] != col ) continue;
		x = pGameState->flash_virus_pos[i][0];
		y = pGameState->flash_virus_pos[i][1];
		// 横サーチ
		if ( cy == y ) {
			idx = 0;
			for ( j = 0; j < 8; j++ ) tmp[j] = -1;
			for ( j = -3; j <= 3; j++ ) {
				if ( ( (x+j) >= 0 ) && ( (x+j) < FIELDX ) ) {
					if ( aiFieldData[y][x+j].co != col ) {
						if ( aiFieldData[y][x+j].co == COL_NOTHING ) {
							// 空欄
							tmp[idx] = 0;
						} else {
							// 違う色が在る
							tmp[idx] = -1;
						}
					} else {
						// 同じ色が在る
						tmp[idx] = 1;
					}
				} else {
					// ビンからはみ出し
					tmp[idx] = -1;
				}
				idx++;
			}
			// 同時色チェック
			if ( ec == 1 && tory == yoko ) {
				if ( tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 1 ) {
					if ( cx == ( x - 3 ) ) return(10000);
				}
				if ( tmp[1] == 0 && tmp[2] == 0 && tmp[4] == 1 ) {
					if ( cx == ( x - 2 ) ) return(10000);
				}
				if ( tmp[0] == 1 && tmp[1] == 0 && tmp[2] == 0 ) {
					if ( cx == ( x - 2 ) ) return(10000);
				}
				if ( tmp[2] == 1 && tmp[4] == 0 && tmp[5] == 0 ) {
					if ( cx == ( x + 1 ) ) return(10000);
				}
				if ( tmp[4] == 0 && tmp[5] == 0 && tmp[6] == 1 ) {
					if ( cx == ( x + 1 ) ) return(10000);
				}
				if ( tmp[4] == 1 && tmp[5] == 0 && tmp[6] == 0 ) {
					if ( cx == ( x + 2 ) ) return(10000);
				}
				if ( tmp[1] == 0 && tmp[2] == 0 ) {
					if ( cx == ( x - 2 ) ) return(5000);
				}
				if ( tmp[4] == 0 && tmp[5] == 0 ) {
					if ( cx == ( x + 1 ) ) return(5000);
				}
			}
			// 消えるか？
			for ( j = 0; j < 7; j++ ) tmp2[j] = tmp[j];
			for ( j = 0; j < 7; j++ ) {
				if ( tmp[j] == 0 ) {
					tmp[j] = 1;
					cnt = 0;
					for ( k = 0; k < 7; k++ ) {
						if ( tmp[k] != 1 ) cnt = 0; else cnt++;
						if ( cnt > 3 ) if ( cx == ( x + tbl[j] ) ) return(5000);
					}
					tmp[j] = 0;
				}
			}
			// 置けるか？
/*
			flg[0] = flg[1] = 0;
			if ( tory == tate ) {
				for ( j = 1; j <= 3; j++ ) {
					if ( tmp2[3+j] == 0 && flg[0] == 0 ) {
						if ( cx == ( x + tbl[3+j] ) ) return(1000);
					} else if ( tmp2[3+j] == -1 ) {
						flg[0] = 1;
					}
					if ( tmp2[3-j] == 0 && flg[1] == 0 ) {
						if ( cx == ( x + tbl[3-j] ) ) return(1000);
					} else if ( tmp2[3-j] == -1 ) {
						flg[1] = 1;
					}
				}
			}
*/
		}
		// 縦サーチ
		if ( cx == x ) {
			idx = 0;
			for ( j = 0; j < 8; j++ ) tmp[j] = -1;
			for ( j = -3; j <= 3; j++ ) {
				if ( ( (y+j) >= 0 ) && ( (y+j) < FIELDY ) ) {
					if ( aiFieldData[y+j][x].co != col ) {
						if ( aiFieldData[y+j][x].co == COL_NOTHING ) {
							// 空欄
							tmp[idx] = 0;
						} else {
							// 違う色が在る
							tmp[idx] = -1;
						}
					} else {
						// 同じ色が在る
						tmp[idx] = 1;
					}
				} else {
					// ビンからはみ出し
					tmp[idx] = -1;
				}
				idx++;
			}
			// 同時色チェック
			if ( ec == 1 && tory == tate ) {
				if ( tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 1 ) {
					if ( cy == ( y - 2 ) ) return(10000);
				}
				if ( tmp[1] == 0 && tmp[2] == 0 && tmp[4] == 1 ) {
					if ( cy == ( y - 1 ) ) return(10000);
				}
				if ( tmp[0] == 1 && tmp[1] == 0 && tmp[2] == 0 ) {
					if ( cy == ( y - 1 ) ) return(10000);
				}
				if ( tmp[2] == 1 && tmp[4] == 0 && tmp[5] == 0 ) {
					if ( cy == ( y + 2 ) ) return(10000);
				}
				if ( tmp[4] == 0 && tmp[5] == 0 && tmp[6] == 1 ) {
					if ( cy == ( y + 2 ) ) return(10000);
				}
				if ( tmp[4] == 1 && tmp[5] == 0 && tmp[6] == 0 ) {
					if ( cy == ( y + 3 ) ) return(10000);
				}
				if ( tmp[1] == 0 && tmp[2] == 0 ) {
					if ( cy == ( y - 1 ) ) return(5000);
				}
				if ( tmp[4] == 0 && tmp[5] == 0 ) {
					if ( cy == ( y + 2 ) ) return(5000);
				}
			}
			// 消えるか？
			for ( j = 0; j < 7; j++ ) tmp2[j] = tmp[j];
			for ( j = 0; j < 7; j++ ) {
				if ( tmp[j] == 0 ) {
					tmp[j] = 1;
					cnt = 0;
					for ( k = 0; k < 7; k++ ) {
						if ( tmp[k] != 1 ) cnt = 0; else cnt++;
						if ( cnt > 3 ) if ( cy == ( y + tbl[j] ) ) return(5000);
					}
					tmp[j] = 0;
				}
			}
			// 置けるか？
/*
			flg[0] = flg[1] = 0;
			if ( tory == yoko ) {
				for ( j = 1; j <= 3; j++ ) {
					if ( tmp2[3+j] == 0 && flg[0] == 0 ) {
						if ( cy == ( y + tbl[3+j] ) ) return(1000);
					} else if ( tmp2[3+j] == -1 ) {
						flg[0] = 1;
					}
					if ( tmp2[3-j] == 0 && flg[1] == 0 ) {
						if ( cy == ( y + tbl[3-j] ) ) return(1000);
					} else if ( tmp2[3-j] == -1 ) {
						flg[1] = 1;
					}
				}
			}
*/
		}
	}

	return(0);
}



//-----------------------------------------------------------------------------
// フラッシュに必要な置き場だけを取り出す
//-----------------------------------------------------------------------------
static	int		flash_special(void)
{
	int	i, x[2], y[2], col[2], ec, ret;

	// 対フラッシュ用に活用できるか？
	delpos_cnt = 0;
	for ( i = 0; i < aiFlagCnt; i++ ) {
		if ( aiFlag[i].ok == 1 ) {
			// 場所
			x[0] = aiFlag[i].x-1;
			y[0] = aiFlag[i].y;
			if ( aiFlag[i].tory == tate ) {
				// 縦置き
				x[1] = x[0];
				y[1] = y[0]-1;
				// 色
				if ( aiFlag[i].rev == FALSE ) {
					col[0] = aiNext.cb;
					col[1] = aiNext.ca;
				} else {
					col[0] = aiNext.ca;
					col[1] = aiNext.cb;
				}
			} else {
				// 横置き
				x[1] = x[0]+1;
				y[1] = y[0];
				// 色
				if ( aiFlag[i].rev == FALSE ) {
					col[0] = aiNext.ca;
					col[1] = aiNext.cb;
				} else {
					col[0] = aiNext.cb;
					col[1] = aiNext.ca;
				}
			}
			if ( col[0] == col[1] ) ec = 1; else ec = 0;
			// チェック
			ret = search_flash_3(x[0], y[0], col[0], aiFlag[i].tory, ec);
			if ( ret > 0 ) {
				delpos_tbl[delpos_cnt] = i;
				delpos_cnt++;
				aiFlag[i].pri += ret;
			}
			ret = search_flash_3(x[1], y[1], col[1], aiFlag[i].tory, ec);
			if ( ret > 0 ) {
				delpos_tbl[delpos_cnt] = i;
				delpos_cnt++;
				aiFlag[i].pri += ret;
			}
		}
	}

	// 余分な情報を消す
	if ( delpos_cnt > 0 ) {
		for ( i = 0; i < aiFlagCnt; i++ ) aiFlag[i].ok = 0;
		for ( i = 0; i < delpos_cnt; i++ ) aiFlag[delpos_tbl[i]].ok = 1;
	}

	return(delpos_cnt);
}


// 時間がたつとＣＯＭが失敗するようになる
void	aiCOM_MissTake(void)
{
	u32		tmp;

	PlayTime++;

	// ５分以降はミスをするようになる
	if ( PlayTime > ( 5 * 60 * FRAME_PAR_SEC ) ) {
		// 0.5[%] 1.0[%] 1.5[%] 2.0[%] 2.5[%] 3.0[%] 3.5[%]
		tmp = PlayTime - ( 5 * 60 * FRAME_PAR_SEC );
		MissRate = ( 5 * ( tmp / FRAME_PAR_SEC ) ) / 60;
		if ( MissRate > 50 ) MissRate == 50;
	} else {
		MissRate = 0;
	}
}
