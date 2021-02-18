#ifndef _EVSWORKS_H_
#define _EVSWORKS_H_
// Search "ﾋﾙﾀﾙﾋ" to jump routine's top( & keypoint ) : present by ﾋﾙﾀﾙﾋ

//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ 汎用的定義文, ｼｽﾃﾑ
#define NONE		0
#define TOPNUM		0
#define FIRST		1
#define NOT1ST		0
#define WBUFER		2
#define EOD			0xff
#define TAG			0xfe

#define VI_RETRACE	1								// 垂直ﾘﾄﾚｰｽ待ち
#define M_PI		3.14159265358979323846			// π
#define DegToRad(a)	((float)(a) * M_PI / 180.0)		// 度数からラジアン
#define RadToDeg(a)	((float)(a) / M_PI * 180.0)		// ラジアンから度数
#define SHIF(x)	(1<<x)


//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ ｺﾝﾄﾛｰﾗ関連
#define SPRITE_ANTI	D_CBUTTONS		// ｽﾌﾟﾗｲﾄｱﾝﾁｴｲﾘｱｽ
#define	R_ROLL		A_BUTTON		// ｶﾌﾟｾﾙ Rﾛｰﾙ
#define	L_ROLL		B_BUTTON		//	 ..	 Lﾛｰﾙ
#define SELECT		A_BUTTON		// 選択	 selection
#define CANSEL		B_BUTTON		// ｷｬﾝｾﾙ rejection
#define	LEVER_UP	U_JPAD			// 上
#define	LEVER_DOWN	D_JPAD			// 下
#define	LEVER_LEFT	L_JPAD			// 左
#define	LEVER_RIGHT	R_JPAD			// 右
#define	LEVER_MINUS	L_JPAD			// - (左)
#define	LEVER_PLUS	R_JPAD			// + (右)

//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ ｻｳﾝﾄﾞ

enum {
	SEQ_Fever,
	SEQ_Chill,
	SEQ_Dizzy,
	SEQ_Opening,
	SEQ_Menu,
	SEQ_VSEnd,
	SEQ_Win,
	SEQ_Lose,
	SEQ_Gingle,
	SEQ_Clear,
	SEQ_Clear2,
	SEQ_MAX,
};
enum {
	SE_gFreeFall,
	SE_gVrsErase,
	SE_gVrsErsAl,
	SE_gSpeedUp,
	SE_gCombo1P,
	SE_gPause,
	SE_gCapErase,
	SE_mDecide,
	SE_mLeftRight,
	SE_mUpDown,
	SE_gLeftRight,
	SE_gTouchDown,
	SE_gCapRoll,
	SE_mCANCEL,
	SE_mDispChg,
	SE_mDataErase,
	SE_mError,
	SE_gCount1,
	SE_gCountEnd,
	SE_gVirusStruggle,
	SE_gCombo2P,
	SE_gCombo3P,
	SE_gCombo4P,
	SE_gCoin,
	SE_gWarning,
	SE_gReach,
	SE_gREraseP11,
	SE_gREraseP12,
	SE_gREraseP13,
	SE_gDamageP1,
	SE_gREraseP21,
	SE_gREraseP22,
	SE_gREraseP23,
	SE_gDamageP2,
	SE_gDamageP22,
	SE_xNoRequest,
};

//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ ｹﾞｰﾑ定義数値

#define LEVEL_MAX	  20	// ｹﾞｰﾑﾚﾍﾞﾙ
#define PLAYER_MAX	   4	// ﾌﾟﾚｲﾔｰ数
#define MFx			  12	// mﾌｨｰﾙﾄﾞMAX x,y
#define MFy			  21	//
#define FALL_CNT	  14	// 落下ｶｳﾝﾀ
#define	PIXEL_MAX	   6	// ﾌﾞﾛｯｸﾋﾟｸｾﾙ
#define ERASE_CNT1	  18	// ｲﾚｰｽｶｳﾝﾀ
#define ERASE_CNT2	  (13+14)	//	  ..
#define	PAUSE_NOREQ	  77	// 非ﾎﾟｰｽﾞ要請
#define	CMB_THROUGH	   7	// "ｺﾝﾎﾞ"補間数
#define	SCORE_MAX	   6	// ｽｺｱｲﾝﾃﾞｸｽ
#define	MAGA_MAX	 256	// ﾏｶﾞｼﾞﾝmax
#define	MAG_NO		   3	// ﾏｶﾞｼﾞﾝ数
#define	ID_MAX		   9	// .id max
#define	IDMAXp	ID_MAX+1	// .id max +1
#define LEVLIMIT	  23	// 内部ﾚﾍﾞﾙ上限
#define	STD_MFieldX	   8	// Mﾌｨｰﾙﾄﾞ標準 Xｻｲｽﾞ
#define	STD_MFieldY	  17	//			   Yｻｲｽﾞ( 見えない列含む )
#define	VS4_MFieldX	   8	// Mﾌｨｰﾙﾄﾞ 4p Xｻｲｽﾞ
#define	VS4_MFieldY	  17	//			  Yｻｲｽﾞ( 見えない列含む )
#define	MFITEM_SMALL   8	// Mﾌｨｰﾙﾄﾞｱｲﾃﾑｻｲｽﾞ S
#define	MFITEM_LARGE  10	//		 ..		   L
#define ACCEL_DIV	  10	// ｽﾋﾟｰﾄﾞﾚﾍﾞﾙｱｯﾌﾟﾀｲﾐﾝｸﾞ( ﾃﾞﾌｫﾙﾄ )
#define DELETE_LINE	   4	//
#define PINPOINT_CHG 120	// ﾋﾟﾝﾎﾟｲﾝﾄ攻撃時ｶｳﾝﾄﾘﾐｯﾄ
#define JOYrTIMEvm 13500	// DrMｹﾞｰﾑ時ｺﾝﾄﾛｰﾗ読みとりﾀｲﾐﾝｸﾞ( vsMEN )
#define JOYrTIMEvc 10500	//				..				( vsCPU )
#define PARAM_NUMx	   8	// ﾊﾟﾗﾒｰﾀ数字ｻｲｽﾞ x
#define PARAM_NUMy	   8	//		..		  y
#define PARAM_ALPy	   8	// ﾊﾟﾗﾒｰﾀ英字ｻｲｽﾞ y
#define PARAML_NUMx	  12	// Lﾊﾟﾗﾒｰﾀ数字ｻｲｽﾞ x
#define PARAML_NUMy	  16	//		 ..		   y

#define KINGBODY_MAX 100		// ｷﾝｸﾞﾎﾞﾝﾋﾞｰｶﾌﾟｾﾙ格納ｻｲｽﾞ
#define KING_WID	  32		// ｷﾝｸﾞﾎﾞﾝﾋﾞｰｻｲｽﾞw
#define KING_HEI	  32		//		..		 h
#define KING_COL	 256		//	  ..	 ﾊﾟﾚｯﾄ数
#define KING_SIZ_xb G_IM_SIZ_8b	//	  ..	 ﾊﾟﾚｯﾄﾋﾞｯﾄ数

