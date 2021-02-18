//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_title_main.c
//	create		:	1999/03/08
//	modify		:	1999/12/01
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#include <ultra64.h>
#ifndef LINUX_BUILD
#include <math.h>
#endif

#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "graphic.h"
#include "audio.h"
#include "joy.h"
#include "static.h"
#include "tex_func.h"

#include "evsworks.h"
#include "record.h"
#include "dm_title_main.h"
#include "main_story.h"
#include "msgwnd.h"

#include "util.h"
#include "debug.h"
#include "dbg_font.h"
#include "local.h"

#define _getKeyTrg(playerNo) ((int)joyupd[main_joy[(playerNo)]])

static int title_demo_flg = 0;  // デモに飛ぶか、操作説明に飛ぶかのフラグ
static int title_demo_no = 0;   // どのデモに飛ぶか
static int title_manual_no = 0; // どの操作説明に飛ぶか

static int title_exit_flag = 0;
static int title_mode_type;  // 内部制御変数

static int title_fade_count;
static int title_fade_step;

//////////////////////////////////////////////////////////////////////////////
// タイトル初期化関数

void dm_title_init(void)
{
	int i, j;

	title_exit_flag = 0;
	title_mode_type = 0; // 内部制御の初期化

	title_fade_count = (main_old == MAIN_MENU ? 255 : 0);
	title_fade_step = -8;

	evs_seqence = FALSE;

	init_title((u32)_codeSegmentEnd, main_old != MAIN_MENU);
}

//////////////////////////////////////////////////////////////////////////////
// タイトル処理メイン関数

static const u8 _stageTbl[] = {
	STORY_M_ST1, STORY_M_ST2, STORY_M_ST3, STORY_M_ST4,
	STORY_M_ST5, STORY_M_ST6, STORY_M_ST7, STORY_M_ST8,
	STORY_M_ST9,
	STORY_W_ST1, STORY_W_ST2, STORY_W_ST3, STORY_W_ST4,
	STORY_W_ST5, STORY_W_ST6, STORY_W_ST7, STORY_W_ST8,
	STORY_W_ST9,
};

