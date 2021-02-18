//////////////////////////////////////////////////////////////////////////////
// ﾀｲﾄﾙ   : セーブデータ関連の宣言
// ﾌｧｲﾙ名 : record.h
// 作者   : 小林 康規
// 作成日 : 1999/03/10(水)
// 更新日 : 1999/10/28(木)
// 内容   : セーブデータ関連の宣言です
//////////////////////////////////////////////////////////////////////////////

#if !defined(_RECORD_H_)
#define _RECORD_H_

#include <ultra64.h>
#include "msgwnd.h"

//////////////////////////////////////////////////////////////////////////////
//{### マクロ

//## EEPRom のエラーコード
typedef enum EepRomErr {
	EepRomErr_NoError,    // OK
	EepRomErr_NotFound,   // EEPRom が無い
	EepRomErr_NotInit,    // 初期化されていない
	EepRomErr_ReadError,  // 読み込み中にエラー
	EepRomErr_WriteError, // 書き込み中にエラー
	EepRomErr_BadSum,     // チェックサム不良
} EepRomErr;

//////////////////////////////////////////////////////////////////////////////
//{### カーソル位置

// 新ゲーム設定保存用構造体
typedef struct {
	u8 st_lv;    // ゲームレベル
	u8 st_sh;    // S-HARD MODE
	u8 st_st;    // ステージ番号
	u8 st_no;    // 使用キャラクター番号 0:マリオ 1:ワリオ

	u8 p1_lv;    // LEVEL SELECT ウイルスレベル
	u8 p1_sp;    // LEVEL SELECT カプセル速度
	u8 p1_m;     // LEVEL SELECT 音楽

	u8 p1_ta_lv; // TIME ATTACK LEVEL
	u8 p1_tq_lv; // 耐久        LEVEL

	u8 vc_fl_lv[2]; // VSCOM フラッシュモードの難易度 (0: MAN 1:CPU )
	u8 vc_lv[2]; // VSCOM ウイルスレベル (0: MAN 1:CPU )
	u8 vc_sp[2]; // VSCOM カプセル速度 (0: MAN 1:CPU )
	u8 vc_no[2]; // VSCOM 使用キャラクター番号 (0: MAN 1:CPU )
	u8 vc_st;    // 背景番号
	u8 vc_m;     // VSCOM 音楽

	u8 vm_fl_lv; // VSMAN フラッシュモードの難易度
	u8 vm_ta_lv; // VSMAN タイムアタックの難易度
	u8 vm_lv;    // VSMAN ウイルスレベル
	u8 vm_sp;    // VSMAN カプセル速度
	u8 vm_no;    // VSMAN 使用キャラクター番号
	u8 vm_st;    // 背景番号
	u8 vm_m;     // VSMAN 音楽
} game_config;

typedef struct {
	u8 p4_team[4]; // 4P チーム番号
	u8 p4_lv[4];   // 4P ウイルスレベル
	u8 p4_fl_lv[4];// 4P フラッシュモードのゲームレベル
	u8 p4_no[4];   // 4P 使用キャラクター番号
	u8 p4_sp[4];   // 4Pカプセル速度
	u8 p4_st;      // 4P 背景番号
	u8 p4_m;       // 4P 音楽番号
} game_cfg_4p;

//////////////////////////////////////////////////////////////////////////////
//{### セーブデータ

// セーブ用ストリーモードのデータ
typedef struct {
	u32 score;   // 得点
	u32 time;    // クリアタイム
	u8  c_stage; // クリアステージ
} mem_story;

// セーブ用レベルセレクトのデータ
typedef struct {
	u32 score;   // 得点
	u8  c_level; // クリアレベル
} mem_level;

// セーブ用耐久モードのデータ
typedef struct {
	u32 score;   // 得点
	u32 time;    // タイム
} mem_taiQ;

// セーブ用タイムアタックのデータ
typedef struct {
	u32 score; // 得点
	u32 time;  // タイム
	u8  erase; // 消したウィルス
} mem_timeAt;