enum { Xp,Yp,Zp,};			// XYZﾃﾞｰﾀｲﾝﾃﾞｸｽ

//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ (ｾﾚｸﾄ)ﾒﾆｭｰ関連

// ﾎﾟｰｽﾞ, ｻﾌﾞﾒﾆｭｰ
enum {
	SUB_TA,
	SUB_TATATA,
	SUB_COFFEE,
	SUB_MAX,
};

// ﾎﾟｰｽﾞ解除
enum {
	PAUSE_CONT,	// ﾎﾟｰｽﾞﾒﾆｭｰ継続
	PAUSE_END,	//	   ..	終了
	PAUSE_EXIT,	// ｹﾞｰﾑ終了
};

// 消滅ｾﾚｸﾄ
enum {
	ERS_CAPSULE,
	ERS_VIRUS,
	ERS_MAX,
};

//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ 各種ｽﾃｰﾀｽ関連

// ﾌﾟﾛｸﾞﾗﾑｽﾃｰﾀｽ evs_prgstat
enum {
	PRG_OPEN,		//-ｵｰﾌﾟﾝﾌｪｰﾄﾞ
	PRG_READY,		// ﾚﾃﾞｨ
	PRG_GAME,		// ｹﾞｰﾑ
	PRG_CLEAR,		// ｹﾞｰﾑｸﾘｱｰ
	PRG_OVER,		// ｹﾞｰﾑｵｰﾊﾞｰ
	PRG_PAUSE,		//-ﾎﾟｰｽﾞ
	PRG_PAUSE_NF,	// ﾌｫﾝﾄﾅｼﾎﾟｰｽﾞ
	PRG_1P_WIN,		// 1P勝利
	PRG_2P_WIN,		// 2P勝利
	PRG_3P_WIN,		// 3P勝利
	PRG_4P_WIN,		//-4P勝利
	PRG_DRAW,		// ﾄﾞﾛｰ
	PRG_EXITVS,		// 対戦ﾓｰﾄﾞ終了
	PRG_JUDGEWAIT,	// 判定ｳｴｲﾄ
	PRG_CLOSE,		// ｸﾛｰｽﾞﾌｪｰﾄﾞ
};

// ｹﾞｰﾑｽﾃｰﾀｽ pw->gs
enum {
	GST_NO_USE,		// 非使用
	GST_1ST,		// ﾌｧ-ｽﾄﾌﾚｰﾑ
	GST_INIT,		// ｲﾆｼｬﾗｲｽﾞ中
	GST_MFIELD,		// mﾌｨｰﾙﾄﾞ表示
	GST_GAME,		// ｹﾞｰﾑ進行中
	GST_PAUSE,		// ﾎﾟｰｽﾞ
	GST_OVER,		// 敵がｸﾘｱしてｹﾞｰﾑｵｰﾊﾞｰ
	GST_CLEAR,		// 自分の力でｸﾘｱｰ
	GST_TRAIN,		// 4pGAMEOVER時の練習
	GST_OVPAUSE,	// ｹﾞｰﾑｵｰﾊﾞｰ者のﾎﾟｰｽﾞ
	GST_TRPAUSE,	// 練習者のﾎﾟｰｽﾞ
};

// mﾌｨｰﾙﾄﾞｽﾃｰﾀｽ pw->mt
enum {
	MF_INIT,	// mﾌｨｰﾙﾄﾞｲﾆｼｬﾗｲｽﾞ中
	MF_NORMAL,	// 通常処理中
	MF_ERASE,	// 消去決定及び消去中
	MF_FALL,	// 落下決定及び落下中
	MF_BOUNCE,	// ｶｰｿﾙｶﾌﾟｾﾙﾊﾞｳﾝﾄﾞ中
};

// ｹﾞｰﾑｽﾋﾟｰﾄﾞ pw->sp
enum {
	SPEED_LOW,
	SPEED_MID,
	SPEED_HIGH,
	SPEED_MAX,
};

// ﾌﾞﾛｯｸｽﾃｰﾀｽ pw->blk[][].st
enum {
	MB_CAPS_U,		//-ｶﾌﾟｾﾙ上向き
	MB_CAPS_D,		//	..	下 ..
	MB_CAPS_L,		//	..	左 ..
	MB_CAPS_R,		//	..	右 ..
	MB_CAPS_BALL,	//	..	玉
	MB_VIRUS_A,		//-ｳｲﾙｽA
	MB_VIRUS_B,		// ｳｲﾙｽB
	MB_VIRUS_C,		// ｳｲﾙｽC
	MB_ERASE_CAPS,	// ｶﾌﾟｾﾙ消滅中
	MB_ERASE_VIRUS,	//-ｳｲﾙｽ	  ..
	MB_NOTHING,		// 何もない
};

// ｺﾝﾄﾛｰﾙｶﾌﾟｾﾙｽﾃｰﾀｽ pw->cap(nex).st
enum {
	ONES_NONE,	//-表示無し
	ONES_FLYI,	// 投げﾊﾟﾀｰﾝ初期
	ONES_FLYN,	// 投げられ中
	ONES_INIT,	// 新規表示 = ONES_ooAB
	ONES_ooAB,	// ﾊﾟﾀｰﾝ A
	ONES_8_AB,	//-	 ..	 B
	ONES_ooBA,	//	 ..	 C
	ONES_8_BA,	//	 ..	 D
	ONES_STOP,	// その場で停止
	ONES_BLOK,	// 硬化
};

// ｶﾗｰ (属性) pw->blk[][].co, pw->cap(nex).co
enum {
	COL_RED,			// 順序固定
	COL_YELLOW,			//	  ..
	COL_BLUE,			//	  ..
	COL_NOTHING,
};
#define COL_MAX	 COL_NOTHING

// ﾏﾄﾘｸｽﾌﾞﾛｯｸ落下ﾌﾗｸﾞ pw->blk[][].ff
enum {
	FALL_OFF,
	FALL_ON,
};

// 当たり判定 use "SHIF(x)"
enum {
	MBLK_UL,	//(U L),( U ),(U R),(URR)
	MBLK_U,		//( L ),( C ),( R ),(R R)
	MBLK_UR,	//(D L),( D ),(D R),(DRR)
	MBLK_URR,
	MBLK_L,
	MBLK_C,
	MBLK_R,
	MBLK_RR,
	MBLK_DL,
	MBLK_D,
	MBLK_DR,
	MBLK_DRR,
};

// pw->pu[x]
enum {
	PU_PLYFLG,
	PU_CPULEV,
	PU_DEMOno,
};

