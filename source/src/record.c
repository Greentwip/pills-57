//////////////////////////////////////////////////////////////////////////////
// ﾀｲﾄﾙ   : セーブデータ関連のプログラム
// ﾌｧｲﾙ名 : record.c
// 作者   : 小林 康規
// 作成日 : 1999/03/10(水)
// 更新日 : 1999/10/28(木)
// 内容   : セーブデータ関連のプログラムです
//////////////////////////////////////////////////////////////////////////////

#include <ultra64.h>
#include "evsworks.h"

#include "nnsched.h"

#include "record.h"

#include "def.h"
#include "util.h"
#include "tex_func.h"

#include "debug.h"
#include "local.h"

//////////////////////////////////////////////////////////////////////////////
//{### マクロ

//## 各データロックで重複するビットサイズ
#define SCORE_BITS      17
#define SCORE2_BITS     16 // スコアアタックのスコア 633000
#define TIME_BITS       13
#define TIME2_BITS      11 // スコアアタックのタイム 1800
#define VS_RESULT_BITS   7
#define CHECK_SUM_BITS   8 // 各プレイヤーのチェックサム
#define CHECK_SUM_BITS2  4 // システム情報のチェックサム

//## スコアは、100ノ位まで使用する。
//## タイムアタックの場合は、10の位まで使用。
#define SCORE_DIV  100
#define SCORE2_DIV  10

#define BPUT(count, bits) BitField_PutBit(bf, (count), (bits))
#define BGET(count, bits) ((void)((bits) = BitField_GetBit(bf, (count))))

#define EEPROM_MAXSIZE (EEPROM_MAXBLOCKS * EEPROM_BLOCK_SIZE)
#define EEPROM_HEADER_BLOCK (EEPROM_MAXBLOCKS - 1)
#define EEPROM_HEADER_ADDR  (EEPROM_HEADER_BLOCK * EEPROM_BLOCK_SIZE)

//////////////////////////////////////////////////////////////////////////////
//{### データ

//## EEPROM書き込み中の警告メッセージの下地
#define USE_TEX_SIZE
#include "texture/font/mess_panel.txt"
#undef USE_TEX_SIZE

//## コントローラーメッセージキュー
#define EEPROM_MQ (&joyMsgQ)
extern OSMesgQueue	joyMsgQ;

//## EEPRom が初期化されている事を示すラベル
static const char eeprom_header[4] = { 'D', 'M', '6', '4' };
static const char eeprom_header_bits[4] = { 7, 7, 6, 6 };

//////////////////////////////////////////////////////////////////////////////
//{### ビットを詰めてバッファに出力

typedef struct BitField {
	u8  *buf;
	int bufSiz;
	int bufIdx;
	int bitIdx;
	u32 sum;
} BitField;

//## バッファーを設定
static void BitField_Attach(BitField *st, void *buf, int size)
{
	st->buf = (u8 *)buf;
	st->bufSiz = size;
	st->bufIdx = 0;
	st->bitIdx = 0;
	st->sum = 0;
}

//## 構造体を初期化
static void BitField_Init(BitField *st, void *buf, int size)
{
	bzero(buf, size);
	BitField_Attach(st, buf, size);
}

//## ビットを出力
static void BitField_PutBit(BitField *st, int count, u32 bits)
{
	int i;

	st->sum += bits & ((1 << count) - 1);

	for(i = count - 1; i >= 0; i--) {
		st->buf[st->bufIdx] |= ((bits >> i) & 1) << st->bitIdx;

		st->bitIdx++;
		st->bufIdx += st->bitIdx >> 3;
		st->bitIdx &= 7;
	}

	DBG0(st->bufIdx >= st->bufSiz, "ビットが溢れた\n");
}

//## ビットを取得
static u32 BitField_GetBit(BitField *st, int count)
{
	u32 bits = 0;
	int i;

	for(i = count - 1; i >= 0; i--) {
		bits |= ((st->buf[st->bufIdx] >> st->bitIdx) & 1) << i;

		st->bitIdx++;
		st->bufIdx += st->bitIdx >> 3;
		st->bitIdx &= 7;
	}

	DBG0(st->bufIdx >= st->bufSiz, "ビットが溢れた\n");

	st->sum += bits;

	return bits;
}

//////////////////////////////////////////////////////////////////////////////
//{### セーブデータ

// 各個人のデータ変数
mem_char evs_mem_data[MEM_CHAR_SIZE];

// 4P各設定の保存用変数
game_cfg_4p evs_cfg_4p;

//////////////////////////////////////////////////////////////////////////////
//{### セーブデータ初期化関数群

//////////////////////////////////////
//## カーソルの位置などの初期化

void dm_init_config_save(game_config *config)
{
	config->st_lv    = 1;         // NORMAL
	config->st_sh    = 0;         // S-HARD MODE
	config->st_st    = 0;         // STAGE 1
	config->st_no    = 0;         // MARIO

	config->p1_lv    = 10;        // LEVEL 10
	config->p1_sp    = 1;         // SPEAD MID
	config->p1_m     = 0;         // MUSIC

	config->p1_ta_lv = 1;         // TIME ATTACK LEVEL NORMAL
	config->p1_tq_lv = 1;         // 耐久        LEVEL NORMAL

	config->vc_fl_lv[0] = 1;      // VSCOM FLASH LEVEL NORMAL
	config->vc_fl_lv[1] = 1;      // VSCOM FLASH LEVEL NORMAL
	config->vc_lv[0] = 10;        // VSCOM LEVEL 10
	config->vc_lv[1] = 10;        // VSCOM LEVEL 10
	config->vc_sp[0] = 1;         // VSCOM SPEAD MID
	config->vc_sp[1] = 1;         // VSCOM SPEAD MID
	config->vc_no[0] = DMC_MARIO; // VSCOM CHARCTER NO
	config->vc_no[1] = DMC_WARIO; // VSCOM CHARCTER NO
	config->vc_st    = 0;         // BG 0
	config->vc_m     = 0;         // MUSIC

	config->vm_fl_lv = 1;         // VSMAN FLASH LEVEL NORMAL
	config->vm_ta_lv = 1;         // VSMAN TIME ATTACK LEVEL NORMAL
	config->vm_lv    = 10;        // VSMAN LEVEL 10
	config->vm_sp    = 1;         // VSMAN SPEAD MID
	config->vm_no    = DMC_MARIO; // VSMAN CHARCTER NO
	config->vm_st    = 0;         // BG 0
	config->vm_m     = 0;         // MUSIC
}

