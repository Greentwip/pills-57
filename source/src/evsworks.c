
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include "aiset.h"
#include "evsworks.h"
#include "segment.h"
#include "record.h"

//////////////////////////////////////////////////////////////////////////////
// EVSﾜｰｸ ( prototype in evsworks.h )

u8 evs_stereo = TRUE;         // true:ｽﾃﾚｵ or false:ﾓﾉﾗﾙ
u8 evs_seqence = TRUE;        // ｼｰｹﾝｽ on/off
u8 evs_seqnumb = 0;           // ｼｰｹﾝｽ番号
u8 evs_playmax;               // ﾌﾟﾚｲﾔｰ数max( ｺﾝﾄﾛｰﾗ数 )
u8 evs_playcnt = 1;           // ﾌﾟﾚｲﾔｰ
u8 evs_keyrept[2] = { 12,6 }; // ｷｰﾘﾋﾟｰﾄ
u8 evs_gamespeed = 1;         // ゲーム全体の速度(通常は１)
u8 evs_score_flag = 1;        // スコア表示フラグ

GAME_SELECT evs_gamesel;      // ﾀｲﾄﾙでのｹﾞｰﾑｾﾚｸﾄ 0-3 == 1P,2P,4P,vsCPU
GAME_MODE   evs_gamemode;     // ゲームモード

s8  evs_story_flg = 0;            // ストーリーモード判別フラグ
s8  evs_story_no = 0;             // ストーリーモードステージ番号
s8  evs_story_level = 1;          // ストーリーモードの難易度
u8  evs_secret_flg[2] = { 0, 0 }; // 隠れキャラ使用フラグ [0]:vwario, [1]:mmario
u8  evs_one_game_flg = 0;         // ストーリーステージセレクト時にON
u8  evs_level_21 = 0;             // "エンドレス２１レベルをクリアした" フラグ
s8  evs_manual_no = 0;            // 操作説明番号
u8  evs_select_name_no[2];        // 選択された名前の配列番号
u32 evs_high_score = 0;           // 最高得点
int evs_vs_count = 3;             // 対戦回数
u32 evs_game_time;                // ストーリー用時間
int evs_default_name[MEM_NAME_SIZE];// 名前（NEW）のデータ

//////////////////////////////////////////////////////////////////////////////
// 各種基本処理ﾃﾞｰﾀ

// ｶﾌﾟｾﾙ投げ( default ={ 39,18,19 } )
u8 FlyingCnt[] = { 40, 20, 20 };

// ｶｰｿﾙｶﾌﾟｾﾙ y=1~3時, 投入余力ｳｴｲﾄ
u8 BonusWait[][3] = {
	// { L M H },
	{  2,  2,  3, },
	// { L M H },
	{  1,  1,  1, },
	// { L M H }
	{  1,  0,  1, },
};

// ｶﾌﾟｾﾙ下にﾌﾞﾛｯｸが有る時の追加ｳｴｲﾄ( != 0 )	 1P 2P 3P 4P
u8 TouchDownWait[] = { 1, 2, 2, 2 };

// ｽﾋﾟｰﾄﾞ初期ｲﾝﾃﾞｸｽ by FallSpeed[]		Low Mid Hi Max
u8 GameSpeed[] = { 0, 10, 15, 55, };

// 落下ｽﾋﾟｰﾄﾞ
u8 FallSpeed[] = {
	39,37,35,33,31,
	29,27,25,23,21,
	19,18,17,16,15,
	14,13,12,11,10,
	 9, 9, 8, 8, 7,
	 7, 6, 6, 5, 5,
	 5, 5, 5, 5, 5,
	 5, 5, 5, 5, 5,
	 4, 4, 4, 4, 4,
	 3, 3, 3, 3, 3,
	 2, 2, 2, 2, 2,
	 1,
};

// ｽｺｱ計算
u8 Score1p[][SCORE_MAX] = {
	{  1, 2, 4, 8,16,32, },
	{  2, 4, 8,16,32,64, },
	{  3, 6,12,24,48,96, },
};

// ｶﾌﾟｾﾙﾏｶﾞｼﾞﾝ
u8 CapsMagazine[MAGA_MAX];

//////////////////////////////////////////////////////////////////////////////