// セーブ用各個人のデータ
typedef struct {

	// さまざまなビットフラグ 下記の DM_MEM_? のビットが立つ
	u8 mem_use_flg;
		#define DM_MEM_USE 0x01 // 使用中
		#define DM_MEM_CAP 0x02 // カプセル落下位置表示のＯＮ／ＯＦＦ

	// 名前
	#define MEM_NAME_SIZE 4
	u8 mem_name[MEM_NAME_SIZE];

	// ストーリーモードのクリアーしたステージ[0:easy, 1:normal, 2:hard, 3:s-hard][0:mario, 1:wario]
	int clear_stage[4][2];

	// ストーリーモードのデータ(ゲームレベル毎)
	mem_story story_data[3];

	// レベルセレクトのデータ(ゲームスピード毎)
	mem_level level_data[3];

	// 耐久のデータ(ゲームレベル毎)
	mem_taiQ taiQ_data[3];

	// タイムアタックのデータ(ゲームレベル毎)
	mem_timeAt timeAt_data[3];

	// 勝敗数の最大値
	#define MEM_VS_RESULT_MAX 99

	// VSCOMの勝敗[0:勝利 1:敗北]
	u16 vscom_data[2];

	// VSCOM(FLASH)の勝敗[0:勝利 1:敗北]
	u16 vc_fl_data[2];

	// 2PLAYの勝敗[0:勝利 1:敗北]
	u16 vsman_data[2];

	// 2PLAY(FLASH)の勝敗[0:勝利 1:敗北]
	u16 vm_fl_data[2];

	// 2PLAY(TIME ATTACK)の勝敗[0:勝利 1:敗北]
	u16 vm_ta_data[2];

	// 各ゲームの設定
	game_config config;

} mem_char;

// 各個人のデータ変数
#define DM_MEM_GUEST 0x08 // ゲスト用配列番号
#define REC_CHAR_SIZE 8   // セーブする配列数
#define MEM_CHAR_SIZE 9   // ゲストを含む配列数
extern mem_char evs_mem_data[MEM_CHAR_SIZE];

// 4P各設定の保存用変数
extern game_cfg_4p evs_cfg_4p;

//////////////////////////////////////////////////////////////////////////////
//{### 各種データ操作

extern void dm_story_sort_set(int player_no, int char_no, int g_level, int score, int time, int c_stage, int ignoreFlag);
extern void dm_level_sort_set(int player_no, int g_speed, int score, int c_level);

extern void dm_taiQ_sort_set(int player_no, int dif, int score, int time);
extern void dm_timeAt_sort_set(int player_no, int dif, int score, int time, int erase);

extern void dm_vscom_set(int player_no, int win, int lose);
extern void dm_vc_fl_set(int player_no, int win, int lose);

extern void dm_vsman_set(int player_no, int win, int lose);
extern void dm_vm_fl_set(int player_no, int win, int lose);
extern void dm_vm_ta_set(int player_no, int win, int lose);

//////////////////////////////////////////////////////////////////////////////
//{### セーブデータ初期化関数群

extern void dm_init_config_save(game_config *config);
extern void dm_init_config_4p_save(game_cfg_4p *config);
extern void dm_init_story_save(mem_story *st);
extern void dm_init_level_save(mem_level *st);
extern void dm_init_save_mem(mem_char *mem);
extern void dm_init_system_mem();

//////////////////////////////////////////////////////////////////////////////
//{### 各種データをソート