//////////////////////////////////////
//## ４人対戦の初期化
void dm_init_config_4p_save(game_cfg_4p *config)
{
	int i;

	for(i = 0; i < 4; i++) {
		config->p4_team[i] = i % 2; // チーム
		config->p4_lv[i]   = 10;    // ウイルスレベル
		config->p4_fl_lv[i]= 1;     // フラッシュモードのゲームレベル
		config->p4_no[i]   = i;     // キャラクター番号
		config->p4_sp[i]   = 1;     // 初期速度
	}

	config->p4_st  = 0; // 背景
	config->p4_m   = 0; // 音楽
}

//////////////////////////////////////
//## ストーリーモードの初期化
void dm_init_story_save(mem_story *st)
{
	st->score   = 0;
	st->time    = 0;
	st->c_stage = 0;
}

//////////////////////////////////////
//## レベルセレクトの初期化
void dm_init_level_save(mem_level *st)
{
	st->score   = 0;
	st->c_level = 0;
}

//////////////////////////////////////
//## 耐久モードの初期化
void dm_init_taiQ_save(mem_taiQ *st)
{
	st->score = 0;
	st->time  = 0;
}

//////////////////////////////////////
//## タイムアタックモードの初期化
void dm_init_timeAt_save(mem_timeAt *st)
{
	st->score = 0;
	st->time  = 0;
	st->erase = 0;
}

//////////////////////////////////////
//## プレイヤー一人分の初期化
void dm_init_save_mem(mem_char *mem)
{
	int i, j;

	// 使用フラグ
	mem->mem_use_flg = 0;//DM_MEM_CAP;

	// ストーリーモードのクリアーしたステージ
	for(i = 0; i < ARRAY_SIZE(mem->clear_stage); i++) {
		for(j = 0; j < ARRAY_SIZE(mem->clear_stage[i]); j++) {
			mem->clear_stage[i][j] = 0;
		}
	}

	// 初期名前(ゲスト)設定
	for(i = 0; i < MEM_NAME_SIZE; i++) {
		mem->mem_name[i] = evs_default_name[i];
	}

	// ストーリーモードデータ
	for(i = 0; i < ARRAY_SIZE(mem->story_data); i++) {
		dm_init_story_save(&mem->story_data[i]);
	}

	// レベルセレクトデータ
	for(i = 0; i < ARRAY_SIZE(mem->level_data); i++) {
		dm_init_level_save(&mem->level_data[i]);
	}

	// 耐久データ
	for(i = 0; i < ARRAY_SIZE(mem->taiQ_data); i++) {
		dm_init_taiQ_save(&mem->taiQ_data[i]);
	}

	// タイムアタックデータ
	for(i = 0; i < ARRAY_SIZE(mem->timeAt_data); i++) {
		dm_init_timeAt_save(&mem->timeAt_data[i]);
	}

	// VSCOMデータ
	for(i = 0; i < ARRAY_SIZE(mem->vscom_data); i++) {
		mem->vscom_data[i] = 0;
	}

	// VSCOM(FLASH)データ
	for(i = 0; i < ARRAY_SIZE(mem->vc_fl_data); i++) {
		mem->vc_fl_data[i] = 0;
	}

	// VSMANデータ
	for(i = 0; i < ARRAY_SIZE(mem->vsman_data); i++) {
		mem->vsman_data[i] = 0;
	}

	// VSMAN(FLASH)データ
	for(i = 0; i < ARRAY_SIZE(mem->vm_fl_data); i++) {
		mem->vm_fl_data[i] = 0;
	}

	// VSMAN(TIME ATTACK)データ
	for(i = 0; i < ARRAY_SIZE(mem->vm_ta_data); i++) {
		mem->vm_ta_data[i] = 0;
	}

	// 設定の初期化
	dm_init_config_save(&mem->config);
}

//////////////////////////////////////
//## システムの初期化
void dm_init_system_mem()
{
	static const char _defName[] = {
#if LOCAL==CHINA
		0x81,0x40,
		0x81,0x40,
		0x81,0x40,
		0x81,0x40,
#else
		0x82,0x6d, // Ｎ
		0x82,0x64, // Ｅ
		0x82,0x76, // Ｗ
		0x81,0x40, //
#endif
	};
	int i;

	// デフォルトの名前を設定
	for(i = 0; i < MEM_NAME_SIZE; i++) {
		evs_default_name[i] = font2index((char *)&_defName[i << 1]);
	}

	evs_stereo = TRUE;
	dm_audio_set_stereo(evs_stereo);

	evs_gamesel = GSL_1PLAY;
	evs_secret_flg[0] = evs_secret_flg[1] = 0;
	evs_level_21 = 0;
	evs_vs_count = 3;
	evs_score_flag = 1;
}

//////////////////////////////////////////////////////////////////////////////
//{### 各種データを操作

//## ストーリーモードセーブデータソート・入れ替え関数
void dm_story_sort_set(int player_no, int char_no, int g_level, int score, int time, int c_stage, int ignoreFlag)
{
	mem_char *mc = &evs_mem_data[player_no];

	time /= FRAME_PAR_SEC;
	time = MIN(5999, time);
	c_stage = MAX(0, c_stage - 1);

	if(!ignoreFlag && g_level < 3) {
		mem_story *ms = &mc->story_data[g_level];
		int hi, lo;

		do {
			hi = ms->c_stage;
			lo = c_stage;
			if(hi != lo) break;

			hi = ms->score;
			lo = score;
			if(hi != lo) break;

			hi = time;
			lo = ms->time;
		} while(0);

		if(hi < lo) {
			ms->score   = score;
			ms->time    = time;
			ms->c_stage = c_stage;
		}
	}

	// 最大クリアステージ数を保存
	mc->clear_stage[g_level][char_no] = MAX(mc->clear_stage[g_level][char_no], c_stage);
	mc->clear_stage[g_level][char_no] = MIN(mc->clear_stage[g_level][char_no], 7);
}

