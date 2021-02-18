#include <ultra64.h>
#include <math.h>

#include "def.h"
#include "nnsched.h"
#include "sound.h"
#include "musdrv.h"
#include "util.h"
#include "evsworks.h"
#include "boot_data.h"
#include "debug.h"

//////////////////////////////////////////////////////////////////////////////
// マクロ

#define AUDIO_HEAP_SIZE 0x00031000
#define AUDIO_THREAD_PRI 50
//#define AUDIO_THREAD_PRI (MAIN_THREAD_PRI - 1)

#define AUDIO_SEQ_PBK_START (_romDataTbl[_pbk_drmario][0])
#define AUDIO_SEQ_PBK_END   (_romDataTbl[_pbk_drmario][1])
#define AUDIO_SEQ_PBK_SIZE ((u32)AUDIO_SEQ_PBK_END - (u32)AUDIO_SEQ_PBK_START)
#define AUDIO_SEQ_BUF_SIZE _getMaxSeqSize()
#define AUDIO_SEQ_BUF_COUNT 2

#define AUDIO_SND_FBK_START (_romDataTbl[_fbk_drmario][0])
#define AUDIO_SND_FBK_END   (_romDataTbl[_fbk_drmario][1])
#define AUDIO_SND_FBK_SIZE ((u32)AUDIO_SND_FBK_END - (u32)AUDIO_SND_FBK_START)
#define AUDIO_SND_BUF_COUNT 4

#define AUDIO_WBK_START (_romDataTbl[_wbk_drmario][0])

//////////////////////////////////////////////////////////////////////////////
// サウンドラベルのテーブル

const unsigned char _charSE_tbl[] = {
	FX_14_AT1, // マリオ
	FX_12_AT1, // ワリオ
	FX_08_AT1, // マユピー
	FX_09_AT1, // ヤリマル
	FX_10_AT1, // りんごろう
	FX_04_AT1, // フウセンまじん
	FX_06_AT1, // つまりカエル
	FX_05_AT1, // ふっくらげ
	FX_01_AT1, // イカタコテング
	FX_02_AT1, // キグモン
	FX_03_AT1, // ハンマーロボ
	FX_07_AT1, // マッド＝シタイン
	FX_11_AT1, // なぞのぞう
	FX_13_AT1, // バンパイアワリオ
	FX_15_AT1, // メタルマリオ
};

//////////////////////////////////////////////////////////////////////////////
// サウンドデータ

// シーケンスデータテーブル
static const int _seqDataTbl[] = {
	_seq_fever,   // fever
	_seq_feverF,  // fever
	_seq_chill,   // chill
	_seq_chillF,  // chill
	_seq_game_c,  // ゲーム画面、奇数ステージ
	_seq_game_cF, // ゲーム画面、奇数ステージ
	_seq_game_d,  // ゲーム画面、最終ステージを除く偶数ステージ
	_seq_game_dF, // ゲーム画面、最終ステージを除く偶数ステージ
	_seq_game_e,  // ゲーム画面、最終ステージ
	_seq_game_eF, // ゲーム画面、最終ステージ
	_seq_opening, // オープニング
	_seq_title,   // タイトル
	_seq_menu,    // メニュー
	_seq_staff,   // スタッフロール
	_seq_end_a,   // クリア／ウィン／ドロー／ルーズ（ＶＳ　ＣＯＭ・２人で遊ぶ・みんなで遊ぶモード）
	_seq_end_b,   // ３本先取した時
	_seq_end_b_s, // ３本先取した時(short ver)
	_seq_end_c,   // ゲームオーバー／ルーズ（ストーリーモード）
	_seq_end_c_s, // ゲームオーバー／ルーズ（ストーリーモード）(short ver)
	_seq_story_a, // ストーリー画面、奇数ステージ
	_seq_story_b, // ストーリー画面、「なぞのぞうのしんでん」ステージ除く偶数ステージ
	_seq_story_c, // ストーリー画面、「なぞのぞうのしんでん」
	_seq_ending,  // ストーリー画面、エンディング
	_seq_coffee,  // コーヒーブレーク
};

// シーケンスデータの最大値を取得
static int _getMaxSeqSize()
{
	void **seq;
	int i, size, maxSize = 0;

	for(i = 0; i < ARRAY_SIZE(_seqDataTbl); i++) {
		seq = _romDataTbl[ _seqDataTbl[i] ];
		size = (u32)seq[1] - (u32)seq[0];
		maxSize = MAX(size, maxSize);
	}

	return maxSize;
}

// SE のパラメータ
#include "sound/tune.h"

//////////////////////////////////////////////////////////////////////////////

// オーディオヒープ
static u8 heaparea[AUDIO_HEAP_SIZE];

//////////////////////////////////////////////////////////////////////////////
// サウンド管理

// サウンドステータス構造体
typedef struct {
	const FxTune *tune;
	int sndBuf;
	int frmCnt;
} SndState;

