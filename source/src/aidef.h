#if !defined(_AIDEF_H_)
#define _AIDEF_H_

//////////////////////////////////////////////////////////////////////////////
// マクロ

#define LEVLIMIT 23 // 内部ﾚﾍﾞﾙ上限
#define MFieldX   8 // Mﾌｨｰﾙﾄﾞ Xｻｲｽﾞ
#define MFieldY  17 //         Yｻｲｽﾞ( 見えない列含む )

//////////////////////////////////////////////////////////////////////////////
// ai work 構造体

#define	ROOTCNT		50
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
typedef struct {
	u8	ok;			// 移動可能ﾌﾗｸﾞ
	u8	tory;		// 縦/横ﾌﾗｸﾞ
	u8	x;			// X座標
	u8	y;			// Y座標
	u8	rev;		// 反転ﾌﾗｸﾞ
	u8	ccnt;		// 同色数
					//------- re-make by hiru
	s32	pri;		// 優先度( ﾎﾟｲﾝﾄ計算 )
	s32	dead;		// 危険度(	  ..	 )
	u8	hei[2][LnTableMax];	// 縦ﾗｲﾝ計測( ﾏｽﾀｰ､ｽﾚｰﾌﾞ )
	u8	wid[2][LnTableMax];	// 横  ..	(	  ..	 )
	u8	elin[2];	// 消去ﾗｲﾝ合計数( ﾒｲﾝ,ｻﾌﾞ )
	u8	only[2];	// ﾏｽﾀｰ､ｽﾚｰﾌﾞ単独ﾌﾗｸﾞ
	u8	wonly[2];	// ﾏｽﾀｰ､ｽﾚｰﾌﾞ単独ﾌﾗｸﾞ(横ﾗｲﾝ専用)
	u8	sub;		// ｽﾚｰﾌﾞ側は ﾒｲﾝ? ｻﾌﾞ?
	u8	rensa;		// 連鎖するかﾌﾗｸﾞ
} AI_FLAG;

typedef struct {
	u8	x;
	u8	y;
} AI_ROOT;

typedef struct {
	AI_FLAG	aiFlagDecide;			//
	AI_ROOT	aiRootDecide[ROOTCNT];	// 投下ｶﾌﾟｾﾙのﾙｰﾄ
	u8	aiKeyCount;					//
	u8	aiSpeedCnt;					//
	u8	aiKRFlag;					// CPUｷｰ操作変更時ｳｴｲﾄ
	u8	aiRollCnt;					// 操作ｶﾌﾟｾﾙ回転方向ｶｳﾝﾀ
	u8	aiRollFinal;				// 最後に回転操作するかどうかﾌﾗｸﾞ
	u8	aiRollHabit;				// 操作ｶﾌﾟｾﾙを回転させる癖ﾌﾗｸﾞ
	u8	aiSpUpFlag;					// ｽﾋﾟｰﾄﾞｱｯﾌﾟするかﾌﾗｸﾞ
	u8	aiSpUpStart;				// ｽﾋﾟｰﾄﾞｱｯﾌﾟするｽﾀｰﾄ位置
//	u8	aiSpUpCnt;					// ｽﾋﾟｰﾄﾞｱｯﾌﾟするｶｳﾝﾄ
	u8	aivl;						// virus level
	u8	aiok;						// ﾙｰﾄ検索を行ったかどうかﾌﾗｸﾞ
	u8	aiRandFlag;					// ﾎﾟｲﾝﾄ判定時乱数をﾌﾟﾗｽするかﾌﾗｸﾞ
	u8	aiEX;						// 目的地Ｘ座標
	u8	aiEY;						// 目的地Ｙ座標
	u8	aiOldRollCnt;				// 旧操作ｶﾌﾟｾﾙ回転方向ｶｳﾝﾀ(目的地に行けなかったチェック用)
//	u8	aiNum;						// キャラ別思考用番号
//	u8	aiTimer;					// キャラ別思考用タイマー

	#define NUM_AI_EFFECT 16
	u8	aiEffectNo[NUM_AI_EFFECT];// 実行中の特殊処理番号
	s16	aiEffectParam[NUM_AI_EFFECT];// 実行中の特殊処理のパラメータ
	s16	aiEffectCount[NUM_AI_EFFECT];// 実行中の特殊処理制御カウンタ

	u8	aiState;					// COMの状態ﾌﾗｸﾞ
		#define	AIF_DAMAGE	0x01
		#define	AIF_DAMAGE2	0x02
		#define	AIF_AGAPE	0x04

	u8	aiSelSpeed;					// COMの操作スピード
	u8	aiRootP;					// 移動距離比率(移動範囲が狭くなっていることをチェック)
	u16	aiPriOfs;					// 下手度
} AIWORK;

typedef struct {
	u8			mx,my;		// mﾌｨｰﾙﾄﾞx,y(8x15)
	u8			ca;			// ｶﾗｰa番号					:COL_?(RED/YELLOW/...)
	u8			cb;			//	..b	   ..
	u8			sp;			// FallSpeed[ｲﾝﾃﾞｸｽ], ｶｳﾝﾀ上限
	u8			cn;			//	   ..	  ｶｳﾝﾀ( next時は CapsMagazine[ｲﾝﾃﾞｸｽ] )
} AICAPS;			// ｴｯｼﾞ情報

typedef struct {
	u8			st;			// ﾌﾞﾛｯｸの種類				:MB_?(CAPS/VIRUS/...)
	u8			co;			// ｶﾗｰ番号					:COL_?(RED/YELLOW/...)
} AIBLK;

//////////////////////////////////////////////////////////////////////////////

#endif // _AIDEF_H_