//## LEVEL セーブデータソート・入れ替え関数
void dm_level_sort_set(int player_no, int g_speed, int score, int c_level)
{
	mem_char *mc = &evs_mem_data[player_no];
	mem_level *ml = &mc->level_data[g_speed];
	int hi, lo;

	do {
		hi = ml->c_level;
		lo = c_level;
		if(hi != lo) break;

		// 同カプセル速度の場合得点で競う
		hi = ml->score;
		lo = score;
	} while(0);

	if(hi < lo) {
		ml->score   = score;
		ml->c_level = c_level;
	}
}

//## 耐久 セーブデータソート・入れ替え関数
void dm_taiQ_sort_set(int player_no, int dif, int score, int time)
{
	mem_char *mc = &evs_mem_data[player_no];
	mem_taiQ *tq = &mc->taiQ_data[dif];
	int hi, lo;

	time /= FRAME_PAR_SEC;
	time = MIN(5999, time);

	do {
		hi = tq->score;
		lo = score;
		if(hi != lo) break;

		// 同スコアの場合、せり上がり回数で競う
		hi = tq->time;
		lo = time;
	} while(0);

	if(hi < lo) {
		tq->score = score;
		tq->time  = time;
	}
}

//## タイムアタック セーブデータソート・入れ替え関数
void dm_timeAt_sort_set(int player_no, int dif, int score, int time, int erase)
{
	mem_char *mc = &evs_mem_data[player_no];
	mem_timeAt *ta = &mc->timeAt_data[dif];
	int hi, lo;

	time /= FRAME_PAR_MSEC;
	time = MIN(1800, time);

	do {
		hi = ta->score;
		lo = score;
		if(hi != lo) break;

		// 同スコアの場合、タイムで競う
		hi = time;
		lo = ta->time;
		if(hi != lo) break;

		// 同タイムの場合、消したウィルス数で競う
		hi = ta->erase;
		lo = erase;
	} while(0);

	if(hi < lo) {
		ta->score = score;
		ta->time  = time;
		ta->erase = erase;
	}
}

//## VSCPU セーブデータ設定関数
void dm_vscom_set(int player_no, int win, int lose)
{
	mem_char *mc = &evs_mem_data[player_no];

	mc->vscom_data[0] = MIN(MEM_VS_RESULT_MAX, mc->vscom_data[0] + win);
	mc->vscom_data[1] = MIN(MEM_VS_RESULT_MAX, mc->vscom_data[1] + lose);
}

//## VSCPU(FLASH) セーブデータ設定関数
void dm_vc_fl_set(int player_no, int win, int lose)
{
	mem_char *mc = &evs_mem_data[player_no];

	mc->vc_fl_data[0] = MIN(MEM_VS_RESULT_MAX, mc->vc_fl_data[0] + win);
	mc->vc_fl_data[1] = MIN(MEM_VS_RESULT_MAX, mc->vc_fl_data[1] + lose);
}

//## 2PLAY セーブデータ設定関数
void dm_vsman_set(int player_no, int win, int lose)
{
	mem_char *mc = &evs_mem_data[player_no];

	mc->vsman_data[0] = MIN(MEM_VS_RESULT_MAX, mc->vsman_data[0] + win);
	mc->vsman_data[1] = MIN(MEM_VS_RESULT_MAX, mc->vsman_data[1] + lose);
}

//## 2PLAY(FLASH) セーブデータ設定関数
void dm_vm_fl_set(int player_no, int win, int lose)
{
	mem_char *mc = &evs_mem_data[player_no];

	mc->vm_fl_data[0] = MIN(MEM_VS_RESULT_MAX, mc->vm_fl_data[0] + win);
	mc->vm_fl_data[1] = MIN(MEM_VS_RESULT_MAX, mc->vm_fl_data[1] + lose);
}

//## 2PLAY(TIME ATTACK) セーブデータ設定関数
void dm_vm_ta_set(int player_no, int win, int lose)
{
	mem_char *mc = &evs_mem_data[player_no];

	mc->vm_ta_data[0] = MIN(MEM_VS_RESULT_MAX, mc->vm_ta_data[0] + win);
	mc->vm_ta_data[1] = MIN(MEM_VS_RESULT_MAX, mc->vm_ta_data[1] + lose);
}

//////////////////////////////////////////////////////////////////////////////
//{### 各種データをソート

//## 記録データソート関数の内部インターフェイス群
typedef enum {
	_1P_STORY,
	_1P_LEVEL,
	_1P_TaiQ,
	_1P_TimeAt,
} _1P_MODE;
static u8 *_get1PSort(SRankSortInfo *st, _1P_MODE mode, int level)
{
	u8 *ptr;
	switch(mode) {
	case _1P_STORY:  ptr = st->story_sort[level];  break;
	case _1P_LEVEL:  ptr = st->level_sort[level];  break;
	case _1P_TaiQ:   ptr = st->taiQ_sort[level];   break;
	case _1P_TimeAt: ptr = st->timeAt_sort[level]; break;
	}
	return ptr;
}
static u8 *_get1PRank(SRankSortInfo *st, _1P_MODE mode, int level)
{
	u8 *ptr;
	switch(mode) {
	case _1P_STORY:  ptr = st->story_rank[level];  break;
	case _1P_LEVEL:  ptr = st->level_rank[level];  break;
	case _1P_TaiQ:   ptr = st->taiQ_rank[level];   break;
	case _1P_TimeAt: ptr = st->timeAt_rank[level]; break;
	}
	return ptr;
}
static bool _get1PLess(mem_char *mc1, mem_char *mc2, _1P_MODE mode, int level)
{
	int hi, lo;

	switch(mode) {
	case _1P_STORY: {
		mem_story *story1 = &mc1->story_data[level];
		mem_story *story2 = &mc2->story_data[level];
		hi = story1->c_stage;
		lo = story2->c_stage;
		if(hi != lo) break;
		hi = story1->score;
		lo = story2->score;
		if(hi != lo) break;
		hi = story2->time;
		lo = story1->time;
		} break;

	case _1P_LEVEL: {
		mem_level *level1 = &mc1->level_data[level];
		mem_level *level2 = &mc2->level_data[level];
		hi = level1->c_level;
		lo = level2->c_level;
		if(hi != lo) break;
		hi = level1->score;
		lo = level2->score;
		} break;

	case _1P_TaiQ: {
		mem_taiQ *taiQ1 = &mc1->taiQ_data[level];
		mem_taiQ *taiQ2 = &mc2->taiQ_data[level];
		hi = taiQ1->time;
		lo = taiQ2->time;
		if(hi != lo) break;
		hi = taiQ1->score;
		lo = taiQ2->score;
		} break;

	case _1P_TimeAt: {
		mem_timeAt *timeAt1 = &mc1->timeAt_data[level];
		mem_timeAt *timeAt2 = &mc2->timeAt_data[level];
		hi = timeAt1->score;
		lo = timeAt2->score;
		if(hi != lo) break;
		hi = timeAt2->time;
		lo = timeAt1->time;
		if(hi != lo) break;
		hi = timeAt1->erase;
		lo = timeAt2->erase;
		} break;
	}

	return hi < lo;
}