// pw->pu[PU_PLYFLG]
enum {
	PUF_PlayerMAN,
	PUF_PlayerCPU,
	PUF_PlayerDEMO,
};
// pw->pu[PU_CPULEV]
enum {
	PMD_CPU0,
	PMD_CPU1,
	PMD_CPU2,
};

// ｹﾞｰﾑ設定 evs_gamesel
enum {
	GSL_1PLAY,
	GSL_2PLAY,
	GSL_4PLAY,
	GSL_VSCPU,
	GSL_1DEMO,
	GSL_2DEMO,
	GSL_4DEMO,
	GSL_MAX,
//	渡辺追加分
	GSL_OPTION,
};

// ｷｬﾗｸﾀｰ番号
enum {
	CHR_MARIO,
	CHR_NOKO,
	CHR_BOMB,
	CHR_PUKU,
	CHR_CHOR,
	CHR_PROP,
	CHR_HANA,
	CHR_TERE,
	CHR_PACK,
	CHR_KAME,
	CHR_KUPP,
	CHR_PEACH,
	CHR_PLAIN1,
	CHR_PLAIN2,
	CHR_PLAIN3,
	CHR_PLAIN4,
};

// ﾃｰﾌﾞﾙ内 0,1,2番の意味pw->vsp.ac(.fo)
enum {
	BOMB_FORMT,
	BOMB_COUNT,
	BOMB_TABLE,
};
#define BOMBING_MIN 2
#define BOMBING_MAX 4

// 判定時状況 pw->vsp.jd
enum {
	JDG_PLAY,
	JDG_CLEAR,
	JDG_OVER,
};

// ｺﾝﾎﾞ監視ﾌﾗｸﾞ
enum {
	CMBChkStart,
	CMBChkCont,
	CMBChkEnd,
};

// 着地時状況判定 pw->vsp.jf
#define JDW_ErsCaps		0x0001	// ｶﾌﾟｾﾙのみのﾗｲﾝ消去
#define JDW_ErsVirus	0x0002	// ｳｲﾙｽ含むﾗｲﾝ消去
#define JDW_Combo		0x0004	// ｺﾝﾎﾞ
								//---
#define	JDW_JudgeA		0x0100	// a.単色ｶﾌﾟｾﾙのみ
#define	JDW_JudgeB		0x0200	// b.複数ｶﾌﾟｾﾙのみ(ｶﾌﾟｾﾙのみの連鎖)
#define	JDW_JudgeC		0x0400	// c.単色ｳｲﾙｽ消し
#define	JDW_JudgeD		0x0800	// d.複数ｳｲﾙｽ含む(ｳｲﾙｽのみの連鎖)
#define	JDW_JudgeE		0x1000	// e.ｳｲﾙｽ､ｶﾌﾟｾﾙの組み合わせで連鎖
								//---
#define JDW_EndJudge	0x8000	// 判定終了

// ｷﾝｸﾞﾎﾞﾝﾋﾞｰ移動,攻撃ﾄﾘｶﾞｰ
#define JDW_KingJud	( JDW_JudgeA|JDW_JudgeB|JDW_JudgeC|JDW_JudgeD|JDW_JudgeE )

// ｷﾝｸﾞﾎﾞﾝﾋﾞｰ行動ｽﾃｰﾀｽ king_status
enum {
	KBom_Start,		// 初期設定
	KBom_Init,		//	  ..
	KBom_MoveF,		// 次に移動( 初期 )
	KBom_MoveU,		//	  ..   ( 上昇 )
	KBom_MoveT,		//	  ..   ( 上昇終了 )
	KBom_MoveD,		//	  ..   ( 落下 )
	KBom_MoveX,		//	  ..   ( 着地 )
	KBom_Damage,	// ﾀﾞﾒｰｼﾞを受ける
	KBom_Wait1st,	// 待ち初期
	KBom_Wait,		// 待ち
	KBom_Attack,	// 攻撃中
	KBom_Gameover,	// ｹﾞｰﾑｵｰﾊﾞｰ
	KBom_Max,
};


//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ 構造体
// ai work 構造体
#define	ROOTCNT		50										// ﾙｰﾄ検索ﾜｰｸ最大数

// ｶｰｿﾙｶﾌﾟｾﾙを置いたことにより作られる状態
enum {	// af->hei(wid)[][x]: ﾃｰﾌﾞﾙ要素
	LnEraseLin,	// 消去ﾗｲﾝ数
	LnEraseVrs,	// 消去ｳｲﾙｽ数
	LnRinsetsu,	// 隣接数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ )くっついた状態で並んだ数
	LnLinesAll,	// 並び数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ,歯欠の間1ならばｶｳﾝﾄ )くっついた状態で並んだ数(空間が1つだけ空いてもOK)
	LnOnLinVrs,	// 並びの中のｳｲﾙｽ数
	LnLinSpace,	// ﾗｲﾝｽﾍﾟｰｽ( 3以下なら無効? )色を4個並べられるかﾁｪｯｸ用
	LnEraseVrsSide,	// ｳｨﾙｽ周辺消去数(旧Dummy)
	LnHighCaps,	// 入り口付近にあるｶﾌﾟｾﾙの数
	LnHighVrs,	// 入り口付近にあるｳｨﾙｽの数
	LnNonCount,	// TRUE：ﾉｰｶｳﾝﾄ( ex.ﾀﾃが消去の時のﾖｺのﾎﾟｲﾝﾄ計算をしないﾌﾗｸﾞ)
	LnTableMax,	//------ MAX
};

// 移動ｶﾌﾟｾﾙ移動先での状態
typedef struct {
	u8	ok;			// 移動可能ﾌﾗｸﾞ
	u8	tory;		// 縦/横ﾌﾗｸﾞ
	u8	x;			// 目的地X座標
	u8	y;			// 目的地Y座標
	u8	rev;		// ｶﾌﾟｾﾙの上下or左右反転ﾌﾗｸﾞ
	u8	ccnt;		// 同色数
					//------- re-make by hiru
	s32	pri;		// ﾎﾟｲﾝﾄ計算算出値 = 優先度
	s32	dead;		// ﾎﾟｲﾝﾄ計算算出用ﾃﾝﾎﾟﾗﾘｰﾜｰｸ
	u8	hei[2][LnTableMax];	// ｶｰｿﾙｶﾌﾟｾﾙに作られる縦ﾗｲﾝの状態（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ,ｽﾚｰﾌﾞ）
	u8	wid[2][LnTableMax];	// ｶｰｿﾙｶﾌﾟｾﾙに作られる横ﾗｲﾝの状態（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ,ｽﾚｰﾌﾞ）
	u8	elin[2];	// 消去ﾗｲﾝ合計数( ﾒｲﾝ,ｻﾌﾞ )
	u8	only[2];	// 上下左右に同じ色がない時「単一ｶﾌﾟｾﾙの状況」の数値が入る（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ､ｽﾚｰﾌﾞ）
	u8	wonly[2];	// 左右に同じ色がない時「単一ｶﾌﾟｾﾙの状況」の数値が入る（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ､ｽﾚｰﾌﾞ）
	u8	sub;		// ﾏｽﾀｰｶﾌﾟｾﾙが消去しない時、ｽﾚｰﾌﾞも通常計算するためのﾌﾗｸﾞ
	u8	rensa;		// 連鎖するかﾌﾗｸﾞ（TRUE：連鎖する）
} AI_FLAG;

