#ifndef _EVSWORKS_H_
#define _EVSWORKS_H_

//////////////////////////////////////////////////////////////////////////////
// 汎用的定義文, ｼｽﾃﾑ

#define M_PI		3.14159265358979323846			// π
#define DegToRad(a)	((float)(a) * M_PI / 180.0)		// 度数からラジアン
#define RadToDeg(a)	((float)(a) / M_PI * 180.0)		// ラジアンから度数

//////////////////////////////////////////////////////////////////////////////
// ｹﾞｰﾑ定義数値

#if 0 // 削除
/*
#define STD_MFieldX	   8	// Mﾌｨｰﾙﾄﾞ標準 Xｻｲｽﾞ
#define STD_MFieldY	  17	//			   Yｻｲｽﾞ( 見えない列含む )
#define VS4_MFieldX	   8	// Mﾌｨｰﾙﾄﾞ 4p Xｻｲｽﾞ
#define VS4_MFieldY	  17	//			  Yｻｲｽﾞ( 見えない列含む )
#define MFITEM_SMALL   8	// Mﾌｨｰﾙﾄﾞｱｲﾃﾑｻｲｽﾞ S
#define MFITEM_LARGE  10	//		 ..		   L
#define LEVEL_MAX	  20	// ｹﾞｰﾑﾚﾍﾞﾙ
#define PLAYER_MAX	   4	// ﾌﾟﾚｲﾔｰ数
*/
#endif

#define FRAME_PAR_MSEC  6	// 1/10秒間のフレーム数
#define FRAME_PAR_SEC  60	// 1秒間のフレーム数
#define SCORE_MAX	   6	// ｽｺｱｲﾝﾃﾞｸｽ
#define MAGA_MAX	 256	// ﾏｶﾞｼﾞﾝmax
#define DEFAULT_HIGH_SCORE 56600 // デフォルトのハイスコア

//////////////////////////////////////////////////////////////////////////////
// 各種ｽﾃｰﾀｽ関連

#if 0 // 削除
/*
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
*/
#endif

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
	ONES_8_AB,	//-  ..  B
	ONES_ooBA,	//	 ..  C
	ONES_8_BA,	//	 ..  D
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

#if 0 // 削除
/*
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
*/
#endif

// ｹﾞｰﾑ設定 evs_gamesel
typedef enum {
	GSL_1PLAY,
	GSL_2PLAY,
	GSL_4PLAY,
	GSL_VSCPU,
	GSL_1DEMO,
	GSL_2DEMO,
	GSL_4DEMO,
	GSL_MAX,
} GAME_SELECT;

// ゲームモード
typedef enum {
	GMD_NORMAL,
	GMD_FLASH,
	GMD_TaiQ,
	GMD_TIME_ATTACK,
} GAME_MODE;

// 思考用ｷｬﾗｸﾀｰ番号
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

#if 0 // 削除
/*
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
*/
#endif

//////////////////////////////////////////////////////////////////////////////
// 汎用ﾜｰｸｽ EVS

extern u8 evs_stereo;    // true:ｽﾃﾚｵ or false:ﾓﾉﾗﾙ
extern u8 evs_seqence;   // ｼｰｹﾝｽ on/off
extern u8 evs_seqnumb;   // ｼｰｹﾝｽ番号
extern u8 evs_playmax;   // ﾌﾟﾚｲﾔｰ数max( ｺﾝﾄﾛｰﾗ数 )
extern u8 evs_playcnt;   // ﾌﾟﾚｲﾔｰ数
extern u8 evs_keyrept[]; // ｷｰﾘﾋﾟｰﾄ
extern u8 evs_gamespeed; // ゲーム全体の速度(通常は１)
extern u8 evs_score_flag;// スコア表示フラグ

extern GAME_SELECT evs_gamesel;  // ﾀｲﾄﾙでのｹﾞｰﾑｾﾚｸﾄ 0-3 == 1P,2P,4P,vsCPU
extern GAME_MODE   evs_gamemode; // ゲームモード