//## 順位ソート関数
static void _sort1PMode(SRankSortInfo *st, _1P_MODE mode, int level)
{
	mem_char *mc = evs_mem_data;
	u8 *sort = _get1PSort(st, mode, level);
	u8 *rank = _get1PRank(st, mode, level);
	int i, j, rankNo, hi, lo;

	for(i = 0; i < REC_CHAR_SIZE; i++) {
		sort[i] = i;
	}

	// ソート
	for(i = 0; i < REC_CHAR_SIZE - 1; i++) {
		for(j = i + 1; j < REC_CHAR_SIZE; j++) {
			hi = mc[sort[i]].mem_use_flg & DM_MEM_USE;
			lo = mc[sort[j]].mem_use_flg & DM_MEM_USE;

			if(_get1PLess(&mc[sort[i]], &mc[sort[j]], mode, level) || hi < lo) {
				u8 temp;
				SWAP(sort[i], sort[j], temp);
			}
		}
	}

	// 表示用順位設定
	rank[0] = rankNo = 1;
	for(i = 1; i < REC_CHAR_SIZE; i++) {
		if(_get1PLess(&mc[sort[i]], &mc[sort[i-1]], mode, level)) {
			rankNo++;
		}
		rank[i] = rankNo;
	}
}

//## ストーリーモード順位ソート関数
void dm_data_mode_story_sort(SRankSortInfo *st)
{
	int i;
	for(i = 0; i < 3; i++) {
		_sort1PMode(st, _1P_STORY, i);
	}
}

//## レベルセレクト順位ソート関数
void dm_data_mode_level_sort(SRankSortInfo *st)
{
	int i;
	for(i = 0; i < 3; i++) {
		_sort1PMode(st, _1P_LEVEL, i);
	}
}

//## 耐久モード順位ソート関数
void dm_data_mode_taiQ_sort(SRankSortInfo *st)
{
	int i;
	for(i = 0; i < 3; i++) {
		_sort1PMode(st, _1P_TaiQ, i);
	}
}

//## 耐久モード順位ソート関数
void dm_data_mode_timeAt_sort(SRankSortInfo *st)
{
	int i;
	for(i = 0; i < 3; i++) {
		_sort1PMode(st, _1P_TimeAt, i);
	}
}

//## 記録データ(VSモード)ソート関数の内部インターフェイス群
typedef enum {
	_VS_COM,
	_VS_COM_FLASH,
	_VS_MAN,
	_VS_MAN_FLASH,
	_VS_MAN_TIME_AT,
} _VS_MODE;
static u16 *_getVsResult(mem_char *mc, _VS_MODE mode)
{
	u16 *ptr;
	switch(mode) {
	case _VS_COM:         ptr = mc->vscom_data; break;
	case _VS_COM_FLASH:   ptr = mc->vc_fl_data; break;
	case _VS_MAN:         ptr = mc->vsman_data; break;
	case _VS_MAN_FLASH:   ptr = mc->vm_fl_data; break;
	case _VS_MAN_TIME_AT: ptr = mc->vm_ta_data; break;
	}
	return ptr;
}
static u8 *_getVsSort(SRankSortInfo *st, _VS_MODE mode)
{
	u8 *ptr;
	switch(mode) {
	case _VS_COM:         ptr = st->vscom_sort; break;
	case _VS_COM_FLASH:   ptr = st->vc_fl_sort; break;
	case _VS_MAN:         ptr = st->vsman_sort; break;
	case _VS_MAN_FLASH:   ptr = st->vm_fl_sort; break;
	case _VS_MAN_TIME_AT: ptr = st->vm_ta_sort; break;
	}
	return ptr;
}
static u8 *_getVsRank(SRankSortInfo *st, _VS_MODE mode)
{
	u8 *ptr;
	switch(mode) {
	case _VS_COM:         ptr = st->vscom_rank; break;
	case _VS_COM_FLASH:   ptr = st->vc_fl_rank; break;
	case _VS_MAN:         ptr = st->vsman_rank; break;
	case _VS_MAN_FLASH:   ptr = st->vm_fl_rank; break;
	case _VS_MAN_TIME_AT: ptr = st->vm_ta_rank; break;
	}
	return ptr;
}
static u16 *_getVsAve(SRankSortInfo *st, _VS_MODE mode)
{
	u16 *ptr;
	switch(mode) {
	case _VS_COM:         ptr = st->vscom_ave; break;
	case _VS_COM_FLASH:   ptr = st->vc_fl_ave; break;
	case _VS_MAN:         ptr = st->vsman_ave; break;
	case _VS_MAN_FLASH:   ptr = st->vm_fl_ave; break;
	case _VS_MAN_TIME_AT: ptr = st->vm_ta_ave; break;
	}
	return ptr;
}