// 移動ﾙｰﾄ保存用ﾜｰｸ
typedef struct {
	u8	x;
	u8	y;
} AI_ROOT;

typedef struct {
	AI_FLAG	aiFlagDecide;			// 操作ｶﾌﾟｾﾙの状態
	AI_ROOT	aiRootDecide[ROOTCNT];	// 操作ｶﾌﾟｾﾙのﾙｰﾄ
	u8	aiKeyCount;					// 操作ｶﾌﾟｾﾙのﾙｰﾄのｶｳﾝﾀ
	u8	aiSpeedCnt;					// 移動や回転操作を行った後次の操作ができるまでのｳｪｲﾄ
	u8	aiKRFlag;					// CPUｷｰﾘﾋﾟｰﾄ操作ﾌﾗｸﾞ（０：ｷｰﾘﾋﾟｰﾄ中）
	u8	aiRollCnt;					// 操作ｶﾌﾟｾﾙ回転方向ｶｳﾝﾀ
	u8	aiRollFinal;				// 移動終了時に１回だけ回転操作するかどうかﾌﾗｸﾞ（TRUE：１回右に回す）
	u8	aiRollHabit;				// 操作ｶﾌﾟｾﾙを回転させる癖ﾌﾗｸﾞ（0x02bit on:適当に回しながら落す、0x01bit on:半回転中）
	u8	aiSpUpFlag;					// 操作ｶﾌﾟｾﾙの落下操作を行うかどうかのﾌﾗｸﾞ（TRUE:行う）
	u8	aiSpUpStart;				// 操作ｶﾌﾟｾﾙの落下操作を行うﾀｲﾐﾝｸﾞを移動した距離と比較して行うための数値
	u8	aiSpUpCnt;					// ｽﾋﾟｰﾄﾞｱｯﾌﾟするｶｳﾝﾄ（未使用ﾜｰｸ）
	u8	aivl;						// virus level
	u8	aiok;						// ﾙｰﾄ検索思考処理を行いﾙｰﾄが決まったかどうかﾌﾗｸﾞ（TRUE:検索待ち）
	u8	aiRandFlag;					// ﾎﾟｲﾝﾄ判定時乱数をﾌﾟﾗｽするかﾌﾗｸﾞ（FALSE:４Ｐ対戦で３人同じＣＰＵのとき同じ操作をしないよう優先順位ﾎﾟｲﾝﾄに乱数値を足す）
	u8	aiEX;						// 目的地Ｘ座標
	u8	aiEY;						// 目的地Ｙ座標
	u8	aiOldRollCnt;				// 旧操作ｶﾌﾟｾﾙ回転方向ｶｳﾝﾀ(目的地に行けなかったチェック用)
	u8	aiNum;						// キャラ別思考用汎用ﾜｰｸ１
	u8	aiTimer;					// キャラ別思考用汎用ﾜｰｸ２
	u8	aiState;					// COMの状態ﾌﾗｸﾞ
	u8	aiSelSpeed;					// COMの操作ｽﾋﾟｰﾄﾞﾓｰﾄﾞ
	u8	aiRootP;					// 移動ｶﾌﾟｾﾙの移動距離の平均値(積みあがったり左右をｶﾌﾟｾﾙの壁に囲まれた時などに移動範囲が狭いので数値が小さくなる)
	u16	aiPriOfs;					// 下手なキャラクターに優先度のﾎﾟｲﾝﾄにﾗﾝﾀﾞﾑで足す数値（400から800程度CPUと戦ってみて決める）
} AIWORK;

// CPUの状態ﾌﾗｸﾞ
#define	AIF_DAMAGE	0x01	// 攻撃を受けた
#define	AIF_DAMAGE2	0x02	// 連続攻撃を受けた
#define	AIF_AGAPE	0x04	// COMの操作をストップさせる

// 操作ｶﾌﾟｾﾙ情報を受け取るﾜｰｸ
typedef struct {
	u8			st;			// （未使用）状態						:ONES_????
	u8			wc;			// （未使用）非表示ｳｴｲﾄｶｳﾝﾄ( 1>=強制 )
	u8			mx,my;		// 移動経路検索前の操作ｶﾌﾟｾﾙの座標及び操作中の座標（mﾌｨｰﾙﾄﾞ上）x,y(8x17)
	u8			ca;			// 操作ｶﾌﾟｾﾙのｶﾗｰa(ﾏｽﾀｰ)番号			:COL_???
	u8			cb;			// 操作ｶﾌﾟｾﾙのｶﾗｰb(ｽﾚｰﾌﾞ)番号
	u8			sp;			// 現在の落下ｽﾋﾟｰﾄﾞ番号（FallSpeed[番号]）
	u8			cn;			// 現在の落下ｽﾋﾟｰﾄﾞのｶｳﾝﾀ(cn == FallSpeed[番号]の時操作ｶﾌﾟｾﾙが１段落ちる)
	u16			bc;			// （未使用）ﾌﾞﾛｯｸ化ｶｳﾝﾀ
	u8			fc;			// （未使用）ﾌﾗｲﾝｸﾞｶｳﾝﾀ
	u8			bd;			// （未使用）ﾌﾞﾛｯｸ化ｶｳﾝﾀ
} AICAPS;

// mﾌｨｰﾙﾄﾞ上のｶﾌﾟｾﾙ&ｳｨﾙｽの状態ﾜｰｸ
typedef struct {
	u8			st;			// ﾌﾞﾛｯｸの種類				:CAPS/VIRUS..
	u8			co;			// ｶﾗｰ番号					:COL_???
//	u8			ff;			// 落下中ｽﾃｰﾀｽ
//	u8			rf;			// 病原菌作成予約ﾌﾗｸﾞbyｳﾙﾋﾟｰ
//	u8			csf;		// コンボ起点フラグ	ogura
//	u8			dmy0;		// (dummy)			ogura
//	u16			wk[4];		// 操作ﾜｰｸ
//	float		fc;			// 浮動小数点ｶｳﾝﾀ
} AIBLK;