int dm_title_main(NNSched *sched)
{
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	int i, j;
	int sec_count = (main_old == MAIN_MENU) ? 99 : 0;
	int out_flg = 0; // ループ脱出用フラグ

	// メッセージキューの作成
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// クライアントに登録
	nnScAddClient(sched, &client, &msgQ);

	// シャッフル
	sgenrand( osGetCount() );

	// 変数初期化
	dm_title_init();

	// コントローラ接続状況確認
	evs_playmax = joyResponseCheck();

	PRT0("Start Title\n");

	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

	// メインループ
	while(out_flg == 0) {
		joyProcCore();

		// リトレースメッセージ
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

		if ( GameHalt ) {
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		// フェードイン・アウトの制御
		title_fade_count = CLAMP(0, 255, title_fade_count + title_fade_step);

		switch(title_mode_type) {
		case 0:
			// タイムアウト処理
			sec_count++;
			if(title_exit_flag == -1) {
				title_mode_type = 7;
				break;
			}

			// シーケンスを再生
			if(sec_count == 100) {
				dm_seq_play(SEQ_Title);
			}

			// キー受付処理
			if(title_exit_flag == 1) {
				title_mode_type = 6;
//				dm_snd_play(SE_mGameStart);

#if !defined(NINTENDO)
				for(i = 0; i < MAXCONTROLLERS; i++) {
					if(joynew[i] & DM_KEY_Z) {

						// 隠しキャラを使用可能にする
						evs_secret_flg[0] = evs_secret_flg[1] = 1;

						// 全ステージ制覇
						for(i = 0; i < 9; i++) {
							mem_char *mc = &evs_mem_data[i];
							mc->clear_stage[0][0] = mc->clear_stage[0][1] = 7;
							mc->clear_stage[1][0] = mc->clear_stage[1][1] = 7;
							mc->clear_stage[2][0] = mc->clear_stage[2][1] = 7;
							mc->clear_stage[3][0] = mc->clear_stage[3][1] = 7;
						}

						// 変な音を再生
						dm_snd_play_strange_sound();

						title_mode_type = 6;
						break;
					}
				}
#endif
			}
			break;

		case 6: case 7:
			title_fade_step = -title_fade_step;

			if(title_mode_type == 6) {
				// ゲームへ
				out_flg = 1;
			}
			else if(title_mode_type == 7) {
				// デモへ
				out_flg = 2;
			}
			break;
		}

		// オーディオのメイン処理
		dm_audio_update();

		// グラフィックの設定
		graphic_no = GFX_TITLE;
	}

	// 音楽停止
	dm_seq_stop();

	// グラフィック・オーディオタスク終了待ち
	while(!dm_audio_is_stopped() || title_fade_count < 255) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();

		// フェードイン・アウトの制御
		title_fade_count = CLAMP(0, 255, title_fade_count + title_fade_step);
	}

	// 表示停止
	graphic_no = GFX_NULL;

	// グラフィック・オーディオタスク終了待ち
	while(pendingGFX != 0) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// クライアントの抹消
	nnScRemoveClient(sched, &client);

	if(out_flg == 1) {
		return MAIN_MENU;
	}
	else if(out_flg == 2) {
		if(title_demo_flg == 0) {
			// 背景を指定
			story_proc_no = _stageTbl[rand() % ARRAY_SIZE(_stageTbl)];

			// ゲームデモの場合
			if(title_demo_no == 0) {
				// 1Pデモへ
				evs_gamesel = GSL_1DEMO;
			}
			else if(title_demo_no == 1) {
				// 2Pデモへ
				evs_gamesel = GSL_2DEMO;
			}
			else if(title_demo_no == 2) {
				// 4Pデモへ
				evs_gamesel = GSL_4DEMO;
			}
			evs_gamemode = GMD_NORMAL;
			title_demo_no++;

			// 現在は4Pデモは行わない
			if(title_demo_no > 2) {
				title_demo_no = 0;
			}

			//デモと操作説明を交互に行う
			title_demo_flg ^= 1;

			return MAIN_12;
		}
		else if(title_demo_flg == 1) {
			// 操作説明の場合
			evs_manual_no = title_manual_no;
			title_manual_no++;
			if(title_manual_no > 3) {
				title_manual_no = 0;
			}

			//デモと操作説明を交互に行う
			title_demo_flg ^= 1;

			return MAIN_MANUAL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// タイトル描画メイン関数

void dm_title_graphic(void)
{
	NNScTask *gt;

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄﾊﾞｯﾌｧ･ﾀｽｸﾊﾞｯﾌｧの指定
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	gSPSegment(gp++, 0, 0x0);

	// RSP 初期設定
	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);
	gSPDisplayList(gp++, S2Spriteinit_dl);

	// 描画範囲の指定
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	title_exit_flag = demo_title(&gp, evs_level_21 ? 1 : 0);

	// フェードフィルタ
	{
		int c, a;
		c = 255;
		a = CLAMP(0, 255, (title_fade_count - 127) * 1.2 + 127);
		if(a > 0) {
			FillRectRGBA(&gp, 0, 0, SCREEN_WD, SCREEN_HT, c, c, c, a);
		}
	}

#if defined(NN_SC_PERF)
	nnScWritePerfMeter(&gp, 32, 180);
#endif

#if defined(ROM_VERSION)
	DbgCon_Open(&gp, NULL, 32, 32);
	DbgCon_Format("%s\n%s\n", _debugVersionDate, _debugVersionTime);
//	DbgCon_Format("OS %s patch %d\n", OS_MAJOR_VERSION, OS_MINOR_VERSION);
	DbgCon_Close();
#endif

	/*------------------*/
	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ作成終了, ｸﾞﾗﾌｨｯｸﾀｽｸの開始
	gDPFullSync(gp++);			// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ最終処理
	gSPEndDisplayList(gp++);	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ終端
	osWritebackDCacheAll();		// dinamicｾｸﾞﾒﾝﾄのﾌﾗｯｼｭ
	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}

//////////////////////////////////////////////////////////////////////////////

typedef struct {
	SMsgWnd msgWnd;
} SMainBootError;

#if LOCAL==JAPAN
#include "dm_title.mes"
#elif LOCAL==AMERICA
#include "dm_title_am.mes"
#elif LOCAL==CHINA
#include "dm_title_zh.mes"
#endif

int main_boot_error(void *arg)
{
	SMainBootError *st = (SMainBootError *)ALIGN_16(_codeSegmentEnd);
	void *heap = st + 1;

	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	bool loop = true;

	// ワークをクリア
	bzero(st, sizeof(SMainBootError));

	// メッセージキューの作成
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// クライアントに登録
	nnScAddClient((NNSched *)arg, &client, &msgQ);

	switch(main_no) {
	case MAIN_CONT_ERROR:
#if LOCAL==JAPAN
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 96);
#elif LOCAL==AMERICA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 64, 96);
#elif LOCAL==CHINA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 64, 96);
#endif
		msgWnd_addStr(&st->msgWnd, _mesBootContErr);
		break;

	case MAIN_TV_ERROR:
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 64, 114);
		msgWnd_addStr(&st->msgWnd, _mesBootTvErr);
		break;

	case MAIN_CSUM_ERROR:
#if LOCAL==JAPAN
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 108);
#elif LOCAL==AMERICA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 108);
#elif LOCAL==CHINA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 108);
#endif
		msgWnd_addStr(&st->msgWnd, _mesBootCSumErr);
		break;
	}

	msgWnd_skip(&st->msgWnd);

	while(loop) {
		joyProcCore();

		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

		if ( GameHalt ) {
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		msgWnd_update(&st->msgWnd);

		switch(main_no) {
		case MAIN_CSUM_ERROR:
			if(_getKeyTrg(0) & A_BUTTON) {
				loop = false;
				dm_snd_play(SE_mDataErase);
			}
			break;
		}

		dm_audio_update();

		graphic_no = GFX_BOOT_ERROR;
	}

	// 表示停止
	graphic_no = GFX_NULL;

	// グラフィック・オーディオタスク終了待ち
	while(pendingGFX != 0) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	// クライアントの抹消
	nnScRemoveClient((NNSched *)arg, &client);

	switch(main_no) {
	case MAIN_CSUM_ERROR:
		EepRom_WriteAll(NULL, NULL);
		break;
	}

	return MAIN_TITLE;
}

void graphic_boot_error()
{
	SMainBootError *st = (SMainBootError *)ALIGN_16(_codeSegmentEnd);
	int i;

	gp = gfx_glist[gfx_gtask_no];

	F3RCPinitRtn();
	F3ClearFZRtn(1);

	msgWnd_draw(&st->msgWnd, &gp);

	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();

	gfxTaskStart(&gfx_task[gfx_gtask_no], gfx_glist[gfx_gtask_no],
		(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
		GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}

//////////////////////////////////////////////////////////////////////////////