//## VSモード 順位ソート関数
static void _sortVsMode(SRankSortInfo *st, _VS_MODE mode)
{
	mem_char *mc = evs_mem_data;
	u8 *sort = _getVsSort(st, mode);
	u8 *rank = _getVsRank(st, mode);
	u16 *ave = _getVsAve(st, mode);
	int i, j, aveVal, rankNo, hi, lo;

	// VSCOM 勝率計算
	for(i = 0; i < REC_CHAR_SIZE; i++) {
		u16 *res = _getVsResult(&mc[i], mode);

		sort[i] = i;

		// 試合をしていた場合, 勝率割り出し
		if(res[0] != 0 || res[1] != 0) {
			ave[i] = res[0] * 1000 / (res[0] + res[1]);
		}
		else {
			ave[i] = 0;
		}
	}

	// ソート
	for(i = 0; i < REC_CHAR_SIZE - 1; i++) {
		for(j = i + 1; j < REC_CHAR_SIZE; j++) {
			do {
				hi = ave[sort[i]];
				lo = ave[sort[j]];
				if(hi != lo) break;
				hi = mc[sort[i]].mem_use_flg & DM_MEM_USE;
				lo = mc[sort[j]].mem_use_flg & DM_MEM_USE;
			} while(0);

			if(hi < lo) {
				u8 temp;
				SWAP(sort[i], sort[j], temp);
			}
		}
	}

	// 表示用順位設定
	aveVal = ave[sort[0]];
	rank[0] = rankNo = 1;
	for(i = 1; i < REC_CHAR_SIZE; i++) {
		if(ave[sort[i]] != aveVal) {
			aveVal = ave[sort[i]];
			rankNo++;
		}
		rank[i] = rankNo;
	}
}

//## VSCOM 順位ソート関数
void dm_data_vscom_sort(SRankSortInfo *st)
{
	_sortVsMode(st, _VS_COM);
}

//## VSCOM(FLASH) 順位ソート関数
void dm_data_vc_fl_sort(SRankSortInfo *st)
{
	_sortVsMode(st, _VS_COM_FLASH);
}

//## VSMAN 順位ソート関数
void dm_data_vsman_sort(SRankSortInfo *st)
{
	_sortVsMode(st, _VS_MAN);
}

//## VSMAN(FLASH) 順位ソート関数
void dm_data_vm_fl_sort(SRankSortInfo *st)
{
	_sortVsMode(st, _VS_MAN_FLASH);
}

//## VSMAN(TIME ATTACK) 順位ソート関数
void dm_data_vm_ta_sort(SRankSortInfo *st)
{
	_sortVsMode(st, _VS_MAN_TIME_AT);
}

//////////////////////////////////////////////////////////////////////////////
//{### その他

//## 名前のセンタリングを行うのに必要な情報を取得
void fontName_getRange(const unsigned char *name, int *offset, int *size)
{
	int i, start, end;

	start = end = 0;

	for(i = 0; i < MEM_NAME_SIZE; i++) {
		if(name[i] != 0) {
			start = i;
			break;
		}
	}

	for(; i < MEM_NAME_SIZE; i++) {
		if(name[i] != 0) {
			end = i + 1;
		}
	}

	*offset = start;
	*size = end - start;
}

//////////////////////////////////////////////////////////////////////////////
//{### システム情報

//## 圧縮
static void RecSystem_Compress(BitField *bf)
{
	BPUT(1, evs_stereo ? 1 : 0);
	BPUT(1, evs_secret_flg[0] ? 1 : 0);
	BPUT(1, evs_secret_flg[1] ? 1 : 0);
	BPUT(1, evs_level_21 ? 1 : 0);
	BPUT(2, evs_vs_count);
	BPUT(1, evs_score_flag ? 1 : 0);
}

//## 展開
static void RecSystem_Extract(BitField *bf)
{
	BGET(1, evs_stereo);
	BGET(1, evs_secret_flg[0]);
	BGET(1, evs_secret_flg[1]);
	BGET(1, evs_level_21);
	BGET(2, evs_vs_count);
	BGET(1, evs_score_flag);

	dm_audio_set_stereo(evs_stereo);
}

//////////////////////////////////////////////////////////////////////////////
//{### プレイヤー

//## 圧縮
static void RecPlayer_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	BPUT(2, player->mem_use_flg);

	for(i = 0; i < ARRAY_SIZE(player->mem_name); i++) {
		BPUT(8, player->mem_name[i]);
	}
}

//## 展開
static void RecPlayer_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	BGET(2, player->mem_use_flg);

	for(i = 0; i < ARRAY_SIZE(player->mem_name); i++) {
		BGET(8, player->mem_name[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### ストーリーモード

//## 圧縮
static void RecStory_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i, j;

	for(i = 0; i < ARRAY_SIZE(player->clear_stage); i++) {
		for(j = 0; j < ARRAY_SIZE(player->clear_stage[i]); j++) {
			BPUT(3, player->clear_stage[i][j]);
		}
	}

	for(i = 0; i < ARRAY_SIZE(player->story_data); i++) {
		mem_story *story = &player->story_data[i];

		BPUT(SCORE_BITS, story->score / SCORE_DIV);
		BPUT(TIME_BITS,  story->time);
		BPUT(4,          story->c_stage);
	}
}

//## 展開
static void RecStory_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i, j;

	for(i = 0; i < ARRAY_SIZE(player->clear_stage); i++) {
		for(j = 0; j < ARRAY_SIZE(player->clear_stage[i]); j++) {
			BGET(3, player->clear_stage[i][j]);
		}
	}

	for(i = 0; i < ARRAY_SIZE(player->story_data); i++) {
		mem_story *story = &player->story_data[i];

		BGET(SCORE_BITS, story->score);
		story->score *= SCORE_DIV;

		BGET(TIME_BITS,  story->time);
		BGET(4,          story->c_stage);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### レベルセレクト

//## 圧縮
static void RecLevel_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->level_data); i++) {
		mem_level *level = &player->level_data[i];

		BPUT(SCORE_BITS, level->score / SCORE_DIV);
		BPUT(7,          level->c_level);
	}
}