// ｶﾌﾟｾﾙﾜｰｸ
typedef struct {
	u8			st;			// 状態						:ONES_????
	u8			wc;			// 非表示ｳｴｲﾄｶｳﾝﾄ( 1>=強制 )
	u8			mx,my;		// mﾌｨｰﾙﾄﾞx,y(8x15)
	u8			ca;			// ｶﾗｰa番号					:COL_???
	u8			cb;			//	..b	   ..
	u8			sp;			// FallSpeed[ｲﾝﾃﾞｸｽ], ｶｳﾝﾀ上限
	u8			cn;			//	   ..	  ｶｳﾝﾀ( next時は CapsMagazine[ｲﾝﾃﾞｸｽ] )
	u16			bc;			// ﾌﾞﾛｯｸ化ｶｳﾝﾀ
	u8			fc;			// ﾌﾗｲﾝｸﾞｶｳﾝﾀ
	u8			bd;			// ﾌﾞﾛｯｸ化ｶｳﾝﾀ
} CONCAP;

// mﾌｨｰﾙﾄﾞ 1ﾌﾞﾛｯｸﾜｰｸ
typedef struct {
	u8			st;			// ﾌﾞﾛｯｸの種類				:CAPS/VIRUS..
	u8			co;			// ｶﾗｰ番号					:COL_???
	u8			ff;			// 落下中ｽﾃｰﾀｽ
	u8			rf;			// 病原菌作成予約ﾌﾗｸﾞbyｳﾙﾋﾟｰ
	u8			csf;		// コンボ起点フラグ	ogura
	u8			dmy0;		// (dummy)			ogura
	u16			wk[4];		// 操作ﾜｰｸ
	float		fc;			// 浮動小数点ｶｳﾝﾀ
} BLKCAP;

// 前回ﾃﾞｰﾀ取得ﾜｰｸ( 処理切替等でのﾌﾟﾚｲﾔｰ設定記憶ｴﾘｱ )
typedef struct {
	u8		mt[2];		// .mt
	u8		vp;			// .vir.vm
	u8		vm;			// .vir.vm
	u8		mp;			// .mp
	u8		cs;			// .cap.st
	u8		vs;			// .vs
	short	rx;			// .rx
} PREVWK;

// 対戦ﾜｰｸ
typedef struct {
	u16		hi,sc;		// ﾊｲｽｺｱ, ｽｺｱ
	u8		wi,lo;		// 勝利数, 勝利者ﾌﾗｸﾞ
	u8		mcb,mec;	// (max)ｺﾝﾎﾞｶｳﾝﾀ, 連続消し数
	u8		ncb,nec;	// (now)ｺﾝﾎﾞｶｳﾝﾀ, 連続消し数
	u8		si,ei;		// 消去開始時ｶﾌﾟｾﾙ&ｳｲﾙｽ数, 消去ｶﾌﾟｾﾙ&ｳｲﾙｽ数
	u8		sv,ev;		// 消去開始時ｳｲﾙｽ数, 消去ｳｲﾙｽ数
	u16		jf;			// ｶｰｿﾙｶﾌﾟｾﾙ着地時判定ﾌﾗｸﾞ
	u16		jc;			// ｹﾞｰﾑｵｰﾊﾞｰｶｳﾝﾀ( Mﾌｨｰﾙﾄﾞを暗くする )
	u8		jd;			// 判定時状況( JDG_??? )
	u8		fe;			// 落下終了又は落下と落下の合間( ｸﾘｱは落下ﾒｲﾝのﾄｯﾌﾟ )
	u8		ee;			// 消去終了( ｸﾘｱは消去ﾒｲﾝのﾄｯﾌﾟ )
	u8		an;			// 攻撃相手( ﾋﾟﾝﾎﾟｲﾝﾄ時 )
	int		pc;			// 攻撃用ｶｳﾝﾀ( ﾋﾟﾝﾎﾟｲﾝﾄ時 )
	u8		aapc;		// 攻撃相手数 ogura
	u8		aap[3];		// 攻撃相手攻撃フラグ(1:攻撃する,0:しない)(青,黄,赤) ogura
	u8		aapf;		// 攻撃相手表示指定フラグ(b0:1p,b1:2p,b2:3p,b3,4pを攻撃)
	u8		ac[18];		// 攻撃ｶﾌﾟｾﾙ集計
	u8		fo[10];		// 被爆ｶﾌﾟｾﾙｾｯﾄ
} VSPLAY;

// 1ﾌﾟﾚｲﾔ総合ﾜｰｸ
typedef struct {
	int		aa;			//( ﾃﾞｰﾀ保護ﾜｰｸ )
	u8		pn;			// ﾌﾟﾚｲﾔｰ no.
	u8		gs;			// ｹﾞｰﾑｽﾃｰﾀｽ
	u8		mt;			// ﾏﾄﾘｸｽｽﾃｰﾀｽ MB_???
	u8		pe;			// ﾋﾟｸｾﾙ数
	u16		wx,wy;		// ﾜｰﾙﾄﾞ左上座標(ｶﾌﾟｾﾙ用)
	u16		nx,ny;		// ﾈｸｽﾄ x,y座標(wx,wy+α)
	u8		sx,sy;		// mﾌｨｰﾙﾄﾞｻｲｽﾞx,y(8x15)
	u8		lv;			// 設定ﾚﾍﾞﾙ(0-29)
	u8		sp;			// 設定ｽﾋﾟｰﾄﾞ				:SPEED_???
	u8		mp;			// Mﾌｨｰﾙﾄﾞ作成ﾌﾟﾛｸﾞﾗﾑ		:MFP_???
	u8		vs;			// ｳｲﾙｽ残数(ｶｳﾝﾀではなく、毎ｲﾝﾄごとにﾏｯﾌﾟﾃﾞｰﾀ上から探した数字が入る)
	u8		id;			// 設定値検索ｲﾝﾃﾞｸｽ
	u8		dg;			// ﾃﾞﾊﾞｯｸﾞｸﾞﾘｯﾄﾞ on/off
	u8		pu[3];		// 0:TRUE==ｺﾝﾋﾟｭｰﾀ, 1:LEVEL( 0,1,2 ), 2:ﾃﾞﾓ no.
	u8		ef;			// 消去,落下用ｶｳﾝﾀ
	CONCAP	cap;		// 現在操作するｶﾌﾟｾﾙ
	CONCAP	nex;		// ﾈｸｽﾄｶﾌﾟｾﾙ
	BLKCAP	blk[MFy][MFx];//ﾌﾞﾛｯｸﾜｰｸ
	PREVWK	prv;		// 前回ﾃﾞｰﾀ取得ﾜｰｸ
	VSPLAY*	ene[4];		// 敵対戦ﾜｰｸﾎﾟｲﾝﾀ
	VSPLAY	vsp;		// 対戦用ﾜｰｸ
	AIWORK	ai;			// COM思考用ﾜｰｸ
	int		zz;			//( ﾃﾞｰﾀ保護ﾜｰｸ )
} PWORK;

//ﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾋﾙﾀﾙﾋ 汎用ﾜｰｸｽ
// EVS
extern u8		evs_fram1st;	// 1stﾌﾚｰﾑ
extern u8		evs_seqence;	// ｼｰｹﾝｽ on/off
extern u8		evs_seqnumb;	// ｼｰｹﾝｽ番号
extern u16		evs_seffect[];	// SE no.
extern u16		evs_seSetPtr;
extern u16		evs_seGetPtr;
extern u8		evs_selmenu;	// ｾﾚｸﾄﾒﾆｭｰ
extern u8		evs_selmen2;	// ｾﾚｸﾄﾒﾆｭｰ2P
extern u8		evs_playmax;	// ﾌﾟﾚｲﾔｰ数max( ｺﾝﾄﾛｰﾗ数 )
extern u8		evs_playcnt;	// ﾌﾟﾚｲﾔｰ数
extern PWORK	evs_playwrk[];	// ｹﾞｰﾑ総合処理
extern u8		evs_prgstat;	// ｹﾞｰﾑｽﾃｰﾀｽ
extern u8		evs_taskbar;	// ﾀｽｸﾊﾞｰ
extern u8		evs_bground;	// 背景
extern u8		evs_pauplay;	// ﾎﾟｰｽﾞ優先ﾌﾟﾚｲﾔ
extern short	evs_fadecol;	// ｶﾗｰﾌｪｰﾀﾞｰ
extern u8		evs_dsjudge;	// 判定描画
extern u8		evs_gamesel;	// ﾀｲﾄﾙでのｹﾞｰﾑｾﾚｸﾄ 0-3 == 1P,2P,4P,vsCPU
extern u8		evs_keyrept[];	// ｷｰﾘﾋﾟｰﾄ
extern int		evs_maincnt;	// ﾒｲﾝｶｳﾝﾀ
extern int		evs_grphcnt;	// ｸﾞﾗﾌｨｯｸｶｳﾝﾀ
extern u8		evs_mainx10;	// main10処理中ﾌﾗｸﾞ
extern u8		evs_graph10;	// graphic10許可ﾌﾗｸﾞ
extern u8		evs_aceldiv;	// 落下速度ﾚﾍﾞﾙｱｯﾌﾟ( 何個毎にｽﾋﾟｰﾄﾞｱｯﾌﾟするか )
extern u8		evs_nextprg;	// ｹﾞｰﾑ終了時帰り先
extern u8		evs_atk4pkb;	// ｷﾝｸﾞﾎﾞﾝﾋﾞｰ位置
extern int		evs_story;		// ｽﾄｰﾘｰﾓｰﾄﾞ番号
extern int		evs_4pnum;		// 4p mode MAN人数
extern u8		evs_trainf;		// 4p mode 練習モード攻撃ｱﾘ:TRUE/ﾅｼ:FALSE
extern u8		evs_gamespeed;	// ゲーム全体の速度(通常は１)

/*----- 渡辺追加分 -----*/
extern	s8		evs_vs_com_flg;		//	VSCOMモード使用可能フラグ		渡辺追加分	99/03/18
extern	s8		evs_seaclet_flg;	//	隠れキャラ使用フラグ			渡辺追加分	99/03/18
extern	s8		evs_story_flg;		//	ストーリーモード判別フラグ		渡辺追加分	99/03/30
extern	s8		evs_story_no;		//	ストーリーモードステージ番号	渡辺追加分	99/04/06
extern	u8		evs_sound_flg;		//	ステレオ・モノラル用フラグ		渡辺追加分	99/04/06
extern	s8		evs_story_level;	//	ストーリーモードの難易度		渡辺追加分	99/04/06
extern	s8		evs_vscom_level;	//	VSCOMの難易度					渡辺追加分	99/04/06
extern	s8		evs_level_flg;		//	レベルセレクトフラグ			渡辺追加分	99/05/07
extern	s8		evs_stage_no;		//	背景番号						渡辺追加分	99/05/07
extern	s8		evs_manual_no;		//	操作説明番号					渡辺追加分	99/07/30
extern	u16		evs_high_score;		//	最高得点						渡辺追加分	99/09/09
extern	u32		evs_game_time;		//	ストーリー用時間				渡辺追加分	99/09/10
/*-----	テスト -----*/
extern	u8		name_test[8][4];	//	名前データ
extern	u8		name_use_flg[8];	//	名前使用フラグ
/*------------------*/

/*----------------------*/


// 各種基本処理ﾃﾞｰﾀ
typedef struct {
	u16		wx,wy;
	s16		nx,ny;
} PERSON;

extern u8	GameSpeed[];			// ｹﾞｰﾑｽﾋﾟｰﾄﾞ
extern u8	FlyingCnt[];			// ﾌﾗｲﾝｸﾞｶｳﾝﾄ
extern u8	BonusWait[][3];			// ｶｰｿﾙｶﾌﾟｾﾙ y=1~3時, 余力ｳｴｲﾄ
extern u8	TouchDownWait[];		// ｶﾌﾟｾﾙ下にﾌﾞﾛｯｸが有る時の追加ｳｴｲﾄ
extern u8	NextWait[];				// 次ｶﾌﾟｾﾙへのｳｴｲﾄ
extern u8	FallSpeed[];			// 降下ｽﾋﾟｰﾄﾞ
extern u8	Score1p[][SCORE_MAX];	// ｳｲﾙｽ消去時得点表
extern u8	Gameselindex[];			// ｹﾞｰﾑｾﾚｸﾄ時のid設定ﾎﾟｲﾝﾄ
extern u8	CapsMagazine[];			// ｶﾌﾟｾﾙﾏｶﾞｼﾞﾝ
extern PERSON	Personalinit[];		// ﾜｰﾙﾄﾞ位置, ﾈｸｽﾄ位置
extern u8	GameSize[][6];			// ｹﾞｰﾑｻｲｽﾞ( 人数、BG番号, ｶﾌﾟｾﾙｻｲｽﾞ, Mﾌｨｰﾙﾄﾞx, y )
extern u8	AttackPattern[];		// 攻撃ﾊﾟﾀｰﾝ

extern u8	king_status;			// ｷﾝｸﾞﾎﾞﾝﾋﾞｰ現在ｽﾃｰﾀｽ
extern u8	king_playnx;			// ｷﾝｸﾞﾎﾞﾝﾋﾞｰﾌﾟﾚｲﾔｰ寄生位置 next keep
extern u8	king_player;			// ｷﾝｸﾞﾎﾞﾝﾋﾞｰﾌﾟﾚｲﾔｰ寄生位置
extern float	tray_posx[];				// 現在のﾄﾚｲxﾎﾟｼﾞｼｮﾝ( 描画用なので graphic4p.c にて初期化されている )
extern float	tray_posy[];				//	   ..	y	..	(						 ..						)
extern float	tray_next[];				// 次の	  ..		(						 ..						)
extern float	tray_move[];				// x座標移動値		(						 ..						)
extern u8	king_count[];					// 多機能ｶｳﾝﾀ
extern u8	king_index[];					// KingBomBody[x]
extern u8	KingBomBody[][KINGBODY_MAX];	// ｶﾌﾟｾﾙ格納