/*----- 渡辺追加分 -----*/
extern s8  evs_story_flg;         // ストーリーモード判別フラグ
extern s8  evs_story_no;          // ストーリーモードステージ番号
extern s8  evs_story_level;       // ストーリーモードの難易度
extern u8  evs_secret_flg[];      // 隠れキャラ使用フラグ [0]:vwario, [1]:mmario
extern u8  evs_one_game_flg;      // ストーリーステージセレクト時にON
extern u8  evs_level_21;          // "エンドレス２１レベルをクリアした" フラグ
extern s8  evs_manual_no;         // 操作説明番号
extern u8  evs_select_name_no[];  // 選択された名前の配列番号
extern u32 evs_high_score;        // 最高得点
extern int evs_vs_count;          // 対戦回数
extern u32 evs_game_time;         // ストーリー用時間
extern int evs_default_name[];    // 名前（NEW）のデータ
/*----------------------*/
// 各種基本処理ﾃﾞｰﾀ

extern u8 FlyingCnt[];          // ﾌﾗｲﾝｸﾞｶｳﾝﾄ
extern u8 BonusWait[][3];       // ｶｰｿﾙｶﾌﾟｾﾙ y=1~3時, 余力ｳｴｲﾄ
extern u8 TouchDownWait[];      // ｶﾌﾟｾﾙ下にﾌﾞﾛｯｸが有る時の追加ｳｴｲﾄ
extern u8 GameSpeed[];          // ｹﾞｰﾑｽﾋﾟｰﾄﾞ
extern u8 FallSpeed[];          // 降下ｽﾋﾟｰﾄﾞ
extern u8 Score1p[][SCORE_MAX]; // ｳｲﾙｽ消去時得点表
extern u8 CapsMagazine[];       // ｶﾌﾟｾﾙﾏｶﾞｼﾞﾝ

// 渡辺追加分
#define DM_MAX_TIME (5999 * FRAME_PAR_SEC) // 時間の最大値( 99分59秒 )

// キー対応表
#define	DM_KEY_A		0x8000
#define	DM_KEY_B		0x4000
#define DM_KEY_Z		0x2000
#define DM_KEY_START	0x1000
#define	DM_KEY_UP		0x0800
#define	DM_KEY_DOWN		0x0400
#define	DM_KEY_LEFT		0x0200
#define	DM_KEY_RIGHT	0x0100
#define	DM_KEY_L		0x0020
#define	DM_KEY_R		0x0010
#define	DM_KEY_CU		0x0008
#define	DM_KEY_CD		0x0004
#define	DM_KEY_CL		0x0002
#define	DM_KEY_CR		0x0001

#define	DM_ROTATION_R	DM_KEY_A
#define	DM_ROTATION_L	DM_KEY_B
#define	DM_ANY_KEY		0xff3f
#define	DM_KEY_OK		0x9000

// キャラクター番号
enum {
	DMC_MARIO,		//  0:マリオ
	DMC_WARIO,		//  1:ワリオ
	DMC_MAYU,		//  2:マユピー
	DMC_YARI,		//  3:ヤリマル
	DMC_RINGO,		//  4:りんごろう
	DMC_FUSEN,		//  5:フウセンまじん
	DMC_KAERU,		//  6:つまりカエル
	DMC_KURAGE,		//  7:ふっくらげ
	DMC_IKA,		//  8:イカタコテング
	DMC_KUMO,		//  9:キグモン
	DMC_ROBO,		// 10:ハンマーロボ
	DMC_MAD,		// 11:マッド＝シタイン
	DMC_NAZO,		// 12:なぞのぞう
	DMC_VWARIO,		// 13:バンパイアワリオ
	DMC_MMARIO,		// 14:メタルマリオ
};

//////////////////////////////////////////////////////////////////////////////

#endif // _EVSWORKS_H_