//## 展開
static void RecLevel_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->level_data); i++) {
		mem_level *level = &player->level_data[i];

		BGET(SCORE_BITS, level->score);
		level->score *= SCORE_DIV;

		BGET(7,          level->c_level);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### 耐久

//## 圧縮
static void RecLevelTaiQ_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->taiQ_data); i++) {
		mem_taiQ *taiQ = &player->taiQ_data[i];

		BPUT(SCORE_BITS, taiQ->score / SCORE_DIV);
		BPUT(TIME_BITS,  taiQ->time);
	}
}

//## 展開
static void RecLevelTaiQ_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->taiQ_data); i++) {
		mem_taiQ *taiQ = &player->taiQ_data[i];

		BGET(SCORE_BITS, taiQ->score);
		taiQ->score *= SCORE_DIV;

		BGET(TIME_BITS,  taiQ->time);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### タイムアタック

//## 圧縮
static void RecLevelTimeAT_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->timeAt_data); i++) {
		mem_timeAt *timeAt = &player->timeAt_data[i];

		BPUT(SCORE2_BITS, timeAt->score / SCORE2_DIV);
		BPUT(TIME2_BITS,  timeAt->time);
		BPUT(6,           timeAt->erase);
	}
}

//## 展開
static void RecLevelTimeAT_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->timeAt_data); i++) {
		mem_timeAt *timeAt = &player->timeAt_data[i];

		BGET(SCORE2_BITS, timeAt->score);
		timeAt->score *= SCORE2_DIV;

		BGET(TIME2_BITS,  timeAt->time);
		BGET(6,           timeAt->erase);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### VS COM

//## 圧縮
static void RecVsCom_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vscom_data); i++) {
		BPUT(VS_RESULT_BITS, player->vscom_data[i]);
	}
}

//## 展開
static void RecVsCom_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vscom_data); i++) {
		BGET(VS_RESULT_BITS, player->vscom_data[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### VS COM(フラッシュ)

//## 圧縮
static void RecVsComFlash_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vc_fl_data); i++) {
		BPUT(VS_RESULT_BITS, player->vc_fl_data[i]);
	}
}

//## 展開
static void RecVsComFlash_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vc_fl_data); i++) {
		BGET(VS_RESULT_BITS, player->vc_fl_data[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### VS MAN

//## 圧縮
static void RecVsMan_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vsman_data); i++) {
		BPUT(VS_RESULT_BITS, player->vsman_data[i]);
	}
}

//## 展開
static void RecVsMan_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vsman_data); i++) {
		BGET(VS_RESULT_BITS, player->vsman_data[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### VS MAN(フラッシュ)

//## 圧縮
static void RecVsManFlash_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vm_fl_data); i++) {
		BPUT(VS_RESULT_BITS, player->vm_fl_data[i]);
	}
}