extern u8	story_virlv[][11];

// ｷｰｼｰｹﾝｽ: ﾃﾞｰﾀ[REC]しないのなら下記をｺﾒﾝﾄｱｳﾄ
//#define PAD_SEQ_KEEP

#define	JOYKEP	3600
#define	STARTJ	255
#define	ENDOFJ	254

typedef struct {
	u8	st,co;
} VIBLOCKm;
typedef struct {
	u8	count;
	u8	joykey;
} KEYSEQ;
typedef struct	tagLVSPD {
	u8	lv;
	u8	sp;
} LVSPD, *PLVSPD;

extern VIBLOCKm	viskeep[ID_MAX][MFy][MFx];	// Mﾌｨｰﾙﾄﾞ
extern u8		magkeep[MAG_NO][MAGA_MAX];	// ﾏｶﾞｼﾞﾝ
extern KEYSEQ	joykeep[ID_MAX][JOYKEP];	// ｺﾝﾄﾛｰﾗ
extern LVSPD	levkeep[IDMAXp];			// level,speed

// ｺﾝﾄﾛｰﾗ
extern u8		joygmf[];		// 下記使用ﾌﾗｸﾞ
extern u16		joygam[];		// ｹﾞｰﾑ用ｺﾝﾄﾛｰﾗﾌﾗｸﾞ
extern u8		keepjoy[];		// ｺﾝﾄﾛｰﾗ関連
extern u8		counjoy[];
extern u16		idexjoy[];

// ｽﾄｰﾘｰﾓｰﾄﾞ関連, ﾜｰｸ

extern u8		story_flag;

// ﾒｲﾝｸﾞﾛｰﾊﾞﾙ
extern PWORK	*pw;			//( ほかのﾒｲﾝでも使用中,注意 )
extern CONCAP	*caps,*next;
extern BLKCAP	*block;
extern VSPLAY	*vsp;

// ﾃﾞﾊﾞｯｸﾞｶｳﾝﾀ
extern u16	roll_invalid[];
extern u16	coffee_cnt;

// BGｾｸﾞﾒﾝﾄﾃｰﾌﾞﾙ (ﾒｲﾝ用 ) :ﾒﾓﾘ管理
// BGｾｸﾞﾒﾝﾄﾃｰﾌﾞﾙ (ﾒｲﾝ用 ) :ﾒﾓﾘ管理
//extern u32	SegBGaddr[][2];

extern	u64		BGBuffer[];

enum {
	BG_1P,
	BG_VS,
	BG_VS4P,
	BG_2PS1,
	BG_2PS2,
	BG_2PS3,
	BG_2PS4,
	BG_2PS5,
	BG_2PS6,
	BG_2PS7,
	BG_2PS8,
	BG_2PS9,
	BG_2PS10,
	BG_2PS11,
};

#define		BGB_SIZE	(256*2+320*240)

enum {
	GSA_KIHON,
	GSA_ATTACK,
	GSA_DAMAGE,
	GSA_WIN,
	GSA_LOSE,
	GSA_DRAW,
	GSA_TOUJOU,
};

//	渡辺追加分

#define	DAMAGE_MAX	0x10	//	最大被弾数(4P時)
#define	DAMAGE_TYPE			//	4P時の攻撃方法変更用フラグ

/*
//	キャラクター番号
enum	{
	DMC_MARIO,	//	0:マリオ
	DMC_NOKO,	//	1:ノコノコ
	DMC_BOM,	//	2:ボム兵
	DMC_PUKU,	//	3:プクプク
	DMC_CHORO,	//	4:チョロブー
	DMC_PRO,	//	5:プロペラヘイホー
	DMC_HANA,	//	6:ハナちゃん
	DMC_TERE,	//	7:テレサ
	DMC_PAK,	//	8:パックンフラワー
	DMC_KAME,	//	9:カメック
	DMC_KUPPA,	//	10:クッパ
	DMC_PEACH,	//	11:ピーチ姫
};
*/
//	基本アニメーションの配列番号
enum	{
	ANIME_opening,	//	登場
	ANIME_nomal,	//	通常
	ANIME_attack,	//	攻撃
	ANIME_damage,	//	被弾
	ANIME_win,		//	勝利
	ANIME_lose,		//	敗北
	ANIME_draw,		//	引き分け
};


//	ゲーム情報用構造体
typedef	struct	{
	s8	pos_m_x,pos_m_y;	//	表示座標(8 x 16)
	s8	capsel_m_g;			//	カプセルのグラフィック番号
	s8	capsel_m_p;			//	カプセルのパレット番号
	s8	capsel_m_flg[6];	//	カプセルの表示フラグと落下判定フラグ
}game_map;

//	落下カプセル用構造体
typedef	struct	{
	s8	pos_x[2],pos_y[2];	//	表示座標(8 x 16)
	s8	capsel_g[2];		//	カプセルのグラフィック番号
	s8	capsel_p[2];		//	カプセルのパレット番号
	s8	capsel_flg[4];		//	カプセルの表示フラグと落下判定フラグ
}game_cap;

//	攻撃カプセル用構造体
typedef	struct	{
	s8	pos_a_x,pos_a_y;	//	表示座標(8 x 16)
	s8	capsel_a_p;			//	カプセルのパレット番号
	s8	capsel_a_flg[3];	//	カプセルの表示フラグと落下判定フラグ
}game_a_cap;

//	キャラクタ( マリオ・ノコノコ等 )のデータ構造体
typedef	struct{
	u16	*anime_pal;			//	パレットポインタ
	u8	*anime_dat;			//	グラフィックポインタ(キャラクタを２回に分けて描くため)
	s8	pos_an_x,pos_an_y;	//	座標
	u8	def_w_size;			//	横幅
	u8	def_h_size;			//	縦幅
	u8	h_size[3];			//	0:分割１ 1:分割２ 2:分割３
	u8	aniem_wait;			//	表示時間
	s8	aniem_flg;			//	フラグ
}game_anime_data;

//	キャラクタ( マリオ・ノコノコ等 )の制御用構造体
typedef	struct{
	u8	cnt_charcter_no;		//	キャラクター番号(上記の DMC_???)
	u8	cnt_now_frame;			//	現在アニメーション配列の何番目を指しているか
	u8	cnt_anime_count;		//	次のアニメまでのカウンター
	u8	cnt_anime_loop_count;	//	ループ用カウンター
	s8	cnt_anime_flg;			//	フラグ
	u32	cnt_anime_address;		//	キャラクタグラフィックの読込みアドレス
	game_anime_data	**anime_charcter;	//	１キャラの全アニメーションデータの配列を指す
	game_anime_data	*cnt_now_type;	//	現在どのアニメーションをしているか
}game_anime;

