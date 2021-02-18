#ifndef _SOUND_H_
#define _SOUND_H_

#include "sound/se.h"

//////////////////////////////////////////////////////////////////////////////
// サウンドのラベル

enum {
	SEQ_Fever,		// fever
	SEQ_FeverF,		// fever
	SEQ_Chill,		// chill
	SEQ_ChillF,		// chill
	SEQ_Game_C,		// ゲーム画面、奇数ステージ
	SEQ_Game_CF,	// ゲーム画面、奇数ステージ
	SEQ_Game_D,		// ゲーム画面、最終ステージを除く偶数ステージ
	SEQ_Game_DF,	// ゲーム画面、最終ステージを除く偶数ステージ
	SEQ_Game_E,		// ゲーム画面、最終ステージ
	SEQ_Game_EF,	// ゲーム画面、最終ステージ
	SEQ_Opening,	// オープニング
	SEQ_Title,		// タイトル
	SEQ_Menu,		// メニュー
	SEQ_Staff,		// スタッフロール
	SEQ_End_A,		// クリア／ウィン／ドロー／ルーズ（ＶＳ　ＣＯＭ・２人で遊ぶ・みんなで遊ぶモード）
	SEQ_End_B,		// ３本先取した時
	SEQ_End_BS,		// ３本先取した時(short ver)
	SEQ_End_C,		// ゲームオーバー／ルーズ（ストーリーモード）
	SEQ_End_CS,		// ゲームオーバー／ルーズ（ストーリーモード）(short ver)
	SEQ_Story_A,	// ストーリー画面、奇数ステージ
	SEQ_Story_B,	// ストーリー画面、「なぞのぞうのしんでん」ステージ除く偶数ステージ
	SEQ_Story_C,	// ストーリー画面、「なぞのぞうのしんでん」
	SEQ_Ending,		// ストーリー画面、エンディング
	SEQ_Coffee,		// コーヒーブレーク
	_SEQ_SUM,
};

extern const unsigned char _charSE_tbl[];

// 攻撃音
#define SE_getAttack(charNo, attackLv) (_charSE_tbl[(charNo)] + (attackLv))

// ダメージ音
#define SE_getDamage(charNo)           (_charSE_tbl[(charNo)] + 3)

// 勝利
#define SE_getWin(charNo)              (_charSE_tbl[(charNo)] + 4)

enum {
	SE_gFreeFall      = FX_SE01, // カプセル落下音の番号
	SE_gVrsErase      = FX_SE02, // ウイルス消去音の番号
	SE_gVrsErsAl      = FX_SE03, // 巨大ウイルス消滅御
	SE_gSpeedUp       = FX_SE04, // 落下速度アップ音の番号
	SE_gCombo1P       = FX_SE05, // １Ｐ連鎖時の音
	SE_gPause         = FX_SE06, // ポーズ効果音
	SE_gCapErase      = FX_SE07, // カプセル消去音の番号
	SE_mDecide        = FX_SE08, // 決定
	SE_mLeftRight     = FX_SE09, // メニュー時のカーソル移動
	SE_mUpDown        = FX_SE10, // メニュー切り替え

	SE_gLeftRight     = FX_SE11, // 左右移動音の番号
	SE_gTouchDown     = FX_SE12, // 着地音の番号
	SE_gCapRoll       = FX_SE13, // 回転音の番号
	SE_mCANCEL        = FX_SE14, // キャンセル
	SE_mDispChg       = FX_SE15, // 画面きり変え
	SE_mDataErase     = FX_SE16, // バックアップデータ削除
	SE_mError         = FX_SE17, // エラー
	SE_gCount1        = FX_SE18, // カウントダウン,
	SE_gCountEnd      = FX_SE19, // カウントダウン終了
	SE_gVirusStruggle = FX_SE20, // 巨大ウイルスダメージ

	SE_gCombo2P       = FX_SE21, // ２Ｐ連鎖時の音
	SE_gCombo3P       = FX_SE22, // ３Ｐ連鎖時の音
	SE_gCombo4P       = FX_SE23, // ４Ｐ連鎖時の音
	SE_gCoin          = FX_SE24, // コイン獲得
	SE_gWarning       = FX_SE25, // カプセル積み上げ警告音
	SE_gReach         = FX_SE26, // ウイルスが残り３個のときの音

	SE_gBottomUp      = FX_SE27, // 
	SE_gEraseFlash    = FX_SE28, // 
	SE_mGameStart     = FX_SE29, // 

//	SE_gREraseP11,      // 1P連鎖音起点
//	SE_gREraseP12,      // 1P２連鎖音
//	SE_gREraseP13,      // 1P３連鎖以上音
//	SE_gDamageP1,       // 1Pくらい音

//	SE_gREraseP21,      // 2P連鎖音起点
//	SE_gREraseP22,      // 2P２連鎖音
//	SE_gREraseP23,      // 2P３連鎖以上音
//	SE_gDamageP2,       // 2Pくらい音
//	SE_gDamageP22,      // 2Pくらい音

//	SE_xNoRequest,
};

//////////////////////////////////////////////////////////////////////////////

extern void dm_audio_set_stereo(int stereo);

extern void dm_audio_init_driver(NNSched *sched);
extern void dm_audio_update();

extern void dm_audio_stop();
extern int  dm_audio_is_stopped();

extern void  dm_seq_play(int seqNo);
extern void _dm_seq_play(int bufNo, int seqNo);

extern void  dm_seq_play_fade(int seqNo, int fade);
extern void _dm_seq_play_fade(int bufNo, int seqNo, int fade);

extern void  dm_seq_play_in_game(int seqNo);
extern void _dm_seq_play_in_game(int bufNo, int seqNo);

extern void  dm_seq_stop();
extern void _dm_seq_stop();

extern void  dm_seq_set_volume(int vol);
extern void _dm_seq_set_volume(int bufNo, int vol);

extern int  dm_seq_is_stopped();
extern int _dm_seq_is_stopped(int bufNo);

extern void dm_snd_play(int sndNo);
extern void dm_snd_play_in_game(int sndNo);

extern void dm_snd_play_strange_sound();

//////////////////////////////////////////////////////////////////////////////

#endif