// サウンドステータスを初期化
static void sndState_init(SndState *st, int seqBuf)
{
	st->tune = 0;
	st->sndBuf = seqBuf;
	st->frmCnt = 0;
}

// FxTune を設定
static void sndState_set(SndState *st, const FxTune *tune)
{
	muSndStop(st->sndBuf, 0);
	st->tune = tune;
	st->frmCnt = 0;
}

// サウンドを再生する
static bool sndState_play(SndState *st)
{
	if(!st->tune || muSndAsk(st->sndBuf) != 0) {
		return false;
	}

	muSndPlay(st->sndBuf, st->tune->no);
	muSndSetVol(st->sndBuf, st->tune->vol);
	muSndSetFreq(st->sndBuf, st->tune->coarse * (1.0 / 8.0));

	return true;
}

// サウンドを再生する2
static bool sndState_play2(SndState *st)
{
	if(!st->tune || muSndAsk(st->sndBuf) != 0) {
		return false;
	}

	muSndPlay2(st->sndBuf, st->tune->no, st->tune->vol, 0x80, TRUE, st->tune->pri);
	muSndSetFreq(st->sndBuf, st->tune->coarse * (1.0 / 8.0));

	return true;
}

// フレーム毎の更新処理
static void sndState_update(SndState *st)
{
	if(!st->tune) {
		return;
	}

	// 再生が終了しているかチェック
	if(muSndAsk(st->sndBuf) == 0) {
		st->tune = 0;
	}
	// 再生時間が経過しているかチェック
	else if(st->tune->time) {
		if(st->frmCnt < st->tune->time * FRAME_PAR_SEC / 10) {
			st->frmCnt++;
		}
		else {
			muSndStop(st->sndBuf, 0);
			st->tune = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void dm_audio_set_stereo(int stereo)
{
	extern Acmd *alEnvmixerPull(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p);
	u32 *addr[2] = {
		(u32 *)((u8 *)alEnvmixerPull + 0x0000010c),
		(u32 *)((u8 *)alEnvmixerPull + 0x000002ec),
	};

/*XXXblythe - this seems like a bad idea */
#if defined(NDEBUG) && 0
	if(stereo) {
		*addr[0] = 0x92220012;
		*addr[1] = 0x8C82000C;
	}
	else {
		*addr[0] = 0x24020040;
		*addr[1] = 0x24020040;
	}
#endif

	// 全てのデータキャッシュラインをライトバックし無効化します
	osWritebackDCacheAll();
}

//////////////////////////////////////////////////////////////////////////////
// ドクマリ用オーディオドライバー

// オーディオドライバー構造体
typedef struct {
	int seqNo[AUDIO_SEQ_BUF_COUNT];
	SndState sndState[AUDIO_SND_BUF_COUNT];
	SndState *sndQueue[AUDIO_SND_BUF_COUNT];
} AudioDriver;

AudioDriver audDrv;

// サウンドドライバーを初期化
void dm_audio_init_driver(NNSched *sched)
{
	AudioDriver *st = &audDrv;
	int i;

	// 再生中のシーケンス番号
	for(i = 0; i < AUDIO_SEQ_BUF_COUNT; i++) {
		st->seqNo[i] = -1;
	}

	// サウンドステータスを初期化
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		sndState_init(&st->sndState[i], i);
	}

	// MUSドライバーを初期化
	i = muInitDriver(
		sched,
		heaparea,
		AUDIO_HEAP_SIZE,
		AUDIO_SEQ_PBK_SIZE,
		AUDIO_SEQ_BUF_SIZE,
		AUDIO_SEQ_BUF_COUNT,
		AUDIO_SND_FBK_SIZE,
		AUDIO_SND_BUF_COUNT,
		AUDIO_THREAD_PRI);
	PRTFL();
	PRT1("use audio heap %08x + α\n", i);

	// サンプルポインタバンクを読み込む
	muSetPbkData(AUDIO_SEQ_PBK_START, AUDIO_SEQ_PBK_SIZE, AUDIO_WBK_START);

	// サウンドエフェクトバンクを読み込む
	muSetFbkData(AUDIO_SND_FBK_START, AUDIO_SND_FBK_SIZE);
}

// サウンドドライバーの処理を更新
void dm_audio_update()
{
	AudioDriver *st = &audDrv;
	int i;

	// シーケンスを切り替える
	for(i = 0; i < AUDIO_SEQ_BUF_COUNT; i++) {
		if(st->seqNo[i] >= 0 && _dm_seq_is_stopped(i)) {
			void **seq = _romDataTbl[ _seqDataTbl[st->seqNo[i]] ];
			muSeqSetData(i, seq[0], (u32)seq[1] - (u32)seq[0]);
			muSeqPlay(i);
			muSeqSetVol(i, 0x80);
			st->seqNo[i] = -1;
		}
	}

	// サウンド再生処理
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {

		// サウンドキューをチェック
		if(st->sndQueue[i]) {
			// サウンドを再生
			if(sndState_play(st->sndQueue[i])) {
				st->sndQueue[i] = 0;
			}
		}
		// アップデート
		else {
			sndState_update(&st->sndState[i]);
		}
	}
}

// 全サウンドを停止
void dm_audio_stop()
{
	dm_seq_stop();
	MusStop(MUSFLAG_SONGS | MUSFLAG_EFFECTS, 0);
}

// 全サウンドが停止しているか確認
int dm_audio_is_stopped()
{
	return MusAsk(MUSFLAG_SONGS | MUSFLAG_EFFECTS) == 0;
}

//////////////////////////////////////////////////////////////////////////////

// シーケンスを再生する
void  dm_seq_play(int seqNo)            { _dm_seq_play(0, seqNo); }
void _dm_seq_play(int bufNo, int seqNo) { _dm_seq_play_fade(bufNo, seqNo, 0); }

// シーケンスを再生する2
void  dm_seq_play_fade(int seqNo, int fade) { _dm_seq_play_fade(0, seqNo, fade); }
void _dm_seq_play_fade(int bufNo, int seqNo, int fade)
{
	AudioDriver *st = &audDrv;

	if(seqNo != st->seqNo[bufNo]) {
		muSeqStop(bufNo, fade);
		st->seqNo[bufNo] = seqNo;
	}
}

// シーケンスを再生する(ゲーム画面用)
void  dm_seq_play_in_game(int seqNo) { _dm_seq_play_in_game(0, seqNo); }
void _dm_seq_play_in_game(int bufNo, int seqNo)
{
	if(evs_seqence) {
		_dm_seq_play(bufNo, seqNo);
	}
	else {
		switch(seqNo) {
		case SEQ_Fever:  case SEQ_FeverF:
		case SEQ_Chill:  case SEQ_ChillF:
		case SEQ_Game_C: case SEQ_Game_CF:
		case SEQ_Game_D: case SEQ_Game_DF:
		case SEQ_Game_E: case SEQ_Game_EF:
			_dm_seq_stop(bufNo);
			break;
		default:
			_dm_seq_play(bufNo, seqNo);
			break;
		}
	}
}

// シーケンスを停止する
void  dm_seq_stop() { _dm_seq_stop(0); }
void _dm_seq_stop(int bufNo)
{
	AudioDriver *st = &audDrv;

	muSeqStop(bufNo, 0);
	st->seqNo[bufNo] = -1;
}

// シーケンスのボリュームを設定
void  dm_seq_set_volume(int vol) { _dm_seq_set_volume(0, vol); }
void _dm_seq_set_volume(int bufNo, int vol)
{
	AudioDriver *st = &audDrv;
	muSeqSetVol(bufNo, vol);
}

// シーケンスが停止しているか調べる
int  dm_seq_is_stopped() { _dm_seq_is_stopped(0); }
int _dm_seq_is_stopped(int bufNo)
{
	return muSeqAsk(bufNo) == 0;
}

//////////////////////////////////////////////////////////////////////////////

// サウンドを再生する
void dm_snd_play(int sndNo)
{
	AudioDriver *st = &audDrv;
	const FxTune *tune, *newTune = &_fxTuneTbl[sndNo];
	int i;

	// キューに同じ音がある場合は中断
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(!st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// 同一音か?
		if(tune && tune->no == newTune->no) {
			return;
		}
	}

	// 空のバッファをチェック
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// NULL?
		if(!tune) {
			sndState_set(&st->sndState[i], newTune);
			st->sndQueue[i] = &st->sndState[i];
			return;
		}
	}

	// 再生中のバッファをチェック
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// プライオリティ <= ?
		if(tune && tune->pri <= newTune->pri) {
			sndState_set(&st->sndState[i], newTune);
			st->sndQueue[i] = &st->sndState[i];
			return;
		}
	}

	// キューをチェック
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(!st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// プライオリティ < ?
		if(tune && tune->pri < newTune->pri) {
			sndState_set(&st->sndState[i], newTune);
			return;
		}
	}

	// 再生を断念
}

// サウンドを再生する(ゲーム画面用)
void dm_snd_play_in_game(int sndNo)
{
	if(evs_gamespeed < 6) {
		dm_snd_play(sndNo);
	}
}

// サウンドエフェクトの数を返します
int dm_snd_get_effects()
{
	return MusFxBankNumberOfEffects(muGetFbkAddr());
}

// 変な音を再生
void dm_snd_play_strange_sound()
{
	int snd[4]; // <- 変な音バッファー
	int i, j;

	// 変な音を作成
	for(i = 0; i < ARRAY_SIZE(snd); i++) {
		snd[i] = rand() % 15;
		for(j = 0; j < i; j++) {
			if(snd[i] == snd[j]) {
				i--;
				break;
			}
		}
	}

	// 変な音を再生
	for(i = 0; i < ARRAY_SIZE(snd); i++) {
		dm_snd_play(SE_getDamage(snd[i]));
	}
}

//////////////////////////////////////////////////////////////////////////////