//	ゲームの制御用構造体
typedef	struct	{
	u16	game_score;			//	得点
	s16	map_x,map_y;		//	マップの座標
	s8	map_item_size;		//	マップのアイテムの大きさ
	u8	game_mode[2];		//	0:内部処理番号 1:内部処理番号保存変数
	s8	game_condition[3];	//	0:その時の状態 2:その時の状態保存変数 3:変化しない状況変数(トレーニング中だと０以外)
	u8	virus_number;		//	ウイルス数
	u8	virus_level;		//	ウイルスレベル
	u8	virus_anime;		//	ウイルスアニメーション番号
	s8	virus_anime_vec;	//	ウイルスアニメーション進行方向
	u8	virus_anime_count;	//	ウイルスアニメーションカウンタ
	u8	virus_anime_max;	//	ウイルスアニメーションの最大コマ数
	u8	virus_anime_spead;	//	ウイルスアニメーション速度
	u8	cap_def_speed;		//	カプセル速度(SPEED_?(LOW/MID/HIGH/MAX))
	u8	cap_speed;			//	カプセル速度
	u8	cap_count;			//	カプセル落下数
	u8	cap_speed_count;	//	カプセル落下用カウンタ
	u8	cap_speed_vec;		//	カプセル落下用カウンタ増加値
	u8	cap_speed_max;		//	カプセル落下速度(カウンタがこの数値以上になると１段落下)
	u8	cap_magazine_cnt;	//	カプセルマガジン参照用変数
	u8	cap_magazine_save;	//	カプセルマガジン参照用保存変数
	s8	cap_move_se_flg;	//	カプセル左右移動時のＳＥを鳴らすためのフラグ
	u8	erase_anime;		//	消滅アニメーションコマ数
	u8	erase_anime_count;	//	消滅アニメーションカウンタ	//	粒落下のカウンタも兼ねる
	u8	erase_virus_count;	//	消滅ウイルス数
	u8	chain_count;		//	連鎖数
	u8	chain_line;			//	消滅列数
	u8	chain_color[4];		//	0: 赤 1:黄 2:青 消した色をカウントする。 3: 連鎖開始時に消した色のビットを立てる 0x01:赤 0x02:黄 0x04:青 ウイルス含む場合 0x80 のビットを立てる
	u8	warning_flg;		//	警戒音とうを鳴らすためのフラグ (ビットで制御する) 0x08:ウイルスラスト３ 0x80 積み上げすぎ
	u8	work_flg;			//	汎用性フラグ
	u8	retire_flg[3];		//	リタイアフラグ
	u8	player_state[3];	//	0:TRUE == コンピュータ 1:LEVEL(0.1.2); 2:プレイヤー番号
#ifdef	DAMAGE_TYPE
	u16	cap_attack_work[DAMAGE_MAX];	//	自分に落下する攻撃カプセルのワーク(2bitで制御,00:赤,01:黄,10:青,11:無し)
#endif
#ifndef	DAMAGE_TYPE
	u8	cap_attack_work[DAMAGE_MAX][6];	//	自分に落下する攻撃カプセルのワーク
#endif
	game_anime	anime;		//	アニメーション制御
	game_cap	now_cap;	//	現在操作するカプセル
	game_cap	next_cap;	//	次のカプセル
	AIWORK	ai;			// COM思考用ﾜｰｸ
	u8		pn;			// ﾌﾟﾚｲﾔｰ no.
	u8		gs;			// ｹﾞｰﾑｽﾃｰﾀｽ
	u8		lv;			// 設定ﾚﾍﾞﾙ(0-29)
	u8		vs;			// ｳｲﾙｽ残数(ｶｳﾝﾀではなく、毎ｲﾝﾄごとにﾏｯﾌﾟﾃﾞｰﾀ上から探した数字が入る)
	u8		pu[3];		// 0:TRUE==ｺﾝﾋﾟｭｰﾀ, 1:LEVEL( 0,1,2 ), 2:ﾃﾞﾓ no.
	AIBLK	blk[STD_MFieldY+1][STD_MFieldX];//ﾌﾞﾛｯｸﾜｰｸ
	AICAPS	cap;

}game_state;

//	ゲーム設定保存用構造体
typedef	struct{
	u8	virus_level;		//	ウイルスレベル
	u8	cap_def_speed;		//	カプセル速度(SPEED_?(LOW/MID/HIGH/MAX))
	u8	player_state[2];	//	0:TRUE == コンピュータ 1:LEVEL(0.1.2);
	u8	cnt_charcter_no;	//	キャラクター番号(上記の DMC_???)
}game_state_sub;

//	ゲーム設定保存用構造体
typedef	struct{
	game_state_sub	play_1p_mode;
	game_state_sub	play_vs_com_mode;
	game_state_sub	play_2p_mode[2];
	game_state_sub	play_4p_mode[4];
}game_state_old;


//	ウイルス配置用構造体
typedef	struct	{
	s8	virus_type;			//	ウイルスの種類(色も兼ねる)
	u8	x_pos,y_pos;		//	Ｘ・Ｙ座標
}virus_map;

extern	game_state_old	evs_state_old;			//	各設定の保存用変数
extern	game_state	game_state_data[4];			//	各プレイヤーの状態及び制御
extern	game_map	game_map_data[4][8 << 4];	//	瓶の中の情報
extern	virus_map	virus_map_data[4][8 << 4];	//	ウイルス設定用配列


extern	u8	dm_mode_select_back_bm0_0[];			//	モードセレクト用背景グラフィックデータ
extern	u16	dm_mode_select_back_bm0_0tlut[];		//	モードセレクト用背景パレットデータ


#define	DM_DEBUG_FLG	//	デバック機能用フラグ

#ifdef	DM_DEBUG_FLG

#define	NUM_RIGHT	0x80
#define	NUM_ZERO	0x40

typedef	struct{
	s16	x_pos,y_pos;
	u16	color;
	s8	*str;
}STRTBL;

typedef	struct{
	s16	x_pos,y_pos;
	u16	color;
	u8	flg;
	s16	num;
}NUMTBL;

typedef	struct{
	s16	x_pos,y_pos;
	u16	color;
	s16	max;
	s16	min;
	s16	*data;
}PARAMTBL;

extern		s16		num_keta[4];
extern		s16		param_pos[4];
extern		void	disp_font_load(void);
extern		void	disp_debug_font(s16	x_pos,s16 y_pos,s8 font);
extern		void	disp_debug_string(STRTBL *str);
extern		void	disp_debug_num(NUMTBL *num);
extern		void	disp_debug_param(u8 player_no,PARAMTBL *param, u8 ctrlno);
extern		void	cnt_debug_param( s8 player_no,PARAMTBL *param );
extern		void	cnt_debug_main(s8 player_no);
extern		void	disp_debug_main_1p(void);


#endif
// ON == 各種ﾃﾞﾊﾞｯｸﾞ操作,表示
#define DEBUG_ButtonAct_on

#endif