typedef struct SRankSortInfo {
	u8 story_sort[3][REC_CHAR_SIZE];
	u8 story_rank[3][REC_CHAR_SIZE];

	u8 level_sort[3][REC_CHAR_SIZE];
	u8 level_rank[3][REC_CHAR_SIZE];

	u8 taiQ_sort[3][REC_CHAR_SIZE];
	u8 taiQ_rank[3][REC_CHAR_SIZE];

	u8 timeAt_sort[3][REC_CHAR_SIZE];
	u8 timeAt_rank[3][REC_CHAR_SIZE];

	u8  vscom_sort[REC_CHAR_SIZE];
	u8  vscom_rank[REC_CHAR_SIZE];
	u16 vscom_ave[REC_CHAR_SIZE]; // VSCOM の勝率を入れる

	u8  vc_fl_sort[REC_CHAR_SIZE];
	u8  vc_fl_rank[REC_CHAR_SIZE];
	u16 vc_fl_ave[REC_CHAR_SIZE]; // VSCOM(FLASH) の勝率を入れる

	u8  vsman_sort[REC_CHAR_SIZE];
	u8  vsman_rank[REC_CHAR_SIZE];
	u16 vsman_ave[REC_CHAR_SIZE]; // VSMAN の勝率を入れる

	u8  vm_fl_sort[REC_CHAR_SIZE];
	u8  vm_fl_rank[REC_CHAR_SIZE];
	u16 vm_fl_ave[REC_CHAR_SIZE]; // VSMAN(FLASH) の勝率を入れる

	u8  vm_ta_sort[REC_CHAR_SIZE];
	u8  vm_ta_rank[REC_CHAR_SIZE];
	u16 vm_ta_ave[REC_CHAR_SIZE]; // VSMAN(TIME ATTACK) の勝率を入れる
} SRankSortInfo;

extern void dm_data_mode_story_sort(SRankSortInfo *st);
extern void dm_data_mode_level_sort(SRankSortInfo *st);

extern void dm_data_mode_taiQ_sort(SRankSortInfo *st);
extern void dm_data_mode_timeAt_sort(SRankSortInfo *st);

extern void dm_data_vscom_sort(SRankSortInfo *st);
extern void dm_data_vc_fl_sort(SRankSortInfo *st);

extern void dm_data_vsman_sort(SRankSortInfo *st);
extern void dm_data_vm_fl_sort(SRankSortInfo *st);
extern void dm_data_vm_ta_sort(SRankSortInfo *st);

//////////////////////////////////////////////////////////////////////////////
//{### その他

//## 名前のセンタリングを行うのに必要な情報を取得
extern void fontName_getRange(const unsigned char *name, int *offset, int *size);

//////////////////////////////////////////////////////////////////////////////
//{### EEPRom 初期化

//## osContInit より後に呼びます
extern EepRomErr EepRom_Init();

//## 出荷時の初期化
extern EepRomErr EepRom_InitFirst(void (*proc)(void *), void *args);

//## 変数を初期化
extern void EepRom_InitVars();

//////////////////////////////////////////////////////////////////////////////
//{### 各構造体 <-> EEPRom

//## 全読み
extern EepRomErr EepRom_ReadAll();

//## 全書き
extern EepRomErr EepRom_WriteAll(void (*proc)(void *), void *args);

//////////////////////////////////////////////////////////////////////////////
//{### デバッグ用

//## エラーメッセージを出力
extern void EepRom_DumpErrMes(EepRomErr err);

//## 各データのサイズを出力
extern void EepRom_DumpDataSize();

//////////////////////////////////////////////////////////////////////////////
//{### EEPROM書き込み中の警告メッセージ

typedef struct {
	SMsgWnd msgWnd;
	int timeout;
	int count;
} RecWritingMsg;

extern void RecWritingMsg_init(RecWritingMsg *st, void **heap);
extern void RecWritingMsg_setStr(RecWritingMsg *st, const unsigned char *str);
extern void RecWritingMsg_calc(RecWritingMsg *st);
extern void RecWritingMsg_draw(RecWritingMsg *st, Gfx **gpp);
extern void RecWritingMsg_start(RecWritingMsg *st);
extern void RecWritingMsg_end(RecWritingMsg *st);
extern int  RecWritingMsg_isEnd(RecWritingMsg *st);
extern void RecWritingMsg_setPos(RecWritingMsg *st, int x, int y);

//////////////////////////////////////////////////////////////////////////////
//{### スリープタイマー

extern void setSleepTimer(int msec);

//////////////////////////////////////////////////////////////////////////////
//{### EOF

#endif // _RECORD_H_