//## 展開
static void RecVsManFlash_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vm_fl_data); i++) {
		BGET(VS_RESULT_BITS, player->vm_fl_data[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### VS MAN(タイムアタック)

//## 圧縮
static void RecVsManTimeAT_Compress(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vm_ta_data); i++) {
		BPUT(VS_RESULT_BITS, player->vm_ta_data[i]);
	}
}

//## 展開
static void RecVsManTimeAT_Extract(BitField *bf, int num)
{
	mem_char *player = &evs_mem_data[num];
	int i;

	for(i = 0; i < ARRAY_SIZE(player->vm_ta_data); i++) {
		BGET(VS_RESULT_BITS, player->vm_ta_data[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{###

//## 圧縮
static void RecAll_Compress(BitField *bf)
{
	int i, sum;

	// サムをクリア
	bf->sum = 0;

	// システム
	RecSystem_Compress(bf);

	// サムを保存
	sum = bf->sum;

	// チェックサム
	BitField_PutBit(bf, CHECK_SUM_BITS2, sum);

	for(i = 0; i < REC_CHAR_SIZE; i++) {
		// サムをクリア
		bf->sum = 0;

		// プレイヤー
		RecPlayer_Compress(bf, i);

		// ストーリー
		RecStory_Compress(bf, i);

		// レベル
		RecLevel_Compress(bf, i);

		// 耐久1P
		RecLevelTaiQ_Compress(bf, i);

		// タイムアタック1P
		RecLevelTimeAT_Compress(bf, i);

		// VS COM
		RecVsCom_Compress(bf, i);

		// VS COM フラッシュ
		RecVsComFlash_Compress(bf, i);

		// VS MAN
		RecVsMan_Compress(bf, i);

		// VS MAN フラッシュ
		RecVsManFlash_Compress(bf, i);

		// VS MAN タイムアタック
		RecVsManTimeAT_Compress(bf, i);

		// サムを保存
		sum = bf->sum;

		// チェックサム
		BitField_PutBit(bf, CHECK_SUM_BITS, sum);
	}

	// ヘッダー破損チェック
	DBG2(bf->bufIdx >= EEPROM_HEADER_ADDR,
		"ヘッダーが壊れた(%d >= %d)\n",
		bf->bufIdx, EEPROM_HEADER_ADDR);

	// ヘッダー
	bf->bufIdx = EEPROM_HEADER_ADDR;
	bf->bitIdx = 0;
	for(i = 0; i < ARRAY_SIZE(eeprom_header); i++) {
		BitField_PutBit(bf, eeprom_header_bits[i], eeprom_header[i]);
	}
}

//## 展開
static int RecAll_Extract(BitField *bf, char header[4])
{
	int i, sum, bad = 0;

	// サムをクリア
	bf->sum = 0;

	// システム
	RecSystem_Extract(bf);

	// サムを保存
	sum = bf->sum & ((1 << CHECK_SUM_BITS2) - 1);

	// チェックサム不良?
	if(sum != BitField_GetBit(bf, CHECK_SUM_BITS2)) {
		// システムをクリア
		dm_init_system_mem();
		bad++;
	}

	for(i = 0; i < REC_CHAR_SIZE; i++) {
		// サムをクリア
		bf->sum = 0;

		// プレイヤー
		RecPlayer_Extract(bf, i);

		// ストーリー
		RecStory_Extract(bf, i);

		// レベル
		RecLevel_Extract(bf, i);

		// 耐久1P
		RecLevelTaiQ_Extract(bf, i);

		// タイムアタック1P
		RecLevelTimeAT_Extract(bf, i);

		// VS COM
		RecVsCom_Extract(bf, i);

		// VS COM フラッシュ
		RecVsComFlash_Extract(bf, i);

		// VS MAN
		RecVsMan_Extract(bf, i);

		// VS MAN フラッシュ
		RecVsManFlash_Extract(bf, i);

		// VS MAN タイムアタック
		RecVsManTimeAT_Extract(bf, i);

		// サムを保存
		sum = bf->sum & ((1 << CHECK_SUM_BITS) - 1);

		// チェックサム不良?
		if(sum != BitField_GetBit(bf, CHECK_SUM_BITS)) {
			// プレイヤーを削除
			dm_init_save_mem(&evs_mem_data[i]);
			bad++;
		}
	}

	// ヘッダー破損チェック
	DBG2(bf->bufIdx >= EEPROM_HEADER_ADDR,
		"ヘッダーが壊れた(%d >= %d)\n",
		bf->bufIdx, EEPROM_HEADER_ADDR);

	// ヘッダー
	bf->bufIdx = EEPROM_HEADER_ADDR;
	bf->bitIdx = 0;
	for(i = 0; i < ARRAY_SIZE(eeprom_header); i++) {
		header[i] = BitField_GetBit(bf, eeprom_header_bits[i]);
	}

	return bad;
}

//////////////////////////////////////////////////////////////////////////////
//{### EEPRom の初期化

//## osContInit より後に呼びます
EepRomErr EepRom_Init()
{
	s32 res;

	PRT0("EepRom_Init\n");

	// システム、プレイヤーを初期化
	EepRom_InitVars();

	// 16K EEPRom 有無のチェック
	res = osEepromProbe(EEPROM_MQ);
	if(res != EEPROM_TYPE_4K && res != EEPROM_TYPE_16K) {
		PRTFL();
		EepRom_DumpErrMes(EepRomErr_NotFound);
		return EepRomErr_NotFound;
	}

	return EepRom_ReadAll();
}

//## 出荷時の初期化
EepRomErr EepRom_InitFirst(void (*proc)(void *), void *args)
{
	s32 res;

	PRT0("EepRom_InitFirst\n");

	// システム、プレイヤーを初期化
	EepRom_InitVars();

	// 16K EEPRom 有無のチェック
	res = osEepromProbe(EEPROM_MQ);
	if(res != EEPROM_TYPE_4K && res != EEPROM_TYPE_16K) {
		PRTFL();
		EepRom_DumpErrMes(EepRomErr_NotFound);
		return EepRomErr_NotFound;
	}

	return EepRom_WriteAll(proc, args);
}

//## 変数を初期化
void EepRom_InitVars()
{
	int i;

	dm_init_system_mem();
	dm_init_config_4p_save(&evs_cfg_4p);

	for(i = 0; i < MEM_CHAR_SIZE; i++) {
		dm_init_save_mem(&evs_mem_data[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### システム情報 <-> EEPRom

//## ロングリード
static u8 *eepRom_longRead(bool forceRead)
{
	static bool _cached = false;
	static u8   _cache[EEPROM_MAXSIZE];
	s32 res;

	if(!_cached || forceRead) {
		// 4K EEP-ROM ロングリード
#if defined(USE_EEP)
		res = osEepromLongRead(EEPROM_MQ, 0, _cache, EEPROM_MAXSIZE);
#endif
		_cached = (res == 0);
	}

	return _cached ? _cache : NULL;
}

//## 差分書き
static s32 EepRom_WriteDif(void *oldBuf, void *newBuf, int size,
	void (*proc)(void *), void *args)
{
	u8 *oldPtr, *newPtr;
	int count, write;
	s32 res;

	oldPtr = (u8 *)oldBuf;
	newPtr = (u8 *)newBuf;

	for(count = write = 0; count < size; count += EEPROM_BLOCK_SIZE) {

		// 変更点のみ書く
#if defined(USE_EEP)
		if(bcmp(oldPtr, newPtr, EEPROM_BLOCK_SIZE) != 0) {
			if(proc != NULL) {
				proc(args);
				proc = NULL;
			}
//if(write==0) osViBlack(TRUE);
			res = osEepromLongWrite(EEPROM_MQ, count / EEPROM_BLOCK_SIZE, newPtr, EEPROM_BLOCK_SIZE);
			if(res != 0) {
				PRTFL();
				EepRom_DumpErrMes(EepRomErr_WriteError);
				return EepRomErr_WriteError;
			}
			write++;
		}
#endif

		oldPtr += EEPROM_BLOCK_SIZE;
		newPtr += EEPROM_BLOCK_SIZE;
	}
//if(write!=0) osViBlack(FALSE);

	PRTFL();
	PRT1("%d 個所の差分を書き込みました\n", write);

	return 0;
}

//## 全読み
EepRomErr EepRom_ReadAll()
{
	BitField bf;
	u8 *buf, header[4];
	int bad;

	PRT0("EepRom_ReadAll\n");

	// 4K EEP-ROM ロングリード
	buf = eepRom_longRead(true);
	if(buf == NULL) {
		PRTFL();
		EepRom_DumpErrMes(EepRomErr_ReadError);
		return EepRomErr_ReadError;
	}

	// 入力用バッファを設定
	BitField_Attach(&bf, buf, EEPROM_MAXSIZE);

	// データを展開
	bad = RecAll_Extract(&bf, header);

	// ヘッダー破損
	if(bcmp(header, eeprom_header, sizeof(eeprom_header)) != 0) {
		return EepRomErr_NotInit;
	}

	// チェックサム不良
	if(bad != 0) {
		return EepRomErr_BadSum;
	}

	return EepRomErr_NoError;
}

//## 全書き
EepRomErr EepRom_WriteAll(void (*proc)(void *), void *args)
{
	BitField bf;
	u8 *in, out[EEPROM_MAXSIZE];
	s32 res;
	int sum;

	PRT0("EepRom_WriteAll\n");

	// 4K EEP-ROM ロングリード
	in = eepRom_longRead(true);
	if(in == NULL) {
		PRTFL();
		EepRom_DumpErrMes(EepRomErr_ReadError);
		return EepRomErr_ReadError;
	}

	// 出力用バッファを初期化
	BitField_Init(&bf, out, sizeof(out));

	// データを圧縮
	RecAll_Compress(&bf);

	// 変更点のみ書く
	res = EepRom_WriteDif(in, out, EEPROM_MAXSIZE, proc, args);

	if(res != 0) {
		PRTFL();
		EepRom_DumpErrMes(EepRomErr_WriteError);
		return EepRomErr_WriteError;
	}

	// 読み込みキャッシュを更新
	bcopy(out, in, EEPROM_MAXSIZE);

	return EepRomErr_NoError;
}

//////////////////////////////////////////////////////////////////////////////
//{### デバッグ用

//## エラーメッセージを出力
void EepRom_DumpErrMes(EepRomErr err)
{
#if defined(DEBUG)
	switch(err) {
	case EepRomErr_NoError:
		PRT0("EepRomErr_NoError\n");
		break;
	case EepRomErr_NotFound:
		PRT0("EepRomErr_NotFound\n");
		break;
	case EepRomErr_NotInit:
		PRT0("EepRomErr_NotInit\n");
		break;
	case EepRomErr_ReadError:
		PRT0("EepRomErr_ReadError\n");
		break;
	case EepRomErr_WriteError:
		PRT0("EepRomErr_WriteError\n");
		break;
	case EepRomErr_BadSum:
		PRT0("EepRomErr_BadSum\n");
		break;
	}
#endif
}

//## 各データのサイズを出力
void EepRom_DumpDataSize()
{
#if defined(DEBUG)
	BitField bf;
	u8 buf[0x800];
	int bits, total = 0;
	int i;

	BitField_Init(&bf, buf, sizeof(buf));
	RecSystem_Compress(&bf);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits;
	PRT1("system        %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecPlayer_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("player        %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecStory_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("story         %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecLevel_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("level         %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecLevelTaiQ_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("taiQ          %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecLevelTimeAT_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("timeAt        %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecVsCom_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("vs com        %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecVsComFlash_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("vs com flash  %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecVsMan_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("vs man        %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecVsManFlash_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("vs man flash  %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecVsManTimeAT_Compress(&bf, 0);
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits * REC_CHAR_SIZE;
	PRT1("vs man timeAt %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	for(i = 0; i < ARRAY_SIZE(eeprom_header); i++) {
		BitField_PutBit(&bf, eeprom_header_bits[i], eeprom_header[i]);
	}
	bits = bf.bufIdx * 8 + bf.bitIdx;
	total += bits;
	PRT1("header        %4d\n", bits);

	BitField_Init(&bf, buf, sizeof(buf));
	RecAll_Compress(&bf);
	bits = bf.bufIdx * REC_CHAR_SIZE + bf.bitIdx;
	bits -= total;
	total += bits;
	PRT1("csum          %4d\n", bits);

	PRT1("total         %4d / 4096 bits\n", total);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//{### EEPROM書き込み中の警告メッセージ

//#define FRAME_THICK 8

//## 初期化
void RecWritingMsg_init(RecWritingMsg *st, void **heap)
{
#if LOCAL==JAPAN
	msgWnd_init(&st->msgWnd, heap, 24, 3, 0, 0);
#elif LOCAL==AMERICA
	msgWnd_init(&st->msgWnd, heap, 24, 2, 0, 0);
#elif LOCAL==CHINA
	msgWnd_init(&st->msgWnd, heap, 24, 2, 0, 0);
#endif
	st->msgWnd.centering = 1;

	RecWritingMsg_setPos(st, 0, 0);

	st->timeout = st->count = FRAME_PAR_SEC * 2;
}

//## 文字列を設定
void RecWritingMsg_setStr(RecWritingMsg *st, const unsigned char *str)
{
	msgWnd_clear(&st->msgWnd);
	msgWnd_addStr(&st->msgWnd, str);
	msgWnd_skip(&st->msgWnd);
}

//## 計算
void RecWritingMsg_calc(RecWritingMsg *st)
{
	if(RecWritingMsg_isEnd(st)) {
		return;
	}

	msgWnd_update(&st->msgWnd);
	st->count++;
}

//## 描画
void RecWritingMsg_draw(RecWritingMsg *st, Gfx **gpp)
{
	enum { TEX_W = 16, TEX_H = 16 };
	Gfx *gp;
	int w, h;

	if(RecWritingMsg_isEnd(st)) {
		return;
	}

	gp = *gpp;

	w = msgWnd_getWidth(&st->msgWnd);
	h = msgWnd_getHeight(&st->msgWnd);

	gSPDisplayList(gp++, normal_texture_init_dl);

	StretchTexTile8(&gp,
		mess_panel_tex_size[0], mess_panel_tex_size[1],
		mess_panel_lut, mess_panel_tex,
		0, 0,
		mess_panel_tex_size[0], mess_panel_tex_size[1],
		st->msgWnd.posX - (mess_panel_tex_size[0] - w) / 2,
		st->msgWnd.posY - (mess_panel_tex_size[1] - h) / 2,
		mess_panel_tex_size[0], mess_panel_tex_size[1]);

	msgWnd_draw(&st->msgWnd, &gp);

	*gpp = gp;
}

//## 表示開始
void RecWritingMsg_start(RecWritingMsg *st)
{
	st->count = 0;
}

//## 表示終了
void RecWritingMsg_end(RecWritingMsg *st)
{
	st->count = st->timeout;
}

//## 表示終了?
int RecWritingMsg_isEnd(RecWritingMsg *st)
{
	return st->count >= st->timeout;
}

//## 座標を設定
void RecWritingMsg_setPos(RecWritingMsg *st, int x, int y)
{
	st->msgWnd.posX = x;
	st->msgWnd.posY = y;
}

//////////////////////////////////////////////////////////////////////////////
//{### スリープタイマー

void setSleepTimer(int msec) {
	OSMesg msg;
	OSMesgQueue msgQ;
	OSTimer timer;

	osCreateMesgQueue(&msgQ, &msg, 1);
	osSetTimer(&timer, OS_USEC_TO_CYCLES(msec * 1000), 0, &msgQ, 0);
	osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
}

//////////////////////////////////////////////////////////////////////////////
//{### EOF

